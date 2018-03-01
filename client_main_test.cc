#include "client_main.c"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(simpleReceive, IO)
{
    
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}