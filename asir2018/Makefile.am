## $OpenXM: OpenXM_contrib2/asir2018/Makefile.am,v 1.1 2018/09/19 05:45:05 noro Exp $

AUTOMAKE_OPTIONS = foreign no-dependencies 1.5
if USE_PLOT
    PLOTDIR = plot
    PLLIB   = plot/libplot.a
    X11LIB  = @X_LIBS@ -lXaw -lXmu -lXt @X_PRE_LIBS@ @XPMLIB@ -lXext -lX11 @X_EXTRA_LIBS@ 
endif

SUBDIRS  = asm builtin engine fft include io lib parse ${PLOTDIR}

Asirdir       = ${libdir}/Asir
Asir_PROGRAMS = Asir
Asir_SOURCES  = parse/main.c
Asir_LDADD    = libAsir.a @ASIR_GCLIB@
CLEANFILES    = ${Asir_LDADD} Asir-openxm-${VERSION}.tgz

GC_TAR_GZ     = gc-7.4.2.tar.gz
GC_PATCH      = gc-7.diff gc-7.0-risa.diff
LIB_ATOMIC     = libatomic_ops-7.4.0
LIB_ATOMIC_TAR_GZ     = ${LIB_ATOMIC}.tar.gz
#GC_TAR_GZ     = ${GC}.tar.gz
#GC_PATCH      = gc6.diff gc6-risa.diff
GC_MASTER_SITE=http://www.math.kobe-u.ac.jp/pub/OpenXM/misc/
OX_FETCH      = ${bindir}/oxfetch.sh

DEFS     = @DEFS@ -DASIR_LIBDIR=\"${Asirdir}\"
AM_CPPFLAGS = -I${top_srcdir}/include -I${top_srcdir}/parse -I${top_srcdir}/io \
           -I${prefix}/include @GCINC@ @PARIINC@ @X_CFLAGS@

GLIB  = @ASIR_GCLIB@
ALIB  = asm/libasm.a
BLIB  = builtin/libfunc.a
ELIB  = engine/libca.a
FLIB  = fft/libdft.a
IOLIB = io/libio.a
PLIB  = parse/libparse.a
GMPLIB = -L${libdir} @GMPLIB@

ASIRLIB  = ${BLIB} ${PLIB} ${IOLIB} ${PLLIB} ${ELIB} ${FLIB} ${ALIB}
LIBS     = @LIBS@ @GCLIB@ @PARILIB@ @LAPACKLIB@ ${X11LIB} ${FEPLIB} ${GMPLIB} -lm @WSLIB@ ${EXTRALIBS}
# EXTRALIBS = @EXTRALIBS@

all-recursive: ${GLIB}

umain.o: parse/main.c
	${COMPILE} -DUINIT main.c -o umain.o

libAsir-gc.a: ${GC_PATCH}
	-if [ ! -f ${top_srcdir}/.gc_fetch_done ]; then \
		if [ ! -f ${GC_DISTDIR}/${GC_TAR_GZ} ]; then \
			${OX_FETCH} ${GC_MASTER_SITE}${GC_TAR_GZ} ${GC_DISTDIR}; \
			${OX_FETCH} ${GC_MASTER_SITE}${LIB_ATOMIC_TAR_GZ} ${GC_DISTDIR}; \
		fi ; \
		touch ${top_srcdir}/.gc_fetch_done ; \
	fi
	-if [ ! -f ${top_srcdir}/.gc_risa_done ]; then \
	    (cd ${top_srcdir}; tar xzvf ${GC_DISTDIR}/${GC_TAR_GZ}) ; \
	    (cd ${top_srcdir}/${GC}; tar xzvf ${GC_DISTDIR}/${LIB_ATOMIC_TAR_GZ}; mv ${LIB_ATOMIC} libatomic_ops) ; \
		for i in ${GC_PATCH} ; do \
	    (cd ${top_srcdir}/${GC}; patch -p1 < ../$$i) ; \
		done ; \
	    touch ${top_srcdir}/.gc_risa_done ; \
	fi
	-if [ "${USE_GC_SPARC64}" = yes \
            -a ! -f ${top_srcdir}/${GC}/.patch_gc_sparc64_done ]; then \
	    (cd ${top_srcdir}/${GC}; patch -p0 < ../gc_sparc64.diff) ; \
	    touch ${top_srcdir}/${GC}/.patch_gc_sparc64_done ; \
	fi
	-if [ ! -f ${top_srcdir}/${GC}/.configure_done ]; then \
	    (cd ${top_srcdir}/${GC}; ./configure ${GC_CONFIGURE_ARGS}) ; \
	    touch ${top_srcdir}/${GC}/.configure_done ; \
	fi
	(cd ${top_srcdir}/${GC}; ${MAKE})
	cp ${top_srcdir}/${GC}/.libs/libgc.a $@

