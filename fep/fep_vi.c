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
	/* 1       */	{"1",		vi_num},
	/* 2       */	{"2",		vi_num},
	/* 3       */	{"3",		vi_num},
	/* 4       */	{"4",		vi_num},
	/* 5       */	{"5",		vi_num},
	/* 6       */	{"6",		vi_num},
	/* 7       */	{"7",		vi_num},
	/* 8       */	{"8",		vi_num},
	/* 9       */	{"9",		vi_num},

	/* ~       */	{"~",		vi_edit},
	/* A       */	{"A",		vi_edit},
	/* C       */	{"C",		vi_edit},
	/* D       */	{"D",		vi_edit},
	/* P       */	{"P",		vi_edit},
	/* S       */	{"S",		vi_edit},
	/* I       */	{"I",		vi_edit},
	/* a       */	{"a",		vi_edit},
	/* i       */	{"i",		vi_edit},
	/* p       */	{"p",		vi_edit},
	/* r       */	{"r",		vi_edit},
	/* s       */	{"s",		vi_edit},
	/* x       */	{"x",		vi_edit},
	/* X       */	{"X",		vi_edit},
	/* ^U      */	{"\\^U",	vi_edit},

	/*         */	{" ",		vi_motion},
	/* ^H      */	{"\b",		vi_motion},
	/* l       */	{"l",		vi_motion},
	/* h       */	{"h",		vi_motion},
	/* k       */	{"k",		vi_motion},
	/* j       */	{"j",		vi_motion},
	/* b       */	{"b",		vi_motion},
	/* B       */	{"B",		vi_motion},
	/* e       */	{"e",		vi_motion},
	/* E       */	{"E",		vi_motion},
	/* w       */	{"w",		vi_motion},
	/* W       */	{"W",		vi_motion},
	/* n       */	{"n",		vi_motion},
	/* N       */	{"N",		vi_motion},
	/* 0       */	{"0",		vi_motion},
	/* ^       */	{"^",		vi_motion},
	/* $       */	{"$",		vi_motion},
	/* |       */	{"|",		vi_motion},
	/* -       */	{"-",		vi_motion},
	/* +       */	{"+",		vi_motion},

	/* cb      */	{"cb",		vi_c},
	/* cB      */	{"cb",		vi_c},
	/* cw      */	{"cw",		vi_c},
	/* cW      */	{"cW",		vi_c},
	/* c0      */	{"c0",		vi_c},
	/* c^      */	{"c^",		vi_c},
	/* c$      */	{"c$",		vi_c},
	/* cc      */	{"cc",		vi_c},

	/* db      */	{"db",		vi_d},
	/* dB      */	{"dB",		vi_d},
	/* dw      */	{"dw",		vi_d},
	/* dW      */	{"dW",		vi_d},
	/* d0      */	{"d0",		vi_d},
	/* d^      */	{"d^",		vi_d},
	/* d$      */	{"d$",		vi_d},
	/* dd      */	{"dd",		vi_d},

	/* ^E      */	{"\\^E",	expand_file_name},
	/* ^J      */	{"\\^J",	vi_new_line},
	/* ^L      */	{"\\^L",	list_file_name},
	/* ^M      */	{"\\^M",	vi_new_line},
	/* ^P      */	{"\\^P",	previous_history},
	/* ^N      */	{"\\^N",	next_history},
	/* ^R      */	{"\\^R",	reprint},
	/* ^^      */	{"\\^^",	toggle_through},
	/* ^X-^B   */	{"\\^X\\^B",	show_bindings},
	/* ^X-B	   */	{"\\^XB",	show_bindings},
	/* ^X-b	   */	{"\\^Xb",	show_bindings},
	/* ^X-^H   */	{"\\^X\\^H",	show_history},
	/* ^X-h    */	{"\\^Xh",	show_history},
	/* ^X-H    */	{"\\^XH",	show_history},
	/* ^X-l    */	{"\\^Xl",	list_file_name},
	/* ^X-L    */	{"\\^XL",	list_file_name},
	/* ^X-^L   */	{"\\^X\\^L",	fep_repaint},
	/* ^X-^V   */	{"\\^X\\^V",	view_buffer},
	/* ^X-^X   */	{"\\^X\\^X",	expand_file_name},
	/* ^X-?    */	{"\\^X?",	show_bindings},
	/* ^X-^C   */	{"\\^X\\^C",	(FUNC)terminate},
	/* ^X-^D   */	{"\\^X\\^D",	send_eof},
	/* ^X-(	   */	{"\\^X(",	fep_start_script},
	/* ^X-)	   */	{"\\^X)",	fep_end_script},
	/*         */	{NULL,		NULL}
};

