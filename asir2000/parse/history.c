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
 * e-mail at risa-admin@flab.fujitsu.co.jp of the detailed specification
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
 * $OpenXM: OpenXM_contrib2/asir2000/parse/history.c,v 1.1.1.1 1999/12/03 07:39:12 noro Exp $ 
*/
#include <stdio.h>
#include <string.h>
#include <malloc.h>

char *prev_hist(void), *next_hist(void), *search_hist(char *p);
void write_hist(char *fname), read_hist(char *fname);
void add_hist(char *p), init_hist(int max);
void reset_traverse(void);

static char **hist_table;
static int hist_full, hist_current, hist_length;
static int hist_traverse, hist_traverse_index;
#define CURRENT (hist_table[hist_current])

void init_hist(int max)
{
	hist_length = max;
	hist_table = (char **)calloc(hist_length,sizeof(char *));
	hist_full = 0;
	hist_current = -1;
}

void reset_traverse(void)
{
	hist_traverse = 0;
}

void add_hist(char *p)
{
	if ( hist_current >= 0 && CURRENT && !strcmp(p,CURRENT) )
		return;
	hist_current++;
	if ( hist_current == hist_length ) {
		hist_current = 0;
		hist_full = 1;
	}
	CURRENT = (char *)realloc(CURRENT,strlen(p)+1);
	strcpy(CURRENT,p);
}

char *search_hist(char *p)
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

void read_hist(char *fname)
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

void write_hist(char *fname)
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

char *next_hist(void) {
	if ( !hist_traverse || hist_traverse_index == hist_current )
		return 0;
	else {
		hist_traverse_index = (hist_traverse_index+1) % hist_length;
		return hist_table[hist_traverse_index];
	}
}

char *prev_hist(void) {
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
