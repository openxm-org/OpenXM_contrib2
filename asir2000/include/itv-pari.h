/*
 * $OpenXM: $
*/
#ifndef	_ITVPARI_H
#define	_ITVPARI_H

#if PARI
GEN	PariAddDown(GEN, GEN);
GEN	PariAddirDown(GEN, GEN);
GEN	PariAddrrDown(GEN, GEN);
GEN	PariSubDown(GEN, GEN);
GEN	PariSubirDown(GEN, GEN);
GEN	PariSubriDown(GEN, GEN);
GEN	PariSubrrDown(GEN, GEN);
GEN	PariMulDown(GEN, GEN);
GEN	PariMulirDown(GEN, GEN);
GEN	PariMulrrDown(GEN, GEN);
GEN	PariDivDown(GEN, GEN);
GEN	PariDivrrDown(GEN, GEN);
GEN	PariDivirDown(GEN, GEN);
GEN	PariDivriDown(GEN, GEN);
#endif

#endif /* end of _ITVPARI_H */
