/*************************************************
  Copyright (C),   
  File name:       Rsa.c
  Author:          
  Version:         V1.0 
  Date: 		   2015-03-03
  Description:     
  Function List:  
    			  
  History:         				  
    1. Date:
       Author:
       Modification:
    2. ...
*************************************************/

#include <stdio.h>
#include <string.h>
#include "Rsa.h"
typedef unsigned int NN_DIGIT;
typedef unsigned short NN_HALF_DIGIT;

static __asm void dmult(unsigned int a, unsigned int b, unsigned int *cHigh, unsigned int *cLow)	
{
	PUSH {R0-R5,LR}
	umull r4, r5, r0, r1
		str r5, [r2]
		str r4, [r3]
	POP {R0-R5,PC}
}

static NN_DIGIT subdigitmult(NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT c, NN_DIGIT *d, NN_DIGIT digits)
{
	NN_DIGIT borrow, thigh, tlow;
	NN_DIGIT i;

	borrow = 0;

	if(c != 0)
	{
		for(i = 0; i < digits; i++)
		{
			dmult(c, d[i], &thigh, &tlow);
			if((a[i] = b[i] - borrow) > (MAX_NN_DIGIT - borrow))
				borrow = 1;
			else
				borrow = 0;
			if((a[i] -= tlow) > (MAX_NN_DIGIT - tlow))
				borrow++;
			borrow += thigh;
		}
	}
	return (borrow);
}

static void NN_Decode (NN_DIGIT *a, unsigned int digits, unsigned char *b, unsigned int len)
{
	NN_DIGIT t;
	int j;
	unsigned int i, u;

	for (i = 0, j = len - 1; i < digits && j >= 0; i++)
	{
		t = 0;
		for (u = 0; j >= 0 && u < NN_DIGIT_BITS; j--, u += 8)
		{
			t |= ((NN_DIGIT)b[j]) << u;
		}
		a[i] = t;
	}

	for (; i < digits; i++)
		a[i] = 0;
}

static void NN_Encode (unsigned char *a, unsigned int len, NN_DIGIT *b, unsigned int digits)
{
	NN_DIGIT t;
	int j;
	unsigned int i, u;

	for (i = 0, j = len - 1; i < digits && j >= 0; i++)
	{
		t = b[i];
		for (u = 0; j >= 0 && u < NN_DIGIT_BITS; j--, u += 8)
			a[j] = (unsigned char)(t >> u);
	}

	for (; j >= 0; j--)
		a[j] = 0;
}

static void NN_Assign (NN_DIGIT *a, NN_DIGIT *b, unsigned int digits)
{
	if(digits)
	{
		do
		{
			*a++ = *b++;
		}while(--digits);
	}
}

static void NN_AssignZero (NN_DIGIT *a, unsigned int digits)
{
	if(digits)
	{
		do
		{
			*a++ = 0;
		}while(--digits);
	}
}

static unsigned int NN_DigitBits (NN_DIGIT a)
{
	unsigned int i;

	for (i = 0; i < NN_DIGIT_BITS; i++, a >>= 1)
	{
		if (a == 0)
			break;
	}

	return (i);
}

static unsigned int NN_Digits (NN_DIGIT *a, unsigned int digits)
{
	int i;

	for (i = digits - 1; i >= 0; i--)
	{
		if (a[i])
			break;
	}

	return (i + 1);
}

static int NN_Cmp (NN_DIGIT *a, NN_DIGIT *b, unsigned int digits)
{
	int i;

	for (i = digits - 1; i >= 0; i--)
	{
		if (a[i] > b[i])
			return (1);
		if (a[i] < b[i])
			return (-1);
	}

	return (0);
}

static NN_DIGIT NN_Add (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, unsigned int digits)
{
	NN_DIGIT ai, carry;
	unsigned int i;

	carry = 0;

	for (i = 0; i < digits; i++)
	{
		if ((ai = b[i] + carry) < carry)
			ai = c[i];
		else if ((ai += c[i]) < c[i])
			carry = 1;
		else
			carry = 0;
		a[i] = ai;
	}

	return (carry);
}


static NN_DIGIT NN_Sub (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, unsigned int digits)
{
	NN_DIGIT ai, borrow;
	unsigned int i;

	borrow = 0;

	for (i = 0; i < digits; i++)
	{
		if ((ai = b[i] - borrow) > (MAX_NN_DIGIT - borrow))
			ai = MAX_NN_DIGIT - c[i];
		else if ((ai -= c[i]) > (MAX_NN_DIGIT - c[i]))
			borrow = 1;
		else
			borrow = 0;
		a[i] = ai;
	}

	return (borrow);
}

