#include <sys/socket.h>
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

// #include <pthread.h>

#include "constants.h"
#include "tcpGUI.h"
#include "tcpPacket.h"
#include "protocol.h"
#include "protocol_const.h"
#include "userActions.h"

#define DEBUG

int main(void)
{
    // server and packet
    serverConnection *server = openConnection();
    assert(server);
    char packet[PACKAGE_SIZE] = "";
    char message[MESS_LIMIT] = "";

    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    // constant var
    char *testPacketList[] = {"Test1", "Test2", "Test3", "Test4"};

#ifdef CLIENT_1
    char senderName[] = "USER";
#else
    char senderName[] = "ADMIN";
#endif
    int testListLength = sizeof(testPacketList) / sizeof(testPacketList[0]);
    int totalPacketSent = 0;
    char sourceName[MAX_USER_NAME];
    int errorCode;
    int connectedEstablished = 0; // flag for if both client has joined the room

    fifo *outputBuffer = initBuffer(CLIENT_OUTPUT_FIFO_MAX);
    inboxQueue *inbox = initInboxQueue();

    // user Room stuffs
    roomList *userRoomList = roomsetInit();

#ifdef CLIENT_1
    printf("\nrequesting room\n");
    int requestRoomIndex = requestRoom(userRoomList, outputBuffer, senderName);
    assert(sendAllToServer(outputBuffer, server)>0);
    printf("\nfinished sending request\n");
    while(1)
    {
        if((recvMessageFromServer(userRoomList, inbox, server)==ISCOMM))
        {
        parseInboxCommand(inbox, userRoomList,  outputBuffer, senderName, server);
        break;
        }
    }
    int i=0;
for (i = 0; i < CHAT_ROOM_LIMIT; ++i)
    {
        if ((userRoomList->roomList)[i].status== ROOM_TAKEN)
        {
            sendInvite((userRoomList->roomList)[i].roomNum, senderName, "ADMIN", outputBuffer);
        }
    }
     
    printf("\nsending room request\n");
    assert(sendAllToServer(outputBuffer, server)>0);
#endif

   
    for (;;)
    {
        while ((errorCode = recvMessageFromServer(userRoomList, inbox, server)) > 0)
        {
            if (errorCode == ISMESSAGE)
            {
                for (int i = 0; i < CHAT_ROOM_LIMIT; ++i)
                {
                    if (userRoomList->roomList[i].status == ROOM_TAKEN)
                    {
                        readBuffer(userRoomList->roomList[i].inMessage, packet);
                        getMessageBody(packet, message);
                        printf("\nThe message is %s and from %s\n", message, sourceName);
                    }
                }
            }
            else if (errorCode == ISCOMM)
            {
                printf("\nparsing command\n");
                if(parseInboxCommand(inbox, userRoomList,  outputBuffer, senderName, server)==ACCEPTED_ROOM)
                {
                    printf("\nconnection established\n");
                    connectedEstablished=1;
                }

            }
        }

        if ((totalPacketSent < testListLength) && connectedEstablished)
        {
            assembleMessage(0, senderName, testPacketList[totalPacketSent], packet);
            sendPacket(packet, server->socket);
            ++totalPacketSent;
        }
    }
    closeConnection(server);
    return 0;
}