/*
 * Copyright (c) 1994-2000 FUJITSU LABORATORIES LIMITED 
 * All rights reserved.
 * 
 * FUJITSU LABORATORIES LIMITED ("FLL") hereby grants you a limited,
 * non-exclusive and royalty-free license to use, copy, modify and
 * redistribute, solely for non-commercial and non-profit purposes, the
 * computer program, "Risa/Asir" ("SOFTWARE"), subject to the terms and
 * conditions of this Agreement. For the avoidance of doubt, you acquire
 * only a limited right to use the SOFTWARE hereunder, and FLL or any
 * third party developer retains all rights, including but not limited to
 * copyrights, in and to the SOFTWARE.
 * 
 * (1) FLL does not grant you a license in any way for commercial
 * purposes. You may use the SOFTWARE only for non-commercial and
 * non-profit purposes only, such as academic, research and internal
 * business use.
 * (2) The SOFTWARE is protected by the Copyright Law of Japan and
 * international copyright treaties. If you make copies of the SOFTWARE,
 * with or without modification, as permitted hereunder, you shall affix
 * to all such copies of the SOFTWARE the above copyright notice.
 * (3) An explicit reference to this SOFTWARE and its copyright owner
 * shall be made on your publication or presentation in any form of the
 * results obtained by use of the SOFTWARE.
 * (4) In the event that you modify the SOFTWARE, you shall notify FLL by
 * e-mail at risa-admin@flab.fujitsu.co.jp of the detailed specification
 * for such modification or the source code of the modified part of the
 * SOFTWARE.
 * 
 * THE SOFTWARE IS PROVIDED AS IS WITHOUT ANY WARRANTY OF ANY KIND. FLL
 * MAKES ABSOLUTELY NO WARRANTIES, EXPRESSED, IMPLIED OR STATUTORY, AND
 * EXPRESSLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT OF THIRD PARTIES'
 * RIGHTS. NO FLL DEALER, AGENT, EMPLOYEES IS AUTHORIZED TO MAKE ANY
 * MODIFICATIONS, EXTENSIONS, OR ADDITIONS TO THIS WARRANTY.
 * UNDER NO CIRCUMSTANCES AND UNDER NO LEGAL THEORY, TORT, CONTRACT,
 * OR OTHERWISE, SHALL FLL BE LIABLE TO YOU OR ANY OTHER PERSON FOR ANY
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, PUNITIVE OR CONSEQUENTIAL
 * DAMAGES OF ANY CHARACTER, INCLUDING, WITHOUT LIMITATION, DAMAGES
 * ARISING OUT OF OR RELATING TO THE SOFTWARE OR THIS AGREEMENT, DAMAGES
 * FOR LOSS OF GOODWILL, WORK STOPPAGE, OR LOSS OF DATA, OR FOR ANY
 * DAMAGES, EVEN IF FLL SHALL HAVE BEEN INFORMED OF THE POSSIBILITY OF
 * SUCH DAMAGES, OR FOR ANY CLAIM BY ANY OTHER PARTY. EVEN IF A PART
 * OF THE SOFTWARE HAS BEEN DEVELOPED BY A THIRD PARTY, THE THIRD PARTY
 * DEVELOPER SHALL HAVE NO LIABILITY IN CONNECTION WITH THE USE,
 * PERFORMANCE OR NON-PERFORMANCE OF THE SOFTWARE.
 /* $OpenXM: OpenXM_contrib2/asir2000/io/des.c,v 1.1.1.1 1999/12/03 07:39:11 noro Exp $ 
*/
 /* Copyright (C) 1996 S.Amada */

#include    <stdio.h>
#include    <stdlib.h>

#define	ROUND	16  /* 段数 */

typedef    unsigned long   ULONG;
typedef    unsigned short  USHORT;
typedef    unsigned char   UCHAR;