static NN_DIGIT NN_LShift (NN_DIGIT *a, NN_DIGIT *b, unsigned int c, unsigned int digits)
{
	NN_DIGIT bi, carry;
	unsigned int i, t;

	if (c >= NN_DIGIT_BITS)
		return (0);

	t = NN_DIGIT_BITS - c;

	carry = 0;

	for (i = 0; i < digits; i++)
	{
		bi = b[i];
		a[i] = (bi << c) | carry;
		carry = c ? (bi >> t) : 0;
	}

	return (carry);
}

static NN_DIGIT NN_RShift (NN_DIGIT *a, NN_DIGIT *b, unsigned int c, unsigned int digits)
{
	NN_DIGIT bi, carry;
	int i;
	unsigned int t;

	if (c >= NN_DIGIT_BITS)
		return (0);

	t = NN_DIGIT_BITS - c;

	carry = 0;

	for (i = digits - 1; i >= 0; i--)
	{
		bi = b[i];
		a[i] = (bi >> c) | carry;
		carry = c ? (bi << t) : 0;
	}

	return (carry);
}


static void NN_Mult (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, unsigned int digits)
{
	NN_DIGIT dhigh=0, dlow=0, carry;
	NN_DIGIT bDigits, cDigits, i, j;
	NN_DIGIT mt[2*MAX_NN_DIGITS];

	NN_AssignZero (mt, 2 * digits);

	bDigits = NN_Digits (b, digits);
	cDigits = NN_Digits (c, digits);

	for (i = 0; i < bDigits; i++)
	{
		carry = 0;
		if(*(b+i) != 0)
		{
			for(j = 0; j < cDigits; j++)
			{
				dmult(*(b+i), *(c+j), &dhigh, &dlow);
				if((*(mt+(i+j)) = *(mt+(i+j)) + carry) < carry)
					carry = 1;
				else
					carry = 0;
				if((*(mt+(i+j)) += dlow) < dlow)
					carry++;
				carry += dhigh;
			}
		}
		*(mt+(i+cDigits)) += carry;
	}

	NN_Assign (a, mt, 2 * digits);

	memset ((unsigned char *)mt, 0, sizeof (mt));
}

