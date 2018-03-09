#include <gtest/gtest.h>
#include <gmock/gmock.h>

extern "C"
{
    #include "utils.h"
    #include <string.h> 
}

TEST(maxTest, maxFunc)
{
    EXPECT_EQ(max(5,2),5);
    EXPECT_EQ(max(5,5),5);
    EXPECT_EQ(max(-5,2),2);
    EXPECT_EQ(max(1000,1001),1001);
}

TEST(slicerBeging, stringSlicerTest)
{
    char testString[]="This is a reall long string";
    char result[50];
    
    stringSlicer(testString, result, 0, 4);
    EXPECT_EQ(0, strcmp(result,"This "));

    stringSlicer(testString, result, 0, 8);
    EXPECT_EQ(0, strcmp(result,"This is a"));

    stringSlicer(testString, result, 0, 12);
    EXPECT_EQ(0, strcmp(result,"This is a rea"));

    stringSlicer(testString, result, 0, 2);
    EXPECT_EQ(0, strcmp(result,"Thi"));
}

TEST(slicerMid, stringSlicerTest)
{
    char testString[]="This is a reall long string";
    char result[50];
    
    stringSlicer(testString, result, 2, 4);
    EXPECT_EQ(0, strcmp(result,"is "));

    stringSlicer(testString, result, 3, 8);
    EXPECT_EQ(0, strcmp(result,"s is a"));

    stringSlicer(testString, result, 1, 12);
    EXPECT_EQ(0, strcmp(result,"his is a rea"));

    stringSlicer(testString, result, 12, 16);
    EXPECT_EQ(0, strcmp(result,"all l"));
}

TEST(slicerEnd, stringSlicerTest)
{
    char testString[]="This is a reall long string";
    char result[50];
    
    stringSlicer(testString, result, 18, 27);
    EXPECT_EQ(0, strcmp(result,"ng string"));
    printf("%s\n", result);

    stringSlicer(testString, result, 23, 27);
    EXPECT_EQ(0, strcmp(result,"ring"));

    stringSlicer(testString, result, 1, 27);
    EXPECT_EQ(0, strcmp(result,"his is a reall long string"));

    stringSlicer(testString, result, 12, 27);
    EXPECT_EQ(0, strcmp(result,"all long string"));
}

TEST(charInt, characterToInteger)
{
    EXPECT_EQ(charToInt('9'), 9);
    EXPECT_EQ(charToInt('1'), 1);
    EXPECT_EQ(charToInt('0'), 0);
    EXPECT_EQ(charToInt('4'), 4);
}

TEST(IntChar, IntToChar)
{
    EXPECT_EQ(intToChar(9),'9');
    EXPECT_EQ(intToChar(8),'8');
    EXPECT_EQ(intToChar(7),'7');
    EXPECT_EQ(intToChar(6),'6');
    EXPECT_EQ(intToChar(5),'5');
    EXPECT_EQ(intToChar(4),'4');
}

TEST(uniqueRandgen, UniqueGen)
{
    int randList[1000];
    uniqueRandGen(10000, randList, 1000);
    for(int i=0; i<1000;++i)
    {
        for(int j=0; j<i; ++j)
        {
            ASSERT_TRUE(randList[j]!=randList[i]);
        }
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
