## $OpenXM: OpenXM_contrib2/asir2000/Makefile.am,v 1.20 2004/06/25 13:41:33 ohara Exp $

AUTOMAKE_OPTIONS = foreign no-dependencies 1.5
if USE_PLOT
    PLOTDIR = plot
    PLLIB   = plot/libplot.a
    X11LIB  = @X_LIBS@ -lXaw -lXmu -lXt @X_PRE_LIBS@ @XPMLIB@ -lXext -lX11 @X_EXTRA_LIBS@ 
endif

SUBDIRS  = asm builtin engine fft include io lib parse ${PLOTDIR}

asirdir       = ${libdir}/asir
asir_PROGRAMS = asir
asir_SOURCES  = parse/main.c
asir_LDADD    = libasir.a libasir-gc.a
CLEANFILES    = ${asir_LDADD} asir-openxm-${VERSION}.tgz

gc            = gc6.5

DEFS     = @DEFS@ -DASIR_LIBDIR=\"${asirdir}\"
INCLUDES = -I${top_srcdir}/include -I${top_srcdir}/parse -I${top_srcdir}/io \
           -I${top_srcdir}/${gc}/include \
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

umain.o: parse/main.c
	${COMPILE} -DUINIT main.c -o umain.o

libasir-gc.a: ${gc}.tar.gz ${gc}-risa.diff parse/gc_risa.c
	-if [ ! -f ${top_srcdir}/.gc-risa_done ]; then \
	    (cd ${top_srcdir}; tar xzvf ${gc}.tar.gz; patch -p0 < ${gc}-risa.diff) ; \
	    touch ${top_srcdir}/.gc-risa_done ; \
	fi
	-if [ "${USE_GC_ZEROPAGE}" = yes \
            -a ! -f ${top_srcdir}/${gc}/.patch_gc_zeropage_done ]; then \
	    (cd ${top_srcdir}/${gc}; patch < ../gc_zeropage.diff) ; \
	    touch ${top_srcdir}/${gc}/.patch_gc_zeropage_done ; \
	fi
	-if [ "${USE_GC_INTERIX}" = yes \
            -a ! -f ${top_srcdir}/${gc}/.patch_gc_interix_done ]; then \
	    (cd ${top_srcdir}/${gc}; patch -p0 < ../gc_interix.diff) ; \
	    touch ${top_srcdir}/${gc}/.patch_gc_interix_done ; \
	fi
	-if [ "${USE_GC_SPARC64}" = yes \
            -a ! -f ${top_srcdir}/${gc}/.patch_gc_sparc64_done ]; then \
	    (cd ${top_srcdir}/${gc}; patch -p0 < ../gc_sparc64.diff) ; \
	    touch ${top_srcdir}/${gc}/.patch_gc_sparc64_done ; \
	fi
	-if [ ! -f ${top_srcdir}/${gc}/.configure_done ]; then \
	    if [ -n "${host_alias}" ]; then \
	        (cd ${top_srcdir}/${gc}; ./configure --host=${host_alias} --disable-threads) ; \
	    else \
	        (cd ${top_srcdir}/${gc}; ./configure --disable-threads) ; \
	    fi ; \
	    touch ${top_srcdir}/${gc}/.configure_done ; \
	fi
	(cd ${top_srcdir}/${gc}; ${MAKE})
	cp ${top_srcdir}/${gc}/.libs/libgc.a ${GLIB}
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

install-libasir: libasir.a
	mkdir -p ${DESTDIR}${libdir}
	${INSTALL_DATA} libasir.a ${DESTDIR}${libdir}/libasir${libasir_postfix}.a

install-libasir-gc: libasir-gc.a
	mkdir -p ${DESTDIR}${libdir}
	${INSTALL_DATA} libasir-gc.a ${DESTDIR}${libdir}/libasir-gc.a

install-openxm: asir${EXEEXT} install-libasir install-libasir-gc
	-mkdir -p ${DESTDIR}${bindir}
	${INSTALL_PROGRAM} asir ${DESTDIR}${bindir}
	-mkdir -p ${DESTDIR}${asirdir}
	(cd ${top_srcdir}/lib; ${MAKE} install-doc install-lib)
	-rm -f ${DESTDIR}${bindir}/ox_asir${EXEEXT}
	(cd ${DESTDIR}${bindir} ; ${LN_S} asir${EXEEXT} ox_asir${EXEEXT})
	-(cd ${DESTDIR}${asirdir}; \
    rm -f asir${EXEEXT} ox_asir${EXEEXT} ox_launch${EXEEXT} ox_plot${EXEEXT})
	(cd ${DESTDIR}${asirdir} ; \
	${LN_S} ../../bin/asir${EXEEXT} asir${EXEEXT} ; \
	${LN_S} asir${EXEEXT} ox_asir${EXEEXT} ; \
	${LN_S} asir${EXEEXT} ox_launch${EXEEXT} ; \
	${LN_S} asir${EXEEXT} ox_plot${EXEEXT})
	(cd ${top_srcdir}/include; ${MAKE} install)
	(cd ${top_srcdir}/parse; ${MAKE} install)

install-lib:
	(cd ${top_srcdir}/lib; ${MAKE} install-lib)
	-(cd ${DESTDIR}${asirdir}; \
	rm -f ox_asir${EXEEXT} ox_launch${EXEEXT} ox_plot${EXEEXT})
	(cd ${DESTDIR}${asirdir} ; \
	${LN_S} asir${EXEEXT} ox_asir${EXEEXT} ; \
	${LN_S} asir${EXEEXT} ox_launch${EXEEXT} ; \
	${LN_S} asir${EXEEXT} ox_plot${EXEEXT})
	-mkdir -p ${DESTDIR}${bindir}
	-rm -f ${DESTDIR}${bindir}/asir${EXEEXT}
	(cd ${DESTDIR}${bindir} ; ${LN_S} ../lib/asir/asir${EXEEXT} asir${EXEEXT})

install-doc:
	(cd ${top_srcdir}/lib; ${MAKE} install-doc)

tarball:
	-rm -rf tmp
	-mkdir tmp
	DESTDIR=`cd tmp; pwd` ${MAKE} install-openxm
	(cd tmp${prefix}; tar cf - * ) | gzip -c > asir-openxm-${VERSION}.tgz
	-rm -rf tmp
