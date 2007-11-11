## $OpenXM: OpenXM_contrib2/asir2000/parse/Makefile.am,v 1.6 2007/02/18 08:46:47 ohara Exp $

incdir      = ${prefix}/include/asir
inc_HEADERS = comp.h cpp.h parse.h w_stdio.h

asirdir  = ${prefix}/lib/asir
DEFS     = @DEFS@ -DASIR_LIBDIR=\"${asirdir}\"
INCLUDES = -I${top_srcdir}/include -I${top_srcdir}/parse -I${top_srcdir}/io \
           -I${top_srcdir}/${GC}/include \
           @PARIINC@ @MPIINC@ @X_CFLAGS@

CLEANFILES         = y.tab.c y.tab.h
BUILT_SOURCES      = y.tab.c y.tab.h
noinst_LIBRARIES   = libparse.a
libparse_a_SOURCES = arith.c comp.c debug.c eval.c evall.c function.c \
    glob.c kwd.c lex.c load.c parser.c puref.c pvar.c quote.c \
    struct.c util.c xdeb.c ${inc_HEADERS}
nodist_libparse_a_SOURCES = y.tab.c y.tab.h

ex.o kwd.o: y.tab.h
y.tab.c y.tab.h: parse.y
	${YACC} ${YFLAGS} -d parse.y
