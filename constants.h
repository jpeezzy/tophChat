#ifndef _CONSTANTS_H
#define _CONSTANTS_H

// NETWORK CONNECTION CONSTANT START HERE
#define CHAT_SERVER_ADDR "zuma.eecs.uci.edu"
#define RECONNECT_NUM 5 //how many times server try to reconnect
#define CHAT_SERVER_PORT "5000"

#define SOCKET_NUM_CHAR 5 // how many char needed to describe the socket number
#define MAX_USER_NAME 20  // 20 character maximum
#define MESS_LIMIT 500    // how many char user can send
// #define PACKAGE_SIZE 510 // 500 char, 7 word command/message ID and and 2 char for room ID, 1 for /0
#define PACKAGE_SIZE 515                    // 5 additional char for socket number
#define CHAT_ROOM_LIMIT 20                  // a user can have maximum of
#define CHAT_ROOM_CHARACTER 3               // how many char needed to define the room number, doesn't have null terminator

// Queue Limit Constant
#define CLIENT_CHAT_ROOM_INTPUT_FIFO_MAX 30 // can store 30 messages in the fifo
#define SERVER_CHAT_ROOM_INPUT_FIFO_MAX 100
#define CLIENT_OUTPUT_FIFO_MAX 100 // how many messages can output fifo store
#define MAX_SERVER_USERS 20 // maximum number of user the server can support

// maximum inbox request the user can ahve
#define MAX_REQUEST 10
#define MAX_SERVER_CHATROOMS 400 // equal to max user * max room per user

#define MAX_USER_PER_ROOM 5

#define LISTENER_TIMEOUT 50 // seconds for timeout
#define CLIENT_TIMEOUT 50   // timeout for listening on clients

// ERROR CODE STARTS HERE

#define SOCKET_CLOSED -2
#define SOCKET_NO_DATA -3
#define SOCKET_CANT_CONNECT -4
#define SOCKET_NO_CONNECTION -5

#define FIFO_FULL -3
#define FIFO_NO_DATA -4

#define UNKNOWN_COMMAND_TYPE -6

#define NO_WAITING_ROOM -1
#define NO_SUCH_ROOM_EXIST -2

#define STR_BUFFER_TOO_SHORT -1

#define SERVER_FULL -1

//ROOM STATUS GO HERE FOR CLIENTS
#define ROOM_TAKEN 3
#define ROOM_READY 2       // synchronized with server
#define ROOM_WAITING 1     // used for marking which room is wating
#define ROOM_UNALLOCATED 0 // haven't talked to server

//ROOM STATUS GO HERE FOR SERVER
#define ROOM_NOT_OCCUPIED 0
#define ROOM_BUSY 1

//USER PERSONAL STUFFS GO HERE
#define NOT_ONLINE 0
#define ONLINE 1

#endif