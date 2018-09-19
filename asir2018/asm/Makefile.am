## $OpenXM$

asirdir    = ${prefix}/lib/asir
DEFS       = @DEFS@ -DASIR_LIBDIR=\"${asirdir}\"
AM_CPPFLAGS   = -I${top_srcdir}/include -I${top_srcdir}/parse -I${top_srcdir}/io \
              -I${top_srcdir}/gc/include \
              -I${prefix}/include @PARIINC@ @X_CFLAGS@
ASFLAGS    = ${DEFS} ${DEFAULT_INCLUDES} ${INCLUDES}
SOLARIS_AS = /usr/ccs/bin/as
SOLARIS_AR = /usr/ccs/bin/ar

noinst_LIBRARIES      = libasm.a
libasm_a_SOURCES      = ddM.c ddN.c
libasm_a_LIBADD       = @asm_obj@
libasm_a_DEPENDENCIES = @asm_obj@

asm4-2.o: asm4-2.s
	${SOLARIS_AS} -o $@ -P ${ASFLAGS} asm4-2.s

asm5-2.o: asm5-2.s
	${SOLARIS_AS} -o $@ -P ${ASFLAGS} asm5-2.s

edr-2.o: edr-2.s
	${SOLARIS_AS} -o $@ -P ${ASFLAGS} edr-2.s

sparc-2.o: sparc-2.s-cc
	${SOLARIS_AS} -o $@ -P ${ASFLAGS} sparc-2.s-cc

asmi_l.o: asmi_l.s
	${COMPILE} -c -o $@ asmi_l.s

asmi.o: asmi.s
	${COMPILE} -c -o $@ asmi.s