/*  関数のproto type	*/
void	des_enc(ULONG *, UCHAR *, ULONG *);
ULONG	round_func(ULONG , UCHAR *);
ULONG	s_box_func(UCHAR *);
void	des_dec(ULONG *, UCHAR *, ULONG *);

/*  鍵の縮約型転置 PC-1	*/
static USHORT	pc_1[56] = {
    56, 48, 40, 32, 24, 16,  8,
     0, 57, 49, 41, 33, 25, 17,
     9,  1, 58, 50, 42, 34, 26,
    18, 10,  2, 59, 51, 43, 35,
    62, 54, 46, 38, 30, 22, 14,
     6, 61, 53, 45, 37, 29, 21,
    13,  5, 60, 52, 44, 36, 28,
    20, 12,  4, 27, 19, 11,  3
};

/*  鍵のシフト回数	*/
static USHORT	k_rot[16] = {
     1,  2,  4,  6,  8, 10, 12, 14,
    15, 17, 19, 21, 23, 25, 27, 28
};

/*  鍵の縮約型転置 PC-2	*/
static USHORT	pc_2[48] = {
    13, 16, 10, 23,  0,  4,
     2, 27, 14,  5, 20,  9,
    22, 18, 11,  3, 25,  7,
    15,  6, 26, 19, 12,  1,
    40, 51, 30, 36, 46, 54,
    29, 39, 50, 44, 32, 47,
    43, 48, 38, 55, 33, 52,
    45, 41, 49, 35, 28, 31
};

/*  初期転置(IP)    */
static USHORT ip_1[64] = {
    57, 49, 41, 33, 25, 17,  9,  1,
    59, 51, 43, 35, 27, 19, 11,  3,
    61, 53, 45, 37, 29, 21, 13,  5,
    63, 55, 47, 39, 31, 23, 15,  7,
    56, 48, 40, 32, 24, 16,  8,  0,
    58, 50, 42, 34, 26, 18, 10,  2,
    60, 52, 44, 36, 28, 20, 12,  4,
    62, 54, 46, 38, 30, 22, 14,  6
};

/*  初期転置(IP^-1) */
static USHORT	ip_2[64] = {
    39,  7, 47, 15, 55, 23, 63, 31,
    38,  6, 46, 14, 54, 22, 62, 30,
    37,  5, 45, 13, 53, 21, 61, 29,
    36,  4, 44, 12, 52, 20, 60, 28,
    35,  3, 43, 11, 51, 19, 59, 27,
    34,  2, 42, 10, 50, 18, 58, 26,
    33,  1, 41,  9, 49, 17, 57, 25,
    32,  0, 40,  8, 48, 16, 56, 24
};

/* 拡大転置(E)	*/
static USHORT f_expand[48] = {
    31,  0,  1,  2,  3,  4,
     3,  4,  5,  6,  7,  8,
     7,  8,  9, 10, 11, 12,
    11, 12, 13, 14, 15, 16,
    15, 16, 17, 18, 19, 20,
    19, 20, 21, 22, 23, 24,
    23, 24, 25, 26, 27, 28,
    27, 28, 29, 30, 31,  0
};

/* S-box    */
static USHORT s_1[64] = {
    14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
     0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
     4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
    15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13
};

static USHORT s_2[64] = {
    15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
     3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
     0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
    13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9
};

static USHORT s_3[64] = {
    10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
    13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
    13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
     1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12
};

static USHORT s_4[64] = {
     7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
    13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
    10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
     3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14
};

static USHORT s_5[64] = {
     2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
    14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
     4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
    11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3
};

static USHORT s_6[64] = {
    12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
    10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
     9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
     4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13
};

static USHORT s_7[64] = {
     4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
    13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
     1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
     6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12
};

static USHORT s_8[64] = {
    13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
     1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
     7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
     2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
};

/* 転置(P)  */
static USHORT f_perm[32] = {
    15,  6, 19, 20,
    28, 11, 27, 16,
     0, 14, 22, 25,
     4, 17, 30,  9,
     1,  7, 23, 13,
    31, 26,  2,  8,
    18, 12, 29,  5,
    21, 10,  3, 24
};


