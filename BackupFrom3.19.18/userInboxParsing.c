#include "userInboxParsing.h"
#include "tcpGUI.h"
#include "fifo.h"
#include "constants.h"
#include "protocol.h"
#include "protocol_const.h"
#include "tcpPacket.h"
#include "utils.h"
#include "userActions.h"
// GTK stuffs
#include <stdio.h>
#include <gtk/gtk.h>

#include "fifo.h"
#include "constants.h"
#include "protocol_const.h"
#include "tcpGUI.h"
#include "utils.h"
#include "protocol.h"

int handleRO(inboxInformationStruct *infoStruct)
{
    return 0;
}

int handleCO(inboxInformationStruct *infoStruct)
{

    char message[MESS_LIMIT];
    switch (infoStruct->comID)
    {
    case OPENCOM:
        // open communication
        getCommandSender( infoStruct->packet, message);
        if (!strcmp(LOGIN_SUCCESS, message))
        {
            strcat(infoStruct->message, "login success");
        }
        else if (!strcmp(LOGIN_FAILED_PASSWORD, message))
        {
            strcat(infoStruct->message, "login failed, wrong password");
        }

        else if (!strcmp(LOGIN_FAILED_SERVER_FULL, message))
        {
            strcat(infoStruct->message, "login server full");
        }

        // sign up stuffs
        if (!strcmp(SIGNUP_SUCCESS, message))
        {
            strcat(infoStruct->message, "signup success");
        }
        else if (!strcmp(SIGN_UP_FAILED_INVALID_NAME, message))
        {
            strcat(infoStruct->message, "signup failed, invalid name");
        }
        else if (!strcmp(SIGN_UP_FAILED_SERVER_FULL, message))
        {
            strcat(infoStruct->message, "signup failed, server full");
        }
        else if (!strcmp(SIGN_UP_FAILED_USER_EXISTED, message))
        {
            strcat(infoStruct->message, "signup failed, user name existed");
        }
        return 15;
        break;

    case GETFRIEND:
        // display list of user
        printf("\n%s is the packet\n", infoStruct->packet);
        printf("\nentering getfriend\n");
        getUserFriendList(infoStruct->allFriendList, infoStruct->packet, MAX_FRIENDS);
        printf("We finished the case GETFRIEND\n");
        break;

    case GETONLINEFRIEND:
        // display list of online friend
        getUserFriendList(infoStruct->friendList, infoStruct->packet, MAX_FRIENDS);
        printf("We finished the case GETONLINE FRIEND\n");
        break;
    }
    return 0;
}

int handleFR(inboxInformationStruct *infoStruct)
{
    (infoStruct->message)[0]='\0';
    switch (infoStruct->comID)
    {
    
    // both of these need linking with GTK
    case FRIEACCEPT:
    strcat(infoStruct->message, infoStruct->messageStruct->targetName);
    strcat(infoStruct->message, " accepted your friend request");
    popup_with_label(infoStruct->message);
        

        break;

    case DEREQUEST:
#ifdef DEBUG
        printf("\n%s denied your friend request\n", infoStruct->messageStruct->targetName);
#endif
        strcat(infoStruct->message, infoStruct->messageStruct->targetName);
        strcat(infoStruct->message, " denied your friend request");
        popup_with_label(infoStruct->message);

        break;

    case DEFRIEND:
        strcat(infoStruct->message, infoStruct->messageStruct->targetName);
        strcat(infoStruct->message, " deleted you as friend");
        popup_with_label(infoStruct->message);
        break;

    case FREQUEST:
        
        break;
    }
    return 0;
}

