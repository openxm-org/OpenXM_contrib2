/* $OpenXM: OpenXM_contrib2/asir2000/parse/load.c,v 1.1.1.1 1999/12/03 07:39:12 noro Exp $ */
#include "ca.h"
#include "parse.h"
#if defined(THINK_C)
#include <string.h>
#include        <StandardFile.h>
#include        <Windows.h>
#include        <OSUtils.h>
#include        <ToolUtils.h>
#else
#if defined(VISUAL)
#include <string.h>
#include <fcntl.h>
#if defined(GO32)
#include <unistd.h>
#endif
#include <sys/stat.h>
#else
#if defined(_PA_RISC1_1) || defined(SYSV) || defined(__SVR4)
#include <unistd.h>
#include <string.h>
#else
#include <strings.h>
#endif
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#endif
#endif

#if defined(linux)
#include <unistd.h>
#endif

#if defined(VISUAL)
#include <io.h>
#endif

#undef MALLOC
#define MALLOC(x) GC_malloc((x)+4)

char *ASIRLOADPATH[32];

void encrypt_file(char *,char *);
void decrypt_file(char *,char *);

#if defined(THINK_C)
void searchasirpath(char *,char **);
void loadasirfile(char *);

short initvol;

void initVol();
short getVol();
void setDir(short);
void resetDir();

void initVol() {
	char buf[BUFSIZ];
	
	GetVol((StringPtr)buf,&initvol);
}

short getVol() {
	short vol;
	char buf[BUFSIZ];
	
	GetVol((StringPtr)buf,&vol);
	return vol;
}

void setDir(short vol)
{
	SetVol(0,vol);
}

void resetDir() {
	SetVol(0,initvol);
}
#endif

#if defined(VISUAL)
#define ENVDELIM ';'
#define MORE "more < "
#else
#define ENVDELIM ':'
#define MORE "more"
#endif

#ifndef ASIR_LIBDIR
#define ASIR_LIBDIR "."
#endif

char *getenv();
extern char *asir_libdir;
extern char *asir_pager;
extern int read_exec_file;
extern int main_parser;

void env_init() {
	char *e,*p,*q;
	int i,l;
	char *getenv();
	char *oxhome;
	char rootname[BUFSIZ];

	if ( !(asir_libdir = getenv("ASIR_LIBDIR")) ) {
		if ( oxhome = getenv("OpenXM_HOME") ) {
			asir_libdir = (char *)malloc(strlen(oxhome)+strlen("/lib/asir")+1);
			sprintf(asir_libdir,"%s/lib/asir",oxhome);
		} else {
#if defined(VISUAL)
			get_rootdir(rootname,sizeof(rootname));
			asir_libdir = (char *)malloc(strlen(rootname)+strlen("/lib")+1);
			sprintf(asir_libdir,"%s/lib",rootname);
#else
			asir_libdir = (char *)malloc(strlen(ASIR_LIBDIR)+1);
			strcpy(asir_libdir,ASIR_LIBDIR);
#endif
		}
	}
	if ( !(asir_pager = getenv("PAGER")) ) {
		asir_pager = (char *)malloc(strlen(MORE)+1);
		strcpy(asir_pager,MORE);
	}
	if ( e = getenv("ASIRLOADPATH" ) )
		for ( i = 0; ; i++, e = p+1 ) {
			p = (char *)index(e,ENVDELIM);
			l = p ? p-e : strlen(e); q = (char *)MALLOC(l+1);
			if ( l ) {
				strncpy(q,e,l); q[l] = 0; ASIRLOADPATH[i] = q;
			}
			if ( !p )
				break;
		}
	for ( i = 0; ASIRLOADPATH[i]; i++ );
	ASIRLOADPATH[i] = asir_libdir;
}

