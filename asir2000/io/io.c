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
 * e-mail at risa-admin@sec.flab.fujitsu.co.jp of the detailed specification
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
 * $OpenXM: OpenXM_contrib2/asir2000/io/io.c,v 1.11 2003/03/07 03:12:28 noro Exp $ 
*/
#include <stdio.h>
#include "ca.h"
#include "parse.h"
#if defined(VISUAL) || defined(MPI)
#include "wsio.h"
#endif

extern int little_endian,lib_ox_need_conv;
extern int ox_do_copy, ox_do_count, ox_count_length, ox_file_io, ox_need_conv;
extern char *ox_copy_bptr;

/* XXX */
void write_cmo(pointer,Obj);

void reset_io()
{
	ox_file_io = 0;
}

void endian_init()
{
	unsigned int et = 0xff;

	if ( *((char *)&et) )
		little_endian = 1;
	else
		little_endian = 0;
	ox_need_conv = 1;
}

int countobj(Obj p)
{
	ox_count_length = 0;
	ox_do_count = 1; saveobj(0,p); ox_do_count = 0;
	return ox_count_length;
}

int count_as_cmo(Obj p)
{
	ox_count_length = 0;
	ox_do_count = 1; write_cmo(0,p); ox_do_count = 0;
	return ox_count_length;
}

int countvl(VL vl)
{
	ox_count_length = 0;
	ox_do_count = 1; savevl(0,vl); ox_do_count = 0;
	return ox_count_length;
}

void ox_copy_init(char *s)
{
	ox_copy_bptr = s;
}

#if !defined(VISUAL)
/*
 * library mode functions
 * byte order is controlled by lib_ox_need_conv.
 */

void ox_obj_to_buf_as_cmo(Obj p)
{
	ox_need_conv = lib_ox_need_conv;
	ox_do_copy = 1; write_cmo(0,p); ox_do_copy = 0;
}

void ox_buf_to_obj_as_cmo(Obj *p)
{
	ox_need_conv = lib_ox_need_conv;
	ox_do_copy = 1; read_cmo(0,p); ox_do_copy = 0;
}

void ox_vl_to_buf(VL vl)
{
	ox_do_copy = 1; savevl(0,vl); ox_do_copy = 0;
}
#endif

int gen_fread (char *ptr,int size,int nitems,FILE *stream)
{
	int n;

	if ( ox_do_copy ) {
		n = size*nitems;
		memcpy(ptr,ox_copy_bptr,n);
		ox_copy_bptr += n;
		/* dummy return */
		return 0;
	} else {
#if defined(VISUAL)
		if ( _fileno(stream) < 0 )
			n = cread(ptr,size,nitems,(STREAM *)stream);
		else
#elif defined(MPI)
		if ( (char)fileno(stream) < 0 )
			n = cread(ptr,size,nitems,(STREAM *)stream);
		else
#endif
		n = fread(ptr,size,nitems,stream);
		if ( !n ) {
			ExitAsir();
			/* NOTREACHED */
			return 0;
		} else
			return n;
	}
}

int gen_fwrite (char *ptr,int size,int nitems,FILE *stream)
{
	int n;

	if ( ox_do_count ) {
		ox_count_length += size*nitems;
		/* dummy return  */
		return 0;
	} else if ( ox_do_copy ) {
		n = size*nitems;
		memcpy(ox_copy_bptr,ptr,n);
		ox_copy_bptr += n;
		/* dummy return  */
		return 0;
	} else
#if defined(VISUAL)
	if ( _fileno(stream) < 0 )
		return cwrite(ptr,size,nitems,(STREAM *)stream);
	else
#elif defined(MPI)
	if ( (char)fileno(stream) < 0 )
		return cwrite(ptr,size,nitems,(STREAM *)stream);
	else
#endif
		return fwrite(ptr,size,nitems,stream);
}

void write_char(FILE *f,unsigned char *p)
{
	gen_fwrite(p,sizeof(unsigned char),1,f);
}

void write_short(FILE *f,unsigned short *p)
{
	unsigned short t;

	if ( little_endian && (ox_file_io || ox_need_conv) ) {
		t = htons(*p);
		gen_fwrite((char *)&t,sizeof(unsigned short),1,f);
	} else
		gen_fwrite((char *)p,sizeof(unsigned short),1,f);
}

void write_int(FILE *f,unsigned int *p)
{
	unsigned int t;

	if ( little_endian && (ox_file_io || ox_need_conv) ) {
		t = htonl(*p);
		gen_fwrite((char *)&t,sizeof(unsigned int),1,f);
	} else
		gen_fwrite((char *)p,sizeof(unsigned int),1,f);
}

#if defined(VISUAL) && defined(DES_ENC)
int des_encryption;
static unsigned char asir_deskey[8] = {0xc7,0xe0,0xfc,0xb5,0xc3,0xad,0x8e,0x3a};
static unsigned char deskey_string[96];

void init_deskey()
{
	static int deskey_initialized = 0;

	if ( !deskey_initialized ) {
		key_schedule(asir_deskey,deskey_string);
		deskey_initialized = 1;
	}
}
#endif

