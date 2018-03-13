#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include <netdb.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>
#include <strings.h>    //bzero
#include <sys/select.h> //io multiplexing
#include <errno.h>

#include "server_back_end.h"
#include "constants.h"
#include "tcpPacket.h"
#include "tophChatUsers.h"
#include "protocol_const.h"
#include "protocol.h"
#include "utils.h"

//#define TEST_SERVER

/**
 * as of right now, the server is single threaded, the server will need to call
 * update free room occassionally 
 */
int listenSocketInit(void)
{
    // struct timeval timeoutListener;
    // timeoutListener.tv_sec = LISTENER_TIMEOUT;

    // struct timeval timeoutClient;
    // timeoutListener.tv_sec = CLIENT_TIMEOUT;

    // the socket that will be listening
    struct addrinfo *serverListener;
    int socketListener;

    // give the server hints about the type of connection we want
    struct addrinfo serverHints;
    memset(&serverHints, 0, sizeof(serverHints));
    serverHints.ai_family = AF_INET;       // IPV4 address
    serverHints.ai_socktype = SOCK_STREAM; // TCP socket
    serverHints.ai_flags = AI_PASSIVE;     // listening socket
    int n = 1;

    // create a socket for listening incoming collection
    getaddrinfo(NULL, CHAT_SERVER_PORT, &serverHints, &serverListener);
    socketListener = socket(serverListener->ai_family, serverListener->ai_socktype, serverListener->ai_protocol);
    setsockopt(socketListener, SOL_SOCKET, SO_REUSEADDR, &n, sizeof(n));
    if (socketListener < 0)
    {
        freeaddrinfo(serverListener);
        return SOCKET_NO_CONNECTION;
    }
    else
    {
        if (bind(socketListener, serverListener->ai_addr, serverListener->ai_addrlen) == -1)
        {
            printf("The error is %s\n", strerror(errno));
        }
        assert(listen(socketListener, MAX_SERVER_USERS) >= 0);
        freeaddrinfo(serverListener);
        return socketListener;
    }
}

/**********************SERVER ROOM STUFFS HERE********************/
// create list of all rooms on server
struct allServerRoom *serverRoomSetCreate(void)
{
    struct allServerRoom *allRoom = malloc(sizeof(struct allServerRoom));
    allRoom->firstFreeRoom = 0;
    allRoom->totalRoom = 0;

    for (int i = 0; i < MAX_SERVER_CHATROOMS; ++i)
    {
        ((allRoom->roomList)[i]).isOccupied = ROOM_NOT_OCCUPIED;
        ((allRoom->roomList)[i]).roomNum = i;
        ((allRoom->roomList)[i]).peopleNum = 0;
        ((allRoom->roomList)[i]).inMessage = initBuffer(SERVER_CHAT_ROOM_INPUT_FIFO_MAX);
        for (int j = 0; j < MAX_USER_PER_ROOM; ++j)
        {
            ((allRoom->roomList[i]).socketList[j]) = -1;
            ((allRoom->roomList[i]).userList[j]) = NULL; // will be filled with pointer to the Justin database
        }
    }
    return allRoom;
}
void serverRoomSetDel(struct allServerRoom *allRoom)
{
    for (int i = 0; i < MAX_SERVER_CHATROOMS; ++i)
    {
        closeBuffer(((allRoom->roomList)[i]).inMessage);
        for (int j = 0; j < MAX_USER_PER_ROOM; ++j)
        {
            (allRoom->roomList[i]).userList[j] = NULL;
        }
    }
    free(allRoom);
}

// update which room is not occupied
void updateFreeRoom(struct allServerRoom *allRoom)
{
    assert(allRoom);
    int i;
    for (i = 0; i < MAX_SERVER_CHATROOMS; ++i)
    {
        if ((((allRoom->roomList)[i]).isOccupied) == ROOM_NOT_OCCUPIED)
        {
            allRoom->firstFreeRoom = i;
            break;
        }
    }
    if (i == MAX_SERVER_CHATROOMS)
    {
        allRoom->firstFreeRoom = -1;
    }
}

// return a free room
struct messageServerRoom *serverRoomCreate(struct allServerRoom *allRoom)
{
    assert(allRoom);
    updateFreeRoom(allRoom);
    if (allRoom->firstFreeRoom == -1)
    {
        return NULL;
    }
    else
    {
        allRoom->roomList[allRoom->firstFreeRoom].isOccupied = ROOM_BUSY;
        return &(allRoom->roomList[allRoom->firstFreeRoom]);
    }
}

