## $OpenXM: OpenXM_contrib2/asir2000/lib/Makefile.am,v 1.4 2003/03/12 08:22:53 ohara Exp $

RR      = alpi bfct bgk const cyclic defs.h dfff dmul fctrdata fctrtest \
          fff glib gr ifplot katsura mat nf num primdec primdec_lex \
          ratint robot solve sp sturm sym xm

install-doc:
	-mkdir -p ${DESTDIR}${prefix}/lib/asir
	-for i in jp eg ; do \
		uudecode help-$$i.uu ; \
		gzip -dc help-$$i.tgz | ( cd ${DESTDIR}${prefix}/lib/asir; tar xf - ) ; \
		rm -f help-$$i.tgz ; \
	done
	-rm ${DESTDIR}${prefix}/lib/asir/help
	(cd ${DESTDIR}${prefix}/lib/asir; ${LN_S} help-eg help)

install-lib:
	-mkdir -p ${DESTDIR}${prefix}/lib/asir
	${INSTALL_DATA} ${RR} ${DESTDIR}${prefix}/lib/asir/