int handleROP(inboxInformationStruct *infoStruct)
{

    switch (infoStruct->comID)
    {

    case ROGRANTED:
#ifdef DEBUG
        printf("\nroom allocated\n");
#endif

        receiveRoom(infoStruct->roomList, infoStruct->roomNum);
        joinCreatedRoom(infoStruct->roomList, infoStruct->roomNum);
        break;

    case ROINVITED:
#ifdef DEBUG
        printf("\nyou are invited to room number %d\n", infoStruct->roomNum);
#endif

        // TODO: handle if this user ran out of room
        if (infoStruct->roomList->totalAllocatedRoom == CHAT_ROOM_LIMIT)
        {

#ifdef DEBUG
            printf("\nyou have reached maximum room limit\n");
#endif
            denyInvitedRoom(infoStruct->roomNum, infoStruct->userName, infoStruct->messageStruct->targetName, infoStruct->outputBuffer);

            strcat(infoStruct->message, "you have reached maximum room limit can't join more room");
            return 0;
        }

#ifdef DEBUG
        printf(" have invited to join, y/n? ");
#endif
        strcat(infoStruct->message, infoStruct->messageStruct->targetName);
        strcat(infoStruct->message, "have invited to join, y/n?");
        Popup(infoStruct->message, ROID, ROREQUEST, 0, infoStruct->messageStruct->targetName, infoStruct->messageStruct);
        break;

    case RODENIED:
#ifdef DEBUG
        printf("your invitation to has been denied");
#endif

        strcat(infoStruct->message, "your invitation to ");
        strcat(infoStruct->message, infoStruct->messageStruct->targetName);
        strcat(infoStruct->message, " has been denied");
        popup_with_label(infoStruct->message);
        break;

    case ROOMJOINDENIED:
        // TODO: integrate with Jason's code
        strcat(infoStruct->message, "your room invitation to ");
        strcat(infoStruct->message, infoStruct->messageStruct->targetName);
        strcat(infoStruct->message, " has been denied");
        popup_with_label(infoStruct->message);
        break;

    case ROSYNCED:
        getUserFriendList(infoStruct->friendList, infoStruct->packet, MAX_USER_PER_ROOM);
        updateRoomFriendList(retrieveRoom(infoStruct->roomList, infoStruct->roomNum), infoStruct->friendList, infoStruct->packet);
        break;
    }
    return 0;
}

int parseInboxCommand(inboxInformationStruct *infoStruct)
{

    char packet[PACKAGE_SIZE];
    infoStruct->packet=packet;

    if (readBuffer((infoStruct->inbox)->messageQueue, packet) == 0)
    {
        infoStruct->comID=getCommandID(packet);
        infoStruct->comType=getCommandType(packet);

        if(infoStruct->comType == FRIENDID)
        {
            if(infoStruct->comID==FREQUEST)
            {
                printf("\nthe target name is %s\n", infoStruct->messageStruct->targetName);
            Popup(infoStruct->message, FRIENDID, FREQUEST, 0, infoStruct->messageStruct->targetName, infoStruct->messageStruct);
            }
            else if(infoStruct->comID==FRIEACCEPT)
            {
                strcat(infoStruct->message, infoStruct->messageStruct->targetName);
                strcat(infoStruct->message, " accepted your friend request");
                popup_with_label(infoStruct->message);
            }
            return handleFR(infoStruct);
        }

        else if (infoStruct->comType == ROID_PASSIVE)
        {
            return handleROP(infoStruct);
        }
        else if (infoStruct->comType == COMID)
        {
            return handleCO(infoStruct);
        }
        else
        {
            return UNKNOWN_COMMAND_TYPE;
        }
    }
    // else
    // {
    //     return FIFO_NO_DATA;
    // }
return 0;    
}


inboxInformationStruct *createInboxInfoStruct(void)
{
    inboxInformationStruct *tempStruct = malloc(sizeof(inboxInformationStruct));
    tempStruct->message = malloc(sizeof(char) * MESS_LIMIT);
    tempStruct->friendList = createFriendList(MAX_USER_FRIEND);
    tempStruct->allFriendList = createFriendList(MAX_USER_FRIEND);

    return tempStruct;
}

int delInboxInfoStruct(inboxInformationStruct *infoStruct)
{
    free(infoStruct->friendList);
    free(infoStruct->friendList);
    free(infoStruct->message);
    free(infoStruct);
    return 0;
}

