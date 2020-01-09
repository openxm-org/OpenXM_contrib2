## $OpenXM: OpenXM_contrib2/asir2018/Makefile.am,v 1.6 2020/01/08 17:10:53 ohara Exp $

AUTOMAKE_OPTIONS = foreign no-dependencies subdir-objects 1.16

asirdir   = ${prefix}/lib/asir
asir_DATA = lib/alpi lib/bfct lib/bgk lib/const lib/cyclic lib/defs.h \
  lib/dfff lib/dmul lib/dmul102 lib/fctrdata lib/fctrtest lib/fff lib/glib \
  lib/gr lib/ifplot lib/katsura lib/mat lib/nf lib/num lib/primdec \
  lib/primdec_lex lib/ratint lib/robot lib/solve lib/sp lib/sturm lib/sym \
  lib/xm lib/primdec_mod lib/weight

incdir      = ${prefix}/include/asir
inc_HEADERS = parse/comp.h parse/cpp.h parse/parse.h parse/w_stdio.h \
  include/al.h include/b.h include/base.h include/bf.h include/ca-27.h \
  include/ca.h include/com.h include/inline.h include/ox.h include/prime.h \
  include/version.h include/wsio.h include/interval.h

DEFS       = @DEFS@ -DASIR_LIBDIR=\"${asirdir}\"
AM_CPPFLAGS   = -I${top_srcdir}/include -I${top_srcdir}/parse -I${top_srcdir}/io \
              -I${top_srcdir}/gc/include \
              -I${prefix}/include @X_CFLAGS@

## libasir

ASM_SRCS = asm/ddM.c asm/ddN.c asm/asmalpha.c
BUILTIN_SRCS = builtin/ftab.c builtin/algnum.c builtin/array.c \
  builtin/cplxnum.c builtin/ctrl.c builtin/dp.c builtin/fctr.c \
  builtin/file.c builtin/help.c builtin/int.c builtin/list.c \
  builtin/miscf.c builtin/parif.c builtin/poly.c builtin/pdiv.c \
  builtin/print.c builtin/pf.c builtin/rat.c builtin/reduct.c \
  builtin/result.c builtin/itvnum.c builtin/mattran.c builtin/isolv.c \
  builtin/strobj.c builtin/subst.c builtin/time.c builtin/type.c \
  builtin/var.c builtin/compobj.c builtin/gr.c builtin/dp-supp.c \
  builtin/gf.c builtin/math.c builtin/numerical.c builtin/ec.c \
  builtin/al.c builtin/batch.c builtin/sha1.c builtin/user.c \
  builtin/round.c builtin/iarray.c builtin/bfaux.c 
ENGINE_SRCS = engine/init.c engine/mat.c engine/vect.c engine/str.c \
  engine/dist.c engine/distm.c engine/_distm.c engine/ui.c engine/N.c \
  engine/Q.c engine/QM.c engine/real.c engine/bf.c engine/cplx.c \
  engine/num.c engine/pari.c engine/mi.c engine/lmi.c engine/mt19937.c \
  engine/P.c engine/PM.c engine/PD.c engine/PDM.c engine/up.c \
  engine/up_lm.c engine/up_gf2n.c engine/up2.c engine/gf2n.c \
  engine/gfpn.c engine/gfs.c engine/gfspn.c engine/PU.c engine/PUM.c \
  engine/R.c engine/RU.c engine/F.c engine/D.c engine/E.c engine/Ebug.c \
  engine/EZ.c engine/H.c engine/M.c engine/NEZ.c engine/Mgfs.c \
  engine/Hgfs.c engine/Fgfs.c engine/C.c engine/A.c engine/alg.c \
  engine/p-itv.c engine/f-itv.c engine/d-itv.c engine/nd.c \
  engine/ndbug.c engine/Z.c engine/dalg.c engine/gmpq.c 
