#include "userActions.h"
#include "fifo.h"
#include "tcpGUI.h"
#include "constants.h"
#include "protocol_const.h"
#include "protocol.h"
#include "tcpPacket.h"
#include "utils.h"

// ROOM EXIT

int userSignUp(char *userName, char *passWord, serverConnection *server)
{
    printf("\nentering user sign up\n");
    char packet[PACKAGE_SIZE];
    char result[MAX_USER_NAME] = "";

    assembleCommand(0, COMID, SIGNUP, userName, passWord, packet);
    sendPacket(packet, server->socket);

    printf("\nwaiting for server response\n");
    recv(server->socket, packet, sizeof(char) * PACKAGE_SIZE, 0);
    printf("\nreceived one packet %s with command type %c\n", packet, getCommandType(packet));

    if (getCommandType(packet) == COMID)
    {
        printf("\nthe command type is C\n");
    }
    else
    {
        printf("\nwaiting for correct command\n");
        // wait until the correct type of command arrived
        recv(server->socket, packet, sizeof(char) * PACKAGE_SIZE, 0);
    }

    printf("\ndone getting result is %s\n", result);
    if (!strcmp(result, SIGNUP_SUCCESS))
    {
        printf("\nthe signup is successful\n");
        return RESULT_SIGNUP_SUCCESS;
    }
    else if (!strcmp(result, SIGN_UP_FAILED_INVALID_NAME))
    {
        return RESULT_SIGN_UP_FAILED_INVALID_NAME;
    }
    else if (!strcmp(result, SIGN_UP_FAILED_SERVER_FULL))
    {
        return RESULT_SIGN_UP_FAILED_SERVER_FULL;
    }
    else if (!strcmp(result, SIGN_UP_FAILED_USER_EXISTED))
    {
        return RESULT_SIGN_UP_FAILED_USER_EXISTED;
    }

    return -1;
}

int userLogin(char *userName, char *passWord, serverConnection *server)
{
    printf("\nentering user login\n");
    char packet[PACKAGE_SIZE];
    char result[MAX_USER_NAME] = "";

    assembleCommand(0, COMID, LOGIN, userName, passWord, packet);
    printf("\nsending packet %s\n", packet);
    sendPacket(packet, server->socket);
    printf("\nwaiting for response from server for login\n");

    recv(server->socket, packet, sizeof(char) * PACKAGE_SIZE, MSG_WAITALL);
    printf("\nthe packet is %s\n", packet);
    if (getCommandType(packet) == COMID)
    {
        printf("\nthe command type is C\n");
    }
    else
    {
        printf("\nwaiting for correct command\n");
        // wait until the correct type of command arrived
        recv(server->socket, packet, sizeof(char) * PACKAGE_SIZE, 0);
    }

    getCommandSender(packet, result);

    printf("\nthe packet is %s\n", packet);
    if (!strcmp(result, LOGIN_SUCCESS))
    {
        return RESULT_LOGIN_SUCCESS;
    }
    else
    {

        printf("\nthere is an error\n");
        if (!strcmp(result, LOGIN_FAILED_ALREADY_ONLINE))
        {
            return RESULT_LOGIN_FAILED_ALREADY_ONLINE;
        }
        else if (!strcmp(result, LOGIN_FAILED_EXISTED))
        {
            return RESULT_LOGIN_FAILED_EXISTED;
        }
        else if (!strcmp(result, LOGIN_FAILED_PASSWORD))
        {
            return RESULT_LOGIN_FAILED_PASSWORD;
        }
        else if (!strcmp(result, LOGIN_FAILED_SERVER_FULL))
        {
            return RESULT_LOGIN_FAILED_SERVER_FULL;
        }
    }
    printf("\ndone logging in\n");
    return -1;
}

