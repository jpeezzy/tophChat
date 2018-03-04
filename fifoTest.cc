extern "C"
{
#include "fifo.h"
}
#include "constants.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string.h>

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
    uint8_t fifo_length[]={5, 40, 70, 100};
    const uint8_t fifo_list_length=sizeof(fifo_length)/sizeof(fifo_length[0]);

    for(int j=0; j< fifo_list_length; ++j)
    {
    fifo *buf=nullptr;
    buf= initBuffer(fifo_length[j]);
    
    ASSERT_TRUE(buf);
    for(int i=0; i<fifo_length[j]; ++i)
    {
        ASSERT_EQ((buf->buffer)[i], nullptr);
    }
    ASSERT_EQ(buf->bufLength, fifo_length[j]);
    ASSERT_EQ(buf->readPos, 0);
    ASSERT_EQ(buf->writePos, 0);
    closeBuffer(buf);
    }
}

TEST_F(FifoTest, writeTest)
{
    fifo *buf=nullptr;
    const char* buf_test_data[]={"This is nice", "Hi John, this is My", "WELL, HELLO THERE", 
    "kdtrinh1@uci.edu", "249367897124hfsdakj892173!@($&#@*&$"};
    const uint8_t fifo_list_length=sizeof(buf_test_data)/sizeof(buf_test_data[0]);
    buf= initBuffer(fifo_list_length);

    for(int i=0; i<fifo_list_length; ++i)
    {
        writeBuffer(buf, buf_test_data[i], strlen(buf_test_data[i]));
    }
    ASSERT_EQ(writeBuffer(buf, buf_test_data[0], (strlen(buf_test_data[0])+1)*sizeof(char)), FIFO_FULL);
    
    for(int i=0; i<fifo_list_length; ++i)
    {
        ASSERT_EQ(0, strcmp(buf_test_data[i], buf->buffer[i]));
        printf("\n%s", buf->buffer[i]);
    }

    printf("\n");
    closeBuffer(buf);    
}

TEST_F(FifoTest, readTest)
{
    fifo *buf=nullptr;
    char* buf_test_data[] = {"This is nice", "Hi John, this is My", "WELL, HELLO THERE", 
    "kdtrinh1@uci.edu", "249367897124hfsdakj892173!@($&#@*&$"};
    const uint8_t fifo_list_length = sizeof(buf_test_data)/sizeof(buf_test_data[0]);
    char** buf_test_data_temp = (char**)malloc(fifo_list_length*sizeof(char*));
    char temp[MESS_LIMIT];

    for(int i=0; i<fifo_list_length; ++i)
    {
        buf_test_data_temp[i]=(char*)malloc((strlen(buf_test_data[i])+1)*sizeof(char));
        strcpy(buf_test_data_temp[i], buf_test_data[i]);
    }
    buf= initBuffer(fifo_list_length);
    ASSERT_EQ(FIFO_NO_DATA, readBuffer(buf, temp));

    for(int i=0; i<fifo_list_length; ++i)
    {
        buf->buffer[i]=buf_test_data_temp[i];
    }

    for(int i=0; i<fifo_list_length; ++i)
    {
        readBuffer(buf, temp);
        ASSERT_EQ(0, strcmp(buf_test_data[i], temp));
        printf("\n%s", buf_test_data[i]);
    }
    ASSERT_EQ(FIFO_NO_DATA, readBuffer(buf, temp));
    printf("\n");
    closeBuffer(buf);
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}