FFT_SRCS = fft/dft.c fft/polmul.c fft/fft_primes.c
IO_SRCS = io/pexpr.c io/spexpr.c io/biovar.c io/bsave.c io/bload.c \
  io/io.c io/sio.c io/cpexpr.c io/cio.c io/ox.c io/ox_asir.c \
  io/ox_launch.c io/tcpf.c io/ws_fileio.c 
PARSE_SRCS = parse/arith.c parse/comp.c parse/debug.c parse/eval.c \
  parse/evall.c parse/function.c parse/glob.c parse/kwd.c parse/lex.c \
  parse/load.c parse/parser.c parse/puref.c parse/pvar.c parse/quote.c \
  parse/struct.c parse/util.c parse/xdeb.c parse/gc_risa.c
PLOT_SRCS = plot/calc.c plot/if.c plot/plotp.c plot/plotg.c \
  plot/ox_plot.c plot/plotf.c plot/ox_plot_xevent.c plot/smoothing.c

YTAB_SRCS = parse/y.tab.c parse/y.tab.h
BUILT_SOURCES = ${YTAB_SRCS}
parse/lex.o parse/kwd.o: parse/y.tab.h
${YTAB_SRCS}: parse/parse.y
	${YACC} ${YFLAGS} -o parse/y.tab.c -d parse/parse.y 

lib_LTLIBRARIES = libasir@sufx@.la
libasir@sufx@_la_SOURCES = ${ASM_SRCS} ${BUILTIN_SRCS} ${ENGINE_SRCS} ${FFT_SRCS} \
  ${IO_SRCS} ${PARSE_SRCS} ${inc_HEADERS}
nodist_libasir@sufx@_la_SOURCES = ${YTAB_SRCS}

if USE_PLOT
  X11LIB  = @X_LIBS@ -lXaw -lXmu -lXt @X_PRE_LIBS@ @XPMLIB@ -lXext -lX11 @X_EXTRA_LIBS@ 
  libasir@sufx@_la_SOURCES += ${PLOT_SRCS}
endif

LIBS = -L${libdir} @LIBS@ @GCLIB@ @LAPACKLIB@ ${X11LIB} ${FEPLIB} @GMPLIB@ \
  -lm @WSLIB@ ${EXTRALIBS}

## main program

bin_PROGRAMS = asir
asir_SOURCES = parse/main.c
asir_LDADD   = -L${top_srcdir}/.libs -lasir@sufx@ ${LIBS}
asir_DEPENDENCIES = libasir@sufx@.la

CLEANFILES = ${YTAB_SRCS}

install-doc:
	${MKDIR_P} ${DESTDIR}${asirdir}
	-for i in ja en ; do \
		uudecode lib/help-$$i.uu ; \
		gzip -dc help-$$i.tgz | ( cd ${DESTDIR}${asirdir}; tar xf - ) ; \
		rm -f help-$$i.tgz ; \
	done
	-(cd ${DESTDIR}${asirdir}; rm -rf help-jp help-eg help)
	(cd ${DESTDIR}${asirdir}; ${LN_S} help-ja help-jp)
	(cd ${DESTDIR}${asirdir}; ${LN_S} help-en help-eg)
	(cd ${DESTDIR}${asirdir}; ${LN_S} help-en help)

install-openxm: install install-doc
	${MKDIR_P} ${DESTDIR}${asirdir}
	-rm -f ${DESTDIR}${bindir}/ox_asir${EXEEXT}
	(cd ${DESTDIR}${bindir} ; ${LN_S} asir${EXEEXT} ox_asir${EXEEXT})
	-(cd ${DESTDIR}${asirdir}; \
    rm -f asir${EXEEXT} ox_asir${EXEEXT} ox_launch${EXEEXT} ox_plot${EXEEXT})
	(cd ${DESTDIR}${asirdir} ; \
	${LN_S} ../../bin/asir${EXEEXT} asir${EXEEXT} ; \
	${LN_S} asir${EXEEXT} ox_asir${EXEEXT} ; \
	${LN_S} asir${EXEEXT} ox_launch${EXEEXT} ; \
	${LN_S} asir${EXEEXT} ox_plot${EXEEXT})

