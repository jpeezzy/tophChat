#include "fifo.c"
#include <gtest/gtest.h>

class FifoTest : public ::testing::Test
{
    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

TEST_F(FifoTest, InpuTest)
{
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
