## $OpenXM: OpenXM_contrib2/asir2000/lib/Makefile.am,v 1.8 2003/06/26 08:24:55 noro Exp $

asirdir = ${libdir}/asir
RR      = alpi bfct bgk const cyclic defs.h dfff dmul fctrdata fctrtest \
          fff glib gr ifplot katsura mat nf num primdec primdec_lex \
          ratint robot solve sp sturm sym xm primdec_mod weight

install-doc:
	-mkdir -p ${DESTDIR}${asirdir}
	-for i in jp eg ; do \
		uudecode help-$$i.uu ; \
		gzip -dc help-$$i.tgz | ( cd ${DESTDIR}${asirdir}; tar xf - ) ; \
		rm -f help-$$i.tgz ; \
	done
	-rm ${DESTDIR}${asirdir}/help
	(cd ${DESTDIR}${asirdir}; ${LN_S} help-eg help)

install-lib:
	-mkdir -p ${DESTDIR}${asirdir}
	-for i in ${RR} ; do \
		${INSTALL_DATA} $$i ${DESTDIR}${asirdir} ; \
	done
