# $OpenXM$

DEFS     = @DEFS@ -DASIR_LIBDIR=\"${prefix}/lib/asir\"
INCLUDES = -I${top_srcdir}/include -I${top_srcdir}/parse -I${top_srcdir}/io \
           -I${top_srcdir}/gc/include \
           @PARIINC@ @MPIINC@ @X_CFLAGS@
noinst_LIBRARIES  = libplot.a
libplot_a_SOURCES = calc.c if.c plotp.c plotg.c ox_plot.c plotf.c \
                    ox_plot_xevent.c smoothing.c
