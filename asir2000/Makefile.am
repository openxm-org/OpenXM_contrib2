# $OpenXM: OpenXM_contrib2/asir2000/Makefile.am,v 1.1 2003/02/23 03:05:35 ohara Exp $

AUTOMAKE_OPTIONS = foreign no-dependencies 1.5
if USE_PLOT
    PLOTDIR = plot
    PLLIB   = plot/libplot.a
    X11LIB  = @X_LIBS@ -lXaw -lXmu -lXt @X_PRE_LIBS@ -lXpm -lXext -lX11 @X_EXTRA_LIBS@ 
endif

SUBDIRS  = asm builtin engine fft include io lib parse ${PLOTDIR}

DEFS     = @DEFS@ -DASIR_LIBDIR=\"${prefix}/lib/asir\"
INCLUDES = -I${top_srcdir}/include -I${top_srcdir}/parse -I${top_srcdir}/io \
           -I${top_srcdir}/gc/include \
           @PARIINC@ @MPIINC@ @X_CFLAGS@

GLIB  = libasir-gc.a
ALIB  = asm/libasm.a
BLIB  = builtin/libfunc.a
ELIB  = engine/libca.a
FLIB  = fft/libdft.a
IOLIB = io/libio.a
PLIB  = parse/libparse.a

# The ASIRLIB cannot be rearrenged.
ASIRLIB  = ${BLIB} ${PLIB} ${IOLIB} ${PLLIB} ${ELIB} ${FLIB} ${ALIB}
LIBS     = @LIBS@ @PARILIB@ @MPILIB@ @LAPACKLIB@ ${X11LIB} ${FEPLIB} -lm ${EXTRALIBS}
# EXTRALIBS = @EXTRALIBS@

bin_PROGRAMS   = asir
asir_SOURCES   = parse/main.c
asir_LDADD     = ${ASIRLIB} ${GLIB}
lib_LIBRARIES  = libasir.a libasir-gc.a
CLEANFILES     = libasir.a libasir-gc.a asir${EXEEXT}

libasir_gc_a_SOURCES =

umain.o: parse/main.c
	${COMPILE} -DUINIT main.c -o umain.o

libasir-gc.a: parse/gc_risa.o
	-if [ ! -f ${top_srcdir}/gc/.configure_done ]; then \
	    (cd ${top_srcdir}/gc; ./configure --disable-threads) ; \
	    touch ${top_srcdir}/gc/.configure_done ; \
	fi
	(cd ${top_srcdir}/gc; ${MAKE})
	cp ${top_srcdir}/gc/.libs/libgc.a ${top_srcdir}/${GLIB}
	${AR} q ${top_srcdir}/${GLIB} ${top_srcdir}/parse/gc_risa.o
	${RANLIB} ${top_srcdir}/${GLIB}

libasir.a: ${ASIRLIB}
	-mkdir ${top_srcdir}/libtmp
	-rm -f ${top_srcdir}/libtmp/*  ${top_srcdir}/$@
	for i in ${ASIRLIB}; do (cd ${top_srcdir}/libtmp; ar x ../$$i; chmod 644 *) done
	(cd ${top_srcdir}/libtmp; ${AR} cq ../$@ *.o)
	(cd ${top_srcdir}; ${RANLIB} $@)
	-rm -rf ${top_srcdir}/libtmp

#asir.o: umain.o ${ASIRLIB} ${GLIB}
#	-rm $@
#	ld -r $@ umain.o ${ASIRLIB} ${GLIB} ${LIBS} ${EXTRALIBS}

install-exec-hook:
	-if [ x${libasir_postfix} != x ] ; then \
	    (cd ${prefix}/lib ; mv libasir.a libasir${libasir_postfix}.a) ; \
	fi
	-rm -f ${prefix}/bin/ox_asir${EXEEXT}
	(cd ${prefix}/bin ; ${LN_S} asir${EXEEXT} ox_asir${EXEEXT})
	mkdir -p ${prefix}/lib/asir
	(cd ${prefix}/lib/asir ; \
	${LN_S} ../../bin/asir${EXEEXT} asir${EXEEXT} ; \
	${LN_S} asir${EXEEXT} ox_asir${EXEEXT} ; \
	${LN_S} asir${EXEEXT} ox_launch${EXEEXT} ; \
	${LN_S} asir${EXEEXT} ox_plot${EXEEXT})
