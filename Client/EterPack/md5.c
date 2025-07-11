/*

 ***********************************************************************

 ** Copyright (C) 1990, RSA Data Security, Inc. All rights reserved.  **

 **                                                                   **

 ** License to copy and use this software is granted provided that    **

 ** it is identified as the "RSA Data Security, Inc. MD5 Message-     **

 ** Digest Algorithm" in all material mentioning or referencing this  **

 ** software or this function.                                        **

 **                                                                   **

 ** License is also granted to make and use derivative works          **

 ** provided that such works are identified as "derived from the RSA  **

 ** Data Security, Inc. MD5 Message-Digest Algorithm" in all          **

 ** material mentioning or referencing the derived work.              **

 **                                                                   **

 ** RSA Data Security, Inc. makes no representations concerning       **

 ** either the merchantability of this software or the suitability    **

 ** of this software for any particular purpose.  It is provided "as  **

 ** is" without express or implied warranty of any kind.              **

 **                                                                   **

 ** These notices must be retained in any copies of any part of this  **

 ** documentation and/or software.                                    **

 ***********************************************************************

 */


#include "md5.h"



/*

 ***********************************************************************

 **  Message-digest routines:                                         **

 **  To form the message digest for a message M                       **

 **    (1) Initialize a context buffer mdContext using MD5Init        **

 **    (2) Call MD5Update on mdContext and M                          **

 **    (3) Call MD5Final on mdContext                                 **

 **  The message digest is now in mdContext->digest[0...15]           **

 ***********************************************************************

 */



static unsigned char PADDING[64] = {

  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00

};



#ifndef CPU386  /* Alternate defs exist for 386 assembler version */

/* F, G, H and I are basic MD5 functions */

#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))

#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))

#define H(x, y, z) ((x) ^ (y) ^ (z))

#define I(x, y, z) ((y) ^ ((x) | (~z)))



/* ROTATE_LEFT rotates x left n bits */

#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))



/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4 */

/* Rotation is separate from addition to prevent recomputation */

#define FF(a, b, c, d, x, s, ac) \
  (a) += F ((b), (c), (d)) + (x) + (UINT4)(ac); \
  (a) = ROTATE_LEFT ((a), (s)); \
  (a) += (b); \

#define GG(a, b, c, d, x, s, ac) \
	(a) += G ((b), (c), (d)) + (x) + (UINT4)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \

#define HH(a, b, c, d, x, s, ac) \
	(a) += H ((b), (c), (d)) + (x) + (UINT4)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \

#define II(a, b, c, d, x, s, ac) \
	(a) += I ((b), (c), (d)) + (x) + (UINT4)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \

#endif  /* CPU386 */



/* The routine MD5Init initializes the message-digest context

   mdContext. All fields are set to zero.

 */

void MD5Init (MD5_CTX *mdContext)
{

  mdContext->i[0] = mdContext->i[1] = (UINT4)0;



  /* Load magic initialization constants.

   */

  mdContext->buf[0] = (UINT4)0x67452301;

  mdContext->buf[1] = (UINT4)0xefcdab89;

  mdContext->buf[2] = (UINT4)0x98badcfe;

  mdContext->buf[3] = (UINT4)0x10325476;

}



/* The routine MD5Update updates the message-digest context to

   account for the presence of each of the characters inBuf[0..inLen-1]

   in the message whose digest is being computed.

 */

