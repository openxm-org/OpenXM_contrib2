#define VISUAL
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
 * $OpenXM: OpenXM_contrib2/asir2000/parse/cpp.c,v 1.3 2000/08/22 05:04:26 noro Exp $ 
*/
#if defined(__MWERKS__)
#define THINK_C
#endif

#include <stdio.h>
#include <ctype.h>
#include <setjmp.h>
jmp_buf cpp_env;

#if 0
#include "accum.h"
#include "expr.h"
#include "if.h"
#include "io.h"
#include "is.h"
#include "malloc.h"
#include "stats.h"
#include "symtbl.h"
#include "unctrl.h"
#endif

#if defined(THINK_C)
#include <string.h>

int define(char *,int,unsigned char *,int);
void Bcopy(char *,char *,int);
#endif


#include "cpp.h"

char *init_accum(void)
{
 ACCUM *a;

 a = NEW(ACCUM);
 check_malloc(a);
 a->used = 0;
 a->have = 8;
 a->buf = malloc(8);
 check_malloc(a->buf);
 return((char *)a);
}

void accum_char(char *A, char c)
        
#define a ((ACCUM *)A)
       
{
 while (a->used >= a->have)
  { a->buf = realloc(a->buf,a->have+=8);
    check_malloc(a->buf);
  }
 a->buf[a->used++] = c;
}
#undef a

char accum_regret(char *A)
        
#define a ((ACCUM *)A)
{
 if (a->used > 0)
  { return(a->buf[--a->used]);
  }
 else
  { return(0);
  }
}
#undef a

char *accum_result(char *A)
        
#define a ((ACCUM *)A)
{
 char *cp;

 cp = realloc(a->buf,a->used+1);
 check_malloc(cp);
 cp[a->used] = '\0';
 OLD(a);
 return(cp);
}
#undef a

char *accum_sofar(char *A)
        
#define a ((ACCUM *)A)
{
 char *cp;

 cp = malloc(a->used+1);
 check_malloc(cp);
 Bcopy(a->buf,cp,a->used);
 cp[a->used] = '\0';
 return(cp);
}
#undef a

char *accum_buf(char *A)
        
#define a ((ACCUM *)A)
{
 return(a->buf);
}
#undef a

int accum_howfar(char *A)
        
#define a ((ACCUM *)A)
{
 return(a->used);
}
#undef a

void do_at(void)
{
 char *w;

 w = read_ctrl();
 if (strcmp(w,"include") == 0)
  { do_include(0);
  }
 else if (strcmp(w,"define") == 0)
  { do_define(0,0);
  }
 else if (strcmp(w,"undef") == 0)
  { do_undef(0);
  }
 else if (strcmp(w,"redefine") == 0)
  { do_define(0,1);
  }
 else if (strcmp(w,"ifdef") == 0)
  { do_ifdef(0);
  }
 else if (strcmp(w,"ifndef") == 0)
  { do_ifndef(0);
  }
 else if (strcmp(w,"if") == 0)
  { do_if(0);
  }
 else if (strcmp(w,"else") == 0)
  { do_else(0);
  }
 else if (strcmp(w,"elif") == 0)
  { do_elif(0);
  }
 else if (strcmp(w,"endif") == 0)
  { do_endif(0);
  }
 else if (strcmp(w,"set") == 0)
  { do_set();
  }
 else if (strcmp(w,"while") == 0)
  { do_while();
  }
 else if (strcmp(w,"endwhile") == 0)
  { do_endwhile();
  }
 else if (strcmp(w,"dump") == 0)
  { do_dump();
  }
 else if (strcmp(w,"debug") == 0)
  { do_debug();
  }
 else if (strcmp(w,"eval") == 0)
  { do_eval();
  }
 else
  { if (! in_false_if())
     { err_head();
       fprintf(stderr,"unknown control `%s'\n",w);
     }
  }
 free(w);
}

static char *temp;

void autodef_file(char *f)
{
 int i;

 i = strlen(f);
 temp = malloc(i+2+1);
 check_malloc(temp);
 sprintf(temp,"\"%s\"",f);
 undef("__FILE__");
 define("__FILE__",-1,(unsigned char *)temp,0);
}

void autodef_line(int l)
{
 defd("__LINE__",l);
}

char *copyofstr(char *str)
{
 char *cp;

 if (str == 0)
  { return(0);
  }
 cp = malloc(strlen(str)+1);
 if (cp == 0)
  { return(0);
  }
 strcpy(cp,str);
 return(cp);
}

char *copyofblk(char *blk, int len)
{
 char *cp;

 if (blk == 0)
  { return(0);
  }
 cp = malloc(len);
 if (cp == 0)
  { return(0);
  }
 Bcopy(blk,cp,len);
 return(cp);
}

/* these really don't beint here; they should be in their own file */

#if 0
char *malloc(nb)
#undef malloc
int nb;
{
 extern char *malloc();

 return(malloc(nb?nb:1));
}

char *realloc(old,nb)
#undef realloc
char *old;
int nb;
{
 extern char *realloc();

 return(realloc(old,(nb?nb:1)));
}
#endif

static char buf[BUFSIZ];

int debugging = 0;

void do_debug(void)
{
 char c;

 fflush(outfile);
 c = getnonspace();
 switch (c)
  { case '1':
    case 'y':
    case 'Y':
       debugging = 1;
       break;
    default:
       debugging = 0;
       break;
  }
 setbuf(outfile,debugging?NULL:buf);
}

static int nargs;
static int nfound;
static char **argnames;

void read_formals(void)
{
 char *arg;
 char c;

 nargs = 0;
 argnames = (char **) malloc(1);
 check_malloc(argnames);
 while (1)
  { arg = read_ident();
    if (arg)
     { argnames = (char **) realloc((char *)argnames,(nargs+1)*sizeof(char *));
       check_malloc(argnames);
       argnames[nargs] = arg;
       nargs ++;
       c = getnonspace();
       if (c == ')')
	{ return;
	}
       else if (c != ',')
	{ err_head();
	  fprintf(stderr,"invalid macro parameter delimiter\n");
	}
     }
    else
     { c = Get();
       if ((c == ')') && (nargs == 0))
	{ return;
	}
       else
	{ Push(c);
	}
       err_head();
       fprintf(stderr,"missing/illegal macro parameter\n");
       while (1)
	{ c = Get();
	  if (c == ')')
	   { return;
	   }
	  if (isbsymchar(c))
	   { Push(c);
	     break;
	   }
	}
     }
  }
}

void do_define(int sharp, int redef)
{
 char *mac;
 char c;
 char *acc;
 unsigned char *repl;
 int quoted;
 int incomment;
 unsigned char *f;
 unsigned char *t;
 unsigned char *g;
 int i;

 if (in_false_if())
  { char e = '\0';
    if (sharp)
     { while (1)
	{ c = e;
	  e = Get();
	  if (e == '\n')
	   { if (c == '\\')
	      { maybe_print('\n');
	      }
	     else
	      { break;
	      }
	   }
	}
     }
    else
     { do
	{ c = e;
	  e = Get();
	} while ((c != '@') || (e == '@'));
       Push(e);
     }
    return;
  }
 mac = read_ident();
 if (! mac)
  { err_head();
    fprintf(stderr,"missing/illegal macro name\n");
    flush_sharp_line();
    return;
  }
 c = Get();
 if (c == '(')
  { read_formals();
    nfound = nargs;
    if (nargs > 128)
     { err_head();
       fprintf(stderr,"too many macro formals, more than 128 ignored\n");
       nargs = 128;
     }
  }
 else
  { argnames = 0;
    nargs = -1;
    nfound = -1;
    if ((c != ' ') && (c != '\t'))
     { Push(c);
     }
  }
 quoted = 0;
 incomment = 0;
 acc = init_accum();
 if (sharp)
  { while (1)
     { c = Get();
       if (quoted && (c == '\n'))
	{ quoted = 0;
	  maybe_print('\n');
	}
       else if (quoted)
	{ accum_char(acc,'\\');
	  accum_char(acc,c);
	  quoted = 0;
	}
       else if (c == '/')
	{ char d = Get();
	  accum_char(acc,'/');
	  if (d == '*')
	   { accum_char(acc,'*');
	     incomment = 1;
	   }
	  else
	   { Push(d);
	   }
	}
       else if (incomment)
	{ accum_char(acc,c);
	  if (c == '*')
	   { char d = Get();
	     if (d == '/')
	      { accum_char(acc,'/');
		incomment = 0;
	      }
	     else
	      { Push(d);
	      }
	   }
	  else if (c == '\n')
	   { maybe_print('\n');
	   }
	}
       else if (c == '\\')
	{ quoted = 1;
	}
       else if (c == '\n')
	{ break;
	}
       else
	{ accum_char(acc,c);
	}
     }
  }
 else
  { while (1)
     { c = Get();
       if (quoted && (c == '@'))
	{ accum_char(acc,'@');
	  quoted = 0;
	}
       else if (quoted)
	{ Push(c);
	  break;
	}
       else if (c == '/')
	{ char d = Get();
	  accum_char(acc,'/');
	  if (d == '*')
	   { accum_char(acc,'*');
	     incomment = 1;
	   }
	  else
	   { Push(d);
	   }
	}
       else if (incomment)
	{ accum_char(acc,c);
	  if (c == '*')
	   { char d = Get();
	     if (d == '/')
	      { accum_char(acc,'/');
		incomment = 0;
	      }
	     else
	      { Push(d);
	      }
	   }
	  else if (c == '\n')
	   { maybe_print('\n');
	   }
	}
       else if (c == '@')
	{ quoted = 1;
	}
       else
	{ if (c == '\n')
	   { maybe_print('\n');
	   }
	  accum_char(acc,c);
	}
     }
  }
 repl = (unsigned char *) accum_result(acc);
 f = repl;
 t = repl;
 while (*f)
  { if (isbsymchar(*f) && !incomment)
     { for (g=f;issymchar(*g);g++) ;
       c = *g;
       *g = '\0';
       for (i=0;i<nargs;i++)
	{ if (strcmp((char *)f,argnames[i]) == 0)
	   { break;
	   }
	}
       if (i < nargs)
	{ *t++ = 0x80 | i;
	  f = g;
	}
       else
	{ while (*t++ = *f++) ;
	  f --;
	  t --;
	}
       *g = c;
     }
    else if ((f[0] == '/') && (f[1] == '*'))
     { f += 2;
       *t++ = '/';
       *t++ = '*';
       incomment = 1;
     }
    else if (incomment && (f[0] == '*') && (f[1] == '/'))
     { f += 2;
       *t++ = '*';
       *t++ = '/';
       incomment = 0;
     }
    else
     { *t++ = *f++;
     }
  }
 *t++ = '\0';
 repl = (unsigned char *) realloc((char *)repl,t-repl);
 check_malloc(repl);
 if (redef)
  { undef(mac);
  }
 n_defines ++;
 define(mac,nargs,repl,DEF_DEFINE);
 if (argnames)
  { for (i=0;i<nfound;i++)
     { free(argnames[i]);
     }
    free((char *)argnames);
  }
 free(mac);
}

