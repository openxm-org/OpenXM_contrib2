/* $OpenXM: OpenXM_contrib2/asir2000/builtin/help.c,v 1.1.1.1 1999/12/03 07:39:07 noro Exp $ */
#include "ca.h"
#include "parse.h"

void Phelp();
void ghelp(void);

struct ftab help_tab[] = {
	{"help",Phelp,-1},
	{0,0,0},
};

void Phelp(arg,rp)
NODE arg;
Obj *rp;
{
	if ( !arg || !ARG0(arg) )
		 help(0);
	else
		switch (OID(ARG0(arg))) {
			case O_P:
				help(NAME(VR((P)ARG0(arg))));
				break;
			case O_STR:
				help(BDY((STRING)ARG0(arg)));
				break;
			default:
				break;
		}
	*rp = 0;
}

static char *ghelpstr[] = {
"<expression> ';'  : eval and print",
"<expression> '$'  : eval",
"end (quit)        : close the current input stream",
"debug             : enter the debug mode",
0
};

void help(s)
char *s;
{
	extern char *asir_libdir;
	extern char *asir_pager;
	char *e;
	static char helpdir[16];

	if ( !helpdir[0] ) {
		e = (char *)getenv("LANG");
		if ( !e )
			strcpy(helpdir,"help");
		else if ( !strncmp(e,"japan",strlen("japan"))
			|| !strncmp(e,"ja_JP",strlen("ja_JP")) )
			strcpy(helpdir,"help-jp");
		else
			strcpy(helpdir,"help-eg");
	}

	if ( !s )
		ghelp();
	else {
#if !defined(VISUAL)
		int i;
		FUNC f;
		char name[BUFSIZ],com[BUFSIZ];
		FILE *fp;

		sprintf(name,"%s/%s/%s",asir_libdir,helpdir,s);
		if ( fp = fopen(name,"r") ) {
			fclose(fp);
			sprintf(com,"%s %s",asir_pager,name);
			system(com);
		} else {
			searchf(usrf,s,&f);			
			if ( f && f->f.usrf->desc )
				fprintf(stderr,"%s\n",f->f.usrf->desc);
		}
#endif
	}
}

void ghelp() {
	int i;

	for ( i = 0; ghelpstr[i]; i++ )
		fprintf(stderr,"%s\n",ghelpstr[i]);
}
