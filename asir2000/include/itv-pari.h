/*
 * $OpenXM: OpenXM_contrib2/asir2000/include/itv-pari.h,v 1.2 2003/02/14 22:29:14 ohara Exp $
*/
#ifndef  _ITVPARI_H
#define  _ITVPARI_H

#if defined(PARI)
GEN  PariAddDown(GEN, GEN);
GEN  PariAddirDown(GEN, GEN);
GEN  PariAddrrDown(GEN, GEN);
GEN  PariSubDown(GEN, GEN);
GEN  PariSubirDown(GEN, GEN);
GEN  PariSubriDown(GEN, GEN);
GEN  PariSubrrDown(GEN, GEN);
GEN  PariMulDown(GEN, GEN);
GEN  PariMulirDown(GEN, GEN);
GEN  PariMulrrDown(GEN, GEN);
GEN  PariDivDown(GEN, GEN);
GEN  PariDivrrDown(GEN, GEN);
GEN  PariDivirDown(GEN, GEN);
GEN  PariDivriDown(GEN, GEN);
#endif

#endif /* end of _ITVPARI_H */
