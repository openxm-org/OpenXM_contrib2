/* $OpenXM: OpenXM/src/asir99/parse/debug.c,v 1.2 1999/11/18 05:42:02 noro Exp $ */
#include "ca.h"
#include "parse.h"
#include <ctype.h>
#include "genpari.h"

void show_stack(VS);
void change_stack(int,NODE *);
void showpos(void);
void printvars(char *,VS);
void println(int,char **,int);
void bp(SNODE);
void searchsn(SNODE *,int,SNODE **);
void showbp(int);
void showbps(void);
void delbp(int,char **);
int searchbp(void);
void clearbp(FUNC);
void settp(char *);
void setbp(char *);
void setf(int,char **);
void show_alias(char *);
void add_alias(char *,char *);

extern do_server_in_X11,do_file;

typedef enum {
	D_NEXT, D_STEP, D_FINISH, D_CONT, D_QUIT,
	D_UP, D_DOWN, D_FRAME,
	D_LIST, D_PRINT,
	D_SETF, D_SETBP, D_SETTP, D_DELBP,
	D_SHOWBP, D_WHERE, D_ALIAS, D_UNKNOWN
} did;

struct oDEBCOM {
	NODE names;
	did id;
};

int nextbp,nextbplevel;
FUNC cur_binf;

static FUNC targetf;
static int curline = 1;

extern NODE PVSS;
extern unsigned int evalstatline;
extern int debug_mode;
extern jmp_buf debug_env;

char *debcom[] = {
	"next",
	"step",
	"finish",
	"cont",
	"quit",
	"up",
	"down",
	"frame",
	"list",
	"print",
	"func",
	"stop",
	"trace",
	"delete",
	"status",
	"where",
	"alias",
};

struct oDEBCOM dckwd[] = {
	{0, D_NEXT},
	{0, D_STEP},
	{0, D_FINISH},
	{0, D_CONT},
	{0, D_QUIT},
	{0, D_UP},
	{0, D_DOWN},
	{0, D_FRAME},
	{0, D_LIST},
	{0, D_PRINT},
	{0, D_SETF},
	{0, D_SETBP},
	{0, D_SETTP},
	{0, D_DELBP},
	{0, D_SHOWBP},
	{0, D_WHERE},
	{0, D_ALIAS},
	{0, D_UNKNOWN}
};

void debug_init() {
	int i,n,ac;
	char *home;
	char buf[BUFSIZ+1];
#if defined(__MWERKS__)
	char *av[64];
#else
	char *av[BUFSIZ];
#endif
	FILE *fp;

	for ( n = 0; dckwd[n].id != D_UNKNOWN; n++ );
	for ( i = 0; i < n; i++ )
		MKNODE(dckwd[i].names,debcom[i],0);
#if defined(THINK_C)
	strcpy(buf,"dbxinit");
#else
	home = (char *)getenv("HOME");
	if ( home )
		strcpy(buf,home);
	else
		buf[0] = 0;
	strcat(buf,"/.dbxinit");
#endif
	if ( (fp = fopen(".dbxinit","r")) || (fp = fopen(buf,"r")) ) {
		while ( fgets(buf,BUFSIZ,fp) ) {
			stoarg(buf,&ac,av);
			if ((ac == 3) && !strcmp(av[0],"alias"))
				add_alias(av[2],av[1]);
		}
		fclose(fp);
	}
#if !defined(VISUAL)
	if ( do_server_in_X11 )
		init_cmdwin();
#endif
}

void add_alias(com,alias)
char *com,*alias;
{
	int i;
	NODE tn;
	char *s;

	for ( i = 0; dckwd[i].names; i++ )
		if (!strcmp(com,debcom[i])) {
			for ( tn = dckwd[i].names; NEXT(tn); tn = NEXT(tn) );
			s = (char *)MALLOC(strlen(alias)+1); strcpy(s,alias);
			MKNODE(NEXT(tn),s,0);
			return;
		}
}

