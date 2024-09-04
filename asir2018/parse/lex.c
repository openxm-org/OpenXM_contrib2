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
 * $OpenXM: OpenXM_contrib2/asir2018/parse/lex.c,v 1.5 2020/10/06 06:45:37 noro Exp $
*/
#include <ctype.h>
#include "ca.h"
#include "al.h"
#include "base.h"
#include "parse.h"
#include <sys/types.h>
#include <sys/stat.h>
#if defined(VISUAL) || defined(__MINGW32__)
#include "ytab.h"
#else
#include "y.tab.h"
#endif
#if FEP
#include <editline/readline.h>
#endif

extern int do_fep, do_file;

static int Getc();
static void Ungetc(int c);
static void Gets(char *s);
static int skipspace();

extern INFILE asir_infile;
extern struct oTKWD kwd[];
extern Obj VOIDobj;

extern int main_parser;
extern char *parse_strp;

#define NBUFSIZ (BUFSIZ*10)
#define TBUFSIZ (BUFSIZ)

#define REALLOC_NBUF \
if ( i >= nbufsize ) {\
  nbufsize += NBUFSIZ;\
  if ( nbuf == nbuf0 ) {\
    nbuf = (char *)MALLOC_ATOMIC(nbufsize);\
    bcopy(nbuf0,nbuf,nbufsize-NBUFSIZ);\
  } else\
    nbuf = REALLOC(nbuf,nbufsize);\
}

#define REALLOC_TBUF \
if ( i >= tbufsize ) {\
  tbufsize += TBUFSIZ;\
  if ( tbuf == tbuf0 ) {\
    tbuf = (char *)MALLOC_ATOMIC(tbufsize);\
    bcopy(tbuf0,tbuf,tbufsize-TBUFSIZ);\
  } else\
    tbuf = REALLOC(tbuf,tbufsize);\
}

#define READ_ALNUM_NBUF \
while ( 1 ) {\
  c = Getc();\
  if ( isalnum(c) ) {\
    REALLOC_NBUF nbuf[i++] = c;\
  } else\
    break;\
}

#define READ_DIGIT_NBUF \
while ( 1 ) {\
  c = Getc();\
  if ( isdigit(c) ) {\
    REALLOC_NBUF nbuf[i++] = c;\
  } else\
    break;\
}

