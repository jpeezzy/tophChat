#ifndef _TCP_GUI_H
#define _TCP_GUI_H

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

#include "constants.h"
#include "fifo.h"

typedef struct serverConnection connection;
typedef struct messageRoom chatRoom;

struct serverConnection
{
    // socket used for connection
    int socket;
};


struct room
{
    struct serverConnection server;
    int roomNum;
    struct FIFObuffer inMessage; 
};


int openConnection(connection* server);

int closeConnection(connection* server);

int openRoom(chatRoom* room);

int closeRoom(chatRoom* room);
#endif