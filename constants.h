#ifndef _CONSTANTS_H
#define _CONSTANTS_H
#include "protocol.h"

// NETWORK CONNECTION CONSTANT START HERE
#define CHAT_SERVER_ADDR "zuma.eecs.uci.edu"
#define RECONNECT_NUM 5
#define CHAT_SERVER_PORT "30001"

#define MAX_USER_NAME 20 // 20 character maximum
#define MESS_LIMIT 500   // how many char user can send
// #define PACKAGE_SIZE 510 // 500 char, 7 word command/message ID and and 2 char for room ID, 1 for /0
#define PACKAGE_SIZE 510
#define CHAT_ROOM_LIMIT 20    // a user can have maximum of
#define CHAT_ROOM_CHARACTER 3 // how many char needed to define the room number, doesn't have null terminator

#define MAX_SERVER_USERS 20 // maximum number of user the server can support

// for alpha make it 20
#define MAX_SERVER_CHATROOMS 20
//#define MAX_SERVER_CHATROOMS 400 // equal to max user * max room per user

#define MAX_USER_PER_ROOM 5

#define LISTENER_TIMEOUT 50 // 5 seconds for timeout
#define CLIENT_TIMEOUT 50   // timeout for listening on clients

// ERROR CODE STARTS HERE

#define SOCKET_CLOSED -2
#define SOCKET_NO_DATA -3
#define SOCKET_CANT_CONNECT -4
#define SOCKET_NO_CONNECTION -5

#define FIFO_FULL -3
#define FIFO_NO_DATA -4

#endif