static void NN_Div (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, unsigned int cDigits, NN_DIGIT *d, unsigned int dDigits)
{
	NN_DIGIT ai;
	int i;
	unsigned int ddDigits, shift;
	NN_DIGIT div_cc[2*MAX_NN_DIGITS+1], div_dd[MAX_NN_DIGITS]; 

	NN_DIGIT s;
	NN_DIGIT t[2], u, v, *ccptr;
	NN_HALF_DIGIT aHigh, aLow, cHigh, cLow;

	ddDigits = NN_Digits (d, dDigits);
	if (ddDigits == 0)
		return;

	/* Normalize operands.
	*/
	shift = NN_DIGIT_BITS - NN_DigitBits (d[ddDigits-1]);
	NN_AssignZero (div_cc, ddDigits);
	div_cc[cDigits] = NN_LShift (div_cc, c, shift, cDigits);
	NN_LShift (div_dd, d, shift, ddDigits);

	s = div_dd[ddDigits-1];

	NN_AssignZero (a, cDigits);

	for (i = cDigits-ddDigits; i >= 0; i--)
	{
		if (s == MAX_NN_DIGIT)
		{
			ai = div_cc[i+ddDigits];
		}
		else
		{
			ccptr = &div_cc[i+ddDigits-1];

			s++;
			cHigh = (NN_HALF_DIGIT)HIGH_HALF(s);
			cLow = (NN_HALF_DIGIT)LOW_HALF(s);

			*t = *ccptr;
			*(t+1) = *(ccptr+1);

			if (cHigh == MAX_NN_HALF_DIGIT)
			{
				aHigh = (NN_HALF_DIGIT)HIGH_HALF(*(t+1));
			}
			else
			{
				aHigh = (NN_HALF_DIGIT)(*(t+1) / (cHigh + 1));
			}
			u = (NN_DIGIT)aHigh * (NN_DIGIT)cLow;
			v = (NN_DIGIT)aHigh * (NN_DIGIT)cHigh;
			if ((*t -= TO_HIGH_HALF(u)) > (MAX_NN_DIGIT - TO_HIGH_HALF(u)))
			{
				t[1]--;
			}
			*(t+1) -= HIGH_HALF(u);
			*(t+1) -= v;

			while ((*(t+1) > cHigh) ||	((*(t+1) == cHigh) && (*t >= TO_HIGH_HALF(cLow))))
			{
				if ((*t -= TO_HIGH_HALF(cLow)) > MAX_NN_DIGIT - TO_HIGH_HALF(cLow))
					t[1]--;
				*(t+1) -= cHigh;
				aHigh++;
			}

			if (cHigh == MAX_NN_HALF_DIGIT)
			{
				aLow = (NN_HALF_DIGIT)LOW_HALF(*(t+1));
			}
			else
			{
				aLow = (NN_HALF_DIGIT)((TO_HIGH_HALF(*(t+1)) + HIGH_HALF(*t)) / (cHigh + 1));
			}
			u = (NN_DIGIT)aLow * (NN_DIGIT)cLow;
			v = (NN_DIGIT)aLow * (NN_DIGIT)cHigh;
			if ((*t -= u) > (MAX_NN_DIGIT - u))
				t[1]--;
			if ((*t -= TO_HIGH_HALF(v)) > (MAX_NN_DIGIT - TO_HIGH_HALF(v)))
				t[1]--;
			*(t+1) -= HIGH_HALF(v);

			while ((*(t+1) > 0) || ((*(t+1) == 0) && *t >= s))
			{
				if ((*t -= s) > (MAX_NN_DIGIT - s))
					t[1]--;
				aLow++;
			}

			ai = TO_HIGH_HALF(aHigh) + aLow;
			s--;
		}

		div_cc[i+ddDigits] -= subdigitmult(&div_cc[i], &div_cc[i], ai, div_dd, ddDigits);

		while (div_cc[i+ddDigits] || (NN_Cmp(&div_cc[i], div_dd, ddDigits) >= 0))
		{
			ai++;
			div_cc[i+ddDigits] -= NN_Sub(&div_cc[i], &div_cc[i], div_dd, ddDigits);
		}

		a[i] = ai;
	}

	NN_AssignZero (b, dDigits);
	NN_RShift (b, div_cc, shift, ddDigits);

	memset ((unsigned char *)div_cc, 0, sizeof (div_cc));
	memset ((unsigned char *)div_dd, 0, sizeof (div_dd));
}

static void NN_Mod (NN_DIGIT *a, NN_DIGIT *b, unsigned int bDigits, NN_DIGIT *c, unsigned int cDigits)
{
	NN_DIGIT mod_t[2 * MAX_NN_DIGITS];  

	NN_Div (mod_t, a, b, bDigits, c, cDigits);

	memset ((unsigned char *)mod_t, 0, sizeof (mod_t));
}

static void NN_ModMult (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, NN_DIGIT *d, unsigned int digits)
{
	NN_DIGIT mod_t[2 * MAX_NN_DIGITS]; 

	NN_Mult (mod_t, b, c, digits);
	NN_Mod (a, mod_t, 2 * digits, d, digits);

	memset ((unsigned char *)mod_t, 0, sizeof (mod_t));
}

static void NN_ModExp (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, unsigned int cDigits, NN_DIGIT *d, unsigned int dDigits)
{
	int i;
	unsigned int ciBits, j, s;
	NN_DIGIT ci;
	NN_DIGIT bPower[3][MAX_NN_DIGITS], Exp_t[MAX_NN_DIGITS];  

	NN_Assign (bPower[0], b, dDigits);
	NN_ModMult (bPower[1], bPower[0], b, d, dDigits);
	NN_ModMult (bPower[2], bPower[1], b, d, dDigits);

	NN_ASSIGN_DIGIT (Exp_t, 1, dDigits);

	cDigits = NN_Digits (c, cDigits);
	for (i = cDigits - 1; i >= 0; i--)
	{
		ci = c[i];
		ciBits = NN_DIGIT_BITS;

		if (i == (int)(cDigits - 1))
		{
			while (! DIGIT_2MSB (ci))
			{
				ci <<= 2;
				ciBits -= 2;
			}
		}

		for (j = 0; j < ciBits; j += 2, ci <<= 2)
		{
			NN_ModMult (Exp_t, Exp_t, Exp_t, d, dDigits);
			NN_ModMult (Exp_t, Exp_t, Exp_t, d, dDigits);
			if ((s = DIGIT_2MSB (ci)) != 0)
				NN_ModMult (Exp_t, Exp_t, bPower[s-1], d, dDigits);
		}
	}

	NN_Assign (a, Exp_t, dDigits);

	memset ((unsigned char *)bPower, 0, sizeof (bPower));
	memset ((unsigned char *)Exp_t, 0, sizeof (Exp_t));
}

