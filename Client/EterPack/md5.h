#ifndef __INC_ETERPACKLIB_MD5_H__
#define __INC_ETERPACKLIB_MD5_H__

/*

 ***********************************************************************

 ** md5.h -- header file for implementation of MD5                    **

 ** RSA Data Security, Inc. MD5 Message-Digest Algorithm              **

 ** Created: 2/17/90 RLR                                              **

 ** Revised: 12/27/90 SRD,AJ,BSK,JT Reference C version               **

 ** Revised (for MD5): RLR 4/27/91                                    **

 **   -- G modified to have y&~z instead of y&z                       **

 **   -- FF, GG, HH modified to add in last register done             **

 **   -- Access pattern: round 2 works mod 5, round 3 works mod 3     **

 **   -- distinct additive constant for each step                     **

 **   -- round 4 added, working mod 7                                 **

 ***********************************************************************

 */

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

/* typedef a 32-bit type */

typedef unsigned long int UINT4;

/* Data structure for MD5 (Message-Digest) computation */

typedef struct {
  UINT4 i[2];                   /* number of _bits_ handled mod 2^64 */

  UINT4 buf[4];                                    /* scratch buffer */

  unsigned char in[64];                              /* input buffer */

  unsigned char digest[16];     /* actual digest after MD5Final call */
} MD5_CTX;

#if defined (__cplusplus)
extern "C" {
#endif

void MD5Init (MD5_CTX *);

void MD5Update (MD5_CTX *,unsigned char *,unsigned int);

void MD5Final (MD5_CTX *);

void MD5Transform(UINT4 *,UINT4 *);

#if defined (__cplusplus)
};
#endif

/*

 ***********************************************************************

 ** End of md5.h                                                      **

 ******************************** (cut) ********************************

 */

#endif
//martysama0134's 4ee2cf2c13ce32ff35a20010fbba866e
