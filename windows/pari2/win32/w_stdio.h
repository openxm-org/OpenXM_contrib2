#include <stdio.h>

int get_line(char *);
void put_line(char *);

int w_filbuf(void);
int w_flushbuf(void);
void w_noflush_stderr(int);

int get_char(void);     
char *get_string(char *s,int n);
int unget_char(int c);
int put_char(int c,FILE *file);

int w_fgetc(FILE *file);
char *w_fgets(char *s,int n, FILE *file);
int w_ungetc(int c, FILE *file);
int w_fputc(int c, FILE *file);
int w_fputs(char *s, FILE *file);
int w_printf(char *format, ...);
int w_fprintf(FILE *file, char *format, ...);
int w_fflush(FILE *file);

int check_break(void);