/****************************************************************************
 *									    *
 *	暗号化関数							    *
 *	void	des_enc(plane,r_key,cipher)				    *
 *	ULONG	*plane	: 平文(64 bit)		(input)			    *
 *	UCHAR	*r_key	: 拡大鍵(48 * 16 bit)	(input)			    *
 *	ULONG	*cipher	: 暗号文(64 bit)	(output)		    *
 *									    *
 *	機能	: 平文及び拡大鍵を入力とし、DESのアルゴリズムに基づき暗号文 *
 *		  を生成する。						    *
 *	戻り値	: なし							    *
 *									    *
 ****************************************************************************/
void	des_enc(plane,r_key,cipher)
ULONG	*plane;	    /* 平文(64 bit)		*/
UCHAR	*r_key;	    /* 拡大鍵(48 * 16 bit)	*/
ULONG	*cipher;    /* 暗号文(64 bit)		*/
{
    ULONG   l_text,r_text,
	    l_work,r_work,tmp_text,c_text[2];
    int	    loop;


    l_text = *plane;
    r_text = *(plane+1);

/* 初期転置(IP)	*/
    l_work = 0x00000000L;
    r_work = 0x00000000L;
    for(loop = 0;loop < 64;loop++)
    {
	if(loop < 32)
	{
	    if(ip_1[loop] < 32)
	    {
		if(l_text & (0x80000000L >> ip_1[loop]))
		    l_work |= 0x80000000L >> loop;
	    }
	    else
	    {
		if(r_text & (0x80000000L >> (ip_1[loop] - 32)))
		    l_work |= 0x80000000L >> loop;
	    }
	}
	else
	{
	    if(ip_1[loop] < 32)
	    {
		if(l_text & (0x80000000L >> ip_1[loop]))
		    r_work |= 0x80000000L >> (loop - 32);
	    }
	    else
	    {
		if(r_text & (0x80000000L >> (ip_1[loop] - 32)))
		    r_work |= 0x80000000L >> (loop - 32);
	    }
	}
    }
    l_text = l_work;
    r_text = r_work;

#ifdef DEBUG
printf("l_text: %08x,r_text: %08x.\n",l_text,r_text);
#endif

/* 段関数 f(R,K)    */
    for(loop = 0;loop < ROUND;loop++,r_key+=6)
    {
	tmp_text = l_text;
	l_text = r_text;
	r_text = (ULONG)tmp_text ^ (ULONG)round_func(r_text,r_key);
#ifdef DEBUG
printf("round[%d] l_text: %08x, r_text: %08x.\n",loop,l_text,r_text);
#endif
    }
    tmp_text = l_text;
    l_text = r_text;
    r_text = tmp_text;

/* 最終転置(IP^-1)  */
    l_work = 0x00000000L;
    r_work = 0x00000000L;
    for(loop = 0;loop < 64;loop++)
    {
	if(loop < 32)
	{
	    if(ip_2[loop] < 32)
	    {
		if(l_text & (0x80000000L >> ip_2[loop]))
		    l_work |= 0x80000000L >> loop;
	    }
	    else
	    {
		if(r_text & (0x80000000L >> (ip_2[loop] - 32)))
		    l_work |= 0x80000000L >> loop;
	    }
	}
	else
	{
	    if(ip_2[loop] < 32)
	    {
		if(l_text & (0x80000000L >> ip_2[loop]))
		    r_work |= 0x80000000L >> (loop - 32);
	    }
	    else
	    {
		if(r_text & (0x80000000L >> (ip_2[loop] - 32)))
		    r_work |= 0x80000000L >> (loop - 32);
	    }
	}
    }
    /*l_text = l_work;
    r_text = r_work;*/

#ifdef DEBUG
printf("l_text: %08x,r_text: %08x.\n",l_text,r_text);
#endif

    *cipher = l_work;
    *(cipher+1) = r_work;

    return;
}


