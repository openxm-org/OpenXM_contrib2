## $OpenXM: OpenXM_contrib2/asir2000/plot/Makefile.am,v 1.3 2003/03/20 07:34:13 ohara Exp $

asirdir  = ${prefix}/lib/asir
DEFS     = @DEFS@ -DASIR_LIBDIR=\"${asirdir}\"
INCLUDES = -I${top_srcdir}/include -I${top_srcdir}/parse -I${top_srcdir}/io \
           -I${top_srcdir}/gc/include \
           @PARIINC@ @X_CFLAGS@
noinst_LIBRARIES  = libplot.a
libplot_a_SOURCES = calc.c if.c plotp.c plotg.c ox_plot.c plotf.c \
                    ox_plot_xevent.c smoothing.c