void do_dump(void)
{
 int i;
 DEF *d;
 extern char *cur_incldir;
 extern char *incldir[];
 extern int fstackdepth;

 fprintf(stderr,
	"\n\n\tDump of definition table (%d entries in %d buckets):\n\n",
						n_in_table,symtbl_size);
 for (i=0;i<symtbl_size;i++)
  { fprintf(stderr,"Bucket %d:\n",i);
    for (d=symtbl[i];d;d=d->link)
     { dump_single(d);
       putc('\n',stderr);
     }
  }
 fprintf(stderr,"\n\tInclude directory stack:\n\n");
 for (i=0;i<fstackdepth;i++)
  { fprintf(stderr,"\t\t%s\n",incldir[i]);
  }
 fprintf(stderr,"\t\t%s\n",cur_incldir);
}

void dump_single(DEF *d)
{
 unsigned char *cp;

 fprintf(stderr,"\t%s",d->name);
 if (d->nargs == 0)
  { fprintf(stderr,"()");
  }
 else if (d->nargs > 0)
  { int i;
    for (i=0;i<d->nargs;i++)
     { fprintf(stderr,"%c#%d",i?',':'(',i);
     }
    putc(')',stderr);
  }
 putc(' ',stderr);
 for (cp=d->repl;*cp;cp++)
  { if (*cp & 0x80)
     { fprintf(stderr,"#%d",(*cp)&~0x80);
     }
    else
     { putc(*cp,stderr);
     }
  }
}

void err_head(void)
{
 fprintf(stderr,"\"%s\", line %d: ",curfile(),curline());
}

void Check_malloc(char *ptr)
{
 if (ptr == 0)
  { fprintf(stderr,"out of memory!\n");
    abort();
  }
}

void do_eval(void)
{
 char c;
 char temp[64];
 int i;

 if (! in_false_if())
  { c = getnonspace();
    if (c != '(')
     { err_head();
       fprintf(stderr,"@eval must have ()s\n");
       Push(c);
       return;
     }
    sprintf(temp,"%d",eval_expr(0,1));
    for (i=strlen(temp)-1;i>=0;i--)
     { Push(temp[i]);
     }
  }
}

#define DEBUG_EXPAND/**/

#ifdef DEBUG_EXPAND
extern int debugging;
#endif

extern int keep_comments;

static char **actuals;
static int *actlens;

void read_actuals(DEF *d)
{
 int n;
 int i;
 int pc;
 char c;
 char last;
 char quote;
 int backslash;
 int comment;
 char *acc;

 n = d->nargs;
 actuals = (char **) malloc(n*sizeof(char *));
 check_malloc(actuals);
 actlens = (int *) malloc(n*sizeof(int));
 check_malloc(actlens);
 c = getnonspace();
 if (c != '(')
  { err_head();
    if (n == 0)
     { fprintf(stderr,"missing () on %s\n",d->name);
     }
    else
     { fprintf(stderr,"missing argument%s to %s\n",(n==1)?"":"s",d->name);
     }
    for (i=0;i<n;i++)
     { actuals[i] = copyofstr("");
       check_malloc(actuals[i]);
       actlens[i] = 0;
     }
    Push(c);
    return;
  }
 if (n == 0)
  { c = getnonspace();
    if (c != ')')
     { err_head();
       fprintf(stderr,"unwanted argument to %s\n",d->name);
       Push(c);
     }
    return;
  }
 i = 0;
 while (1)
  { pc = 0;
    quote = 0;
    backslash = 0;
    comment = 0;
    c = 0;
    acc = init_accum();
    while (1)
     { last = c;
       c = Get();
       accum_char(acc,c);
       if (comment)
	{ if ((last == '*') && (c == '/'))
	   { comment = 0;
	   }
	}
       else
	{ if (backslash)
	   { backslash = 0;
	   }
	  else if (quote && (c == quote))
	   { quote = 0;
	   }
	  else if (c == '\\')
	   { backslash = 1;
	   }
	  else if (quote)
	   {
	   }
	  else if ((last == '/') && (c == '*'))
	   { comment = 1;
	   }
	  else if ((c == '\'') || (c == '"'))
	   { quote = c;
	   }
	  else if (c == '(')
	   { pc ++;
	   }
	  else if (c == ')')
	   { if (pc > 0)
	      { pc --;
	      }
	     else
	      { accum_regret(acc);
		break;
	      }
	   }
	  else if ((c == ',') && (pc == 0))
	   { accum_regret(acc);
	     break;
	   }
	}
     }
    if (i < n)
     { actuals[i] = accum_result(acc);
       actlens[i] = strlen(actuals[i]);
       i ++;
     }
    else if (i == n)
     { err_head();
       fprintf(stderr,"too many arguments to %s\n",d->name);
       i ++;
     }
    if (c == ')')
     { break;
     }
  }
 if (i < n)
  { err_head();
    fprintf(stderr,"too few arguments to %s\n",d->name);
    for (;i<n;i++)
     { actuals[i] = copyofstr("");
       check_malloc(actuals[i]);
       actlens[i] = 0;
     }
  }
}

void expand_def(DEF *d)
{
 unsigned char *cp;
 char *dp;
 char *ep;
 char *result;
 int ok;
 int incomm;
 char last;

 if (d->nargs >= 0)
  { read_actuals(d);
  }
#ifdef DEBUG_EXPAND
 if (debugging)
  { char *cp;
    outputs("~EXPAND:");
    outputs(d->name);
    if (d->nargs == 0)
     { outputs("()");
     }
    else if (d->nargs > 0)
     { int i;
       for (i=0;i<d->nargs;i++)
	{ outputc((char)(i?',':'('));
	  outputs(actuals[i]);
	}
       outputc(')');
     }
    outputs("-->");
    for (cp=(char *)d->repl;*cp;cp++)
     { outputs(unctrl(*cp));
     }
    outputc('~');
  }
#endif
 result = init_accum();
 for (cp=d->repl;*cp;cp++)
  { if (*cp & 0x80)
     { char *dp;
       int i;
       i = *cp & ~0x80;
       for (dp=actuals[i];*dp;dp++)
	{ accum_char(result,*dp);
	}
     }
    else
     { accum_char(result,*cp);
     }
  }
 dp = accum_result(result);
#ifdef DEBUG_EXPAND
 if (debugging)
  { outputs("first:");
    for (ep=dp;*ep;ep++)
     { outputs(unctrl(*ep));
     }
    outputc('~');
  }
#endif
 result = init_accum();
 last = '\0';
 ok = 1;
 incomm = 0;
 for (ep=dp;*ep;ep++)
  { if (!incomm && (last == '/') && (*ep == '*'))
     { incomm = 1;
       if (!keep_comments || (strncmp(ep,"**/",3) == 0))
	{ accum_regret(result);
	  ok = 0;
	}
     }
    if (ok)
     { accum_char(result,*ep);
     }
    if ((last == '*') && (*ep == '/'))
     { incomm = 0;
       ok = 1;
     }
    last = *ep;
  }
 free(dp);
 result = accum_result(result);
#ifdef DEBUG_EXPAND
 if (debugging)
  { outputs("/**/strip:");
    outputs(result);
    outputc('~');
  }
#endif
 for (dp=result+strlen(result)-1;dp>=result;dp--)
  { Push(*dp);
  }
 free(result);
 if (d->nargs >= 0)
  { int i;
    for (i=0;i<d->nargs;i++)
     { free(actuals[i]);
     }
    free((char *)actuals);
  }
}

#define DEBUG_EXPR/**/

#define TWOCHAR(c1,c2) (((c1)<<8)|(c2))
#define LSH TWOCHAR('<','<')
#define RSH TWOCHAR('>','>')
#define LEQ TWOCHAR('<','=')
#define GEQ TWOCHAR('>','=')
#define EQL TWOCHAR('=','=')
#define NEQ TWOCHAR('!','=')
#define AND TWOCHAR('&','&')
#define OR  TWOCHAR('|','|')

#define ALLBINS		\
	BIN('*',*)	\
	BIN('/',/)	\
	BIN('%',%)	\
	BIN('+',+)	\
	BIN(LSH,<<)	\
	BIN(RSH,>>)	\
	BIN('<',<)	\
	BIN('>',>)	\
	BIN(LEQ,<=)	\
	BIN(GEQ,>=)	\
	BIN(EQL,==)	\
	BIN(NEQ,!=)	\
	BIN('&',&)	\
	BIN('^',^)	\
	BIN('|',|)	\
	BIN(AND,&&)	\
	BIN(OR ,||)

char *Index(char *s, int c);

static NODE *expr;
int expr_sharp;
#define sharp expr_sharp
static int complain;

#ifdef DEBUG_EXPR
extern int debugging;
#endif

void free_expr(NODE *n)
{
 switch (n->op)
  { case 0:
       break;
    case '-':
       if (n->left)
	{ free_expr(n->left);
	}
       free_expr(n->right);
       break;
    case '!':
    case '~':
       free_expr(n->right);
       break;
    case 'd':
       free(n->name);
       break;
    default:
       free_expr(n->left);
       free_expr(n->right);
       break;
  }
 OLD(n);
}

int exec_free(NODE *n)
{
 int rv;
 int l;
 int r;

 switch (n->op)
  { case 0:
       rv = n->leaf;
       break;
    case '-':
       if (n->left)
	{ rv = exec_free(n->left);
	}
       else
	{ rv = 0;
	}
       rv -= exec_free(n->right);
       break;
    case '!':
       rv = ! exec_free(n->right);
       break;
    case '~':
       rv = ~ exec_free(n->right);
       break;
    case 'd':
       rv = !! find_def(n->name);
       free(n->name);
       break;
#define BIN(key,op) case key:l=exec_free(n->left);r=exec_free(n->right);rv=l op r;break;
    ALLBINS
#undef BIN
  }
 OLD(n);
 return(rv);
}

int exec_nofree(NODE *n)
{
 int rv;

 switch (n->op)
  { case 0:
       rv = n->leaf;
       break;
    case '-':
       if (n->left)
	{ rv = exec_nofree(n->left);
	}
       else
	{ rv = 0;
	}
       rv -= exec_nofree(n->right);
       break;
    case '!':
       rv = ! exec_nofree(n->right);
       break;
    case '~':
       rv = ~ exec_nofree(n->right);
       break;
    case 'd':
       rv = !! find_def(n->name);
       free(n->name);
       break;
#define BIN(key,op) case key:rv=(exec_nofree(n->left)op exec_nofree(n->right));break;
    ALLBINS
#undef BIN
  }
 return(rv);
}

NODE *newnode(NODE *l, int c, NODE *r)
{
 NODE *n;

 n = NEW(NODE);
 check_malloc(n);
 n->left = l;
 n->right = r;
 n->op = c;
 return(n);
}

NODE *newleaf(int v)
{
 NODE *n;

 n = NEW(NODE);
 check_malloc(n);
 n->op = 0;
 n->left = 0;
 n->right = 0;
 n->leaf = v;
 return(n);
}

NODE *newname(char *name)
{
 NODE *n;

 n = NEW(NODE);
 check_malloc(n);
 n->op = 'd';
 n->name = copyofstr(name);
 check_malloc(n->name);
 return(n);
}

