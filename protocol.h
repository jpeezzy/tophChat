#ifndef _PROTOCOLS_H
#define _PROTOCOLS_H
#include "server_back_end.h"

// define protocol command for communications
#define ID_MESS "1TPM{}H" // identify that this his a message
#define ID_COMM "1TPC{}H" // idenfity that this is a command
#define ID_LENGTH 7       // doesn't account for the null terminator

// friend related request
#define FRIENDID 'F' // initials for friend related command
#define FREQUEST 1   // friend request
#define DEREQUEST 2  // deny friend request
#define DEFRIEND 3   // delete friends
#define TOTAL_FRRQ 3 // how many friend related request we have

// room related request
#define ROID 'R'
#define ROCREATE 1 // need to make someone admin
#define RODEL 2    // delete room, only admin can do that
#define ROADMIN 3  // change admin
#define ROINVITE 4 // invite someone to room
#define TOTAL_RORQ 4

// communication related
#define COMID 'C'
#define CLOSECOM 1 // close the communication
#define OPENCOM 2  // open a new communication
#define TOTAL_COMRQ 2

// return value of parse function based on the the ype of request
#define VAL_FREQUEST 1  // friend request
#define VAL_DEREQUEST 2 // deny friend request
#define VAL_DEFRIEND 3  // delete friends
#define VAL_CLOSECOM 4  // close the communication
#define VAL_OPENCOM 5   // open a new communication
#define VAL_ROCREATE 6  // need to make someone admin
#define VAL_RODEL 7     // delete room, only admin can do that
#define VAL_ROADMIN 8   // change admin

// overall wrapper of all message handling function

int processPacket(char *packet);
// return the type of message
int messageTriage(char *message);

// divide special command into categories:
// room related

// friend related

// personal related only

// take a packet and decode it into a room number and message
int messageDecode(int *roomNum, char *message);
#endif