static int RSAPublicBlock ( void *output,                  /* output block */
							    unsigned int *outputLen,              /* length of output block */
							    const void *input,                /* input block */
							    unsigned int inputLen,                /* length of input block */
							    R_RSA_PUBLIC_KEY *publicKey)      /* RSA public key */
{
	unsigned int eDigits, nDigits;
	NN_DIGIT pc[MAX_NN_DIGITS]; 
	NN_DIGIT pe[MAX_NN_DIGITS];    
	NN_DIGIT pm[MAX_NN_DIGITS];    
	NN_DIGIT pn[MAX_NN_DIGITS];      

	NN_Decode (pm, MAX_NN_DIGITS, (unsigned char *)input, inputLen);
	NN_Decode (pn, MAX_NN_DIGITS, publicKey->modulus, publicKey->bits/8);
	NN_Decode (pe, MAX_NN_DIGITS, publicKey->exponent, 4);
	nDigits = NN_Digits (pn, MAX_NN_DIGITS);
	eDigits = NN_Digits (pe, MAX_NN_DIGITS);

	if (NN_Cmp (pm, pn, nDigits) >= 0)
		return (RE_DATA);

	NN_ModExp (pc, pm, pe, eDigits, pn, nDigits);

	*outputLen = (publicKey->bits + 7) / 8;
	NN_Encode ((unsigned char *)output, *outputLen, pc, nDigits);

	memset ((unsigned char *)pc, 0, sizeof (pc));
	memset ((unsigned char *)pm, 0, sizeof (pm));

	return (0);
}

static int RSAPrivateBlock ( void *output,                /* output block */
							     unsigned int *outputLen,            /* length of output block */
							     const void *input,              /* input block */
							     unsigned int inputLen,              /* length of input block */
							     R_RSA_PRIVATE_KEY *privateKey)  /* RSA private key */
{
	unsigned int cDigits, nDigits, pDigits, OutLen;
	NN_DIGIT c[MAX_NN_DIGITS];            // 私钥运算临时输入运算缓冲区
	NN_DIGIT cP[MAX_NN_DIGITS];           // c模p分量结果
	NN_DIGIT cQ[MAX_NN_DIGITS];           // c模q分量结果
	NN_DIGIT dP[MAX_NN_DIGITS];           // 私钥指数p分量
	NN_DIGIT dQ[MAX_NN_DIGITS];           // 私钥指数q分量
	NN_DIGIT mP[MAX_NN_DIGITS];           // mP = cP^dP mod p
	NN_DIGIT mQ[MAX_NN_DIGITS];           // mQ = cQ^dQ mod q
	NN_DIGIT n[MAX_NN_DIGITS];            // 私钥模
	NN_DIGIT p[MAX_NN_DIGITS];            // 私钥p分量
	NN_DIGIT q[MAX_NN_DIGITS];            // 私钥q分量
	NN_DIGIT qInv[MAX_NN_DIGITS];         // 私钥p和q的相关分量
	NN_DIGIT t[MAX_NN_DIGITS];            // 私钥运算后输出数据

	NN_Decode (c, MAX_NN_DIGITS, (unsigned char *)input, inputLen);


	NN_Decode(n, MAX_NN_DIGITS, privateKey->modulus, privateKey->bits/8);
	NN_Decode(p, MAX_NN_DIGITS, privateKey->prime[0], privateKey->bits/16);
	NN_Decode(q, MAX_NN_DIGITS, privateKey->prime[1], privateKey->bits/16);
	NN_Decode(dP, MAX_NN_DIGITS, privateKey->primeExponent[0], privateKey->bits/16);
	NN_Decode(dQ, MAX_NN_DIGITS, privateKey->primeExponent[1], privateKey->bits/16);
	NN_Decode(qInv, MAX_NN_DIGITS, privateKey->coefficient, privateKey->bits/16);

	cDigits = NN_Digits (c, MAX_NN_DIGITS);
	nDigits = NN_Digits (n, MAX_NN_DIGITS);
	pDigits = NN_Digits (p, MAX_NN_DIGITS);

	if (NN_Cmp (c, n, nDigits) >= 0)
		return (RE_DATA);

	NN_Mod (cP, c, cDigits, p, pDigits);
	NN_Mod (cQ, c, cDigits, q, pDigits);
	NN_ModExp (mP, cP, dP, pDigits, p, pDigits);
	NN_AssignZero (mQ, nDigits);
	NN_ModExp (mQ, cQ, dQ, pDigits, q, pDigits);

	if (NN_Cmp (mP, mQ, pDigits) >= 0)
		NN_Sub (t, mP, mQ, pDigits);
	else
	{
		NN_Sub (t, mQ, mP, pDigits);
		NN_Sub (t, p, t, pDigits);
	}
	NN_ModMult (t, t, qInv, p, pDigits);
	NN_Mult (t, t, q, pDigits);
	NN_Add (t, t, mQ, nDigits);

    OutLen = (privateKey->bits + 7) / 8;
    if(outputLen != NULL)
    {
	    *outputLen = OutLen;
    }
    if(output != NULL)
    {
	    NN_Encode ((unsigned char *)output, OutLen, t, nDigits);
    }

	memset ((unsigned char *)c, 0, sizeof (c));
	memset ((unsigned char *)cP, 0, sizeof (cP));
	memset ((unsigned char *)cQ, 0, sizeof (cQ));
	memset ((unsigned char *)dP, 0, sizeof (dP));
	memset ((unsigned char *)dQ, 0, sizeof (dQ));
	memset ((unsigned char *)mP, 0, sizeof (mP));
	memset ((unsigned char *)mQ, 0, sizeof (mQ));
	memset ((unsigned char *)p, 0, sizeof (p));
	memset ((unsigned char *)q, 0, sizeof (q));
	memset ((unsigned char *)qInv, 0, sizeof (qInv));
	memset ((unsigned char *)t, 0, sizeof (t));

	return (0);
}