/****************************************************************************
 *									    *
 *	段関数(f(R,K)							    *
 *	ULONG	round_func(text,key)					    *
 *	ULONG	text	: 入力文(32 bit)	(input)			    *
 *	UCHAR	*key	: 拡大鍵(48 bit)	(input)			    *
 *									    *
 *	機能	: 入力文及び拡大鍵を入力とし、DESの段関数の出力を返す。	    *
 *	戻り値	: 段関数(f(R,K))の出力値				    *
 *									    *
 ****************************************************************************/
ULONG	round_func(text,key)
ULONG	text;
UCHAR	*key;
{
    UCHAR   e_val[6],k_work[6];
    ULONG   t_work,t_val;
    int	    loop;


    t_work = text;

/* 拡大転置(E) 32bit(4byte) -> 48bit(6byte)		*/
    memset(e_val,0x00L,6);
    for(loop = 0;loop < 48;loop++)
	if(t_work & (0x80000000L >> f_expand[loop]))
	    e_val[loop / 8] |= 0x80L >> (loop % 8);

#ifdef DEBUG
printf("Expand: %02x%02x%02x%02x%02x%02x.\n",
    e_val[0],e_val[1],e_val[2],e_val[3],e_val[4],e_val[5]);
#endif

/* 鍵との排他的論理和演算 48bit(6byte) -> 48bit(6byte)	*/
    memset(k_work,0x00L,6);
    for(loop = 0;loop < 6;loop++,key++)
	k_work[loop] = e_val[loop] ^ *key;

#ifdef DEBUG
printf("Key EXORed: %02x%02x%02x%02x%02x%02x.\n",
    k_work[0],k_work[1],k_work[2],k_work[3],k_work[4],k_work[5]);
#endif

/* S-box 48bit(6byte) -> 32bit(4byte)			*/
    t_work = s_box_func(k_work);

#ifdef DEBUG
printf("s-box value: %08x.\n",t_work);
#endif

/* 転置(P) 32bit(4byte) -> 32bit(4byte)			*/
    t_val = 0x00000000L;
    for(loop = 0;loop < 32;loop++)
	if(t_work & (0x80000000L >> f_perm[loop]))
	    t_val |= 0x80000000L >> loop;

#ifdef DEBUG
printf("Round func value: %08x.\n",t_val);
#endif

    return(t_val);
}


/****************************************************************************
 *									    *
 *	段関数(f(R,K)							    *
 *	ULONG	s_box_func(i_data)					    *
 *	UCHAR	*i_data	: 入力文(48 bit)	(input)			    *
 *									    *
 *	機能	: 入力文を入力とし、DESのS-Boxの出力値(32 bit)を返す。	    *
 *	戻り値	: S-Boxの出力値						    *
 *									    *
 ****************************************************************************/
