## $OpenXM: OpenXM_contrib2/asir2000/fft/Makefile.am,v 1.5 2013/12/20 03:46:13 noro Exp $

asirdir  = ${prefix}/lib/asir
DEFS     = @DEFS@ -DASIR_LIBDIR=\"${asirdir}\"
AM_CPPFLAGS = -I${top_srcdir}/include -I${top_srcdir}/parse -I${top_srcdir}/io \
           -I${top_srcdir}/gc/include \
           -I${prefix}/include @PARIINC@ @X_CFLAGS@
noinst_LIBRARIES = libdft.a
libdft_a_SOURCES = dft.c polmul.c fft_primes.c
