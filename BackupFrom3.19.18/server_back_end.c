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
    if(room->isOccupied!=ROOM_BUSY)
    {
        printf("\nthe room is not occupied\n");
    }
    if (readBuffer(room->inMessage, serverPacket) == 0)
    {
        char userName[50];
        TUSER *tempUser;
        assert(room);
        getSenderName(userName, serverPacket);
        tempUser = findUserByName(userName, dataBase);
#ifdef TDD_OFFLINE
        printf("\nthe name of the sender is %s\n", userName);
#endif
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
#ifdef TDD_OFFLINE
                    printf("\nsending offline to user with socket: %d\n", room->socketList[i]);
#else
                    printf("\nsending online to user with socket: %d\n", room->socketList[i]);
                    assert(sendPacket(serverPacket, room->socketList[i]) >= 0);
#endif
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
     assert(writeBuffer(room->inMessage, userPacket)==0);
     return 0;
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

onlineUser *serverSignUpOnlineUser(char *userName, onlineUserList *allUser, TINFO *database, char *packet)
{
    printf("\nentering sign up server\n");
    char sentPacket[PACKAGE_SIZE];
    char messageContent[MESS_LIMIT];
    char passWord[PASSWORD_LENGTH];

    char packetSender[MAX_USER_NAME];
    TUSER *tempUser;

    int totalSlash = 0;
    for (int i = 0; i < MAX_SERVER_USERS; ++i)
    {
        if ((database->Users[i])->isRegistered == 0)
        {
            tempUser = database->Users[i];
            break;
        }
    }
    // TODO: del user if inappropriate
    assert(tempUser);
    getMessageBody(packet, messageContent);

    if (allUser->totalOnlineUser == MAX_SERVER_USERS)
    {
        assembleCommand(0, COMID, OPENCOM, SIGN_UP_FAILED_SERVER_FULL, NULL, sentPacket);
        sendPacket(sentPacket, tempUser->socket);
        return NULL;
    }

    // check if user alread existed
    if ((tempUser->isRegistered) == 1)
    {
        assembleCommand(0, COMID, OPENCOM, SIGN_UP_FAILED_USER_EXISTED, NULL, sentPacket);
        sendPacket(sentPacket, tempUser->socket);
        return NULL;
    }
    strncpy(tempUser->name, packetSender, sizeof(char) * MAX_USER_NAME);
    strncpy(tempUser->userName, packetSender, sizeof(char) * MAX_USER_NAME);
    strncpy(tempUser->password, passWord, sizeof(char) * PASSWORD_LENGTH);
    printf("\nchecking sign up username\n");

    for (int i = 0; i < PACKAGE_SIZE; ++i)
    {
        if (packet[i] == '/')
        {
            ++totalSlash;
        }
        if (packet[i] == '\0' || i >= MAX_USER_NAME + ID_LENGTH + COM_LENGTH + CHAT_ROOM_CHARACTER)
        {
            break;
        }
    }
    printf("\nfinished checking user name with packet %s\n", packet);
    if (totalSlash >= 3)
    {
        assembleCommand(0, COMID, OPENCOM, SIGN_UP_FAILED_INVALID_NAME, NULL, sentPacket);
        sendPacket(sentPacket, tempUser->socket);
        return NULL;
    }
    else
    {
        // getCommandTarget(packet, passWord);
        printf("\nthe password is %s\n", passWord);
        strcpy(tempUser->password, passWord);
        // TODO: change this

        printf("\nfinish changing user name %s\n", packet);
        tempUser->isRegistered = 1;
        printf("\nfinish changing user status %s\n", packet);
        assembleCommand(0, COMID, OPENCOM, SIGNUP_SUCCESS, NULL, sentPacket);
        sendPacket(sentPacket, tempUser->socket);
    }
    return NULL;
}

onlineUser *serverLoginOnlineUser(char *userName, onlineUserList *allUser, TINFO *database, char *packet)
{
    int i;
    printf("\nentering server login\n");
    char sentPacket[PACKAGE_SIZE];
    char passWord[MESS_LIMIT];
    TUSER *tempUser = findUserByName(userName, database);
    assert(tempUser);
    if (allUser->totalOnlineUser == MAX_SERVER_USERS)
    {
        printf("\nserver max user\n");
        assembleCommand(0, COMID, OPENCOM, LOGIN_FAILED_SERVER_FULL, NULL, sentPacket);
        sendPacket(sentPacket, tempUser->socket);
        return NULL;
    }

    // check if user existed
    if ((tempUser->isRegistered) != 1)
    {
        printf("\nuser is not registered\n");
        assembleCommand(0, COMID, OPENCOM, LOGIN_FAILED_EXISTED, NULL, sentPacket);
        sendPacket(sentPacket, tempUser->socket);
        return NULL;
    }
    printf("\nchecking passwords\n");
    // check password
    getMessageBody(packet, passWord);
    TUSER *user = findUserByName(userName, database);
    assert(user);
    if (strcmp(passWord, user->password) != 0)
    {
        printf("\npassword doesn't match\n");
        assembleCommand(0, COMID, OPENCOM, LOGIN_FAILED_PASSWORD, NULL, sentPacket);
        sendPacket(sentPacket, tempUser->socket);
        return NULL;
    }

    printf("\nassigning online slots\n");
    for (i = 0; i < MAX_SERVER_USERS; ++i)
    {
        if (allUser->userList[i].slot_status == NOT_ONLINE)
        {
            allUser->userList[i].slot_status = ONLINE;
            allUser->userList[i].userProfile = tempUser;
            tempUser->numOfRoomUserIn = 0;
            assembleCommand(0, COMID, OPENCOM, LOGIN_SUCCESS, NULL, sentPacket);
            sendPacket(sentPacket, tempUser->socket);
            return &(allUser->userList[i]);
        }
    }

    printf("\ndone logging in users\n");
    return NULL;
}

int serverAddOnlineUser(char *userName, onlineUserList *allUser, TINFO *database)
{
    TUSER *tempUser = findUserByName(userName, database);
    assert(tempUser);
    int i = 0;
    for (i = 0; i < MAX_SERVER_USERS; ++i)
    {
        if (allUser->userList[i].slot_status == NOT_ONLINE)
        {
            allUser->userList[i].slot_status = ONLINE;
            allUser->userList[i].userProfile = tempUser;
            tempUser->numOfRoomUserIn = 0;
            return 0;
        }
    }

    if (i == MAX_SERVER_USERS)
    {
        return -1;
    }
    return 0;
}

int serverLogOffUser(onlineUser *user, TINFO *dataBase, serverRoomList *allRoom)
{
    close(user->userProfile->socket);
    user->slot_status = NOT_ONLINE;
    user->userProfile->socket = NOT_ONLINE;

    for (int i = 0; i < CHAT_ROOM_LIMIT; ++i)
    {

        if ((user->userProfile)->listOfRooms[i] >= 0)
        {
            removeUserFromServerRoom(
                &((allRoom->roomList)[((user->userProfile)->listOfRooms)[i]]), (user->userProfile)->userName, dataBase);
        }
    }

    for (int j = 0; j < CHAT_ROOM_LIMIT; ++j)
    {
        user->userProfile->listOfRooms[j] = -1;
    }
    user->userProfile->numOfRoomUserIn = 0;
    return 0;
}

int serverSendRoomInvite(char *userNameRequester, char *userNameTarget, serverChatRoom *room, TINFO *database)
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
    assembleCommand(room->roomNum, ROID, ROINVITED, userNameRequester, userNameTarget, packet);

#ifdef TDD_OFFLINE
    printf("the invitation packet is: %s\n", packet);
#else
    sendPacket(packet, target->socket);
#endif
    return 0;
}