int get_quote_char(void)
{
 char c;
 char d;

 c = Get();
 if (c == '\'')
  { return(-1);
  }
 if (c == '\n')
  { err_head();
    fprintf(stderr,"newline in character constant\n");
    return(-1);
  }
 if (c != '\\')
  { return(0xff&(int)c);
  }
 c = Get();
 if ((c >= '0') && (c <= '7'))
  { d = c - '0';
    c = Get();
    if ((c >= '0') && (c <= '7'))
     { d = (d << 3) + c - '0';
       c = Get();
       if ((c >= '0') && (c <= '7'))
	{ d = (d << 3) + c - '0';
	}
       else
	{ Push(c);
	}
     }
    else
     { Push(c);
     }
    return(0xff&(int)d);
  }
 switch (c)
  { case 'n':
       return('\n');
       break;
    case 'r':
       return('\r');
       break;
    case 'e':
       return('\033');
       break;
    case 'f':
       return('\f');
       break;
    case 't':
       return('\t');
       break;
    case 'b':
       return('\b');
       break;
    case 'v':
       return('\v');
       break;
    default:
       return(0xff&(int)c);
       break;
  }
}

NODE *read_expr_11(void)
{
 char c;

#ifdef DEBUG_EXPR
 if (debugging)
  { outputs("~E11:");
  }
#endif
 while (1)
  { c = getnhsexpand();
    if (c == '(')
     { NODE *n;
       NODE *read_expr_(void);
#ifdef DEBUG_EXPR
       if (debugging)
	{ outputs("()");
	}
#endif
       n = read_expr_();
       c = getnhsexpand();
       if (c != ')')
	{ err_head();
	  fprintf(stderr,"expression syntax error -- missing ) supplied\n");
	  Push(c);
	}
#ifdef DEBUG_EXPR
       if (debugging)
	{ outputs("~");
	}
#endif
       return(n);
     }
    else if (isdigit(c))
     { int base;
       static char digits[] = "0123456789abcdefABCDEF";
       static char values[] =
		"\0\1\2\3\4\5\6\7\10\11\12\13\14\15\16\17\12\13\14\15\16\17";
       char *d;
       int v;
#ifdef DEBUG_EXPR
       if (debugging)
	{ outputs("N");
	}
#endif
       base = 10;
       if (c == '0')
	{ base = 8;
	  c = Get();
	  if ((c == 'x') || (c == 'X'))
	   { base = 16;
	     c = Get();
	   }
	}
       v = 0;
       while (1)
	{ d = Index(digits,c);
	  if (d == 0)
	   { Push(c);
#ifdef DEBUG_EXPR
	     if (debugging)
	      { outputd(v);
		outputs("~");
	      }
#endif
	     return(newleaf(v));
	   }
	  else if (values[d-digits] >= base)
	   { err_head();
	     fprintf(stderr,"warning: illegal %sdigit `%c'\n",
			(base==16)?"hex ":(base==8)?"octal ":"",c);
	   }
	  v = (v * base) + values[d-digits];
	  c = Get();
	}
     }
    else if (c == '\'')
     { int i;
       int j;
       int n;
       i = 0;
       n = 0;
       while (1)
	{ j = get_quote_char();
	  if (j < 0)
	   { break;
	   }
	  i = (i << 8) | j;
	  n ++;
	}
       if (n > 4)
	{ err_head();
	  fprintf(stderr,"warning: too many characters in character constant\n");
	}
       return(newleaf(i));
     }
    else if ((c == '\n') && !sharp)
     {
     }
    else
     { char *id;
       if (complain)
	{ err_head();
	  fprintf(stderr,"expression syntax error -- number expected\n");
	}
       if (isbsymchar(c))
	{ Push(c);
	  id = read_ident();
	}
       else
	{ id = 0;
	}
#ifdef DEBUG_EXPR
       if (debugging)
	{ outputs("0(");
	  outputc(c);
	  outputs(":");
	  outputs(id?id:"(none)");
	  outputs(")~");
	}
#endif
       if (id)
	{ free(id);
	}
       return(newleaf(0));
     }
  }
}

NODE *read_expr_10(void)
{
 char c;
 char *w;
 NODE *n;

#ifdef DEBUG_EXPR
 if (debugging)
  { outputs("~E10:");
  }
#endif
 while (1)
  { c = getnhsexpand();
    switch (c)
     { case '-':
       case '~':
       case '!':
#ifdef DEBUG_EXPR
	  if (debugging)
	   { outputc(c);
	   }
#endif
	  n = read_expr_10();
#ifdef DEBUG_EXPR
	  if (debugging)
	   { outputs("~");
	   }
#endif
	  return(newnode(0,c,n));
	  break;
       case 'd':
	  Push(c);
	  input_mark();
	  w = read_ident();
	  if (strcmp(w,"defined") == 0)
	   { c = getnonspace();
	     if (c == '(')
	      { char *id;
		id = read_ident();
		if (id)
		 { c = getnonspace();
		   if (c == ')')
		    { input_unmark();
#ifdef DEBUG_EXPR
		      if (debugging)
		       { outputs("ifdef");
		       }
#endif
		      return(newname(id));
		    }
		 }
	      }
	     else if (isbsymchar(c))
	      { char *id;
		Push(c);
		id = read_ident();
		if (id)
		 { input_unmark();
#ifdef DEBUG_EXPR
		   if (debugging)
		    { outputs("ifdef");
		    }
#endif
		   return(newname(id));
		 }
	      }
	   }
	  input_recover();
	  n = read_expr_11();
#ifdef DEBUG_EXPR
	  if (debugging)
	   { outputs("~");
	   }
#endif
	  return(n);
	  break;
       default:
	  Push(c);
	  n = read_expr_11();
#ifdef DEBUG_EXPR
	  if (debugging)
	   { outputs("~");
	   }
#endif
	  return(n);
	  break;
     }
  }
}

NODE *read_expr_9(void)
{
 NODE *l;
 NODE *r;
 char c;

#ifdef DEBUG_EXPR
 if (debugging)
  { outputs("~E9:");
  }
#endif
 l = read_expr_10();
 while (1)
  { c = getnhsexpand();
    switch (c)
     { case '*':
       case '/':
       case '%':
#ifdef DEBUG_EXPR
	  if (debugging)
	   { outputc(c);
	   }
#endif
	  r = read_expr_10();
	  l = newnode(l,c,r);
	  break;
       default:
#ifdef DEBUG_EXPR
	  if (debugging)
	   { outputs("~");
	   }
#endif
	  Push(c);
	  return(l);
	  break;
     }
  }
}

NODE *read_expr_8(void)
{
 NODE *l;
 NODE *r;
 char c;

#ifdef DEBUG_EXPR
 if (debugging)
  { outputs("~E8:");
  }
#endif
 l = read_expr_9();
 while (1)
  { c = getnhsexpand();
    switch (c)
     { case '+':
       case '-':
#ifdef DEBUG_EXPR
	  if (debugging)
	   { outputc(c);
	   }
#endif
	  r = read_expr_9();
	  l = newnode(l,c,r);
	  break;
       default:
#ifdef DEBUG_EXPR
	  if (debugging)
	   { outputs("~");
	   }
#endif
	  Push(c);
	  return(l);
	  break;
     }
  }
}

NODE *read_expr_7(void)
{
 NODE *l;
 NODE *r;
 char c;
 char d;

#ifdef DEBUG_EXPR
 if (debugging)
  { outputs("~E7:");
  }
#endif
 l = read_expr_8();
 while (1)
  { c = getnhsexpand();
    switch (c)
     { case '<':
       case '>':
	  d = Get();
	  if (d == c)
	   {
#ifdef DEBUG_EXPR
	     if (debugging)
	      { outputc(c);
		outputc(d);
	      }
#endif
	     r = read_expr_8();
	     l = newnode(l,(c=='<')?LSH:RSH,r);
	     break;
	   }
	  Push(d);
	  /* fall through ... */
       default:
#ifdef DEBUG_EXPR
	  if (debugging)
	   { outputs("~");
	   }
#endif
	  Push(c);
	  return(l);
	  break;
     }
  }
}

NODE *read_expr_6(void)
{
 NODE *l;
 NODE *r;
 char c;
 char d;

#ifdef DEBUG_EXPR
 if (debugging)
  { outputs("~E6:");
  }
#endif
 l = read_expr_7();
 while (1)
  { c = getnhsexpand();
    switch (c)
     { case '<':
       case '>':
	  d = Get();
	  if (d == '=')
	   {
#ifdef DEBUG_EXPR
	     if (debugging)
	      { outputc(c);
		outputc(d);
	      }
#endif
	     r = read_expr_7();
	     l = newnode(l,(c=='<')?LEQ:GEQ,r);
	   }
	  else
	   {
#ifdef DEBUG_EXPR
	     if (debugging)
	      { outputc(c);
	      }
#endif
	     Push(d);
	     r = read_expr_7();
	     l = newnode(l,c,r);
	   }
	  break;
       default:
#ifdef DEBUG_EXPR
	  if (debugging)
	   { outputs("~");
	   }
#endif
	  Push(c);
	  return(l);
	  break;
     }
  }
}

NODE *read_expr_5(void)
{
 NODE *l;
 NODE *r;
 char c;
 char d;

#ifdef DEBUG_EXPR
 if (debugging)
  { outputs("~E5:");
  }
#endif
 l = read_expr_6();
 while (1)
  { c = getnhsexpand();
    switch (c)
     { case '=':
       case '!':
	  d = Get();
	  if (d == '=')
	   {
#ifdef DEBUG_EXPR
	     if (debugging)
	      { outputc(c);
		outputc(d);
	      }
#endif
	     r = read_expr_6();
	     l = newnode(l,(c=='=')?EQL:NEQ,r);
	     break;
	   }
	  Push(d);
	  /* fall through ... */
       default:
#ifdef DEBUG_EXPR
	  if (debugging)
	   { outputs("~");
	   }
#endif
	  Push(c);
	  return(l);
	  break;
     }
  }
}

NODE *read_expr_4(void)
{
 NODE *l;
 NODE *r;
 char c;
 char d;

#ifdef DEBUG_EXPR
 if (debugging)
  { outputs("~E4:");
  }
#endif
 l = read_expr_5();
 while (1)
  { c = getnhsexpand();
    switch (c)
     { case '&':
	  d = Get();
	  if (d != '&')
	   {
#ifdef DEBUG_EXPR
	     if (debugging)
	      { outputc(c);
		outputc(d);
	      }
#endif
	     r = read_expr_5();
	     l = newnode(l,c,r);
	     break;
	   }
	  Push(d);
	  /* fall through ... */
       default:
#ifdef DEBUG_EXPR
	  if (debugging)
	   { outputs("~");
	   }
#endif
	  Push(c);
	  return(l);
	  break;
     }
  }
}

NODE *read_expr_3(void)
{
 NODE *l;
 NODE *r;
 char c;

#ifdef DEBUG_EXPR
 if (debugging)
  { outputs("~E3:");
  }
#endif
 l = read_expr_4();
 while (1)
  { c = getnhsexpand();
    switch (c)
     { case '^':
#ifdef DEBUG_EXPR
	  if (debugging)
	   { outputc(c);
	   }
#endif
	  r = read_expr_4();
	  l = newnode(l,c,r);
	  break;
       default:
#ifdef DEBUG_EXPR
	  if (debugging)
	   { outputs("~");
	   }
#endif
	  Push(c);
	  return(l);
	  break;
     }
  }
}

NODE *read_expr_2(void)
{
 NODE *l;
 NODE *r;
 char c;
 char d;

#ifdef DEBUG_EXPR
 if (debugging)
  { outputs("~E2:");
  }
#endif
 l = read_expr_3();
 while (1)
  { c = getnhsexpand();
    switch (c)
     { case '|':
	  d = Get();
	  if (d != '|')
	   { Push(d);
#ifdef DEBUG_EXPR
	     if (debugging)
	      { outputc(c);
	      }
#endif
	     r = read_expr_3();
	     l = newnode(l,c,r);
	     break;
	   }
	  Push(d);
	  /* fall through ... */
       default:
#ifdef DEBUG_EXPR
	  if (debugging)
	   { outputs("~");
	   }
#endif
	  Push(c);
	  return(l);
	  break;
     }
  }
}

