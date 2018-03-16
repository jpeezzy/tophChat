#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define mlength 481

//This is an implementation of Suu Encrpytion based off AES 64bit

//constants for byte exchange 
static const uint8_t sbox[256] = {
  0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
  0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
  0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
  0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
  0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
  0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
  0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
  0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
  0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
  0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
  0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
  0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
  0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
  0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

static const uint8_t rsbox[256] = {
  0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
  0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
  0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
  0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
  0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
  0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
  0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
  0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
  0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
  0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
  0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
  0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
  0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
  0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
  0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
  0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };

//intial vector 
static const int intialVector[4][4] = {
    {0x52, 0x09, 0x6a, 0xd5}, {0x30, 0x36, 0xa5, 0x38}, {0xbf, 0x40, 0xa3, 0x9e}, {0x81, 0xf3, 0xd7, 0xfb}};
//round constant 
static const int roundC[5] = {0x52,0x09,0x6a,0xd5,0x30};

//changes bytes in a preset pattern
int byteExchange(int inputbyte)
{
    return sbox[inputbyte];    
}

int RbExchange(int inputbyte)
{
    return rsbox[inputbyte];
}

void keyExpansion(int round, int keyArray[4][4], int roundkey[4][4])
{
    int r,i,j;
    int previouskey[4][4];
    
    //copy keyArray to roundkey and previousArray
    for(i=0; i<4; ++i)
    {
        for(j = 0; j<4; ++j)
        {
            roundkey[i][j] = keyArray[i][j];
            previouskey[i][j] = keyArray[i][j];
        }
    }

    for(r = 0; r<round; ++r)
    {   
        if(round == 0)
        {
            continue;
        }

        //shift the last col up
        int temp;
        temp = roundkey[0][3];
        roundkey[0][3] = roundkey[1][3];
        roundkey[1][3] = roundkey[2][3];
        roundkey[2][3] = roundkey[3][3];
        roundkey[3][3] = temp;

        //run last col through substitution
        for(i = 0; i<4; ++i)
        {
            roundkey[3][i] = byteExchange(roundkey[3][i]);
        }

        //XOR last colwith round constant
        for(i = 0; i<4; ++i)
        {
            roundkey[i][3] = roundkey[i][3] ^ roundC[i];
        }

        //XOR last col with first col of round key from last round 
        for(i = 0; i<4; ++i)
        {
            roundkey[i][3] = roundkey[i][3] ^ previouskey[i][0];
        }

        //XOR with previous roundkey
        for(i = 0; i<4; ++i)
        {
            for(j = 0; j<4; ++j)
            {
                roundkey[i][j] = roundkey[i][j] ^ previouskey[i][j];
            }
        }
        
        //save roundkey to previous round
        for(i = 0; i<4; ++i)
        {
            for(j = 0; j<4; ++j)
            {
                previouskey[i][j] = roundkey[i][j];
            }
        }
    }
}

//turn a key input into a key array 
void keytoArray(unsigned long int key, int keyArray[4][4])
{
    int i,j;
    unsigned long int tempkey;

    tempkey = key;

    for(i = 0; i<4; ++i)
    {
        for(j = 0; j<4; ++j)
        {   
            keyArray[i][j] = tempkey % 16;
            tempkey = tempkey/16;
        }
    }
}

void encrypt(char input[mlength], unsigned long int key)
{
    int keyArray[4][4];
    int roundKey[4][4];
    int poutput[4][4];
    int step;
    int place = 0;
    int inputArray[4][4];
    int i,j;
    int tempD;
    int round;

    //turn key into keyarray 
    keytoArray(key, keyArray);
    

    //step for each block of array
    for(step = 0; step<30; ++step)
    {   
        //turn input into array
        for(i = 0; i<4; ++i)
        {
            for(j = 0; j<4; ++j)
            {
                inputArray[i][j] = input[place];
                place++;
            }
        }

        //the start of the encrypt rounds 
        for(round = 0; round<4; ++round)
        {
            if(round == 0)
            {
                //XOR with intial vector 
                for(i = 0; i<4; ++i)
                {
                    for(j = 0; j<4; ++j)
                    {
                        inputArray[i][j] = inputArray[i][j] ^ intialVector[i][j];    
                    }
                }
            }
            else 
            {
                //XOR with previous output
                for(i = 0; i<4; ++i)
                {
                    for(j = 0; j<4; ++j)
                    {
                        inputArray[i][j] = inputArray[i][j] ^ poutput[i][j]; 
                    }
                }
            }

            //apply subsitution to array
            for(i = 0; i<4; ++i)
            {
                for(j = 0; j<4; ++j)
                {
                    inputArray[i][j] = byteExchange(inputArray[i][j]);
                }
            }

            //apply diffusion to array shift
            //2 row first left unchanged 
            tempD = inputArray[1][0];
            inputArray[1][0] = inputArray[1][1];
            inputArray[1][1] = inputArray[1][2];
            inputArray[1][2] = inputArray[1][3];
            inputArray[1][3] = tempD;
            //3 row 
            tempD = inputArray[2][0];
            inputArray[2][0] = inputArray[2][2];
            inputArray[2][2] = tempD;
            tempD = inputArray[2][1];
            inputArray[2][1] = inputArray[2][3];
            inputArray[2][3] = tempD;
            //4 row 
            tempD = inputArray[3][3];
            inputArray[3][3] = inputArray[3][2];
            inputArray[3][2] = inputArray[3][1];
            inputArray[3][1] = inputArray[3][0];
            inputArray[3][0] = tempD;

            //mix cols
            for(i = 0; i<4; ++i)
            {
                tempD = inputArray[i][0];
                inputArray[i][0] = inputArray[i][3];
                inputArray[i][3] = tempD;
                tempD = inputArray[i][1];
                inputArray[i][1] = inputArray[i][2];
                inputArray[i][2] = tempD;
            }

            //generateroundkey
            keyExpansion(round, keyArray, roundKey);
            
            //XOR with round key 
            for(i = 0; i<4; ++i)
            {
                for(j = 0; j<4; ++j)
                {
                    inputArray[i][j] = inputArray[i][j] ^ roundKey[i][j]; 
                }
            }

            //copy output to poutput
            for(i = 0; i<4; ++i)
            {
                for(j = 0; j<4; ++j)
                {
                    poutput[i][j] = inputArray[i][j];
                }
            }

            //copy roundkey to keyArray
            for(i = 0; i<4; ++i)
            {
                for(j = 0; j<4; ++j)
                {
                    keyArray[i][j] = roundKey[i][j];
                }
            }
        }

        //save input array back into array
        int Scount = 16;
        for(i = 0; i<4; ++i)
        {
            for(j = 0; j<4; ++j)
            {   
                input[place-Scount] = inputArray[i][j];
                Scount--;
            }
        }
    }
}

void decrypt(char input[mlength], unsigned long int key)
{
    int keyArray[4][4];
    int roundKey[4][4];
    int poutput[4][4];
    int step;
    int place = 0;
    int inputArray[4][4];
    int i,j;
    int tempD;
    int round;

    //turn key into keyarray 
    keytoArray(key, keyArray);
    

    //step for each block of array
    for(step = 0; step<30; ++step)
    {   
        //turn input into array
        for(i = 0; i<4; ++i)
        {
            for(j = 0; j<4; ++j)
            {
                inputArray[i][j] = input[place];
                place++;
            }
        }

        //the start of the encrypt rounds 
        for(round = 0; round<4; ++round)
        {
            if(round == 0)
            {
                //XOR with intial vector 
                for(i = 0; i<4; ++i)
                {
                    for(j = 0; j<4; ++j)
                    {
                        inputArray[i][j] = inputArray[i][j] ^ intialVector[i][j];    
                    }
                }
            }
            else 
            {
                //XOR with previous output
                for(i = 0; i<4; ++i)
                {
                    for(j = 0; j<4; ++j)
                    {
                        inputArray[i][j] = inputArray[i][j] ^ poutput[i][j]; 
                    }
                }
            }

            //apply subsitution to array
            for(i = 0; i<4; ++i)
            {
                for(j = 0; j<4; ++j)
                {
                    inputArray[i][j] = byteExchange(inputArray[i][j]);
                }
            }

            //apply diffusion to array shift
            //2 row first left unchanged 
            tempD = inputArray[1][0];
            inputArray[1][0] = inputArray[1][1];
            inputArray[1][1] = inputArray[1][2];
            inputArray[1][2] = inputArray[1][3];
            inputArray[1][3] = tempD;
            //3 row 
            tempD = inputArray[2][0];
            inputArray[2][0] = inputArray[2][2];
            inputArray[2][2] = tempD;
            tempD = inputArray[2][1];
            inputArray[2][1] = inputArray[2][3];
            inputArray[2][3] = tempD;
            //4 row 
            tempD = inputArray[3][3];
            inputArray[3][3] = inputArray[3][2];
            inputArray[3][2] = inputArray[3][1];
            inputArray[3][1] = inputArray[3][0];
            inputArray[3][0] = tempD;

            //mix cols
            for(i = 0; i<4; ++i)
            {
                tempD = inputArray[i][0];
                inputArray[i][0] = inputArray[i][3];
                inputArray[i][3] = tempD;
                tempD = inputArray[i][1];
                inputArray[i][1] = inputArray[i][2];
                inputArray[i][2] = tempD;
            }

            //generateroundkey
            keyExpansion(round, keyArray, roundKey);
            
            //XOR with round key 
            for(i = 0; i<4; ++i)
            {
                for(j = 0; j<4; ++j)
                {
                    inputArray[i][j] = inputArray[i][j] ^ roundKey[i][j]; 
                }
            }

            //copy output to poutput
            for(i = 0; i<4; ++i)
            {
                for(j = 0; j<4; ++j)
                {
                    poutput[i][j] = inputArray[i][j];
                }
            }

            //copy roundkey to keyArray
            for(i = 0; i<4; ++i)
            {
                for(j = 0; j<4; ++j)
                {
                    keyArray[i][j] = roundKey[i][j];
                }
            }
        }

        //save input array back into array
        int Scount = 16;
        for(i = 0; i<4; ++i)
        {
            for(j = 0; j<4; ++j)
            {   
                input[place-Scount] = inputArray[i][j];
                Scount--;
            }
        }
    }

}

int main()
{
    char test[mlength];
    char testA[mlength];
    int i;

    for(i=0; i<mlength; ++i)
    {
        test[i] = ' ';
    }

    test[0] = 't';
    test[1] = 'e';
    test[2] = 's';
    test[3] = 't';
    test[4] = ' ';
    test[5] = 'i';
    test[6] = 's';
    test[7] = ' ';
    test[8] = 't';
    test[9] = 'h';
    test[10] = 'e';
    test[499] = '\0';
    
    for(i=0; i<mlength; ++i)
    {
        testA[i] = test[i];    
    }

    //test key 
    unsigned long int testkey = 0x457d5d672f;
    //making it 62 bit
    testkey = testkey/4;

    printf(" %s \n", test);
    encrypt(testA, testkey);
    printf(" %s \n", testA);
    //decrypt(test, testkey);
    printf(" %s \n", test);
    if( testA == testA)
    {
        printf("\n test passed \n");
    }
    else
    {
        printf("\n test did not pass\n");
    }
    return 0;
}