int RSAPublicDecrypt(  void *output,				/* output block */
                           unsigned int *outputLen,			/* length of output block */
                           const void *input,			/* input block */
                           unsigned int inputLen,			/* length of input block */
                           R_RSA_PUBLIC_KEY *publicKey)	/* RSA public key */
{
	int status;
	unsigned int i, modulusLen, pkcsBlockLen, OutLen;
	unsigned char pkcsBlock[MAX_RSA_MODULUS_LEN]; // 公钥操作时用做临时输出缓冲区

    if(input == NULL || publicKey == NULL)
    {
        return(-1);
    }
	modulusLen = (publicKey->bits + 7) / 8;
	if (inputLen > modulusLen)
		return (RE_LEN);

	status = RSAPublicBlock(pkcsBlock, &pkcsBlockLen, input, inputLen, publicKey);
	if(status)
	{
		return (status);
	}

	if (pkcsBlockLen != modulusLen)
		return (RE_LEN);

	i = 0;
    OutLen = modulusLen - i;
    if(OutLen > modulusLen)
    {
        return(RE_DATA);
    }
    if(outputLen != NULL)
    {
    	*outputLen = OutLen;
    }

    if(output != NULL)
    {
	    memcpy(output, &pkcsBlock[i], OutLen);
    }

	memset(pkcsBlock, 0, sizeof (pkcsBlock));

	return (0);
}


int RSAPrivateEncrypt ( void *output,					/* output block */
							unsigned int *outputLen,			/* length of output block */
							const void *input,				/* input block */
							unsigned int inputLen,				/* length of input block */
							R_RSA_PRIVATE_KEY *privateKey)	/* RSA private key */
{
	int status;
	unsigned int i, modulusLen;
	unsigned char pkcsBlock[MAX_RSA_MODULUS_LEN]; 

    if(input == NULL || privateKey == NULL)
    {
        return(-1);
    }
	modulusLen = (privateKey->bits + 7) / 8;

	if (inputLen  > modulusLen)
		return (RE_LEN);

	i = 0;
	memcpy (pkcsBlock+i, input, inputLen);

	status = RSAPrivateBlock(output, outputLen, pkcsBlock, modulusLen, privateKey);

	memset (pkcsBlock, 0, sizeof (pkcsBlock));

	return (status);
}

