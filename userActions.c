#include "userActions.h"
#include "fifo.h"
#include "tcpGUI.h"
#include "constants.h"
#include "protocol_const.h"

// ROOM EXIT

// when receiving the confirmation of room from a server, mark the room as ready
int receiveRoom(roomList *allRoom, int serverroomNum)
{
    int i = 0;
    for (i = 0; i < allRoom->totalRoom; ++i)
    {
        if ((allRoom->roomList)[i].status == ROOM_UNALLOCATED)
        {
            (allRoom->roomList)[i].roomNum = serverroomNum;
            (allRoom->roomList)[i].status = ROOM_READY;
            break;
        }
    }

    if (i == allRoom->totalRoom)
    {
        return NO_WAITING_ROOM;
    }
    else
    {
        return 0;
    }
}

int joinCreatedRoom(roomList *allRoom, int roomNumber)
{
    if (receiveRoom(allRoom, roomNumber) >= 0)
    {
        chatRoom *tempRoom = retrieveRoom(allRoom, roomNumber);
        tempRoom->status = ROOM_TAKEN;
        return 0;
    }
    else
    {
        return -1;
    }
}

int joinInvitedRoom(roomList *allRoom, int roomNumber, char *userName, serverConnection *server, fifo *outputBuffer)
{
    char packet[PACKAGE_SIZE];
    receiveRoom(allRoom, roomNumber);

    if (receiveRoom(allRoom, roomNumber) >= 0)
    {
        chatRoom *tempRoom = retrieveRoom(allRoom, roomNumber);
        tempRoom->status = ROOM_TAKEN;
        assembleCommand(roomNumber, ROID, ROACCEPT, userName, NULL, packet);
        return writeBuffer(outputBuffer, packet);
    }
    else
    {
        // TODO: Change error code
        return -1;
    }
}

int denyInvitedRoom(roomList *allRoom, int roomNumber, char *userName, serverConnection *server, fifo *outputBuffer)
{
    char packet[PACKAGE_SIZE];

    assembleCommand(roomNumber, ROID, RODENY, userName, NULL, packet);
    return writeBuffer(outputBuffer, packet);
}

int leaveRoom(chatRoom *room, char *userName, serverConnection *server, fifo *outputBuffer)
{
    char packet[PACKAGE_SIZE];
    closeBuffer(room->inMessage);
    room->inMessage = initBuffer(CLIENT_CHAT_ROOM_INTPUT_FIFO_MAX);
    room->status = ROOM_UNALLOCATED;
    assembleCommand(room->roomNum, ROID, ROLEAVE, userName, NULL, packet);
    writeBuffer(outputBuffer, packet);
    return 0;
}

int closeRoom(chatRoom *room, serverConnection *server, char *userName, fifo *outputBuffer)
{
    char packet[PACKAGE_SIZE];
    closeBuffer(room->inMessage);
    room->inMessage = initBuffer(CLIENT_CHAT_ROOM_INTPUT_FIFO_MAX);
    room->status = ROOM_UNALLOCATED;
    assembleCommand(room->roomNum, ROID, RODEL, userName, NULL, packet);
    writeBuffer(outputBuffer, packet);
    return 0;
}

// copy the received message to the buffer
int fetchMessage(chatRoom *room, char *buffer)
{
    assert(room);
    assert(buffer);
    // TODO: implement muxtex if using multithreaded
    if (readBuffer(room->inMessage, buffer) == FIFO_NO_DATA)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

// copy the user message to the output queue of the program
int sendMessage(chatRoom *room, fifo *outputFIFO, char *userName, char *message)
{
    assert(message);
    char tempPacket[PACKAGE_SIZE];
    // TODO: multithreaded mutex
    assembleMessage(room->roomNum, userName, message, tempPacket);
    writeBuffer(room->inMessage, message);
    if (writeBuffer(outputFIFO, tempPacket) == FIFO_FULL)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

// close the room and let the server know that the room number is free to be used by others
int closeRoom(chatRoom *room, fifo *outputFIFIO, char *userName)
{
    char tempPacket[PACKAGE_SIZE] = "";
    room->status = ROOM_UNALLOCATED; // freed
    assembleCommand(room->roomNum, ROID, RODEL, userName, NULL, tempPacket);
    closeBuffer(room->inMessage);
    room->inMessage = initBuffer(CLIENT_CHAT_ROOM_INTPUT_FIFO_MAX);
    writeBuffer(outputFIFIO, tempPacket);
    return 0;
}