NODE *read_expr_1(void)
{
 NODE *l;
 NODE *r;
 char c;
 char d;

#ifdef DEBUG_EXPR
 if (debugging)
  { outputs("~E1:");
  }
#endif
 l = read_expr_2();
 while (1)
  { c = getnhsexpand();
    switch (c)
     { case '&':
	  d = Get();
	  if (d == c)
	   {
#ifdef DEBUG_EXPR
	     if (debugging)
	      { outputc(c);
		outputc(d);
	      }
#endif
	     r = read_expr_2();
	     l = newnode(l,AND,r);
	     break;
	   }
	  Push(d);
	  /* fall through ... */
       default:
#ifdef DEBUG_EXPR
	  if (debugging)
	   { outputs("~");
	   }
#endif
	  Push(c);
	  return(l);
	  break;
     }
  }
}

NODE *read_expr_0(void)
{
 NODE *l;
 NODE *r;
 char c;
 char d;

#ifdef DEBUG_EXPR
 if (debugging)
  { outputs("~E0:");
  }
#endif
 l = read_expr_1();
 while (1)
  { c = getnhsexpand();
    switch (c)
     { case '|':
	  d = Get();
	  if (d == c)
	   {
#ifdef DEBUG_EXPR
	     if (debugging)
	      { outputc(c);
		outputc(d);
	      }
#endif
	     r = read_expr_1();
	     l = newnode(l,OR,r);
	     break;
	   }
	  Push(d);
	  /* fall through ... */
       default:
#ifdef DEBUG_EXPR
	  if (debugging)
	   { outputs("~");
	   }
#endif
	  Push(c);
	  return(l);
	  break;
     }
  }
}

NODE *read_expr_(void)
{
 NODE *l;
 NODE *r;
 char c;

#ifdef DEBUG_EXPR
 if (debugging)
  { outputs("~E");
  }
#endif
 l = read_expr_0();
 while (1)
  { c = getnhsexpand();
    switch (c)
     { case '\n':
       case ')':
	  Push(c);
	  return(l);
	  break;
       case ',':
	  r = read_expr_0();
	  l = newnode(l,c,r);
	  break;
       default:
	  err_head();
	  fprintf(stderr,"expression syntax error -- bad operator `%c'\n",c);
	  return(l);
	  break;
     }
  }
}

NODE *read_expr_p(void)
{
 char c;
 NODE *rv;

 sharp = 0;
 complain = 1;
 c = getnonspace();
 if (c != '(')
  { Push(c);
    return(0);
  }
 rv = read_expr_();
 c = getnonspace();
 if (c != ')')
  { err_head();
    fprintf(stderr,"junk after expression\n");
  }
 return(rv);
}

int eval_expr(int Sharp, int Complain)
{
 char c;
 char d;
 int rv;

 sharp = Sharp;
 complain = Complain;
 expr = read_expr_();
 if (sharp)
  { c = getnonhspace();
    d = '\n';
  }
 else
  { c = getnonspace();
    d = ')';
  }
 if (c != d)
  { if (complain)
     { err_head();
       fprintf(stderr,"expression syntax error -- junk after expression\n");
     }
    while (Get() != d) ;
  }
#ifdef DEBUG_EXPR
 if (debugging)
  { outputc('<');
    dump_expr(expr);
    outputc('=');
    rv = exec_free(expr);
    outputd(rv);
    outputc('>');
  }
 else
  { rv = exec_free(expr);
  }
 return(rv);
#else
 return(exec_free(expr));
#endif
}

#ifdef DEBUG_EXPR
void dump_expr(NODE *n)
{
 switch (n->op)
  { case 0:
       outputd(n->leaf);
       break;
    case '-':
       if (n->left)
	{ dump_expr(n->left);
	}
       outputc('-');
       dump_expr(n->right);
       break;
    case '!':
       outputc('!');
       dump_expr(n->right);
       break;
    case '~':
       outputc('~');
       dump_expr(n->right);
       break;
    case 'd':
       outputs("defined(");
       outputs(n->name);
       outputc(')');
       break;
#define BIN(key,op) case key:dump_expr(n->left);\
			 putx(key);\
			 dump_expr(n->right);\
		break;
    ALLBINS
#undef BIN
  }
}

void putx(int i)
{
 if (i > 0xff) outputc((char)(i>>8));
 outputc((char)(i&0xff));
}
#endif

/*#define DEBUG_IF/**/

#ifdef DEBUG_IF
extern int debugging;
#endif

void do_if(int expr_sharp)
{
 char c;
 char d;

#ifdef DEBUG_IF
 if (debugging)
  { outputc('<');
    outputc(sharp?'#':'@');
    outputs("if: ");
    fflush(outfile);
  }
#endif
 if (in_false_if())
  { n_skipped_ifs ++;
#ifdef DEBUG_IF
    if (debugging)
     { outputs("in-false, skipped>");
       fflush(outfile);
     }
#endif
    if (sharp)
     { d = '\0';
       do
	{ c = d;
	  d = Get();
	} while ((c == '\\') || (d != '\n'));
     }
    return;
  }
 if (! sharp)
  { c = getnonspace();
    if (c != '(')
     { err_head();
       fprintf(stderr,"@if must have ()s\n");
       Push(c);
       iffalse();
#ifdef DEBUG_IF
       if (debugging)
	{ outputc('>');
	  fflush(outfile);
	}
#endif
       return;
     }
  }
 if (eval_expr(sharp,0))
  { iftrue();
  }
 else
  { iffalse();
  }
#ifdef DEBUG_IF
 if (debugging)
  { outputc('>');
    fflush(outfile);
  }
#endif
}

void do_ifdef(int expr_sharp)
{
 char *w;

#ifdef DEBUG_IF
 if (debugging)
  { outputc('<');
    outputc(sharp?'#':'@');
    outputs("ifdef: ");
    fflush(outfile);
  }
#endif
 if (in_false_if())
  { n_skipped_ifs ++;
#ifdef DEBUG_IF
    if (debugging)
     { outputs("in-false, skipped>");
       fflush(outfile);
     }
#endif
  }
 else
  { w = read_ident();
    if (! w)
     {
#ifdef DEBUG_IF
       if (debugging)
	{ outputs("no ident ");
	  fflush(outfile);
	}
#endif
       iffalse();
     }
    else
     {
#ifdef DEBUG_IF
       if (debugging)
	{ outputs(w);
	  outputc(' ');
	  fflush(outfile);
	}
#endif
       if (find_def(w))
	{ iftrue();
	}
       else
	{ iffalse();
	}
       free(w);
     }
#ifdef DEBUG_IF
    if (debugging)
     { outputc('>');
       fflush(outfile);
     }
#endif
  }
 if (sharp)
  { flush_sharp_line();
  }
}

void do_ifndef(int expr_sharp)
{
 char *w;

#ifdef DEBUG_IF
 if (debugging)
  { outputc('<');
    outputc(sharp?'#':'@');
    outputs("ifndef: ");
    fflush(outfile);
  }
#endif
 if (in_false_if())
  { n_skipped_ifs ++;
#ifdef DEBUG_IF
    if (debugging)
     { outputs("in-false, skipped>");
       fflush(outfile);
     }
#endif
  }
 else
  { w = read_ident();
    if (! w)
     {
#ifdef DEBUG_IF
       if (debugging)
	{ outputs("no ident ");
	  fflush(outfile);
	}
#endif
       iftrue();
     }
    else
     {
#ifdef DEBUG_IF
       if (debugging)
	{ outputs(w);
	  outputc(' ');
	  fflush(outfile);
	}
#endif
       if (find_def(w))
	{ iffalse();
	}
       else
	{ iftrue();
	}
       free(w);
     }
#ifdef DEBUG_IF
    if (debugging)
     { outputc('>');
       fflush(outfile);
     }
#endif
  }
 if (sharp)
  { flush_sharp_line();
  }
}

void do_else(int expr_sharp)
{
#ifdef DEBUG_IF
 if (debugging)
  { outputc('<');
    outputc(sharp?'#':'@');
    outputs("else: ");
    fflush(outfile);
  }
#endif
 if (n_skipped_ifs == 0)
  { if (ifstack)
     {
#ifdef DEBUG_IF
       if (debugging)
	{ outputs("top ");
	  output_ifstate(ifstack->condstate);
	  fflush(outfile);
	}
#endif
       switch (ifstack->condstate)
	{ case IFSTATE_TRUE:
	     ifstack->condstate = IFSTATE_STAYFALSE;
	     break;
	  case IFSTATE_FALSE:
	     ifstack->condstate = IFSTATE_TRUE;
	     break;
	}
#ifdef DEBUG_IF
       if (debugging)
	{ outputs(" now ");
	  output_ifstate(ifstack->condstate);
	  outputc('>');
	  fflush(outfile);
	}
#endif
     }
    else
     {
#ifdef DEBUG_IF
       if (debugging)
	{ outputs(" no if>");
	  fflush(outfile);
	}
#endif
       err_head();
       fprintf(stderr,"if-less else\n");
     }
  }
 else
  {
#ifdef DEBUG_IF
    if (debugging)
     { outputs("in-false, forgetting>");
       fflush(outfile);
     }
#endif
  }
 if (sharp)
  { flush_sharp_line();
  }
}

#ifdef DEBUG_IF
void output_ifstate(state)
int state;
{
 switch (state)
  { case IFSTATE_TRUE:
       outputs("TRUE");
       break;
    case IFSTATE_FALSE:
       outputs("FALSE");
       break;
    case IFSTATE_STAYFALSE:
       outputs("STAYFALSE");
       break;
    default:
       outputs("BAD");
       outputd(state);
       break;
  }
}
#endif

void do_elif(int expr_sharp)
{
 char c;
 char d;
 int e;

#ifdef DEBUG_IF
 if (debugging)
  { outputc('<');
    outputc(sharp?'#':'@');
    outputs("elif: ");
    fflush(outfile);
  }
#endif
 if (ifstack == 0)
  { err_head();
    fprintf(stderr,"if-less elif converted to normal if\n");
    iffalse();
  }
 if (n_skipped_ifs > 0)
  {
#ifdef DEBUG_IF
    if (debugging)
     { outputs("being skipped, ignoring>");
       fflush(outfile);
     }
#endif
    if (sharp)
     { d = '\0';
       do
	{ c = d;
	  d = Get();
	} while ((c == '\\') || (d != '\n'));
     }
    return;
  }
 if (! sharp)
  { c = getnonspace();
    if (c != '(')
     { err_head();
       fprintf(stderr,"@elif must have ()s\n");
       Push(c);
       ifstack->condstate = IFSTATE_STAYFALSE;
#ifdef DEBUG_IF
       if (debugging)
	{ outputs("forcing STAYFALSE>");
	  fflush(outfile);
	}
#endif
       return;
     }
  }
 e = eval_expr(sharp,0);
#ifdef DEBUG_IF
 if (debugging)
  { outputs("expr ");
    outputd(e);
    outputc(' ');
    fflush(outfile);
  }
#endif
#ifdef DEBUG_IF
 if (debugging)
  { outputs(" top ");
    output_ifstate(ifstack->condstate);
    fflush(outfile);
  }
#endif
 switch (ifstack->condstate)
  { case IFSTATE_TRUE:
       ifstack->condstate = IFSTATE_STAYFALSE;
       break;
    case IFSTATE_FALSE:
       if (e)
	{ ifstack->condstate = IFSTATE_TRUE;
	}
       break;
  }
#ifdef DEBUG_IF
 if (debugging)
  { outputs(" now ");
    output_ifstate(ifstack->condstate);
    outputc('>');
    fflush(outfile);
  }
#endif
}

