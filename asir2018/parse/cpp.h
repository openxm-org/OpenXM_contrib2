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
 * $OpenXM$
*/
#ifndef _ACCUM_
#define _ACCUM_

#include <string.h>

typedef struct {
    int have;
    int used;
    char *buf; } ACCUM;

char *init_accum(void);
char *accum_result(char *A);
char accum_regret(char *A);
char *accum_buf(char *A);

#endif
#ifndef _EXPR_
#define _EXPR_

typedef struct _node {
    int leaf;
    char *name;
    struct _node *left;
    struct _node *right;
    int op; } NODE;

NODE *read_expr();
NODE *read_expr_p(void);

extern int expr_sharp;

#endif
#ifndef _IF_
#define _IF_

typedef struct _if {
    struct _if *next;
    int condstate; } IF;
#define IFSTATE_TRUE 0
#define IFSTATE_FALSE 1
#define IFSTATE_STAYFALSE 2

extern IF *ifstack;
extern int n_skipped_ifs;

#endif
#ifndef _IO_
#define _IO_

#include <stdio.h>

#if 0
#define MAX_PUSHBACK 8192
#define MAXFILES 20
#endif
#define MAX_PUSHBACK 512
#define MAXFILES 4

extern int linefirst;
extern int willbefirst;

#define outfile cpp_outfile
extern FILE *outfile;

/*
extern int fstackdepth;
extern char pushback[_NFILE][MAX_PUSHBACK];
extern int npushed[_NFILE];
extern FILE *fstack[_NFILE];
extern char *fn[_NFILE];
extern char *incldir[_NFILE];
extern int lineno[_NFILE];
*/

char Get(void);
char getnonspace(void);
char getnonhspace(void);
char getnhsexpand(void);
char getexpand(void);
char *curfile(void);
char **Curfile(void);
int curline(void);
int *Curline(void);
char *curdir(void);
char **Curdir(void);
char *read_ctrl(void);
char *read_ident(void);

#endif
#ifndef _IS_
#define _IS_

#include <ctype.h>

int ishspace(char c);
int isbsymchar(char c);
int issymchar(char c);

#endif
#ifndef _MALLOC_
#define _MALLOC_

char *malloc(size_t);
char *realloc(void *, size_t);
char *copyofstr(char *str);
char *copyofblk(char *blk, int len);
#define NEW(type) ((type *) malloc((int)sizeof(type)))
#define OLD(x) free((char *) x)
#define check_malloc(ptr) Check_malloc((char *)(ptr))

#endif
#ifndef _STATS_H_9617d11a_
#define _STATS_H_9617d11a_

extern int n_defines;
extern int n_undefines;
extern int n_hits;
extern int n_misses;

#endif
#ifndef _SYMTBL_
#define _SYMTBL_

typedef struct _def {
    struct _def *link;
    char *name;
    int nargs;
    unsigned char *repl;
    int how;
#define DEF_PREDEF  0
#define DEF_CMDLINE 1
#define DEF_DEFINE  2
    } DEF;

extern DEF **symtbl;
extern int symtbl_size;
extern int n_in_table;

DEF *find_def(char *name);

#endif
#ifndef unctrl
#define unctrl(c) _unctrl[0xff&(int)(c)]
#define Unctrl(c) _Unctrl[0xff&(int)(c)]
extern char *_unctrl[];
extern char *_Unctrl[];
#endif

void dump_single(DEF *);
void dump_expr(NODE *);
void do_at(void);
void do_debug(void);
void read_formals(void);
void do_dump(void);
void err_head(void);
void do_eval(void);
int get_quote_char(void);
NODE *read_expr_11(void);
NODE *read_expr_10(void);
NODE *read_expr_9(void);
NODE *read_expr_8(void);
NODE *read_expr_7(void);
NODE *read_expr_6(void);
NODE *read_expr_5(void);
NODE *read_expr_4(void);
NODE *read_expr_3(void);
NODE *read_expr_2(void);
NODE *read_expr_1(void);
NODE *read_expr_0(void);
NODE *read_expr_(void);
NODE *read_expr_p(void);
void iftrue(void);
void iffalse(void);
void init_include(void);
void flush_final_nl(void);
void input_mark(void);
void input_unmark(void);
void input_recover(void);
void mark_file_beginning(void);
void mark_file_ending(void);
void mark_charpushed(void);
void mark_get_line(void);
void do_line(void);
void do_pragma(void);
void do_set(void);
void do_sharp(void);
void flush_sharp_line(void);
void init_stats(void);
void save_stats(void);
void init_symtbl(void);
void undef_predefs(void);
void do_while(void);
void do_endwhile(void);
void Check_malloc(char * ptr);

void output_ifstate(int state);
void out_at(int line, char *file);
void outputc(char c);
void outputs(char *s);
void outputd(int n);

void do_at(void);
void do_debug(void);
void do_define(int sharp, int redef);
void do_dump(void);
void do_eval(void);
void do_if(int expr_sharp);
void do_ifdef(int expr_sharp);
void do_ifndef(int expr_sharp);
void do_else(int expr_sharp);
void do_elif(int expr_sharp);
void do_endif(int expr_sharp);
void do_include(int expr_sharp);
void do_line(void);
void do_pragma(void);
void do_set(void);
void do_sharp(void);
void do_undef(int expr_sharp);
void do_while(void);
void do_endwhile(void);
int free(char *);
void Push(char);
void putx(int);
void Bcopy(char *from, char *to, int len);
void maybe_print(char);
void read_include_file(char *,int,int);
void mark_got_from_pushback(char);
void mark_got_from_file(char);
void mark_got(char);
void define(char *,int,unsigned char *,int);
int undef(char *);
void push_new_file(char *,FILE *);
void defd(char *,int);
void free_expr(NODE *n);
int in_false_if(void);
int eval_expr(int,int);
void abort();
