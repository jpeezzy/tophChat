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
    size_t packetLeft = sizeof(char) * PACKAGE_SIZE;
    int sent;

    //implement check to make it sends all bytes
    while (packetLeft > 0)
    {
        sent = send(socket, curByte, packetLeft, 0);
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
    assert(packetLeft==0);
    return 0;
}

int fetchPacket(char *packet, int socket)
{
    assert(socket>=0);
    // peek allow the socket to check how much data without removing data from the buffer
    // #ifdef DEBUG
    //     printf("\ncollecting data\n");
    //     fflush(stdout);
    // #endif
    int temp = recv(socket, packet, sizeof(char) * PACKAGE_SIZE, MSG_PEEK | MSG_DONTWAIT);
    
    if (temp == -1)
    {
        if (errno == EWOULDBLOCK)
        {
            // #ifdef DEBUG
            // printf("\nno data\n");
            // #endif
            // no data to read
            return SOCKET_NO_DATA;
        }

        
        // #ifdef DEBUG
        // printf("-1 error in fetch packet is %s with socket number %d",strerror(errno), socket);
        // sleep(10);
        // #endif
    }
    else if (temp == 0)
    {
        // #ifdef DEBUG
        // printf("0 error in fetch packet is %s",strerror(errno));
        // // sleep(10);
        // #endif
        // #ifdef DEBUG
        // printf("\nclosed socket\n");
        // sleep(10);
        // #endif
        return SOCKET_CLOSED;
    }
    else if (temp != sizeof(char) * PACKAGE_SIZE)
    {
        // #ifdef DEBUG
        // printf("\nsocket not enough data\n");
        // // sleep(10);
        // #endif
        return SOCKET_NOT_ENOUGH_DATA;
    }
    else
    {
         #ifdef DEBUG
        printf("\nbegin collecting data\n");
        #endif
         assert(recv(socket, packet, sizeof(char) * PACKAGE_SIZE, MSG_WAITALL)== sizeof(char) * PACKAGE_SIZE);
         #ifdef DEBUG
        printf("\ndata received\n");
        #endif
        return (sizeof(char) * PACKAGE_SIZE);
    }
    return (sizeof(char) * PACKAGE_SIZE);
}
