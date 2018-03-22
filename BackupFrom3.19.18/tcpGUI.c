#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include <netdb.h>
#include <arpa/inet.h>
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
#include <assert.h>

#include "tcpGUI.h"
#include "fifo.h"
#include "constants.h"
#include "protocol.h"
#include "protocol_const.h"
#include "tcpPacket.h"
#include "utils.h"
#include "userActions.h"


#define DEBUG
serverConnection *openConnection(void)
{
    serverConnection *server = NULL;
    server = (serverConnection *)malloc(sizeof(serverConnection));
    assert(server);

    struct addrinfo *serverInfo = NULL;
    struct addrinfo serverHints;

    bzero(&serverHints, sizeof(serverHints));

    int count = 0;

    // give hints about TCP connection and IPV4
    serverHints.ai_family = AF_INET;
    serverHints.ai_socktype = SOCK_STREAM;
getaddrinfo(CHAT_SERVER_ADDR, CHAT_SERVER_PORT, &serverHints, &serverInfo);
// #ifndef DEBUG
//     // assuming that the chat server is on zuma
//     getaddrinfo(CHAT_SERVER_ADDR, CHAT_SERVER_PORT, &serverHints, &serverInfo);
// #else
//     getaddrinfo("localhost", CHAT_SERVER_PORT, &serverHints, &serverInfo);
// #endif
    assert(serverInfo);
    server->socket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
    while ((connect(server->socket, serverInfo->ai_addr, serverInfo->ai_addrlen)) == -1 && count < RECONNECT_NUM)
    {
        ++count;
    }

    // can't connect to server
    if (count == 5)
    {
        // return SOCKET_CANT_CONNECT;
        return NULL;
    }

    // public key exchange
    // char publicKeyChar[MESS_LIMIT];
    // char packet[PACKAGE_SIZE];
    // intToString(publicKey, publicKeyChar, 62);
    // assembleCommand(1, COMID, OPENCOM, userName, publicKeyChar, packet);
    // sendPacket(packet, server->socket);
    freeaddrinfo(serverInfo);
    return server;
}

int closeConnection(serverConnection *server)
{
    close(server->socket);
    free(server);
    return 0;
}

inboxQueue *initInboxQueue(void)
{
    inboxQueue *inbox = malloc(sizeof(inboxQueue));
    inbox->messageQueue = initBuffer(MAX_REQUEST);
    return inbox;
}

int delInboxQueue(inboxQueue *inbox)
{
    closeBuffer(inbox->messageQueue);
    free(inbox);
    return 0;
}

int retrieveInboxMessage(char *message, inboxQueue *inbox)
{
    readBuffer(inbox->messageQueue, message);
    return 0;
}

int writeInboxMessage(char *message, inboxQueue *inbox)
{
    assert(writeBuffer(inbox->messageQueue, message)==0);
    return 0;
}

/******************* ROOM_STUFFS *************************/

// ROOM FINDING
// return a pointer to the room specified by the server room number
chatRoom *retrieveRoom(roomList *allRoom, int roomNum)
{
    int i = 0;
    for (i = 0; i < CHAT_ROOM_LIMIT; ++i)
    {
        if ((allRoom->roomList)[i].roomNum == roomNum)
        {
            break;
        }
    }
    if (i == CHAT_ROOM_LIMIT)
    {
        return NULL;
    }
    else
    {
        return &((allRoom->roomList)[i]);
    }
}

// find a room that has been synchronized with the server
chatRoom *findReadyRoom(roomList *allRoom)
{
    int i = 0;
    for (i = 0; i < CHAT_ROOM_LIMIT; ++i)
    {
        if ((allRoom->roomList[i]).status == ROOM_READY)
        {
            break;
        }
    }
    if (i == CHAT_ROOM_LIMIT)
    {
        return NULL;
    }
    else
    {
        return &(allRoom->roomList[i]);
    }
}