// mark the room as free when conversation is done
void serverRoomReturn(serverChatRoom *room)
{
    assert(room);
    room->isOccupied = ROOM_NOT_OCCUPIED;
    for (int i = 0; i < MAX_USER_PER_ROOM; ++i)
    {
        room->socketList[i] = -1;
    }

    for (int i = 0; i < MAX_USER_PER_ROOM; ++i)
    {
        room->userList[i] = NULL;
    }
    room->peopleNum = 0;
}

// send message to everyone in the room except the writer
int serverRoomSpreadMessage(struct messageServerRoom *room, TINFO *dataBase)
{
    char serverPacket[PACKAGE_SIZE];
    if (readBuffer(room->inMessage, serverPacket) == 0)
    {
        char userName[50];
        TUSER *tempUser;
        assert(room);
        getSenderName(userName, serverPacket);
        tempUser = findUserByName(userName, dataBase);
        if (tempUser == NULL)
        {
            return USER_NOT_EXIST;
        }
        for (int i = 0; i < MAX_USER_PER_ROOM; ++i)
        {
            if (room->socketList[i] >= 0)
            {
                if ((room->socketList[i]) != tempUser->socket)
                {
                    assert(sendPacket(serverPacket, room->socketList[i]) >= 0);
                }
            }
        }
        return 0;
    }
    else
    {
        return FIFO_NO_DATA;
    }
}

// put the user message onto the server room fifo
int sendServerRoomMessage(struct messageServerRoom *room, char *userPacket)
{
    // TODO: not thread safe
    return writeBuffer(room->inMessage, userPacket);
}

// /*******************************USER STUFFS HERE**************************/
// create a list of online user
onlineUserList *serverCreateOnlineList(void)
{
    onlineUserList *userList = malloc(sizeof(onlineUserList));
    assert(userList);
    userList->totalOnlineUser = 0;
    for (int i = 0; i < MAX_SERVER_USERS; ++i)
    {
        userList->userList[i].slot_status = NOT_ONLINE;
    }
    return userList;
}

void serverDelOnlineList(onlineUserList *allOnlineUser)
{
    free(allOnlineUser);
}

// add user to the list of online user
onlineUser *serverAddOnlineUser(char *userName, onlineUserList *allUser, int socket, TINFO *database)
{
    int i;

    if (allUser->totalOnlineUser == MAX_SERVER_USERS)
    {
        // server full
        return NULL;
    }
    TUSER *tempUser = findUserByName(userName, database);

    // user already online
    if (tempUser->socket != NOT_ONLINE)
    {
        return NULL;
    }
    for (i = 0; i < MAX_SERVER_USERS; ++i)
    {
        if (allUser->userList[i].slot_status == NOT_ONLINE)
        {
            allUser->userList[i].slot_status = ONLINE;
            allUser->userList[i].userProfile = tempUser;
            tempUser->socket = socket;
            return &(allUser->userList[i]);
        }
    }
    return NULL;
}

int serverLogOffUser(onlineUser *user)
{
    close(user->userProfile->socket);
    user->slot_status = NOT_ONLINE;
    user->userProfile->socket = NOT_ONLINE;

    for (int j = 0; j < CHAT_ROOM_LIMIT; ++j)
    {
        user->userProfile->listOfRooms[j] = -1;
    }
    user->userProfile->numOfRoomUserIn = 0;
    return 0;
}

int sendRoomInvite(char *userNameRequester, char *userNameTarget, serverChatRoom *room, TINFO *database)
{
    char packet[PACKAGE_SIZE];
    TUSER *target = findUserByName(userNameTarget, database);
    TUSER *sender = findUserByName(userNameRequester, database);
    if (target == NULL || sender == NULL)
    {
        return USER_NOT_EXIST;
    }

    if (!checkIfFriends(target, sender))
    {
        return USER_NOT_FRIEND;
    }

    if (target->socket == NOT_ONLINE)
    {
        return USER_NOT_ONLINE;
    }
    assembleCommand(room->roomNum, ROID, ROINVITE, userNameRequester, userNameTarget, packet);
    sendPacket(packet, target->socket);
    return 0;
}

