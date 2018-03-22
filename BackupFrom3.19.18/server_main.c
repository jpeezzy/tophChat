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
#include <strings.h>
#include <sys/select.h>
#include <time.h>

// #include <pthread.h>

#include "constants.h"
#include "tcpPacket.h"
#include "server_back_end.h"
#include "protocol_const.h"
#include "protocol.h"
#include "fifo.h"
#include "tophChatUsers.h"
#include "triage.h"

int main(void)
{
    int incomingSocket = -1;
    struct sockaddr addrDummy;
    socklen_t socklenDummy;
    struct timeval timeout;
    bzero(&timeout, sizeof(timeout));
    timeout.tv_sec = 2;
    timeout.tv_usec = 5000;
    fd_set setListener;
    FD_ZERO(&setListener);

    int socketListener = listenSocketInit();
    assert(socketListener >= 0);
    char packet[PACKAGE_SIZE] = "";
    char message[MESS_LIMIT] = "";
    FD_SET(socketListener, &setListener);
    int j = 0;

    TINFO *dataBase = createTINFO();
    serverRoomList *roomList = serverRoomSetCreate();
    struct messageServerRoom *testRoom = serverRoomCreate(roomList);
    onlineUserList *userList = serverCreateOnlineList();

    TUSER *testFriend1;
    TUSER *testFriend2;
    char *userName[] = {"USER", "ADMIN"};

    // added a test user with many friends
    
    // testFriend2 = addUser("fr2", "fr2", "pass", dataBase);

    // addUser("fr3", "fr3", "213123", dataBase);
    // addUser("fr4", "fr4", "213123", dataBase);

    // addFriend("fr2", testFriend1, dataBase);
    // addFriend("fr3", testFriend1, dataBase);
    // addFriend("fr4", testFriend1, dataBase);

    
    // addFriend("fr6", testFriend2, dataBase);
    // addFriend("fr7", testFriend2, dataBase);
    // addFriend("fr8", testFriend2, dataBase);

    TUSER *userProfile0 = findUserByName(userName[0], dataBase);
    TUSER *userProfile1 = findUserByName(userName[1], dataBase);

    // bypass friend invitation
    // addFriend(userName[0], userProfile1, dataBase);

    // bypass room invitation
    char allfriendList0[MESS_LIMIT];
    char allfriendList1[MESS_LIMIT];

    char onlinefriendList0[MESS_LIMIT];
    char onlinefriendList1[MESS_LIMIT];


    char testLoginName[] = "Login Test";
    char passWd[MESS_LIMIT] = "";
    TUSER *testLoginUser = NULL;
    // limit to two users

    int prevTime = time(0);
    for (;;)
    {

        if (j < 2)
        {
            FD_ZERO(&setListener);
            FD_SET(socketListener, &setListener);

            if (select(socketListener + 1, &setListener, NULL, NULL, &timeout) > 0)
            {
                // wait until the client connect then save the socket number into the test friend account
                if (((incomingSocket = accept(socketListener, &addrDummy, &socklenDummy)) >= 0))
                {
#ifdef DEBUG
                    printf("\nconnection received the socket is %d\n", incomingSocket);
#endif
                    ++(userList->totalOnlineUser);
                    printf("\nadding user\n");
                    if(j==0)
                    {
                    testFriend1 = addUser("user1", "user1", "pass", dataBase);
                    testFriend1->socket = incomingSocket; // used to by pass login
                    testFriend1->isRegistered = 1;        // used to bypass registration
                    addUserToServerRoom(testRoom, "user1", dataBase);
                    printf("\n registering user 1\n");
                    assert(testFriend1);
                    ++j; //increase j to 1 to make sure we only accept one client
                    }
                    else if(j==1)
                    {
                    testFriend2 = addUser("user2", "user2", "pass", dataBase);
                    testFriend2->socket = incomingSocket; // used to by pass login
                    testFriend2->isRegistered = 1;        // used to bypass registration
                    addUserToServerRoom(testRoom, "user2", dataBase);
                    printf("\n registering user 2\n");
                    assert(testFriend2);
                    ++j; //increase j to 1 to make sure we only accept one client
                    }
                }
                else
                {
#ifdef DEBUG
                    printf("\nfailed to acceept socket\n");
#endif
                }
            }
        }

        // simple communication test area
        // loop permanently
        if (j == 2)
        {
            if (time(0) - prevTime > 2)
            {
                processTriagePacket(userList, roomList, dataBase);
                serverRoomSpreadMessage(testRoom, dataBase);
                prevTime = time(0);
            }
            // printf("\nentering package processing now\n");
        }

        // test two people Communication
        if (j == 3)
        {
            // #ifdef DEBUG
            // printf("\nreceived both\n");
            // fflush(stdout);
            // #endif
//             if (processTriagePacket(userList, roomList, dataBase) == 2)
//             {
// #ifdef DEBUG
//                 printf("\naccessing message\n");
// #endif
//                 readBuffer(testRoom->inMessage, packet);
//                 getMessageBody(packet, message);
//                 printf("received message: %s\n", message);
//                 serverRoomSpreadMessage(testRoom, dataBase);

//                 // updating user room friend list
//                 getRoomFriendList(testRoom, allfriendList0, userProfile0);
//                 getRoomFriendList(testRoom, allfriendList1, userProfile1);
//                 assembleCommand(0, COMID, GETONLINEFRIEND, "server", allfriendList0, packet);
//                 sendPacket(packet, userProfile0->socket);
//                 assembleCommand(0, COMID, GETONLINEFRIEND, "server", allfriendList1, packet);
//                 sendPacket(packet, userProfile1->socket);

//                 // updating user friend list
//                 getFriends(userProfile0->userName, dataBase, onlinefriendList0);
//                 assembleCommand(0, COMID, GETFRIEND, "server", onlinefriendList0, packet);
//                 sendPacket(packet, userProfile0->socket);
//                 getFriends(userProfile0->userName, dataBase, onlinefriendList1);
//                 assembleCommand(0, COMID, GETFRIEND, "server", onlinefriendList1, packet);
//                 sendPacket(packet, userProfile1->socket);
//             }
        }
    }
    return 0;
}