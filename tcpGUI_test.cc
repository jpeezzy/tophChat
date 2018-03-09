#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string.h>

extern "C" {
#include "tcpGUI.h"
#include "fifo.h"
#include "protocol.h"
#include "protocol_const.h"
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
}

#define NO_SERVER
#ifndef NO_SERVER
TEST(ClientConnectionLocal, CloseOpenTest)
{
    serverConnection *server = openConnection();
    assert(server);
    char dummyPacket[] = "123";
    char receivedPacket[5];
    socklen_t socketOption;
    socklen_t socketOptionLength = sizeof(socketOption);

    // check if we can send message and check if the socket is the correct type
    // TODO: Adjust buffer size
    ASSERT_GE(server->socket, 0);
    ASSERT_GE(send(server->socket, dummyPacket, sizeof(dummyPacket), 0), 0);
    ASSERT_GE(recv(server->socket, receivedPacket, sizeof(receivedPacket), MSG_WAITALL), 0);
    ASSERT_EQ(0, strcmp(receivedPacket, dummyPacket));
    printf("\n%s\n", receivedPacket);
    ASSERT_EQ(strcmp(dummyPacket, receivedPacket), 0);

    getsockopt(server->socket, SOL_SOCKET, SO_TYPE, &socketOption, &socketOptionLength);
    ASSERT_EQ(socketOption, SOCK_STREAM);

    getsockopt(server->socket, SOL_SOCKET, SO_DOMAIN, &socketOption, &socketOptionLength);
    ASSERT_EQ(socketOption, AF_INET);

    // check if the connection is closed properly
    int socketCopy = server->socket;
    closeConnection(server);
    ASSERT_LE(recv(socketCopy, receivedPacket, sizeof(receivedPacket), 0), 0);
}
#endif

TEST(InboxTest, InitCloseTest)
{
    inboxQueue *tempInboxQueue = initInboxQueue();
    fifo *tempBuffer = tempInboxQueue->messageQueue;
    ASSERT_TRUE(tempInboxQueue != NULL);
    ASSERT_EQ(tempBuffer->bufLength, MAX_REQUEST);
    ASSERT_EQ(tempBuffer->readPos, 0);
    ASSERT_EQ(tempBuffer->writePos, 0);

    delInboxQueue(tempInboxQueue);
}

TEST(InboxTest, ReadWriteTest)
{
    inboxQueue *tempInboxQueue = initInboxQueue();
    fifo *tempBuffer = tempInboxQueue->messageQueue;
    char buffer[500] = "";
    char *testMessage[] = {"This is nice", "That's not right", "fjldshvjdsnkjwehg;ke", "9283741892uioUoi@fds", "dfsfdsq1@!##$!",
                           "fjdslfjew", "sjdklfjdslkgndsv,mnklwje", "123451fdsfdsa", "@!!@}{FL:ASL<><AD", "#PHFGJSBH(*@!P(*"
                           , "fdshjfjjdslkfjwdsjlnvkls", "2", "3", "~#@!3ewdflkna;", "nc,mxvns/.,/", "1", "2", "546", "142423"
                           , "fi2jwrwe", "12412fdsa", "bcnxz., flkawej", "/,/.3,12/lkaf", "fsdlj912pfa", "`13124ujrklj"};
    int testMessageNum = sizeof(testMessage) / sizeof(char *);
    for (int i = 0; i < testMessageNum; ++i)
    {
        writeInboxMessage(testMessage[i], tempInboxQueue);
        retrieveInboxMessage(buffer, tempInboxQueue);

        printf("\ntesting %s\n", testMessage[i]);
        ASSERT_EQ(strcmp(buffer, testMessage[i]), 0);
    }
}