void show_alias(alias)
char *alias;
{
	int i;
	NODE tn;

	if ( !alias )
		for ( i = 0; dckwd[i].names; i++ ) {
			if ( tn = NEXT(dckwd[i].names) )
				fprintf(stderr,"%s\t",debcom[i]);
			for ( ; tn; tn = NEXT(tn) ) {
				fputs(BDY(tn),stderr);
				if ( NEXT(tn) )
					fputc(' ',stderr);
				else
					fputc('\n',stderr);
			}
		}
	else
		for ( i = 0; dckwd[i].names; i++ )
			for ( tn = dckwd[i].names; tn; tn = NEXT(tn) )
				if ( !strcmp(alias,BDY(tn)) ) {
					fprintf(stderr,"%s->%s\n",alias,debcom[i]); return;
				}
}

void debug(f)
SNODE f;
{
	int ac,i,n;
	did id;
#if defined(__MWERKS__)
	char *av[64];
#else
	char *av[BUFSIZ];
#endif
	char buf[BUFSIZ];
	char prompt[BUFSIZ];
	char *p,*pe;
	NODE tn;
	extern int kernelmode,do_fep;
	NODE pvss;

#if !defined(VISUAL)
	if ( !isatty(fileno(stdin)) && !do_server_in_X11 )
		if ( do_file )
			ExitAsir();
		else
			return;
#endif
#if defined(VISUAL) || defined(THINK_C)
	suspend_timer();
#endif
	pvss = PVSS; debug_mode = 1;
#if !defined(VISUAL)
	if ( do_server_in_X11 )
#endif
		show_debug_window(1);
	sprintf(prompt,"(debug)%c",kernelmode?0xfe:' ');
	if ( kernelmode )
		fputc('\0',stderr);
	setjmp(debug_env);
	while ( 1 ) {
		if ( !do_server_in_X11 )
			fputs(prompt,stderr);
		bzero(buf,BUFSIZ);
		while ( 1 ) {
			{
				int len;

#if !defined(VISUAL)
				if ( do_server_in_X11 )
					get_line(buf);
				else 
#endif
				if ( !fgets(buf,BUFSIZ,stdin) )
					goto LAST;
				len = strlen(buf);
				if ( buf[len-1] == '\n' )
					buf[len-1] = 0;
			}
			for ( p = buf; *p && isspace(*p); p++ );
			if ( *p )
				break;
		}
		for ( pe = p; *pe && !isspace(*pe); pe++ );
		*pe = 0;
		for ( i = 0; dckwd[i].names; i++ ) {
			for ( tn = dckwd[i].names; tn; tn = NEXT(tn) )
				if ( !strcmp(BDY(tn),p) )
					break;
			if ( tn )
				break;
		}
		id = dckwd[i].id; p = pe+1;
		switch ( id ) {
			case D_NEXT:
				if ( f ) { nextbp = 1; nextbplevel = 0; }
				goto LAST; break;
			case D_STEP:
				if ( f ) { nextbp = 1; nextbplevel = -1; }
				goto LAST; break;
			case D_FINISH:
				if ( f ) { nextbp = 1; nextbplevel = 1; }
				goto LAST; break;
			case D_CONT: case D_QUIT:
				goto LAST; break;
			case D_UP:
				if ( f ) {
					stoarg(p,&ac,av); n = ac ? atoi(av[0]) : 1;
					change_stack(n,&pvss);
				}
				break;
			case D_DOWN:
				if ( f ) {
					stoarg(p,&ac,av); n = ac ? atoi(av[0]) : 1;
					change_stack(-n,&pvss);
				}
				break;
			case D_FRAME:
				if ( f ) {
					stoarg(p,&ac,av);
					if ( !ac )
						show_stack((VS)BDY(pvss));
					else {
						n = atoi(av[0]);
						change_stack(((VS)BDY(pvss))->level-((VS)BDY(PVSS))->level+n,&pvss);
					}
				}
				break;
			case D_PRINT:
				printvars(p,pvss?(VS)BDY(pvss):GPVS); break;
			case D_LIST:
				stoarg(p,&ac,av); println(ac,av,10); break;
			case D_SETF:
				stoarg(p,&ac,av); setf(ac,av); break;
			case D_SETBP:
				setbp(p); break;
			case D_SETTP:
				settp(p); break;
			case D_DELBP:
				stoarg(p,&ac,av); delbp(ac,av); break;
			case D_SHOWBP:
				showbps(); break;
			case D_WHERE:
				showpos(); break;
			case D_ALIAS:
				stoarg(p,&ac,av);
				switch ( ac ) {
					case 0:
						show_alias(0); break;
					case 1:
						show_alias(av[0]); break;
					case 2: default:
						add_alias(av[1],av[0]); break;
				}
				break;
			default:
				break;
		}
	}
LAST:
	if ( kernelmode )
		fputc('\0',stderr);
	debug_mode = 0;
#if defined(THINK_C)
	show_debug_window(0);
	resume_timer();
#elif !defined(VISUAL)
	if ( do_server_in_X11 )
#endif
		show_debug_window(0);
}

