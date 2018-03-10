
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string.h>

extern "C" {
#include "protocol_const.h"
#include "protocol.h"
}

TEST(getRoom, protocolTest)
{
    char testPacket1[MESS_LIMIT] = "1001TPM{}Hdhfkjewhfhdskjfadsh";
    char testPacket2[MESS_LIMIT] = "6571TPM{}Hwejhfkjdshfuiqwe";
    char testPacket3[MESS_LIMIT] = "8921TPM{}Hgfadhjgvnekjbgjke";

    char testPacket4[MESS_LIMIT] = "2131TPC{}HR1";
    char testPacket5[MESS_LIMIT] = "3211TPC{}HF3";
    char testPacket6[MESS_LIMIT] = "3411TPC{}HC2";
    char testPacket7[MESS_LIMIT] = "3411TPC{}HR3";

    ASSERT_EQ(getroomNumber(testPacket1), 100);
    ASSERT_EQ(getroomNumber(testPacket2), 657);
    ASSERT_EQ(getroomNumber(testPacket3), 892);

    ASSERT_EQ(getroomNumber(testPacket4), 213);
    ASSERT_EQ(getroomNumber(testPacket5), 321);
    ASSERT_EQ(getroomNumber(testPacket6), 341);
    ASSERT_EQ(getroomNumber(testPacket7), 341);
}

TEST(getType, protocolTest)
{
    char testPacket1[MESS_LIMIT] = "1001TPM{}Hdhfkjewhfhdskjfadsh";
    char testPacket2[MESS_LIMIT] = "6571TPM{}Hwejhfkjdshfuiqwe";
    char testPacket3[MESS_LIMIT] = "8921TPM{}Hgfadhjgvnekjbgjke";

    char testPacket4[MESS_LIMIT] = "2131TPC{}HR1";
    char testPacket5[MESS_LIMIT] = "3211TPC{}HF3";
    char testPacket6[MESS_LIMIT] = "3411TPC{}HC2";
    char testPacket7[MESS_LIMIT] = "3411TPC{}HR3";

    char testPacket8[MESS_LIMIT] = "daskfjklqwjfmdsnkjewh";
    char testPacket9[MESS_LIMIT] = "aslkjdlasjdlkasjd";
    ASSERT_EQ(getpacketType(testPacket1), ISMESSAGE);
    ASSERT_EQ(getpacketType(testPacket2), ISMESSAGE);
    ASSERT_EQ(getpacketType(testPacket3), ISMESSAGE);

    ASSERT_EQ(getpacketType(testPacket4), ISCOMM);
    ASSERT_EQ(getpacketType(testPacket5), ISCOMM);
    ASSERT_EQ(getpacketType(testPacket6), ISCOMM);
    ASSERT_EQ(getpacketType(testPacket7), ISCOMM);

    ASSERT_EQ(getpacketType(testPacket8), -1);
    ASSERT_EQ(getpacketType(testPacket9), -1);
}

TEST(getMessageBody, protocolTest)
{
    char testPacket1[MESS_LIMIT] = "1001TPM{}HdhfkjewhfhdskjfadshghMESSAGE_HERE";
    char testPacket2[MESS_LIMIT] = "6571TPM{}HwejhfkjdshfuiqwesdsddUTHdsalk!";
    char testPacket3[MESS_LIMIT] = "8921TPM{}Hgfadhjgvnekjbgjkeds23!)#*@!ldsj";

    char result[100];

    getMessageBody(testPacket1, result);
    ASSERT_EQ(0, strcmp(result, "MESSAGE_HERE"));
    getMessageBody(testPacket2, result);
    ASSERT_EQ(0, strcmp(result, "UTHdsalk!"));
    getMessageBody(testPacket3, result);
    ASSERT_EQ(0, strcmp(result, "!)#*@!ldsj"));
}
TEST(getComID, protocolTest)
{
    char testPacket4[MESS_LIMIT] = "2131TPC{}HR1";
    char testPacket5[MESS_LIMIT] = "3211TPC{}HF3";
    char testPacket6[MESS_LIMIT] = "3411TPC{}HC2";
    char testPacket7[MESS_LIMIT] = "3411TPC{}HR3";

    ASSERT_EQ(getCommandID(testPacket4), 1);
    ASSERT_EQ(getCommandID(testPacket5), 3);
    ASSERT_EQ(getCommandID(testPacket6), 2);
    ASSERT_EQ(getCommandID(testPacket7), 3);
}