int yylex()
{
#define yylvalp (&yylval)
  int c,c1;
  int *ptr;
  char *cptr;
  int d,i,j;
  char nbuf0[NBUFSIZ],tbuf0[TBUFSIZ];
  char *nbuf, *tbuf;
  int nbufsize, tbufsize;
  Z n,n1;
  Q q;
  Obj r;
  int floatingpoint = 0;
  double dbl;
  Real real;
//  double atof();
  extern int bigfloat;

  
  /* initialize buffer pointers */
  nbuf = nbuf0; tbuf = tbuf0;
  nbufsize = NBUFSIZ; tbufsize = TBUFSIZ;

  switch ( c = skipspace() ) {
    case EOF :  
      asir_terminate(2); break;
    case ';' :
      if ( do_fep && asir_infile->fp != 0 && isatty(fileno(asir_infile->fp)) ) {
        return c;
      } else {
        if ( (c1 = Getc()) == c ) return SEMISEMI;
        else { Ungetc(c1); return c; }
      }
      break;
    case '0' :
      while ( ( c = Getc() ) == '0' );
      if ( c == '.' ) {
        Ungetc(c); c = '0';
      } else if ( c == 'x' || c == 'X' ) {
        for ( i = 0; i < 8; i++ ) 
          nbuf[i] = '0';
        READ_ALNUM_NBUF
        Ungetc(c); REALLOC_NBUF nbuf[i] = 0;
        hextoz(nbuf,&n1); r = (Obj)n1;
        yylvalp->p = (pointer)r;
        return ( FORMULA );
      } else if ( c == 'b' || c == 'B' ) {
        for ( i = 0; i < 32; i++ ) 
          nbuf[i] = '0';
        READ_ALNUM_NBUF
        Ungetc(c); REALLOC_NBUF nbuf[i] = 0;
        binarytoz(nbuf,&n1); r = (Obj)n1;
        yylvalp->p = (pointer)r;
        return ( FORMULA );
      } else if ( !isdigit(c) ) {
        yylvalp->p = 0; Ungetc(c); 
        return ( FORMULA );
      }
      break;
    case '\'' :
      for ( i = 0; ; i++ ) {
        c = Getc();
        if ( c == '\'' )
          break;
        if ( c == '\\' )
          c = Getc();
        REALLOC_TBUF tbuf[i] = c;
      }
      REALLOC_TBUF tbuf[i] = 0;
      cptr = (char *)MALLOC(strlen(tbuf)+1); strcpy(cptr,tbuf);
      yylvalp->p = (pointer)cptr;
      return LCASE; break;
    case '"' :
      i = 0;
      do {
        c = Getc();
                if ( c == '\n' ) asir_infile->ln++;
        if ( c == '\\' ) {
          c1 = Getc();
          if ( c1 == 'n' ) {
            c1 = '\n';
          }else if ( c1 == 'r' ) {
            c1 = '\r';
                    }else if ( c1 == 't' ) {
                        c1 = '\t';
                    }else if ( isdigit(c1) ){
                        d = c1 - '0';
                        c1 = Getc();
                        if ( isdigit(c1) ) {
                             d = 8*d + (c1 - '0');
                             c1 = Getc();
                             if ( isdigit(c1) ) {
                                 d = 8*d + (c1 - '0');
                             }else {
                                 Ungetc(c1);
                             }
                        }else {
                            Ungetc(c1);
                        }
                        c1 = d;
                    }
          REALLOC_NBUF nbuf[i++] = c1;
        } else {
          REALLOC_NBUF nbuf[i++] = c;
        }
      } while ( c != '"' );
      nbuf[i-1] = 0; /* REALLOC_NBUF is not necessary */
      cptr = (char *)MALLOC(strlen(nbuf)+1);
      strcpy(cptr,nbuf); yylvalp->p = (pointer) cptr;
      return ( STR ); break;
    case '>': case '<': case '=': case '!':
      if ( (c1 = Getc()) == '=' )
        switch ( c ) {
          case '>': yylvalp->i = (int)C_GE; break;
          case '<': yylvalp->i = (int)C_LE; break;
          case '=': yylvalp->i = (int)C_EQ; break;
          case '!': yylvalp->i = (int)C_NE; break;
          default: break;
        }
      else if ( (c == '<' && c1 == '<') || (c == '>' && c1 == '>') )
        return c;
      else {
        Ungetc(c1);
        switch ( c ) {
          case '>': yylvalp->i = (int)C_GT; break;
          case '<': yylvalp->i = (int)C_LT; break;
          default: return c; break;
        }
      }
      return CMP; break;
    case '+': case '-': case '*': case '/': case '%': case '^': 
    case '|': case '&':
      switch ( c ) {
        case '+': yylvalp->p = (pointer)addfs; break;
        case '-': yylvalp->p = (pointer)subfs; break;
        case '*': yylvalp->p = (pointer)mulfs; break;
        case '/': yylvalp->p = (pointer)divfs; break;
        case '%': yylvalp->p = (pointer)remfs; break;
        case '^': yylvalp->p = (pointer)pwrfs; break;
        default: break;
      }
      if ( (c1 = Getc()) == c )
        switch ( c ) {
          case '+': case '-': return SELF; break;
          case '|': return OR; break;
          case '&': return AND; break;
          default: Ungetc(c1); return c; break;
        }
      else if ( c1 == '=' ) 
        return BOPASS;
      else if ( (c == '-') && (c1 == '>') )
        return PNT;
      else {
        Ungetc(c1); return c;
      }
      break;
    default :
      break;
  }
  if ( isdigit(c) ) {
    for ( i = 0; i < DLENGTH; i++ ) 
      nbuf[i] = '0';
    REALLOC_NBUF nbuf[i++] = c;
    READ_DIGIT_NBUF
    if ( c == '.' ) {
      floatingpoint = 1;

      REALLOC_NBUF nbuf[i++] = c;
      READ_DIGIT_NBUF
      if ( c == 'e' || c == 'E' ) {
        REALLOC_NBUF nbuf[i++] = c;
        c = Getc();
        if ( (c == '+') || (c == '-') ) {
          REALLOC_NBUF nbuf[i++] = c;
        } else
          Ungetc(c);
        READ_DIGIT_NBUF
      }
    } else if ( c == 'e' || c == 'E' ) {
      floatingpoint = 1;
      REALLOC_NBUF nbuf[i++] = c;
      c = Getc();
      if ( (c == '+') || (c == '-') ) {
        REALLOC_NBUF nbuf[i++] = c;
      } else
        Ungetc(c);
      READ_DIGIT_NBUF
    }
    Ungetc(c); REALLOC_NBUF nbuf[i] = 0;
    if ( floatingpoint ) {
      if ( !bigfloat ) {
        dbl = (double)atof(nbuf+DLENGTH);
        MKReal(dbl,real); r = (Obj)real;
      } else
        strtobf(nbuf,(BF *)&r);
    } else {
      mpz_t z;

      mpz_init(z);
      mpz_set_str(z,nbuf,10);
      MPZTOZ(z,n1); r = (Obj)n1;
    }
    yylvalp->p = (pointer)r;
    return ( FORMULA );
  } else if ( isalpha(c) || c == ':' || c == '_' ) {
    if ( c == ':' ) {
      c1 = Getc();
      if ( c1 != ':' ) {
        Ungetc(c1);
        return c;
      }
      c1 = Getc();
      if ( !isalpha(c1) ) {
        Ungetc(c1);
        return COLONCOLON;
      }
      i = 0;
      tbuf[i++] = ':';
      tbuf[i++] = ':';
      tbuf[i++] = c1;
    } else {
      i = 0;
      tbuf[i++] = c;
    }
    while ( 1 ) {
      c = Getc();
      if ( isalpha(c)||isdigit(c)||(c=='_')||(c=='.') ) {
        REALLOC_TBUF tbuf[i++] = c;
      } else
        break;
    }
    REALLOC_TBUF tbuf[i] = 0; Ungetc(c); 
    if ( isupper(tbuf[0]) || (tbuf[0] == '_' && isupper(tbuf[1])) ) {
      cptr = (char *)MALLOC(strlen(tbuf)+1); strcpy(cptr,tbuf);
      yylvalp->p = (pointer)cptr;
      return UCASE;
    } else {
      for ( i = 0; kwd[i].name && strcmp(tbuf,kwd[i].name); i++ );
      if ( kwd[i].name ) {
        yylvalp->i = asir_infile->ln;
        return kwd[i].token;
      } else {
        cptr = (char *)MALLOC(strlen(tbuf)+1); strcpy(cptr,tbuf);
        yylvalp->p = (pointer)cptr;
        return LCASE;
      }
    }
  } else if ( c == '@' ) {
    if ( isdigit(c = Getc()) ) {
      i = 0;
      nbuf[i++] = c;
      READ_DIGIT_NBUF
      Ungetc(c); REALLOC_NBUF nbuf[i] = 0;
      yylvalp->i = atoi(nbuf);
      return ANS;
    } else if ( c == '@' ) {
      yylvalp->i = MAX(0,APVS->n-1);
      return ANS;
    } else if ( c == '>' ||  c == '<' ||  c == '=' || c == '!' ) {
      if ( (c1 = Getc()) == '=' )
        switch ( c ) {
          case '>': yylvalp->i = (int)L_GE; break;
          case '<': yylvalp->i = (int)L_LE; break;
          case '=': yylvalp->i = (int)L_EQ; break;
          case '!': yylvalp->i = (int)L_NE; break;
          default: break;
        }
      else {
        Ungetc(c1);
        switch ( c ) {
          case '>': yylvalp->i = (int)L_GT; break;
          case '<': yylvalp->i = (int)L_LT; break;
          case '=': yylvalp->i = (int)L_EQ; break;
          case '!': yylvalp->i = (int)L_NOT; return FOP_NOT; break;
          default: break;
        }
      }
      return LOP;
    } else if ( c == '|' ||  c == '&' ) {
      if ( (c1 = Getc()) != c )
        Ungetc(c1);
      switch ( c ) {
        case '|': yylvalp->i = (int)L_OR;
          return FOP_OR; break;
        case '&': yylvalp->i = (int)L_AND;
          return FOP_AND; break;
      }
    } else if ( isalpha(c) ) {
      i = 0;
      tbuf[i++] = '@';
      tbuf[i++] = c;
      while ( 1 ) {
        c = Getc();
        if ( isalpha(c) ) {
          REALLOC_TBUF tbuf[i++] = c;
        } else
          break;
      }
      Ungetc(c); REALLOC_TBUF tbuf[i] = 0;
      if ( !strcmp(tbuf,"@p") )
        return GFPNGEN;
      else if ( !strcmp(tbuf,"@s") )
        return GFSNGEN;
      else if ( !strcmp(tbuf,"@void") ) {
        yylvalp->p = VOIDobj;
        return FORMULA;
      } else if ( !strcmp(tbuf,"@i") ) {
        extern pointer IU;

        yylvalp->p = IU;
        return FORMULA;
      } else if ( !strcmp(tbuf,"@true") ) {
        yylvalp->p = F_TRUE;
        return FORMULA;
      } else if ( !strcmp(tbuf,"@false") ) {
        yylvalp->p = F_FALSE;
        return FORMULA;
      } else if ( !strcmp(tbuf,"@impl") ) {
        yylvalp->i = (int)L_IMPL;
        return FOP_IMPL;
      } else if ( !strcmp(tbuf,"@repl") ) {
        yylvalp->i = (int)L_REPL;
        return FOP_REPL;
      } else if ( !strcmp(tbuf,"@equiv") ) {
        yylvalp->i = (int)L_EQUIV;
        return FOP_EQUIV;
      } else if ( !strcmp(tbuf,"@grlex") ) {
        yylvalp->p = Symbol_grlex;
        return FORMULA;
      } else if ( !strcmp(tbuf,"@glex") ) {
        yylvalp->p = Symbol_glex;
        return FORMULA;
      } else if ( !strcmp(tbuf,"@lex") ) {
        yylvalp->p = Symbol_lex;
        return FORMULA;
      } else {
        cptr = (char *)MALLOC(strlen(tbuf)+1); strcpy(cptr,tbuf);
        yylvalp->p = (pointer)cptr;
        return LCASE;
      }
    } else {
      Ungetc(c);
      return GF2NGEN;
    }
  } else
    return ( c );
  /* XXX */
  return 0;
}

