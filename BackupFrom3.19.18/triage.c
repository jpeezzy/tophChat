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
#include <strings.h>    //bzero
#include <sys/select.h> //io multiplexing
#include <errno.h>

#include "server_back_end.h"
#include "constants.h"
#include "tcpPacket.h"
#include "tophChatUsers.h"
#include "protocol_const.h"
#include "protocol.h"
#include "utils.h"
#include "triage.h"


//takes in a packet and process the packet
int processTriagePacket(onlineUserList *userList, struct allServerRoom *allRoom, TINFO *dataBase)
{
    assert(userList);
    assert(allRoom);
    assert(dataBase);

    triagePacket *Tripacket;
    Tripacket = malloc(sizeof(triagePacket));
    Tripacket->errorflag = 0;

    //NEEDS to BE DONE maybe
    //char sentPacket[PACKAGE_SIZE] = "";
    //char packet[PACKAGE_SIZE] = "";

    int userIndex = 0;

    for (userIndex = 0; userIndex < MAX_SERVER_USERS; ++userIndex)
    {
        if (dataBase->Users[userIndex] != NULL)
        {
            if (dataBase->Users[userIndex]->socket >= 0)
            {
                if (fetchPacket(Tripacket->packet, dataBase->Users[userIndex]->socket) == PACKAGE_SIZE * sizeof(char))
                {
                    printf("\nreceived packet%s\n", Tripacket->packet);
                    packageProcess(Tripacket, userList, allRoom, dataBase);
                }
            }
        }
    }

    // printf("\ndone triaging\n");
    int tempflag = Tripacket->errorflag;
    //free(Tripacket);
    return tempflag;
}

//if a packet is recieved process it correctly
void packageProcess(triagePacket *tpacket, onlineUserList *userList, struct allServerRoom *allRoom, TINFO *dataBase)
{
#ifdef DEBUG
    // printf("\nreceived packet, processing\n");
#endif
    if (getpacketType(tpacket->packet) == ISMESSAGE)
    {
        messageProcess(allRoom, tpacket, dataBase);
    }
    else if (getpacketType(tpacket->packet) == ISCOMM)
    {
        commandProcess(tpacket, userList, allRoom, dataBase);
    }
#ifdef DEBUG
    else
    {
        printf("\nreceived unknown packet\n");
    }
#endif
}

//if package is a message
void messageProcess( struct allServerRoom *allRoom, triagePacket *tpacket, TINFO *dataBase)
{
#ifdef DEBUG
    printf("\nreceived user message\n");
#endif
    getSenderName(tpacket->senderName, tpacket->packet);
    
    assert((tpacket->tempUser = findUserByName(tpacket->senderName, dataBase))!=NULL);
    sendServerRoomMessage(findServerRoomByNumber(allRoom, getroomNumber(tpacket->packet)), tpacket->packet);
    printf("\nthe packet is %s\n", tpacket->packet);
    serverRoomSpreadMessage(findServerRoomByNumber(allRoom, getroomNumber(tpacket->packet)), dataBase);
#ifdef DEBUG
    printf("\nsent user message to server room\n");
#endif
    tpacket->errorflag = 2;
    return;
}

//if packet is a command
void commandProcess(triagePacket *tpacket, onlineUserList *userList, struct allServerRoom *allRoom, TINFO *dataBase)
{
#ifdef DEBUG
    printf("\nreceived command\n");
#endif
    getCommandSender(tpacket->packet,tpacket->senderName);
    tpacket->receiverName[0]='\0';

    if(strcmp("user1", tpacket->senderName)==0)
    {
        strcpy(tpacket->receiverName, "user2");
    }
    else 
    {
        strcpy(tpacket->receiverName, "user1");
    }

    int roomNum = getroomNumber(tpacket->packet);

    if (getCommandType(tpacket->packet) == FRIENDID)
    {
        processFriendID(tpacket, dataBase);
    }
    else if (getCommandType(tpacket->packet) == ROID)
    {
        processRoomID(tpacket, allRoom, dataBase, roomNum);
    }
    else if (getCommandType(tpacket->packet) == COMID)
    {
        processCommandID(tpacket, userList, allRoom, dataBase);
    }
    else
    {
#ifdef DEBUG
        printf("\nreceived unknown command\n");
#endif
        tpacket->errorflag = UNKNOWN_COMMAND_TYPE;
        return;
    }
}