void searchasirpath(name,pathp)
char *name;
char **pathp;
{
	char **p;
	char *q;
	int l;
#if !defined(THINK_C) && !defined(VISUAL)
	struct stat sbuf;
	
	if ( (name[0] == '/') || ( name[0] == '.') || strchr(name,':')
		|| !ASIRLOADPATH[0] ) {
		if ( access(name,R_OK) >= 0 ) {
			stat(name,&sbuf);
			if ( (sbuf.st_mode & S_IFMT) != S_IFDIR )
				*pathp = name;
			else
				*pathp = 0;
		} else
			*pathp = 0;
	} else {
		for ( p = ASIRLOADPATH; *p; p++ ) {
			l = strlen(*p)+strlen(name)+2;
			q = (char *)ALLOCA(l); sprintf(q,"%s/%s",*p,name);
			if ( access(q,R_OK) >= 0 ) {
				stat(q,&sbuf);
				if ( (sbuf.st_mode & S_IFMT) != S_IFDIR ) {
					*pathp = (char *)MALLOC(l); strcpy(*pathp,q);
					return;
				}
			}
		}
		*pathp = 0;
	}
#elif defined(VISUAL)
	if ( (name[0] == '/') || ( name[0] == '.') || strchr(name,':')
		|| !ASIRLOADPATH[0] )
		*pathp = name;
	else {
		for ( p = ASIRLOADPATH; *p; p++ ) {
			l = strlen(*p)+strlen(name)+2;
			q = (char *)ALLOCA(l); sprintf(q,"%s/%s",*p,name);
			if ( access(q,04) >= 0 ) {
				*pathp = (char *)MALLOC(l); strcpy(*pathp,q);
				return;
			}
		}
		*pathp = 0;
	}
#else
	if ( (name[0] == '/') || ( name[0] == '.') || !ASIRLOADPATH[0] )
		*pathp = name;
	else {
		for ( p = ASIRLOADPATH; *p; p++ ) {
			l = strlen(*p)+strlen(name)+2;
			q = (char *)ALLOCA(l); sprintf(q,"%s/%s",*p,name);
			*pathp = (char *)MALLOC(l); strcpy(*pathp,q);
			return;
		}
		*pathp = 0;
	}
#endif
}

#if defined(THINK_C)
#define DELIM ':'
#elif defined(VISUAL)
#define DELIM '/'
#endif

void Eungetc(int,FILE *);

void loadasirfile(name0)
char *name0;
{
	FILE *in;
	IN t;
	extern char cppname[];
#if defined(THINK_C) || defined(VISUAL)
	char tname[BUFSIZ],dname[BUFSIZ],ibuf1[BUFSIZ],ibuf2[BUFSIZ];
	int ac;
#if defined(__MWERKS__)
	char *av[64];
#else
	char *av[BUFSIZ];
#endif
	char *p,*c;
	FILE *fp;
	char name[BUFSIZ],tname0[BUFSIZ];
	int encoded;

#if defined(VISUAL)
	fp = fopen(name0,"rb");
	if ( getc(fp) == 0xff ) {
		/* encoded file */
		fclose(fp);
		sprintf(name,"%s.$$$",name0);	
		decrypt_file(name0,name);
		encoded = 1;
	} else {
		fclose(fp);
		strcpy(name,name0);
		encoded = 0;
	}
#else
	strcpy(name,name0);
#endif

	strcpy(dname,name);
	p = strrchr(dname,DELIM);
	av[0] = "cpp"; av[1] = name; av[2] = tname;
	sprintf(ibuf1,"-I%s",asir_libdir); av[3] = ibuf1; av[4] = "-DWINDOWS";
	if ( !p ) {
		sprintf(tname,"%s.___",name); av[5] = 0; ac = 5;
	} else {
		*p++ = 0;
#if defined(VISUAL)
		if ( c = strchr(dname,':') ) {
			*c = 0;
			sprintf(tname,"%s:%s%c%s.___",dname,c+1,DELIM,p);
			*c = ':';
		} else
			
#endif
		sprintf(tname,"%s%c%s.___",dname,DELIM,p);
		sprintf(ibuf2,"-I%s",dname); av[5] = ibuf2; av[6] = 0; ac = 6;
	}
	cpp_main(ac,av);
	if ( encoded ) {
		unlink(name);
		strcpy(tname0,tname);
		sprintf(tname,"%s.###",tname0);
		encrypt_file(tname0,tname);
		unlink(tname0);
		in = fopen(tname,"rb");
	} else
		in = fopen(tname,"r");
	if ( !in ) {
		perror("fopen");
		error("load : failed");
	}
	t = (IN)MALLOC(sizeof(struct oIN));
	t->name = (char *)MALLOC(strlen(name)+1); strcpy(t->name,name);
	t->tname = (char *)MALLOC(strlen(tname)+1); strcpy(t->tname,tname);
	t->encoded = encoded;
#if defined(THINK_C)
	t->vol = getVol(); 
#endif
#else
	char com[BUFSIZ];

	sprintf(com,"%s -I%s %s",cppname,asir_libdir,name0); in = popen(com,"r");
	if ( !in ) {
		perror("popen");
		error("load : failed");
	}
	t = (IN)MALLOC(sizeof(struct oIN));
	t->name = (char *)MALLOC(strlen(name0)+1); strcpy(t->name,name0);
#endif
	t->fp = in; t->ln = 1; t->next = asir_infile; asir_infile = t;
	main_parser = 1; /* XXX */
	Eungetc(afternl(),asir_infile->fp);
	if ( !EPVS->va )
		reallocarray((char **)&EPVS->va,(int *)&EPVS->asize,(int *)&EPVS->n,(int)sizeof(struct oPV));
}