void MD5Update (MD5_CTX *mdContext, unsigned char *inBuf, unsigned int inLen)
{

  UINT4 in[16];

  int mdi;

  unsigned int i, ii;



  /* compute number of bytes mod 64 */

  mdi = (int)((mdContext->i[0] >> 3) & 0x3F);



  /* update number of bits */

  if ((mdContext->i[0] + ((UINT4)inLen << 3)) < mdContext->i[0])

    mdContext->i[1]++;

  mdContext->i[0] += ((UINT4)inLen << 3);

  mdContext->i[1] += ((UINT4)inLen >> 29);



#ifdef  LITTLE_ENDIAN

  /* Speedup for little-endian machines suggested in MD5 report --P Karn */

     if(mdi == 0 && ((int)inBuf & 3) == 0){

          while(inLen >= 64){

               MD5Transform(mdContext->buf,(UINT4 *)inBuf);

               inLen -= 64;

               inBuf += 64;

          }

     }

#endif  /* LITTLE_ENDIAN */

  while (inLen--) {

    /* add new character to buffer, increment mdi */

    mdContext->in[mdi++] = *inBuf++;



    /* transform if necessary */

    if (mdi == 0x40) {

      for (i = 0, ii = 0; i < 16; i++, ii += 4)

     in[i] = (((UINT4)mdContext->in[ii+3]) << 24) |

          (((UINT4)mdContext->in[ii+2]) << 16) |

          (((UINT4)mdContext->in[ii+1]) << 8) |

          ((UINT4)mdContext->in[ii]);

      MD5Transform (mdContext->buf, in);

      mdi = 0;

    }

  }

}



/* The routine MD5Final terminates the message-digest computation and

   ends with the desired message digest in mdContext->digest[0...15].

 */

void MD5Final (MD5_CTX *mdContext)
{

  UINT4 in[16];

  int mdi;

  unsigned int i, ii;

  unsigned int padLen;



  /* save number of bits */

  in[14] = mdContext->i[0];

  in[15] = mdContext->i[1];



  /* compute number of bytes mod 64 */

  mdi = (int)((mdContext->i[0] >> 3) & 0x3F);



  /* pad out to 56 mod 64 */

  padLen = (mdi < 56) ? (56 - mdi) : (120 - mdi);

  MD5Update (mdContext, PADDING, padLen);



  /* append length in bits and transform */

  for (i = 0, ii = 0; i < 14; i++, ii += 4)

    in[i] = (((UINT4)mdContext->in[ii+3]) << 24) |

         (((UINT4)mdContext->in[ii+2]) << 16) |

         (((UINT4)mdContext->in[ii+1]) << 8) |

         ((UINT4)mdContext->in[ii]);

  MD5Transform (mdContext->buf, in);



  /* store buffer in digest */

  for (i = 0, ii = 0; i < 4; i++, ii += 4) {

    mdContext->digest[ii] = (unsigned char)(mdContext->buf[i] & 0xFF);

    mdContext->digest[ii+1] =

      (unsigned char)((mdContext->buf[i] >> 8) & 0xFF);

    mdContext->digest[ii+2] =

      (unsigned char)((mdContext->buf[i] >> 16) & 0xFF);

    mdContext->digest[ii+3] =

      (unsigned char)((mdContext->buf[i] >> 24) & 0xFF);

  }

}



#ifndef CPU386  /* Fast assembler version exists for 386/486 */



/* Basic MD5 step. Transforms buf based on in.

 */

void MD5Transform (buf, in)

UINT4 *buf;

UINT4 *in;

