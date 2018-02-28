#include "fifo.c"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace testing;

class FifoTest : public ::testing::Test
{
    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }
};

TEST_F(FifoTest, InitTest)
{
    uint8_t fifo_length[]={0, 5, 40, 70, 100};
    const uint8_t fifo_list_length=sizeof(fifo_length)/sizeof(uint8_t);

    for(int j=0; j< fifo_list_length; ++j)
    {
    fifo *buf=NULL;

    buf= initBuffer(fifo_length[j]);
    if(fifo_length[j]==0)
    {
        ASSERT_EQ(buf, NULL);
    }
    else
    {
        ASSERT_TRUE(buf);
        for(int i=0; i<fifo_length[j]; ++i)
        {
            ASSERT_EQ(buf[i],NULL);
        }
        ASSERT_EQ(buf->bufLength, fifo_length[j]);
        ASSERT_EQ(buf->readPos, 0);
        ASSERT_EQ(buf->writePos, 0);
        }
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
