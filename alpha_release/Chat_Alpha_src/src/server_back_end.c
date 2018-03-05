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
#include <errno.h>

#include "server_back_end.h"
#include "constants.h"
#include "testString.h"
#include "tcpPacket.h"

#define TEST_SERVER
int listenSocketInit(void)
{
    struct timeval timeoutListener;
    timeoutListener.tv_sec = LISTENER_TIMEOUT;

    struct timeval timeoutClient;
    timeoutListener.tv_sec = CLIENT_TIMEOUT;

    // the socket that will be listening
    struct addrinfo *serverListener;
    int socketListener;
    // give the server hints about the type of connection we want
    struct addrinfo serverHints;
    memset(&serverHints, 0, sizeof(serverHints));
    serverHints.ai_family = AF_INET;       // IPV4 address
    serverHints.ai_socktype = SOCK_STREAM; // TCP socket
    serverHints.ai_flags = AI_PASSIVE;     // listening socket

    // create a socket for listening incoming collection
    getaddrinfo(NULL, CHAT_SERVER_PORT, &serverHints, &serverListener);
    socketListener = socket(serverListener->ai_family, serverListener->ai_socktype, 0);
    if (socketListener < 0)
    {
        freeaddrinfo(serverListener);
        return SOCKET_NO_CONNECTION;
    }
    else
    {
        if (bind(socketListener, serverListener->ai_addr, serverListener->ai_addrlen) < 0)
        {
            printf("The error is %s\n", strerror(errno));
        }
        assert(listen(socketListener, MAX_SERVER_USERS) >= 0);
        freeaddrinfo(serverListener);
        return socketListener;
    }
}

struct allServerRoom *serverRoomSetCreate(void)
{
    struct allServerRoom *allRoom = malloc(sizeof(struct allServerRoom));
    allRoom->firstFreeRoom = 0;
    allRoom->totalRoom = MAX_SERVER_CHATROOMS;
    for (int i = 0; i < allRoom->totalRoom; ++i)
    {
        ((allRoom->roomList)[i]).isOccupied = 0;
        ((allRoom->roomList)[i]).roomNum = i;
        ((allRoom->roomList)[i]).peopleNum = 0;
    }
}
void serverRoomSetDel(struct allServerRoom *allRoom)
{
    free(allRoom);
}

// update which room is not occupied
void updateFreeRoom(struct allServerRoom *allRoom)
{
    int i;
    for (i = 0; i < allRoom->totalRoom; ++i)
    {
        if (((allRoom->roomList)[i]).isOccupied == 0)
        {
            allRoom->firstFreeRoom = i;
            break;
        }
        if (i == allRoom->totalRoom)
        {
            allRoom->firstFreeRoom = -1;
        }
    }
}

// return a free room
struct messageServerRoom *serverRoomCreate(struct allServerRoom *allRoom)
{
    assert(allRoom);
    if (allRoom->firstFreeRoom == -1)
    {
        return NULL;
    }
    else
    {
        return &(allRoom->roomList[allRoom->firstFreeRoom]);
    }
}

// mark the room as free
void serverRoomReturn(serverChatRoom *room)
{
    room->isOccupied = 0;
    room->peopleNum = 0;
}

// send message to everyone in the room except the writer
int serverRoomSpreadMessage(int writerSocket, struct messageServerRoom *room, char *packet)
{
    assert(packet);
    assert(room);
    assert(writerSocket >= 0);
    for (int i = 0; i < room->peopleNum; ++i)
    {
        if ((room->socketList[i]) != writerSocket)
        {
            // TODO: Change this
            assert(sendPacket(packet, room->socketList[i]) >= 0);
        }
    }
}

#ifdef TEST_SERVER
int main(int argc, char *argv[])
{
    struct sockaddr *addr;
    socklen_t len;
    char dummyPacket[PACKAGE_SIZE];
    char receivedPacket[PACKAGE_SIZE];
    int socket = listenSocketInit();
    int newsocket = accept(socket, addr, &len);
    assert(newsocket >= 0);
    if (strcmp(argv[1], "TestClientSend") == 0)
    {
        recv(newsocket, receivedPacket, PACKAGE_SIZE, 0);
        printf("the comparision result is %d\n", strcmp(receivedPacket, testString1));
    }
    else if (strcmp(argv[1], "TestClientFetch") == 0)
    {
        assert(send(newsocket, testString1, PACKAGE_SIZE * sizeof(char), 0) >= 0);
        printf("\nsent package\n");
    }
    close(socket);
    close(newsocket);
    return 0;
}
#endif