void setf(ac,av)
int ac;
char **av;
{
	FUNC r;

	if ( !ac )
		return;
	searchf(usrf,av[0],&r);
	if ( r ) {
		targetf = r;
		curline = targetf->f.usrf->startl;
	}
}

#define MAXBP 64 

static struct {
	int at;
	FUNC f;
	SNODE *snp;
	char *texpr;
	char *cond;
} bpt[MAXBP];

static int bpindex = 0;

void setbp(p)
char *p;
{
	int ac;
	char *av[BUFSIZ];
	char *buf,*savp;
	char *fname;
	FUNC r;
	USRF t;
	SNODE *snp = 0;
	FNODE cond;
	NODE tn;
	int n,at,ln,bpi;

	buf = (char *)ALLOCA(strlen(p)+1); strcpy(buf,p); stoarg(buf,&ac,av);
	if ( ac < 2 )
		return;
	if ( !strcmp(av[0],"at") ) {
		if ( !targetf )
			return;
		n = atoi(av[1]); fname = targetf->f.usrf->fname;
		for ( tn = usrf; tn; tn = NEXT(tn) ) {
			r = (FUNC)BDY(tn); t = r->f.usrf;
			if ( t && t->fname && !strcmp(t->fname,fname) 
				&& ( t->startl <= n ) && ( n <= t->endl ) )
				break;
		}
		if ( !r ) {
			fprintf(stderr,"no such line in %s\n",fname);
			return;
		} else {
			targetf = r; curline = n;
		}
		at = 1; searchsn(&BDY(t),n,&snp);
	} else if ( !strcmp(av[0],"in") ) {
		searchf(usrf,av[1],&r);
		if ( !r ) {
			fprintf(stderr,"%s() : no such function\n",av[1]);
			return;
		} else if ( r->id == A_UNDEF ) {
			fprintf(stderr,"%s : undefined\n",av[1]);
			return;
		}
		for ( tn = (NODE)FA0(BDY(r->f.usrf)); tn && !BDY(tn); tn = NEXT(tn) );
		if ( tn ) {
			snp = (SNODE *)&(BDY(tn)); at = 0; targetf = r; curline = (*snp)->ln;
		}
	} else
		return;
	if ( snp ) {
		cond = 0;
		if ( ac >= 3 && !strncmp(av[2],"if",2) ) {
			savp = p+(av[2]-buf)+2;
			exprparse(targetf,savp,&cond);
		}
		bpi = searchbp();
		if ( bpi < 0 )
			fprintf(stderr,"too many breakpoints\n");
		else if ( ID(*snp) == S_BP ) 
			return;
		else {
			switch ( ID(*snp) ) {
				case S_IFELSE: case S_FOR: case S_DO:
					ln = (int)FA0(*snp); break;
				default:
					ln = (*snp)->ln; break;
			}
			*snp = (SNODE)mksnode(3,S_BP,*snp,cond,0);
			(*snp)->ln = ln;
			if ( cond ) {
				bpt[bpi].cond = (char *)MALLOC(strlen(savp)+1);
				strcpy(bpt[bpi].cond,savp);
			} else
				bpt[bpi].cond = 0;
			bpt[bpi].at = at; 
			bpt[bpi].f = targetf;
			bpt[bpi].snp = snp;
			bpt[bpi].texpr = 0;
			showbp(bpi);
		}
	}
}

