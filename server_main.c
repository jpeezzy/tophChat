#include <socket.h>
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
#include "server_back_end.h"
#include "utils.h"
#include "tcpPacket.h"

#define ALPLHA_RELEASE
// TODO: implement multi-threaded

#ifdef ALPLHA_RELEASE
// alpha release will just have two machines connecting to each other

#define MAX_USERS 2 // maximum number of user conversation
int main_loop(void)
{
    // when connection is read, user needs to send a packet with the name of the persion they need to connect

    int socketList[MAX_USERS];
    for (int i = 0; i < MAX_USERS; ++i)
    {
        socketList[i] = -1;
    }
    struct sockaddr *addrList[MAX_USERS];
    for (int i = 0; i < MAX_USERS; ++i)
    {
        addrList[i] = malloc(sizeof(struct sockaddr));
    }
    socklen_t socklenList[MAX_USERS];
    int listenerSocket = listenSocketInit();

    // fd_set *setClient;
    // FD_ZERO(setClient);
    fd_set setListener;
    FD_ZERO(&setListener);

    int socketListener = listenSocketInit();
    int nextFreeSocket = 0;
    int isFull = 0;
    char packet[PACKAGE_SIZE];
    listen(socketListener, MAX_SERVER_USERS);
    FD_SET(socketListener, &setListener);
    int j = 0;
    for (;;)
    {
        if (select(listenerSocket + 1, &setListener, NULL, NULL, NULL) > 0 || isFull)
        {
            if (!isFull)
            {
                socketList[j] = accept(listenerSocket, addrList[j], socklenList[j]);
                ++j;
            }
            else
            {
                if (fetchPacket(packet, socketList[0]) > 0)
                {
                    sendPacket(packet, socketList[1]);
                }

                if (fetchPacket(packet, socketList[1]) > 0)
                {
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