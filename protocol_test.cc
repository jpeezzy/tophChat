
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

TEST(getCommandSEnder, protocolTest)
{
    char testPacket4[MESS_LIMIT] = "2131TPC{}HR1UserName/dssdsada";
    char testPacket5[MESS_LIMIT] = "3211TPC{}HF3dkm12344/dasdas";
    char testPacket6[MESS_LIMIT] = "3411TPC{}HC2!f;lwldsal/asdasdads";
    char testPacket7[MESS_LIMIT] = "3411TPC{}HR398789kshhejastyehasnd/asdadsa";

    char *userNameList[] = {"UserName", "dkm12344", "!f;lwldsal", "98789kshhejastyehasnd"};
    char commandSender[100] = "";

    getCommandSender(testPacket4, commandSender);
    ASSERT_EQ(0, strcmp(userNameList[0], commandSender));

    getCommandSender(testPacket5, commandSender);
    ASSERT_EQ(0, strcmp(userNameList[1], commandSender));

    getCommandSender(testPacket6, commandSender);
    ASSERT_EQ(0, strcmp(userNameList[2], commandSender));

    getCommandSender(testPacket7, commandSender);
    ASSERT_EQ(0, strcmp(userNameList[3], commandSender));
}

TEST(getCommandTARget, protocolTest)
{
    char testPacket4[MESS_LIMIT] = "2131TPC{}HR1UserName/dssdsada";
    char testPacket5[MESS_LIMIT] = "3211TPC{}HF3dkm12344/dasdas";
    char testPacket6[MESS_LIMIT] = "3411TPC{}HC2!f;lwldsal/asdasdads";
    char testPacket7[MESS_LIMIT] = "3411TPC{}HR398789kshhejastyehasnd/asdadsa";

    char *userNameList[] = {"dssdsada", "dasdas", "asdasdads", "asdadsa"};
    char commandTarget[100] = "";

    getCommandTarget(testPacket4, commandTarget);
    ASSERT_EQ(0, strcmp(userNameList[0], commandTarget));

    getCommandTarget(testPacket5, commandTarget);
    ASSERT_EQ(0, strcmp(userNameList[1], commandTarget));

    getCommandTarget(testPacket6, commandTarget);
    ASSERT_EQ(0, strcmp(userNameList[2], commandTarget));

    getCommandTarget(testPacket7, commandTarget);
    ASSERT_EQ(0, strcmp(userNameList[3], commandTarget));
}

TEST(getSenderNAme, getUSERNAme)
{
    char *testPacket[] = {"2131TPC{}HUserName/ljfdshldsf",
                          "3211TPC{}Hdkm12344/fsljsljflsd",
                          "3411TPC{}H!f;lwldsal/fdsjlfjsd",
                          "3411TPC{}H98789kshhejastyehasnd/fldslfsdjl"};

    char *userNameList[] = {"UserName", "dkm12344", "!f;lwldsal", "98789kshhejastyehasnd"};
    char userNameOutput[MAX_USER_NAME + 1];
    int listSize = sizeof(testPacket) / sizeof(testPacket[0]);
    for (int i = 0; i < listSize; ++i)
    {
        getSenderName(userNameOutput, testPacket[i]);
        ASSERT_EQ(0, strcmp(userNameOutput, userNameList[i]));
    }
}

TEST(getMessageBody, protocolTest)
{
    char testPacket1[MESS_LIMIT] = "1001TPM{}HUserName/dhfkjewhfhdskjfadshghMESSAGE_HERE";
    char testPacket2[MESS_LIMIT] = "6571TPM{}Hdkm12344/wejhfkjdshfuiqwesdsddUTHdsalk!";
    char testPacket3[MESS_LIMIT] = "8921TPM{}H!f;lwldsal/gfadhjgvnekjbgjkeds23!)#*@!ldsj";

    char result[100];

    getMessageBody(testPacket1, result);
    ASSERT_EQ(0, strcmp(result, "dhfkjewhfhdskjfadshghMESSAGE_HERE"));
    getMessageBody(testPacket2, result);
    ASSERT_EQ(0, strcmp(result, "wejhfkjdshfuiqwesdsddUTHdsalk!"));
    getMessageBody(testPacket3, result);
    ASSERT_EQ(0, strcmp(result, "gfadhjgvnekjbgjkeds23!)#*@!ldsj"));
}

TEST(AssembleCommand, AssembleTest)
{
    char testPacket[PACKAGE_SIZE];
    assembleCommand(111, ROID, RODEL, "dkm1234", "ghsdfsl", testPacket);
    EXPECT_EQ(strcmp("1111TPC{}HR2dkm1234/ghsdfsl", testPacket), 0);
    printf("\n%s\n", testPacket);

    assembleCommand(10, COMID, CLOSECOM, "user_name1", "rewjjdfsl!@#", testPacket);
    EXPECT_EQ(strcmp("0101TPC{}HC1user_name1/rewjjdfsl!@#", testPacket), 0);
    printf("\n%s\n", testPacket);

    assembleCommand(931, FRIENDID, DEFRIEND, "fhskjfds", NULL, testPacket);
    EXPECT_EQ(strcmp("9311TPC{}HF3fhskjfds/1", testPacket), 0);
    printf("\n%s\n", testPacket);

    assembleCommand(312, ROID, RODEL, "!JFLAHS", "!@#\dasjl", testPacket);
    EXPECT_EQ(strcmp("3121TPC{}HR2!JFLAHS/!@#\dasjl", testPacket), 0);
    printf("\n%s\n", testPacket);
}

TEST(AssembleMessage, AssembleTest)
{
    char testPacket[PACKAGE_SIZE] = "";
    char messageBody[MESS_LIMIT] = "";
    char *userName[] = {"123456789123\78912",
                        "!@RFKLASJKLFA",
                        "1932ryhfkjdsnkjds",
                        "fskddvjh2iuy3ir",
                        "fu12y7iyfuas",
                        "fjlh2u3yr8jkl!)&*",
                        "fuyu12hrfljsdhnj",
                        "12849754379fh",
                        "treyhsgdfkjh2397",
                        "237yfusdh1231"};
    char outputUserName[30];
    char *testMessage[] = {"This is nice", "That's not right", "fjldshvjdsnkjwehg;ke", "9283741892uioUoi@fds", "dfsfdsq1@!##$!",
                           "fjdslfjew", "sjdklfjdslkgndsv,mnklwje", "123451fdsfdsa", "@!!@}{FL:ASL<><AD", "#PHFGJSBH(*@!P(*"};
    int testMessageNum = sizeof(testMessage) / sizeof(char *);

    for (int i = 0; i < testMessageNum; ++i)
    {
        assembleMessage(i, userName[i], testMessage[i], testPacket);
        getMessageBody(testPacket, messageBody);
        EXPECT_EQ(strcmp(messageBody, testMessage[i]), 0);
        EXPECT_EQ(getroomNumber(testPacket), i);
        EXPECT_EQ(getpacketType(testPacket), ISMESSAGE);
        getSenderName(outputUserName, testPacket);
        EXPECT_EQ(strcmp(outputUserName, userName[i]), 0);
        printf("\n%s\n", testPacket);
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}