ULONG	s_box_func(i_data)
UCHAR	*i_data;
{
    int	    loop;
    UCHAR   work[6],s_work[8],val;
    ULONG   fval;


    for(loop = 0;loop < 6;loop++,i_data++) work[loop] = *i_data;

/* S1 */
    s_work[0] = (work[0] >> 2) & 0x3fL;
    val = (s_work[0] >> 1) & 0x0fL;
    if(s_work[0] & 0x20L)
    {
	if(s_work[0] & 0x01L) s_work[0] = s_1[val+48];
	else s_work[0] = s_1[val+32];
    }
    else
    {
	if(s_work[0] & 0x01L) s_work[0] = s_1[val+16];
	else s_work[0] = s_1[val];
    }

/* S2 */
    s_work[1] = ((work[0] << 4) | (work[1] >> 4)) & 0x3fL;
    val = (s_work[1] >> 1) & 0x0fL;
    if(s_work[1] & 0x20L)
    {
	if(s_work[1] & 0x01L) s_work[1] = s_2[val+48];
	else s_work[1] = s_2[val+32];
    }
    else
    {
	if(s_work[1] & 0x01L) s_work[1] = s_2[val+16];
	else s_work[1] = s_2[val];
    }

/* S3 */
    s_work[2] = ((work[1] << 2) | (work[2] >> 6)) & 0x3fL;
    val = (s_work[2] >> 1) & 0x0fL;
    if(s_work[2] & 0x20L)
    {
	if(s_work[2] & 0x01L) s_work[2] = s_3[val+48];
	else s_work[2] = s_3[val+32];
    }
    else
    {
	if(s_work[2] & 0x01L) s_work[2] = s_3[val+16];
	else s_work[2] = s_3[val];
    }

/* S4 */
    s_work[3] = work[2] & 0x3fL;
    val = (s_work[3] >> 1) & 0x0fL;
    if(s_work[3] & 0x20L)
    {
	if(s_work[3] & 0x01L) s_work[3] = s_4[val+48];
	else s_work[3] = s_4[val+32];
    }
    else
    {
	if(s_work[3] & 0x01L) s_work[3] = s_4[val+16];
	else s_work[3] = s_4[val];
    }

/* S5 */
    s_work[4] = (work[3] >> 2) & 0x3fL;
    val = (s_work[4] >> 1) & 0x0fL;
    if(s_work[4] & 0x20L)
    {
	if(s_work[4] & 0x01L) s_work[4] = s_5[val+48];
	else s_work[4] = s_5[val+32];
    }
    else
    {
	if(s_work[4] & 0x01L) s_work[4] = s_5[val+16];
	else s_work[4] = s_5[val];
    }

/* S6 */
    s_work[5] = ((work[3] << 4) | (work[4] >> 4)) & 0x3fL;
    val = (s_work[5] >> 1) & 0x0fL;
    if(s_work[5] & 0x20L)
    {
	if(s_work[5] & 0x01L) s_work[5] = s_6[val+48];
	else s_work[5] = s_6[val+32];
    }
    else
    {
	if(s_work[5] & 0x01L) s_work[5] = s_6[val+16];
	else s_work[5] = s_6[val];
    }

/* S7 */
    s_work[6] = ((work[4] << 2) | (work[5] >> 6)) & 0x3fL;
    val = (s_work[6] >> 1) & 0x0fL;
    if(s_work[6] & 0x20L)
    {
	if(s_work[6] & 0x01L) s_work[6] = s_7[val+48];
	else s_work[6] = s_7[val+32];
    }
    else
    {
	if(s_work[6] & 0x01L) s_work[6] = s_7[val+16];
	else s_work[6] = s_7[val];
    }

/* S8 */
    s_work[7] = work[5] & 0x3fL;
    val = (s_work[7] >> 1) & 0x0fL;
    if(s_work[7] & 0x20L)
    {
	if(s_work[7] & 0x01L) s_work[7] = s_8[val+48];
	else s_work[7] = s_8[val+32];
    }
    else
    {
	if(s_work[7] & 0x01L) s_work[7] = s_8[val+16];
	else s_work[7] = s_8[val];
    }

    fval = (s_work[0] << 28) | (s_work[1] << 24)
	    | (s_work[2] << 20) | (s_work[3] << 16)
	    | (s_work[4] << 12) | (s_work[5] << 8)
	    | (s_work[6] << 4)  | s_work[7];

    return(fval);
}


/****************************************************************************
 *									    *
 *	復号関数							    *
 *	void	des_dec(cipher,r_key,plane)				    *
 *	ULONG	*cipher	: 暗号文(64 bit)	(output)		    *
 *	UCHAR	*r_key	: 拡大鍵(48 * 16 bit)	(input)			    *
 *	ULONG	*plane	: 平文(64 bit)		(input)			    *
 *									    *
 *	機能	: 暗号文及び拡大鍵を入力とし、DESのアルゴリズムに基づき平文 *
 *		  を生成する。						    *
 *	戻り値	: なし							    *
 *									    *
 ****************************************************************************/