/* for __fpurge() */
#if defined(linux)
#include <stdio_ext.h>
#endif

void purge_stdin()
{
#if defined(__FreeBSD__) || defined(__DARWIN__) || defined(ANDROID)
  fpurge(stdin);
#elif defined(linux)
//  stdin->_IO_read_end = stdin->_IO_read_base;
//  stdin->_IO_read_ptr = stdin->_IO_read_base;
  __fpurge(stdin);
#elif defined(VISUAL_LIB)
  void w_purge_stdin();

  w_purge_stdin();
#elif defined(VISUAL) && _MSC_VER >= 1900
  rewind(stdin);
#elif defined(sparc) || defined(__alpha) || defined(__SVR4) || defined(mips) || defined(VISUAL) || defined(__MINGW32__) || defined(_IBMR2)
  stdin->_ptr = stdin->_base; stdin->_cnt = 0;
#elif (defined(__MACH__) && defined(__ppc__)) || defined(__CYGWIN__) || defined(__FreeBSD__) || defined(__INTERIX)
  stdin->_r = 0; stdin->_p = stdin->_bf._base;
#else
--->FIXIT
#endif
}

static int skipspace() {
  int c,c1;

  for ( c = Getc(); ; )
    switch ( c ) {
      case ' ': case '\t': case '\r':
        c = Getc(); break;
      case '\n':
        c = afternl(); break;
      case '/':
        if ( (c1 = Getc()) == '*' )
          c = aftercomment();
        else {
          Ungetc(c1); return c;
        }
        break;
      default:
        return c; break;
    }
}

