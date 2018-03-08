#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string.h>

extern "C" {
#include "tcpGUI.h"
#include "fifo.h"
#include "protocol_const.h"
#include "constants.h"
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
                           "fjdslfjew", "sjdklfjdslkgndsv,mnklwje", "123451fdsfdsa", "@!!@}{FL:ASL<><AD", "#PHFGJSBH(*@!P(*"};
    int testMessageNum = sizeof(testMessage) / sizeof(char *);
    for (int i = 0; i < testMessageNum; ++i)
    {
        writeInboxMessage(testMessage[i], tempInboxQueue);
        retrieveInboxMessage(buffer, tempInboxQueue);

        printf("\ntesting %s\n", testMessage[i]);
        if (i >= MAX_REQUEST)
        {
            ASSERT_TRUE(buffer == NULL);
        }
        else
        {
            ASSERT_EQ(strcmp(buffer, testMessage[i]), 0);
        }
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}