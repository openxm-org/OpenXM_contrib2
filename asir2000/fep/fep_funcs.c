/*	Copyright (c) 1987, 1988 by Software Research Associates, Inc.	*/

#ifndef lint
static char rcsid[]=
"$Header$ (SRA)";
#endif /* lint */

#include <stdio.h>
#include "fep_defs.h"
#include "fep_funcs.h"

/*
 * FunctionNameTable
 */
FunctionTableEnt FunctionNameTable[] = {
	{mark,				"mark",
	 "Mark position"},
	{fep_abort,			"abort",
	 "Abort function"},
	{self_insert,			"self-insert",
	 "Insert the character"},
	{beginning_of_line,		"beginning-of-line",
	 "Jump to beginning of line"},
	{backward_character,		"backward-character",
	 "Backward character"},
	{backward_word,			"backward-word",
	 "Backward word (alpha-numeric)"},
	{backward_Word,			"backward-Word",
	 "Backward word (non-space)"},
	{end_of_line,			"end-of-line",
	 "Jump to end of line"},
	{forward_character,		"forward-character",
	 "Forward character"},
	{forward_word,			"forward-word",
	 "Forward word (alpha-numeric)"},
	{forward_Word,			"forward-Word",
	 "Forward word (non-space)"}, 
	{forward_to_end_of_word,	"forward-to-end-of-word",
	 "Forward to end of word (alpha-numeric)"},
	{forward_to_end_of_Word,	"forward-to-end-of-Word",
	 "Forward to end of word (non-space)"},
	{delete_previous_character,	"delete-previous-character",
	 "Delete previous character"},
	{delete_next_character,		"delete-next-character",
	 "Delete next character"},
	{insert_tab,			"insert-tab",
	 "Insert tab"},
	{new_line,			"new-line",
	 "Insert newline"},
	{insert_and_flush,		"insert-and-flush",
	 "Insert the character and flush buffer"},
	{send_eof,			"send-eof",
	 "Send eof"},
	{kill_to_end_of_line,		"kill-to-end-of-line",
	 "Delete current position to eol"},
	{kill_to_top_of_line,		"kill-to-top-of-line",
	 "Delete tol to current position"},
	{delete_to_kill_buffer,		"delete-to-kill-buffer",
	 "Delete resion to buffer"},
	{yank_from_kill_buffer,		"yank-from-kill-buffer",
	 "Yank from kill buffer"},
	{clear_screen,			"clear-screen",
	 "Clear screen"},
	{next_history,			"next-history",
	 "Get next history"},
	{previous_history,		"previous-history",
	 "Get previous history"},
	{insert_next_history,		"insert-next-history",
	 "Insert next history"},
	{insert_previous_history,	"insert-previous-history",
	 "Insert previous history"},
	{insert_current_history,	"insert-current-history",
	 "Insert current history"},
	{ignore,			"ignore",
	 "Ignore"},
	{delete_line,			"delete-line",
	 "Delete whole line"},
	{literal_next,			"literal-next",
	 "Treat next character as literal"},
	{delete_previous_word,		"delete-previous-word",
	 "Delete previous word (alpha-numeric)"},
	{delete_previous_Word,		"delete-previous-Word",
	 "Delete previous word (non-space)"},
	{delete_next_word,		"delete-next-word",
	 "Delete next word (alpha-numeric)"},
	{delete_next_Word,		"delete-next-Word",
	 "Delete next word (non-space)"},
	{reprint,			"reprint",
	 "Reprint line"},
	{show_history,			"show-history",
	 "Show history"},
	{show_bindings,			"show-bindings",
	 "Show binding table"},
	{expand_file_name,		"expand-file-name",
	 "Expand file name"},
	{list_file_name,		"list-file-name",
	 "List file name"},
	{(FUNC)terminate,			"terminate",
	 "Terminate fep"},
	{suspend,			"suspend",
	 "Suspend fep"},
	{toggle_through,		"toggle-through",
	 "Change transparency mode"},
	{fix_transparency,		"fix-transparency",
	 "Check tty and change transparency mode"},
	{invoke_shell,			"invoke-shell",
	 "Invoke shell process"},
	{search_reverse,		"search-reverse",
	 "Search backward last !history"},
	{search_forward,		"search-forward",
	 "Search forward last !history"},
	{fep_start_script,		"start-script",
	 "Start script"},
	{fep_end_script,		"end-script",
	 "End script"},
	{fep_repaint,			"repaint",
	 "Repaint screen"},
	{view_buffer,			"view-buffer",
	 "View buffer"},
	{show_help,			"help",
	 "Show help"},

	{vi_c,				"vi-c",
	 "Vi c? commands"},
	{vi_d,				"vi-d",
	 "Vi d? commands"},
	{vi_edit,			"vi-edit",
	 "Vi edit commands"},
	{vi_ins_edit,			"vi-ins-edit",
	 "Vi insert mode"},
	{vi_motion,			"vi-motion",
	 "Vi cursor motion commands"},
	{vi_new_line,			"vi-new-line",
	 "Vi new line"},
	{vi_num,			"vi-num",
	 "Vi prefix number"},

	{NULL,				NULL}
};

/*
 * Table of built-in functions
 */
FunctionTableEnt BuiltinFuncTable[] = {
	{bind_to_key,			"fep-bind",
	 "Change binding table"},
	{alias,				"fep-alias",
	 "Set or show aliases"},
	{unalias,			"fep-unalias",
	 "Delete alias"},
	{set,				"fep-set",
	 "Set or show variables"},
	{unset,				"fep-unset",
	 "Unset variables"},
	{fep_chdir,			"fep-cd",
	 "Change directory"},
	{fep_chdir,			"fep-chdir",
	 "Change directory"},
	{fep_pwd,			"fep-pwd",
	 "Print working directory"},
	{fep_history,			"fep-history",
	 "Show history"},
	{fep_echo,			"fep-echo",
	 "Print arguments"},
	{fep_source,			"fep-source",
	 "Read and execute file"},
	{fep_command,			"fep-command",
	 "Invoke unix command"},
	{fep_command,			"fep-!",
	 "Same as fep_command"},
	{fep_save_history,		"fep-save-history",
	 "Save history to file"},
	{fep_read_history,		"fep-read-history",
	 "Read history from file"},
	{fep_read_from_file,		"fep-read-from-file",
	 "Send file contens to slave command"},
	{fep_read_from_file,		"fep-<",
	 "Same as fep_read_from_file"},
	{fep_read_from_command,		"fep-read-from-command",
	 "Send output of command to slave command"},
	{fep_read_from_command,		"fep-<!",
	 "Same as fep_read_from_command"},
	{fep_start_script,		"fep-start-script",
	 "Start script"},
	{fep_start_script,		"fep-script",
	 "Start script"},
	{fep_end_script,		"fep-end-script",
	 "End script"},
	{fep_if,			"fep-if",
	 "Operation \"if\""},
	{fep_if,			"fep-elseif",
	 "Operation \"endif\""},
	{fep_else,			"fep-else",
	 "Operation \"else\""},
	{fep_endif,			"fep-endif",
	 "Operation \"endif\""},
	{(FUNC)terminate,			"fep-exit",
	 "Terminate fep"},
	{suspend,			"fep-suspend",
	 "Suspend fep"},
	{show_bindings,			"fep-show-bind",
	 "Print bindings"},
	{show_bindings,			"fep-showbind",
	 "Print bindings"},
	{fep_repaint,			"fep-repaint",
	 "Repaint screen"},
#ifdef STAT
	{fep_showstat,			"fep-showstat",
	 "Show statistical information"},
#endif
	{NULL,				NULL}
};