int afternl() {
  int c,ac,i,quote;
  char *ptr,*buf0;
  char *av[BUFSIZ];
  static int ilevel = 0;
  char buf[BUFSIZ];

  if ( !ilevel )
    asir_infile->ln++;
  while ( (c = Getc()) == '#' ) {
    Gets(buf);
#define LINE "line"
    if ( !strncmp(buf,LINE,strlen(LINE)) ) buf0 = buf+strlen(LINE);
    else buf0 = buf;
    for ( quote = 0, ptr = buf0; *ptr; ptr++ )
      if ( *ptr == '"' )
        quote = quote ? 0 : 1;
      else if ( quote && (*ptr == ' ') )
        *ptr = '_';
    stoarg(buf0,&ac,av); 
    if ( ac == 3 ) {
      if ( (i = atoi(av[2])) == 1 )
        ilevel++;
      else if ( i == 2 )
        ilevel--;
    }
    if ( !ilevel )
      asir_infile->ln = atoi(av[0]);
  }
  return c;
}

int aftercomment() {
  int c,c1;

  for ( c = Getc(); ; ) {
    if ( c == '\n' ) asir_infile->ln++;
    c1 = Getc();
    if ( (c == '*') && (c1 == '/') )
      return Getc();
    else
      c = c1;
  }
}
      