// ROOM STARTING ASKING AND ACCEPTING

// request room from server, return the index of the room that is used for requesting
int requestRoom(roomList *allRoom, fifo *outputFIFO, char *userName)
{
    // TODO: thread sensitive
    int i = 0;
    char tempMessage[PACKAGE_SIZE] = "";
    for (i = 0; i < CHAT_ROOM_LIMIT; ++i)
    {
        if ((allRoom->roomList[i]).status == ROOM_UNALLOCATED)
        {
            assembleCommand(i, ROID, ROCREATE, userName, NULL, tempMessage);
            assert(writeBuffer(outputFIFO, tempMessage)==0);
            return i;
        }
    }
    return -1;
}

roomList *roomsetInit(void)
{
    roomList *temproomSet = malloc(sizeof(struct allRoom));
    temproomSet->totalRoom = 0;
    for (int i = 0; i < CHAT_ROOM_LIMIT; ++i)
    {
        (temproomSet->roomList[i]).status = ROOM_UNALLOCATED;
        (temproomSet->roomList[i]).inMessage = initBuffer(CLIENT_CHAT_ROOM_INTPUT_FIFO_MAX);
        (temproomSet->roomList[i]).memberChanged = 0;
        (temproomSet->roomList[i]).totalPeople = 0;
        for (int j = 0; j < MAX_USER_PER_ROOM; ++j)
        {
            temproomSet->roomList[i].friendList[j] = malloc(sizeof(char) * PACKAGE_SIZE);
        }
    }

    return temproomSet;
}

int roomsetDel(roomList *allRoom)
{
    for (int i = 0; i < CHAT_ROOM_LIMIT; ++i)
    {
        closeBuffer(allRoom->roomList[i].inMessage);
        for (int j = 0; j < MAX_USER_PER_ROOM; ++j)
        {
            free(allRoom->roomList[i].friendList[j]);
        }
    }
    free(allRoom);
    return 0;
}

// get all friends including online and offline
int getAllFriend(char *userName, fifo *outputBuffer)
{
    char packet[PACKAGE_SIZE];
    assembleCommand(0, COMID, GETFRIEND, userName, NULL, packet);
    printf("\nwriting to buffer %s \n", packet);
    assert(writeBuffer(outputBuffer, packet)==0);
    return 0;
}

int getAllOnlineFriend(char*userName, fifo *outputBuffer)
{
    char packet[PACKAGE_SIZE];
    assembleCommand(0, COMID, GETONLINEFRIEND, userName, NULL, packet);
    assert(writeBuffer(outputBuffer, packet)==0);
    return 0;
}

// called in parsing function
int updateRoomFriendList(chatRoom *room, char **friendList, char *packet)
{
    char senderName[MAX_USER_NAME]="";
    getSenderName(senderName, packet);
    getUserFriendList(friendList, packet, MAX_USER_PER_ROOM);
    strcpy(friendList[0], senderName);
    // for (int i = 0; i < MAX_USER_PER_ROOM; ++i)
    // {
    //     if (friendList[i] != NULL)
    //     {
    //         if (!strcmp(room->friendList[i], friendList[i]))
    //         {
    //             room->memberChanged = 1;
    //             friendList[i][0]='\0';
    //             strcpy(room->friendList[i], friendList[i]);
    //         }
    //     }
    // }
    return 0;
}

// called in the parsing function to update the list of current online friend
int updateOnlineFriendList(char **outputList, char *packet)
{
    getUserFriendList(outputList, packet, MAX_FRIENDS);
    return 0;
}

int sendToServer(fifo *outputFIFO, serverConnection *server)
{
    char packet[PACKAGE_SIZE];
    if (readBuffer(outputFIFO, packet) == 0)
    {
        return sendPacket(packet, server->socket);
    }
    else
    {
        return FIFO_NO_DATA;
    }
}