void do_endif(int expr_sharp)
{
 IF *i;

#ifdef DEBUG_IF
 if (debugging)
  { outputc('<');
    outputc(sharp?'#':'@');
    outputs("endif: ");
    fflush(outfile);
  }
#endif
 if (n_skipped_ifs > 0)
  { n_skipped_ifs --;
#ifdef DEBUG_IF
    if (debugging)
     { outputs("n_skipped -->");
       fflush(outfile);
     }
#endif
  }
 else if (ifstack)
  { i = ifstack->next;
    OLD(ifstack);
    ifstack = i;
#ifdef DEBUG_IF
    if (debugging)
     { outputs("popping stack>");
       fflush(outfile);
     }
#endif
  }
 else
  { err_head();
    fprintf(stderr,"if-less endif\n");
#ifdef DEBUG_IF
    if (debugging)
     { outputc('>');
       fflush(outfile);
     }
#endif
  }
 if (sharp)
  { flush_sharp_line();
  }
}

void iftrue(void)
{
 IF *i;

 i = NEW(IF);
 check_malloc(i);
 i->next = ifstack;
 ifstack = i;
 i->condstate = IFSTATE_TRUE;
#ifdef DEBUG_IF
 if (debugging)
  { outputs("IFTRUE");
    fflush(outfile);
  }
#endif
}

void iffalse(void)
{
 IF *i;

 i = NEW(IF);
 check_malloc(i);
 i->next = ifstack;
 ifstack = i;
 i->condstate = IFSTATE_FALSE;
#ifdef DEBUG_IF
 if (debugging)
  { outputs("IFFALSE");
    fflush(outfile);
  }
#endif
}

int in_false_if(void)
{
 return(ifstack && (ifstack->condstate != IFSTATE_TRUE));
}

void maybe_print(char c)
{
 extern int incldep;

 if (incldep)
  { return;
  }
 if ((c == '\n') || !in_false_if())
  { outputc(c);
  }
}

char *Rindex(char *s, int c);

int nIfiles;
char **Ifiles;

void init_include(void)
{
 nIfiles = 0;
 Ifiles = (char **) malloc(1);
 check_malloc(Ifiles);
}

void Ifile(char *f)
{
 Ifiles = (char **) realloc((char *)Ifiles,(nIfiles+1)*sizeof(char *));
 check_malloc(Ifiles);
 Ifiles[nIfiles] = copyofstr(f);
 check_malloc(Ifiles[nIfiles]);
 nIfiles ++;
}

void do_include(int expr_sharp)
{
 char c;
 char *acc;

 if (in_false_if())
  { if (sharp)
     { flush_sharp_line();
     }
    return;
  }
 while (1)
  { c = getnonspace();
    if (isbsymchar(c))
     { char *cp;
       DEF *d;
       cp = init_accum();
       while (issymchar(c))
	{ accum_char(cp,c);
	  c = Get();
	}
       Push(c);
       cp = accum_result(cp);
       d = find_def(cp);
       if (d)
	{ expand_def(d);
	  n_hits ++;
	}
       else
	{ char *dp;
	  for (dp=cp+strlen(cp);dp>cp;dp--) Push(*dp);
	  n_misses ++;
	}
     }
    else
     { break;
     }
  }
 acc = init_accum();
 if (c == '"')
  { while (1)
     { c = Get();
       if (c == '\n')
	{ Push('\n');
	  c = '"';
	  err_head();
	  fprintf(stderr,"warning: unterminated %cinclude filename\n",
							sharp?'#':'@');
	}
       if (c == '"')
	{ break;
	}
       accum_char(acc,c);
     }
    if (sharp)
     { flush_sharp_line();
     }
    read_include_file(accum_result(acc),1,sharp);
  }
 else if (c == '<')
  { while (1)
     { c = Get();
       if (c == '\n')
	{ Push('\n');
	  c = '>';
	  err_head();
	  fprintf(stderr,"warning: unterminated %cinclude filename\n",
							sharp?'#':'@');
	}
       if (c == '>')
	{ break;
	}
       accum_char(acc,c);
     }
    if (sharp)
     { flush_sharp_line();
     }
    read_include_file(accum_result(acc),0,sharp);
  }
 else
  { free(accum_result(acc));
    err_head();
    fprintf(stderr,"illegal %cinclude filename delimiter\n",sharp?'#':'@');
  }
}

#if defined(THINK_C)
#if defined(DELIM)
#undef DELIM
#endif
#define DELIM ':'
#endif

#if  defined(SYSV)
#else
#if defined(DELIM)
#undef DELIM
#endif
#define DELIM '/'
#endif

#if defined(VISUAL)
#if defined(DELIM)
#undef DELIM
#endif
#define DELIM '\\'
#endif

void read_include_file(char *name, int dohere, int expr_sharp)
{
 FILE *f;
 char *n;
 char temp[1024];
 char *cp;
 extern int incldep;
 extern char *incldep_o;

 f = NULL;
 if (dohere)
  { if ((strcmp(curdir(),".") != 0) && (name[0] != DELIM))
     { sprintf(temp,"%s%c%s",curdir(),DELIM,name);
       n = temp;
     }
    else
     { n = name;
     }
    f = fopen(n,"r");
  }
 if (f == NULL)
  { if (name[0] == DELIM)
     { f = fopen(name,"r");
       n = name;
     }
    else
     { int i;
       n = temp;
       for (i=0;i<nIfiles;i++)
	{ sprintf(temp,"%s%c%s",Ifiles[i],DELIM,name);
	  f = fopen(temp,"r");
	  if (f != NULL)
	   { break;
	   }
	}
     }
  }
 if (f == NULL)
  { err_head();
    fprintf(stderr,"can't find include file %s\n",name);
    free(name);
    return;
  }
 if (incldep)
  { printf("%s: %s\n",incldep_o,n);
  }
 out_at(!sharp,n);
 autodef_file(n);
 autodef_line(1);
 Push('\n');
 Get();
 push_new_file(n,f);
 cp = Rindex(n,DELIM);
 if (cp)
  { *cp = '\0';
    cp = copyofstr(n);
  }
 else
  { cp = copyofstr(".");
  }
 check_malloc(cp);
 *Curdir() = cp;
 free(name);
 return;
}

/*#define DEBUG_IO/**/
/*#define DEBUG_INCL/**/

#if defined(DEBUG_IO) || defined(DEBUG_INCL)
extern int debugging;
#endif

extern int no_line_lines;

int linefirst;
int willbefirst;
int lineno[MAXFILES];
FILE *fstack[MAXFILES];
int fstackdepth;
char *fn[MAXFILES];
int npushed[MAXFILES];
char pushback[MAXFILES][MAX_PUSHBACK];
char *incldir[MAXFILES];
FILE *outfile;

char *cur_pushback;
int cur_npushed;
FILE *cur_fstack;
char *cur_fn;
char *cur_incldir;
int cur_lineno;

typedef struct _mark {
	  struct _mark *link;
	  char *acc;
	  int startdepth;
	  int incs[MAXFILES];
	  int nincs;
	  int nignore; } MARK;

static MARK *marks;
static int atline;
static char *atfile;
static int done_line;
static int nnls;

/* XXX these definitions assume no input chars are 81, 82, 83 */
#define NOEXPAND ((char)0x81)
#define MARKLINE ((char)0x82)
#define MARKLEND ((char)0x83)

void init_io(FILE *f, char *fnname)
{
 register int i;

 fstackdepth = 0;
 cur_pushback = pushback[0];
 cur_npushed = 0;
 cur_fstack = f;
 cur_fn = copyofstr(fnname);
 check_malloc(cur_fn);
 cur_lineno = 1;
 nnls = 0;
 atfile = copyofstr("");
 check_malloc(atfile);
 for (i=0;i<MAXFILES;i++)
  { npushed[i] = 0;
  }
 marks = 0;
}

void init_incldir(char *d)
{
 cur_incldir = copyofstr(d);
 check_malloc(cur_incldir);
}

void change_fstackdepth(int delta)
{
 npushed[fstackdepth] = cur_npushed;
 fstack[fstackdepth] = cur_fstack;
 fn[fstackdepth] = cur_fn;
 incldir[fstackdepth] = cur_incldir;
 lineno[fstackdepth] = cur_lineno;
 fstackdepth += delta;
 cur_pushback = pushback[fstackdepth];
 cur_npushed = npushed[fstackdepth];
 cur_fstack = fstack[fstackdepth];
 cur_fn = fn[fstackdepth];
 cur_incldir = incldir[fstackdepth];
 cur_lineno = lineno[fstackdepth];
}

#define GET() (cur_pushback[--cur_npushed])
#define PUSH() (cur_pushback[cur_npushed++])

char Get(void)
{
 char c;

 linefirst = willbefirst;
 willbefirst = 0;
 while (1)
  { if (cur_npushed > 0)
     { do
	{ c = GET();
	} while (c == NOEXPAND);
       if (c == MARKLINE)
	{ mark_get_line();
	  continue;
	}
       mark_got_from_pushback(c);
#ifdef DEBUG_IO
       if (debugging)
	{ outputc('(');
	  outputs(unctrl(c));
	  outputc(')');
	  fflush(outfile);
	}
#endif
       break;
     }
    else
     { c = getc(cur_fstack);
       if (feof(cur_fstack))
	{ if (fstackdepth > 0)
	   {
#ifdef DEBUG_INCL
	     if (debugging)
	      { outputs("--eof:");
		outputs(cur_fn);
		outputs("--");
	      }
#endif
	     fclose(cur_fstack);
	     free(cur_fn);
	     free(cur_incldir);
	     change_fstackdepth(-1);
	     mark_file_ending();
	     out_at(cur_lineno,cur_fn);
	     autodef_file(curfile());
	     autodef_line(curline());
	     continue;
	   }
	  else
	   { flush_final_nl();
	     save_stats();
#if 0
	     exit(0);
#endif
	     longjmp(cpp_env,1);
	   }
	}
       mark_got_from_file(c);
#ifdef DEBUG_IO
       if (debugging)
	{ outputc('<');
	  outputs(unctrl(c));
	  outputc('>');
	  fflush(outfile);
	}
#endif
       break;
     }
  }
 if (c == '\n')
  { cur_lineno ++;
    autodef_line(curline());
    willbefirst = 1;
  }
 return(c);
}

void push_new_file(char *name, FILE *f)
{
 change_fstackdepth(1);
 cur_fn = copyofstr(name);
 check_malloc(cur_fn);
 cur_lineno = 1;
 cur_fstack = f;
 cur_npushed = 0;
#ifdef DEBUG_INCL
 if (debugging)
  { outputs("--newfile:");
    outputs(name);
    outputs("--");
  }
#endif
 mark_file_beginning();
}

