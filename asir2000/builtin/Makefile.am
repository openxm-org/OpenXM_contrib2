## $OpenXM: OpenXM_contrib2/asir2000/builtin/Makefile.am,v 1.3 2003/03/12 08:22:51 ohara Exp $

asirdir  = ${prefix}/lib/asir
DEFS     = @DEFS@ -DASIR_LIBDIR=\"${asirdir}\"
INCLUDES = -I${top_srcdir}/include -I${top_srcdir}/parse -I${top_srcdir}/io \
           -I${top_srcdir}/gc/include \
           @PARIINC@ @MPIINC@ @X_CFLAGS@
noinst_LIBRARIES  = libfunc.a
libfunc_a_SOURCES = ftab.c algnum.c array.c cplxnum.c ctrl.c dp.c \
                    fctr.c file.c help.c int.c list.c miscf.c \
                    parif.c poly.c pdiv.c print.c pf.c \
                    rat.c reduct.c result.c itvnum.c mattran.c \
                    strobj.c subst.c time.c type.c var.c compobj.c \
                    gr.c dp-supp.c gf.c math.c numerical.c ec.c al.c \
                    batch.c sha1.c user.c mattran.c