BINDENT viInsertBindings[] = {
	/* ^H      */	{"\\^H",	vi_ins_edit},
	/* ^W      */	{"\\^W",	vi_ins_edit},
	/* ^U      */	{"\\^U",	vi_ins_edit},
	/* ^V      */	{"\\^V",	vi_ins_edit},
	/* ^J      */	{"\\^J",	vi_new_line},
	/* ^L      */	{"\\^L",	list_file_name},
	/* ^M      */	{"\\^M",	vi_new_line},
	/* ^P      */	{"\\^P",	previous_history},
	/* ^N      */	{"\\^N",	next_history},
	/* ESC     */	{"\\^[",	vi_ins_edit},
	/* ^E      */	{"\\^E",	expand_file_name},
	/* ^^      */	{"\\^^",	toggle_through},
	/* ^X-^B   */	{"\\^X\\^B",	show_bindings},
	/* ^X-B	   */	{"\\^XB",	show_bindings},
	/* ^X-b	   */	{"\\^Xb",	show_bindings},
	/* ^X-^H   */	{"\\^X\\^H",	show_history},
	/* ^X-h    */	{"\\^Xh",	show_history},
	/* ^X-H    */	{"\\^XH",	show_history},
	/* ^X-l    */	{"\\^Xl",	list_file_name},
	/* ^X-L    */	{"\\^XL",	list_file_name},
	/* ^X-^L   */	{"\\^X\\^L",	fep_repaint},
	/* ^X-^X   */	{"\\^X\\^X",	expand_file_name},
	/* ^X-?    */	{"\\^X?",	show_bindings},
	/* ^X-^C   */	{"\\^X\\^C",	(FUNC)terminate},
	/* ^X-^D   */	{"\\^X\\^D",	send_eof},
	/* ^X-^V   */	{"\\^X\\^V",	view_buffer},
	/* ^X-(	   */	{"\\^X(",	fep_start_script},
	/* ^X-)	   */	{"\\^X)",	fep_end_script},
	/*         */	{NULL,		NULL}
};

FUNC *viComTable;
FUNC *viInsTable;

initViBindings (cft, aft)
FUNC cft[], aft[];
{
    register int i;
    BINDENT *ftp;

    for (i = 0; i < 256; i++)
	aft[i] = fep_abort;
    for (i = 0; i < 256; i++)
	cft[i] = self_insert;

#define import(table,key,fn) if((int)key>0)table[(int)key]=fn
#ifdef TERMIOS
    /* Now, using cbreak mode
    import (cft, initial_ttymode.c_cc[VSTART], ignore);
    import (cft, initial_ttymode.c_cc[VSTOP], ignore);
    */
#if 0
    import (cft, initial_ttymode.c_cc[VINTR], insert_and_flush);
    import (aft, initial_ttymode.c_cc[VINTR], insert_and_flush);
#else
	/* added by noro */
    import (cft, initial_ttymode.c_cc[VINTR], send_int_sig);
	/* XXX : this may not be necessary */
    import (aft, initial_ttymode.c_cc[VINTR], send_int_sig);
#endif
    import (cft, initial_ttymode.c_cc[VQUIT], insert_and_flush);
    import (cft, initial_ttymode.c_cc[VEOF], send_eof);
#ifdef VSWTC
    import (cft, initial_ttymode.c_cc[VSWTC], insert_and_flush);
#endif
    import (cft, initial_ttymode.c_cc[VSUSP], insert_and_flush);
#ifdef VDSUSP
    import (cft, initial_ttymode.c_cc[VDSUSP], self_insert);
#endif
    import (cft, initial_ttymode.c_cc[VREPRINT], reprint);
#ifdef VDISCARD
    import (cft, initial_ttymode.c_cc[VDISCARD], self_insert);
#endif
    import (cft, initial_ttymode.c_cc[VWERASE], delete_previous_word);
    import (cft, initial_ttymode.c_cc[VLNEXT], literal_next);
    import (cft, initial_ttymode.c_cc[VERASE], delete_previous_character);
    import (cft, initial_ttymode.c_cc[VKILL], delete_line);
#else
    /* Now, using cbreak mode
    import (cft, tchars_buf.t_startx, ignore);
    import (cft, tchars_buf.t_stopc, ignore);
    */
#if 0
    import (cft, tchars_buf.t_intrc, insert_and_flush);
    import (aft, tchars_buf.t_intrc, insert_and_flush);
#else
    import (cft, tchars_buf.t_intrc, send_int_sig);
    import (aft, tchars_buf.t_intrc, send_int_sig);
#endif
    import (cft, tchars_buf.t_quitc, insert_and_flush);
    import (cft, tchars_buf.t_eofc, send_eof);
    import (cft, tchars_buf.t_brkc, insert_and_flush);
    import (cft, ltchars_buf.t_suspc, insert_and_flush);
    import (cft, ltchars_buf.t_dsuspc, self_insert);
    import (cft, ltchars_buf.t_rprntc, reprint);
    import (cft, ltchars_buf.t_flushc, self_insert);
    import (cft, ltchars_buf.t_werasc, delete_previous_word);
    import (cft, ltchars_buf.t_lnextc, literal_next);
    import (cft, initial_ttymode.sg_erase, delete_previous_character);
    import (cft, initial_ttymode.sg_kill, delete_line);
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

vi_edit (c)
    int c;
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

altenateEditmode ()
{
    FUNC *tmp;

    tmp = curFuncTab;
    curFuncTab = altFuncTab;
    altFuncTab = tmp;
    vi_mode = (vi_mode == INSERTMODE) ? COMMANDMODE : INSERTMODE;

    return (0);
}

vi_num (c)
    int c;
{
    vi_count = vi_count * 10 + ((int)c - (int)'0');
    return (0);
}

vi_motion (c)
    int c;
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

vi_c (c)
    int c;
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

vi_d (c)
    int c;
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

vi_new_line (c)
    int c;
{
    int count = vi_count ? vi_count : 1;

    vi_count = 0;

    (void) new_line (c);
    if (vi_mode == COMMANDMODE)
	altenateEditmode ();

    return (1);
}

vi_ins_edit (c)
    int c;
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
