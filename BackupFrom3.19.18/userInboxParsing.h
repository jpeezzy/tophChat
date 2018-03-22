#ifndef _USER_INBOX_PARSING_H
#define _USER_INBOX_PARSING_H

#include "tcpGUI.h"
#include "fifo.h"
#include "constants.h"
#include "protocol.h"
#include "protocol_const.h"
#include "tcpPacket.h"
#include "utils.h"
#include "userActions.h"
#include "GTKMain.h"


typedef struct inboxInformationStruct
{
    inboxQueue *inbox;
    roomList *roomList;
    int roomNum;
    fifo *outputBuffer;
    char *userName;
    serverConnection *server;
    char *packet;
    char comType;
    int comID;
    char **friendList;
    char **allFriendList;
    char *message;
    MESSAGE_STRUCT* messageStruct;
} inboxInformationStruct;

int parseInboxCommand(inboxInformationStruct *infoStruct);

int handleRO(inboxInformationStruct *infoStruct);

int handleCO(inboxInformationStruct *infoStruct);

int handleFR(inboxInformationStruct *infoStruct);

int handleROP(inboxInformationStruct *infoStruct);

inboxInformationStruct *createInboxInfoStruct(void);

int delInboxInfoStruct(inboxInformationStruct *infoStruct);

#endif