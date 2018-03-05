#ifndef _PROTOCOL_CONST_H
#define _PROTOCOL_CONST_H

/**
 * PROTOCOL EXPLAINED:
 * A packet will be a string and have the form:
 * "string_numIDstring_COMMAND_OR_MESSAGE"
 * EXAMPLE:
 * define constants used in protocol, 
 * a message will have format:
 * 0011TPM{}HMESSAGESTARTHERE
 * 001 is room number
 * 1TPM{}H is ID string for message
 * after that is message
 * 
 * a command will have format
 * 1001TPC{}HF1
 * 100 is the target room of the command
 * 1TPC{}H is the id string of the command
 * F is the identifier for type of command F is for friend
 * 1 is the command ID, every command has an ID
 */

// define protocol command for communications
#define ID_MESS "1TPM{}H" // identify that this his a message
#define ISMESSAGE 1
#define ID_COMM "1TPC{}H" // idenfity that this is a command
#define ISCOMM 2
#define ID_LENGTH 7 // doesn't account for the null terminator

#define COM_LENGTH 2 // length of a command

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
#define ROACCEPT 5 // accept room request
#define TOTAL_RORQ 5

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

#endif