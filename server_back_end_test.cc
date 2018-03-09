
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string.h>

extern "C" {
#include "protocol_const.h"
#include "protocol.h"
#include "server_back_end.h"
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}