void write_intarray(FILE *f,unsigned int *p,int l)
{
	int i;
	unsigned int t;
#if defined(VISUAL) && defined(DES_ENC)
	int l2;
	unsigned int plain[2],encrypted[2];

	if ( des_encryption ) {
		l2 = l>>1;
		for ( i = 0; i < l2; i++ ) {
			plain[0] = *p++;
			plain[1] = *p++;
			des_enc(plain,deskey_string,encrypted);
			encrypted[0] = htonl(encrypted[0]);
			encrypted[1] = htonl(encrypted[1]);
			gen_fwrite((char *)encrypted,sizeof(unsigned int),2,f);
		}
		if ( (l2<<1) < l ) {
			plain[0] = *p;
			plain[1] = 0;
			des_enc(plain,deskey_string,encrypted);
			encrypted[0] = htonl(encrypted[0]);
			encrypted[1] = htonl(encrypted[1]);
			gen_fwrite((char *)encrypted,sizeof(unsigned int),2,f);
		}
	} else
#endif
	if ( little_endian && (ox_file_io || ox_need_conv) )
		for ( i = 0; i < l; i++, p++) {
			t = htonl(*p);
			gen_fwrite((char *)&t,sizeof(unsigned int),1,f);
		}
	else
		gen_fwrite((char *)p,sizeof(unsigned int),l,f);
}

#if defined(LONG_IS_64BIT)
void write_longarray(FILE *f,unsigned long *p,int l)
{
	int i;
	unsigned long w;
	unsigned int hi,lo;

	if ( little_endian && (ox_file_io || ox_need_conv) )
		for ( i = 0; i < l; i++, p++) {
			w = *p; hi = w>>32; lo = w&0xffffffff;
			hi = htonl(hi); lo = htonl(lo);
			gen_fwrite(&hi,sizeof(unsigned int),1,f);
			gen_fwrite(&lo,sizeof(unsigned int),1,f);
		}
	else
		gen_fwrite(p,sizeof(unsigned long),l,f);
}
#endif

void write_double(FILE *f,double *p)
{
	unsigned int t;

	if ( little_endian && (ox_file_io || ox_need_conv) ) {
		t = htonl(((unsigned int *)p)[1]);
		gen_fwrite((char *)&t,sizeof(unsigned int),1,f);
		t = htonl(((unsigned int *)p)[0]);
		gen_fwrite((char *)&t,sizeof(unsigned int),1,f);
	} else
		gen_fwrite((char *)p,sizeof(double),1,f);
}

void write_string(FILE *f,unsigned char *p,int l)
{
	gen_fwrite(p,sizeof(unsigned char),l,f);
}

void read_char(FILE *f,unsigned char *p)
{
	gen_fread((char *)p,sizeof(unsigned char),1,f);
}

void read_short(FILE *f,unsigned short *p)
{
	gen_fread((char *)p,sizeof(unsigned short),1,f);
	if ( little_endian && (ox_file_io || ox_need_conv) )
		*p = ntohs(*p);
}

void read_int(FILE *f,unsigned int *p)
{
	gen_fread((char *)p,sizeof(unsigned int),1,f);
	if ( little_endian && (ox_file_io || ox_need_conv) )
		*p = ntohl(*p);
}

void read_intarray(FILE *f,unsigned int *p,int l)
{
	int i;
#if defined(VISUAL) && defined(DES_ENC)
	int l2;
	unsigned int plain[2],encrypted[2];

	if ( des_encryption ) {
		l2 = l>>1;
		for ( i = 0; i < l2; i++ ) {
			gen_fread((char *)encrypted,sizeof(unsigned int),2,f);
			encrypted[0] = ntohl(encrypted[0]);
			encrypted[1] = ntohl(encrypted[1]);
			des_dec(encrypted,deskey_string,plain);
			*p++ = plain[0];
			*p++ = plain[1];
		}
		if ( (l2<<1) < l ) {
			gen_fread((char *)encrypted,sizeof(unsigned int),2,f);
			encrypted[0] = ntohl(encrypted[0]);
			encrypted[1] = ntohl(encrypted[1]);
			des_dec(encrypted,deskey_string,plain);
			*p = plain[0];
		}
	} else
#endif
	{
		gen_fread((char *)p,sizeof(unsigned int),l,f);
		if ( little_endian && (ox_file_io || ox_need_conv) )
			for ( i = 0; i < l; i++, p++ )
				*p = ntohl(*p);
	}
}

#if defined(LONG_IS_64BIT)
void read_longarray(FILE *f,unsigned long *p,int l)
{
	int i;
	unsigned int hi,lo;

	if ( little_endian && (ox_file_io || ox_need_conv) ) {
		for ( i = l; i > 1; i -= 2, p++ ) {
			gen_fread(&hi,sizeof(unsigned int),1,f);
			gen_fread(&lo,sizeof(unsigned int),1,f);
			hi = ntohl(hi); lo = ntohl(lo);
			*p = (((unsigned long)hi)<<32)|((unsigned long)lo);
		}
		if ( i == 1 ) {
			gen_fread(&hi,sizeof(unsigned int),1,f);
			hi = ntohl(hi); *p = (((unsigned long)hi)<<32);
		}
	} else
		gen_fread(p,sizeof(unsigned int),l,f);
}
#endif

void read_string(FILE *f,unsigned char *p,int l)
{
	gen_fread((char *)p,sizeof(unsigned char),l,f);
}

void read_double(FILE *f,double *p)
{
	unsigned int t;

	if ( little_endian && (ox_file_io || ox_need_conv) ) {
		gen_fread((char *)&t,sizeof(unsigned int),1,f);
		((unsigned int *)p)[1] = ntohl(t);
		gen_fread((char *)&t,sizeof(unsigned int),1,f);
		((unsigned int *)p)[0] = ntohl(t);
	} else
		gen_fread((char *)p,sizeof(double),1,f);
}
