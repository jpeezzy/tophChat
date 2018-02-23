#ifndef _CONSTANTS_H
#define _CONSTANTS_H

// NETWORK CONNECTION CONSTANT START HERE
#define CHAT_SERVER_ADDR "zuma.eecs.uci.edu"
#define RECONNECT_NUM 5
#define CHAT_SERVER_PORT "30001"
#define PORT "20000"
#define MESS_LIMIT 500 // how many char user can send

// ERROR CODE STARTS HERE

#define SOCKET_CLOSED -2
#define SOCKET_NO_DATA -3
#define SOCKET_CANT_CONNECT -4

#define FIFO_FULL -3

#endif