void Push(char c)
{
 if (cur_npushed > MAX_PUSHBACK)
  { fprintf(stderr,"too much pushback\n");
    cur_npushed = 0;
  }
 PUSH() = c;
 mark_charpushed();
 willbefirst = 0;
 if (c == '\n')
  { cur_lineno --;
    autodef_line(curline());
  }
#ifdef DEBUG_IO
 if (debugging)
  { outputc('[');
    outputs(unctrl(c));
    outputc(']');
    fflush(outfile);
  }
#endif
}

char *curfile(void)
{
 return(cur_fn);
}

char **Curfile(void)
{
 return(&cur_fn);
}

int curline(void)
{
 return(cur_lineno);
}

int *Curline(void)
{
 return(&cur_lineno);
}

char *curdir(void)
{
 return(cur_incldir);
}

char **Curdir(void)
{
 return(&cur_incldir);
}

char getnonspace(void)
{
 char c;

 while (isspace(c=Get())) ;
 return(c);
}

char getnonhspace(void)
{
 char c;

 while (ishspace(c=Get())) ;
 return(c);
}

char getnhsexpand(void)
{
 char c;

 while (1)
  { c = getexpand();
    if (ishspace(c))
     { continue;
     }
    if (expr_sharp && (c == '\\'))
     { c = Get();
       if (c == '\n')
	{ continue;
	}
       else
	{ Push(c);
	  c = '\\';
	}
     }
    break;
  }
 return(c);
}

char getexpand(void)
{
 char c;
 char *str;
 DEF *d;

 while (1)
  { c = Get();
    if (c == '/')
     { char d;
       d = Get();
       if (d == '*')
	{ d = '\0';
	  do
	   { c = d;
	     d = Get();
	   } while ((c != '*') || (d != '/'));
	  continue;
	}
       else
	{ Push(d);
	  return('/');
	}
     }
    else if (c == NOEXPAND)
     { c = Get();
       if (issymchar(c))
	{ Push(NOEXPAND);
	}
       return(c);
     }
    else if (! isbsymchar(c))
     {
       return(c);
     }
    str = init_accum();
    do
     { accum_char(str,c);
     } while (issymchar(c=Get()));
    Push(c);
    str = accum_result(str);
    if (d = find_def(str))
     { expand_def(d);
       free(str);
     }
    else
     { int i;
       for (i=strlen(str)-1;i>0;i--)
	{ Push(str[i]);
	}
       Push(NOEXPAND);
       c = str[0];
       free(str);
       return(c);
     }
  }
}

char *read_ctrl(void)
{
 char c;
 char *acc;

 acc = init_accum();
 while (ishspace(c=Get())) ;
 Push(c);
 while (1)
  { c = Get();
    if (islower(c))
     { accum_char(acc,c);
     }
    else
     { Push(c);
       return(accum_result(acc));
     }
  }
}

char *read_ident(void)
{
 char *acc;
 char c;

 c = getnonspace();
 if (! isbsymchar(c))
  { Push(c);
    return(0);
  }
 acc = init_accum();
 do
  { accum_char(acc,c);
  } while (issymchar(c=Get()));
 Push(c);
 return(accum_result(acc));
}

void out_at(int line, char *file)
{
 if ((line-nnls != atline) || strcmp(file,atfile))
  { free(atfile);
    atline = line - nnls;
    atfile = copyofstr(file);
    check_malloc(atfile);
    done_line = 0;
  }
}

void outputc(char c)
{
 extern int incldep;
 static char *white = 0;

 if (incldep)
  { return;
  }
 if (c == '\n')
  { nnls ++;
    if (white)
     { free(accum_result(white));
       white = 0;
     }
  }
 else if ((nnls > 0) && ((c == ' ') || (c == '\t')))
  { if (! white)
     { white = init_accum();
     }
    accum_char(white,c);
  }
 else
  { if ((nnls > 2) || !done_line)
     { atline += nnls;
       nnls = 0;
       if (no_line_lines)
	{ fprintf(outfile,"\n");
	}
       else
	{ fprintf(outfile,"\n# %d \"%s\"\n",atline,atfile);
	}
       done_line = 1;
     }
    for (;nnls;nnls--)
     { atline ++;
       putc('\n',outfile);
     }
    if (white)
     { white = accum_result(white);
       fputs(white,outfile);
       free(white);
       white = 0;
     }
    putc(c,outfile);
  }
}

void flush_final_nl(void)
{
 if (nnls > 0)
  { putc('\n',outfile);
  }
}

void outputs(char *s)
{
 for (;*s;s++)
  { outputc(*s);
  }
}

void outputd(int n)
{
 char temp[64];

 sprintf(temp,"%d",n);
 outputs(temp);
}

void mark_push_here(MARK *,int,char *);

void input_mark(void)
{
 MARK *m;

 m = NEW(MARK);
 check_malloc(m);
 m->link = marks;
 marks = m;
 m->startdepth = fstackdepth;
 m->acc = init_accum();
 m->nignore = 0;
 m->nincs = 0;
 mark_push_here(m,curline(),curfile());
#ifdef DEBUG_IO
 if (debugging)
  { outputs("~MARK~");
  }
#endif
}

void input_unmark(void)
{
 MARK *m;

 if (marks)
  { m = marks;
    marks = m->link;
    free(accum_result(m->acc));
    OLD(m);
#ifdef DEBUG_IO
    if (debugging)
     { outputs("~UNMARK~");
     }
#endif
  }
}

void input_recover(void)
{
 register MARK *m;
 register char *txt;
 register int i;
 register int l;
 char c;

 if (marks)
  { m = marks;
    marks = m->link;
#ifdef DEBUG_IO
    if (debugging)
     { outputs("~RECOVER~");
     }
#endif
    for (;m->nignore>0;m->nignore--)
     { accum_regret(m->acc);
     }
    txt = accum_result(m->acc);
    i = strlen(txt) - 1;
    while (m->nincs > 0)
     { l = m->incs[--m->nincs];
       for (;i>=l;i--)
	{ Push(txt[i]);
	}
       Push(MARKLEND);
       c = txt[i];
       for (i--;txt[i]!=c;i--)
	{ Push(txt[i]);
	}
       Push('A');
       Push('@');
       Push('@');
       Push('@');
       Push('@');
       Push('@');
       Push('@');
       Push('@');
       for (;(txt[i]!='#')&&(txt[i]!='@');i--) ;
       Push(MARKLINE);
       i --;
     }
    for (;i>=0;i--)
     { Push(txt[i]);
     }
    free(txt);
    OLD(m);
  }
}

void mark_file_beginning(void)
{
 register MARK *m;

 for (m=marks;m;m=m->link)
  { m->incs[m->nincs++] = accum_howfar(m->acc);
  }
}

void mark_file_ending(void)
{
 register MARK *m;
 register int i;
 register int to;
 register char *acc;

 for (m=marks;m;m=m->link)
  { if (m->startdepth > fstackdepth)
     { m->startdepth = fstackdepth;
       mark_push_here(m,curline(),curfile());
     }
    else if (m->nincs <= 0)
     { fprintf(stderr,"INTERNAL BUG: nincs<0 in mark_file_ending\n");
       abort();
     }
    else
     { to = m->incs[--m->nincs];
       acc = m->acc;
       for (i=accum_howfar(acc);i>to;i--)
	{ accum_regret(acc);
	}
     }
  }
}

void mark_charpushed(void)
{
 register MARK *m;

 for (m=marks;m;m=m->link)
  { m->nignore ++;
  }
}

void mark_got_from_pushback(char c)
{
 mark_got(c);
}

void mark_got_from_file(char c)
{
 mark_got(c);
}

void mark_got(char c)
{
 register MARK *m;

 for (m=marks;m;m=m->link)
  { if (m->nignore > 0)
     { m->nignore --;
     }
    else
     { accum_char(m->acc,c);
     }
  }
}

void mark_push_here(MARK *m, int l, char *f)
{
 int i;
 char c;

 switch (c=accum_regret(m->acc))
  { case 0:
       break;
    case MARKLEND:
       while (accum_regret(m->acc) != MARKLINE) ;
       break;
    default:
       accum_char(m->acc,c);
       break;
  }
 accum_char(m->acc,MARKLINE);
 for (i=28;i>=0;i-=4)
  { accum_char(m->acc,(char)('@'+((l>>i)&0xf)));
  }
 for (;*f;f++)
  { accum_char(m->acc,*f);
  }
 accum_char(m->acc,MARKLEND);
}

void mark_get_line(void)
{
 int l;
 char *f;
 int i;
 char c;
 MARK *m;

 l = 0;
 for (i=0;i<8;i++)
  { l = (l << 4) + (GET() & 0xf);
  }
 f = init_accum();
 while ((c=GET()) != MARKLEND)
  { accum_char(f,c);
  }
 f = accum_result(f);
 out_at(l,f);
 for (m=marks;m;m=m->link)
  { mark_push_here(m,l,f);
  }
 free(f);
}

int ishspace(char c)
{
 return((c == ' ') || (c == '\t'));
}

int isbsymchar(char c)
{
 return(isalpha(c) || (c == '_'));
}

int issymchar(char c)
{
 return(isalnum(c) || (c == '_') || (c == '$'));
}

void do_line(void)
{
 char c;

 outputc('#');
 while ((c=Get()) != '\n')
  { outputc(c);
  }
}

#define DEBUG_MAIN/**/

extern char **argvec;

char *Index(char *s, int c);
char *Rindex(char *s, int c);

#ifdef DEBUG_MAIN
extern int debugging;
#endif

static char quote;

IF *ifstack;
int n_skipped_ifs;

int keep_comments;
int no_line_lines;
int incldep;
char *incldep_o;
int do_at_ctrls;
extern char *predefs[];
	
