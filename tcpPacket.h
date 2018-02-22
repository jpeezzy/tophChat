#ifndef _TRANSPORT_CLIENT_H
#define _TRANSPORT_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "fifo.h"
#include "tcpGUI.h"
#include "constants.h"


typedef struct PacketClient clientPacket;
struct messageClient
{
    char message[MESS_LIMIT];
    uint8_t roomNum;
};

int sendPacket(clientPacket* packet, struct serverConnection* server);

int fetchPacket(clientPacket* packet, struct serverConnection* server);


#endif