#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string.h>

extern "C"
{
#include "tcpGUI.h"
}
//#ifdef DEBUG
TEST(ClientConnectionLocal, CloseOpenTest)
{
    serverConnection* server=openConnection();
    assert(server);
    char dummyPacket[]="123";
    char receivedPacket[5];
    socklen_t socketOption;
    socklen_t socketOptionLength=sizeof(socketOption);
    
    // check if we can send message and check if the socket is the correct type 
    // TODO: Adjust buffer size 
    ASSERT_GE(server->socket, 0);
    ASSERT_GE(send(server->socket, dummyPacket, sizeof(dummyPacket),0), 0);
    ASSERT_GE(recv(server->socket,receivedPacket, sizeof(receivedPacket), MSG_WAITALL), 0);
    ASSERT_EQ(0, strcmp(receivedPacket, dummyPacket));
    printf("\n%s\n", receivedPacket);
    ASSERT_EQ(strcmp(dummyPacket, receivedPacket), 0);
    
    getsockopt(server->socket, SOL_SOCKET, SO_TYPE, &socketOption, &socketOptionLength);
    ASSERT_EQ(socketOption, SOCK_STREAM);

    getsockopt(server->socket, SOL_SOCKET, SO_DOMAIN, &socketOption, &socketOptionLength);
    ASSERT_EQ(socketOption, AF_INET);

    // check if the connection is closed properly
    int socketCopy=server->socket;   
    closeConnection(server); 
    ASSERT_LE(recv(socketCopy,receivedPacket, sizeof(receivedPacket), 0), 0);
}
//#endif 


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}