{

  UINT4 a = buf[0], b = buf[1], c = buf[2], d = buf[3];



  /* Round 1 */

#define S11 7

#define S12 12

#define S13 17

#define S14 22

  FF ( a, b, c, d, in[ 0], S11, 3614090360); /* 1 */

  FF ( d, a, b, c, in[ 1], S12, 3905402710); /* 2 */

  FF ( c, d, a, b, in[ 2], S13,  606105819); /* 3 */

  FF ( b, c, d, a, in[ 3], S14, 3250441966); /* 4 */

  FF ( a, b, c, d, in[ 4], S11, 4118548399); /* 5 */

  FF ( d, a, b, c, in[ 5], S12, 1200080426); /* 6 */

  FF ( c, d, a, b, in[ 6], S13, 2821735955); /* 7 */

  FF ( b, c, d, a, in[ 7], S14, 4249261313); /* 8 */

  FF ( a, b, c, d, in[ 8], S11, 1770035416); /* 9 */

  FF ( d, a, b, c, in[ 9], S12, 2336552879); /* 10 */

  FF ( c, d, a, b, in[10], S13, 4294925233); /* 11 */

  FF ( b, c, d, a, in[11], S14, 2304563134); /* 12 */

  FF ( a, b, c, d, in[12], S11, 1804603682); /* 13 */

  FF ( d, a, b, c, in[13], S12, 4254626195); /* 14 */

  FF ( c, d, a, b, in[14], S13, 2792965006); /* 15 */

  FF ( b, c, d, a, in[15], S14, 1236535329); /* 16 */



  /* Round 2 */

#define S21 5

#define S22 9

#define S23 14

#define S24 20

  GG ( a, b, c, d, in[ 1], S21, 4129170786); /* 17 */

  GG ( d, a, b, c, in[ 6], S22, 3225465664); /* 18 */

  GG ( c, d, a, b, in[11], S23,  643717713); /* 19 */

  GG ( b, c, d, a, in[ 0], S24, 3921069994); /* 20 */

  GG ( a, b, c, d, in[ 5], S21, 3593408605); /* 21 */

  GG ( d, a, b, c, in[10], S22,   38016083); /* 22 */

  GG ( c, d, a, b, in[15], S23, 3634488961); /* 23 */

  GG ( b, c, d, a, in[ 4], S24, 3889429448); /* 24 */

  GG ( a, b, c, d, in[ 9], S21,  568446438); /* 25 */

  GG ( d, a, b, c, in[14], S22, 3275163606); /* 26 */

  GG ( c, d, a, b, in[ 3], S23, 4107603335); /* 27 */

  GG ( b, c, d, a, in[ 8], S24, 1163531501); /* 28 */

  GG ( a, b, c, d, in[13], S21, 2850285829); /* 29 */

  GG ( d, a, b, c, in[ 2], S22, 4243563512); /* 30 */

  GG ( c, d, a, b, in[ 7], S23, 1735328473); /* 31 */

  GG ( b, c, d, a, in[12], S24, 2368359562); /* 32 */



  /* Round 3 */

#define S31 4

#define S32 11

#define S33 16

#define S34 23

  HH ( a, b, c, d, in[ 5], S31, 4294588738); /* 33 */

  HH ( d, a, b, c, in[ 8], S32, 2272392833); /* 34 */

  HH ( c, d, a, b, in[11], S33, 1839030562); /* 35 */

  HH ( b, c, d, a, in[14], S34, 4259657740); /* 36 */

  HH ( a, b, c, d, in[ 1], S31, 2763975236); /* 37 */

  HH ( d, a, b, c, in[ 4], S32, 1272893353); /* 38 */

  HH ( c, d, a, b, in[ 7], S33, 4139469664); /* 39 */

  HH ( b, c, d, a, in[10], S34, 3200236656); /* 40 */

  HH ( a, b, c, d, in[13], S31,  681279174); /* 41 */

  HH ( d, a, b, c, in[ 0], S32, 3936430074); /* 42 */

  HH ( c, d, a, b, in[ 3], S33, 3572445317); /* 43 */

  HH ( b, c, d, a, in[ 6], S34,   76029189); /* 44 */

  HH ( a, b, c, d, in[ 9], S31, 3654602809); /* 45 */

  HH ( d, a, b, c, in[12], S32, 3873151461); /* 46 */

  HH ( c, d, a, b, in[15], S33,  530742520); /* 47 */

  HH ( b, c, d, a, in[ 2], S34, 3299628645); /* 48 */



  /* Round 4 */

#define S41 6

#define S42 10

#define S43 15

#define S44 21

  II ( a, b, c, d, in[ 0], S41, 4096336452); /* 49 */

  II ( d, a, b, c, in[ 7], S42, 1126891415); /* 50 */

  II ( c, d, a, b, in[14], S43, 2878612391); /* 51 */

  II ( b, c, d, a, in[ 5], S44, 4237533241); /* 52 */

  II ( a, b, c, d, in[12], S41, 1700485571); /* 53 */

  II ( d, a, b, c, in[ 3], S42, 2399980690); /* 54 */

  II ( c, d, a, b, in[10], S43, 4293915773); /* 55 */

  II ( b, c, d, a, in[ 1], S44, 2240044497); /* 56 */

  II ( a, b, c, d, in[ 8], S41, 1873313359); /* 57 */

  II ( d, a, b, c, in[15], S42, 4264355552); /* 58 */

  II ( c, d, a, b, in[ 6], S43, 2734768916); /* 59 */

  II ( b, c, d, a, in[13], S44, 1309151649); /* 60 */

  II ( a, b, c, d, in[ 4], S41, 4149444226); /* 61 */

  II ( d, a, b, c, in[11], S42, 3174756917); /* 62 */

  II ( c, d, a, b, in[ 2], S43,  718787259); /* 63 */

  II ( b, c, d, a, in[ 9], S44, 3951481745); /* 64 */



  buf[0] += a;

  buf[1] += b;

  buf[2] += c;

  buf[3] += d;

}

