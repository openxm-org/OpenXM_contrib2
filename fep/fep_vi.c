/*	Copyright (c) 1987, 1988 by Software Research Associates, Inc.	*/

#ifndef lint
static char rcsid[]=
"$Header$ (SRA)";
#endif /* lint */

#include <stdio.h>
#include <sys/ioctl.h>
#ifdef TERMIOS
#include <termios.h>
#if defined(__linux__) || defined(__CYGWIN__)
#ifndef _POSIX_VDISABLE
#define _POSIX_VDISABLE '\0'
#endif
#endif
#else
#include <sgtty.h>
#endif
#include <ctype.h>
#include "fep_defs.h"
#include "fep_glob.h"
#include "fep_funcs.h"

enum {INSERTMODE, COMMANDMODE} vi_mode = COMMANDMODE;
int vi_count;

extern char *CommandLine;
extern int CurrentPosition;

/*
 * Default binding table
 */
BINDENT viComBindings[] = {
	/* 1       */	{"1",		(FUNC)vi_num},
	/* 2       */	{"2",		(FUNC)vi_num},
	/* 3       */	{"3",		(FUNC)vi_num},
	/* 4       */	{"4",		(FUNC)vi_num},
	/* 5       */	{"5",		(FUNC)vi_num},
	/* 6       */	{"6",		(FUNC)vi_num},
	/* 7       */	{"7",		(FUNC)vi_num},
	/* 8       */	{"8",		(FUNC)vi_num},
	/* 9       */	{"9",		(FUNC)vi_num},

	/* ~       */	{"~",		(FUNC)vi_edit},
	/* A       */	{"A",		(FUNC)vi_edit},
	/* C       */	{"C",		(FUNC)vi_edit},
	/* D       */	{"D",		(FUNC)vi_edit},
	/* P       */	{"P",		(FUNC)vi_edit},
	/* S       */	{"S",		(FUNC)vi_edit},
	/* I       */	{"I",		(FUNC)vi_edit},
	/* a       */	{"a",		(FUNC)vi_edit},
	/* i       */	{"i",		(FUNC)vi_edit},
	/* p       */	{"p",		(FUNC)vi_edit},
	/* r       */	{"r",		(FUNC)vi_edit},
	/* s       */	{"s",		(FUNC)vi_edit},
	/* x       */	{"x",		(FUNC)vi_edit},
	/* X       */	{"X",		(FUNC)vi_edit},
	/* ^U      */	{"\\^U",	(FUNC)vi_edit},

	/*         */	{" ",		(FUNC)vi_motion},
	/* ^H      */	{"\b",		(FUNC)vi_motion},
	/* l       */	{"l",		(FUNC)vi_motion},
	/* h       */	{"h",		(FUNC)vi_motion},
	/* k       */	{"k",		(FUNC)vi_motion},
	/* j       */	{"j",		(FUNC)vi_motion},
	/* b       */	{"b",		(FUNC)vi_motion},
	/* B       */	{"B",		(FUNC)vi_motion},
	/* e       */	{"e",		(FUNC)vi_motion},
	/* E       */	{"E",		(FUNC)vi_motion},
	/* w       */	{"w",		(FUNC)vi_motion},
	/* W       */	{"W",		(FUNC)vi_motion},
	/* n       */	{"n",		(FUNC)vi_motion},
	/* N       */	{"N",		(FUNC)vi_motion},
	/* 0       */	{"0",		(FUNC)vi_motion},
	/* ^       */	{"^",		(FUNC)vi_motion},
	/* $       */	{"$",		(FUNC)vi_motion},
	/* |       */	{"|",		(FUNC)vi_motion},
	/* -       */	{"-",		(FUNC)vi_motion},
	/* +       */	{"+",		(FUNC)vi_motion},

	/* cb      */	{"cb",		(FUNC)vi_c},
	/* cB      */	{"cb",		(FUNC)vi_c},
	/* cw      */	{"cw",		(FUNC)vi_c},
	/* cW      */	{"cW",		(FUNC)vi_c},
	/* c0      */	{"c0",		(FUNC)vi_c},
	/* c^      */	{"c^",		(FUNC)vi_c},
	/* c$      */	{"c$",		(FUNC)vi_c},
	/* cc      */	{"cc",		(FUNC)vi_c},

	/* db      */	{"db",		(FUNC)vi_d},
	/* dB      */	{"dB",		(FUNC)vi_d},
	/* dw      */	{"dw",		(FUNC)vi_d},
	/* dW      */	{"dW",		(FUNC)vi_d},
	/* d0      */	{"d0",		(FUNC)vi_d},
	/* d^      */	{"d^",		(FUNC)vi_d},
	/* d$      */	{"d$",		(FUNC)vi_d},
	/* dd      */	{"dd",		(FUNC)vi_d},

	/* ^E      */	{"\\^E",	(FUNC)expand_file_name},
	/* ^J      */	{"\\^J",	(FUNC)vi_new_line},
	/* ^L      */	{"\\^L",	(FUNC)list_file_name},
	/* ^M      */	{"\\^M",	(FUNC)vi_new_line},
	/* ^P      */	{"\\^P",	(FUNC)previous_history},
	/* ^N      */	{"\\^N",	(FUNC)next_history},
	/* ^R      */	{"\\^R",	(FUNC)reprint},
	/* ^^      */	{"\\^^",	(FUNC)toggle_through},
	/* ^X-^B   */	{"\\^X\\^B",	(FUNC)show_bindings},
	/* ^X-B	   */	{"\\^XB",	(FUNC)show_bindings},
	/* ^X-b	   */	{"\\^Xb",	(FUNC)show_bindings},
	/* ^X-^H   */	{"\\^X\\^H",	(FUNC)show_history},
	/* ^X-h    */	{"\\^Xh",	(FUNC)show_history},
	/* ^X-H    */	{"\\^XH",	(FUNC)show_history},
	/* ^X-l    */	{"\\^Xl",	(FUNC)list_file_name},
	/* ^X-L    */	{"\\^XL",	(FUNC)list_file_name},
	/* ^X-^L   */	{"\\^X\\^L",	(FUNC)fep_repaint},
	/* ^X-^V   */	{"\\^X\\^V",	(FUNC)view_buffer},
	/* ^X-^X   */	{"\\^X\\^X",	(FUNC)expand_file_name},
	/* ^X-?    */	{"\\^X?",	(FUNC)show_bindings},
	/* ^X-^C   */	{"\\^X\\^C",	(FUNC)terminate},
	/* ^X-^D   */	{"\\^X\\^D",	(FUNC)send_eof},
	/* ^X-(	   */	{"\\^X(",	(FUNC)fep_start_script},
	/* ^X-)	   */	{"\\^X)",	(FUNC)fep_end_script},
	/*         */	{NULL,		NULL}
};

