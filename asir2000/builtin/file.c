/* $OpenXM: OpenXM/src/asir99/builtin/file.c,v 1.2 1999/11/23 07:14:14 noro Exp $ */
#include "ca.h"
#include "parse.h"
#include "base.h"
#include "genpari.h"

#if defined(VISUAL)
#include <windows.h>
/* #define ECGEN_KEYNAME "SoftWare\\Fujitsu\\WinECgen\\1.00.000" */
#define ECGEN_KEYNAME "SoftWare\\Fujitsu\\FSEcParamGen\\V1.0L10"
#define ASIR_KEYNAME "SoftWare\\Fujitsu\\Asir\\1999.03.31"
#endif

void Pget_rootdir();
void Paccess(),Premove_file();
void Pbsave_enc(), Pbload_enc();

void Pload(), Pwhich(), Ploadfiles(), Poutput();
void Pbsave(), Pbload(), Pbload27();
void Pbsave_compat(), Pbload_compat();
void Pbsave_cmo(), Pbload_cmo();

extern int des_encryption;
extern char *asir_libdir;

struct ftab file_tab[] = {
	{"remove_file",Premove_file,1},
	{"access",Paccess,1},
	{"load",Pload,-1},
	{"which",Pwhich,1},
	{"loadfiles",Ploadfiles,1},
	{"output",Poutput,-1},
	{"bsave",Pbsave,2},
	{"bload",Pbload,1},
	{"get_rootdir",Pget_rootdir,0},
	{"bsave_enc",Pbsave_enc,2},
	{"bload_enc",Pbload_enc,1},
	{"bload27",Pbload27,1},
	{"bsave_compat",Pbsave_compat,2},
	{"bload_compat",Pbload_compat,1},
	{"bsave_cmo",Pbsave_cmo,2},
	{"bload_cmo",Pbload_cmo,1},
	{0,0,0},
};

void Pload(arg,rp)
NODE arg;
Q *rp;
{
	int ret = 0;
	char *name,*name0;
	char errbuf[BUFSIZ];

#if defined THINK_C
	if ( !arg ) {
		ret = finder_loadfile();
	} else
#endif
	if ( ARG0(arg) ) {
		switch (OID(ARG0(arg))) {
			case O_STR:
				name0 = BDY((STRING)ARG0(arg));
				searchasirpath(name0,&name);
				if ( !name ) {
					sprintf(errbuf,"load : \"%s\" not found in the search path",name0);
					error(errbuf);
				}
				ret = loadfile(name);
				if ( !ret ) {
					sprintf(errbuf,"load : \"%s\" could not be loaded",name);
					error(errbuf);
				}
				break;
			default:
				error("load : invalid argument");
				break;
		}
	}
	STOQ(ret,*rp);
}

void Pwhich(arg,rp)
NODE arg;
STRING *rp;
{
	char *name;
	STRING str;

	switch (OID(ARG0(arg))) {
		case O_STR:
			searchasirpath(BDY((STRING)ARG0(arg)),&name);
			break;
		default:
			name = 0;
			break;
	}
	if ( name ) {
		MKSTR(str,name); *rp = str;
	} else
		*rp = 0;
}

void Ploadfiles(arg,rp)
NODE arg;
Q *rp;
{
	int ret;

	if ( ARG0(arg) )
		if ( OID(ARG0(arg)) != O_LIST )
			ret = 0;
		else
			ret = loadfiles(BDY((LIST)ARG0(arg)));
	else
		ret = 0;
	STOQ(ret,*rp);
}

void Poutput(arg,rp)
NODE arg;
Q *rp;
{
#if PARI
	extern FILE *outfile;
#endif
	FILE *fp;

	fflush(asir_out);
	if ( asir_out != stdout )
		fclose(asir_out);
	switch ( argc(arg) ) {
		case 0:
			fp = stdout; break;
		case 1:
			asir_assert(ARG0(arg),O_STR,"output");
			fp = fopen(((STRING)ARG0(arg))->body,"a+");
			if ( !fp )
				error("output : invalid filename");
			break;
	}
#if PARI
	pari_outfile =
#endif
	asir_out = fp;
	*rp = ONE;
}