void	des_dec(cipher,r_key,plane)
ULONG	*cipher;
UCHAR	*r_key;
ULONG	*plane;
{
    ULONG   l_text,r_text,
	    l_work,r_work,tmp_text;
    UCHAR   *tmp_k;
    int	    loop;


    l_text = *cipher;
    r_text = *(cipher+1);


/* 初期転置(IP)	*/
    l_work = 0x00000000L;
    r_work = 0x00000000l;
    for(loop = 0;loop < 64;loop++)
    {
	if(loop < 32)
	{
	    if(ip_1[loop] < 32)
	    {
		if(l_text & (0x80000000L >> ip_1[loop]))
		    l_work |= 0x80000000L >> loop;
	    }
	    else
	    {
		if(r_text & (0x80000000L >> (ip_1[loop] - 32)))
		    l_work |= 0x80000000L >> loop;
	    }
	}
	else
	{
	    if(ip_1[loop] < 32)
	    {
		if(l_text & (0x80000000L >> ip_1[loop]))
		    r_work |= 0x80000000l >> (loop - 32);
	    }
	    else
	    {
		if(r_text & (0x80000000L >> (ip_1[loop] - 32)))
		    r_work |= 0x80000000L >> (loop - 32);
	    }
	}
    }
    l_text = l_work;
    r_text = r_work;

/* 段関数 f(R,K)    */
    tmp_k = r_key+90;
    for(loop = 0;loop < ROUND;loop++,tmp_k-=6)
    {
	tmp_text = l_text;
	l_text = r_text;
	r_text = tmp_text ^ round_func(r_text,tmp_k);

#ifdef DEBUG
printf("round[%d] l_text: %08x, r_text: %08x.\n",loop,l_text,r_text);
#endif

    }
    tmp_text = l_text;
    l_text = r_text;
    r_text = tmp_text;

/* 最終転置(IP^-1)  */
    l_work = 0x00000000L;
    r_work = 0x00000000L;
    for(loop = 0;loop < 64;loop++)
    {
	if(loop < 32)
	{
	    if(ip_2[loop] < 32)
	    {
		if(l_text & (0x80000000L >> ip_2[loop]))
		    l_work |= 0x80000000L >> loop;
	    }
	    else
	    {
		if(r_text & (0x80000000L >> (ip_2[loop] - 32)))
		    l_work |= 0x80000000L >> loop;
	    }
	}
	else
	{
	    if(ip_2[loop] < 32)
	    {
		if(l_text & (0x80000000L >> ip_2[loop]))
		    r_work |= 0x80000000L >> (loop - 32);
	    }
	    else
	    {
		if(r_text & (0x80000000L >> (ip_2[loop] - 32)))
		    r_work |= 0x80000000L >> (loop - 32);
	    }
	}
    }
    l_text = l_work;
    r_text = r_work;

    *plane = l_text;
    *(plane+1) = r_text;

    return;
}


/****************************************************************************
 *									    *
 *	鍵スケジュール関数						    *
 *	void	key_schedule(key)					    *
 *	UCHAR	*key	: 暗号化鍵(64 bit)	(input)			    *
 *									    *
 *	機能	: 暗号化鍵を入力とし、DESの鍵スケジュールアルゴリズムに基づ *
 *		  き段毎の拡大鍵を生成し、大域変数(EX_KEY[96])に格納する。  *
 *	戻り値	: なし							    *
 *									    *
 ****************************************************************************/
