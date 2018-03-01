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
#include "tcpGUI.h"

typedef struct PacketClient clientPacket;
// TODO: change this to the agreed string format 
struct PacketClient
{
    char message[MESS_LIMIT];
    uint8_t roomNum;
};

// send one packet
int sendPacket(void *packet, struct serverConnection *server);

// receive one packet
int fetchPacket(clientPacket *packet, struct serverConnection *server);

// process the packets in the FIFO
int manageIO(connection *server, fifo *inputFIFO, fifo *outputFIFO);

#endif