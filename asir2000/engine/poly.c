void D_ADDP(vl,p1,p2,pr)
VL vl;
P p1,p2,*pr;
{
	register DCP dc1,dc2,dcr0,dcr;
	V v1,v2;
	P t;

	if ( !p1 ) 
		*pr = p2;
	else if ( !p2 ) 
		*pr = p1;
	else if ( NUM(p1) )
		if ( NUM(p2) ) 
			ADDNUM(p1,p2,pr);
		else 
			ADDPQ(p2,p1,pr);
	else if ( NUM(p2) ) 
		ADDPQ(p1,p2,pr);
	else if ( ( v1 = VR(p1) ) ==  ( v2 = VR(p2) ) ) {
		for ( dc1 = DC(p1), dc2 = DC(p2), dcr0 = 0; dc1 && dc2; )
			switch ( cmpq(DEG(dc1),DEG(dc2)) ) {
				case 0: 
					ADDP(vl,COEF(dc1),COEF(dc2),&t);
					if ( t )  {
						NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc1); COEF(dcr) = t;
					}
					dc1 = NEXT(dc1); dc2 = NEXT(dc2); break;
				case 1:
					NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc1); COEF(dcr) = COEF(dc1);
					dc1 = NEXT(dc1); break;
				case -1:
					NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc2); COEF(dcr) = COEF(dc2);
					dc2 = NEXT(dc2); break;
			}
		if ( !dcr0 ) 
			if ( dc1 )
				dcr0 = dc1;
			else if ( dc2 ) 
				dcr0 = dc2;
			else {
				*pr = 0;
				return;
			}
		else 
			if ( dc1 ) 
				NEXT(dcr) = dc1;
			else if ( dc2 ) 
				NEXT(dcr) = dc2;
			else 
				NEXT(dcr) = 0;
		MKP(v1,dcr0,*pr);
	} else {
		while ( v1 != VR(vl) && v2 != VR(vl) ) 
			vl = NEXT(vl);
		if ( v1 == VR(vl) ) 
			ADDPTOC(vl,p1,p2,pr);
		else 
			ADDPTOC(vl,p2,p1,pr);
	}
}
	
void D_ADDPQ(p,q,pr)
P p,q,*pr;
{
	DCP dc,dcr,dcr0;
	P t;

	if ( NUM(p) ) 
		ADDNUM(p,q,pr);
	else {
		dc = DC(p);
		for ( dcr0 = 0; dc && cmpq(DEG(dc),0) > 0; dc = NEXT(dc) ) {
			NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = COEF(dc);
		}
		if ( !dc ) {
			NEXTDC(dcr0,dcr);
			DEG(dcr) = 0; COEF(dcr) = q; NEXT(dcr) = 0;
		} else if ( !DEG(dc) ) {
			ADDPQ(COEF(dc),q,&t);
			if ( t ) {
				NEXTDC(dcr0,dcr);
				DEG(dcr) = 0; COEF(dcr) = t;
			}
			NEXT(dcr) = NEXT(dc);
		} else {
			NEXTDC(dcr0,dcr);
			DEG(dcr) = 0; COEF(dcr) = q; NEXT(dcr) = dc;
		}
		MKP(VR(p),dcr0,*pr);
	}
}

void D_ADDPTOC(vl,p,c,pr)
VL vl;
P p,c,*pr;
{
	DCP dc,dcr,dcr0;
	P t;

	for ( dcr0 = 0, dc = DC(p); dc && cmpq(DEG(dc),0) > 0; dc = NEXT(dc) ) {
		NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = COEF(dc);
	}
	if ( !dc ) {
		NEXTDC(dcr0,dcr);
		DEG(dcr) = 0; COEF(dcr) = c; NEXT(dcr) = 0;
	} else if ( !DEG(dc) ) {
		ADDP(vl,COEF(dc),c,&t);
		if ( t ) {
			NEXTDC(dcr0,dcr);
			DEG(dcr) = 0; COEF(dcr) = t;
		}
		NEXT(dcr) = NEXT(dc);
	} else {
		NEXTDC(dcr0,dcr);
		DEG(dcr) = 0; COEF(dcr) = c; NEXT(dcr) = dc;
	}
	MKP(VR(p),dcr0,*pr);
}

void D_SUBP(vl,p1,p2,pr)
VL vl;
P p1,p2,*pr;
{
	P t;

	if ( !p2 ) 
		*pr = p1;
	else {
		CHSGNP(p2,&t); ADDP(vl,p1,t,pr);
	}
}

void D_MULP(vl,p1,p2,pr)
VL vl;
P p1,p2,*pr;
{
	register DCP dc,dct,dcr,dcr0;
	V v1,v2;
	P t,s,u;
	int n1,n2;

	if ( !p1 || !p2 ) *pr = 0;
	else if ( NUM(p1) ) 
		MULPQ(p2,p1,pr);
	else if ( NUM(p2) )
		MULPQ(p1,p2,pr);
	else if ( ( v1 = VR(p1) ) ==  ( v2 = VR(p2) ) ) {
		for ( dc = DC(p1), n1 = 0; dc; dc = NEXT(dc), n1++ );
		for ( dc = DC(p2), n2 = 0; dc; dc = NEXT(dc), n2++ );
		if ( n1 > n2 )
			for ( dc = DC(p2), s = 0; dc; dc = NEXT(dc) ) {
				for ( dcr0 = 0, dct = DC(p1); dct; dct = NEXT(dct) ) {
					NEXTDC(dcr0,dcr); MULP(vl,COEF(dct),COEF(dc),&COEF(dcr));
					addq(DEG(dct),DEG(dc),&DEG(dcr));
				}
				NEXT(dcr) = 0; MKP(v1,dcr0,t);
				ADDP(vl,s,t,&u); s = u; t = u = 0;
			}
		else
			for ( dc = DC(p1), s = 0; dc; dc = NEXT(dc) ) {
				for ( dcr0 = 0, dct = DC(p2); dct; dct = NEXT(dct) ) {
					NEXTDC(dcr0,dcr); MULP(vl,COEF(dct),COEF(dc),&COEF(dcr));
					addq(DEG(dct),DEG(dc),&DEG(dcr));
				}
				NEXT(dcr) = 0; MKP(v1,dcr0,t);
				ADDP(vl,s,t,&u); s = u; t = u = 0;
			}
		*pr = s;
	} else {
		while ( v1 != VR(vl) && v2 != VR(vl) ) 
			vl = NEXT(vl);
		if ( v1 == VR(vl) ) 
			MULPC(vl,p1,p2,pr);
		else 
			MULPC(vl,p2,p1,pr);
	}
}