void settp(p)
char *p;
{
	int ac;
	char *_av[BUFSIZ];
	char **av;
	char *buf,*savp;
	char *fname;
	char *texprname;
	FUNC r;
	USRF t;
	SNODE *snp = 0;
	FNODE cond,texpr;
	NODE tn;
	int n,at,ln,bpi;

	av = _av;
	buf = (char *)ALLOCA(strlen(p)+1); strcpy(buf,p); stoarg(buf,&ac,av);
	if ( ac < 3 )
		return;
	texprname = av[0]; ac--; av++;
	if ( !strcmp(av[0],"at") ) {
		if ( !targetf )
			return;
		n = atoi(av[1]); fname = targetf->f.usrf->fname;
		for ( tn = usrf; tn; tn = NEXT(tn) ) {
			r = (FUNC)BDY(tn); t = r->f.usrf;
			if ( t && t->fname && !strcmp(t->fname,fname) 
				&& ( t->startl <= n ) && ( n <= t->endl ) )
				break;
		}
		if ( !r ) {
			fprintf(stderr,"no such line in %s\n",fname);
			return;
		} else {
			targetf = r; curline = n;
		}
		at = 1; searchsn(&BDY(t),n,&snp);
	} else if ( !strcmp(av[0],"in") ) {
		searchf(usrf,av[1],&r);
		if ( !r ) {
			fprintf(stderr,"%s() : no such function\n",av[1]);
			return;
		}
		for ( tn = (NODE)FA0(BDY(r->f.usrf)); tn && !BDY(tn); tn = NEXT(tn) );
		if ( tn ) {
			snp = (SNODE *)&(BDY(tn)); at = 0; targetf = r; curline = (*snp)->ln;
		}
	} else
		return;
	if ( snp ) {
		cond = 0;
		exprparse(targetf,texprname,&texpr);
		if ( ac >= 3 && !strncmp(av[2],"if",2) ) {
			savp = p+(av[2]-buf)+2;
			exprparse(targetf,savp,&cond);
		}
		bpi = searchbp();
		if ( bpi < 0 )
			fprintf(stderr,"too many breakpoints\n");
		else if ( ID(*snp) == S_BP ) 
			return;
		else {
			switch ( ID(*snp) ) {
				case S_IFELSE: case S_FOR: case S_DO:
					ln = (int)FA0(*snp); break;
				default:
					ln = (*snp)->ln; break;
			}
			*snp = (SNODE)mksnode(3,S_BP,*snp,cond,texpr);
			(*snp)->ln = ln;
			if ( cond ) {
				bpt[bpi].cond = (char *)MALLOC(strlen(savp)+1);
				strcpy(bpt[bpi].cond,savp);
			} else
				bpt[bpi].cond = 0;
			bpt[bpi].at = at; 
			bpt[bpi].f = targetf;
			bpt[bpi].snp = snp;
			bpt[bpi].texpr = (char *)MALLOC(strlen(texprname)+1);
			strcpy(bpt[bpi].texpr,texprname);
			showbp(bpi);
		}
	}
}

void clearbp(f)
FUNC f;
{
	int i;

	if ( !f )
		return;
	for ( i = 0; i < bpindex; i++ )
	if ( bpt[i].snp && !strcmp(f->name,bpt[i].f->name) ) {
		bpt[i].at = 0; bpt[i].f = 0; bpt[i].snp = 0;
	}
}

int searchbp()
{
	int i;

	for ( i = 0; i < bpindex; i++ )
		if ( !bpt[i].snp )
			return i;
	if ( bpindex == MAXBP )
		return -1;
	else
		return bpindex++;
}

void delbp(ac,av)
int ac;
char **av;
{
	int n;

	if ( !ac )
		return;
	if ( (n = atoi(av[0])) >= bpindex )
		return;
	if ( bpt[n].snp ) {
		*bpt[n].snp = (SNODE)FA0(*bpt[n].snp);
		bpt[n].snp = 0;
	}
}

void showbps() {
	int i;

	for ( i = 0; i < bpindex; i++ )
		showbp(i);
}