BINDENT viInsertBindings[] = {
	/* ^H      */	{"\\^H",	(FUNC)vi_ins_edit},
	/* ^W      */	{"\\^W",	(FUNC)vi_ins_edit},
	/* ^U      */	{"\\^U",	(FUNC)vi_ins_edit},
	/* ^V      */	{"\\^V",	(FUNC)vi_ins_edit},
	/* ^J      */	{"\\^J",	(FUNC)vi_new_line},
	/* ^L      */	{"\\^L",	(FUNC)list_file_name},
	/* ^M      */	{"\\^M",	(FUNC)vi_new_line},
	/* ^P      */	{"\\^P",	(FUNC)previous_history},
	/* ^N      */	{"\\^N",	(FUNC)next_history},
	/* ESC     */	{"\\^[",	(FUNC)vi_ins_edit},
	/* ^E      */	{"\\^E",	(FUNC)expand_file_name},
	/* ^^      */	{"\\^^",	(FUNC)toggle_through},
	/* ^X-^B   */	{"\\^X\\^B",	(FUNC)show_bindings},
	/* ^X-B	   */	{"\\^XB",	(FUNC)show_bindings},
	/* ^X-b	   */	{"\\^Xb",	(FUNC)show_bindings},
	/* ^X-^H   */	{"\\^X\\^H",	(FUNC)show_history},
	/* ^X-h    */	{"\\^Xh",	(FUNC)show_history},
	/* ^X-H    */	{"\\^XH",	(FUNC)show_history},
	/* ^X-l    */	{"\\^Xl",	(FUNC)list_file_name},
	/* ^X-L    */	{"\\^XL",	(FUNC)list_file_name},
	/* ^X-^L   */	{"\\^X\\^L",	(FUNC)fep_repaint},
	/* ^X-^X   */	{"\\^X\\^X",	(FUNC)expand_file_name},
	/* ^X-?    */	{"\\^X?",	(FUNC)show_bindings},
	/* ^X-^C   */	{"\\^X\\^C",	(FUNC)terminate},
	/* ^X-^D   */	{"\\^X\\^D",	(FUNC)send_eof},
	/* ^X-^V   */	{"\\^X\\^V",	(FUNC)view_buffer},
	/* ^X-(	   */	{"\\^X(",	(FUNC)fep_start_script},
	/* ^X-)	   */	{"\\^X)",	(FUNC)fep_end_script},
	/*         */	{NULL,		NULL}
};