TEST(getComType, protocolTest)
{
    char testPacket4[MESS_LIMIT] = "2131TPC{}HR1";
    char testPacket5[MESS_LIMIT] = "3211TPC{}HF3";
    char testPacket6[MESS_LIMIT] = "3411TPC{}HC2";
    char testPacket7[MESS_LIMIT] = "3411TPC{}HR3";

    ASSERT_EQ(getCommandType(testPacket4), 'R');
    ASSERT_EQ(getCommandType(testPacket5), 'F');
    ASSERT_EQ(getCommandType(testPacket6), 'C');
    ASSERT_EQ(getCommandType(testPacket7), 'R');
}

TEST(getUSerName, getUSERNAme)
{
    char *testPacket[] = {"2131TPC{}HUserName",
                          "3211TPC{}Hdkm12344",
                          "3411TPC{}H!f;lwldsal",
                          "3411TPC{}H98789kshhejastyehasnd"};

    char *userNameList[] = {"UserName", "dkm12344", "!f;lwldsal", "98789kshhejastyehasnd"};
    char userNameOutput[MAX_USER_NAME + 1];
    int listSize = sizeof(testPacket) / sizeof(testPacket[0]);
    for (int i = 0; i < listSize; ++i)
    {
        getUserName(userNameOutput, testPacket[i]);
        ASSERT_EQ(0, strcmp(userNameOutput, userNameList[i]));
    }
}

TEST(AssembleCommand, AssembleTest)
{
    char testPacket[PACKAGE_SIZE];
    char userName[] = "12345678912345678912";
    char senderName[] assembleCommand(111, ROID, RODEL, NULL, testPacket);
    EXPECT_EQ(strcmp("1111TPC{}HR2", testPacket), 0);
    printf("\n%s\n", testPacket);

    assembleCommand(10, COMID, CLOSECOM, "Test Command", testPacket);
    EXPECT_EQ(strcmp("0101TPC{}HC1Test Command", testPacket), 0);
    printf("\n%s\n", testPacket);

    assembleCommand(931, FRIENDID, DEFRIEND, NULL, testPacket);
    EXPECT_EQ(strcmp("9311TPC{}HF3", testPacket), 0);
    printf("\n%s\n", testPacket);

    assembleCommand(312, ROID, RODEL, "!JFLAHS", testPacket);
    EXPECT_EQ(strcmp("3121TPC{}HR2!JFLAHS", testPacket), 0);
    printf("\n%s\n", testPacket);
}

TEST(AssembleMessage, AssembleTest)
{
    char testPacket[PACKAGE_SIZE] = "";
    char messageBody[MESS_LIMIT] = "";
    char userName[] = "12345678912345678912";
    char outputUserName[30];
    char *testMessage[] = {"This is nice", "That's not right", "fjldshvjdsnkjwehg;ke", "9283741892uioUoi@fds", "dfsfdsq1@!##$!",
                           "fjdslfjew", "sjdklfjdslkgndsv,mnklwje", "123451fdsfdsa", "@!!@}{FL:ASL<><AD", "#PHFGJSBH(*@!P(*", "fdshjfjjdslkfjwdsjlnvkls", "2", "3", "~#@!3ewdflkna;", "nc,mxvns/.,/", "1", "2", "546", "142423", "fi2jwrwe", "12412fdsa", "bcnxz., flkawej", "/,/.3,12/lkaf", "fsdlj912pfa", "`13124ujrklj"};
    int testMessageNum = sizeof(testMessage) / sizeof(char *);

    for (int i = 0; i < testMessageNum; ++i)
    {
        assembleMessage(i, userName, testMessage[i], testPacket);
        getMessageBody(testPacket, messageBody);
        ASSERT_EQ(strcmp(messageBody, testMessage[i]), 0);
        ASSERT_EQ(getroomNumber(testPacket), i);
        ASSERT_EQ(getpacketType(testPacket), ISMESSAGE);
        getSenderName(outputUserName, testPacket);
        ASSERT_EQ(strcmp(outputUserName, userName), 0);
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}