void showbp(n)
int n;
{
	if ( bpt[n].snp )
		if ( bpt[n].texpr )
			if ( bpt[n].at ) {
				if ( bpt[n].cond )
					fprintf(stderr,"(%d) trace %s at \"%s\":%d if %s\n",
						n,bpt[n].texpr,bpt[n].f->f.usrf->fname,(*bpt[n].snp)->ln,bpt[n].cond);
				else
					fprintf(stderr,"(%d) trace %s at \"%s\":%d\n",
						n,bpt[n].texpr,bpt[n].f->f.usrf->fname,(*bpt[n].snp)->ln);
			} else {
				if ( bpt[n].cond )
					fprintf(stderr,"(%d) trace %s in %s if %s\n",
						n,bpt[n].texpr,bpt[n].f->name,bpt[n].cond);
				else
					fprintf(stderr,"(%d) trace %s in %s\n",n,bpt[n].texpr,bpt[n].f->name);
			}
		else
			if ( bpt[n].at ) {
				if ( bpt[n].cond )
					fprintf(stderr,"(%d) stop at \"%s\":%d if %s\n",
						n,bpt[n].f->f.usrf->fname,(*bpt[n].snp)->ln,bpt[n].cond);
				else
					fprintf(stderr,"(%d) stop at \"%s\":%d\n",
						n,bpt[n].f->f.usrf->fname,(*bpt[n].snp)->ln);
			} else {
				if ( bpt[n].cond )
					fprintf(stderr,"(%d) stop in %s if %s\n",
						n,bpt[n].f->name,bpt[n].cond);
				else
					fprintf(stderr,"(%d) stop in %s\n",n,bpt[n].f->name);
			}
}

void searchsn(fp,n,fpp)
SNODE *fp;
int n;
SNODE **fpp;
{
	NODE tn;
	SNODE sn;
	SNODE *snp;

	*fpp = 0;
	switch (ID(*fp)) {
		case S_CPLX:
			for ( tn = (NODE)FA0(*fp); tn; tn = NEXT(tn) )
				if ( sn = (SNODE)BDY(tn) ) {
					snp = (SNODE *)(ID(sn) == S_BP ? &FA0(sn) : &BDY(tn));
					if ( (*snp)->ln >= n ) {
						searchsn(snp,n,fpp); break;
					}
				}
			break;
		case S_IFELSE:
			if ( n <= (int)FA0(*fp) )
				*fpp = fp;
			else if ( n <= ((SNODE)FA2(*fp))->ln )
				searchsn((SNODE *)&FA2(*fp),n,fpp);
			else if ( FA3(*fp) )
				searchsn((SNODE *)&FA3(*fp),n,fpp);
			if ( !(*fpp) )
				*fpp = fp;
			break;
		case S_FOR:
			if ( n <= (int)FA0(*fp) )
				*fpp = fp;
			else 
				searchsn((SNODE *)&FA4(*fp),n,fpp);
			if ( !(*fpp) )
				*fpp = fp;
			break;
		case S_DO:
			if ( n <= (int)FA0(*fp) )
				*fpp = fp;
			else 
				searchsn((SNODE *)&FA1(*fp),n,fpp);
			if ( !(*fpp) )
				*fpp = fp;
			break;
		case S_BP:
			switch ( ID((SNODE)FA0(*fp)) ) {
				case S_SINGLE:
					*fpp = fp; break;
				default:
					searchsn((SNODE *)&FA0(*fp),n,fpp); break;
			}
			break;
		case S_SINGLE: default:
			*fpp = fp;
			break;
	}
}

void bp(f)
SNODE f;
{
	int ln;

	if ( !f || (CPVS == GPVS) )
		return;

	switch ( ID(f) ) {
		case S_IFELSE: case S_FOR:
			ln = (int)FA0(f); break;
		default:
			ln = f->ln; break;
	}
#if !defined(VISUAL)
	if ( do_server_in_X11 )
#endif
		show_debug_window(1);
	fprintf(stderr,"stopped in %s at line %d in file \"%s\"\n",
		CPVS->usrf->name,ln,CPVS->usrf->f.usrf->fname);
	targetf = CPVS->usrf; curline = ln;
	println(0,0,1);
#if !defined(VISUAL)
	if ( do_server_in_X11 || isatty(0) )
#endif
		debug(f);
}

void println(ac,av,l)
int ac;
char **av;
int l;
{
	FILE *fp;
	char buf[BUFSIZ+1];
	int i;
	int ln;
	FUNC r;
#if defined(THINK_C)
	void setDir(short);
#endif

	if ( !ac )
		ln = curline;
	else if ( isdigit(av[0][0]) )
		ln = atoi(av[0]);
	else {
		searchf(usrf,av[0],&r);
		if ( r && r->id != A_UNDEF ) {
			targetf = r;
			ln = r->f.usrf->startl;
		} else {
			fprintf(stderr,"%s undefined\n",av[0]);
			ln = curline;
		}
	}
	if ( !targetf )
		return;
#if defined(THINK_C)
	setDir(targetf->f.usrf->vol);
#endif
	fp = fopen(targetf->f.usrf->fname,"r");
#if defined(THINK_C)
	resetDir();
#endif
	if ( !fp ) {
		fprintf(stderr,"\"%s\" not found\n",targetf->name);
		return;
	}
	for ( i = 1; i < ln; i++ )
		if ( !fgets(buf,BUFSIZ,fp) )
			return;
	for ( i = 0; i < l; i++ ) {
		if ( !fgets(buf,BUFSIZ,fp) )
			break;
		fprintf(stderr,"%d	%s",ln+i,buf);
	}
	curline = ln + i;
	fclose(fp);
}

