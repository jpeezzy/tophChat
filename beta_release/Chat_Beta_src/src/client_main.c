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
    char packet[PACKAGE_SIZE]="";
    char message[MESS_LIMIT]="";

    fd_set clientSet;
    FD_ZERO(&clientSet);
    struct timeval timeout;
    timeout.tv_sec=10;
    timeout.tv_usec=0;
    int tempSelect;
    FD_SET(fileno(stdin), &clientSet);
    fflush(stdin);
    for (;;)
    {
        FD_ZERO(&clientSet);
        FD_SET(fileno(stdin), &clientSet);
        while(fetchPacket(packet, server->socket) ==0)
        {
            getMessageBody(packet, message);
            printf("\nThe message is %s\n", message);
        }
        fflush(stdin);
        tempSelect=select(fileno(stdin)+1, &clientSet, NULL, NULL, &timeout);
        printf("\n checking for user input \n");
        if(tempSelect>0)
        {
        printf("\nReceived input\n");
        scanf("%s", message);
        packet[0] = '\0';
        strcat(packet, "555");
        strcat(packet, ID_MESS);
        strcat(packet, message);
        printf("\nsending %s \n", packet);
        sendPacket(packet, server->socket);
        packet[0]='\0';
        fflush(stdin);
        fflush(stdout);
        }
        fflush(stdin);
        fflush(stdout);
    }
    closeConnection(server);
    return 0;
}