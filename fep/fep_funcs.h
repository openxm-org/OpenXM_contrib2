/*	Copyright (c) 1987, 1988 by Software Research Associates, Inc.	*/

#ifndef lint
# define FEP_FUNCS \
    "$Header$ (SRA)"
#endif /* lint */

/*
 * command line edit functions
 */
int	fep_abort();
int	backward_character();
int	backward_word();
int	backward_Word();
int	beginning_of_line();
int	clear_screen();
int	delete_line();
int	delete_next_character();
int	delete_next_word();
int	delete_next_Word();
int	delete_previous_character();
int	delete_previous_word();
int	delete_previous_Word();
int	delete_to_kill_buffer();
#if defined(__CYGWIN__) || defined(sun)
void	terminate(int);
#else
void	terminate __P((int));
#endif
int send_int_sig();
int	end_of_line();
int	expand_file_name();
int	forward_character();
int	forward_to_end_of_word();
int	forward_to_end_of_Word();
int	forward_word();
int	forward_Word();
int	ignore();
int	insert_and_flush();
int	insert_tab();
int	kill_to_end_of_line();
int	kill_to_top_of_line();
int	list_file_name();
int	literal_next();
int	mark();
int	new_line();
int	next_history();
int	previous_history();
int	insert_next_history();
int	insert_previous_history();
int	insert_current_history();
int	reprint();
int	search_reverse();
int	search_forward();
int	self_insert();
int	send_eof();
int	show_bindings();
int	show_history();
int	toggle_through();
int	fix_transparency();
int	yank_from_kill_buffer();
int	invoke_shell();
int	show_help();

/*
 * Vi mode functions.
 */
int	vi_num();
int	vi_edit();
int	vi_motion();
int	vi_c();
int	vi_d();
int	vi_ins_edit();
int	vi_new_line();

/*
 * fep builtin command functions.
 */
int	suspend();
int	bind_to_key();
int	alias();
int	unalias();
int	set();
int	unset();
int	fep_chdir();
int	fep_pwd();
int	fep_history();
int	show_bindings();
int	fep_source();
int	fep_command();
int	fep_save_history();
int	fep_start_script();
int	fep_end_script();
int	fep_read_history();
int	fep_read_from_file();
int	fep_read_from_command();
int	fep_echo();
int	fep_if();
int	fep_else();
int	fep_endif();
int	fep_repaint();
int	view_buffer();
#ifdef STAT
int	fep_showstat();
#endif

/*
 * FunctionNameTable
 */
typedef struct {
	int (*func)();
	char *name;
	char *help;
} FunctionTableEnt;

extern FunctionTableEnt FunctionNameTable[];
extern FunctionTableEnt BuiltinFuncTable[];

char *look_var();
char *push_condition();
char *pop_condition();
char *change_condition();
