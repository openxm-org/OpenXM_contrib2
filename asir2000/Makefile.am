## $OpenXM: OpenXM_contrib2/asir2000/Makefile.am,v 1.40 2013/12/20 02:02:23 noro Exp $

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
asir_LDADD    = libasir.a @ASIR_GCLIB@
CLEANFILES    = ${asir_LDADD} asir-openxm-${VERSION}.tgz

GC_TAR_GZ     = gc-7.2b.tar.gz
GC_PATCH      = gc-7.diff gc-7.0-risa.diff
#GC_TAR_GZ     = ${GC}.tar.gz
#GC_PATCH      = gc6.diff gc6-risa.diff
GC_MASTER_SITE=ftp://ftp.math.kobe-u.ac.jp/pub/OpenXM/misc/
OX_FETCH      = ${bindir}/oxfetch.sh

DEFS     = @DEFS@ -DASIR_LIBDIR=\"${asirdir}\"
INCLUDES = -I${top_srcdir}/include -I${top_srcdir}/parse -I${top_srcdir}/io \
           -I${prefix}/include @GCINC@ @PARIINC@ @X_CFLAGS@

GLIB  = @ASIR_GCLIB@
ALIB  = asm/libasm.a
BLIB  = builtin/libfunc.a
ELIB  = engine/libca.a
FLIB  = fft/libdft.a
IOLIB = io/libio.a
PLIB  = parse/libparse.a
GMPLIB = ${libdir}/libgmp.a

ASIRLIB  = ${BLIB} ${PLIB} ${IOLIB} ${PLLIB} ${ELIB} ${FLIB} ${ALIB}
LIBS     = @LIBS@ @GCLIB@ @PARILIB@ @LAPACKLIB@ ${X11LIB} ${FEPLIB} ${GMPLIB} -lm ${EXTRALIBS}
# EXTRALIBS = @EXTRALIBS@

all-recursive: ${GLIB}

umain.o: parse/main.c
	${COMPILE} -DUINIT main.c -o umain.o

libasir-gc.a: ${GC_PATCH}
	-if [ ! -f ${top_srcdir}/.gc_fetch_done ]; then \
		if [ ! -f ${GC_DISTDIR}/${GC_TAR_GZ} ]; then \
			${OX_FETCH} ${GC_MASTER_SITE}${GC_TAR_GZ} ${GC_DISTDIR}; \
		fi ; \
		touch ${top_srcdir}/.gc_fetch_done ; \
	fi
	-if [ ! -f ${top_srcdir}/.gc_risa_done ]; then \
	    (cd ${top_srcdir}; tar xzvf ${GC_DISTDIR}/${GC_TAR_GZ}) ; \
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

libasir.a: ${ASIRLIB} ${GLIB} parse/gc_risa.c
	-mkdir ${top_srcdir}/libtmp
	-rm -f ${top_srcdir}/libtmp/*  ${top_srcdir}/$@
	for i in ${ASIRLIB}; do (cd ${top_srcdir}/libtmp; ar x ../$$i; chmod 644 *) done
	(cd ${top_srcdir}/parse; ${MAKE} gc_risa.o; cp gc_risa.o ../libtmp)
	(cd ${top_srcdir}/libtmp; ${AR} cq ../$@ *.o)
	(cd ${top_srcdir}; ${RANLIB} $@)
	-rm -rf ${top_srcdir}/libtmp

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

clean-gc:
	-rm -rf ${top_srcdir}/.gc_*_done ${top_srcdir}/${GC}
	-rm -rf ${top_srcdir}/autom4te.cache

distclean-recursive: clean-gc
