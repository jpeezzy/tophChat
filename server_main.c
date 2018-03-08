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
#include <strings.h>    
#include <sys/select.h>

// #include <pthread.h>

#include "constants.h"
#include "tcpPacket.h"
#include "server_back_end.h"

#define ALPLHA_RELEASE
// TODO: implement multi-threaded

#ifdef ALPLHA_RELEASE
// alpha release will just have two machines connecting to each other

#define MAX_USERS 2 // maximum number of user conversation
int main(void)
{
    int socketList[2];
    struct sockaddr *addrList[MAX_USERS];
    for (int i = 0; i < MAX_USERS; ++i)
    {
        addrList[i] = malloc(sizeof(struct sockaddr));
    }
    socklen_t socklenList[MAX_USERS];

    // fd_set *setClient;
    // FD_ZERO(setClient);
    fd_set setListener;
    FD_ZERO(&setListener);

    int socketListener = listenSocketInit();
    int isFull = 0;
    char packet[PACKAGE_SIZE]="";
    FD_SET(socketListener, &setListener);
    int j = 0;
    for (;;)
    {
        FD_ZERO(&setListener);
        FD_SET(socketListener, &setListener);
        if (isFull || select(socketListener + 1, &setListener, NULL, NULL, NULL) > 0)
        {
            
            if (isFull != 1)
            {
                socketList[j] = accept(socketListener, addrList[j], &socklenList[j]);
                printf("\nreceived connection\n");
                ++j;
            }
            else
            {
                while (fetchPacket(packet, socketList[0]) > 0)
                {
                    printf("\nreceived packet from 0\n");
                    printf("\nthe packet is \n%s", packet);
                    sendPacket(packet, socketList[1]);
                }

                while (fetchPacket(packet, socketList[1]) > 0)
                {
                    printf("\nreceived packet from 1\n");
                    printf("\nthe packet is \n%s", packet);
                    sendPacket(packet, socketList[0]);
                }
            }
            if (j == 2)
            {
                isFull = 1;
            }
        }
    }
    return 0;
}
#else
int main_loop(void)
{

    fd_set *setClient;
    FD_ZERO(setClient);
    fd_set *setListener;
    FD_ZERO(setListener);

    int socketListener = listenSocketInit();
    listen(socketListener, MAX_SERVER_USERS);
    FD_SET(socketListener, setListener);

    // forever loop, main thread watches for new client, other thread handles one client
    for (;;)
    {
    }
    return 0;
}
#endif