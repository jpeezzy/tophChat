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
#include "tcpPacket.h"

#define DEBUG 

int main_loop(void)
{
    serverConnection* serverEndPoint=openConnection();
    closeConnection(serverEndPoint);
    return 0;
}