TEST(RoomSetTest, CreateDelTest)
{
    roomList *testroomList = roomsetInit();
    ASSERT_TRUE(testroomList != NULL);
    fifo *tempBuffer;
    for (int i = 0; i < testroomList->totalRoom; ++i)
    {
        ASSERT_EQ((testroomList->roomList[i]).status, ROOM_UNALLOCATED);
        tempBuffer = testroomList->roomList[i].inMessage;
        ASSERT_TRUE(tempBuffer != NULL);
        ASSERT_EQ(tempBuffer->bufLength, CLIENT_CHAT_ROOM_INTPUT_FIFO_MAX);
        ASSERT_EQ(tempBuffer->readPos, 0);
        ASSERT_EQ(tempBuffer->writePos, 0);
    }
    roomsetDel(testroomList);
}

#define TEST_ROOM_TOTAL_CASES 15
TEST(RoomSetTest, RetrieveFindTest)
{
    srand(time(NULL));
    roomList *testroomList = roomsetInit();
    int testRoomServerNum[TEST_ROOM_TOTAL_CASES];
    int testRoomNum[TEST_ROOM_TOTAL_CASES];
    chatRoom *tempRoom;
    chatRoom *tempRoomReady;
    for (int i = 0; i < TEST_ROOM_TOTAL_CASES; ++i)
    {
    randGen_1:
        testRoomServerNum[i] = rand() % MAX_SERVER_CHATROOMS;

        for (int j = 0; j < i; ++j)
        {
            if (testRoomServerNum[j] == testRoomServerNum[i])
            {
                goto randGen_1;
            }
        }

    randGen_2:
        testRoomNum[i] = rand() % CHAT_ROOM_LIMIT;

        for (int j = 0; j < i; ++j)
        {
            if (testRoomNum[j] == testRoomNum[i])
            {

                goto randGen_2;
            }
        }

        testroomList->roomList[testRoomNum[i]].roomNum = testRoomServerNum[i];
        testroomList->roomList[testRoomNum[i]].status = ROOM_READY;
    }
    for (int i = 0; i < TEST_ROOM_TOTAL_CASES; ++i)
    {
        tempRoom = retrieveRoom(testroomList, testRoomServerNum[i]);
        ASSERT_EQ(tempRoom->roomNum, testRoomServerNum[i]);

        tempRoomReady = findReadyRoom(testroomList);
        ASSERT_EQ(tempRoomReady->status, ROOM_READY);
        tempRoomReady->status = ROOM_UNALLOCATED;
    }
    tempRoomReady = findReadyRoom(testroomList);
    ASSERT_TRUE(tempRoomReady == NULL);

    roomsetDel(testroomList);
}

TEST(roomTest, CreateCloseTest)
{
    fifo* outputFIFO= initBuffer(CLIENT_OUTPUT_FIFO_MAX);
    roomList *testroomList = roomsetInit();
    int firstUnAllocatedRoom;
    char tempPacket[PACKAGE_SIZE]="";
    char messageBody[MESS_LIMIT];
    for(int i=0; i<testroomList->totalRoom;++i)
    {
    requestRoom(testroomList, outputFIFO);
    readBuffer(outputFIFO, tempPacket);
    
    ASSERT_EQ(getroomNumber(tempPacket), i);
    ASSERT_EQ(getCommandType(tempPacket), ROID);
    ASSERT_EQ(getCommandID(tempPacket), ROCREATE);
    }
    
    for(int i=0; i<testroomList->totalRoom;++i)
    {
        testroomList->roomList[i].status=ROOM_WAITING;
    }       
    ASSERT_EQ(-1, requestRoom(testroomList, outputFIFO));
    
    for(int i=0; i<testroomList->totalRoom;++i)
    {
    receiveRoom(testroomList, i+2);
    ASSERT_EQ(testroomList->roomList[i].roomNum, i+2);
    ASSERT_EQ(testroomList->roomList[i].status, ROOM_READY);
    closeRoom(&(testroomList->roomList[i]), outputFIFO);
    readBuffer(outputFIFO, tempPacket);
    
    ASSERT_EQ(testroomList->roomList[i].status, ROOM_UNALLOCATED);
    ASSERT_EQ(getroomNumber(tempPacket), i+2);
    ASSERT_EQ(getCommandType(tempPacket), ROID);
    ASSERT_EQ(getCommandID(tempPacket), RODEL);
    }
}



int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}