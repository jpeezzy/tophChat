#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "networkTestClass.h"

extern "C"
{
#include <string.h>
#include "constants.h"
#include "tcpPacket.h"
#include "server_back_end.h"
#include "testString.h"
}

class IOTest : public ::testing::Test
{
    protected:
    virtual void SetUp()
        {
            sleep(2);
            system("kill $(pidof testServer)");
        }
    virtual void TearDown()
        {
            sleep(2);
            system("kill $(pidof testServer)");
        }
};


TEST_F(IOTest, clientsendTest)
{   
    sleep(1);
    system("./testServer TestClientSend &");
    char dummyPacket[PACKAGE_SIZE];    
    char receivedPacket[PACKAGE_SIZE];
    // setup a basic socket for testing
    serverConnection* server=NULL;
    server=(serverConnection*)malloc(sizeof(serverConnection));
    assert(server);

    struct addrinfo *serverInfo=NULL;
    struct addrinfo serverHints;

    bzero(&serverHints, sizeof(serverHints));
    // give hints about TCP connection and IPV4
    serverHints.ai_family = AF_INET;
    serverHints.ai_socktype = SOCK_STREAM;
    getaddrinfo("localhost", CHAT_SERVER_PORT, &serverHints, &serverInfo);
    assert(serverInfo);
    server->socket=socket(serverInfo->ai_family, serverInfo->ai_socktype,serverInfo->ai_protocol);
    sleep(1);
    assert(connect(server->socket, serverInfo->ai_addr, serverInfo->ai_addrlen)>=0);

    sendPacket(testString1, server->socket);    
    close(server->socket);
    free(server);
    freeaddrinfo(serverInfo);
}

TEST_F(IOTest, clientfetchTest)
{
    sleep(1);
    system("./testServer TestClientFetch &");
    char dummyPacket[PACKAGE_SIZE];    
    char receivedPacket[PACKAGE_SIZE];
    // setup a basic socket for testing
    serverConnection* server=NULL;
    server=(serverConnection*)malloc(sizeof(serverConnection));
    assert(server);

    struct addrinfo *serverInfo=NULL;
    struct addrinfo serverHints;

    bzero(&serverHints, sizeof(serverHints));
    // give hints about TCP connection and IPV4
    serverHints.ai_family = AF_INET;
    serverHints.ai_socktype = SOCK_STREAM;
    getaddrinfo("localhost", CHAT_SERVER_PORT, &serverHints, &serverInfo);
    assert(serverInfo);
    server->socket=socket(serverInfo->ai_family, serverInfo->ai_socktype,serverInfo->ai_protocol);
    sleep(1);
    assert(connect(server->socket, serverInfo->ai_addr, serverInfo->ai_addrlen)>=0);
    
    while(fetchPacket(receivedPacket, server->socket)<0)
    {
        // wait for packet
    }
    ASSERT_EQ(strcmp(receivedPacket, testString1),0);
    
    close(server->socket);
    free(server);
    freeaddrinfo(serverInfo);
}
    
    


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
