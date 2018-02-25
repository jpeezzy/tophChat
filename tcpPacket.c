#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include "tcpPacket.h"
#include "constants.h"

int sendPacket(clientPacket *packet, struct serverConnection *server)
{
    // casted as type char for easy implmentation of checking complete transmission
    char *curByte = packet;
    int packetLeft = sizeof(packet);
    int sent;

    //implement check to make it sends all bytes
    while (packetLeft > 0)
    {

        sent = send(server->socket, (void *)curByte, sizeof(packet), 0);
        if (sent == -1)
        {
            if (errno == EPIPE)
            {

                return SOCKET_CLOSED;
            }
        }
        packetLeft -= sent;
        curByte += sent;
    }
    return 0;
}

int fetchPacket(clientPacket *packet, struct serverConnection *server)
{
    int temp = recv(server->socket, packet, sizeof(packet), MSG_DONTWAIT);
    if (temp == -1)
    {
        if (errno == EWOULDBLOCK)
        {
            // no data to read
            return SOCKET_NO_DATA;
        }
    }

    else if (temp == 0)
    {
        return SOCKET_CLOSED;
    }

    return 0;
}

// the mailman thread, used for update I/O buffer
int manageIO(connection *server, fifo *inputFIFO, fifo *outputFIFO)
{
    return 0;
}

#ifdef DEBUG_PACKET
int main(void)
{

    return 0;
}
#endif