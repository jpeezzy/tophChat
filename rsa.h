#ifndef RSA_H
#define RSA_H

/*  How to use RSA.h
 *  First create RSA_PRIV and RSA_PUB using 
 *  createRSA_priv(0, 0) and createRSA_pub(0, 0)
 *  use generateRSA_Keys() to initialize created structures
 *  then use rsa_encrypt() to encrypt 
 *  and use rsa_decrypt() to decrypt */

typedef struct rsa_priv_keys RSA_PRIV;
typedef struct rsa_pub_keys RSA_PUB;

struct rsa_priv_keys
{
    unsigned long n;
    unsigned long d;
};

struct rsa_pub_keys
{
    unsigned long n;
    unsigned long e;
};

/* creates RSA_PRIV and initilizes with n and d */
RSA_PRIV *createRSA_priv(unsigned long n, unsigned long d);

/* creates RSA_PUB and initializes with n and e */
RSA_PUB *createRSA_pub(unsigned long n, unsigned long e);

/* deletes RSA_PRIV struct */
void deleteRSA_priv(RSA_PRIV *rsa_priv);

/* deletes RSA_PUB struct */
void deleteRSA_pub(RSA_PUB *rsa_pub);

/* generate a 1024 bit prime number p for rsa encryption 
 * where p must satisfy the condition (p mod e) != 1 */
unsigned long generateRSA_prime(unsigned long e);

/* test a prime using Rabin-Miller test */
int testPrime(unsigned long p);

/* Rabin-Miller test algorithm */
int millerTest(unsigned long p, unsigned long d);

/* calcuates x^n using fast exponentiation */
unsigned long power(unsigned long x, unsigned long n);

/* calculates (x*y) mod n, takes cares of overflow */
unsigned long mod_mul(unsigned long x, unsigned long y, unsigned long n);

/* calculates (b^e) mod n, take cares of overflow */
unsigned long mod_exp(unsigned long b, unsigned long e, unsigned long n);

/* use extended euclidean algorithm to find x, y such that (a*x)+(b*y) = 1 */
unsigned long ext_gcd(unsigned long a, unsigned long b, long *x, long *y);

/* calculates inverse a^-1 mod b */
unsigned long mod_inv(unsigned long a, unsigned long b);

/* generate RSA keys into rsaPriv and rsaPub */
void generateRSA_Keys(RSA_PRIV* rsaPriv, RSA_PUB* rsaPub);

/* encrypts message into cipher */
unsigned long rsa_encrypt(unsigned long message, RSA_PUB* rsaPub);

/* decrypts cipher into message */
unsigned long rsa_decrypt(unsigned long cipher, RSA_PRIV* rsaPriv);

#endif