#else   /* CPU386 */

/* Fast 386 Borland C inline assembler version of the MD5Transform() function

 * from the RSA Data Security, Inc, MD5 Message Digest Algorithm.

 *

 * This version uses native 32 bit registers, so it needs a 386 or 486 CPU.

 *

 * Because this function does *lots* of 32-bit operations, this version is

 * MUCH faster than the reference C version compiled with a garden-

 * variety 16-bit MS-DOS C compiler.

 *

 * Written and placed into the public domain on

 * 22 February 1992 by Phil Karn, KA9Q

 */

/* I really shouldn't have to do this explicitly... */

#ifdef  __COMPACT__

     asm     .MODEL COMPACT

#elif   __HUGE__

     asm     .MODEL HUGE

#elif   __LARGE__

     asm     .MODEL LARGE

#elif   __MEDIUM__

     asm     .MODEL MEDIUM

#elif   __SMALL__

     asm     .MODEL SMALL

#elif   __TINY__

     asm     .MODEL TINY

#endif



/* Code sequence common to all four rounds.

 * evaluates a = b + (a + edi + x + t) <<< s

 * Assumes a,b are registers, s,t are immediate constants

 * The 'lea' instruction is just a fast way to compute "a = a+t+edi"

 */

#define COM(a,b,x,s,t)\

     asm     lea a,t[a+edi];\

     asm     add a,x;\

     asm     rol a,s;\

     asm     add a,b;



/* Round 1 functions */

/* edi = F(x,y,z) = (x & y) | (~x & z) */

#define F(x,y,z)\

     asm     mov edi,x;\

     asm     and edi,y;\

     asm     mov esi,x;\

     asm     not esi;\

     asm     and esi,z;\

     asm     or edi,esi



/* a = b + ((a + F(x,y,z) + x + t) <<< s); */

#define FF(a,b,c,d,x,s,t)\

     F(b,c,d);\

     COM(a,b,x,s,t)



/* Round 2 functions */

/* edi = G(x,y,z) = F(z,x,y) = (x & z) | (y & ~z) */

#define G(x,y,z) F(z,x,y)



/* a = b + ((a + G(b,c,d) + x + t) <<< s) */

#define GG(a,b,c,d,x,s,t)\

     G(b,c,d);\

     COM(a,b,x,s,t)



/* Round 3 functions */

/* edi = H(x,y,z) = x ^ y ^ z */

#define H(x,y,z)\

     asm     mov edi,x;\

     asm     xor edi,y;\

     asm     xor edi,z



/* a = b + ((a + H(b,c,d) + x + t) <<< s) */

#define HH(a,b,c,d,x,s,t)\

     H(b,c,d);\

     COM(a,b,x,s,t)



/* Round 4 functions */

/* edi = I(x,y,z) = y ^ (x | ~z) */

#define I(x,y,z)\

     asm     mov edi,z;\

     asm     not edi;\

     asm     or edi,x;\

     asm     xor edi,y



/* a = b + ((a + I(b,c,d) + x + t) <<< s) */

#define II(a,b,c,d,x,s,t)\

     I(b,c,d);\

     COM(a,b,x,s,t)



#define A       eax

#define B       ebx

#define C       ecx

#define D       edx



void

MD5Transform(buf,input)