// when receiving the confirmation of room from a server, mark the room as ready
int receiveRoom(roomList *allRoom, int serverroomNum)
{
    int i = 0;
    for (i = 0; i < CHAT_ROOM_LIMIT; ++i)
    {
        if ((allRoom->roomList)[i].status == ROOM_UNALLOCATED)
        {
            (allRoom->roomList)[i].roomNum = serverroomNum;
            (allRoom->roomList)[i].status = ROOM_READY;
            break;
        }
    }

    if (i == CHAT_ROOM_LIMIT)
    {
        return NO_WAITING_ROOM;
    }
    else
    {
        return 0;
    }
}

int sendRoomInvite(int roomNum, char *userName, char *receiverName, fifo *outputBuffer)
{
    char packet[PACKAGE_SIZE];
    assembleCommand(roomNum, ROID, ROINVITE, userName, receiverName, packet);
    assert(writeBuffer(outputBuffer, packet)==0);
     return 0;
}

int sendFriendInvite(char *userName, char *receiverName, fifo *outputBuffer)
{
    char packet[PACKAGE_SIZE];
    assembleCommand(0, FRIENDID, FREQUEST, userName, receiverName, packet);
    assert(writeBuffer(outputBuffer, packet)==0);
    free(receiverName);
    return 0;
}

int acceptFriendInvite(char *userName, char *receiverName, fifo *outputBuffer)
{
    char packet[PACKAGE_SIZE];
    assembleCommand(0, FRIENDID, FRIEACCEPT, userName, receiverName, packet);
    assert(writeBuffer(outputBuffer, packet)==0);
    return 0;
}

int denyFriendInvite(char *userName, char *receiverName, fifo *outputBuffer)
{
    char packet[PACKAGE_SIZE];
    assembleCommand(0, FRIENDID, DEREQUEST, userName, receiverName, packet);
    assert(writeBuffer(outputBuffer, packet)==0);;
     return 0;
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

int joinInvitedRoom(roomList *allRoom, int roomNumber, char *userName, fifo *outputBuffer)
{
    char packet[PACKAGE_SIZE];

    if (receiveRoom(allRoom, roomNumber) >= 0)
    {
        chatRoom *tempRoom = retrieveRoom(allRoom, roomNumber);
        tempRoom->status = ROOM_TAKEN;
        assembleCommand(roomNumber, ROID, ROACCEPT, userName, NULL, packet);
        assert(writeBuffer(outputBuffer, packet)==0);
         return 0;
    }
    else
    {
        // TODO: Change error code
        return -1;
    }
}

int denyInvitedRoom(int roomNumber, char *userName, char *targetName, fifo *outputBuffer)
{
    char packet[PACKAGE_SIZE];
    assembleCommand(roomNumber, ROID, RODENY, userName, targetName, packet);
    assert(writeBuffer(outputBuffer, packet)==0);
     return 0;
}

int leaveRoom(chatRoom *room, char *userName, fifo *outputBuffer)
{
    char packet[PACKAGE_SIZE];
    closeBuffer(room->inMessage);
    room->inMessage = initBuffer(CLIENT_CHAT_ROOM_INTPUT_FIFO_MAX);
    room->status = ROOM_UNALLOCATED;
    room->memberChanged = 0;
    assembleCommand(room->roomNum, ROID, ROLEAVE, userName, NULL, packet);
    assert(writeBuffer(outputBuffer, packet)==0);
    room->totalPeople = 0;
    return 0;
}

int closeRoom(chatRoom *room, char *userName, fifo *outputBuffer)
{
    char packet[PACKAGE_SIZE];
    closeBuffer(room->inMessage);
    room->inMessage = initBuffer(CLIENT_CHAT_ROOM_INTPUT_FIFO_MAX);
    room->status = ROOM_UNALLOCATED;
    assembleCommand(room->roomNum, ROID, RODEL, userName, NULL, packet);
    assert(writeBuffer(outputBuffer, packet)==0);
    room->totalPeople = 0;
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
    if (writeBuffer(outputFIFO, tempPacket) == FIFO_FULL)
    {
        printf("\nthe fifo in sendMessage is full\n");
        return -1;
    }
    else
    {
        return 0;
    }
}