//if command is friend id
void processFriendID(triagePacket *tpacket, TINFO *dataBase)
{
    // getCommandTarget(tpacket->packet, tpacket->receiverName);
    if(strcmp("user1", tpacket->senderName)==0)
    {
        strcpy(tpacket->receiverName, "user2");
    }
    else 
    {
        strcpy(tpacket->receiverName, "user1");
    }
    
    tpacket->tempUser = findUserByName(tpacket->senderName, dataBase);
    tpacket->tempUser2 = findUserByName(tpacket->receiverName, dataBase);
    switch (getCommandID(tpacket->packet))
    {

    case FREQUEST:
        // TODO: make friend request offline too
        printf("\nreceived friend request from %s to %s\n", tpacket->senderName, tpacket->receiverName);
        if ((tpacket->tempUser = findUserByName(tpacket->receiverName, dataBase)) != NULL)
        {

            assembleCommand(1, FRIENDID, FREQUEST, tpacket->senderName, tpacket->receiverName, tpacket->sentPacket);
            if (tpacket->tempUser->socket > -1)
            {
                sendPacket(tpacket->sentPacket, tpacket->tempUser2->socket);
            }
        }
        else
        {
            tpacket->errorflag = -1;
            return;
        }
        break;

    case DEREQUEST:
        sendPacket(tpacket->packet, tpacket->tempUser2->socket);
        break;

    case FRIEACCEPT:
    tpacket->tempUser = findUserByName(tpacket->senderName, dataBase);
    tpacket->tempUser2 = findUserByName(tpacket->receiverName, dataBase);
    printf("\ndoing friend accept\n");
        // TOOD: added error checking
        addFriend(tpacket->tempUser2->userName, tpacket->tempUser, dataBase);
        printf("\nsending accept packet from %s to %s \n", tpacket->tempUser->userName, tpacket->tempUser2->userName);
        sendPacket(tpacket->packet, tpacket->tempUser2->socket);
        break;

    case DEFRIEND:
        deleteFriend(tpacket->tempUser, tpacket->receiverName);
        sendPacket(tpacket->packet, tpacket->tempUser2->socket);
        break;
    }
}

//if command is room id
void processRoomID(triagePacket *tpacket, struct allServerRoom *allRoom, TINFO *dataBase, int roomNum)
{
    switch (getCommandID(tpacket->packet))
    {
    case ROCREATE:
        createRoom(tpacket, allRoom, dataBase, roomNum);
        break;

    case RODEL:
        serverRoomReturn(findServerRoomByNumber(allRoom, getroomNumber(tpacket->packet)));
        break;

    case ROINVITE:
        roomInvite(tpacket, allRoom, dataBase, roomNum);
        break;

    case ROACCEPT:
        acceptRoom(tpacket, dataBase, roomNum);
        break;

    case RODENY:
        assembleCommand(roomNum, ROID_PASSIVE, ROOMJOINDENIED, tpacket->senderName, tpacket->receiverName, tpacket->sentPacket);
        sendPacket(tpacket->sentPacket, tpacket->tempUser2->socket);
        break;

        // sync user list of online server with offline room
    case ROSYNC:
        //getRoomFriendList(findServerRoomByNumber(allRoom, roomNum), tpacket->message);
        assembleCommand(roomNum, ROID_PASSIVE, ROSYNCED, "server", tpacket->message, tpacket->sentPacket);
        sendPacket(tpacket->sentPacket, tpacket->tempUser->socket);
        break;
    }
}

