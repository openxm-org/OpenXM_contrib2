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
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/help.c,v 1.12 2018/03/29 01:32:50 noro Exp $ 
*/
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
  static int  ja_JP_UTF_8 = 0;

  if ( !helpdir[0] ) {
    e = (char *)getenv("LANG");
    if ( !e )
      strcpy(helpdir,"help");
    else if ( !strncmp(e,"japan",strlen("japan"))
          || !strncmp(e,"ja_JP",strlen("ja_JP")) ) {
      strcpy(helpdir,"help-ja");
      if (strcmp(e,"ja_JP.UTF-8")==0) ja_JP_UTF_8 = 1;
    }else
      strcpy(helpdir,"help-en");
  }

  if ( !s )
    ghelp();
  else {
#if !defined(VISUAL) && !defined(__MINGW32__)
    int i;
    FUNC f;
    char name[BUFSIZ],com[BUFSIZ];
    FILE *fp;

    sprintf(name,"%s/%s/%s",asir_libdir,helpdir,s);
    if ( fp = fopen(name,"r") ) {
      fclose(fp);
      if (!ja_JP_UTF_8)
        sprintf(com,"%s %s",asir_pager,name);
      else
        sprintf(com,"nkf -w %s | %s ",name,asir_pager);
      system(com);
    } else {
      gen_searchf_searchonly(s,&f,0);
      if ( f && f->id == A_USR && f->f.usrf->desc )
        fprintf(stderr,"%s\n",f->f.usrf->desc);
    }
#else
    FUNC f;
    gen_searchf_searchonly(s,&f,0);
    if ( f && f->id == A_USR && f->f.usrf->desc )
      fprintf(stderr,"%s\n",f->f.usrf->desc);

#endif
  }
}

void ghelp() {
  int i;

  for ( i = 0; ghelpstr[i]; i++ )
    fprintf(stderr,"%s\n",ghelpstr[i]);
}
