! $OpenXM: OpenXM/src/asir99/asm/asm45.s,v 1.1.1.1 1999/11/10 08:12:25 noro Exp $
#include "base.h"

	.seg	"text"			! [internal]
	.proc	14
	.global	_dm
_dm:
	umul	%o0,%o1,%o0
	rd	%y,%o1
	sll	%o1,CBSH,%o1
	srl	%o0,BSH,%o3
	or	%o1,%o3,%o1
	sethi	%hi(BMASK),%o3
	add	%o3,%lo(BMASK),%o3
	and	%o0,%o3,%o0
	retl
	st	%o1,[%o2]

	.global	_dma
_dma:
	umul	%o0,%o1,%o0
	rd	%y,%o1
	addcc	%o0,%o2,%o0
	addx	%g0,%o1,%o1

	sll	%o1,CBSH,%o1
	srl	%o0,BSH,%o2
	or	%o1,%o2,%o1
	sethi	%hi(BMASK),%o2
	add	%o2,%lo(BMASK),%o2
	and	%o0,%o2,%o0
	retl
	st	%o1,[%o3]

	.proc	14
	.global	_dmb
_dmb:
	save	%sp,-104,%sp
	umul	%i1,%i2,%o0
	rd	%y,%o1

	call	_edr,3
	mov	%i0,%o2

	st	%o1,[%i3]
	ret
	restore	%g0,%o0,%o0

	.proc	14
	.global	_dmab
_dmab:
	save	%sp,-104,%sp
	umul	%i1,%i2,%o0
	rd	%y,%o1

	addcc	%o0,%i3,%o0
	addx	%g0,%o1,%o1

	call	_edr,3
	mov	%i0,%o2

	st	%o1,[%i4]
	ret
	restore	%g0,%o0,%o0

	.proc	14
	.global	_dmar
_dmar:
	save	%sp,-104,%sp
	umul	%i0,%i1,%o0
	rd	%y,%o1

	addcc	%o0,%i2,%o0
	addx	%g0,%o1,%o1

	call	_edr,3
	mov	%i3,%o2

	ret
	restore	%g0,%o0,%o0

	.proc	14
	.global	_dsab
_dsab:
	save	%sp,-104,%sp
	srl	%i1,CBSH,%o1
	sll	%i1,BSH,%o0
	or	%i2,%o0,%o0

	call	_edr,3
	mov	%i0,%o2

	st	%o1,[%i3]
	ret
	restore	%g0,%o0,%o0

	.proc	14
	.global	_dqr
_dqr:
	mov %o7,%g5 !save %o7
	call .udr,2
	mov %o2,%g6 !save %o2

	st %o1,[%g6]
	mov %g5,%o7

	retl
	nop
	.seg	"data"
