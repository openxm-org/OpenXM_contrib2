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
 * $OpenXM: OpenXM_contrib2/asir2000/io/biovar.c,v 1.1.1.1 1999/12/03 07:39:11 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"
#include "ox.h"

static V *vtab;
VL file_vl;

void savevl(s,vl)
FILE *s;
VL vl;
{
	int n,i;
	VL tvl;

	for ( tvl = vl, n = 0; tvl; tvl = NEXT(tvl))
		if ( tvl->v->attr != (pointer)V_UC && tvl->v->attr != (pointer)V_PF )
			n++;
	write_int(s,&n);
	vtab = (V *)CALLOC(n,sizeof(V));
	for ( tvl = vl, i = 0; i < n; tvl = NEXT(tvl) )
		if ( tvl->v->attr != (pointer)V_UC && tvl->v->attr != (pointer)V_PF ) {
			vtab[i] = tvl->v; savev(s,vtab[i]); i++;
		}
}

void loadvl(s)
FILE *s;
{
	int n,i,need_reorder;
	V v1,v2;
	VL t,t1;

	read_int(s,&n);
	vtab = (V *)CALLOC(n,sizeof(V));
	for ( i = 0; i < n; i++ )
		loadv(s,&vtab[i]);
	need_reorder = 0;
	for ( i = 0; i < n-1; i++ ) {
		v1 = vtab[i]; v2 = vtab[i+1];
		for ( t = CO; t->v != v1 && t->v != v2; t = NEXT(t) );
		if ( t->v == v2 ) {
			need_reorder = 1; break;
		}
	}
	if ( need_reorder ) {
		for ( i = n-1, t = 0; i >= 0; i-- ) {
			NEWVL(t1); t1->v = vtab[i]; NEXT(t1) = t; t = t1;
		}
		file_vl = t;
	} else
		file_vl = 0;
}

void skipvl(s)
FILE *s;
{
	int n,i,size,len;

	read_int(s,&n);
	for ( i = 0, size = 0; i < n; i++ ) {
		read_int(s,&len); size += len;
	}
	fseek(s,size,1L);
}

void savev(s,v)
FILE *s;
V v;
{
	savestr(s,NAME(v));
}

void loadv(s,vp)
FILE *s;
V *vp;
{
	P p;
	char *name;

	loadstr(s,&name); 
	if ( name ) {
		makevar(name,&p);
		*vp = VR(p);
	} else
		*vp = 0;
}

int save_convv(v)
V v;
{
	int i;

	if ( ox_do_count )
		return 0;
	if ( v->attr == (pointer)V_PF )
		return -1;
	for ( i = 0; vtab[i]; i++ )
		if ( vtab[i] == v )
			return i;
}

V load_convv(vindex)
int vindex;
{
	return vtab[vindex];
}

void swap_bytes(ptr,unit,size)
char *ptr;
int unit,size;
{
	char *p;
	int i,j,hunit;
	char t;
	
	for ( i = 0, p = ptr, hunit = unit/2; i < size; i++, p += unit )
		for ( j = 0; j < hunit; j++ ) {
			t = p[j]; p[j] = p[unit-j-1]; p[unit-j-1] = t;
		}
}
