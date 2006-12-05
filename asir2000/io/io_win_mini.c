/* $OpenXM: OpenXM_contrib2/asir2000/io/io_win_mini.c,v 1.2 2006/09/29 09:02:49 noro Exp $ */

#include "ca.h"
#include "parse.h"
#include "ox.h"
#include "wsio.h"
#include "signal.h"

#define ISIZ sizeof(int)

#define _NEWNODE(n) ((n)=(NODE)malloc(sizeof(struct oNODE)))
#define _NEWSTR(l) ((l)=(STRING)malloc(sizeof(struct oSTRING)),OID(l)=O_STR)
#define _NEWUSINT(u) ((u)=(USINT)malloc(sizeof(struct oUSINT)),OID(u)=O_USINT)
#define _NEWERR(e) ((e)=(ERR)malloc(sizeof(struct oERR)),OID(e)=O_ERR)
#define _NEWLIST(l) ((l)=(LIST)malloc(sizeof(struct oLIST)),OID(l)=O_LIST)
#define _MKSTR(u,b) (_NEWSTR(u),(u)->body=(unsigned)(b))
#define _MKUSINT(u,b) (_NEWUSINT(u),(u)->body=(unsigned)(b))
#define _MKERR(e,b) (_NEWERR(e),(e)->body=(Obj)(b))
#define _MKLIST(l,b) (_NEWLIST(l),(l)->body=(Obj)(b))
#define _MKNODE(a,b,c) \
(_NEWNODE(a),(a)->body=(pointer)b,NEXT(a)=(NODE)(c))

int ox_usr1_sent, ox_int_received, critical_when_signal;
unsigned int ox_serial;
int ox_flushing;
int ox_batch;
int ox_check=1;
int ox_exchange_mathcap=0;
int little_endian=1;
int terminate;

static int available_cmo[] = {
	CMO_NULL, CMO_INT32, CMO_STRING,CMO_LIST,
	CMO_ERROR, CMO_ERROR2, CMO_ZERO,
	0
};

int ox_data_is_available(int s)
{
	return FP_DATA_IS_AVAILABLE(iofp[s].in);
}

void wait_for_data(int s)
{
	fd_set r;
	int sock;

	if ( !FP_DATA_IS_AVAILABLE(iofp[s].in) ) {
		sock = iofp[s].in->fildes;
		FD_ZERO(&r);
		FD_SET((unsigned int)sock,&r);
		select(0,&r,NULL,NULL,NULL);
	}
}

void ox_send_data(int s,pointer p)
{
	ERR err;

	if ( !valid_as_cmo(p) )
		return;
	ox_write_int(s,OX_DATA);
	ox_write_int(s,ox_serial++);
	ox_write_cmo(s,p);
	ox_flush_stream(s);
}

void ox_send_cmd(int s,int id)
{
	ox_write_int(s,OX_COMMAND);
	ox_write_int(s,ox_serial++);
	ox_write_int(s,id);
	ox_flush_stream(s);
}

void ox_send_sync(int s)
{
	ox_write_int(s,OX_SYNC_BALL);
	ox_write_int(s,ox_serial++);
	ox_flush_stream(s);
}

unsigned int ox_recv(int s, int *id, Obj *p)
{
	unsigned int cmd,serial;
	USINT ui;

	wait_for_data(s);
	if ( terminate ) return -1; 
	ox_read_int(s,id);
	ox_read_int(s,&serial);
	switch ( *id ) {
		case OX_COMMAND:
			ox_read_int(s,&cmd);
			_MKUSINT(ui,cmd);
			*p = (Obj)ui;
			break;
		case OX_DATA:
			ox_read_cmo(s,p);
			break;
		default:
			*p = 0;
			break;
	}
	return serial;
}

void ox_get_result(int s,Obj *rp)
{
	int id;
	Obj obj,r;
	int level;

	level = 0;
	r = 0;
	do {
		ox_recv(s,&id,&obj);
		if ( id == OX_COMMAND ) {
			switch ( ((USINT)obj)->body ) {
				case SM_beginBlock:
					level++;
					break;
				case SM_endBlock:
					level--;
			}
		} else 
			r = obj;
	} while ( level );
	*rp = r;
}

void ox_read_int(int s, int *n)
{
	read_int((FILE *)iofp[s].in,n);
}

void ox_read_cmo(int s, Obj *rp)
{
	read_cmo((FILE *)iofp[s].in,rp);
}

void ox_write_int(int s, int n)
{
	write_int((FILE *)iofp[s].out,&n);
}

void ox_write_cmo(int s, Obj obj)
{
	write_cmo((FILE *)iofp[s].out,obj);
}

void ox_flush_stream(int s)
{
	if ( ox_batch )
		return;
	if ( _fileno(&iofp[s].out->fp) < 0 )
		cflush(iofp[s].out);
	else
		fflush((FILE *)iofp[s].out);
}

void ox_flush_stream_force(int s)
{
	if ( _fileno(&iofp[s].out->fp) < 0 )
		cflush(iofp[s].out);
	else
		fflush((FILE *)iofp[s].out);
}

/* CMO I/O functions */

int valid_as_cmo(Obj obj)
{
	NODE m;

	if ( !obj )
		return 1;
	switch ( OID(obj) ) {
		case O_STR: case O_ERR: case O_USINT:
			return 1;
		default:
			return 0;
	}
}

