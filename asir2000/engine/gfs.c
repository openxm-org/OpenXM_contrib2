/*
 * Copyright (c) 1994-2000 FUJITSU LABORATORIES LIMITED 
 * All rights reserved.
 * 
 * FUJITSU LABORATORIES LIMITED ("FLL") hereby grants you a ligfsted,
 * non-exclusive and royalty-free license to use, copy, modify and
 * redistribute, solely for non-commercial and non-profit purposes, the
 * computer program, "Risa/Asir" ("SOFTWARE"), subject to the terms and
 * conditions of this Agreement. For the avoidance of doubt, you acquire
 * only a ligfsted right to use the SOFTWARE hereunder, and FLL or any
 * third party developer retains all rights, including but not ligfsted to
 * copyrights, in and to the SOFTWARE.
 * 
 * (1) FLL does not grant you a license in any way for commercial
 * purposes. You may use the SOFTWARE only for non-commercial and
 * non-profit purposes only, such as acadegfsc, research and internal
 * business use.
 * (2) The SOFTWARE is protected by the Copyright Law of Japan and
 * international copyright treaties. If you make copies of the SOFTWARE,
 * with or without modification, as pergfstted hereunder, you shall affix
 * to all such copies of the SOFTWARE the above copyright notice.
 * (3) An explicit reference to this SOFTWARE and its copyright owner
 * shall be made on your publication or presentation in any form of the
 * results obtained by use of the SOFTWARE.
 * (4) In the event that you modify the SOFTWARE, you shall notify FLL by
 * e-mail at risa-adgfsn@sec.flab.fujitsu.co.jp of the detailed specification
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
 * $OpenXM: OpenXM_contrib2/asir2000/engine/gfs.c,v 1.1 2001/03/13 01:10:25 noro Exp $
*/
#include "ca.h"

/* q = p^n */

P current_gfs_ext;
int current_gfs_p;
int current_gfs_q;
int current_gfs_q1;
int *current_gfs_plus1;
int *current_gfs_ntoi;
int *current_gfs_iton;

void chsgngfs();
void generate_defpoly_um();

void dec_um(p,a,u)
int p,a;
UM u;
{
	int i;

	for ( i = 0; a; i++, a /= p )
		COEF(u)[i] = a%p;
	DEG(u) = i-1;
}

/*
 * an element of GF(p^n) f(x)=a(n-1)*x^(n-1)+...+a(0)
 * is encodeded to f(p).
 * current_gfs_iton[i] = r^i mod p (i=0,...,p-2)
 * current_gfs_iton[p-1] = 0
 */

void setmod_sf(p,n)
int p,n;
{
	int r,i,q1,q,t,t1;
	UM dp;

	for ( i = 0, q = 1; i < n; i++ )
		q *= p;
	dp = UMALLOC(n);
	generate_defpoly_um(p,n,dp);
	r = generate_primitive_root_enc(p,n,dp);
	current_gfs_p = p;
	current_gfs_q = q;
	current_gfs_q1 = q1 = q-1;
	if ( n > 1 )
		umtop(CO->v,dp,&current_gfs_ext);
	else
		current_gfs_ext = 0;
	current_gfs_iton = (int *)MALLOC(q1*sizeof(int));
	current_gfs_iton[0] = 1;
	for ( i = 1; i < q1; i++ )
		current_gfs_iton[i] = mulremum_enc(p,n,dp,current_gfs_iton[i-1],r);

	current_gfs_ntoi = (int *)MALLOC(q*sizeof(int));
	current_gfs_ntoi[0] = -1;
	for ( i = 0; i < q1; i++ )
		current_gfs_ntoi[current_gfs_iton[i]] = i;
		
	current_gfs_plus1 = (int *)MALLOC(q*sizeof(int));
	for ( i = 0; i < q1; i++ ) {
		t = current_gfs_iton[i];
		/* add 1 to the constant part */
		t1 = (t/p)*p+((t+1)%p);
		current_gfs_plus1[i] = current_gfs_ntoi[t1];
	}
}

void generate_defpoly_um(p,n,dp)
int p,n;
UM dp;
{
	int i,j,a,c,q;
	UM wf,wdf,wgcd;

	wf = W_UMALLOC(n);
	wdf = W_UMALLOC(n);
	wgcd = W_UMALLOC(n);
	COEF(dp)[n] = 1;
	DEG(dp) = n;
	for ( i = 0, q = 1; i < n; i++ )
		q *= p;
	for ( i = 0; i < q; i++ ) {
		for ( j = 0, a = i; a; j++, a /= p )
			COEF(dp)[j] = a%p;
		for ( ; j < n; j++ )
			COEF(dp)[j] = 0;
		cpyum(dp,wf);
		diffum(p,dp,wdf);
		gcdum(p,wf,wdf,wgcd);
		if ( DEG(wgcd) >= 1 )
			continue;
		mini(p,dp,wf);
		if ( DEG(wf) <= 0 )
			return;
	}
}

int generate_primitive_root_enc(p,n,dp)
int p,n;
UM dp;
{
	int i,r,rj,j,q;

	for ( i = 0, q = 1; i < n; i++ )
		 q *= p;
	for ( r = n==1?2:p; r < q; r++ ) {
		rj = r;
		for ( j = 1; j < q-1 && rj != 1; j++ )
			rj = mulremum_enc(p,n,dp,rj,r);
		if ( j == q-1 )
			return r;
	}
}

/* [a(p)]*[b(p)] in GF(p^n) -> [a(x)*b(x) mod dp(x)]_{x->p} */

