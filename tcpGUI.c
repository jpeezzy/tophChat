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

#ifndef DEBUG
    // assuming that the chat server is on zuma
    getaddrinfo(CHAT_SERVER_ADDR, CHAT_SERVER_PORT, &serverHints, &serverInfo);
#else
    getaddrinfo("localhost", CHAT_SERVER_PORT, &serverHints, &serverInfo);
#endif
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
    writeBuffer(inbox->messageQueue, message);
    return 0;
}

/******************* ROOM_STUFFS *************************/

// ROOM FINDING
// return a pointer to the room specified by the server room number
chatRoom *retrieveRoom(roomList *allRoom, int roomNum)
{
    int i = 0;
    for (i = 0; i < allRoom->totalRoom; ++i)
    {
        if ((allRoom->roomList)[i].roomNum == roomNum)
        {
            break;
        }
    }
    if (i == allRoom->totalRoom)
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
    for (i = 0; i < allRoom->totalRoom; ++i)
    {
        if ((allRoom->roomList[i]).status == ROOM_READY)
        {
            break;
        }
    }
    if (i == allRoom->totalRoom)
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
    for (i = 0; i < allRoom->totalRoom; ++i)
    {
        if ((allRoom->roomList[i]).status == ROOM_UNALLOCATED)
        {
            assembleCommand(i, ROID, ROCREATE, userName, NULL, tempMessage);
            writeBuffer(outputFIFO, tempMessage);
            return i; 
        }
    }
    return -1;
}

roomList *roomsetInit(void)
{
    roomList *temproomSet = malloc(sizeof(struct allRoom));
    temproomSet->totalRoom = CHAT_ROOM_LIMIT;
    for (int i = 0; i < CHAT_ROOM_LIMIT; ++i)
    {
        (temproomSet->roomList[i]).status = ROOM_UNALLOCATED;
        (temproomSet->roomList[i]).inMessage = initBuffer(CLIENT_CHAT_ROOM_INTPUT_FIFO_MAX);
    }
    return temproomSet;
}

int roomsetDel(roomList *allRoom)
{
    for (int i = 0; i < allRoom->totalRoom; ++i)
    {
        closeBuffer(allRoom->roomList[i].inMessage);
    }
    free(allRoom);
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
        return 0;
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
    int errCode = 0;
    chatRoom *tempRoom;
    if ((errCode = fetchPacket(packet, server->socket)) == 0)
    {
        if (getpacketType(packet) == ISCOMM)
        {
            tempRoom = retrieveRoom(allRoom, getroomNumber(packet));
            writeBuffer(tempRoom->inMessage, packet);
            return ISCOMM;
        }
        else if (getpacketType(packet) == ISMESSAGE)
        {
            writeBuffer(inbox->messageQueue, packet);
            return ISMESSAGE;
        }
        else
        {
            return UNKNOWN_COMMAND_TYPE;
        }
    }
    else
    {
        return errCode;
    }
    return 0;
}

int parseInboxCommand(inboxQueue *inbox, roomList *roomList, fifo *outputBuffer, char *userName, serverConnection *server)
{
    char packet[PACKAGE_SIZE];

    // may need to change it to if for singular handling
    while (readBuffer(inbox->messageQueue, packet) == 0)
    {
        int roomNum = getroomNumber(packet);
        char comType = getCommandType(packet);
        int comID = getCommandID(packet);
        char senderName[MAX_USER_NAME];
        char receiverName[MAX_USER_NAME];
        if (comType == FRIENDID)
        {
            switch (comID)
            {
            getSenderName(senderName, packet);
            // both of these need linking with GTK
            case FRIEACCEPT:
            printf("\n%s accepted your friend request\n", senderName);
                break;
            case DEREQUEST:
            printf("\n%s denied your friend request\n", senderName);
                break;
            }
        }
        else if (comType == ROID_PASSIVE)
        {
            getSenderName(senderName, packet);
            switch (comID)
            {
            case ROGRANTED:
                receiveRoom(roomList, roomNum);
                joinCreatedRoom(roomList, roomNum);
                break;

            case ROINVITED:

                // TODO: handle if this user ran out of room
                if(roomList->totalAllocatedRoom==CHAT_ROOM_LIMIT)
                {
                    printf("\nyou have reached maximum room limit\n");
                    denyInvitedRoom(roomNum, userName, senderName, outputBuffer);
                    return 0;
                }
                char answer;
                getCommandSender(packet, senderName);
                printf("\nyou haven been invited by%s\n, join? y/n", senderName);
                scanf(" %c", &answer);
                if (answer == 'y')
                {
                    joinInvitedRoom(roomList, roomNum, userName, outputBuffer);
                }
                else
                {
                    denyInvitedRoom(roomNum, userName, senderName, outputBuffer);
                }
                // check if user agreed

                break;
            case RODENIED:
                printf("\nsyour room has been denied\n");
                break;
            }
        }
        else if (comType == COMID)
        {
            switch (comID)
            {
            case GETONLINEUSER:
                // display list of user
                break;
            }
        }
        else
        {
            return UNKNOWN_COMMAND_TYPE;
        }
    }
    // else
    // {
    //     return FIFO_NO_DATA;
    // }
    return 0;
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