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
    assert(socket>=0);
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
            tempUser->numOfRoomUserIn = 0;
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
            onlineList[nextIndex + 1] = '\0';
        }
    }
    return 0;
}

int getRoomFriendList(serverChatRoom* room, char* friendRoomList)
{
    friendRoomList[0]='\0';
    int nextIndex = 0;
    for(int i=0; i < MAX_USER_PER_ROOM;++i)
    {
        if(room->userList[i]!=NULL)
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
int serverSendFIFO(struct allServerRoom* allRoom, TINFO* dataBase)
{
    for(int i=0; i<MAX_SERVER_CHATROOMS; ++i)
    {
        if(allRoom->roomList[i].isOccupied==ROOM_BUSY)
        {
        serverRoomSpreadMessage(&(allRoom->roomList[i]), dataBase);
        }
    }
    return 0;
}

int triagePacket(onlineUserList *userList, struct allServerRoom *allRoom, TINFO *dataBase)
{
    assert(userList);
    assert(allRoom);
    assert(dataBase);

    char message[MESS_LIMIT];
    char sentPacket[PACKAGE_SIZE]="";
    char packet[PACKAGE_SIZE]=""; // received packet
    char senderName[MAX_USER_NAME];
    char receiverName[MAX_USER_NAME];
    
    TUSER* tempUser;
    TUSER* tempUser2;
    int roomNum;
    serverChatRoom* onlineRoom;
    int userIndex=0;

    for (userIndex= 0; userIndex < MAX_SERVER_USERS; ++userIndex)
    {
        // #ifdef DEBUG
        //                 printf("\nuser index is %d\n",userIndex);
        //                 #endif
        if (userList->userList[userIndex].slot_status == ONLINE)
        {
                        // #ifdef DEBUG
                        // printf("\nfound an online user %d\n",userIndex);
                        // #endif
            assert(((userList->userList)[userIndex]).userProfile->socket>=0);

                        // #ifdef DEBUG
                        // printf("\nfound an online user after assert %d\n",userIndex);
                        // #endif
            if (fetchPacket(packet, ((userList->userList)[userIndex]).userProfile->socket) == PACKAGE_SIZE*sizeof(char))
            {
                    #ifdef DEBUG
                        printf("\nreceived packet, processing\n");
                        #endif
                if (getpacketType(packet) == ISMESSAGE)
                {
                     #ifdef DEBUG
                        printf("\nreceived user message\n");
                        #endif
                    sendServerRoomMessage(findServerRoomByNumber(allRoom, getroomNumber(packet)), packet);
                    return 2;
                }
                else if (getpacketType(packet) == ISCOMM)
                {
                    #ifdef DEBUG
                        printf("\nreceived command\n");
                        #endif
                    getCommandSender(packet, senderName);
                    getCommandTarget(packet, receiverName);
                    int roomNum = getroomNumber(packet);
                    if (getCommandType(packet) == FRIENDID)
                    {
                        switch (getCommandID(packet))
                        {
                        tempUser=findUserByName(senderName, dataBase);
                        tempUser2=findUserByName(receiverName, dataBase);

                        case FREQUEST:
                            // TODO: make friend request offline too
                            if((tempUser = findUserByName(receiverName, dataBase))!=NULL)
                            {
                                assembleCommand(1, FRIENDID, FREQUEST, senderName, receiverName, sentPacket);
                                if(tempUser->socket>-1)
                                {
                                sendPacket(sentPacket, tempUser->socket);
                                }
                            }
                            else
                            {
                                return -1;
                            }
                            break;
                        case DEREQUEST:
                            sendPacket(packet, tempUser2->socket);
                            break;
                        case FRIEACCEPT:
                            // TOOD: added error checking
                            addFriend(receiverName, tempUser, dataBase);
                            sendPacket(packet, tempUser2->socket);
                            break;

                        case DEFRIEND:
                            deleteFriend(tempUser, receiverName);
                            sendPacket(packet, tempUser2->socket);
                            break;
                        }
                    }

                    else if (getCommandType(packet) == ROID)
                    {
                        switch (getCommandID(packet))
                        {
                        case ROCREATE:  
                        #ifdef DEBUG
                        printf("\nreceived room create request\n");
                        #endif
                             if((tempUser = findUserByName(senderName, dataBase))!=NULL)
                             {
                                 if(tempUser->numOfRoomUserIn==CHAT_ROOM_LIMIT || allRoom->totalRoom==MAX_SERVER_CHATROOMS)
                                 {
                                     assembleCommand(0, ROID_PASSIVE, RODENIED, senderName, NULL, sentPacket);
                                 }
                                
                                 else
                                 {
                                    #ifdef DEBUG
                                    printf("\nallocating room to %s\n", senderName);
                                    #endif
                                     onlineRoom=serverRoomCreate(allRoom);
                                     onlineRoom->isOccupied=ROOM_BUSY;
                                     addUserToServerRoom(onlineRoom, senderName, dataBase);
                                     assembleCommand(onlineRoom->roomNum, ROID_PASSIVE, ROGRANTED, senderName, NULL, sentPacket);
                                 }
                                 
                                    #ifdef DEBUG
                                    printf("\nsending approval for room number %d create to %s\n", onlineRoom->roomNum, senderName);
                                    #endif
                                 sendPacket(sentPacket, tempUser->socket);
                                  #ifdef DEBUG
                                    printf("\nfinished sending approval for room number %d create to %s\n", onlineRoom->roomNum, senderName);
                                    #endif
                             }
                            break;

                        case RODEL:
                            break;

                        case ROINVITE:
                        #ifdef DEBUG
                        printf("\nreceived room invite\n");
                        #endif
                        tempUser=findUserByName(senderName, dataBase);
                        tempUser2=findUserByName(receiverName, dataBase);
                        onlineRoom=findServerRoomByNumber(allRoom, roomNum);
                            if(!checkIfFriends(tempUser, tempUser2))
                            {
                                
                                assembleCommand(roomNum, ROID_PASSIVE, ROOMJOINDENIED, senderName, NULL, sentPacket);
                                sendPacket(sentPacket, tempUser->socket);
                            }
                            else 
                            {
                                
                                #ifdef DEBUG
                                printf("\nboth users are friends\n");
                                #endif
                                if(onlineRoom->isOccupied==ROOM_NOT_OCCUPIED)
                                {
                                    #ifdef DEBUG
                                    printf("\nroom not allocated\n");
                                    #endif
                                    assembleCommand(roomNum, ROID_PASSIVE, ROOMJOINDENIED, senderName, NULL, sentPacket);
                                    sendPacket(sentPacket, tempUser->socket);
                                }
                                else
                                {
                                    #ifdef DEBUG
                                    printf("\nforwarding invite\n");
                                    #endif
                                // forward packet to the other user
                                 assembleCommand(roomNum, ROID_PASSIVE, ROINVITED, senderName, receiverName, sentPacket);
                                sendPacket(sentPacket, tempUser2->socket);
                                }
                            }
                            break;

                        case ROACCEPT:
                        #ifdef DEBUG
                        printf("\nreceived room accept\n");
                        #endif
                                if(onlineRoom->isOccupied==ROOM_NOT_OCCUPIED)
                                {
                                    return -1;
                                }
                                else
                                {
                                    addUserToServerRoom(onlineRoom, senderName, dataBase);
                                    assembleCommand(roomNum, ROID_PASSIVE, ROOMJOINACCEPTED, senderName, receiverName, sentPacket);
                                    sendPacket(sentPacket, tempUser2->socket);
                                }
                            
                            break;
                        case RODENY:
                            assembleCommand(roomNum, ROID_PASSIVE, ROOMJOINDENIED, senderName, receiverName, sentPacket);
                            sendPacket(sentPacket, tempUser2->socket);
                            break;

                        // sync user list of online server with offline room 
                        case ROSYNC:
                            getRoomFriendList(findServerRoomByNumber(allRoom, roomNum), message);
                            assembleCommand(roomNum, ROID_PASSIVE, ROSYNCED, "server", message, sentPacket);
                            sendPacket(sentPacket, tempUser->socket);
                            break;
                        }
                    }
                    else if (getCommandType(packet) == COMID)
                    {
                        switch (getCommandID(packet))
                        {
                        case OPENCOM:
                            getCommandSender(packet, senderName);
                            tempUser=findUserByName(senderName, dataBase);
                            char* publicKeyChar=malloc(sizeof(char)*MESS_LIMIT);
                            getCommandTarget(packet, publicKeyChar);
                            if(tempUser==NULL)
                            {
                                // user is not yet in database
                                tempUser=addUser(senderName, senderName, 123, dataBase);
                                tempUser->hashID=publicKeyChar;
                            }
                            else 
                            {
                                // user already in database
                                tempUser->hashID=publicKeyChar;
                            }
                        break;

                        case CLOSECOM:
                            // TODO: also take user out of all his current room
                            for(int i=0; i<MAX_SERVER_USERS; ++i)
                            {
                                if(userList->userList[i].userProfile->socket==tempUser->socket)
                                {
                                    serverLogOffUser(&(userList->userList[i]));
                                    break;
                                }
                            }
                           
                            break;

                        case GETONLINEUSER:
                            tempUser=findUserByName(senderName, dataBase);
                            getOnlineUser(userList, message);
                            assembleCommand(111, COMID, GETONLINEUSER, "server", message, sentPacket);
                            sendPacket(sentPacket, tempUser->socket);
                            break;
                        
                        case SIGNUP:

                            break;
                        }
                    }
                    else
                    {
                        #ifdef DEBUG
                        printf("\nreceived unknown command\n");
                        #endif
                        return UNKNOWN_COMMAND_TYPE;
                    }
                }
                #ifdef DEBUG
            else
                {
                    printf("\nreceived unknown packet\n");
                }
                        #endif
            
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