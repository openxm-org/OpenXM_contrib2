## $OpenXM: OpenXM_contrib2/asir2000/fft/Makefile.am,v 1.2 2003/03/12 08:22:52 ohara Exp $

asirdir  = ${prefix}/lib/asir
DEFS     = @DEFS@ -DASIR_LIBDIR=\"${asirdir}\"
INCLUDES = -I${top_srcdir}/include -I${top_srcdir}/parse -I${top_srcdir}/io \
           -I${top_srcdir}/gc/include \
           @PARIINC@ @MPIINC@ @X_CFLAGS@
noinst_LIBRARIES = libdft.a
libdft_a_SOURCES = dft.c polmul.c fft_primes.c
