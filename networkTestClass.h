#ifndef _NETWORK_TEST_CLASS_H
#define _NETWORK_TEST_CLASS_H
extern "C"
{
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

#include "server_back_end.h"
#include "constants.h"
#include "tcpGUI.h"
}
class testServer
{
    private:
        struct sockaddr* addr;
        socklen_t len;
        int newsocket;
    public: 
        int serverSocket;
        void setupServer(void);
        ~testServer(void);
        int sendTestPacket(char* array);
        int recvTestPacket(void);
        char receivedPacket[PACKAGE_SIZE];
};

class testClient
{
    private:
         struct addrinfo *serverInfo;
         struct addrinfo serverHints;
    public:
        serverConnection* server;
        void setupClient(void);
        ~testClient(void);
        int sendTestPacket(char* array);
        int recvTestPacket(void);
        char receivedPacket[PACKAGE_SIZE];
};

#endif 