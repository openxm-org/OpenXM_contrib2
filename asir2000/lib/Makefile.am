# $OpenXM: OpenXM_contrib2/asir2000/lib/Makefile.am,v 1.1 2003/02/23 03:05:39 ohara Exp $

RR      = alpi bfct bgk const cyclic defs.h dfff dmul fctrdata fctrtest \
          fff glib gr ifplot katsura mat nf num primdec primdec_lex \
          ratint robot solve sp sturm sym xm

install-doc:
	-mkdir -p ${prefix}/lib/asir
	-for i in jp eg ; do \
		uudecode help-$$i.uu ; \
		gzip -dc help-$$i.tgz | ( cd ${prefix}/lib/asir; tar xf - ) ; \
		rm -f help-$$i.tgz ; \
	done
	(cd ${prefix}/lib/asir; ${LN_S} help-eg help)

install-lib:
	-mkdir -p ${prefix}/lib/asir
	${INSTALL_DATA} ${RR} ${prefix}/lib/asir/
