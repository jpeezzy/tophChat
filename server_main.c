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

#define ALPLHA_RELEASE

int spawnChildThread()
{
}

// return an available room number
int assignRoom(roomList *);

// TODO: implement multi-threaded

#ifdef ALPLHA_RELEASE
// alpha release will just have two machines connecting to each other
int main_loop(void)
{

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