FUNC *viComTable;
FUNC *viInsTable;

void initViBindings (FUNC cft[], FUNC aft[])
{
    register int i;
    BINDENT *ftp;

    for (i = 0; i < 256; i++)
	aft[i] = (FUNC)fep_abort;
    for (i = 0; i < 256; i++)
	cft[i] = (FUNC)self_insert;

#define import(table,key,fn) if((int)key>0)table[(int)key]=fn
#ifdef TERMIOS
    /* Now, using cbreak mode
    import (cft, initial_ttymode.c_cc[VSTART], (FUNC)ignore);
    import (cft, initial_ttymode.c_cc[VSTOP], (FUNC)ignore);
    */
#if 0
    import (cft, initial_ttymode.c_cc[VINTR], (FUNC)insert_and_flush);
    import (aft, initial_ttymode.c_cc[VINTR], (FUNC)insert_and_flush);
#else
	/* added by noro */
    import (cft, initial_ttymode.c_cc[VINTR], (FUNC)send_int_sig);
	/* XXX : this may not be necessary */
    import (aft, initial_ttymode.c_cc[VINTR], (FUNC)send_int_sig);
#endif
    import (cft, initial_ttymode.c_cc[VQUIT], (FUNC)insert_and_flush);
    import (cft, initial_ttymode.c_cc[VEOF], (FUNC)send_eof);
#ifdef VSWTC
    import (cft, initial_ttymode.c_cc[VSWTC], (FUNC)insert_and_flush);
#endif
    import (cft, initial_ttymode.c_cc[VSUSP], (FUNC)insert_and_flush);
#ifdef VDSUSP
    import (cft, initial_ttymode.c_cc[VDSUSP], (FUNC)self_insert);
#endif
    import (cft, initial_ttymode.c_cc[VREPRINT], (FUNC)reprint);
#ifdef VDISCARD
    import (cft, initial_ttymode.c_cc[VDISCARD], (FUNC)self_insert);
#endif
    import (cft, initial_ttymode.c_cc[VWERASE], (FUNC)delete_previous_word);
    import (cft, initial_ttymode.c_cc[VLNEXT], (FUNC)literal_next);
    import (cft, initial_ttymode.c_cc[VERASE], (FUNC)delete_previous_character);
    import (cft, initial_ttymode.c_cc[VKILL], (FUNC)delete_line);
#else
    /* Now, using cbreak mode
    import (cft, tchars_buf.t_startx, (FUNC)ignore);
    import (cft, tchars_buf.t_stopc, (FUNC)ignore);
    */
#if 0
    import (cft, tchars_buf.t_intrc, (FUNC)insert_and_flush);
    import (aft, tchars_buf.t_intrc, (FUNC)insert_and_flush);
#else
    import (cft, tchars_buf.t_intrc, (FUNC)send_int_sig);
    import (aft, tchars_buf.t_intrc, (FUNC)send_int_sig);
#endif
    import (cft, tchars_buf.t_quitc, (FUNC)insert_and_flush);
    import (cft, tchars_buf.t_eofc, (FUNC)send_eof);
    import (cft, tchars_buf.t_brkc, (FUNC)insert_and_flush);
    import (cft, ltchars_buf.t_suspc, (FUNC)insert_and_flush);
    import (cft, ltchars_buf.t_dsuspc, (FUNC)self_insert);
    import (cft, ltchars_buf.t_rprntc, (FUNC)reprint);
    import (cft, ltchars_buf.t_flushc, (FUNC)self_insert);
    import (cft, ltchars_buf.t_werasc, (FUNC)delete_previous_word);
    import (cft, ltchars_buf.t_lnextc, (FUNC)literal_next);
    import (cft, initial_ttymode.sg_erase, (FUNC)delete_previous_character);
    import (cft, initial_ttymode.sg_kill, (FUNC)delete_line);
#endif

#undef import

    for (ftp = viComBindings; ftp->bt_s; ftp++)
	bind_key (aft, ftp->bt_func, ftp->bt_s, fep_abort);
    for (ftp = viInsertBindings; ftp->bt_s; ftp++)
	bind_key (cft, ftp->bt_func, ftp->bt_s, fep_abort);

    viInsTable = cft;
    viComTable = aft;
    vi_mode = INSERTMODE;
}

