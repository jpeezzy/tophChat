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
#define ID_COMM "1TPC{}H"  // idenfity that this is a command
#define PASSWORD_LENGTH 30 // maximum password length
#define ISCOMM 2
#define ID_LENGTH 7      // doesn't account for the null terminator
#define MAX_USER_NAME 10 // 20 character maximum
#define COM_LENGTH 2     // length of a command

// friend related request
#define FRIENDID 'F' // initials for friend related command
#define FREQUEST 1   // friend request
#define DEREQUEST 2  // deny friend request
#define DEFRIEND 3   // delete friends
#define FRIEACCEPT 4 // accept friend request

// room related request
// active requests like asking for room
#define ROID 'R'
#define ROREQUEST 0 // request to join room
#define ROLEAVE 1   // request to leave room
#define ROCREATE 2  // create room, owner is first admin
#define RODEL 3     // delete room, only admin can do that
#define ROADMIN 4   // change admin
#define ROINVITE 5  // invite someone to room
#define ROACCEPT 6  // accept room request
#define RODENY 7    // deny room request
#define ROSYNC 8    // SYNC the friend list with the server

// passive info like you get denied
#define ROID_PASSIVE 'A'
#define ROGRANTED 0        // room opening granted
#define RODENIED 1         // request to open room denied
#define ROOMJOINACCEPTED 2 // request to join room accepted
#define ROOMJOINDENIED 3   // invitation to join room denied
#define ROINVITED 4        // invited to a room
#define ROSYNCED 5

// communication related
#define COMID 'C'
#define CLOSECOM 1        // close the communication
#define OPENCOM 2         // open a new communication
#define GETONLINEFRIEND 3 // get the list of currently online frined
#define GETFRIEND 4       // get the list of all the person friend
#define SIGNUP 5
#define LOGIN 6
#define GETONLINEPEOPLE 7

#define LOGIN_REQUEST "lr"
#define LOGIN_SUCCESS "y"
#define LOGIN_FAILED_PASSWORD "1"
#define LOGIN_FAILED_SERVER_FULL "2"
#define LOGIN_FAILED_EXISTED "3"        // user doesn't exist
#define LOGIN_FAILED_ALREADY_ONLINE "4" // user already online

// return result for the LOGIN function
#define RESULT_LOGIN_SUCCESS 0
#define RESULT_LOGIN_FAILED_PASSWORD 1
#define RESULT_LOGIN_FAILED_SERVER_FULL 2
#define RESULT_LOGIN_FAILED_EXISTED 3        // user doesn't exist
#define RESULT_LOGIN_FAILED_ALREADY_ONLINE 4 // user already online

#define SIGNUP_REQUEST "sr"
#define SIGNUP_SUCCESS "1y"
#define SIGN_UP_FAILED_INVALID_NAME "11"
#define SIGN_UP_FAILED_SERVER_FULL "12"
#define SIGN_UP_FAILED_USER_EXISTED "13"

// return result for the signup function
#define RESULT_SIGNUP_SUCCESS 0
#define RESULT_SIGN_UP_FAILED_INVALID_NAME 1
#define RESULT_SIGN_UP_FAILED_SERVER_FULL 2
#define RESULT_SIGN_UP_FAILED_USER_EXISTED 3

// TODO: use enum for this
// return value of parse function based on the the the of request
#define VAL_FREQUEST 1  // friend request
#define VAL_DEREQUEST 2 // deny friend request
#define VAL_DEFRIEND 3  // delete friends
#define VAL_CLOSECOM 4  // close the communication
#define VAL_OPENCOM 5   // open a new communication
#define VAL_ROCREATE 6  // need to make someone admin
#define VAL_RODEL 7     // delete room, only admin can do that
#define VAL_ROADMIN 8   // change admin
#define VAL_LOGIN 9
#define VAL_LOGOUT 10
#define VAL_SIGNUP 11

#endif