void write_cmo(FILE *s,Obj obj)
{
	int r;
	char errmsg[BUFSIZ];

	if ( !obj ) {
		r = CMO_NULL; write_int(s,&r);
		return;
	}
	switch ( OID(obj) ) {
		case O_STR:
			write_cmo_string(s,(STRING)obj);
			break;
		case O_USINT:
			write_cmo_uint(s,(USINT)obj);
			break;
		case O_LIST:
			write_cmo_list(s,(LIST)obj);
			break;
		case O_ERR:
			write_cmo_error(s,(ERR)obj);
			break;
	}
}

int cmo_tag(Obj obj,int *tag)
{
	if ( !obj ) {
		*tag = CMO_NULL;
		return 1;
	}
	switch ( OID(obj) ) {
		case O_STR:
			*tag = CMO_STRING; break;
		case O_USINT:
			*tag = CMO_INT32; break;
		case O_LIST:
			*tag = CMO_LIST; break;
		case O_ERR:
			*tag = CMO_ERROR2; break;
		default:
			return 0;
	}
	return 1;
}

void write_cmo_uint(FILE *s,USINT ui)
{
	unsigned int r;

	r = CMO_INT32; write_int(s,&r);
	r = ui->body; write_int(s,&r);
}

void write_cmo_string(FILE *s,STRING str)
{
	int r;
	int size;
	char *p;

	r = CMO_STRING; write_int(s,&r);
	p = BDY(str);
	size = p ? strlen(p) : 0;
	write_int(s,&size);
	if ( size )
		write_string(s,p,size);
}

void write_cmo_list(FILE *s,LIST list)
{
	NODE m;
	int i,n,r;

	for ( n = 0, m = BDY(list); m; m = NEXT(m), n++ );
	r = CMO_LIST; write_int(s,&r);
	write_int(s,&n);
	for ( i = 0, m = BDY(list); i < n; i++, m = NEXT(m) )
		write_cmo(s,BDY(m));
}

void write_cmo_error(FILE *s,ERR e)
{
	int r;

	r = CMO_ERROR2; write_int(s,&r);
	write_cmo(s,BDY(e));
}

void read_cmo(FILE *s,Obj *rp)
{
	int id;
	STRING str;
	LIST list;
	USINT t;
	Obj obj;
	ERR e;

	read_int(s,&id);
	switch ( id ) {
	/* level 0 objects */
		case CMO_NULL:
			*rp = 0;
			break;
		case CMO_INT32:
			read_cmo_uint(s,&t); *rp = (Obj)t;
			break;
		case CMO_STRING:
			loadstring(s,&str); *rp = (Obj)str;
			break;
		case CMO_LIST:
			read_cmo_list(s,&list); *rp = (Obj)list;
			break;
		case CMO_ERROR:
			_MKERR(e,0); *rp = (Obj)e;
			break;
		case CMO_ERROR2:
			read_cmo(s,&obj); _MKERR(e,obj); *rp = (Obj)e;
			break;
		case CMO_ZERO:
			*rp = 0;
			break;
		default:
			_MKUSINT(t,id);
			t->id = O_VOID;
			*rp = (Obj)t;
			break;
	}
}

void read_cmo_uint(FILE *s,USINT *rp)
{
	unsigned int body;

	read_int(s,&body);
	_MKUSINT(*rp,body);
}

void read_cmo_list(FILE *s,Obj *rp)
{
	int len;
	Obj *w;
	int i;
	NODE n0,n1;
	LIST list;

	read_int(s,&len);
	w = (Obj *)ALLOCA(len*sizeof(Obj));
	for ( i = 0; i < len; i++ )
		read_cmo(s,&w[i]);		
	for ( i = len-1, n0 = 0; i >= 0; i-- ) {
		_MKNODE(n1,w[i],n0); n0 = n1;
	}
	_MKLIST(list,n0);
	*rp = (Obj)list;
}

void loadstring(FILE *s,STRING *p)
{
	char *t;

	loadstr(s,&t); _MKSTR(*p,t);
}

void loadstr(FILE *s,char **p)
{
	int len;
	char *t;

	read_int(s,&len);
	t = (char *)malloc(len+1);
	if ( len ) read_string(s,t,len);
	t[len] = 0;
	*p = t;
}

/* low level buffered I/O functions */

int gen_fread (char *ptr,int size,int nitems,FILE *stream)
{
	int n;

	if ( _fileno(stream) < 0 )
		n = cread(ptr,size,nitems,(STREAM *)stream);
	else
		n = fread(ptr,size,nitems,stream);
	if ( !n )
		return 0;
	else
		return n;
}

int gen_fwrite (char *ptr,int size,int nitems,FILE *stream)
{
	if ( _fileno(stream) < 0 )
		return cwrite(ptr,size,nitems,(STREAM *)stream);
	else
		return fwrite(ptr,size,nitems,stream);
}

void write_char(FILE *f,unsigned char *p)
{
	gen_fwrite(p,sizeof(unsigned char),1,f);
}

void write_short(FILE *f,unsigned short *p)
{
	gen_fwrite((char *)p,sizeof(unsigned short),1,f);
}

void write_int(FILE *f,unsigned int *p)
{
	gen_fwrite((char *)p,sizeof(unsigned int),1,f);
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
}

void read_int(FILE *f,unsigned int *p)
{
	gen_fread((char *)p,sizeof(unsigned int),1,f);
}

void read_string(FILE *f,unsigned char *p,int l)
{
	gen_fread((char *)p,sizeof(unsigned char),l,f);
}

/* dummy functions, alternative functions */

double get_current_time()
{
	return GetTickCount()/1000.0;
}

void shutdown_all(){}

