/*	Copyright (c) 1987, 1988 by Software Research Associates, Inc.	*/

#ifndef lint
# define FEP_FUNCS \
    "$Header$ (SRA)"
#endif /* lint */

/*
 * command line edit functions
 */
#if defined(__INTERIX)
#include <sys/cdefs.h>
#endif
#if defined(__CYGWIN__) || defined(sun)
void	terminate(int);
#else
void	terminate __P((int));
#endif

/*
 * fep builtin command functions.
 */
int	suspend();
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

char *push_condition();
char *pop_condition();
char *change_condition();

int getcharacter();
void input_handler();
void recover_signal ();
void fix_signal ();
int set_buffer (BUFFER *bp, int size);
int buf_read (int fd, BUFFER *bp);
void buf_put (BUFFER *bp, char *s);
void swallow_output();
void catchsig(int n);
void exec_to_command(char *argv[]);
void fix_tty();
void kill_process();
void terminate(int n);
int send_int_sig();
void get_pty_master();
void get_pty_slave();
void recover_tty();
void look_cmdinfo (char *command);
void usageAndExit();
void sigwinch(int dummy);
int	set_var		(char *name, char *value);
char	*look_var	(char *name);
int	lookd_var	(char *name);
void set_only_var (char *name, char *value);
VAR * getvp (char *name, int alloc, VAR **lastvp);
void unset_var (char *name);
void show_varlist ();

void init_hist(int size);
char * itoa (int i);
void addHistory(char *string);
void resetCurrentHistory();
char * getHistory(int num);
char * getCurrentHistory();
char * getPreviousHistory();
char * getNextHistory();
int getOldestHistNum();
int getYoungestHistNum();
char * getOldestHistory();
char * getYoungestHistory();
int getCurrentHistNum();
char * allocAndCopyThere(char *string);
char * historyExtract(char *string);
char * search_reverse_history (char *string);
char * search_forward_history (char *string);
int changeHistorySize(int newsize);
void fep_history (char *comline);
void hist_showHistory (int num);
char * mk_home_relative (char *cp);
void fep_save_history (char *comline);
void save_history (char *file, int num);
void fep_read_history (char *comline);
void read_history (char *file);
void message(char *messageString);
void errorBell();
void ctlprint(char *string);
void printS (char *string);
int is_empty_line(char *line);
void putChar(char c);
char * x_dirname (char *dir);
// DIR * x_opendir (char *dir);
int scmp (char **a, char **b);
int prefix (char *sub, char *str);
int any (int c, char *s);
// int max (int d1, int d2);

int callfunc (FUNC ft[], int c);
int self_insert(CHAR c);
int insert_string (CHAR *s);
int mark ();
int set_mark (int pos);
int delete_to_kill_buffer ();
int beginning_of_line();
int end_of_line();
int backward_character();
int backward_n_character(int n);
int backward_word ();
int backward_n_word (int n);
int backward_Word ();
int backward_n_Word (int n);
int forward_character();
int forward_n_character(int n);
int forward_word ();
int forward_n_word (int n);
int forward_Word ();
int forward_n_Word (int n);
int forward_to_end_of_word ();
int forward_to_end_of_n_word (int n);
int forward_to_end_of_Word ();
int forward_to_end_of_n_Word (int n);
int delete_previous_character();
int delete_previous_n_character(int n);
int delete_previous_word();
int delete_previous_n_word(int n);
int delete_previous_Word();
int delete_previous_n_Word(int n);
int delete_next_character ();
int delete_next_n_character (int n);
int delete_next_word ();
int delete_next_n_word (int n);
int delete_next_Word ();
int delete_next_n_Word (int n);
int delete_line();
int kill_to_top_of_line();
int kill_to_end_of_line();
int insert_tab();
int new_line();
int is_tol();
int is_eol();
int refer_history();
int search_reverse ();
int search_forward ();
int search_history (int direct);
int insert_and_flush(char c);
int send_eof();
int alarm_on_eof ();
int clear_screen();
int next_history();
int previous_history();
int insert_current_history();
int insert_next_history();
int insert_previous_history();
int ignore();
int literal_next();
int reprint();
int howlong(char *s, int n);
int expand_file_name ();
int list_file_name ();
int bind_key (FUNC ft[], FUNC func, char *s, FUNC dfunc);
void init_bind_table ();
void init_edit_params ();
void initEmacsBindings (FUNC cft[], FUNC aft[]);
int fep_abort();
void yank_from_kill_buffer ();
void moveto (int position);
void show_history();
void print_com_line();
void repeat(char c, int n);
void repeat_string(char *s, int n);
void clear_edit_line ();
void recover_edit_line (int put_prompt);
void list_remote_file (char *host, char *pattern);
void set_default_vars ();
int iskanji_in_string (char *s, int i);
char *search_string (char *s, char *lookup);
void reverse_strcpy (char *to, char *from);
int mkargv (char *s, char *argv[], int maxarg);
void showArgs (char *comline);

void fep_if (char *comline);
void fep_else ();
void fep_endif ();
void bind_to_key (char *comline);
void alias(char *comline);
void unalias (char *comline);
void set (char *comline);
void unset(char *comline);
void toggle_through();
void fix_transparency();
void putch (int c);
void show_help ();
void fep_chdir (char *line);
void fep_pwd (char *line);
void fep_echo (char *comline);
void fep_command (char *comline);
void fep_source (char *comline);
void sourceRcFile ();
void source_file (char *file);
void invoke_shell ();
void invoke_command (char *cmd);
void fep_read_from_file (char *comline);
void fep_read_from_command (char *comline);
void fep_start_script (char *comline);
void fep_end_script ();
void fep_repaint(char *comline);
void fep_showstat ();
int executeBuiltInFunction (char *comline, char **more);
int is_same_command (char *a, char *b);
int show_bindings ();
int condition ();
int view_buffer (char *comline);
int tgetent(char *bp, const char *name);
int tgetflag(char *id);
int tgetnum(char *id);
int tputs(const char *str, int affcnt, int (*putc)(int));
void set_alias (char *name, char *value);
void unset_alias (char *name);
void show_aliaslist (char *a);
char * look_alias (char *name);
void	catchsig (int);	

int vi_edit (int c);
int altenateEditmode ();
int vi_num (int c);
int vi_motion (int c);
int vi_c (int c);
int vi_d (int c);
int vi_new_line (int c);
int vi_ins_edit (int c);
