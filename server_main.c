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
    char packet[PACKAGE_SIZE] = "";
    FD_SET(socketListener, &setListener);
    int j = 0;

    TINFO *dataBase = createTINFO();
    serverRoomList *roomList = serverRoomSetCreate();
    struct messageServerRoom *testRoom = serverRoomCreate(roomList);
    onlineUserList *userList = serverCreateOnlineList();

    char *userName[] = {"ADMIN", "USER"};
    addUser(userName[0], userName[0], 213123, dataBase);
    addUser(userName[1], userName[1], 213123, dataBase);
    // limit to two users
    for (;;)
    {
        FD_ZERO(&setListener);
        FD_SET(socketListener, &setListener);
        if (j < 2)
        {
            if (select(socketListener + 1, &setListener, NULL, NULL, &timeout) > 0)
            {
                incomingSocket = accept(socketListener, &addrDummy, &socklenDummy);
                ++(userList->totalOnlineUser);
                serverAddOnlineUser(userName[j], userList, incomingSocket, dataBase);
                addUserToServerRoom(testRoom, userName[j], dataBase);
                printf("\nuser added\n");
                ++j;
            }
        }
        if (j == 2)
        {
            triagePacket(userList, roomList, dataBase, packet);
            serverRoomSpreadMessage(testRoom, dataBase);
        }
    }
    return 0;
}