
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string.h>

extern "C" {
#include "protocol_const.h"
#include "protocol.h"
#include "server_back_end.h"
#include "tophChatUsers.h"
#include "socket.h"
}

TEST(ServerRoomSetCreate, SERVER_ROOM_TEST)
{
    allServerRoom *allRoom = serverRoomSetCreate();
    EXPECT_EQ(allRoom->firstFreeRoom, 0);
    EXPECT_EQ(allRoom->totalRoom, 0);
    for (int i = 0; i < MAX_SERVER_CHATROOMS; ++i)
    {
        EXPECT_EQ(allRoom->roomList[i].isOccupied, ROOM_NOT_OCCUPIED);
        EXPECT_EQ(allRoom->roomList[i].peopleNum, 0);
        EXPECT_EQ(allRoom->roomList[i].roomNum, i);
        EXPECT_TRUE(allRoom->roomList[i].inMessage != NULL);
        for (int j = 0; j < MAX_USER_PER_ROOM; ++j)
        {
            EXPECT_EQ(allRoom->roomList[i].socketList[j], -1);
            EXPECT_TRUE(allRoom->roomList[i].userList[j] == NULL);
        }
    }
}

TEST(ServerRoomCreate, SERVER_ROOM_TEST)
{
    allServerRoom *allRoom = serverRoomSetCreate();
    struct messageServerRoom *testRoom;
    EXPECT_TRUE((testRoom = serverRoomCreate(allRoom)) == &(allRoom->roomList[0]));
    EXPECT_TRUE(allRoom->roomList[0].isOccupied == ROOM_BUSY);

    EXPECT_TRUE(serverRoomCreate(allRoom) == &(allRoom->roomList[1]));
    EXPECT_TRUE(allRoom->roomList[0].isOccupied == ROOM_BUSY);

    serverRoomReturn(testRoom);
    EXPECT_EQ(allRoom->roomList[0].isOccupied, ROOM_NOT_OCCUPIED);
    EXPECT_EQ(allRoom->roomList[0].peopleNum, 0);
    for (int i = 0; i < MAX_USER_PER_ROOM; ++i)
    {
        EXPECT_TRUE(allRoom->roomList[0].userList[i] == NULL);
        EXPECT_TRUE(allRoom->roomList[0].socketList[i] == -1);
    }
}

// TODO: test edge cases
TEST(SeverRoomUser, SERVER_ROOM_TEST)
{
    TINFO *database = createTINFO();
    allServerRoom *allRoom = serverRoomSetCreate();
    serverChatRoom *testRoom = &(allRoom->roomList[10]);
    char userName[] = "ADMIN";
    addUser(userName, userName, 1231123, database);
    struct tophChatUser *testUser = findUserByName(userName, database);

    testUser->socket = 1; // give user the online status
    addUserToServerRoom(testRoom, userName, database);
    EXPECT_EQ(testRoom->socketList[0], testUser->socket);
    EXPECT_EQ(testUser->numOfRoomUserIn, 1);
    EXPECT_EQ(testUser->listOfRooms[0], 10);
    EXPECT_EQ(testRoom->peopleNum, 1);

    removeUserFromServerRoom(testRoom, userName, database);
    EXPECT_EQ(testRoom->socketList[0], -1);
    EXPECT_EQ(testUser->numOfRoomUserIn, 0);
    EXPECT_EQ(testUser->listOfRooms[0], -1);
    EXPECT_EQ(testRoom->peopleNum, 0);
}

TEST(ServerRoomSend, SERVER_ROOM_TEST)
{
    // test setup
    char packet[PACKAGE_SIZE];
    TINFO *database = createTINFO();
    allServerRoom *allRoom = serverRoomSetCreate();
    serverChatRoom *testRoom = &(allRoom->roomList[10]);

    char *userName[] = {"ADMIN", "USER1", "USER2", "USER3", "USER4"};

    int userNameListLength = sizeof(userName) / sizeof(userName[0]);
    struct tophChatUser **userList = (struct tophChatUser **)malloc(sizeof(struct tophChatUser *) * userNameListLength);

    for (int i = 0; i < userNameListLength; ++i)
    {
        addUser(userName[i], userName[i], i, database);
        userList[i] = findUserByName(userName[i], database);
        userList[i]->socket = i;
        addUserToServerRoom(testRoom, userName[i], database);
    }

    assembleMessage(10, userName[0], "TEST1", packet);
    sendServerRoomMessage(testRoom, packet);
    serverRoomSpreadMessage(testRoom, database);
}

TEST(ServerRoomInvite, SERVER_ROOM_TEST)
{
    // test setup
    char packet[PACKAGE_SIZE];
    TINFO *database = createTINFO();
    allServerRoom *allRoom = serverRoomSetCreate();
    serverChatRoom *testRoom = &(allRoom->roomList[10]);

    char *userName[] = {"ADMIN", "USER1", "USER2", "USER3", "USER4"};

    int userNameListLength = sizeof(userName) / sizeof(userName[0]);
    struct tophChatUser **userList = (struct tophChatUser **)malloc(sizeof(struct tophChatUser *) * userNameListLength);

    for (int i = 0; i < userNameListLength; ++i)
    {
        addUser(userName[i], userName[i], i, database);
        userList[i] = findUserByName(userName[i], database);
        userList[i]->socket = i;
        addUserToServerRoom(testRoom, userName[i], database);
    }

    sendRoomInvite(userName[0], userName[1], testRoom, database);
}

