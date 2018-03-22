#ifndef USER_ACTIONS_H
#define USER_ACTIONS_H
#include "tcpGUI.h"

int userSignUp(char *userName, char *passWord, serverConnection *server);

int userLogin(char *userName, char *passWord, serverConnection *server);

int receiveRoom(roomList *allRoom, int serverroomNum);

int sendFriendInvite(char *userName, char *receiverName, fifo *outputBuffer);

int sendRoomInvite(int roomNum, char *userName, char *receiverName, fifo *outputBuffer);

int joinCreatedRoom(roomList *allRoom, int roomNumber);

int joinInvitedRoom(roomList *allRoom, int roomNumber, char *userName, fifo *outputBuffer);

int denyInvitedRoom(int roomNumber, char *userName, char *targetName, fifo *outputBuffer);

int leaveRoom(chatRoom *room, char *userName, fifo *outputBuffer);

int closeRoom(chatRoom *room, char *userName, fifo *outputBuffer);

int acceptFriendInvite(char *userName, char *receiverName, fifo *outputBuffer);

int denyFriendInvite(char *userName, char *receiverName, fifo *outputBuffer);

// copy the received message to the buffer
int fetchMessage(chatRoom *room, char *buffer);

// copy the user message to the output queue of the program
int sendMessage(chatRoom *room, fifo *outputFIFO, char *userName, char *message);

#endif

