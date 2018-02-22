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


#define HOSTNAME "zuma.eecs.uci.edu"
#define PORT "20000"
#define MESS_LIMIT 500 // how many char user can send


typedef struct serverConnection connection;
typedef struct messageClient clientPacket;
typedef struct FIFObuffer fifo;
typedef struct messageRoom chatRoom;
// place holder for more stuffs 
struct serverConnection
{
    // socket used for connection
    int socket;
};

struct messageClient
{
    char message[MESS_LIMIT];
    uint8_t roomNum;
};

// buffer used to hold messages coming into a chat room 
struct FIFObuffer
{
    char* buffer[20];
    int curPos; 
};

struct room
{
    struct serverConnection server;
    int roomNum;
    struct FIFObuffer inMessage; 
};

int openConnection(connection* server);

int closeConnection(connection* server);

// place holder for interacting with room struct
int openRoom(chatRoom* room);

int closeRoom(chatRoom* room);

int updateFIFO(fifo* buffer);

int readBuffer(fifo* buffer);

int writeBuffer(fifo* buffer);

int sendPacket(clientPacket* packet, struct serverConnection* server);

int fetchPacket(clientPacket* packet, struct serverConnection* server);


#endif