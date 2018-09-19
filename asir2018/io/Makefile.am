## $OpenXM$

asirdir  = ${prefix}/lib/asir
DEFS     = @DEFS@ -DASIR_LIBDIR=\"${asirdir}\"
AM_CPPFLAGS = -I${top_srcdir}/include -I${top_srcdir}/parse -I${top_srcdir}/io \
           -I${top_srcdir}/gc/include \
           -I${prefix}/include @PARIINC@ @X_CFLAGS@

noinst_LIBRARIES = libio.a
libio_a_SOURCES  = pexpr.c spexpr.c biovar.c bsave.c bload.c io.c sio.c \
                   cpexpr.c cio.c ox.c ox_asir.c ox_launch.c tcpf.c ws_fileio.c
