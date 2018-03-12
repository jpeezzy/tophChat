#ifndef _PROTOCOL_CONST_H
#define _PROTOCOL_CONST_H

/**
 * PROTOCOL EXPLAINED:
 * A packet will be a string and have the form:
 * <ROOM_NUM><ID_STRING><MESSAGE OR COMMAND ID>
 * EXAMPLE:
 * define constants used in protocol, 
 * a message will have format:
 * 0011TPM{}HMESSAGESTARTHERE
 * 001 is room number
 * 1TPM{}H is ID string for message
 * after that is message
 * 
 * a command will have format
 * 1001TPC{}HF1<SENDER_NAME>/<TARGET_NAME>
 * 100 is the target room of the command
 * 1TPC{}H is the id string of the command
 * F is the identifier for type of command F is for friend
 * 1 is the command ID, every command has an ID
 * <SENDER_NAME> is the name of the person sending this message
 * <TARGET_NAME> is who the command is aimed at for example the userName of a friend
 * USER MESSAGE FORMAT:
 * 0011TPM{}H<COM_CODE><USER_NAME>/<TEXT_MESSAGE_CONTENT> max user name length defined by the macro below
 */

// define protocol command for communications
#define ID_MESS "1TPM{}H" // identify that this his a message
#define ISMESSAGE 1
#define ID_COMM "1TPC{}H" // idenfity that this is a command
#define ISCOMM 2
#define ID_LENGTH 7      // doesn't account for the null terminator
#define MAX_USER_NAME 20 // 20 character maximum
#define COM_LENGTH 2     // length of a command

// friend related request
#define FRIENDID 'F'   // initials for friend related command
#define FREQUEST 1     // friend request
#define DEREQUEST 2    // deny friend request
#define DEFRIEND 3     // delete friends
#define FRIEACCEPT 4   // accept friend request
#define FRIEACCEPTED 5 //friend request accepted
#define TOTAL_FRRQ 5   // how many friend related request we have

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

// TODO: use enum for this
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