libAsir.a: ${ASIRLIB} ${GLIB} parse/gc_risa.c
	-mkdir ${top_srcdir}/libtmp
	-rm -f ${top_srcdir}/libtmp/*  ${top_srcdir}/$@
	for i in ${ASIRLIB}; do (cd ${top_srcdir}/libtmp; ar x ../$$i; chmod 644 *) done
	(cd ${top_srcdir}/parse; ${MAKE} gc_risa.o; cp gc_risa.o ../libtmp)
	(cd ${top_srcdir}/libtmp; ${AR} cq ../$@ *.o)
	(cd ${top_srcdir}; ${RANLIB} $@)
	-rm -rf ${top_srcdir}/libtmp

install-libAsir: libAsir.a
	mkdir -p ${DESTDIR}${libdir}
	${INSTALL_DATA} libAsir.a ${DESTDIR}${libdir}/libAsir${libAsir_postfix}.a

install-libAsir-gc: libAsir-gc.a
	mkdir -p ${DESTDIR}${libdir}
	${INSTALL_DATA} libAsir-gc.a ${DESTDIR}${libdir}/libAsir-gc.a

install-openxm: Asir${EXEEXT} install-libAsir install-libAsir-gc
	-mkdir -p ${DESTDIR}${bindir}
	${INSTALL_PROGRAM} Asir ${DESTDIR}${bindir}
	-mkdir -p ${DESTDIR}${Asirdir}
	(cd ${top_srcdir}/lib; ${MAKE} install-doc install-lib)
	-rm -f ${DESTDIR}${bindir}/ox_Asir${EXEEXT}
	(cd ${DESTDIR}${bindir} ; ${LN_S} Asir${EXEEXT} ox_Asir${EXEEXT})
	-(cd ${DESTDIR}${Asirdir}; \
    rm -f Asir${EXEEXT} ox_Asir${EXEEXT} ox_Launch${EXEEXT} ox_Plot${EXEEXT})
	(cd ${DESTDIR}${Asirdir} ; \
	${LN_S} ../../bin/Asir${EXEEXT} Asir${EXEEXT} ; \
	${LN_S} Asir${EXEEXT} ox_Asir${EXEEXT} ; \
	${LN_S} Asir${EXEEXT} ox_Launch${EXEEXT} ; \
	${LN_S} Asir${EXEEXT} ox_Plot${EXEEXT})
	(cd ${top_srcdir}/include; ${MAKE} install)
	(cd ${top_srcdir}/parse; ${MAKE} install)

install-lib:
	(cd ${top_srcdir}/lib; ${MAKE} install-lib)
	-(cd ${DESTDIR}${Asirdir}; \
	rm -f ox_Asir${EXEEXT} ox_Launch${EXEEXT} ox_Plot${EXEEXT})
	(cd ${DESTDIR}${Asirdir} ; \
	${LN_S} Asir${EXEEXT} ox_Asir${EXEEXT} ; \
	${LN_S} Asir${EXEEXT} ox_Launch${EXEEXT} ; \
	${LN_S} Asir${EXEEXT} ox_Plot${EXEEXT})
	-mkdir -p ${DESTDIR}${bindir}
	-rm -f ${DESTDIR}${bindir}/Asir${EXEEXT}
	(cd ${DESTDIR}${bindir} ; ${LN_S} ../lib/Asir/Asir${EXEEXT} Asir${EXEEXT})

install-doc:
	(cd ${top_srcdir}/lib; ${MAKE} install-doc)

tarball:
	-rm -rf tmp
	-mkdir tmp
	DESTDIR=`cd tmp; pwd` ${MAKE} install-openxm
	(cd tmp${prefix}; tar cf - * ) | gzip -c > Asir-openxm-${VERSION}.tgz
	-rm -rf tmp

clean-gc:
	-rm -rf ${top_srcdir}/.gc_*_done ${top_srcdir}/${GC}
	-rm -rf ${top_srcdir}/autom4te.cache

distclean-recursive: clean-gc