void	key_schedule(key,ex_key)
UCHAR	*key,*ex_key;
{
    UCHAR   r_key[6],*k_p;
    ULONG   k_work1,k_work2,
	    c_key,d_key,
	    c_tmp,d_tmp;
    int	    loop,loop2,len,strcnt;


    k_work1 = 0x00000000L;
    k_work2 = 0x00000000L;
    k_work1 |= ((ULONG)*key << 24) | ((ULONG)*(key+1) << 16)
	    | ((ULONG)*(key+2) << 8) | (ULONG)*(key+3);
    k_work2 |= ((ULONG)*(key+4) << 24) | ((ULONG)*(key+5) << 16)
	    | ((ULONG)*(key+6) << 8) | (ULONG)*(key+7);

/* 鍵の縮約転置(PC-1)	*/
    c_key = 0x00000000L;
    d_key = 0x00000000L;
    for(loop = 0;loop < 56;loop++)
    {
	if(loop < 28)
	{
	    if(pc_1[loop] < 32)
	    {
		if(k_work1 & (0x80000000L >> pc_1[loop]))
		    c_key |= 0x80000000L >> loop;
	    }
	    else
	    {
		if(k_work2 & (0x80000000L >> pc_1[loop] - 32))
		    c_key |= 0x80000000L >> loop;
	    }
	}
	else
	{
	    if(pc_1[loop] < 32)
	    {
		if(k_work1 & (0x80000000L >> pc_1[loop]))
		    d_key |= 0x80000000L >> (loop - 28);
	    }
	    else
	    {
		if(k_work2 & (0x80000000L >> pc_1[loop] - 32))
		    d_key |= 0x80000000L >> (loop - 28);
	    }
	}
    }

#ifdef DEBUG
printf("c: %08x,d: %08x.\n",c_key,d_key);
#endif

    k_p = ex_key;
    for(loop = 0;loop < 16;loop++,k_p+=6)
    {
/* 鍵のシフト	    */
	c_tmp = 0x00000000L;
	d_tmp = 0x00000000L;
	c_tmp = (c_key << k_rot[loop]) | (c_key >> (28-k_rot[loop])); 
	d_tmp = (d_key << k_rot[loop]) | (d_key >> (28-k_rot[loop])); 

/* 鍵の縮約型転置(PC-2)	*/
	memset(r_key,0x00L,6);
	for(loop2 = 0;loop2 < 48;loop2++)
	{
	    if(pc_2[loop2] < 28)
	    {
		if(c_tmp & (0x80000000L >> pc_2[loop2]))
		    r_key[loop2 / 8] |= 0x80 >> (loop2 % 8);
	    }
	    else
	    {
		if(d_tmp & (0x80000000L >> pc_2[loop2]-28))
		    r_key[loop2 / 8] |= (0x80 >> (loop2 % 8));
	    }
	}

	memcpy(k_p,r_key,6);

#ifdef DEBUG
printf("key[%d]: %02x %02x %02x %02x %02x %02x\n",
    loop,r_key[0],r_key[1],r_key[2],r_key[3],r_key[4],r_key[5]) ;
#endif
    }
}

#if 0
/* 動作確認用main   */
void	main(void)
{
    ULONG   p_text[2],c_text[2];
    UCHAR   key[8] = 
	    /*{ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};*/
	    /*{ 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};*/
	    { 0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef};
	    /*{ 0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55};*/
	    /*{ 0x03,0x96,0x49,0xc5,0x39,0x31,0x39,0x65};*/
    int	    loop;


    /*p_text[0] = 0x00000000;
    p_text[1] = 0x00000000;*/
    p_text[0] = 0x01234567L;
    p_text[1] = 0x89abcde7L;
    /*p_text[0] = 0xffffffff;
    p_text[1] = 0xffffffff;*/

    /*printf("plane: %08x%08x.\n",p_text[0],p_text[1]);*/


    for(loop =0;loop < 1024*10;loop++)
    {
	key_schedule(key);
	des_enc(p_text,EX_KEY,c_text);
	/*printf("cipher: %08x%08x.\n\n",c_text[0],c_text[1]);*/

	/*p_text[0] = c_text[0];
	p_text[1] = c_text[1];*/
    }

    /*des_dec(c_text,EX_KEY,p_text);
    printf("plane: %08x%08x.\n",p_text[0],p_text[1]);*/

}
#endif

