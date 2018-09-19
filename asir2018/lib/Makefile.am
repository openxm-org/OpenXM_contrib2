## $OpenXM$

asirdir = ${libdir}/asir
RR      = alpi bfct bgk const cyclic defs.h dfff dmul dmul102 fctrdata fctrtest \
          fff glib gr ifplot katsura mat nf num primdec primdec_lex \
          ratint robot solve sp sturm sym xm primdec_mod weight

install-doc:
	-mkdir -p ${DESTDIR}${asirdir}
	-for i in ja en ; do \
		uudecode help-$$i.uu ; \
		gzip -dc help-$$i.tgz | ( cd ${DESTDIR}${asirdir}; tar xf - ) ; \
		rm -f help-$$i.tgz ; \
	done
	-rm ${DESTDIR}${asirdir}/help
	(cd ${DESTDIR}${asirdir}; ${LN_S} help-ja help-jp)
	(cd ${DESTDIR}${asirdir}; ${LN_S} help-en help-eg)
	(cd ${DESTDIR}${asirdir}; ${LN_S} help-en help)

install-lib:
	-mkdir -p ${DESTDIR}${asirdir}
	-for i in ${RR} ; do \
		${INSTALL_DATA} $$i ${DESTDIR}${asirdir} ; \
	done
