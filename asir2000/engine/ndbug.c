#include "nd.h"

ND nd_add_q(ND p1,ND p2)
{
	int n,c,can;
	ND r;
	NM m1,m2,mr0,mr,s;
	Q t;

	if ( !p1 ) return p2;
	else if ( !p2 ) return p1;
	else {
		can = 0;
		for ( n = NV(p1), m1 = BDY(p1), m2 = BDY(p2), mr0 = 0; m1 && m2; ) {
			c = DL_COMPARE(DL(m1),DL(m2));
			switch ( c ) {
				case 0:
					addq(CQ(m1),CQ(m2),&t);
					s = m1; m1 = NEXT(m1);
					if ( t ) {
						can++; NEXTNM2(mr0,mr,s); CQ(mr) = (t);
					} else {
						can += 2; FREENM(s);
					}
					s = m2; m2 = NEXT(m2); FREENM(s);
					break;
				case 1:
					s = m1; m1 = NEXT(m1); NEXTNM2(mr0,mr,s);
					break;
				case -1:
					s = m2; m2 = NEXT(m2); NEXTNM2(mr0,mr,s);
					break;
			}
		}
		if ( !mr0 )
			if ( m1 ) mr0 = m1;
			else if ( m2 ) mr0 = m2;
			else return 0;
		else if ( m1 ) NEXT(mr) = m1;
		else if ( m2 ) NEXT(mr) = m2;
		else NEXT(mr) = 0;
		BDY(p1) = mr0;
		SG(p1) = MAX(SG(p1),SG(p2));
		LEN(p1) = LEN(p1)+LEN(p2)-can;
		FREEND(p2);
		return p1;
	}
}
