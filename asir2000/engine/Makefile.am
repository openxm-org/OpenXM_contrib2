## $OpenXM: OpenXM_contrib2/asir2000/engine/Makefile.am,v 1.4 2003/07/23 01:22:50 noro Exp $

asirdir  = ${prefix}/lib/asir
DEFS     = @DEFS@ -DASIR_LIBDIR=\"${asirdir}\"
INCLUDES = -I${top_srcdir}/include -I${top_srcdir}/parse -I${top_srcdir}/io \
           -I${top_srcdir}/gc/include \
           @PARIINC@ @MPIINC@ @X_CFLAGS@
noinst_LIBRARIES = libca.a
libca_a_SOURCES  = init.c mat.c vect.c str.c dist.c distm.c _distm.c ui.c \
  N.c Q.c QM.c real.c bf.c cplx.c num.c pari.c mi.c lmi.c mt19937.c \
  P.c PM.c PD.c PDM.c up.c up_lm.c up_gf2n.c up2.c \
  gf2n.c gfpn.c gfs.c gfspn.c \
  PU.c PUM.c R.c RU.c F.c  \
  D.c E.c Ebug.c EZ.c H.c M.c NEZ.c Mgfs.c Hgfs.c Fgfs.c \
  C.c A.c alg.c p-itv.c f-itv.c d-itv.c pari-mp.c nd.c ndbug.c