void printvars(s,vs)
char *s;
VS vs;
{
	FNODE expr;
	char *p;
	pointer val = 0;
	int err;
	VS cpvs;

	for ( p = s; *p; p++ )
		if ( *p == '\n' ) {
			*p = 0; break;
		}
	if ( exprparse(vs==GPVS?0:vs->usrf,s,&expr) ) {
		cpvs = CPVS; CPVS = vs;
		if ( !(err = setjmp(debug_env)) )
			val = eval(expr); 
		CPVS = cpvs;
		if ( !err ) {
#if PARI
#if PARI1
			outfile = stderr;
#else
			pari_outfile = stderr;
#endif
#endif
			asir_out = stderr;
			for ( p = s; isspace(*p); p++ );
			fprintf(asir_out,"%s = ",p); 
			if ( val && ID((R)val) == O_MAT )
				putc('\n',asir_out);
#if defined(VISUAL_LIB)
			w_noflush_stderr(1);
#endif
			printexpr(CO,val); putc('\n',asir_out); fflush(asir_out);
#if defined(VISUAL_LIB)
			w_noflush_stderr(0);
#endif
#if PARI
#if PARI1
			outfile = stdout;
#else
			pari_outfile = stdout;
#endif
#endif
			asir_out = stdout;
		}
	}
}

void showpos()
{
	NODE n;
	VS vs;
	int level;

	if ( PVSS ) {
		if ( cur_binf )
			fprintf(stderr,"%s() (builtin)\n",cur_binf->name);
		((VS)BDY(PVSS))->at = evalstatline;
		level = ((VS)BDY(PVSS))->level;
		for ( n = PVSS; n; n = NEXT(n) ) {
			vs = (VS)BDY(n);
			fprintf(stderr,"#%d %s(), line %d in \"%s\"\n",
				level-vs->level,vs->usrf->name,vs->at,vs->usrf->f.usrf->fname);
		}
	}
}

void showpos_to_string(buf)
char *buf;
{
	NODE n;
	VS vs;
	int level;

	buf[0] = 0;
	if ( PVSS ) {
		if ( cur_binf )
			sprintf(buf,"%s() (builtin)\n",cur_binf->name);
		buf += strlen(buf);
		((VS)BDY(PVSS))->at = evalstatline;
		level = ((VS)BDY(PVSS))->level;
		for ( n = PVSS; n; n = NEXT(n) ) {
			vs = (VS)BDY(n);
			sprintf(buf,"#%d %s(), line %d in \"%s\"\n",
				level-vs->level,vs->usrf->name,vs->at,vs->usrf->f.usrf->fname);
			buf += strlen(buf);
		}
	}
}

void change_stack(level,pvss)
int level;
NODE *pvss;
{
	extern NODE PVSS;
	NODE n;
	int i;
	VS vs;

	if ( !level || !PVSS )
		return;
	if ( level < 0 ) {
		for ( n = PVSS, i = 0; n && n != *pvss; n = NEXT(n), i++ );
		for ( n = PVSS, i = MAX(i+level,0); n && i; n = NEXT(n), i-- );
	} else
		for ( n = *pvss, i = level; NEXT(n) && i; n = NEXT(n), i-- );
	*pvss = n; vs = (VS)BDY(n);
	((VS)BDY(PVSS))->at = evalstatline;
	show_stack(vs);
	targetf = vs->usrf; curline = vs->at;
}

void show_stack(vs)
VS vs;
{
	fprintf(stderr,"#%d %s(), line %d in \"%s\"\n",
		((VS)BDY(PVSS))->level-vs->level,vs->usrf->name,vs->at,vs->usrf->f.usrf->fname);
}
