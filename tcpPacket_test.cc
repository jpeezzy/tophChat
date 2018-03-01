#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string.h>

#include "tcpPacket.c"
TEST()
{
    // setup a basic socket for testing
    serverConnection* server=NULL;
    server=(serverConnection*)malloc(sizeof(serverConnection));
    assert(server);

    struct addrinfo *serverInfo=NULL;
    struct addrinfo serverHints;

    bzero(&serverHints, sizeof(serverHints));

    int count = 0;

    // give hints about TCP connection and IPV4
    serverHints.ai_family = AF_INET;
    serverHints.ai_socktype = SOCK_STREAM;
    // TODO: write parallel test 
    getaddrinfo("localhost", CHAT_SERVER_PORT, &serverHints, &serverInfo);
    assert(serverInfo);
    server->socket=socket(serverInfo->ai_family, serverInfo->ai_socktype,serverInfo->ai_protocol);
    assert(connect(server->socket, serverInfo->ai_addr, serverInfo->ai_addrlen);
    freeaddrinfo(serverInfo);
    char dummyPacket[500];

    sendPacket((void*) dummyPacket, server);
}
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
