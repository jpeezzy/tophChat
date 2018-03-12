
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string.h>

extern "C" {
#include "protocol_const.h"
#include "protocol.h"
#include "server_back_end.h"
#include "users/tophChatUsers.h"
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
    struct tophChatUser *testUser = findUserByName(userName, database);
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
    struct tophChatUser *testUser = findUserByName(userName, database);
    onlineUserList *userList = serverCreateOnlineList();
    onlineUser *testOnlUser;

    ASSERT_TRUE((testOnlUser = serverAddOnlineUser(userName, userList, 55, database)) != NULL);
    EXPECT_EQ(&(userList->userList[0]), testOnlUser);
    EXPECT_EQ(userList->userList[0].slot_status, ONLINE);
    EXPECT_EQ(userList->userList[0].userProfile, testUser);
    EXPECT_EQ(userList->userList[0].userProfile->socket, 55);

    serverLogOffUser(testOnlUser);
    EXPECT_EQ(testOnlUser->slot_status, NOT_ONLINE);
    EXPECT_EQ(testOnlUser->userProfile->socket, NOT_ONLINE);
    for (int i = 0; i < CHAT_ROOM_LIMIT; ++i)
    {
        EXPECT_EQ(testOnlUser->userProfile->listOfRooms[i], -1);
    }
    EXPECT_EQ(testOnlUser->userProfile->numOfRoomUserIn, 0);

    char buffer[100];
    EXPECT_GE(recv(testOnlUser->userProfile->socket, buffer, 100, 0), 0);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}