int vi_edit (int c)
{
    int count = vi_count ? vi_count : 1;

    switch (c) {
	case '~':
	    {
		char c = CommandLine [CurrentPosition];

		if (isalpha (c)) {
		    (void) delete_next_character (c);
		    c ^= 0040;
		    (void) self_insert (c);
		}
		else
		    (void) forward_character (c);
	    }
	    break;

	case 'a':
	    if (!is_eol())
		(void) forward_character (c);
	case 'i':
	    (void) altenateEditmode (c);
	    break;

	case 'A':
	    (void) end_of_line (c);
	    (void) altenateEditmode ();
	    break;

	case 'I':
	    (void) beginning_of_line (c);
	    (void) altenateEditmode ();
	    break;

	case 'C':
	    (void) kill_to_end_of_line (c);
	    (void) altenateEditmode (c);
	    break;

	case 'D':
	    (void) kill_to_end_of_line (c);
	    break;

	case 'S':
	    (void) delete_line (c);
	    (void) altenateEditmode ();
	    break;

	case 'r':
	    (void) delete_next_character (c);
	    (void) self_insert (getcharacter ());
	    break;

	case 's':
	    (void) delete_next_n_character (count);
	    (void) altenateEditmode ();
	    break;

	case 'x':
	    (void) delete_next_n_character (count);
	    break;

	case 'X':
	    (void) delete_previous_n_character (count);
	    break;

	case 'p':
	    (void) forward_character ();
	case 'P':
	    (void) yank_from_kill_buffer ();
	    (void) backward_character ();
	    break;

	case ctrl('U'):
	    (void) delete_line (c);
	    (void) altenateEditmode ();
	    break;
    }
    vi_count = 0;
    return (0);
}

int altenateEditmode ()
{
    FUNC *tmp;

    tmp = curFuncTab;
    curFuncTab = altFuncTab;
    altFuncTab = tmp;
    vi_mode = (vi_mode == INSERTMODE) ? COMMANDMODE : INSERTMODE;

    return (0);
}