TEST(OnlineListGenerate, onlineList)
{
    onlineUserList *userList = serverCreateOnlineList();
    ASSERT_TRUE(userList != NULL);
    EXPECT_EQ(userList->totalOnlineUser, 0);
    for (int i = 0; i < MAX_SERVER_USERS; ++i)
    {
        EXPECT_EQ(userList->userList[i].slot_status, NOT_ONLINE);
    }
    serverDelOnlineList(userList);
}

// TODO: test edge cases
TEST(AddRemoveUser, onlineList)
{
    TINFO *database = createTINFO();
    allServerRoom *allRoom = serverRoomSetCreate();
    char userName[] = "ADMIN";
    addUser(userName, userName, 12312131, database);
    struct tophChatUser *testUser = findUserByName(userName, database);
    onlineUserList *userList = serverCreateOnlineList();
    onlineUser *testOnlUser;

    ASSERT_TRUE((testOnlUser = serverAddOnlineUser(userName, userList, 55, database)) != NULL);
    EXPECT_EQ(&(userList->userList[0]), testOnlUser);
    EXPECT_EQ(userList->userList[0].slot_status, ONLINE);
    EXPECT_EQ(userList->userList[0].userProfile, testUser);
    EXPECT_EQ(userList->userList[0].userProfile->socket, 55);
    EXPECT_EQ(userList->userList[0].userProfile->numOfRoomUserIn, 0);

    serverLogOffUser(testOnlUser);
    EXPECT_EQ(testOnlUser->slot_status, NOT_ONLINE);
    EXPECT_EQ(testOnlUser->userProfile->socket, NOT_ONLINE);
    for (int i = 0; i < CHAT_ROOM_LIMIT; ++i)
    {
        EXPECT_EQ(testOnlUser->userProfile->listOfRooms[i], -1);
    }
    EXPECT_EQ(testOnlUser->userProfile->numOfRoomUserIn, 0);

    // test adding many users
    {
        char packet[PACKAGE_SIZE];
        TINFO *database = createTINFO();
        allServerRoom *allRoom = serverRoomSetCreate();
        serverChatRoom *testRoom = &(allRoom->roomList[10]);

        char *userName[] = {"ADMIN", "USER1", "USER2", "USER3", "USER4"};

        int userNameListLength = sizeof(userName) / sizeof(userName[0]);
        struct tophChatUser **userList = (struct tophChatUser **)malloc(sizeof(struct tophChatUser *) * userNameListLength);
        onlineUserList *onlineList = serverCreateOnlineList();

        for (int i = 0; i < userNameListLength; ++i)
        {
            addUser(userName[i], userName[i], i + 20000, database);
            userList[i] = findUserByName(userName[i], database);
            ASSERT_TRUE(serverAddOnlineUser(userName[i], onlineList, i, database) != NULL);
            ASSERT_EQ(addUserToServerRoom(testRoom, userName[i], database), 0);
        }

        for (int i = 0; i < userNameListLength; ++i)
        {
            EXPECT_EQ(onlineList->userList[i].slot_status, ONLINE);
            EXPECT_EQ(onlineList->userList[i].userProfile->socket, i);
            EXPECT_EQ(0, strcmp(onlineList->userList[i].userProfile->userName, userName[i]));
            EXPECT_EQ(onlineList->userList[i].userProfile->numOfRoomUserIn, 1);
        }
    }

    char buffer[100];
    EXPECT_LE(recv(testOnlUser->userProfile->socket, buffer, 100, 0), 0);
}

TEST(getOnlineUSERTEST, onlineList)
{
    // test setup
    char packet[PACKAGE_SIZE];
    TINFO *database = createTINFO();
    allServerRoom *allRoom = serverRoomSetCreate();
    serverChatRoom *testRoom = &(allRoom->roomList[10]);

    char *userName[] = {"ADMIN", "USER1", "USER2", "USER3", "USER4"};

    int userNameListLength = sizeof(userName) / sizeof(userName[0]);
    struct tophChatUser **userList = (struct tophChatUser **)malloc(sizeof(struct tophChatUser *) * userNameListLength);

    for (int i = 0; i < userNameListLength; ++i)
    {
        addUser(userName[i], userName[i], i, database);
        userList[i] = findUserByName(userName[i], database);
        addUserToServerRoom(testRoom, userName[i], database);
    }

    onlineUserList *onlineList = serverCreateOnlineList();
    for (int i = 0; i < userNameListLength; ++i)
    {
        ASSERT_TRUE(serverAddOnlineUser(userName[i], onlineList, i, database) != NULL);
    }

    char onlineListMessage[PACKAGE_SIZE];
    char correct_onlineListMessage[PACKAGE_SIZE] = "";
    int strIndex = 0;
    getOnlineUser(onlineList, onlineListMessage);

    for (int i = 0; i < userNameListLength; ++i)
    {
        strcat(correct_onlineListMessage, userName[i]);
        strIndex += strlen(userName[i]);
        correct_onlineListMessage[strIndex] = '/';
        correct_onlineListMessage[strIndex + 1] = '\0';
    }
    EXPECT_EQ(0, strcmp(correct_onlineListMessage, onlineListMessage));
    printf("\nthe list of online user is %s\n", onlineListMessage);
    printf("the correct online list is %s\n", correct_onlineListMessage);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}