int myatoi(char *s)
{
  int i,r;
  for ( i = 0, r = 0; i < DLENGTH; i++ ) r = r * 10 + ( s[i] - '0' );
  return ( r );
}

extern int ox_do_copy;
extern int I_am_server;
extern JMP_BUF main_env;
extern int at_root;
extern LIST LastStackTrace;
extern char *CUR_FUNC;

void yyerror(char *s)
{
  STRING fname,name,kwd;
  USINT u;
  NODE t;
  LIST l,l2;

  if ( main_parser ) {
    if ( ox_do_copy ) {
      /* push errors to DebugStack */
    } else {
      if ( asir_infile->fp == stdin )
        fprintf(stderr,"%s\n",s);
      else
        fprintf(stderr,"\"%s\", near line %d: %s\n",asir_infile->name,asir_infile->ln,s);
    }
    if ( I_am_server ) {
      if ( NEXT(asir_infile) ) {
        /* error in a file; record the position */
        MKSTR(fname,asir_infile->name);
        if ( CPVS == GPVS )
          MKSTR(name,"");
        else
          MKSTR(name,CUR_FUNC);
        MKUSINT(u,asir_infile->ln);
        t = mknode(3,fname,name,u); MKLIST(l,t);
        /* line number at the toplevel */
        MKSTR(fname,"toplevel"); MKUSINT(u,at_root);
        t = mknode(2,fname,u); MKLIST(l2,t);
        t = mknode(2,l2,l); 
      } else {
        MKSTR(fname,"toplevel"); MKUSINT(u,asir_infile->ln);
        t = mknode(2,fname,u); MKLIST(l,t);
        t = mknode(1,l);
      }
      MKLIST(l,t);
      MKSTR(kwd,"asir_where"); t = mknode(2,kwd,l);
      MKLIST(LastStackTrace,t);
      set_lasterror(s);
      LONGJMP(main_env,1);
    }
  } else
    fprintf(stderr,"exprparse : %s\n",s);
}