void D_MULPQ(p,q,pr)
P p,q,*pr;
{
	DCP dc,dcr,dcr0;
	P t;

	if (!p || !q)
		*pr = 0;
	else if ( Uniq(q) ) 
		*pr = p;
	else if ( NUM(p) ) 
		MULNUM(p,q,pr);
	else {
		for ( dcr0 = 0, dc = DC(p); dc; dc = NEXT(dc) ) {
			MULPQ(COEF(dc),q,&t);
			if ( t ) {
				NEXTDC(dcr0,dcr); COEF(dcr) = t; DEG(dcr) = DEG(dc);
			}
		}
		if ( dcr0 ) {
			NEXT(dcr) = 0; MKP(VR(p),dcr0,*pr);
		} else
			*pr = 0;
	}
}

void D_MULPC(vl,p,c,pr)
VL vl;
P p,c,*pr;
{
	DCP dc,dcr,dcr0;
	P t;

	if ( NUM(c) ) 
		MULPQ(p,c,pr);
	else {
		for ( dcr0 = 0, dc = DC(p); dc; dc = NEXT(dc) ) {
			MULP(vl,COEF(dc),c,&t);
			if ( t ) {
				NEXTDC(dcr0,dcr); COEF(dcr) = t; DEG(dcr) = DEG(dc);
			}
		}
		if ( dcr0 ) {
			NEXT(dcr) = 0; MKP(VR(p),dcr0,*pr);
		} else
			*pr = 0;
	}
}

void D_PWRP(vl,p,q,pr)
VL vl;
P p,*pr;
Q q;
{
	DCP dc,dcr;
	int n,i;
	P *x,*y;
	P t,s,u;
	DCP dct;
	P *pt;

	if ( !q ) { 
		*pr = (P)One; 
	} else if ( !p ) 
		*pr = 0;
	else if ( UNIQ(q) ) 
		*pr = p;
	else if ( NUM(p) ) 
		PWRNUM(p,q,pr);
	else {
		dc = DC(p);
		if ( !NEXT(dc) ) {
			NEWDC(dcr);
			PWRP(vl,COEF(dc),q,&COEF(dcr)); mulq(DEG(dc),q,&DEG(dcr));
			NEXT(dcr) = 0; MKP(VR(p),dcr,*pr);
		} else if ( !INT(q) ) {
			error("pwrp: can't calculate fractional power."); *pr = 0;
		} else if ( PL(NM(q)) == 1 ) {
			n = QTOS(q); x = (P *)ALLOCA((n+1)*sizeof(pointer));
			NEWDC(dct); DEG(dct) = DEG(dc); COEF(dct) = COEF(dc);
			NEXT(dct) = 0; MKP(VR(p),dct,t);
			for ( i = 0, u = (P)One; i < n; i++ ) {
				x[i] = u; MULP(vl,u,t,&s); u = s;
			}
			x[n] = u; y = (P *)ALLOCA((n+1)*sizeof(pointer));

			MKP(VR(p),NEXT(dc),t);
			for ( i = 0, u = (P)One; i < n; i++ ) {
				y[i] = u; MULP(vl,u,t,&s); u = s;
			}
			y[n] = u; 
			pt = (P *)ALLOCA((n+1)*sizeof(pointer)); MKBC(n,pt);
			for ( i = 0, u = 0; i <= n; i++ ) {
				MULP(vl,x[i],y[n-i],&t); MULP(vl,t,pt[i],&s); 
				ADDP(vl,u,s,&t); u = t;
			}
			*pr = u;
		} else {
			error("exponent too big");
			*pr = 0;
		}		
	}
}

void D_CHSGNP(p,pr)
P p,*pr;
{
	register DCP dc,dcr,dcr0;
	P t;

	if ( !p ) 
		*pr = NULL;
	else if ( NUM(p) ) {
#if defined(_PA_RISC1_1) || defined(__alpha) || defined(mips) || defined(_IBMR2)
#ifdef FBASE
		chsgnnum((Num)p,(Num *)pr);
#else
		MQ mq;

		NEWMQ(mq); CONT(mq)=mod-CONT((MQ)p); *pr = (P)mq;
#endif
#else
		CHSGNNUM(p,t); *pr = t;
#endif
	} else {
		for ( dcr0 = 0, dc = DC(p); dc; dc = NEXT(dc) ) {
			NEXTDC(dcr0,dcr); CHSGNP(COEF(dc),&COEF(dcr)); DEG(dcr) = DEG(dc);
		}
		NEXT(dcr) = 0; MKP(VR(p),dcr0,*pr);
	}
}