int addUserToServerRoom(serverChatRoom *room, char *userNameTarget, TINFO *dataBase)
{
    TUSER *tempUser = findUserByName(userNameTarget, dataBase);
    if (room->peopleNum == MAX_USER_PER_ROOM)
    {
        return ROOM_FULL;
    }
    else
    {
        if (tempUser->numOfRoomUserIn == CHAT_ROOM_LIMIT)
        {
            return USER_OCCUPIED_MAX_ROOM;
        }
        ++(room->peopleNum);
        for (int i = 0; i < MAX_USER_PER_ROOM; ++i)
        {
            // check if the current socket slot in the room is free
            if (room->socketList[i] == -1)
            {
                room->socketList[i] = tempUser->socket;

                // add this room to user list of room
                for (int j = 0; j < CHAT_ROOM_LIMIT; ++j)
                {
                    if (tempUser->listOfRooms[i] == -1)
                    {
                        tempUser->listOfRooms[i] = room->roomNum;
                        ++(tempUser->numOfRoomUserIn);
                        break;
                    }
                }

                break;
            }
        }
    }
    return 0;
}

int removeUserFromServerRoom(serverChatRoom *room, char *userNameTarget, TINFO *dataBase)
{
    TUSER *tempUser = findUserByName(userNameTarget, dataBase);
    if (tempUser != NULL)
    {
        --(tempUser->numOfRoomUserIn);
        --(room->peopleNum);
        for (int i = 0; i < MAX_USER_PER_ROOM; ++i)
        {
            if (room->socketList[i] == tempUser->socket)
            {
                room->socketList[i] = -1;
                break;
            }
        }

        for (int i = 0; i < CHAT_ROOM_LIMIT; ++i)
        {
            if (tempUser->listOfRooms[i] == room->roomNum)
            {
                tempUser->listOfRooms[i] = -1;
                break;
            }
        }
        return 0;
    }
    else
    {
        return USER_NOT_EXIST;
    }
}

serverChatRoom *findServerRoomByNumber(struct allServerRoom *allRoom, int roomNum)
{
    for (int i = 0; i < MAX_SERVER_CHATROOMS; ++i)
    {
        if (allRoom->roomList[i].roomNum == roomNum)
        {
            return &(allRoom->roomList[i]);
        }
    }
    return NULL;
}

// int getFriendList(TUSER *user)
// {

//     return 0;
// }

// get a list of online user separated by '/'
int getOnlineUser(onlineUserList *allUsers, char *onlineList)
{
    onlineList[0] = '\0';
    int nextIndex = 0;
    for (int i = 0; i < MAX_SERVER_USERS; ++i)
    {
        if (allUsers->userList[i].slot_status == ONLINE)
        {
            strcat(onlineList, allUsers->userList[i].userProfile->userName);
            nextIndex += strlen(allUsers->userList[i].userProfile->userName);
            onlineList[nextIndex] = '/';
            onlineList[nextIndex] = '\0';
        }
    }
    return 0;
}

int triagePacket(onlineUserList *userList, struct allServerRoom *allRoom, TINFO *dataBase, char *packet)
{
    char message[MESS_LIMIT];
    for (int i = 0; i < MAX_SERVER_USERS; ++i)
    {
        if (userList->userList[i].slot_status == ONLINE)
        {
            if (fetchPacket(packet, userList->userList[i].userProfile->socket) >= 0)
            {
                if (getpacketType(packet) == ISMESSAGE)
                {
                    sendServerRoomMessage(findServerRoomByNumber(allRoom, getroomNumber(packet)), packet);
                    return 2;
                }
                else if (getpacketType(packet) == ISCOMM)
                {
                    if (getCommandType(packet) == FRIENDID)
                    {
                        switch (getCommandID(packet))
                        {
                        case FREQUEST:
                            break;
                        case DEREQUEST:
                            break;
                        case FRIEACCEPT:
                            break;
                        case FRIEACCEPTED:
                            break;
                        case FRIEDENIED:
                            break;
                        }
                    }
                    else if (getCommandType(packet) == ROID)
                    {
                        switch (getCommandID(packet))
                        {
                        case ROCREATE:
                            break;
                        case RODEL:
                            break;
                        case ROINVITE:
                            break;
                        case ROACCEPT:
                            break;
                        case RODENY:
                            break;
                        }
                    }
                    else if (getCommandType(packet) == COMID)
                    {
                        switch (getCommandID(packet))
                        {
                        case CLOSECOM:
                            break;

                        case GETONLINEUSER:
                            getOnlineUser(userList, message);
                            assembleCommand(111, COMID, GETONLINEUSER, "server", message, packet);
                            sendPacket(packet, userList->userList[i].userProfile->socket);
                            break;
                        }
                    }
                    else
                    {
                        return UNKNOWN_COMMAND_TYPE;
                    }
                }
            }
        }
    }
    return 0;
}