#if defined(SYSV)
void main(ac,av)
#else
void cpp_main(int ac, char **av)
#endif
{
 int argno;
 int rest;
 char *cp;
 static FILE *inf;
 static FILE *outf;
 char *inname;
 int backslash;
 int i;
 
 if ( setjmp(cpp_env) ) {
 	if ( inf != stdin )
 		fclose(inf);
 	if ( outf == stdout )
 		fflush(outf);
 	else
 		fclose(outf);
 	return;
 }

 init_symtbl();
#define predef(str) ( (cp=copyofstr("1")), \
		      check_malloc(cp), \
		      define((str),-1,(unsigned char *)cp,DEF_PREDEF) )
 for (i=0;predefs[i];i++)
  { predef(predefs[i]);
  }
 init_include();
 inf = stdin;
 outf = stdout;
 inname = "";
 keep_comments = 0;
 no_line_lines = 0;
 do_at_ctrls = 0;
 incldep = 0;
 argno = 0;
 for (ac--,av++;ac;ac--,av++)
  { if (**av == '-')
     { rest = 0;
       if (!strcmp(*av,"-undef"))
	{ undef_predefs();
	}
       else
	{ for (++*av;(**av)&&!rest;++*av)
	   { switch (**av)
	      { case 'C':
		   keep_comments = 1;
		   break;
		case 'D':
		   rest = 1;
		   ++*av;
		   if (strcmp(*av,"@") == 0)
		    { do_at_ctrls = 1;
		    }
		   else
		    { cp = Index(*av,'=');
		      if (cp)
		       { if (cp != *av)
			  { char *dp;
			    *cp++ = '\0';
			    dp = copyofstr(cp);
			    check_malloc(dp);
			    define(*av,-1,(unsigned char *)dp,DEF_CMDLINE);
			  }
			 else
			  { fprintf(stderr,"Must give a name for -D\n");
			  }
		       }
		      else
		       { char *dp;
			 dp = copyofstr("1");
			 check_malloc(dp);
			 define(*av,-1,(unsigned char *)dp,DEF_CMDLINE);
		       }
		    }
		   break;
		case 'E':
		   break;
		case 'I':
		   rest = 1;
		   ++*av;
		   if (**av)
		    { Ifile(*av);
		    }
		   else
		    { fprintf(stderr,"Must give a directory name for -I\n");
		    }
		   break;
		case 'M':
		   incldep = 1;
		   break;
		case 'P':
		   no_line_lines = 1;
		   break;
		case 'R':
		   break;
		case 'U':
		   rest = 1;
		   ++*av;
		   if (**av)
		    { undef(*av);
		    }
		   else
		    { fprintf(stderr,"Must give a name for -U\n");
		    }
		   break;
		default:
		   fprintf(stderr,"Unknown flag -%c\n",**av);
		   break;
	      }
	   }
	}
     }
    else
     { switch (argno++)
	{ case 0:
	     if (strcmp(*av,"-") != 0)
	      { FILE *f;
		f = fopen(*av,"r");
		if (f == NULL)
		 { fprintf(stderr,"Cannot open source file %s\n",*av);
		 }
		else
		 { inf = f;
		   inname = *av;
		 }
	      }
	     break;
	  case 1:
	     if (strcmp(*av,"-") != 0)
	      { FILE *f;
		f = fopen(*av,"w");
		if (f == NULL)
		 { fprintf(stderr,"Can't create %s\n",*av);
		 }
		else
		 { outf = f;
		 }
	      }
	     break;
	  default:
	     fprintf(stderr,"Extra name %s ignored\n",*av);
	     break;
	}
     }
  }
 if (incldep && !inname[0])
  { fprintf(stderr,"No input file for -M flag\n");
#if 0
    exit(1);
#endif
	longjmp(cpp_env,1);
  }
 if (do_at_ctrls)
  { predef("at_sign_ctrls");
  }
 Ifile("/local/include");
 Ifile("/usr/include");
 willbefirst = 1;
 quote = 0;
 backslash = 0;
 init_io(inf,inname);
 outfile = outf;
 ifstack = 0;
 n_skipped_ifs = 0;
 cp = Rindex(inname,'/');
 if (cp)
  { *cp = '\0';
    init_incldir(inname);
    *cp = '/';
  }
 else
  { init_incldir(".");
  }
 autodef_file(inname);
 autodef_line(1);
 out_at(1,inname);
 if (incldep)
  { char *dp;
    cp = Rindex(inname,'/');
    if (cp)
     { cp ++;
     }
    else
     { cp = inname;
     }
    dp = cp + strlen(cp) - 2;
    if ((dp < cp) || strcmp(dp,".c"))
     { fprintf(stderr,"Missing .c in %s\n",inname);
#if 0
       exit(1);
#endif
       longjmp(cpp_env,1);
     }
    incldep_o = copyofstr(cp);
    incldep_o[dp+1-cp] = 'o';
    fprintf(outfile,"%s: ",incldep_o);
    fprintf(outfile,"%s\n",inname);
  }
 init_stats();
 while (1)
  { char c;
    int haddigit;
    c = Get();
    if (backslash)
     { maybe_print(c);
       backslash = 0;
       continue;
     }
    if (!incldep && (isdigit(c) || (c == '.')))
     { haddigit = 0;
       while (isdigit(c) || (c == '.'))
	{ haddigit |= isdigit(c);
	  maybe_print(c);
	  c = Get();
	}
       if (haddigit && ((c == 'e') || (c == 'E')))
	{ maybe_print(c);
	  c = Get();
	  while (isdigit(c))
	   { maybe_print(c);
	     c = Get();
	   }
	}
       Push(c);
       continue;
     }
    if (quote)
     { if (c == '\\')
	{ maybe_print(c);
	  backslash = 1;
	  continue;
	}
       else if ((c == quote) || (c == '\n'))
	{ maybe_print(c);
	  quote = 0;
	  continue;
	}
       else
	{ maybe_print(c);
	  continue;
	}
     }
    if (c == '\\') /* this weirdness is Reiser semantics.... */
     { backslash = 1;
       maybe_print(c);
       continue;
     }
    if ((c == '\'') || (c == '"'))
     { quote = c;
       maybe_print(c);
     }
    else if (linefirst && (c == '#'))
     { do_sharp();
     }
    else if (do_at_ctrls && (c == '@'))
     { do_at();
     }
    else if (! incldep)
     { if (isbsymchar(c) && !in_false_if())
	{ char *cp;
	  DEF *d;
	  cp = init_accum();
	  while (issymchar(c))
	   { accum_char(cp,c);
	     c = Get();
	   }
	  Push(c);
	  cp = accum_result(cp);
#ifdef DEBUG_MAIN
	  if (debugging)
	   { outputs("<word:");
	     outputs(cp);
	     outputs(">");
	   }
#endif
	  d = find_def(cp);
	  if (d)
	   { expand_def(d);
	     n_hits ++;
	   }
	  else
	   { for (;*cp;cp++)
	      { maybe_print(*cp);
	      }
	     n_misses ++;
	   }
	}
       else if (c == '/')
	{ char d;
	  d = Get();
	  if (d == '*')
	   { d = '\0';
	     if (keep_comments)
	      { maybe_print('/');
		maybe_print('*');
	      }
	     do
	      { c = d;
		d = Get();
		if ((d == '\n') || keep_comments)
		 { maybe_print(d);
		 }
	      } while ((c != '*') || (d != '/'));
	   }
	  else
	   { Push(d);
	     maybe_print(c);
	   }
	}
       else
	{ maybe_print(c);
	}
     }
  }
}

void do_pragma(void)
{
 char c;

 if (in_false_if())
  { while ((c=Get()) != '\n') ;
  }
 else
  { outputc('#');
    outputc('p');
    outputc('r');
    outputc('a');
    outputc('g');
    outputc('m');
    outputc('a');
    outputc(' ');
    while ((c=Get()) != '\n')
     { outputc(c);
     }
  }
}

char *predefs[] = {
		    "mc68000",
		    "unix",
		    "NeXT",
		    "__MACH__",
		    0
};

void do_set(void)
{
 char *mac;
 char c;
 char temp[64];

 mac = read_ident();
 if (! mac)
  { err_head();
    fprintf(stderr,"@set: missing/illegal macro name\n");
    return;
  }
 if (! in_false_if())
  { char *cp;
    c = getnonspace();
    if (c != '(')
     { err_head();
       fprintf(stderr,"@set must have ()s\n");
       Push(c);
       return;
     }
    sprintf(temp,"%d",eval_expr(0,1));
    undef(mac);
    cp = copyofstr(temp);
    check_malloc(cp);
    define(mac,-1,(unsigned char *)cp,DEF_DEFINE);
  }
 free(mac);
}

void do_sharp(void)
{
 char *w;
 char c;

 w = read_ctrl();
 if (strcmp(w,"ifdef") == 0)
  { do_ifdef(1);
  }
 else if (strcmp(w,"ifndef") == 0)
  { do_ifndef(1);
  }
 else if (strcmp(w,"if") == 0)
  { do_if(1);
  }
 else if (strcmp(w,"else") == 0)
  { do_else(1);
  }
 else if (strcmp(w,"elif") == 0)
  { do_elif(1);
  }
 else if (strcmp(w,"endif") == 0)
  { do_endif(1);
  }
 else if (strcmp(w,"include") == 0)
  { do_include(1);
  }
 else if (strcmp(w,"define") == 0)
  { do_define(1,0);
  }
 else if (strcmp(w,"undef") == 0)
  { do_undef(1);
  }
 else if (strcmp(w,"line") == 0)
  { do_line();
  }
 else if (strcmp(w,"pragma") == 0)
  { do_pragma();
  }
 else if (strcmp(w,"") == 0)
  {
  }
 else
  { int isnull;
    isnull = 0;
    if (strcmp(w,"") == 0)
     { c = Get();
       if (c != '\n')
	{ Push(c);
	}
       else
	{ isnull = 1;
	}
     }
    if (!isnull && !in_false_if())
     { err_head();
       fprintf(stderr,"unknown control `%s'\n",w);
       flush_sharp_line();
     }
  }
 maybe_print('\n');
 free(w);
}

void flush_sharp_line(void)
{
 int quote;
 int backslash;
 int comment;
 int lastc;
 int c;

 quote = 0;
 backslash = 0;
 comment = 0;
 c = 'x';
 while (1)
  { lastc = c;
    c = Get();
    if (backslash)
     { backslash = 0;
       continue;
     }
    if (comment)
     { if (c == '\\')
	{ backslash = 1;
	}
       else if ((c == '/') && (lastc == '*'))
	{ comment = 0;
	}
       continue;
     }
    if (quote)
     { if (c == '\\')
	{ backslash = 1;
	}
       else if (c == quote)
	{ quote = 0;
	}
     }
    switch (c)
     { case '\\':
	  backslash = 1;
	  continue;
	  break;
       case '"': case '\'':
	  quote = c;
	  continue;
	  break;
       case '*':
	  comment = (lastc == '/');
	  continue;
	  break;
       default:
	  continue;
	  break;
       case '\n':
	  break;
     }
    break;
  }
}


int n_defines;
int n_undefines;
int n_hits;
int n_misses;

void init_stats(void)
{
 n_defines = 0;
 n_undefines = 0;
 n_hits = 0;
 n_misses = 0;
}

void save_stats(void)
{
 FILE *f;

 f = fopen("/@larry/u1/mouse/Mouse-C/cpp.stats","a");
 if (f)
  { fprintf(f,"%d def, %d undef, %d hits, %d misses\n",
			n_defines,n_undefines,n_hits,n_misses);
    fclose(f);
  }
}

DEF **symtbl;
int symtbl_size;
int n_in_table;

static int size_index;
static int sizes[] = { 7, 37, 179, 719, 2003, 8009, 30011, 120011, 0 };

static int hash(unsigned char *s)
{
 register unsigned int i;

 for (i=0;*s;s++)
  { i = (i >> 8) + (i << 3) + *s;
  }
 i &= ~0x80000000;
 return(i%symtbl_size);
}

void init_symtbl(void)
{
 int i;

 symtbl_size = sizes[size_index=0];
 symtbl = (DEF **) malloc(symtbl_size*sizeof(DEF *));
 check_malloc(symtbl);
 for (i=0;i<symtbl_size;i++)
  { symtbl[i] = 0;
  }
 n_in_table = 0;
}

static void rehash_up(void)
{
 int osize;
 DEF **otbl;
 int i;
 DEF *d;
 DEF *n;
 int h;

 if ((sizes[size_index] == 0) || (sizes[++size_index] == 0))
  { return;
  }
 osize = symtbl_size;
 otbl = symtbl;
 symtbl_size = sizes[size_index];
 symtbl = (DEF **) malloc(symtbl_size*sizeof(DEF *));
 check_malloc(symtbl);
 for (i=0;i<symtbl_size;i++)
  { symtbl[i] = 0;
  }
 for (i=0;i<osize;i++)
  { for (d=otbl[i];d;d=n)
     { n = d->link;
       h = hash((unsigned char *)d->name);
       d->link = symtbl[h];
       symtbl[h] = d;
     }
  }
 free((char *)otbl);
}

