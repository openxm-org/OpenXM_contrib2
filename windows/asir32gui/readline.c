//#include <sys/file.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
//#include	<sys/types.h>
//#include	<sys/stat.h>

#define CTRL(c) ((c)&037)

char *search_hist(),*next_hist(),*prev_hist();

static char **hist_table;
static int hist_full, hist_current, hist_length;
static int hist_traverse, hist_traverse_index;

#define CURRENT (hist_table[hist_current])

init_hist(max)
int max;
{
	hist_length = max;
	hist_table = (char **)GC_malloc(hist_length*sizeof(char *));
	hist_full = 0;
	hist_current = -1;
}

add_hist(p)
char *p;
{
	if ( hist_current >= 0 && !strcmp(p,CURRENT) )
		return;
	hist_current++;
	if ( hist_current == hist_length ) {
		hist_current = 0;
		hist_full = 1;
	}
	CURRENT = (char *)GC_malloc_atomic(strlen(p)+1);
	strcpy(CURRENT,p);
}

char *search_hist(p)
char *p;
{
	int len,slen,index,i;

	if ( *p == '!' ) {
		if ( hist_current < 0 )
			return 0;
		else
			return CURRENT;
	}
	slen = strlen(p);
	len = hist_full ? hist_length : hist_current+1;
	index = hist_current;
	for ( i = len; i; i--, index-- ) {
		if ( index < 0 )
			index += hist_length;
		if ( !strncmp(p,hist_table[index],slen) )
			return hist_table[index];
	}
	return 0;
}

read_hist(fname)
char *fname;
{
	int l;
	char buf[BUFSIZ];
	FILE *fp;

	if ( !(fp = fopen(fname,"rb")) )
		return;
	while ( fgets(buf,BUFSIZ-1,fp) ) {
		l = strlen(buf);
		if ( buf[l-1] == '\n' )
			buf[l-1] = 0;
		add_hist(buf);
	}
	fclose(fp);
}

write_hist(fname)
char *fname;
{
	int len,index,i;
	FILE *fp;

	if ( !(fp = fopen(fname,"wb")) )
		return;
	len = hist_full ? hist_length : hist_current+1;
	index = hist_full ? hist_current+1 : 0;
	for ( i = 0; i < len; i++, index++ ) {
		if ( index >= hist_length )
			index -= hist_length;
		fprintf(fp,"%s\n",hist_table[index]);
	}
	fclose(fp);
}

char *next_hist() {
	if ( !hist_traverse || hist_traverse_index == hist_current )
		return 0;
	else {
		hist_traverse_index = (hist_traverse_index+1) % hist_length;
		return hist_table[hist_traverse_index];
	}
}

char *prev_hist() {
	if ( !hist_traverse ) {
		if ( hist_current >= 0 ) {
			hist_traverse = 1;
			hist_traverse_index = hist_current;
			return hist_table[hist_traverse_index];
		} else
			return 0;
	} else {
		hist_traverse_index--;
		if ( hist_traverse_index < 0 )
			if ( hist_full )
				hist_traverse_index += hist_length;
			else
				hist_traverse_index = 0;
		return hist_table[hist_traverse_index];
	}
}
