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
 *
 * $OpenXM: OpenXM_contrib2/asir2000/parse/load.c,v 1.24 2014/03/30 10:48:16 ohara Exp $ 
*/
#include "ca.h"
#include "parse.h"

#if defined(VISUAL)
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#else /* VISUAL */
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

#if defined(linux)
#include <unistd.h>
#endif

#if defined(VISUAL)
#include <io.h>
#endif

#ifdef MALLOC
#undef MALLOC
#define MALLOC(x) Risa_GC_malloc((x)+4)
#endif

char **ASIRLOADPATH;

#if defined(VISUAL)
#define ENVDELIM ';'
#define MORE "more < "
#else
#define ENVDELIM ':'
#define MORE "more"
#endif

#ifndef ASIR_LIBDIR
#define ASIR_LIBDIR "/usr/local/lib/asir"
#endif
#ifndef ASIR_CONTRIB_DIR
#define ASIR_CONTRIB_DIR "/usr/local/lib/asir-contrib"
#endif

char *getenv();
void Pget_rootdir();
char *search_executable(char *name);

extern char *asir_libdir;
extern char *asir_contrib_dir;
extern char *asir_private_dir;
extern char *asir_pager;
extern int main_parser;
extern JMP_BUF exec_env;

char *search_executable(char *name)
{
	char *c,*s,*ret;
	int len,nlen;
	char dir[BUFSIZ],path[BUFSIZ];
	struct stat buf;

	nlen = strlen(name);
	for ( s = (char *)getenv("PATH"); s; ) {
		c = (char *)index(s,':');
		len = c ? c-s : strlen(s);
		if ( len >= BUFSIZ ) continue;
		strncpy(dir,s,len); dir[len] = 0;
		if ( c ) s = c+1;
		else s = 0;
		if ( len+nlen+1 >= BUFSIZ ) continue;
		sprintf(path,"%s/%s",dir,name);
		if ( !stat(path,&buf) && !(buf.st_mode & S_IFDIR) 
#if !defined(VISUAL)
			&& !access(path,X_OK)
#endif
                ) {
			len = strlen(path)+1;
			ret = (char *)MALLOC(len);
			strcpy(ret,path);
			return ret;
		}
	}
	return 0;
}

