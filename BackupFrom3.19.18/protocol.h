#ifndef _PROTOCOL_H
#define _PROTOCOL_H
// overall wrapper of all message handling function

#include "server_back_end.h"

int processPacket(char *packet);

int assembleCommand(int roomNum, char COM_ID, int COM_NUM, char *senderName, char *targetName, char *outputCom);

int assembleMessage(int roomNum, char *senderName, char *message, char *outputPacket);
// divide special command into categories:
// room related

// friend related

// personal related only

int getpacketType(char *packet);

int getroomNumber(char *packet);

// return the body of the message doesn't have room num or DI string
int getMessageBody(char *packet, char *MessageBody);

int getCommandID(char *packet);

// return the letter representing the category of the command
char getCommandType(char *packet);

// also used to get public key
int getCommandTarget(char *packet, char *userName);

int getCommandSender(char *packet, char *userName);

int getUserFriendList(char **friendList, char *packet, int length);

// get the name of the sender of a regular message
void getSenderName(char *userName, char *packet);

#endif