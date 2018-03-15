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
#include "protocol_const.h"
#include "protocol.h"
#include "fifo.h"

int main(void)
{
    int incomingSocket = -1;
    struct sockaddr addrDummy;
    socklen_t socklenDummy;
    struct timeval timeout;
    timeout.tv_sec = 2;
    fd_set setListener;
    FD_ZERO(&setListener);

    int socketListener = listenSocketInit();
    assert(socketListener>=0);
    char packet[PACKAGE_SIZE] = "";
    char message[MESS_LIMIT]="";
    FD_SET(socketListener, &setListener);
    int j = 0;

    TINFO *dataBase = createTINFO();
    serverRoomList *roomList = serverRoomSetCreate();
    struct messageServerRoom *testRoom = serverRoomCreate(roomList);
    onlineUserList *userList = serverCreateOnlineList();

    char *userName[] = {"USER", "ADMIN"};
    addUser(userName[0], userName[0], 213123, dataBase);
    addUser(userName[1], userName[1], 213123, dataBase);

    TUSER* userProfile0=findUserByName(userName[0], dataBase);
    TUSER* userProfile1=findUserByName(userName[1], dataBase);

    addFriend(userName[0], userProfile1, dataBase);
    // limit to two users
    for (;;)
    {
        FD_ZERO(&setListener);
        FD_SET(socketListener, &setListener);
        if (j < 2)
        {
            if (select(socketListener + 1, &setListener, NULL, NULL, &timeout) > 0)
            {
                if(((incomingSocket = accept(socketListener, &addrDummy, &socklenDummy))>=0))
                {
                #ifdef DEBUG
                printf("\nconnection received the socket is %d\n", incomingSocket);
                #endif
                ++(userList->totalOnlineUser);
                if (serverAddOnlineUser(userName[j], userList, incomingSocket, dataBase) == NULL)
                {
                    perror("\ncan't add user to server\n");
                }
                ++j;
            }
            else
            {
                #ifdef DEBUG
                printf("\nfailed to acceept socket\n");
                #endif
            }
            }
        }
        if (j == 2)
        {
            // #ifdef DEBUG
            // printf("\nreceived both\n");
            // fflush(stdout);
            // #endif
            if (triagePacket(userList, roomList, dataBase) == 2)
            {
                readBuffer(testRoom->inMessage, packet);
                getMessageBody(packet, message);
                printf("received message: %s\n", message);
                serverRoomSpreadMessage(testRoom, dataBase);
            }
        }
    }
    return 0;
}