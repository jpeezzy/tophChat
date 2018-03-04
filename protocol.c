#include <string.h>

#include "protocol.h"
#include "server_back_end.h"
#include "constants.h"

// slice including the start the last index
int stringSlicer(char *original, char *result, int start_index, int stop_index)
{
    int length = stop_index - start_index + 1;
    int i = 0;
    assert(strlen(result) >= length);
    for (i = 0; i < length; ++i)
    {
        result[i] = original[start_index + i];
    }
    result[i] = '\0';
}

int charToInt(char character)
{
    // values based on ASCII tables
    return (int)(character - 48);
}

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