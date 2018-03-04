#include <string.h>
#include <math.h>

#include "protocol.h"
#include "server_back_end.h"
#include "constants.h"
#include "utils.h"

int processPacket(char *packet)
{
    // arrays carrying function pointers to appropriate actions to take
    roomFunc allroomFunc[TOTAL_RORQ];
    friendFunc allfriendFunc[TOTAL_FRRQ];
    comFunc allcomFunc[TOTAL_COMRQ];

    // -1 since strlen doesn't take int the null terminator
    assert(strlen(packet) == PACKAGE_SIZE - 1);
    char messageType[ID_LENGTH + 1];
    getpacketType(packet, messageType);
    if (strcmp(messageType, ID_MESS) == 0)
    {
        // this is a message
    }
    else
    {
        // this is a command
        int commandID = charToInt(packet[CHAT_ROOM_CHARACTER + ID_LENGTH] + 1);
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

int getpacketType(char* packet, char* messageType)
{
    stringSlicer(packet, messageType, CHAT_ROOM_CHARACTER, CHAT_ROOM_CHARACTER + ID_LENGTH - 1);
    return 0;
}

int getroomNumber(char* packet)
{
    char roomNum[CHAT_ROOM_CHARACTER+1];
    int roomNumInt=0;
    stringSlicer(packet, roomNUm, 0, CHAT_ROOM_CHARACTER-1);
    for(int i=0; i<CHAT_ROOM_CHARACTER;++i)
    {
        roomNumInt+=pow(10, (CHAT_ROOM_CHARACTER-1-i))* charToInt(roomNUm[i]);
    }
    return roomNumInt;
}

int messageDecode(int *roomNum, char *message);