void createRoom(triagePacket *tpacket, struct allServerRoom *allRoom, TINFO *dataBase, int roomNum)
{
    char sentPacket[PACKAGE_SIZE] = "";
    serverChatRoom *onlineRoom;
    getCommandSender( tpacket->packet, tpacket->senderName);
    if(strcmp("user1", tpacket->senderName)==0)
    {
        strcpy(tpacket->receiverName, "user2");
    }
    else 
    {
        strcpy(tpacket->receiverName, "user1");
    }
    tpacket->tempUser = findUserByName(tpacket->senderName, dataBase);
#ifdef DEBUG
    printf("\nreceived room create request from %s\n", tpacket->senderName);
#endif
    if ((tpacket->tempUser = findUserByName(tpacket->senderName, dataBase)) != NULL)
    {
        if (tpacket->tempUser->numOfRoomUserIn == CHAT_ROOM_LIMIT || allRoom->totalRoom == MAX_SERVER_CHATROOMS)
        {
            assembleCommand(0, ROID_PASSIVE, RODENIED, tpacket->senderName, NULL, sentPacket);
        }

        else
        {
#ifdef DEBUG
            printf("\nallocating room to %s\n", tpacket->senderName);
#endif
            if((onlineRoom = serverRoomCreate(allRoom))!=NULL)
            {
            onlineRoom->isOccupied = ROOM_BUSY;
            addUserToServerRoom(onlineRoom, tpacket->senderName, dataBase);
            assembleCommand(onlineRoom->roomNum, ROID_PASSIVE, ROGRANTED, tpacket->senderName, NULL, sentPacket);
            }
            else
            {
                // room full
                assembleCommand(onlineRoom->roomNum, ROID_PASSIVE, RODENIED, tpacket->senderName, NULL, sentPacket);
            } 
        }

#ifdef DEBUG
        printf("\nsending approval for room number %d create to %s\n", onlineRoom->roomNum, tpacket->senderName);
#endif
        sendPacket(sentPacket, tpacket->tempUser->socket);
#ifdef DEBUG
        printf("\nfinished sending approval for room number %d create to %s\n", onlineRoom->roomNum, tpacket->senderName);
#endif
    }
    else
    {
        printf("\nuser doesn't exist in createRoom\n");
    }
}

void roomInvite(triagePacket *tpacket, struct allServerRoom *allRoom, TINFO *dataBase, int roomNum)
{
#ifdef DEBUG
    printf("\nreceived room invite\n");
#endif
    tpacket->tempUser = findUserByName(tpacket->senderName, dataBase);
    tpacket->tempUser2 = findUserByName(tpacket->receiverName, dataBase);
    tpacket->onlineRoom = findServerRoomByNumber(allRoom, roomNum);

    if (!checkIfFriends(tpacket->tempUser, tpacket->tempUser2))
    {

        assembleCommand(roomNum, ROID_PASSIVE, ROOMJOINDENIED, tpacket->senderName, NULL, tpacket->sentPacket);
        sendPacket(tpacket->sentPacket, tpacket->tempUser->socket);
    }
    else
    {

#ifdef DEBUG
        printf("\nboth users are friends\n");
#endif
        if (tpacket->onlineRoom->isOccupied == ROOM_NOT_OCCUPIED)
        {
#ifdef DEBUG
            printf("\nroom not allocated\n");
#endif
            assembleCommand(roomNum, ROID_PASSIVE, ROOMJOINDENIED, tpacket->senderName, NULL, tpacket->sentPacket);
            sendPacket(tpacket->sentPacket, tpacket->tempUser->socket);
        }
        else
        {
#ifdef DEBUG
            printf("\nforwarding invite\n");
#endif
            // forward packet to the other user
            assembleCommand(roomNum, ROID_PASSIVE, ROINVITED, tpacket->senderName, tpacket->receiverName, tpacket->sentPacket);
            sendPacket(tpacket->sentPacket, tpacket->tempUser2->socket);
        }
    }
}

void acceptRoom(triagePacket *tpacket, TINFO *dataBase, int roomNum)
{
#ifdef DEBUG
    printf("\nreceived room accept\n");
#endif
    if (tpacket->onlineRoom->isOccupied == ROOM_NOT_OCCUPIED)
    {
        tpacket->errorflag = -1;
        return;
    }
    else
    {
        addUserToServerRoom(tpacket->onlineRoom, tpacket->senderName, dataBase);
        assembleCommand(roomNum, ROID_PASSIVE, ROOMJOINACCEPTED, tpacket->senderName, tpacket->receiverName, tpacket->sentPacket);
        sendPacket(tpacket->sentPacket, tpacket->tempUser2->socket);
    }
}

