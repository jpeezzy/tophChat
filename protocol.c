#include <string.h>
#include <math.h>
#include <assert.h>

#include "protocol.h"
#include "server_back_end.h"
#include "constants.h"
#include "utils.h"
#include "protocol_const.h"

// int processPacket(char *packet)
// {
//     // arrays carrying function pointers to appropriate actions to take
//     roomFunc allroomFunc[TOTAL_RORQ];
//     friendFunc allfriendFunc[TOTAL_FRRQ];
//     comFunc allcomFunc[TOTAL_COMRQ];

//     // -1 since strlen doesn't take int the null terminator
//     assert(strlen(packet) == PACKAGE_SIZE - 1);
//     getpacketType(packet);
//     if (getpacketType(packet) == ID_MESS)
//     {
//         // this is a message
//     }
//     else
//     {
//         // this is a command
//         int commandID = getCommandID(packet);
//         switch (packet[CHAT_ROOM_CHARACTER + ID_LENGTH])
//         {
//         case FRIENDID:
//             break;
//         case ROID:
//             break;
//         case COMID:
//             break;
//         }
//     }
// }

int getpacketType(char *packet)
{
    assert(packet);
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
    assert(packet);
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
    stringSlicer(packet, MessageBody, ID_LENGTH + CHAT_ROOM_CHARACTER, MESS_LIMIT - 1);
    return 0;
}

int getCommandID(char *packet)
{
    return charToInt(packet[CHAT_ROOM_CHARACTER + ID_LENGTH + 1]);
}

// return the letter representing the category of the command
char getCommandType(char *packet)
{
    return packet[CHAT_ROOM_CHARACTER + ID_LENGTH];
}

// extract user name from a command
int getUserName(char *packet, char *userName)
{
    stringSlicer(packet, userName, CHAT_ROOM_CHARACTER + ID_LENGTH + COM_LENGTH, PACKAGE_SIZE - 1);
    return 0;
}

// get the socket from a packet on the server room fifo
int getSocketNum(char *serverPacket)
{
    char *dummyPtr = NULL;
    char socketChar[SOCKET_NUM_CHAR + 1];
    stringSlicer(serverPacket, socketChar, 0, SOCKET_NUM_CHAR - 1);
    return (int)strtol(socketChar, &dummyPtr, 10);
}

// assemble a command from a list of details like room number, type of command and which command of the type it is
// additional info can be parameter like the friend name, put NULL if there is nothing
int assembleCommand(int roomNum, char COM_ID, int COM_NUM, char *additionInfo, char *outputCom)
{
    assert(outputCom);
    outputCom[0] = '\0';
    char roomNumChar[CHAT_ROOM_CHARACTER + 1];
    snprintf(roomNumChar, (CHAT_ROOM_CHARACTER + 1) * sizeof(char), "%d", roomNum);
    strcat(outputCom, roomNumChar);
    strcat(outputCom, ID_COMM);
    outputCom[CHAT_ROOM_CHARACTER + COM_LENGTH] = COM_ID;
    outputCom[CHAT_ROOM_CHARACTER + COM_LENGTH + 1] = intToChar(COM_NUM);
    if (additionInfo != NULL)
    {
        strcat(outputCom, additionInfo);
    }
    else
    {
        outputCom[CHAT_ROOM_CHARACTER + COM_LENGTH + 2] = '\0';
    }
    return 0;
}

int assembleMessage(int roomNum, char *message, char *outputPacket)
{
    assert(message);
    outputPacket[0] = '\0';
    char roomNumChar[CHAT_ROOM_CHARACTER + 1];
    snprintf(roomNumChar, (CHAT_ROOM_CHARACTER + 1) * sizeof(char), "%d", roomNum);
    strcat(outputPacket, roomNumChar);
    strcat(outputPacket, ID_MESS);
    strcat(outputPacket, message);
    return 0;
}
