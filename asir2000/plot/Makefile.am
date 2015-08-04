## $OpenXM: OpenXM_contrib2/asir2000/plot/Makefile.am,v 1.5 2013/12/20 03:46:13 noro Exp $

asirdir  = ${prefix}/lib/asir
DEFS     = @DEFS@ -DASIR_LIBDIR=\"${asirdir}\"
AM_CPPFLAGS = -I${top_srcdir}/include -I${top_srcdir}/parse -I${top_srcdir}/io \
           -I${top_srcdir}/gc/include \
           -I${prefix}/include @PARIINC@ @X_CFLAGS@
noinst_LIBRARIES  = libplot.a
libplot_a_SOURCES = calc.c if.c plotp.c plotg.c ox_plot.c plotf.c \
                    ox_plot_xevent.c smoothing.c