// #ifdef MAIN
// int main(void)
// {
//     {
//         allServerRoom *allRoom = serverRoomSetCreate();
//         assert(allRoom->firstFreeRoom == 0);
//         assert(allRoom->totalRoom == 0);
//         for (int i = 0; i < MAX_SERVER_CHATROOMS; ++i)
//         {
//             assert(allRoom->roomList[i].isOccupied == ROOM_NOT_OCCUPIED);
//             assert(allRoom->roomList[i].peopleNum == 0);
//             assert(allRoom->roomList[i].roomNum == i);
//             EXPECT_TRUE(allRoom->roomList[i].inMessage != NULL);
//             for (int j = 0; j < MAX_USER_PER_ROOM; ++j)
//             {
//                 assert(allRoom->roomList[i].socketList[j] == -1);
//                 EXPECT_TRUE(allRoom->roomList[i].userList[j] == NULL);
//             }
//         }
//     }

//     {
//         llServerRoom *allRoom = serverRoomSetCreate();
//         struct messageServerRoom *testRoom;
//         EXPECT_TRUE((testRoom = serverRoomCreate(allRoom)) == &(allRoom->roomList[0]));
//         EXPECT_TRUE(allRoom->roomList[0].isOccupied == ROOM_BUSY);

//         EXPECT_TRUE(serverRoomCreate(allRoom) == &(allRoom->roomList[1]));
//         EXPECT_TRUE(allRoom->roomList[0].isOccupied == ROOM_BUSY);

//         serverRoomReturn(testRoom);
//         assert(allRoom->roomList[0].isOccupied == ROOM_NOT_OCCUPIED);
//         assert(allRoom->roomList[0].peopleNum == 0);
//         for (int i = 0; i < MAX_USER_PER_ROOM; ++i)
//         {
//             EXPECT_TRUE(allRoom->roomList[0].userList[i] == NULL);
//             EXPECT_TRUE(allRoom->roomList[0].socketList[i] == -1);
//         }
//     }

//     {
//         TINFO *database = createTINFO();
//         allServerRoom *allRoom = serverRoomSetCreate();
//         serverChatRoom *testRoom = &(allRoom->roomList[10]);
//         char userName[] = "ADMIN";
//         addUser(userName, userName, 1231123, database);
//         struct tophChatUser *testUser = findUserByName(userName, database);

//         addUserToServerRoom(testRoom, userName, database);
//         assert(testRoom->socketList[0] == testUser->socket);
//         assert(testUser->numOfRoomUserIn == 1);
//         assert(testUser->listOfRooms[0] == 10);
//         assert(testRoom->peopleNum == 1);

//         removeUserFromServerRoom(testRoom, userName, database);
//         assert(testRoom->socketList[0] == -1);
//         assert(testUser->numOfRoomUserIn == 0);
//         assert(testUser->listOfRooms[0] == -1);
//         assert(testRoom->peopleNum == 0);
//     }

//     {
//         onlineUserList *userList = serverCreateOnlineList();
//         assert(userList != NULL);
//         assert(userList->totalOnlineUser, 0);
//         for (int i = 0; i < MAX_SERVER_USERS; ++i)
//         {
//             assert(userList->userList[i].slot_status == NOT_ONLINE);
//         }
//         serverDelOnlineList(userList);
//     }

//     {
//         TINFO *database = createTINFO();
//         allServerRoom *allRoom = serverRoomSetCreate();
//         char userName[] = "ADMIN";
//         addUser(userName, userName, 12312131, database);
//         struct tophChatUser *testUser = findUserByName(userName, database);
//         onlineUserList *userList = serverCreateOnlineList();
//         onlineUser *testOnlUser;

//         assert((testOnlUser = serverAddOnlineUser(userName, userList, 55, database)) != NULL);
//         assert(&(userList->userList[0]) == testOnlUser);
//         assert(userList->userList[0].slot_status == ONLINE);
//         assert(userList->userList[0].userProfile == testUser);
//         assert(userList->userList[0].userProfile->socket == 55);

//         serverLogOffUser(testOnlUser);
//         assert(testOnlUser->slot_status == NOT_ONLINE);
//         assert(testOnlUser->userProfile->socket == NOT_ONLINE);
//         for (int i = 0; i < CHAT_ROOM_LIMIT; ++i)
//         {
//             assert(testOnlUser->userProfile->listOfRooms[i] == -1);
//         }
//         assert(testOnlUser->userProfile->numOfRoomUserIn == 0);

//         char buffer[100];
//         EXPECT_LE(recv(testOnlUser->userProfile->socket, buffer, 100, 0), 0);
//     }

//     return 0;
// }
// #endif