extern int ox_file_io;

void Pbsave(arg,rp)
NODE arg;
Q *rp;
{
	FILE *fp;
	VL vl,t;

	asir_assert(ARG1(arg),O_STR,"bsave");
	get_vars_recursive(ARG0(arg),&vl);
	for ( t = vl; t; t = NEXT(t) )
		if ( t->v->attr == (pointer)V_UC )
			error("bsave : not implemented");
#if defined(THINK_C) || defined(VISUAL)
	fp = fopen(BDY((STRING)ARG1(arg)),"wb");
#else
	fp = fopen(BDY((STRING)ARG1(arg)),"w");
#endif
	if ( !fp )
		error("bsave : invalid filename");
	ox_file_io = 1; /* network byte order is used */
	savevl(fp,vl);
	saveobj(fp,ARG0(arg));
	fclose(fp);
	ox_file_io = 0;
	*rp = ONE;
}

void Pbload(arg,rp)
NODE arg;
Obj *rp;
{
	FILE *fp;

	asir_assert(ARG0(arg),O_STR,"bload");
#if defined(THINK_C) || defined(VISUAL)
	fp = fopen(BDY((STRING)ARG0(arg)),"rb");
#else
	fp = fopen(BDY((STRING)ARG0(arg)),"r");
#endif
	if ( !fp )
		error("bload : invalid filename");
	ox_file_io = 1; /* network byte order is used */
	loadvl(fp);
	loadobj(fp,rp);
	fclose(fp);
	ox_file_io = 0;
}

void Pbsave_cmo(arg,rp)
NODE arg;
Q *rp;
{
	FILE *fp;
	VL vl,t;

	asir_assert(ARG1(arg),O_STR,"bsave_cmo");
#if defined(THINK_C) || defined(VISUAL)
	fp = fopen(BDY((STRING)ARG1(arg)),"wb");
#else
	fp = fopen(BDY((STRING)ARG1(arg)),"w");
#endif
	if ( !fp )
		error("bsave_cmo : invalid filename");
	ox_file_io = 1; /* network byte order is used */
	write_cmo(fp,ARG0(arg));
	fclose(fp);
	ox_file_io = 0;
	*rp = ONE;
}

void Pbload_cmo(arg,rp)
NODE arg;
Obj *rp;
{
	FILE *fp;

	asir_assert(ARG0(arg),O_STR,"bload_cmo");
#if defined(THINK_C) || defined(VISUAL)
	fp = fopen(BDY((STRING)ARG0(arg)),"rb");
#else
	fp = fopen(BDY((STRING)ARG0(arg)),"r");
#endif
	if ( !fp )
		error("bload_cmo : invalid filename");
	ox_file_io = 1; /* network byte order is used */
	read_cmo(fp,rp);
	fclose(fp);
	ox_file_io = 0;
}

#if defined(VISUAL)
void get_rootdir(name,len)
char *name;
int len;
{
	LONG	ret;
	HKEY	hOpenKey;
	DWORD	Type;
		
	name[0] = 0;
	ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, ECGEN_KEYNAME, 0,
		KEY_QUERY_VALUE, &hOpenKey);
	if ( ret != ERROR_SUCCESS )
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, ASIR_KEYNAME, 0,
			KEY_QUERY_VALUE, &hOpenKey);
	if( ret == ERROR_SUCCESS ) {
		RegQueryValueEx(hOpenKey, "Directory", NULL, &Type, name, &len);
		RegCloseKey(hOpenKey);
	}
}
#else
void get_rootdir(name,len)
char *name;
int len;
{
	strcpy(name,asir_libdir);
}
#endif

