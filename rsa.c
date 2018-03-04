/* rsa.c: rsa encryption 
 * Author: Aung Thu
 * references: https://www.di-mgt.com.au/rsa_alg.html
               https://en.wikipedia.org/wiki/RSA_(cryptosystem)*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "rsa.h"
#include "randGen.h"

#define PUBLIC_e_VALUE 65537
#define RSA_BITS 62 
#define PRIME_ACCURACY 5

RSA_PRIV *createRSA_priv(unsigned long n, unsigned long d)
{
    RSA_PRIV *rsaPriv = NULL;
    rsaPriv = malloc(sizeof(RSA_PRIV));
    if(rsaPriv == NULL)
    {
        perror("Out of memory aborting...");
        exit(10);
    }
    rsaPriv->n = n;
    rsaPriv->d = d;
    return rsaPriv;
}

RSA_PUB *createRSA_pub(unsigned long n, unsigned long e)
{
    RSA_PUB *rsaPub = NULL;
    rsaPub = malloc(sizeof(RSA_PUB));
    if(rsaPub == NULL)
    {
        perror("Out of memory aborting...");
        exit(10);
    }
    rsaPub->n = n;
    rsaPub->e = e;
    return rsaPub;
}

void deleteRSA_priv(RSA_PRIV *rsaPriv)
{
    assert(rsaPriv);
    free(rsaPriv);
}

void deleteRSA_pub(RSA_PUB *rsaPub)
{
    assert(rsaPub);
    free(rsaPub);
}

/* generate a 1024 bit prime number p for rsa encryption 
 * where p must satisfy the condition (p mod e) != 1 */
unsigned long generateRSA_prime(unsigned long e)
{
    unsigned long num32, rand32, prime;
    int test, n;
    
    n = RSA_BITS/2;
    num32 = power(2, n);
    
    /*generate a random 32 bit integer */
    rand32 = rand() % (num32 + 1);
    rand32 = rand32 + num32;
    #ifdef DEBUG
        printf("\nrand32 = %lu\n", rand32);
    #endif

    /* if randNum is even, make it odd */
    if(rand32 % 2 == 0)
    {
        rand32--;
    }

    /* find the next prime larger than randNum */
    prime = rand32;
    test = 0;
    while(test == 0)
    {
        while(prime % e == 1)
        {
            prime += 2;       
        }
        test = testPrime(prime);
        if(!test)
        {
            prime += 2;
        }
    }
    #ifdef DEBUG
        printf("Found a prime number!\n");
    #endif
    return prime;         
}

/* using Rabin-Miller primality test */
int testPrime(unsigned long p)
{
    int i,r;
    unsigned long d;
    /* base case */
    if (p == 2 || p == 3)
    {
        return 1;
    }
    /* return false if p is even */
    if (p % 2 == 0)
    {
        return 0;
    }
    
    /* find d such that n-1 can be written as d*(2^r) */
    d = p - 1;
    r = 0;
    while(d % 2 != 1)
    {
        d /= 2;
        r++;
    } 

    for(i = 0; i < PRIME_ACCURACY; i++)
    {
        /* return false if Rabin-Miller test fails */
        if (millerTest(p, d) == 0)
        {
            return 0;
        }        
    }
    return 1;
}

int millerTest(unsigned long p, unsigned long d)
{
    unsigned long a, x;
    /* random number a in range [2, p-2] */
    a = rand() % (p - 3) + 2;
    x = mod_exp(a, d, p);
    
    while (d != p - 1 && x != 1 && x != p - 1)
    {
        x = mod_mul(x, x, p);
        d *= 2;
    }
    if (x != p - 1 && d % 2 == 0)
    {
        return 0;
    }
    return 1;
}

/* power calculated using recursive fast exponentiation method */
unsigned long power(unsigned long x, unsigned long n)
{
    /* base case */
    if (n == 0)
    {
        return 1;
    }
    else if (n == 1)
    {
        return x;
    }
    /* n is even */
    else if (n % 2 == 0)
    {
        return power(x * x, n / 2);
    }
    /* n is odd */
    else if (n % 2 == 1)
    {
        return x * power(x * x, (n - 1) / 2);
    }
    return 0;
}

/* calculate b^e mod n */
unsigned long mod_exp(unsigned long b, unsigned long e, unsigned long n)
{
    unsigned long x = 1;
    while (e > 0)
    {
        /* if e is odd */
        if (e % 2 == 1)
        {
            x = mod_mul(x, b, n);;
        }
        b = mod_mul(b, b, n);
        e = e / 2;
    }
    return x % n;
}

