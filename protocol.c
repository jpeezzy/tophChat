#include <string.h>

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
    stringSlicer(packet, messageType, CHAT_ROOM_CHARACTER, CHAT_ROOM_CHARACTER + ID_LENGTH - 1);

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

int messageDecode(int *roomNum, char *message);