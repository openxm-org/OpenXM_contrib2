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
 * $OpenXM: OpenXM_contrib2/asir2000/parse/id.c,v 1.3 2000/01/26 02:39:33 noro Exp $ 
*/
#include <stdio.h>

static table1(),namemain(),host(),cgen(),instruction();

#define MOD 37
#define MU 0xa9

static unsigned int table[37] = {
0x951abd73,0x13945025,0x3aade018,0x9eabf429,
0xa93f9dc1,0x0e4250b8,0x7eacdf20,0x5edf4cf1,
0x15ccd258,0xfe254fba,0xce4ab7ec,0x85be6c82,
0x35de8043,0xcafc9597,0x0e6237a8,0x16c00729,
0x1622eb3c,0x16d23b40,0xada88ff1,0xf21647cd,
0xc4ce7500,0xac501ff2,0x9e254cc4,0x1e52f533,
0x1fa449cb,0x309aaa45,0x78ae06e3,0x1f3858b2,
0x781fdb92,0xa79eb329,0x7613effb,0xc01f87d1,
0xad4fc2ec,0x9e60acb4,0xb41fdbb3,0x1facf09d,
0x1fd027a3,
};

static int p[7] = { 5, 7, 13, 23, 29, 61, 59 };
static int m[7] = { 1, 4, 9, 12, 16, 57, 51 };
static int s[7] = { 0, 3, 6, 10, 15, 20, 26 };

static table1(seed,index,tab,r)
unsigned int seed;
int index;
unsigned int *tab,*r;
{
	unsigned int v0,v1,v2,v3,v;

	v3 = seed & 0xff;
	v2 = ((seed & 0xff00) >> 8) ^ v3;
	v1 = ((seed & 0xff0000) >> 16) ^ v2;
	v0 = ((seed & 0xff000000) >> 24) ^ v1;
	v = tab[index];
	*r = ((((v0&0xff)<<24)|((v1&0xff)<<16)|((v2&0xff)<<8)|(v3&0xff))^v);
}

static namemain(namebuf,id) 
char *namebuf;
unsigned int *id;
{
	unsigned int v,i,b[4];

	v = 0;
	while ( 1 ) {
		for ( i = 0; i < 4; i++ ) b[i] = 0;
		for ( i = 0; ( i < 4 ) && *namebuf; namebuf++,i++ ) 
			b[i] = (int) *namebuf;
		v ^= ( ( b[3] & 0xff ) << 24 |
		       ( b[2] & 0xff ) << 16 |
		       ( b[1] & 0xff ) << 8  |
		       ( b[0] & 0xff ) );
		if ( *namebuf == 0 ) {
			 *id = v; return;
		 }
	}
}

static host(id)
unsigned int *id;
{
	char name[BUFSIZ];

#if !defined(_PA_RISC1_1)
	if ( !(*id = gethostid()) ) {
#endif
		gethostname(name,BUFSIZ);
		if ( strlen(name) > 0x40 ) 
			name[0x40] = 0;
		namemain(name,id);
#if !defined(_PA_RISC1_1)
	}
#endif
}

static cgen(m1,m2,m3)
unsigned m1,m2,m3;
{
	unsigned int n1,n2,n3,v1;
	unsigned short xml;
	unsigned int id;
	int i;

	host(&id);
	table1(id,id % MOD,table,&n1);
	if ( n1 != m1 )
		return 0;
	xml = ((unsigned short)(m2&0xffff))^((unsigned short)(table[id%MOD]&0xffff));
	if ( ((xml>>8)^(xml&0xff)) != MU )
		return 0;
	n3 = 0;
	for ( i = 0; i < 7; i++ ) 
		n3 |= ( ( ( ( m1 % p[i] ) * m[i] + ( m2 % p[i] ) ) % p[i] ) << s[i] );
	if ( n3 != m3 )
		return 0;
	return 1;
}

check_key()
{
	unsigned int n1,n2,n3;
	int c;
	FILE *fp;
	char key_file[BUFSIZ],buf[BUFSIZ];
	extern char *asir_libdir;
	char *asir_key;

	asir_key = (char *)getenv("ASIR_KEY");
	if ( asir_key )
		strcpy(key_file,asir_key);
	else
		key_file[0] = 0;
	if ( !key_file[0] || !(fp = fopen(key_file,"r")) ) {
		sprintf(key_file,"%s/asir_key",".");
		if ( !(fp = fopen(key_file,"r")) ) {
			sprintf(key_file,"%s/asir_key",asir_libdir);
			if ( !(fp = fopen(key_file,"r")) ) {
				fprintf(stderr,"asir_key not found.\n");
				instruction();
			}
		}
	}
	while ( 1 ) {
		c = fgetc(fp);
		if ( c == EOF )
			break;
		else
			ungetc(c,fp);
		fgets(buf,BUFSIZ,fp);
		sscanf(buf,"%x %x %x",&n1,&n2,&n3);
		if ( cgen(n1,n2,n3) ) {
			fclose(fp);
			return;
		}
	}
	fprintf(stderr,"Key for this machine not found in \"%s\".\n",key_file);
	instruction();
}

static instruction()
{
	unsigned int id;
	extern char *asir_libdir;

	host(&id);
	fprintf(stderr,
"\nYou need a key for each machine on which you want to run Asir.\n\
In order to get the key, send an e-mail consisting of the following\n\
single line to risa@sec.flab.fujitsu.co.jp.\n\n\
ASIR %08x\n\n\
After a while, an e-mail will be returned containing only one line like\n\n\
YYYYYYYY YYYYYYYY YYYYYYYY\n\n\
Create '%s/asir_key'\n\
if necessary and append the returned string to the file.\n",
id,asir_libdir,asir_libdir);
	ExitAsir();
}
