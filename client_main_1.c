#include <sys/socket.h>
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

// #include <pthread.h>

#include "constants.h"
#include "tcpGUI.h"
#include "tcpPacket.h"
#include "protocol.h"
#include "protocol_const.h"

#define DEBUG

int main(void)
{
    serverConnection *server = openConnection();
    assert(server);
    char packet[PACKAGE_SIZE] = "";
    char message[MESS_LIMIT] = "";

    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;
    int tempSelect;

    char *testPacketList[] = {"Test1", "Test2", "Test3", "Test4"};
    for (;;)
    {
        while (fetchPacket(packet, server->socket) == 0)
        {
            getMessageBody(packet, message);
            printf("\nThe message is %s\n", message);
        }

        sendPacket(packet, server->socket);
    }
    closeConnection(server);
    return 0;
}