void env_init() {
	char *e,*p,*q;
	int i,l,japanese;
	char *getenv();
	char *oxhome;
	char rootname[BUFSIZ];
    size_t len;

	if ( oxhome = getenv("OpenXM_HOME") ) {
		len = strlen(oxhome);
	}else {
#if defined(VISUAL)
		get_rootdir(rootname,sizeof(rootname));
		len = strlen(rootname);
		oxhome = rootname;
#endif
	}

	if ( !(asir_libdir = getenv("ASIR_LIBDIR")) ) {
		if ( oxhome ) {
			asir_libdir = (char *)malloc(len+strlen("/lib/asir")+1);
			sprintf(asir_libdir,"%s/lib/asir",oxhome);
		} else {
			asir_libdir = (char *)malloc(strlen(ASIR_LIBDIR)+1);
			strcpy(asir_libdir,ASIR_LIBDIR);
		}
	}

	if ( !(asir_contrib_dir = getenv("ASIR_CONTRIB_DIR")) ) {
		if ( oxhome ) {
			asir_contrib_dir = (char *)malloc(len+strlen("/lib/asir-contrib")+1);
			sprintf(asir_contrib_dir,"%s/lib/asir-contrib",oxhome);
		} else {
			asir_contrib_dir = (char *)malloc(strlen(ASIR_CONTRIB_DIR)+1);
			strcpy(asir_contrib_dir,ASIR_CONTRIB_DIR);
		}
	}

    asir_private_dir = NULL;
#if defined(VISUAL)
    if ( e = getenv("APPDATA") ) {
        asir_private_dir = (char *)malloc(strlen(e)+strlen("/asir/lib/asir-contrib")+1);
        sprintf(asir_private_dir,"%s/asir/lib/asir-contrib",e);
    }
#endif  

	if ( !(asir_pager = getenv("PAGER")) ) {
		japanese = 0;
		if ( (e = getenv("LANGUAGE")) && 
			(!strncmp(e,"japan",5) || !strncmp(e,"ja_JP",5)) ) japanese = 1;
		else if ( (e = getenv("LC_ALL")) && 
			(!strncmp(e,"japan",5) || !strncmp(e,"ja_JP",5)) ) japanese = 1;
		else if ( (e = getenv("LC_CTYPE")) && 
			(!strncmp(e,"japan",5) || !strncmp(e,"ja_JP",5)) ) japanese = 1;
		else if ( (e = getenv("LANG")) && 
			(!strncmp(e,"japan",5) || !strncmp(e,"ja_JP",5)) ) japanese = 1;
		if ( japanese )
			asir_pager = search_executable("jless");
		if ( !asir_pager ) {
			/* default: more */
			asir_pager = (char *)malloc(strlen(MORE)+1);
			strcpy(asir_pager,MORE);
		}
	}
	if ( e = getenv("ASIRLOADPATH" ) ) {
		for ( i = 0; ; i++, e = p+1 ) {
			p = (char *)index(e,ENVDELIM);
			if ( !p )
				break;
		}
		i += 4;
        ASIRLOADPATH=(char **)MALLOC(sizeof(char *)*i);
		for ( l = 0; l<i; l++) ASIRLOADPATH[l] = NULL; 
		e = getenv("ASIRLOADPATH");
		for ( i = 0; ; i++, e = p+1 ) {
			p = (char *)index(e,ENVDELIM);
			l = p ? p-e : strlen(e); q = (char *)MALLOC(l+1);
			if ( l ) {
				strncpy(q,e,l); q[l] = 0; ASIRLOADPATH[i] = q;
			}
			if ( !p )
				break;
		}
    }else{
	  ASIRLOADPATH=(char **)MALLOC(sizeof(char *)*3);
	  ASIRLOADPATH[0] = NULL;
	}

	for ( i = 0; ASIRLOADPATH[i]; i++ );
	if (asir_private_dir) ASIRLOADPATH[i++] = asir_private_dir;
	if (asir_contrib_dir) ASIRLOADPATH[i++] = asir_contrib_dir;
	if (asir_libdir) ASIRLOADPATH[i++] = asir_libdir;
	ASIRLOADPATH[i] = NULL;
}

#if defined(VISUAL)
#define R_OK 4
#endif

void searchasirpath(char *name,char **pathp)
{
	char **p;
	char *q;
	size_t l;
	int ret;
	struct stat sbuf;
	
	if ( (name[0] == '/') || ( name[0] == '.') || strchr(name,':')
		|| !ASIRLOADPATH[0] ) {
		if ( access(name,R_OK) >= 0 ) {
			ret = stat(name,&sbuf);
			if ( ret == 0 && (sbuf.st_mode & S_IFMT) != S_IFDIR )
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
				ret = stat(q,&sbuf);
				if ( ret == 0 && (sbuf.st_mode & S_IFMT) != S_IFDIR ) {
					*pathp = (char *)MALLOC(l); strcpy(*pathp,q);
					return;
				}
			}
		}
		*pathp = 0;
	}
}

#define DELIM '/'

