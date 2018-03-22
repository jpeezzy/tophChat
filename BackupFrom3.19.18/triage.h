#ifndef _TRIAGE_PACKET_H
#define _TRIAGE_PACKET_H

#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include <netdb.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>
#include <strings.h>	//bzero
#include <sys/select.h> //io multiplexing

#include "constants.h"
#include "fifo.h"
#include "tophChatUsers.h"
#include "server_back_end.h"

typedef struct
{
	char message[MESS_LIMIT];
	char sentPacket[PACKAGE_SIZE];
	char packet[PACKAGE_SIZE];
	char senderName[MAX_USER_NAME];
	char receiverName[MAX_USER_NAME];
	TUSER *tempUser;
	TUSER *tempUser2;
	int roomNum;
	serverChatRoom *onlineRoom;
	int errorflag;

} triagePacket;

int processTriagePacket(onlineUserList *userList, struct allServerRoom *allRoom, TINFO *dataBase);

void packageProcess(triagePacket *tpacket, onlineUserList *userList, struct allServerRoom *allRoom, TINFO *dataBase);

void messageProcess( struct allServerRoom *allRoom, triagePacket *tpacket, TINFO *dataBase);

void commandProcess(triagePacket *tpacket, onlineUserList *userList, struct allServerRoom *allRoom, TINFO *dataBase);

void processFriendID(triagePacket *tpacket, TINFO *database);

void processRoomID(triagePacket *tpacket, struct allServerRoom *allRoom, TINFO *dataBase, int roomNum);

void createRoom(triagePacket *tpacket, struct allServerRoom *allRoom, TINFO *dataBase, int roomNum);

void roomInvite(triagePacket *tpacket, struct allServerRoom *allRoom, TINFO *dataBase, int roomNum);

void acceptRoom(triagePacket *tpacket, TINFO *dataBase, int roomNum);

void processCommandID(triagePacket *tpacket, onlineUserList *userList, struct allServerRoom *allRoom, TINFO *dataBase);

#endif