//if command is command id
void processCommandID(triagePacket *tpacket, onlineUserList *userList, struct allServerRoom *allRoom, TINFO *dataBase)
{
    switch (getCommandID(tpacket->packet))
    {
        getCommandSender( tpacket->packet, tpacket->senderName);
        char friendList[MESS_LIMIT] = "";
        char friendOnlineList[MESS_LIMIT] = "";
        onlineUser *user;
        int index = 0;
    case OPENCOM:

    case CLOSECOM:

        for (index = 0; index < MAX_SERVER_CHATROOMS; ++index)
        {
            if (!(strcmp(((userList->userList)[index].userProfile->userName), tpacket->senderName)))
            {
                break;
            }
        }

        if (index == MAX_SERVER_USERS)
        {
            tpacket->errorflag = -1;
        }
        else
        {
            user = &((userList->userList)[index]);
            serverLogOffUser(user, dataBase, allRoom);
        }

        break;

    case SIGNUP:
        getCommandSender( tpacket->packet, tpacket->senderName);
        serverSignUpOnlineUser(tpacket->senderName, userList, dataBase, tpacket->packet);
        tpacket->errorflag == SIGNUP_COMPLETED;
        printf("\nexiting signup switch\n");
        break;
    case LOGIN:
        printf("\nsender name is %s\n", tpacket->senderName);
        getCommandSender(tpacket->packet, tpacket->senderName);
        assert(tpacket->senderName);
        serverLoginOnlineUser(tpacket->senderName, userList, dataBase, tpacket->packet);
        printf("\nlogging user in\n");
        tpacket->errorflag = LOGIN_COMPLETETED;
        break;

    case GETONLINEPEOPLE:
        getCommandSender( tpacket->packet, tpacket->senderName);
        tpacket->tempUser = findUserByName(tpacket->senderName, dataBase);
        char onlineList[MESS_LIMIT];
        getOnlineUser(userList, onlineList);
        assembleCommand(0, COMID, GETONLINEFRIEND, tpacket->senderName, onlineList, tpacket->packet);
        sendPacket(tpacket->packet, tpacket->tempUser->socket);
        break;

    case GETFRIEND:

        // printf("\nentering server get friend\n");
        tpacket->tempUser = findUserByName(tpacket->senderName, dataBase);
        friendOnlineList[0] = '\0';
        getFriends(tpacket->senderName, dataBase, friendOnlineList);

        if(friendOnlineList[0]=='\0')
        {
            // printf("\nuser has no friend\n");
        }
        else
        {
        // printf("\nthe list of friend for %s is %s on server getfriend\n", tpacket->senderName, friendOnlineList);
        // printf("\nthe packet received is %s\n", tpacket->packet);
        assembleCommand(0, COMID, GETFRIEND, tpacket->senderName, friendOnlineList, tpacket->packet);
        // printf("\nthe socket of the user is %d\n", tpacket->tempUser->socket);
        // printf("\nthe packet sent out is %s\n", tpacket->packet);
        sendPacket(tpacket->packet, tpacket->tempUser->socket);
        }
        break;

    case GETONLINEFRIEND:
        tpacket->roomNum = getroomNumber(tpacket->packet);
        tpacket->tempUser = findUserByName(tpacket->senderName, dataBase);
        getOnlineFriends(tpacket->senderName, dataBase, friendOnlineList);
        assembleCommand(0, COMID, GETONLINEFRIEND, tpacket->senderName, friendOnlineList, tpacket->packet);
        sendPacket(tpacket->packet, tpacket->tempUser->socket);
        break;

        /*
        case GETONLINEUSER:
            tpacket->tempUser = findUserByName(tpacket->senderName, dataBase);
            getOnlineUser(userList, tpacket->message);
            assembleCommand(111, COMID, GETONLINEUSER, "server", tpacket->message, tpacket->sentPacket);
            sendPacket(tpacket->sentPacket, tpacket->tempUser->socket);
            break;
        */
        /*
        case SIGNUP:
            break;*/
    }
}