int vi_num (int c)
{
    vi_count = vi_count * 10 + ((int)c - (int)'0');
    return (0);
}

int vi_motion (int c)
{
    int count = vi_count ? vi_count : 1;

    switch (c) {

	case 'w':
	    (void) forward_n_word (count);
	    break;

	case 'W':
	    (void) forward_n_Word (count);
	    break;

	case 'e':
	    (void) forward_to_end_of_n_word (count);
	    break;

	case 'E':
	    (void) forward_to_end_of_n_Word (count);
	    break;

	case 'b':
	    (void) backward_n_word (count);
	    break;

	case 'B':
	    (void) backward_n_Word (count);
	    break;

	case 'l':
	case ' ':
	    (void) forward_n_character (count);
	    break;

	case 'h':
	case '\b':
	    (void) backward_n_character (count);
	    break;

	case 'k':
	case '-':
	    (void) previous_history (c);
	    break;

	case 'j':
	case '+':
	    (void) next_history (c);
	    break;

	case 'n':
	    (void) search_reverse (c);
	    break;

	case 'N':
	    (void) search_forward (c);
	    break;

	case '0':
	    if (vi_count)
		return (vi_num (c));
	    /* falling down */

	case '^':
	    (void) beginning_of_line (c);
	    break;

	case '$':
	    (void) end_of_line (c);
	    break;

	case '|':
	    (void) moveto (count - 1);
	    /***
	    if (strlen (CommandLine) >= count) {
		(void) beginning_of_line (c);
		(void) forward_n_character (count - 1);
	    }
	    else
		(void) end_of_line (c);
	    ***/
	    break;

	default:
	    (void) fep_abort (c);
	    break; 
    }
    vi_count = 0;
    return (0);
}

int vi_c (int c)
{
    int count = vi_count ? vi_count : 1;

    switch (c) {
	case '0':
	case '^':
	    (void) kill_to_top_of_line (c);
	    break;

	case 'c':
	    delete_line(c);
	    break;

	case 'w':
	    (void) delete_next_n_word (count);
	    break;

	case 'W':
	    (void) delete_next_n_Word (count);
	    break;

	case 'b':
	    (void) delete_previous_n_word (count);
	    break;

	case 'B':
	    (void) delete_previous_n_Word (count);
	    break;

	case '$':
	    (void) kill_to_end_of_line(c);
	    break;

	default:
	    return (fep_abort (c));
    }
    vi_count = 0;
    altenateEditmode ();
    return (0);
}

int vi_d (int c)
{
    int count = vi_count ? vi_count : 1;

    switch (c) {
	case '0':
	case '^':
	    (void) kill_to_top_of_line (c);
	    break;

	case 'd':
	    delete_line(c);
	    break;

	case 'w':
	    (void) delete_next_n_word (count);
	    break;

	case 'W':
	    (void) delete_next_n_Word (count);
	    break;

	case 'b':
	    (void) delete_previous_n_word (count);
	    break;

	case 'B':
	    (void) delete_previous_n_Word (count);
	    break;

	case '$':
	    (void) kill_to_end_of_line(c);
	    break;

	default:
	    return (fep_abort (c));
    }
    vi_count = 0;
    return (0);
}

int vi_new_line (int c)
{
    int count = vi_count ? vi_count : 1;

    vi_count = 0;

    (void) new_line (c);
    if (vi_mode == COMMANDMODE)
	altenateEditmode ();

    return (1);
}

int vi_ins_edit (int c)
{
    switch (c) {

	case ctrl('H'):
	    (void) delete_previous_character (c);
	    break;

	case ctrl('W'):
	    (void) delete_previous_word (c);
	    break;

	case ctrl('U'):
	    (void) delete_line (c);
	    break;

	case ctrl('V'):
	    (void) literal_next (c);
	    break;

	case '\033':
	    (void) altenateEditmode ();
	    (void) backward_character ();
	    break;
    }
    return (0);
}