int sendAllToServer(fifo *outputBuffer, serverConnection *server)
{
    int atLeastOnePacket = 0;
    if (sendToServer(outputBuffer, server) != FIFO_NO_DATA && !atLeastOnePacket)
    {
        atLeastOnePacket = 1;
    }
    while (sendToServer(outputBuffer, server) != FIFO_NO_DATA)
    {
        // wait until all data is sent
        ++atLeastOnePacket;
    }

    if (atLeastOnePacket)
    {
        return atLeastOnePacket;
    }
    else
    {
        return FIFO_NO_DATA;
    }
}

// get a message from a server and put it in the correct fifo
int recvMessageFromServer(roomList *allRoom, inboxQueue *inbox, serverConnection *server)
{
    char packet[PACKAGE_SIZE];
    char message[MESS_LIMIT];
    int errCode = 0;
    chatRoom *tempRoom;
    if ((errCode = fetchPacket(packet, server->socket)) > 0)
    {
        printf("\nreceived packet from server %s\n", packet);
        if(getCommandID(packet)<9)
        {
        if (getpacketType(packet) == ISMESSAGE)
        {
        
            tempRoom = retrieveRoom(allRoom, getroomNumber(packet));
            assert(writeBuffer(tempRoom->inMessage, packet)==0);
            return ISMESSAGE;
        }
        else if (getpacketType(packet) == ISCOMM)
        {
            assert(writeBuffer(inbox->messageQueue, packet)==0);
            return ISCOMM;
        }
        }
        else
        {
#ifdef DEBUG
            printf("\nreceived from server unknown command %s\n", packet);
#endif
            return UNKNOWN_COMMAND_TYPE;
        }
    }
    else
    {
        return errCode;
    }
    return 0;
}

// create a list of online user to display in the side bar for the user
char **createFriendList(int length)
{
    char **tempList;
    tempList = malloc(sizeof(char *) * length);
    for(int i=0; i<length;++i)
    {
        tempList[i]=malloc(sizeof(char)*MAX_USER_NAME);
        tempList[i][0]='\0';
        strcat(tempList[i], "empty");
    }
    return tempList;
}

void delFriendList(char **friendList)
{
    assert(friendList);
    free(friendList);
}

// #ifdef MAIN
// int main(void)
// {
//     {
//         inboxQueue *tempInboxQueue = initInboxQueue();
//         fifo *tempBuffer = tempInboxQueue->messageQueue;
//         char buffer[500] = "";
//         char *testMessage[] = {"This is nice", "That's not right", "fjldshvjdsnkjwehg;ke", "9283741892uioUoi@fds", "dfsfdsq1@!##$!",
//                                "fjdslfjew", "sjdklfjdslkgndsv,mnklwje", "123451fdsfdsa", "@!!@}{FL:ASL<><AD", "#PHFGJSBH(*@!P(*", "fdshjfjjdslkfjwdsjlnvkls", "2", "3", "~#@!3ewdflkna;", "nc,mxvns/.,/", "1", "2", "546", "142423", "fi2jwrwe", "12412fdsa", "bcnxz., flkawej", "/,/.3,12/lkaf", "fsdlj912pfa", "`13124ujrklj"};
//         int testMessageNum = sizeof(testMessage) / sizeof(char *);
//         for (int i = 0; i < testMessageNum; ++i)
//         {
//             writeInboxMessage(testMessage[i], tempInboxQueue);
//             retrieveInboxMessage(buffer, tempInboxQueue);

//             printf("\ntesting %s\n", testMessage[i]);
//             assert(strcmp(buffer, testMessage[i]) == 0);
//         }
//     }

//     {
//         roomList *testroomList = roomsetInit();
//         assert(testroomList != NULL);
//         fifo *tempBuffer;
//         for (int i = 0; i < testroomList->totalRoom; ++i)
//         {
//             assert((testroomList->roomList[i]).status == ROOM_UNALLOCATED);
//             tempBuffer = testroomList->roomList[i].inMessage;
//             assert(tempBuffer != NULL);
//             assert(tempBuffer->bufLength == CLIENT_CHAT_ROOM_INTPUT_FIFO_MAX);
//             assert(tempBuffer->readPos == 0);
//             assert(tempBuffer->writePos == 0);
//         }
//         roomsetDel(testroomList);
//     }

