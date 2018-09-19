! $OpenXM$
#include "base.h"

	.seg	"text"			! [internal]
	.proc	14
	.global	dmb
dmb:
	save	%sp,-104,%sp
	umul	%i1,%i2,%o0
	rd	%y,%o1

	call	edr,3
	mov	%i0,%o2

	st	%o1,[%i3]
	ret
	restore	%g0,%o0,%o0

	.proc	14
	.global	dmab
dmab:
	save	%sp,-104,%sp
	umul	%i1,%i2,%o0
	rd	%y,%o1

	addcc	%o0,%i3,%o0
	addx	%g0,%o1,%o1

	call	edr,3
	mov	%i0,%o2

	st	%o1,[%i4]
	ret
	restore	%g0,%o0,%o0

	.proc	14
	.global	dmar
dmar:
	save	%sp,-104,%sp
	umul	%i0,%i1,%o0
	rd	%y,%o1

	addcc	%o0,%i2,%o0
	addx	%g0,%o1,%o1

	call	edr,3
	mov	%i3,%o2

	ret
	restore	%g0,%o0,%o0

	.proc	14
	.global	dsar
dsar:
	save	%sp,-104,%sp
	mov	%i1,%o1
	mov	%i2,%o0

	call	edr,3
	mov	%i0,%o2

	ret
	restore	%g0,%o0,%o0

	.seg	"data"