void Pget_rootdir(rp)
STRING *rp;
{
	static struct oSTRING rootdir;
	static char DirName[BUFSIZ];
	int DirNameLen;

	if ( !rootdir.body ) {
		get_rootdir(DirName,sizeof(DirName));
		rootdir.id = O_STR;
		rootdir.body = DirName;
	}
	*rp = &rootdir;
}

void Pbsave_enc(arg,rp)
NODE arg;
Obj *rp;
{
	init_deskey();
	des_encryption = 1;
	Pbsave(arg,rp);
	des_encryption = 0;
}

void Pbload_enc(arg,rp)
NODE arg;
Obj *rp;
{
	init_deskey();
	des_encryption = 1;
	Pbload(arg,rp);
	des_encryption = 0;
}

void Pbload27(arg,rp)
NODE arg;
Obj *rp;
{
	FILE *fp;
	Obj r;

	asir_assert(ARG0(arg),O_STR,"bload27");
#if defined(THINK_C) || defined(VISUAL)
	fp = fopen(BDY((STRING)ARG0(arg)),"rb");
#else
	fp = fopen(BDY((STRING)ARG0(arg)),"r");
#endif
	if ( !fp )
		error("bload : invalid filename");
	loadvl(fp);
	loadobj(fp,&r);
	fclose(fp);
	bobjtoobj(BASE27,r,rp);
}

void Pbsave_compat(arg,rp)
NODE arg;
Q *rp;
{
	FILE *fp;
	VL vl,t;

	asir_assert(ARG1(arg),O_STR,"bsave_compat");
	get_vars_recursive(ARG0(arg),&vl);
	for ( t = vl; t; t = NEXT(t) )
		if ( t->v->attr == (pointer)V_UC )
			error("bsave : not implemented");
#if defined(THINK_C) || defined(VISUAL)
	fp = fopen(BDY((STRING)ARG1(arg)),"wb");
#else
	fp = fopen(BDY((STRING)ARG1(arg)),"w");
#endif
	if ( !fp )
		error("bsave : invalid filename");
	/* indicator of an asir32 file */
	putw(0,fp); putw(0,fp);
	savevl(fp,vl);
	saveobj(fp,ARG0(arg));
	fclose(fp);
	*rp = ONE;
}

void Pbload_compat(arg,rp)
NODE arg;
Obj *rp;
{
	FILE *fp;
	unsigned int hdr[2];
	Obj r;
	int c;

	asir_assert(ARG0(arg),O_STR,"bload_compat");
#if defined(THINK_C) || defined(VISUAL)
	fp = fopen(BDY((STRING)ARG0(arg)),"rb");
#else
	fp = fopen(BDY((STRING)ARG0(arg)),"r");
#endif
	if ( !fp )
		error("bload : invalid filename");
	fread(hdr,sizeof(unsigned int),2,fp);
	if ( !hdr[0] && !hdr[1] ) {
		/* asir32 file or asir27 0 */
		c = fgetc(fp);
		if ( c == EOF ) {
			/* asir27 0 */
			*rp = 0;
		} else {
			/* asir32 file */
			ungetc(c,fp);
			loadvl(fp);
			loadobj(fp,rp);
		}
	} else {
		/* asir27 file */
		rewind(fp);
		loadvl(fp);
		loadobj(fp,&r);
		bobjtoobj(BASE27,r,rp);
	}
	fclose(fp);
}

void Premove_file(arg,rp)
NODE arg;
Q *rp;
{
	unlink((char *)BDY((STRING)ARG0(arg)));
	*rp = ONE;
}

void Paccess(arg,rp)
NODE arg;
Q *rp;
{
	char *name;
	STRING str;

#if defined(VISUAL)
	if ( access(BDY((STRING)ARG0(arg)),04) >= 0 )
#else
	if ( access(BDY((STRING)ARG0(arg)),R_OK) >= 0 )
#endif
		*rp = ONE;
	else
		*rp = 0;
}