int addUserToServerRoom(serverChatRoom *room, char *userNameTarget, TINFO *dataBase)
{
    assert(room);
    assert(userNameTarget);
    assert(dataBase);
    TUSER *tempUser = findUserByName(userNameTarget, dataBase);
    if (room->peopleNum == MAX_USER_PER_ROOM)
    {
#ifdef DEBUG
        printf("\nroom full\n");
#endif
        return ROOM_FULL;
    }
    else if (tempUser->socket == NOT_ONLINE)
    {
#ifdef DEBUG
        printf("\nuser not online\n");
#endif
        return USER_NOT_ONLINE;
    }
    else
    {
        if (tempUser->numOfRoomUserIn == CHAT_ROOM_LIMIT)
        {
#ifdef DEBUG
            printf("\nuser not online\n");
#endif
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
#ifdef DEBUG
                        printf("\nuser added to room\n");
#endif
                        return 0;
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
    if (tempUser->socket == NOT_ONLINE)
    {
        return USER_NOT_ONLINE;
    }

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
            onlineList[nextIndex + 1] = '\0';
        }
    }
    return 0;
}

int getRoomFriendList(serverChatRoom *room, char *friendRoomList, TUSER *user)
{
    friendRoomList[0] = '\0';
    int nextIndex = 0;
    for (int i = 0; i < MAX_USER_PER_ROOM; ++i)
    {
        if (((room->userList)[i] != NULL) && (strcmp(user->userName, room->userList[i]->userProfile->userName) != 0))
        {
            strcat(friendRoomList, room->userList[i]->userProfile->userName);
            nextIndex += strlen(room->userList[i]->userProfile->userName);
            friendRoomList[nextIndex] = '/';
            friendRoomList[nextIndex + 1] = '\0';
        }
    }
    return 0;
}

// send messages in all of the room fifo to the members of the room except the sender
int serverSendFIFO(struct allServerRoom *allRoom, TINFO *dataBase)
{
    for (int i = 0; i < MAX_SERVER_CHATROOMS; ++i)
    {
        if (allRoom->roomList[i].isOccupied == ROOM_BUSY)
        {
            serverRoomSpreadMessage(&(allRoom->roomList[i]), dataBase);
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