UINT4 *buf;

UINT4 *input;

{

     asm     .386;   /* Allow use of 32-bit general registers */



     /* Save caller's registers */

     asm     push si;

     asm     push di;

     asm     push es;

     asm     if @DataSize NE 0

     asm             push ds;

     asm     endif



     /* Get buf argument */

     asm     if @DataSize NE 0

     asm             lds si,buf;

     asm     else

     asm             mov si,buf;

     asm     endif

     asm     mov A,dword ptr si[0*4];        /* A = buf[0] */

     asm     mov B,dword ptr si[1*4];        /* B = buf[1] */

     asm     mov C,dword ptr si[2*4];        /* C = buf[2] */

     asm     mov D,dword ptr si[3*4];        /* D = buf[3] */



     /* Warning: This makes our other args inaccessible until bp

      * is restored!

     */

     asm     push bp;

     asm     les bp,input



/* Round 1. The *4 factors in the subscripts to bp account for the

 * byte offsets of each long element in the input array.

 */

#define S11 7

#define S12 12

#define S13 17

#define S14 22

     FF(A,B,C,D,es:bp[ 0*4],S11,3614090360); /* 1 */

     FF(D,A,B,C,es:bp[ 1*4],S12,3905402710); /* 2 */

     FF(C,D,A,B,es:bp[ 2*4],S13, 606105819); /* 3 */

     FF(B,C,D,A,es:bp[ 3*4],S14,3250441966); /* 4 */

     FF(A,B,C,D,es:bp[ 4*4],S11,4118548399); /* 5 */

     FF(D,A,B,C,es:bp[ 5*4],S12,1200080426); /* 6 */

     FF(C,D,A,B,es:bp[ 6*4],S13,2821735955); /* 7 */

     FF(B,C,D,A,es:bp[ 7*4],S14,4249261313); /* 8 */

     FF(A,B,C,D,es:bp[ 8*4],S11,1770035416); /* 9 */

     FF(D,A,B,C,es:bp[ 9*4],S12,2336552879); /* 10 */

     FF(C,D,A,B,es:bp[10*4],S13,4294925233); /* 11 */

     FF(B,C,D,A,es:bp[11*4],S14,2304563134); /* 12 */

     FF(A,B,C,D,es:bp[12*4],S11,1804603682); /* 13 */

     FF(D,A,B,C,es:bp[13*4],S12,4254626195); /* 14 */

     FF(C,D,A,B,es:bp[14*4],S13,2792965006); /* 15 */

     FF(B,C,D,A,es:bp[15*4],S14,1236535329); /* 16 */



/* Round 2 */

#define S21 5

#define S22 9

#define S23 14

#define S24 20

     GG(A,B,C,D,es:bp[ 1*4],S21,4129170786); /* 17 */

     GG(D,A,B,C,es:bp[ 6*4],S22,3225465664); /* 18 */

     GG(C,D,A,B,es:bp[11*4],S23, 643717713); /* 19 */

     GG(B,C,D,A,es:bp[ 0*4],S24,3921069994); /* 20 */

     GG(A,B,C,D,es:bp[ 5*4],S21,3593408605); /* 21 */

     GG(D,A,B,C,es:bp[10*4],S22,  38016083); /* 22 */

     GG(C,D,A,B,es:bp[15*4],S23,3634488961); /* 23 */

     GG(B,C,D,A,es:bp[ 4*4],S24,3889429448); /* 24 */

     GG(A,B,C,D,es:bp[ 9*4],S21, 568446438); /* 25 */

     GG(D,A,B,C,es:bp[14*4],S22,3275163606); /* 26 */

     GG(C,D,A,B,es:bp[ 3*4],S23,4107603335); /* 27 */

     GG(B,C,D,A,es:bp[ 8*4],S24,1163531501); /* 28 */

     GG(A,B,C,D,es:bp[13*4],S21,2850285829); /* 29 */

     GG(D,A,B,C,es:bp[ 2*4],S22,4243563512); /* 30 */

     GG(C,D,A,B,es:bp[ 7*4],S23,1735328473); /* 31 */

     GG(B,C,D,A,es:bp[12*4],S24,2368359562); /* 32 */



/* Round 3 */

#define S31 4

#define S32 11

#define S33 16

#define S34 23

     HH(A,B,C,D,es:bp[ 5*4],S31,4294588738); /* 33 */

     HH(D,A,B,C,es:bp[ 8*4],S32,2272392833); /* 34 */

     HH(C,D,A,B,es:bp[11*4],S33,1839030562); /* 35 */

     HH(B,C,D,A,es:bp[14*4],S34,4259657740); /* 36 */

     HH(A,B,C,D,es:bp[ 1*4],S31,2763975236); /* 37 */

     HH(D,A,B,C,es:bp[ 4*4],S32,1272893353); /* 38 */

     HH(C,D,A,B,es:bp[ 7*4],S33,4139469664); /* 39 */

     HH(B,C,D,A,es:bp[10*4],S34,3200236656); /* 40 */

     HH(A,B,C,D,es:bp[13*4],S31, 681279174); /* 41 */

     HH(D,A,B,C,es:bp[ 0*4],S32,3936430074); /* 42 */

     HH(C,D,A,B,es:bp[ 3*4],S33,3572445317); /* 43 */

     HH(B,C,D,A,es:bp[ 6*4],S34,  76029189); /* 44 */

     HH(A,B,C,D,es:bp[ 9*4],S31,3654602809); /* 45 */

     HH(D,A,B,C,es:bp[12*4],S32,3873151461); /* 46 */

     HH(C,D,A,B,es:bp[15*4],S33, 530742520); /* 47 */

     HH(B,C,D,A,es:bp[ 2*4],S34,3299628645); /* 48 */



/* Round 4 */

#define S41 6

#define S42 10

#define S43 15

#define S44 21

     II(A,B,C,D,es:bp[ 0*4],S41,4096336452); /* 49 */

     II(D,A,B,C,es:bp[ 7*4],S42,1126891415); /* 50 */

     II(C,D,A,B,es:bp[14*4],S43,2878612391); /* 51 */

     II(B,C,D,A,es:bp[ 5*4],S44,4237533241); /* 52 */

     II(A,B,C,D,es:bp[12*4],S41,1700485571); /* 53 */

     II(D,A,B,C,es:bp[ 3*4],S42,2399980690); /* 54 */

     II(C,D,A,B,es:bp[10*4],S43,4293915773); /* 55 */

     II(B,C,D,A,es:bp[ 1*4],S44,2240044497); /* 56 */

     II(A,B,C,D,es:bp[ 8*4],S41,1873313359); /* 57 */

     II(D,A,B,C,es:bp[15*4],S42,4264355552); /* 58 */

     II(C,D,A,B,es:bp[ 6*4],S43,2734768916); /* 59 */

     II(B,C,D,A,es:bp[13*4],S44,1309151649); /* 60 */

     II(A,B,C,D,es:bp[ 4*4],S41,4149444226); /* 61 */

     II(D,A,B,C,es:bp[11*4],S42,3174756917); /* 62 */

     II(C,D,A,B,es:bp[ 2*4],S43, 718787259); /* 63 */

     II(B,C,D,A,es:bp[ 9*4],S44,3951481745); /* 64 */



     asm pop bp;             /* We can address our args again */

     asm     if @DataSize NE 0

     asm             lds si,buf

     asm     else

     asm             mov si,buf;

     asm     endif

     asm add dword ptr si[0*4],A;    /* buf[0] += A */

     asm add dword ptr si[1*4],B;    /* buf[1] += B */

     asm add dword ptr si[2*4],C;    /* buf[2] += C */

     asm add dword ptr si[3*4],D;    /* buf[3] += D */



     /* Restore caller's registers */

     asm     if @DataSize NE 0

     asm             pop ds

     asm     endif



     asm     pop es;

     asm     pop di;

     asm     pop si;

}

#endif  /* CPU386 */



/*

 ***********************************************************************

 ** End of md5.c                                                      **

 ******************************** (cut) ********************************

 */
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