void execasirfile(name)
char *name;
{
	loadasirfile(name);
	read_exec_file = 1;
	read_eval_loop();
	read_exec_file = 0;
}

static NODE objfile = 0;

int loadfile(s)
char *s;
{
	FILE *in;

	if ( in = fopen(s,"r") ) {
		fclose(in);
		loadasirfile(s);
		return 1;
	} else	
		return 0;
}

int loadfiles(node) NODE node; { return 0; }

static unsigned char encrypt_tab[128][2] = {
{137,40},{1,194},{133,79},{48,20},{254,76},{98,17},{110,233},{19,231},
{55,223},{75,65},{178,151},{85,222},{201,46},{51,243},{235,61},{106,113},
{116,121},{24,205},{146,244},{89,234},{163,173},{140,162},{188,45},{195,132},
{202,94},{236,87},{2,127},{253,47},{211,138},{58,252},{142,190},{77,209},
{16,53},{200,220},{99,165},{28,59},{78,0},{208,248},{50,229},{217,39},
{112,15},{18,60},{108,175},{10,67},{23,176},{49,245},{160,27},{171,219},
{105,144},{122,172},{255,114},{226,100},{70,117},{197,11},{180,36},{136,101},
{238,212},{125,120},{103,199},{38,119},{7,206},{181,228},{62,3},{30,185},
{154,63},{247,81},{187,80},{225,56},{210,221},{37,135},{155,52},{54,153},
{84,246},{166,90},{124,167},{41,184},{145,204},{147,198},{25,21},{72,97},
{66,128},{95,139},{93,42},{224,43},{35,143},{111,13},{82,249},{12,148},
{32,152},{186,168},{177,115},{216,251},{5,131},{123,170},{149,161},{213,203},
{126,150},{88,158},{74,169},{159,182},{156,26},{34,104},{8,91},{207,183},
{9,64},{83,241},{134,102},{69,33},{179,237},{129,250},{14,71},{230,4},
{218,6},{189,68},{193,22},{57,174},{215,214},{130,92},{240,31},{118,239},
{109,192},{232,196},{86,107},{96,141},{157,227},{164,242},{44,191},{29,73}
};

static unsigned char decrypt_tab[] = {
36,1,26,62,111,92,112,60,102,104,43,53,87,85,110,40,
32,5,41,7,3,78,114,44,17,78,100,46,35,127,63,118,
88,107,101,84,54,69,59,39,0,75,82,83,126,22,12,27,
3,45,38,13,70,32,71,8,67,115,29,35,41,14,62,64,
104,9,80,43,113,107,52,110,79,127,98,9,4,31,36,2,
66,65,86,105,72,11,122,25,97,19,73,102,117,82,24,81,
123,79,5,34,51,55,106,58,101,48,15,122,42,120,6,85,
40,15,50,90,16,52,119,59,57,16,49,93,74,57,96,26,
80,109,117,92,23,2,106,69,55,0,28,81,21,123,30,84,
48,76,18,77,87,94,96,10,88,71,64,70,100,124,97,99,
46,94,21,20,125,34,73,74,89,98,93,47,49,20,115,42,
44,90,10,108,54,61,99,103,75,63,89,66,22,113,30,126,
120,114,1,23,121,53,77,58,33,12,24,95,76,17,60,103,
37,31,68,28,56,95,116,116,91,39,112,47,33,68,11,8,
83,67,51,124,61,38,111,7,121,6,19,14,25,108,56,119,
118,105,125,13,18,45,72,65,37,86,109,91,29,27,4,50
};

unsigned char encrypt_char(unsigned char c)
{
	return encrypt_tab[c][mt_genrand()&1];
}

unsigned char decrypt_char(unsigned char c)
{
	return decrypt_tab[c];
}

void encrypt_file(in,out)
char *in,*out;
{
	FILE *infp,*outfp;
	int c;

	infp = fopen(in,"r");
	outfp = fopen(out,"wb");
	while ( 1 ) {
		c = getc(infp);
		if ( c == EOF )
			break;
		putc(encrypt_char(c),outfp);
	}
	fclose(infp); fclose(outfp);
}

void decrypt_file(in,out)
char *in,*out;
{
	FILE *infp,*outfp;
	int c;

	infp = fopen(in,"rb");
	outfp = fopen(out,"w");
	/* skip the magic number (=0xff) */
	getc(infp); 
	while ( 1 ) {
		c = getc(infp);
		if ( c == EOF )
			break;
		putc(decrypt_char(c),outfp);
	}
	fclose(infp); fclose(outfp);
}
