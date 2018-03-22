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
#include <time.h>

// #include <pthread.h>

#include "constants.h"
#include "tcpGUI.h"
#include "tcpPacket.h"
#include "protocol.h"
#include "protocol_const.h"
#include "userActions.h"
#include "userInboxParsing.h"

//test MACRO HERE
#define TEST_GET_FRIEND
#define ACTIVE_TEST


#define DEBUG
// #define TESTING_LOGIN_LOGOUT

int main(void)
{
// server and packet
#ifdef CLIENT_1
    char senderName[] = "USER";
#else
    char senderName[] = "ADMIN";
#endif
    serverConnection *server = openConnection();
    assert(server);
    char packet[PACKAGE_SIZE] = "";
    char message[MESS_LIMIT] = "";

    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    // constant var
    char *testPacketList[] = {"Test1", "Test2", "Test3", "Test4"};

    int testListLength = sizeof(testPacketList) / sizeof(testPacketList[0]);
    int totalPacketSent = 0;
    char sourceName[MAX_USER_NAME];
    int errorCode;
    int connectedEstablished = 0; // flag for if both client has joined the room

    fifo *outputBuffer = initBuffer(CLIENT_OUTPUT_FIFO_MAX);
    inboxQueue *inbox = initInboxQueue();

    // user Room stuffs
    roomList *userRoomList = roomsetInit();

    char **allFriendList = createFriendList(MAX_USER_FRIEND);
    char **onlineFriendList = createFriendList(MAX_USER_FRIEND);

    // initialize info struct for parsing
    inboxInformationStruct *infoStruct = createInboxInfoStruct();
    infoStruct->inbox = inbox;
    infoStruct->outputBuffer = outputBuffer;
    infoStruct->roomList = userRoomList;
    infoStruct->friendList = onlineFriendList;
    infoStruct->allFriendList = allFriendList;

    int prevTime = time(0);
    for (;;)
    {
        if (time(0) - prevTime > 2)
        {
            sendAllToServer(outputBuffer, server);


             
            if (connectedEstablished == 0)
            {
                // test function sent once here

                // get the list of all friends
                #ifdef ACTIVE_TEST
                #ifdef TEST_GET_FRIEND
                getAllFriend("fr1", outputBuffer);
                
                #endif

                #ifdef TEST_GET_ROOM
                requestRoom(userRoomList, outputBuffer, "fr1");               
                #endif
                
                #ifdef TEST_SEND_FRIEND_INVITE
                sendFriendInvite("fr1", "fr2", outputBuffer);
                #endif

                #ifdef TEST_SEND_ROOM_INVITE
                sendRoomInvite(0, "fr1", "fr2", outputBuffer);                
                #endif

                #ifdef TEST_GET_ALL_ONLINE_USER
                
                #endif

                #ifdef TEST_GET_ONLINE_FRIEND
                getAllOnlineFriend("fr1", outputBuffer);
                #endif
                #endif


                connectedEstablished = 1;
            }

            while ((errorCode = recvMessageFromServer(userRoomList, inbox, server)) > 0)
            {
                // sort type of packet
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

                // login failed
                else if (errorCode == ISCOMM)
                {

                    printf("\nparsing command\n");

                    if (parseInboxCommand(infoStruct) == 15)
                    {
                        printf("\ncommand received\n");
                    }
                }
            }

            // if ((totalPacketSent < testListLength) && connectedEstablished)
            // {
            //     assembleMessage(0, senderName, testPacketList[totalPacketSent], packet);
            //     sendPacket(packet, server->socket);
            //     ++totalPacketSent;
            // }
            prevTime = time(0);
        }
    }
    closeConnection(server);
    return 0;
}