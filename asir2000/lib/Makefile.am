# $OpenXM$

rrdir   = ${prefix}/lib/asir
rr_DATA = alpi bfct bgk const cyclic defs.h dfff dmul fctrdata fctrtest \
          fff glib gr ifplot katsura mat nf num primdec primdec_lex \
          ratint robot solve sp sturm sym xm

install-data-local:
	-for i in jp eg ; do \
		uudecode help-$$i.uu ; \
		gzip -dc help-$$i.tgz | ( cd ${rrdir}; tar xf - ) ; \
		rm -f help-$$i.tgz ; \
	done
