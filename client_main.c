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

int main_loop(void)
{
    serverConnection *server = openConnection();
    char packet[PACKAGE_SIZE];
    char message[MESS_LIMIT];
    int userInputAvail = 0;
    for (;;)
    {
        if (fetchPacket(packet, server->socket) > 0)
        {
            getMessageBody(packet, message);
            printf("\nThe message is %s\n", message);
        }

        if (userInputAvail)
        {
            strcat(packet, 555);
            strcat(packet, ID_MESS);
            strcat(packet, message);
            sendPacket(packet, server->socket);
        }
    }
    closeConnection(server);
    return 0;
}