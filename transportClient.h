#ifndef _TRANSPORT_CLIENT_H
#define _TRANSPORT_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define HOSTNAME "zuma.eecs.uci.edu"
#define PORT "20000"

typedef struct serverConnection connection;

struct serverConnection{
    
    // socket used for connection
    int socket;

    // linked list of possible connection
    struct addrinfo addrList;
};

int openConnection(struct serverConnection*);

int closeConnection(struct serverConnection*);


#endif