int echoback;

unsigned char encrypt_char(unsigned char);
unsigned char decrypt_char(unsigned char);

int Egetc(FILE *fp)
{
  int c;

  if ( fp ) {
#if FEP
    if ( do_fep && isatty(fileno(fp)) )
      c = readline_getc();
    else
#endif
      c = getc(fp);
#if defined(VISUAL) || defined(__MINGW32__)
    check_intr();
#endif
    if ( c == EOF )
      return c;
    if ( asir_infile->encoded )
      c = decrypt_char((unsigned char)c);
    return c;
  } else {
    c = *parse_strp++;
    if ( !c )
      return EOF;
    else
      return c;
  }
}

void Eungetc(int c,FILE *fp)
{
  if ( fp ) {
    if ( asir_infile->encoded )
      c = (int)encrypt_char((unsigned char)c);
#if FEP
    if ( do_fep && isatty(fileno(fp)) )
      readline_ungetc();
    else
#endif
      ungetc(c,fp);
  } else
    *--parse_strp = c;
}

static int Getc() {
  int c;

  if ( main_parser ) {
    while ( 1 ) {
      if ((c = Egetc(asir_infile->fp)) == EOF)
        if ( NEXT(asir_infile) ) {
          closecurrentinput();
          /* if the input is the top level, generate error */
          if ( !NEXT(asir_infile) )
            error("end-of-file detected during parsing");
          else
            c = Getc();
          break;
        } else if ( asir_infile->fp || do_file ) {
          if ( asir_infile->fp )
            clearerr(asir_infile->fp);
          asir_terminate(2);
        } else {
          error("end-of-line detected during parsing");
        }
      else
        break;
    }
    if ( echoback )
      fputc(c,asir_out);
  } else
    c = *parse_strp++;
  return ( c );
}

static void Ungetc(int c) {
  if ( main_parser ) {
    Eungetc(c,asir_infile->fp);
    if ( echoback )
      fputc('',asir_out);
  } else
    *--parse_strp = c;
}

static void Gets(char *s)
{
  int c;

  while ( (c = Getc()) != '\n' )
    *s++ = c;
  *s = 0;
}

#if FEP

static char *readline_line;
static int readline_nc,readline_index;
// char *readline_console();

int readline_getc()
{
        char buf[BUFSIZ];

        if ( !readline_nc ) {
                free(readline_line);
				readline_line = NULL;
                sprompt(buf);
                readline_line = readline_console(buf);
                readline_nc = strlen(readline_line);
                readline_index = 0;
        }
        readline_nc--;
        return readline_line[readline_index++];
}

void readline_ungetc()
{
        readline_nc++; readline_index--;
}

int asir_history_inhibit_expansion_function(const char *s,int i)
{
	return (i>0)? 1: 0;
}

char *readline_console(char *prompt)
{
        char *line;
        int exp_result;
        char *expansion;
        char buf[1024];
        int n;

        while ( 1 ) {
                line = (char *)readline(prompt);
                if ( line && *line ) {
                        using_history();
                        exp_result = history_expand(line,&expansion);
                        if ( !exp_result ) {
                                free(expansion);
#if 0
                                for ( ; isspace((unsigned char)*line); line++ );
#endif
                                add_history(line);
                                break;
                        } else if ( exp_result == 2 ) {
                                // !pattern:p
                                free(line);
                                line = expansion;
                                // rl_insert_text(line) should work, but it does not work.
                                for ( n=0; line[n]!=0; n++ ) rl_insert(1,line[n]);
                                prompt = 0;
                        } else if ( exp_result == 1 ) {
                                // !pattern
                                free(line);
                                line = expansion;
                                puts(line);
                                break;
                        }
                }
        }
        return line;
}
#endif