int mulremum_enc(p,n,dp,a,b)
int p,n;
UM dp;
int a,b;
{
	int i,dr,r;
	UM wa,wb,wc,wq;

	if ( n == 1 )
		return (a*b)%p;
	if ( !a || !b )
		return 0;

	wa = W_UMALLOC(n);
	dec_um(p,a,wa);

	wb = W_UMALLOC(n);
	dec_um(p,b,wb);

	wc = W_UMALLOC(2*n);
	wq = W_UMALLOC(2*n);
	mulum(p,wa,wb,wc);
	dr = divum(p,wc,dp,wq);
	for ( i = dr, r = 0; i >= 0; i-- )
		r = r*p+COEF(wc)[i];
	return r;
}

void mqtogfs(a,c)
MQ a;
GFS *c;
{
	if ( !a )
		*c = 0;
	else {
		MKGFS(current_gfs_ntoi[CONT(a)],*c);	
	}
}

void gfstomq(a,c)
GFS a;
MQ *c;
{
	if ( !a )
		*c = 0;
	else {
		UTOMQ(current_gfs_iton[CONT(a)],*c);
	}
}

void ntogfs(a,b)
Obj a;
GFS *b;
{
	P t;

	if ( !current_gfs_q1 )
		error("addgfs : current_gfs_q is not set");
	if ( !a || (OID(a)==O_N && NID(a) == N_GFS) )
		*b = (GFS)a;
	else if ( OID(a) == O_N && NID(a) == N_M )
		mqtogfs(a,b);
	else if ( OID(a) == O_N && NID(a) == N_Q ) {
		ptomp(current_gfs_q,(P)a,&t); mqtogfs(t,b);
	} else
		error("ntogfs : invalid argument");
}

void addgfs(a,b,c)
GFS a,b;
GFS *c;
{
	int ai,bi,ci;
	GFS z;

	ntogfs(a,&z); a = z;
	ntogfs(b,&z); b = z;
	if ( !a )
		*c = b;
	else if ( !b )
		*c = a;
	else {
		ai = CONT(a); bi = CONT(b);
		if ( ai > bi ) {
			/* tab[ai]+tab[bi] = tab[bi](tab[ai-bi]+1) */
			ci = current_gfs_plus1[ai-bi];
			if ( ci < 0 )
				*c = 0;
			else {
				ci += bi;
				if ( ci >= current_gfs_q1 )
					ci -= current_gfs_q1;
				MKGFS(ci,*c);
			}
		} else {
			/* tab[ai]+tab[bi] = tab[ai](tab[bi-ai]+1) */
			ci = current_gfs_plus1[bi-ai];
			if ( ci < 0 )
				*c = 0;
			else {
				ci += ai;
				if ( ci >= current_gfs_q1 )
					ci -= current_gfs_q1;
				MKGFS(ci,*c);
			}
		}
	}
}

void subgfs(a,b,c)
GFS a,b;
GFS *c;
{
	GFS t,z;

	ntogfs(a,&z); a = z;
	ntogfs(b,&z); b = z;
	if ( !b )
		*c = a;
	else {
		chsgngfs(b,&t);
		addgfs(a,t,c);
	}
}

void mulgfs(a,b,c)
GFS a,b;
GFS *c;
{
	int ai;
	GFS z;

	ntogfs(a,&z); a = z;
	ntogfs(b,&z); b = z;
	if ( !a || !b )
		*c = 0;
	else {
		ai = CONT(a) + CONT(b);
		if ( ai >= current_gfs_q1 )
			ai -= current_gfs_q1;
		MKGFS(ai,*c);
	}
}

void divgfs(a,b,c)
GFS a,b;
GFS *c;
{
	int ai;
	GFS z;

	ntogfs(a,&z); a = z;
	ntogfs(b,&z); b = z;
	if ( !b )
		error("divgfs : division by 0");
	else if ( !a )
		*c = 0;
	else {
		ai = CONT(a) - CONT(b);
		if ( ai < 0 )
			ai += current_gfs_q1;
		MKGFS(ai,*c);
	}
}

void chsgngfs(a,c)
GFS a,*c;
{
	int ai;
	GFS z;

	ntogfs(a,&z); a = z;
	if ( !a )
		*c = 0;
	else if ( current_gfs_q1&1 )
		*c = a;
	else {	
		/* r^((q-1)/2) = -1 */
		ai = CONT(a)+(current_gfs_q1>>1);
		if ( ai >= current_gfs_q1 )
			ai -= current_gfs_q1;
		MKGFS(ai,*c);
	}
}

void pwrgfs(a,b,c)
GFS a;
Q b;
GFS *c;
{
	N an,tn,rn;
	GFS t,s,z;

	ntogfs(a,&z); a = z;
	if ( !b )
		MKGFS(0,*c);
	else if ( !a )
		*c = 0;
	else {
		STON(CONT(a),an); muln(an,NM(b),&tn);
		STON(current_gfs_q1,an); remn(tn,an,&rn);
		if ( !rn )
			MKGFS(0,*c);
		else if ( SGN(b) > 0 )
			MKGFS(BD(rn)[0],*c);
		else {
			MKGFS(0,t);
			MKGFS(BD(rn)[0],s);
			divgfs(t,s,c);
		}
	}
}

int cmpgfs(a,b)
GFS a,b;
{
	GFS z;

	ntogfs(a,&z); a = z;
	if ( !a )
		return !b ? 0 : -1;
	else
		if ( !b )
			return 1;
		else {
			if ( CONT(a) > CONT(b) )
				return 1;
			else if ( CONT(a) < CONT(b) )
				return -1;
			else
				return 0;
		}
}