void define(char *name, int nargs, unsigned char *repl, int how)
{
 int h;
 DEF *d;
 char *n;

 n = copyofstr(name);
 h = hash((unsigned char *)n);
 for (d=symtbl[h];d;d=d->link)
  { if (strcmp(d->name,n) == 0)
     { break;
     }
  }
 if (d)
  { if ( (nargs != d->nargs) || strcmp((char *)repl,(char *)d->repl) )
     { err_head();
       fprintf(stderr,"%s redefined\n",n);
     }
    free((char *)d->repl);
    free(d->name);
    d->name = n;
    d->nargs = nargs;
    d->repl = repl;
    d->how = how;
  }
 else
  { d = NEW(DEF);
    check_malloc(d);
    d->name = n;
    d->nargs = nargs;
    d->repl = repl;
    d->how = how;
    d->link = symtbl[h];
    symtbl[h] = d;
    n_in_table ++;
    if (n_in_table > 2*symtbl_size)
     { rehash_up();
     }
  }
 if (strcmp(n,"at_sign_ctrls") == 0)
  { extern int do_at_ctrls;
    do_at_ctrls = 1;
  }
}

int undef(char *name)
{
 int h;
 DEF **D;
 DEF *d;
 int rv;

 h = hash((unsigned char *)name);
 for (D=symtbl+h;*D;D= &(*D)->link)
  { if (strcmp((*D)->name,name) == 0)
     { break;
     }
  }
 rv = 0;
 if (d = *D)
  { *D = d->link;
    free(d->name);
    free((char *)d->repl);
    OLD(d);
    n_in_table --;
    rv = 1;
  }
 if (strcmp(name,"at_sign_ctrls") == 0)
  { extern int do_at_ctrls;
    do_at_ctrls = 0;
  }
  return rv;
}

DEF *find_def(char *name)
{
 int h;
 DEF *d;

 h = hash((unsigned char *)name);
 for (d=symtbl[h];d;d=d->link)
  { if (strcmp(d->name,name) == 0)
     { break;
     }
  }
 return(d);
}

void defd(char *name, int value)
{
 char temp[64];
 char *cp;

 sprintf(temp,"%d",value);
 undef(name);
 cp = copyofstr(temp);
 check_malloc(cp);
 define(name,-1,(unsigned char *)cp,DEF_DEFINE);
}

void undef_predefs(void)
{
 int h;
 DEF **D;
 DEF *d;

 for (h=symtbl_size-1;h>=0;h--)
  { D = symtbl + h;
    while (*D)
     { d = *D;
       if (d->how == DEF_PREDEF)
	{ free(d->name);
	  free((char *)d->repl);
	  *D = d->link;
	  OLD(d);
	  n_in_table --;
	}
       else
	{ D = &d->link;
	}
     }
  }
}

char *_unctrl[0400] = { "^@",
			"^A",
			"^B",
			"^C",
			"^D",
			"^E",
			"^F",
			"^G",
			"^H",
			"^I",
			"^J",
			"^K",
			"^L",
			"^M",
			"^N",
			"^O",
			"^P",
			"^Q",
			"^R",
			"^S",
			"^T",
			"^U",
			"^V",
			"^W",
			"^X",
			"^Y",
			"^Z",
			"^[",
			"^\\",
			"^]",
			"^^",
			"^_",
			" ",
			"!",
			"\"",
			"#",
			"$",
			"%",
			"&",
			"'",
			"(",
			")",
			"*",
			"+",
			",",
			"-",
			".",
			"/",
			"0",
			"1",
			"2",
			"3",
			"4",
			"5",
			"6",
			"7",
			"8",
			"9",
			":",
			";",
			"<",
			"=",
			">",
			"?",
			"@",
			"A",
			"B",
			"C",
			"D",
			"E",
			"F",
			"G",
			"H",
			"I",
			"J",
			"K",
			"L",
			"M",
			"N",
			"O",
			"P",
			"Q",
			"R",
			"S",
			"T",
			"U",
			"V",
			"W",
			"X",
			"Y",
			"Z",
			"[",
			"\\",
			"]",
			"^",
			"_",
			"`",
			"a",
			"b",
			"c",
			"d",
			"e",
			"f",
			"g",
			"h",
			"i",
			"j",
			"k",
			"l",
			"m",
			"n",
			"o",
			"p",
			"q",
			"r",
			"s",
			"t",
			"u",
			"v",
			"w",
			"x",
			"y",
			"z",
			"{"/*}*/,
			"|",
			/*{*/"}",
			"~",
			"^?",
			"|^@",
			"|^A",
			"|^B",
			"|^C",
			"|^D",
			"|^E",
			"|^F",
			"|^G",
			"|^H",
			"|^I",
			"|^J",
			"|^K",
			"|^L",
			"|^M",
			"|^N",
			"|^O",
			"|^P",
			"|^Q",
			"|^R",
			"|^S",
			"|^T",
			"|^U",
			"|^V",
			"|^W",
			"|^X",
			"|^Y",
			"|^Z",
			"|^[",
			"|^\\",
			"|^]",
			"|^^",
			"|^_",
			"| ",
			"|!",
			"|\"",
			"|#",
			"|$",
			"|%",
			"|&",
			"|'",
			"|(",
			"|)",
			"|*",
			"|+",
			"|,",
			"|-",
			"|.",
			"|/",
			"|0",
			"|1",
			"|2",
			"|3",
			"|4",
			"|5",
			"|6",
			"|7",
			"|8",
			"|9",
			"|:",
			"|;",
			"|<",
			"|=",
			"|>",
			"|?",
			"|@",
			"|A",
			"|B",
			"|C",
			"|D",
			"|E",
			"|F",
			"|G",
			"|H",
			"|I",
			"|J",
			"|K",
			"|L",
			"|M",
			"|N",
			"|O",
			"|P",
			"|Q",
			"|R",
			"|S",
			"|T",
			"|U",
			"|V",
			"|W",
			"|X",
			"|Y",
			"|Z",
			"|[",
			"|\\",
			"|]",
			"|^",
			"|_",
			"|`",
			"|a",
			"|b",
			"|c",
			"|d",
			"|e",
			"|f",
			"|g",
			"|h",
			"|i",
			"|j",
			"|k",
			"|l",
			"|m",
			"|n",
			"|o",
			"|p",
			"|q",
			"|r",
			"|s",
			"|t",
			"|u",
			"|v",
			"|w",
			"|x",
			"|y",
			"|z",
			"|{"/*}*/,
			"||",
			/*{*/"|}",
			"|~",
			"|^?" };

char *_Unctrl[0400] = { "^@",
			"^A",
			"^B",
			"^C",
			"^D",
			"^E",
			"^F",
			"^G",
			"^H",
			"^I",
			"^J",
			"^K",
			"^L",
			"^M",
			"^N",
			"^O",
			"^P",
			"^Q",
			"^R",
			"^S",
			"^T",
			"^U",
			"^V",
			"^W",
			"^X",
			"^Y",
			"^Z",
			"^[",
			"^\\",
			"^]",
			"^^",
			"^_",
			"sp",
			"!",
			"\"",
			"#",
			"$",
			"%",
			"&",
			"'",
			"(",
			")",
			"*",
			"+",
			",",
			"-",
			".",
			"/",
			"0",
			"1",
			"2",
			"3",
			"4",
			"5",
			"6",
			"7",
			"8",
			"9",
			":",
			";",
			"<",
			"=",
			">",
			"?",
			"@",
			"A",
			"B",
			"C",
			"D",
			"E",
			"F",
			"G",
			"H",
			"I",
			"J",
			"K",
			"L",
			"M",
			"N",
			"O",
			"P",
			"Q",
			"R",
			"S",
			"T",
			"U",
			"V",
			"W",
			"X",
			"Y",
			"Z",
			"[",
			"\\",
			"]",
			"^",
			"_",
			"`",
			"a",
			"b",
			"c",
			"d",
			"e",
			"f",
			"g",
			"h",
			"i",
			"j",
			"k",
			"l",
			"m",
			"n",
			"o",
			"p",
			"q",
			"r",
			"s",
			"t",
			"u",
			"v",
			"w",
			"x",
			"y",
			"z",
			"{"/*}*/,
			"|",
			/*{*/"}",
			"~",
			"^?",
			"|^@",
			"|^A",
			"|^B",
			"|^C",
			"|^D",
			"|^E",
			"|^F",
			"|^G",
			"|^H",
			"|^I",
			"|^J",
			"|^K",
			"|^L",
			"|^M",
			"|^N",
			"|^O",
			"|^P",
			"|^Q",
			"|^R",
			"|^S",
			"|^T",
			"|^U",
			"|^V",
			"|^W",
			"|^X",
			"|^Y",
			"|^Z",
			"|^[",
			"|^\\",
			"|^]",
			"|^^",
			"|^_",
			"|sp",
			"|!",
			"|\"",
			"|#",
			"|$",
			"|%",
			"|&",
			"|'",
			"|(",
			"|)",
			"|*",
			"|+",
			"|,",
			"|-",
			"|.",
			"|/",
			"|0",
			"|1",
			"|2",
			"|3",
			"|4",
			"|5",
			"|6",
			"|7",
			"|8",
			"|9",
			"|:",
			"|;",
			"|<",
			"|=",
			"|>",
			"|?",
			"|@",
			"|A",
			"|B",
			"|C",
			"|D",
			"|E",
			"|F",
			"|G",
			"|H",
			"|I",
			"|J",
			"|K",
			"|L",
			"|M",
			"|N",
			"|O",
			"|P",
			"|Q",
			"|R",
			"|S",
			"|T",
			"|U",
			"|V",
			"|W",
			"|X",
			"|Y",
			"|Z",
			"|[",
			"|\\",
			"|]",
			"|^",
			"|_",
			"|`",
			"|a",
			"|b",
			"|c",
			"|d",
			"|e",
			"|f",
			"|g",
			"|h",
			"|i",
			"|j",
			"|k",
			"|l",
			"|m",
			"|n",
			"|o",
			"|p",
			"|q",
			"|r",
			"|s",
			"|t",
			"|u",
			"|v",
			"|w",
			"|x",
			"|y",
			"|z",
			"|{"/*}*/,
			"||",
			/*{*/"|}",
			"|~",
			"|^?" };

void do_undef(int expr_sharp)
{
 char *mac;

 if (! in_false_if())
  { mac = read_ident();
    if (! mac)
     { err_head();
       fprintf(stderr,"missing/illegal macro name\n");
     }
    else
     { if (undef(mac))
	{ n_undefines ++;
	}
     }
  }
 if (sharp)
  { flush_sharp_line();
  }
}
/*#define DEBUG_WHILE/**/

#ifdef DEBUG_WHILE
extern int debugging;
#endif

void do_while(void)
{
 input_mark();
 do_if(0);
}

void do_endwhile(void)
{
 if (in_false_if())
  { do_endif(0);
    input_unmark();
#ifdef DEBUG_WHILE
    if (debugging)
     { outputs("//endwhile:");
       outputd(curline());
       outputs(",");
       outputs(curfile());
       outputs("\\\\");
     }
#endif
    out_at(curline(),curfile());
  }
 else
  { do_endif(0);
    input_recover();
    input_mark();
    do_if(0);
  }
}

char *Index(char *s, int c)
{
	return strchr(s,c);
}

char *Rindex(char *s, int c)
{
	return strrchr(s,c);
}

void Bcopy(char *from, char *to, int len)
{
#if defined(GO32)
	bcopy(from,to,len);
#else
	memmove((void *)from,(void *)to,(int)len);
#endif
}
