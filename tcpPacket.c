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

int sendPacket(char *packet, int socket)
{
    char *curByte = packet;
    int packetLeft = sizeof(char) * PACKAGE_SIZE;
    int sent;

    //implement check to make it sends all bytes
    while (packetLeft > 0)
    {
        sent = send(socket, (void *)curByte, packetLeft, 0);
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

int fetchPacket(char *packet, int socket)
{
    int temp = recv(socket, packet, sizeof(char) * PACKAGE_SIZE, MSG_DONTWAIT | MSG_PEEK);
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
    else if (temp != sizeof(char)* PACKAGE_SIZE)
    {
        return SOCKET_NOT_ENOUGH_DATA; 
    }
    else
    {
        return recv(socket, packet, sizeof(char) * PACKAGE_SIZE, MSG_WAITALL);
    }
    return 0;
}

// the mailman thread, used for update I/O buffer
int manageIO(serverConnection *server, fifo *inputFIFO, fifo *outputFIFO)
{
    return 0;
}
