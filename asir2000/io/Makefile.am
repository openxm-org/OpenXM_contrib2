## $OpenXM: OpenXM_contrib2/asir2000/io/Makefile.am,v 1.4 2009/02/13 13:21:20 ohara Exp $

asirdir  = ${prefix}/lib/asir
DEFS     = @DEFS@ -DASIR_LIBDIR=\"${asirdir}\"
INCLUDES = -I${top_srcdir}/include -I${top_srcdir}/parse -I${top_srcdir}/io \
           -I${top_srcdir}/gc/include \
           -I${prefix}/include @PARIINC@ @X_CFLAGS@

noinst_LIBRARIES = libio.a
libio_a_SOURCES  = pexpr.c spexpr.c biovar.c bsave.c bload.c io.c sio.c \
                   cpexpr.c cio.c ox.c ox_asir.c ox_launch.c tcpf.c ws_fileio.c
