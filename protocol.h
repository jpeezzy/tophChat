#ifndef _PROTOCOLS_H
#define _PROTOCOLS_H
// define protocol command for communications
#define ID_MESS "1TPM{}H" // identify that this his a message
#define ID_COMM "1TPC{}H" // idenfity that this is a command

#define FREQUEST "REQ"       // friend request
#define DEREQUEST "DEQ"      // deny friend request
#define DEFRIEND "DEFR"      // delete friends
#define CLOSECOM "CLOSECHAT" // close the communication
#define OPENCOM "OPENCHAT"   // open a new communication

// return value of parse function based on the the ype of request
#define VAL_FREQUEST 1  // friend request
#define VAL_DEREQUEST 2 // deny friend request
#define VAL_DEFRIEND 3  // delete friends
#define VAL_CLOSECOM 4  // close the communication
#define VAL_OPENCOM 5   // open a new communication
#define VAL_MESSAGE 0   // just a regular message

#endif