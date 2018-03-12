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

int getCommandID(char *packet)
{
    return charToInt(packet[CHAT_ROOM_CHARACTER + ID_LENGTH + 1]);
}

// return the letter representing the category of the command
char getCommandType(char *packet)
{
    return packet[CHAT_ROOM_CHARACTER + ID_LENGTH];
}

// extract sender name from a command
int getCommandSender(char *packet, char *userName)
{
    int separatorIndex = 0;
    for (int i = CHAT_ROOM_CHARACTER + ID_LENGTH + COM_LENGTH; i < PACKAGE_SIZE - 1; ++i)
    {
        if (packet[i] == '/')
        {
            separatorIndex = i;
            break;
        }
    }

    stringSlicer(packet, userName, CHAT_ROOM_CHARACTER + ID_LENGTH + COM_LENGTH, separatorIndex - 1);
    return 0;
}

void getCommandTarget(char *packet, char *userName)
{
    int separatorIndex = 0;
    for (int i = CHAT_ROOM_CHARACTER + ID_LENGTH + COM_LENGTH; i < PACKAGE_SIZE - 1; ++i)
    {
        if (packet[i] == '/')
        {
            separatorIndex = i;
            break;
        }
    }

    stringSlicer(packet, userName, separatorIndex + 1, PACKAGE_SIZE - 2);
}

void getSenderName(char *userName, char *packet)
{
    int separatorIndex = 0;
    for (int i = CHAT_ROOM_CHARACTER + ID_LENGTH; i < PACKAGE_SIZE - 1; ++i)
    {

        if (packet[i] == '/')
        {
            separatorIndex = i;
            break;
        }
    }
    stringSlicer(packet, userName, CHAT_ROOM_CHARACTER + ID_LENGTH, separatorIndex - 1);
}

// return the body of the message doesn't have room num or DI string
int getMessageBody(char *packet, char *MessageBody)
{
    int separatorIndex = 0;
    for (int i = CHAT_ROOM_CHARACTER + ID_LENGTH; i < PACKAGE_SIZE - 1; ++i)
    {

        if (packet[i] == '/')
        {
            separatorIndex = i;
            break;
        }
    }
    stringSlicer(packet, MessageBody, separatorIndex + 1, PACKAGE_SIZE - 1);
    return 0;
}

// assemble a command from a list of details like room number, type of command and which command of the type it is
// additional info can be parameter like the friend name, put NULL if there is nothing
// int assembleCommand(int roomNum, char COM_ID, int COM_NUM, char *senderName, char *targetName, char *additionInfo, char *outputCom)
// {
//     assert(outputCom);
//     char namePadding[] = "12345678912345678912";
//     outputCom[0] = '\0';

//     outputCom[2] = intToChar(roomNum % 10);
//     roomNum /= 10;
//     outputCom[1] = intToChar(roomNum % 10);
//     roomNum /= 10;
//     outputCom[0] = intToChar(roomNum % 10);
//     outputCom[3] = '\0';
//     strcat(outputCom, ID_COMM);
//     outputCom[CHAT_ROOM_CHARACTER + ID_LENGTH] = COM_ID;
//     outputCom[CHAT_ROOM_CHARACTER + ID_LENGTH + 1] = intToChar(COM_NUM);
//     outputCom[CHAT_ROOM_CHARACTER + ID_LENGTH + COM_LENGTH] = '\0';
//     strcat(outputCom, senderName);
//     if (targetName != NULL)
//     {
//         strcat(outputCom, targetName);
//     }
//     else
//     {
//         strcat(outputCom, namePadding);
//     }

//     if (additionInfo != NULL)
//     {
//         strcat(outputCom, additionInfo);
//     }
//     return 0;
// }

// int assembleMessage(int roomNum, char *senderName, char *message, char *outputPacket)
// {
//     assert(message);
//     outputPacket[0] = '\0';
//     char roomNumChar[CHAT_ROOM_CHARACTER + 1];
//     snprintf(roomNumChar, (CHAT_ROOM_CHARACTER + 1) * sizeof(char), "%d", roomNum);
//     strcat(outputPacket, roomNumChar);
//     strcat(outputPacket, ID_MESS);
//     strcat(outputPacket, senderName);
//     strcat(outputPacket, message);
//     return 0;
// }
