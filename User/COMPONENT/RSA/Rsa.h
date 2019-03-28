
#ifndef POS_RSA_H
#define POS_RSA_H

// 返回值定义
#define RE_DATA 0x0401
#define RE_LEN  0x0406

/* RSA key lengths.
 */
#define MIN_RSA_MODULUS_BITS	508
/* #define MAX_RSA_MODULUS_BITS 1024 ** linq modify ->>2048 ***/
#define MAX_RSA_MODULUS_BITS	2048
#define MAX_RSA_MODULUS_LEN		((MAX_RSA_MODULUS_BITS + 7) / 8)
#define MAX_RSA_PRIME_BITS		((MAX_RSA_MODULUS_BITS + 1) / 2)
#define MAX_RSA_PRIME_LEN		((MAX_RSA_PRIME_BITS + 7) / 8)

/* Length of digit in bits */
#define NN_DIGIT_BITS			32
#define NN_HALF_DIGIT_BITS		16
/* Length of digit in bytes */
#define NN_DIGIT_LEN			(NN_DIGIT_BITS / 8)
/* Maximum length in digits */
#define MAX_NN_DIGITS			((MAX_RSA_MODULUS_LEN + NN_DIGIT_LEN - 1) / NN_DIGIT_LEN + 1)
/* Maximum digits */
#define MAX_NN_DIGIT			0xffffffff
#define MAX_NN_HALF_DIGIT		0xffff

/* Macros.
 */
#define LOW_HALF(x)				((x) & MAX_NN_HALF_DIGIT)
#define HIGH_HALF(x)			(((x) >> NN_HALF_DIGIT_BITS) & MAX_NN_HALF_DIGIT)
#define TO_HIGH_HALF(x)			(((NN_DIGIT)(x)) << NN_HALF_DIGIT_BITS)
#define DIGIT_MSB(x)			(unsigned int)(((x) >> (NN_DIGIT_BITS - 1)) & 1)
#define DIGIT_2MSB(x)			(unsigned int)(((x) >> (NN_DIGIT_BITS - 2)) & 3)

#define NN_ASSIGN_DIGIT(a, b, digits) {NN_AssignZero (a, digits); a[0] = b;}
#define NN_EQUAL(a, b, digits)	(! NN_Cmp (a, b, digits))
#define NN_EVEN(a, digits)		(((digits) == 0) || ! (a[0] & 1))

typedef struct     // 公钥结构
{
	unsigned int bits;                   /* length in bits of modulus */
	unsigned char  modulus[256];           /* modulus */
	unsigned char  exponent[4];            /* public exponent */
	unsigned short usCRC;                  /*CRC 校验码*/
} R_RSA_PUBLIC_KEY;

typedef struct    // 私钥结构
{
	unsigned int bits;                   /* length in bits of modulus */
	unsigned char  modulus[256];           /* modulus */
	unsigned char  publicExponent[4];      /* public exponent */
	unsigned char  exponent[256];          /* private exponent */
	unsigned char  prime[2][128];          /* prime factors */
	unsigned char  primeExponent[2][128];  /* exponents for CRT */
	unsigned char  coefficient[128];       /* CRT coefficient */
	unsigned short usCRC;                  /*CRC 校验码*/
} R_RSA_PRIVATE_KEY;


//----------------------------外部函数接口-----------------------------------------
int RSAPublicDecrypt(  void *output,				/* output block */
                           unsigned int *outputLen,			/* length of output block */
                           const void *input,			/* input block */
                           unsigned int inputLen,			/* length of input block */
                           R_RSA_PUBLIC_KEY *publicKey);	/* RSA public key */


#endif /* POS_RSA_H */

