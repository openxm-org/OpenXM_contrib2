/* $OpenXM: OpenXM_contrib2/asir2000/io/io.c,v 1.2 2000/01/18 05:55:07 noro Exp $ */
#include <stdio.h>
#include "ca.h"
#if defined(VISUAL) || MPI
#include "wsio.h"
#endif

extern int little_endian,lib_ox_need_conv;
extern int ox_do_copy, ox_do_count, ox_count_length, ox_file_io, ox_need_conv;
extern char *ox_copy_bptr;

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

int countobj(p)
Obj p;
{
	ox_count_length = 0;
	ox_do_count = 1; saveobj(0,p); ox_do_count = 0;
	return ox_count_length;
}

int count_as_cmo(p)
Obj p;
{
	ox_count_length = 0;
	ox_do_count = 1; write_cmo(0,p); ox_do_count = 0;
	return ox_count_length;
}

int countvl(vl)
VL vl;
{
	ox_count_length = 0;
	ox_do_count = 1; savevl(0,vl); ox_do_count = 0;
	return ox_count_length;
}

void ox_copy_init(s)
char *s;
{
	ox_copy_bptr = s;
}

/*
 * library mode functions
 * byte order is controlled by lib_ox_need_conv.
 */

void ox_obj_to_buf_as_cmo(p)
Obj p;
{
	ox_need_conv = lib_ox_need_conv;
	ox_do_copy = 1; write_cmo(0,p); ox_do_copy = 0;
}

void ox_buf_to_obj_as_cmo(p)
Obj *p;
{
	ox_need_conv = lib_ox_need_conv;
	ox_do_copy = 1; read_cmo(0,p); ox_do_copy = 0;
}

void ox_vl_to_buf(vl)
VL vl;
{
	ox_do_copy = 1; savevl(0,vl); ox_do_copy = 0;
}

int gen_fread (ptr,size,nitems,stream)
char *ptr;
int size,nitems;
FILE *stream;
{
	int n;

	if ( ox_do_copy ) {
		n = size*nitems;
		memcpy(ptr,ox_copy_bptr,n);
		ox_copy_bptr += n;
	} else {
#if defined(VISUAL)
		if ( _fileno(stream) < 0 )
			n = cread(ptr,size,nitems,(STREAM *)stream);
		else
#elif MPI
		if ( (char)fileno(stream) < 0 )
			n = cread(ptr,size,nitems,(STREAM *)stream);
		else
#endif
		n = fread(ptr,size,nitems,stream);
		if ( !n )
			ExitAsir();
		else
			return n;
	}
}

int gen_fwrite (ptr,size,nitems,stream)
char *ptr;
int size,nitems;
FILE *stream;
{
	int n;

	if ( ox_do_count )
		ox_count_length += size*nitems;
	else if ( ox_do_copy ) {
		n = size*nitems;
		memcpy(ox_copy_bptr,ptr,n);
		ox_copy_bptr += n;
	} else
#if defined(VISUAL)
	if ( _fileno(stream) < 0 )
		return cwrite(ptr,size,nitems,(STREAM *)stream);
	else
#elif MPI
	if ( (char)fileno(stream) < 0 )
		return cwrite(ptr,size,nitems,(STREAM *)stream);
	else
#endif
		return fwrite(ptr,size,nitems,stream);
}

void write_char(f,p)
FILE *f;
unsigned char *p;
{
	gen_fwrite(p,sizeof(unsigned char),1,f);
}

void write_short(f,p)
FILE *f;
unsigned short *p;
{
	unsigned short t;

	if ( little_endian && (ox_file_io || ox_need_conv) ) {
		t = htons(*p);
		gen_fwrite(&t,sizeof(unsigned short),1,f);
	} else
		gen_fwrite(p,sizeof(unsigned short),1,f);
}

void write_int(f,p)
FILE *f;
unsigned int *p;
{
	unsigned int t;

	if ( little_endian && (ox_file_io || ox_need_conv) ) {
		t = htonl(*p);
		gen_fwrite(&t,sizeof(unsigned int),1,f);
	} else
		gen_fwrite(p,sizeof(unsigned int),1,f);
}

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

void write_intarray(f,p,l)
FILE *f;
unsigned int *p;
int l;
{
	int i;
	unsigned int t;
#if defined(VISUAL)
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
			gen_fwrite(encrypted,sizeof(unsigned int),2,f);
		}
		if ( (l2<<1) < l ) {
			plain[0] = *p;
			plain[1] = 0;
			des_enc(plain,deskey_string,encrypted);
			encrypted[0] = htonl(encrypted[0]);
			encrypted[1] = htonl(encrypted[1]);
			gen_fwrite(encrypted,sizeof(unsigned int),2,f);
		}
	} else
#endif
	if ( little_endian && (ox_file_io || ox_need_conv) )
		for ( i = 0; i < l; i++, p++) {
			t = htonl(*p);
			gen_fwrite(&t,sizeof(unsigned int),1,f);
		}
	else
		gen_fwrite(p,sizeof(unsigned int),l,f);
}

#if defined(LONG_IS_64BIT)
void write_longarray(f,p,l)
FILE *f;
unsigned long *p;
int l;
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

void write_double(f,p)
FILE *f;
double *p;
{
	unsigned int t;

	if ( little_endian && (ox_file_io || ox_need_conv) ) {
		t = htonl(((unsigned int *)p)[1]);
		gen_fwrite(&t,sizeof(unsigned int),1,f);
		t = htonl(((unsigned int *)p)[0]);
		gen_fwrite(&t,sizeof(unsigned int),1,f);
	} else
		gen_fwrite(p,sizeof(double),1,f);
}

void write_string(f,p,l)
FILE *f;
unsigned char *p;
{
	gen_fwrite(p,sizeof(unsigned char),l,f);
}

void read_char(f,p)
FILE *f;
unsigned char *p;
{
	gen_fread((char *)p,sizeof(unsigned char),1,f);
}

void read_short(f,p)
FILE *f;
unsigned short *p;
{
	gen_fread(p,sizeof(unsigned short),1,f);
	if ( little_endian && (ox_file_io || ox_need_conv) )
		*p = ntohs(*p);
}

void read_int(f,p)
FILE *f;
unsigned int *p;
{
	gen_fread(p,sizeof(unsigned int),1,f);
	if ( little_endian && (ox_file_io || ox_need_conv) )
		*p = ntohl(*p);
}

void read_intarray(f,p,l)
FILE *f;
unsigned int *p;
int l;
{
	int i;
	unsigned int t;
#if defined(VISUAL)
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
		gen_fread(p,sizeof(unsigned int),l,f);
		if ( little_endian && (ox_file_io || ox_need_conv) )
			for ( i = 0; i < l; i++, p++ )
				*p = ntohl(*p);
	}
}

#if defined(LONG_IS_64BIT)
void read_longarray(f,p,l)
FILE *f;
unsigned long *p;
int l;
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

void read_string(f,p,l)
FILE *f;
unsigned char *p;
{
	gen_fread((char *)p,sizeof(unsigned char),l,f);
}

void read_double(f,p)
FILE *f;
double *p;
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
