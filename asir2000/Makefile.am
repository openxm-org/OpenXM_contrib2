## $OpenXM: OpenXM_contrib2/asir2000/Makefile.am,v 1.14 2003/03/20 05:07:59 ohara Exp $

AUTOMAKE_OPTIONS = foreign no-dependencies 1.5
if USE_PLOT
    PLOTDIR = plot
    PLLIB   = plot/libplot.a
    X11LIB  = @X_LIBS@ -lXaw -lXmu -lXt @X_PRE_LIBS@ @XPMLIB@ -lXext -lX11 @X_EXTRA_LIBS@ 
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

ASIRLIB  = ${BLIB} ${PLIB} ${IOLIB} ${PLLIB} ${ELIB} ${FLIB} ${ALIB}
LIBS     = @LIBS@ @PARILIB@ @MPILIB@ @LAPACKLIB@ ${X11LIB} ${FEPLIB} -lm ${EXTRALIBS}
# EXTRALIBS = @EXTRALIBS@

bin_PROGRAMS   = asir
asir_SOURCES   = parse/main.c
asir_LDADD     = libasir.a libasir-gc.a
CLEANFILES     = ${asir_LDADD}

umain.o: parse/main.c
	${COMPILE} -DUINIT main.c -o umain.o

libasir-gc.a: parse/gc_risa.c
	-if [ "${USE_GC_ZEROPAGE}" = yes \
            -a ! -f ${top_srcdir}/gc/.patch_gc_zeropage_done ]; then \
	    (cd ${top_srcdir}/gc; patch < ../gc_zeropage.diff) ; \
	    touch ${top_srcdir}/gc/.patch_gc_zeropage_done ; \
	fi
	-if [ ! -f ${top_srcdir}/gc/.configure_done ]; then \
	    if [ -n "${host_alias}" ]; then \
	        (cd ${top_srcdir}/gc; ./configure --host=${host_alias} --disable-threads) ; \
	    else \
	        (cd ${top_srcdir}/gc; ./configure --disable-threads) ; \
	    fi ; \
	    touch ${top_srcdir}/gc/.configure_done ; \
	fi
	(cd ${top_srcdir}/gc; ${MAKE})
	cp ${top_srcdir}/gc/.libs/libgc.a ${GLIB}
	(cd ${top_srcdir}/parse; ${MAKE} gc_risa.o)
	${AR} q ${GLIB} ${top_srcdir}/parse/gc_risa.o
	${RANLIB} ${GLIB}

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
	-mkdir -p ${prefix}/lib/asir
	-if [ -x ${prefix}/bin/asir ]; then \
		mv ${prefix}/bin/asir${EXEEXT} ${prefix}/lib/asir/ ; \
	fi

install-libasir: libasir.a
	mkdir -p ${prefix}/lib
	${INSTALL_DATA} libasir.a ${prefix}/lib/libasir${libasir_postfix}.a

install-openxm: asir${EXEEXT} libasir.a libasir-gc.a
	-mkdir -p ${prefix}/bin
	${INSTALL_PROGRAM} asir ${prefix}/bin/
	-mkdir -p ${prefix}/lib/asir
	(cd ${top_srcdir}/lib; ${MAKE} install-doc install-lib)
	${INSTALL_DATA} libasir.a    ${prefix}/lib/libasir${libasir_postfix}.a
	${INSTALL_DATA} libasir-gc.a ${prefix}/lib/libasir-gc.a
	-rm -f ${prefix}/bin/ox_asir${EXEEXT}
	(cd ${prefix}/bin ; ${LN_S} asir${EXEEXT} ox_asir${EXEEXT})
	-(cd ${prefix}/lib/asir; \
    rm -f asir${EXEEXT} ox_asir${EXEEXT} ox_launch${EXEEXT} ox_plot${EXEEXT})
	(cd ${prefix}/lib/asir ; \
	${LN_S} ../../bin/asir${EXEEXT} asir${EXEEXT} ; \
	${LN_S} asir${EXEEXT} ox_asir${EXEEXT} ; \
	${LN_S} asir${EXEEXT} ox_launch${EXEEXT} ; \
	${LN_S} asir${EXEEXT} ox_plot${EXEEXT})
	(cd ${top_srcdir}/include; ${MAKE} install)
	(cd ${top_srcdir}/parse; ${MAKE} install)

install-lib:
	(cd ${top_srcdir}/lib; ${MAKE} install-lib)
	-(cd ${prefix}/lib/asir; \
	rm -f ox_asir${EXEEXT} ox_launch${EXEEXT} ox_plot${EXEEXT})
	(cd ${prefix}/lib/asir ; \
	${LN_S} asir${EXEEXT} ox_asir${EXEEXT} ; \
	${LN_S} asir${EXEEXT} ox_launch${EXEEXT} ; \
	${LN_S} asir${EXEEXT} ox_plot${EXEEXT})
	-mkdir -p ${prefix}/bin
	-rm -f ${prefix}/bin/asir${EXEEXT}
	(cd ${prefix}/bin ; ${LN_S} ../lib/asir/asir${EXEEXT} asir${EXEEXT})

install-doc:
	(cd ${top_srcdir}/lib; ${MAKE} install-doc)
