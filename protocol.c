#include <string.h>
#include <math.h>

#include "protocol.h"
#include "server_back_end.h"
#include "constants.h"
#include "utils.h"
#include "protocol_const.h"

int processPacket(char *packet)
{
    // arrays carrying function pointers to appropriate actions to take
    roomFunc allroomFunc[TOTAL_RORQ];
    friendFunc allfriendFunc[TOTAL_FRRQ];
    comFunc allcomFunc[TOTAL_COMRQ];

    // -1 since strlen doesn't take int the null terminator
    assert(strlen(packet) == PACKAGE_SIZE - 1);
    getpacketType(packet);
    if (getpacketType(packet) == ID_MESS)
    {
        // this is a message
    }
    else
    {
        // this is a command
        int commandID = getCommandID(packet);
        switch (packet[CHAT_ROOM_CHARACTER + ID_LENGTH])
        {
        case FRIENDID:
            break;
        case ROID:
            break;
        case COMID:
            break;
        }
    }
}

int getpacketType(char *packet)
{
    char messageType[CHAT_ROOM_CHARACTER + 1];
    stringSlicer(packet, messageType, CHAT_ROOM_CHARACTER, CHAT_ROOM_CHARACTER + ID_LENGTH - 1);
    if (strcmp(messageType, ID_MESS) == 0)
    {
        return ISMESSAGE;
    }
    else if (strcmp(messageType, ID_COMM) == 0)
    {
        return ISCOMM;
    }
    else
    {
        return -1;
    }
}

int getroomNumber(char *packet)
{
    char roomNum[CHAT_ROOM_CHARACTER + 1];
    int roomNumInt = 0;
    stringSlicer(packet, roomNum, 0, CHAT_ROOM_CHARACTER - 1);
    for (int i = 0; i < CHAT_ROOM_CHARACTER; ++i)
    {
        roomNumInt += pow(10, (CHAT_ROOM_CHARACTER - 1 - i)) * charToInt(roomNum[i]);
    }
    return roomNumInt;
}

// return the body of the message doesn't have room num or DI string
int getMessageBody(char *packet, char *MessageBody)
{
    stringSlicer(packet, MessageBody, 0, MESS_LIMIT - 1);
}

int getCommandID(char *packet)
{
    return charToInt(packet[CHAT_ROOM_CHARACTER + ID_LENGTH] + 1);
}

// return the letter representing the category of the command
char getCommandType(char *packet)
{
    return packet[CHAT_ROOM_CHARACTER + ID_LENGTH];
}
int messageDecode(int *roomNum, char *message);