/* calculates (x * y) mod n */
unsigned long mod_mul(unsigned long x, unsigned long y, unsigned long n)
{
    unsigned long a, b;
    a = 0;
    b = x % n;
    while (y > 0)
    {
        if (y % 2 == 1)
        {
            a = (a + b) % n;
        }
        b = (b * 2) % n;
        y /= 2;
    }    
    return a % n;
}
 
unsigned long ext_gcd(unsigned long a, unsigned long b, long *x, long *y)
{
    /* base case */
    if (a == 0)
    {
        *x = 0; 
        *y = 1;
        return b;
    }
    
    long x1, y1;
    unsigned long gcd = ext_gcd(b%a, a, &x1, &y1);

    *x = y1 - (b/a) * x1;
    *y = x1;

    return gcd;
}

unsigned long mod_inv(unsigned long e, unsigned long mod)
{
    long x, y, d; 
    unsigned long g;
    g = ext_gcd(e, mod, &x, &y);
    if(g != 1)
    {
        /* no mod_inv */
        return e*mod;    
    }
    else
    {
        d = x % (long)mod;
        /* for cases when x is negative */
        if (d < 0)
        {
            d += mod;
        }
    }
    return d;
}

void generateRSA_Keys(RSA_PRIV *rsaPriv, RSA_PUB *rsaPub)
{
    unsigned long e, p, q, n, lambda, d;
    
    e = PUBLIC_e_VALUE;  
    #ifdef DEBUG
        printf("\nFinding prime p...\n");
    #endif
    p = generateRSA_prime(e);
    #ifdef DEBUG
        printf("p = %lu\n", p);
        printf("\nFinding a prime q...\n");
    #endif
    q = generateRSA_prime(e);
    #ifdef DEBUG
        printf("q = %lu\n", q);
    #endif
    while(p == q)
    {
        #ifdef DEBUG
            printf("\np = q, so regenerating q...\n");
        #endif
        q = generateRSA_prime(e);
    }
    assert(p != q); 
    n = p * q;
    lambda = (p - 1) * (q - 1);
    d = mod_inv(e, lambda);
    
    rsaPriv->n = n;
    rsaPriv->d = d;
    rsaPub->n = n;
    rsaPub->e = e;
    #ifdef DEBUG
    printf("n = %lu, d = %lu, p = %lu, q = %lu\n", n, d, p, q);
    #endif
}

/* encrypts message into cipher */
unsigned long rsa_encrypt(unsigned long message, RSA_PUB* rsaPub)
{
    assert(rsaPub);
    assert(message < rsaPub->n);
    unsigned long cipher;
    cipher = mod_exp(message, rsaPub->e, rsaPub->n);
    return cipher;
}

/* decrypts cipher into message */
unsigned long rsa_decrypt(unsigned long cipher, RSA_PRIV* rsaPriv)
{
    assert(rsaPriv);
    unsigned long message;
    message = mod_exp(cipher, rsaPriv->d, rsaPriv->n);
    return message;
}

#ifdef MAIN
int main()
{
    RSA_PRIV *rsaPriv = NULL;
    RSA_PUB *rsaPub = NULL;
    srand(time(0));
    /* printf("Test testPrime function\n");
    unsigned long p;
    printf("Enter p: ");
    scanf("%lu", &p);
    printf("p = %lu \n", p);
    int test = testPrime(p);
    printf("test = %d\n", test); 
    unsigned long e, mod, d;
    long temp1 = -367, temp2 = 780;
    printf("-367 mod 780 = %ld\n", temp1%temp2);
    printf("Test mod_inv function\n");
    printf("Enter e: ");
    scanf("%lu", &e);
    printf("Enter mod: ");
    scanf("%lu", &mod);
    d = mod_inv(e, mod);
    printf("d = %lu\n", d); */
    unsigned long x;
    x = randNum();
    printf("x = %lu\n", x);
    rsaPriv = createRSA_priv(0, 0);
    rsaPub = createRSA_pub(0, 0);
    generateRSA_Keys(rsaPriv, rsaPub);
    printf("n = %lu\n", rsaPriv->n);
    printf("d = %lu\n", rsaPriv->d);
    printf("e = %lu\n", rsaPub->e);
    unsigned long message, cipher, decrypt;
    printf("Enter message to encrypt: ");
    scanf("%lu", &message);
    cipher = rsa_encrypt(message, rsaPub);
    printf("cipher = %lu\n", cipher);
    decrypt = rsa_decrypt(cipher, rsaPriv);
    printf("decrypt = %lu\n", decrypt);
    if (decrypt == message)
    {
        printf("RSA works!\n");
    }
    return 0;
}
#endif
