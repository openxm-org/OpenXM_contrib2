## $OpenXM: OpenXM_contrib2/asir2000/builtin/Makefile.am,v 1.11 2015/08/04 06:20:44 noro Exp $

asirdir  = ${prefix}/lib/asir
DEFS     = @DEFS@ -DASIR_LIBDIR=\"${asirdir}\"
AM_CPPFLAGS = -I${top_srcdir}/include -I${top_srcdir}/parse -I${top_srcdir}/io \
           -I${top_srcdir}/gc/include \
           -I${prefix}/include @PARIINC@ @X_CFLAGS@
noinst_LIBRARIES  = libfunc.a
libfunc_a_SOURCES = ftab.c algnum.c array.c cplxnum.c ctrl.c dp.c \
                    fctr.c file.c help.c int.c list.c miscf.c \
                    parif.c poly.c pdiv.c print.c pf.c \
                    rat.c reduct.c result.c itvnum.c mattran.c isolv.c \
                    strobj.c subst.c time.c type.c var.c compobj.c \
                    gr.c dp-supp.c gf.c math.c numerical.c ec.c al.c \
                    batch.c sha1.c user.c round.c iarray.c bfaux.c