void loadasirfile(char *name0)
{
	FILE *in;
	INFILE t;
	extern char cppname[];
#if defined(VISUAL)
	char ibuf1[BUFSIZ],ibuf2[BUFSIZ];
	int ac;
	char *av[BUFSIZ];
	char *p;
	FILE *fp;
	char dname[BUFSIZ],tname0[BUFSIZ];
	char *name,*tname;
	int encoded;
	static char prefix[BUFSIZ];
	int process_id();
	char CppExe[BUFSIZ];
	char nbuf[BUFSIZ],tnbuf[BUFSIZ];
	STRING rootdir;
	void call_exe(char *,char **);

	/* create the unique prefix */
	if ( !prefix[0] )
		sprintf(prefix,"asir%d",process_id());

	fp = fopen(name0,"rb");
	if ( getc(fp) == 0xff ) {
		/* encoded file */
		fclose(fp);
		name = tempnam(NULL,prefix);
		decrypt_file(name0,name);
		/* the file 'name' created */
		encoded = 1;
	} else {
		fclose(fp);
		name = name0;
		encoded = 0;
	}

	strcpy(dname,name);
	av[0] = "cpp";
	sprintf(nbuf,"\"%s\"",name);
	av[1] = nbuf; 
	tname = tempnam(NULL,prefix);
	sprintf(tnbuf,"\"%s\"",tname);
	av[2] = tnbuf;
	sprintf(ibuf1,"-I\"%s\"",asir_libdir);
	av[3] = ibuf1;
	av[4] = "-DWINDOWS";

	/* set the include directory */
	p = strrchr(dname,DELIM);
	if ( !p ) {
		av[5] = 0; ac = 5;
	} else {
		*p = 0;
		sprintf(ibuf2,"-I\"%s\"",dname);
		av[5] = ibuf2;
		av[6] = 0; ac = 6;
	}
//	cpp_main(ac,av);
	Pget_rootdir(&rootdir);
	sprintf(CppExe,"%s\\bin\\cpp.exe",BDY(rootdir));
	call_exe(CppExe,av);
			
	/* the file tname created */
	if ( encoded ) {
		unlink(name); free(name);

		strcpy(tname0,tname); free(tname);
		tname = tempnam(NULL,prefix);

		encrypt_file(tname0,tname);
		/* the file tname created */
		unlink(tname0);

		in = fopen(tname,"rb");
	} else
		in = fopen(tname,"r");
	if ( !in ) {
		perror("fopen");
		error("load : failed");
	}
	t = (INFILE)MALLOC(sizeof(struct oINFILE));
	t->name = (char *)MALLOC(strlen(name0)+1); strcpy(t->name,name0);
	t->tname = (char *)MALLOC(strlen(tname)+1); strcpy(t->tname,tname); free(tname);
	t->encoded = encoded;
#else
	char com[BUFSIZ];

	sprintf(com,"%s -I%s %s",cppname,asir_libdir,name0); in = popen(com,"r");
	if ( !in ) {
		perror("popen");
		error("load : failed");
	}
	t = (INFILE)MALLOC(sizeof(struct oINFILE));
	t->name = (char *)MALLOC(strlen(name0)+1); strcpy(t->name,name0);
#endif
	t->fp = in; t->ln = 1; t->next = asir_infile; asir_infile = t;
	main_parser = 1; /* XXX */
	Eungetc(afternl(),asir_infile->fp);
	if ( !EPVS->va )
		reallocarray((char **)&EPVS->va,(int *)&EPVS->asize,(int *)&EPVS->n,(int)sizeof(struct oPV));
}

void execasirfile(char *name)
{
	loadasirfile(name);
	if ( !SETJMP(asir_infile->jmpbuf) ) {
		asir_infile->ready_for_longjmp = 1;
		read_eval_loop();
	}
	closecurrentinput();
}

static NODE objfile = 0;

int loadfile(char *s)
{
	FILE *in;

	if ( in = fopen(s,"r") ) {
		fclose(in);
		loadasirfile(s);
		return 1;
	} else	
		return 0;
}

int loadfiles(NODE node) { return 0; }

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

void encrypt_file(char *in,char *out)
{
	FILE *infp,*outfp;
	int c;

	infp = fopen(in,"r");
	outfp = fopen(out,"wb");
	while ( 1 ) {
		c = getc(infp);
		if ( c == EOF )
			break;
		putc(encrypt_char((unsigned char)c),outfp);
	}
	fclose(infp); fclose(outfp);
}

void decrypt_file(char *in,char *out)
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
		putc(decrypt_char((unsigned char)c),outfp);
	}
	fclose(infp); fclose(outfp);
}