//     {
//         srand(time(NULL));
//         roomList *testroomList = roomsetInit();
//         int testRoomServerNum[TEST_ROOM_TOTAL_CASES];
//         int testRoomNum[TEST_ROOM_TOTAL_CASES];
//         chatRoom *tempRoom;
//         chatRoom *tempRoomReady;
//         for (int i = 0; i < TEST_ROOM_TOTAL_CASES; ++i)
//         {
//         randGen_1:
//             testRoomServerNum[i] = rand() % MAX_SERVER_CHATROOMS;

//             for (int j = 0; j < i; ++j)
//             {
//                 if (testRoomServerNum[j] == testRoomServerNum[i])
//                 {
//                     goto randGen_1;
//                 }
//             }

//         randGen_2:
//             testRoomNum[i] = rand() % CHAT_ROOM_LIMIT;

//             for (int j = 0; j < i; ++j)
//             {
//                 if (testRoomNum[j] == testRoomNum[i])
//                 {

//                     goto randGen_2;
//                 }
//             }

//             testroomList->roomList[testRoomNum[i]].roomNum = testRoomServerNum[i];
//             testroomList->roomList[testRoomNum[i]].status = ROOM_READY;
//         }
//         for (int i = 0; i < TEST_ROOM_TOTAL_CASES; ++i)
//         {
//             tempRoom = retrieveRoom(testroomList, testRoomServerNum[i]);
//             assert(tempRoom->roomNum == testRoomServerNum[i]);

//             tempRoomReady = findReadyRoom(testroomList);
//             assert(tempRoomReady->status == ROOM_READY);
//             tempRoomReady->status = ROOM_UNALLOCATED;
//         }
//         tempRoomReady = findReadyRoom(testroomList);
//         assert(tempRoomReady == NULL);

//         roomsetDel(testroomList);
//     }

//     {
//         fifo *outputFIFO = initBuffer(CLIENT_OUTPUT_FIFO_MAX);
//         roomList *testroomList = roomsetInit();
//         int firstUnAllocatedRoom;
//         char tempPacket[PACKAGE_SIZE] = "";
//         char messageBody[MESS_LIMIT];
//         char userName[MAX_USER_NAME] = "khoitd";
//         for (int i = 0; i < testroomList->totalRoom; ++i)
//         {
//             requestRoom(testroomList, outputFIFO, userName);
//             readBuffer(outputFIFO, tempPacket);

//             assert(getroomNumber(tempPacket) == i);
//             assert(getCommandType(tempPacket) == ROID);
//             assert(getCommandID(tempPacket) == ROCREATE);
//         }

//         for (int i = 0; i < testroomList->totalRoom; ++i)
//         {
//             testroomList->roomList[i].status = ROOM_WAITING;
//         }
//         assert(-1 == requestRoom(testroomList, outputFIFO, userName));

//         for (int i = 0; i < testroomList->totalRoom; ++i)
//         {
//             receiveRoom(testroomList, i + 2);
//             assert(testroomList->roomList[i].roomNum == i + 2);
//             assert(testroomList->roomList[i].status == ROOM_READY);
//             closeRoom(&(testroomList->roomList[i]), outputFIFO, userName);
//             readBuffer(outputFIFO, tempPacket);

//             assert(testroomList->roomList[i].status == ROOM_UNALLOCATED);
//             assert(getroomNumber(tempPacket) == i + 2);
//             assert(getCommandType(tempPacket) == ROID);
//             assert(getCommandID(tempPacket) == RODEL);
//         }
//     }
//     return 0;
// }
// #endif