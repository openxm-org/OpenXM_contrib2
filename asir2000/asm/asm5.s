! $OpenXM: OpenXM/src/asir99/asm/asm5.s,v 1.1.1.1 1999/11/10 08:12:25 noro Exp $
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

	.proc	14
	.global	_dmb
_dmb:
	umul	%o1,%o2,%o1 
	udivcc	%o1,%o0,%o2
	umul	%o0,%o2,%o0
	subcc	%o1,%o0,%o0
	retl
	st	%o2,[%o3]

	.proc	14
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
	.global	_dmab
_dmab:
	umul	%o1,%o2,%o1 
	rd	%y,%o2
	addcc	%o1,%o3,%o1
	addx	%g0,%o2,%o2
	wr	%o2,%g0,%y

	udivcc	%o1,%o0,%o2
	umul	%o0,%o2,%o0
	subcc	%o1,%o0,%o0
	retl
	st	%o2,[%o4]

	.proc	14
	.global	_dsab
_dsab:
	srl	%o1,CBSH,%o4
	wr	%o4,%g0,%y
	sll	%o1,BSH,%o1
	or	%o2,%o1,%o2

	udivcc	%o2,%o0,%o4
	umul	%o0,%o4,%o0
	subcc	%o2,%o0,%o0
	retl
	st	%o4,[%o3]

	.proc	14
	.global	_dmar
_dmar:
	umul	%o0,%o1,%o0 
	rd	%y,%o1
	addcc	%o0,%o2,%o0
	addx	%g0,%o1,%o1
	wr	%o1,%g0,%y
	udivcc	%o0,%o3,%o1
	umul	%o3,%o1,%o1
	retl
	subcc	%o0,%o1,%o0

	.proc	14
	.global	_dqr
_dqr:
	wr	%g0,%g0,%y
	udiv	%o0,%o1,%o3
	umul	%o1,%o3,%o1
	subcc	%o0,%o1,%o0

	retl
	st	%o3,[%o2]

	.seg	"data"
