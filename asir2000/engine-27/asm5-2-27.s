! $OpenXM: OpenXM/src/asir99/engine-27/asm5-2-27.s,v 1.1.1.1 1999/11/10 08:12:27 noro Exp $
#include "base.h"

	.seg	"text"			! [internal]
	.proc	14
	.global	dm_27
dm_27:
	umul	%o0,%o1,%o0
	rd	%y,%o1
	sll	%o1,CBSH27,%o1
	srl	%o0,BSH27,%o3
	or	%o1,%o3,%o1
	sethi	%hi(BMASK27),%o3
	add	%o3,%lo(BMASK27),%o3
	and	%o0,%o3,%o0
	retl
	st	%o1,[%o2]

	.proc	14
	.global	dmb_27
dmb_27:
	umul	%o1,%o2,%o1 
	udivcc	%o1,%o0,%o2
	umul	%o0,%o2,%o0
	subcc	%o1,%o0,%o0
	retl
	st	%o2,[%o3]

	.proc	14
	.global	dma_27
dma_27:
	umul	%o0,%o1,%o0
	rd	%y,%o1
	addcc	%o0,%o2,%o0
	addx	%g0,%o1,%o1

	sll	%o1,CBSH27,%o1
	srl	%o0,BSH27,%o2
	or	%o1,%o2,%o1
	sethi	%hi(BMASK27),%o2
	add	%o2,%lo(BMASK27),%o2
	and	%o0,%o2,%o0
	retl
	st	%o1,[%o3]

	.proc	14
	.global	dmab_27
dmab_27:
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
	.global	dsab_27
dsab_27:
	srl	%o1,CBSH27,%o4
	wr	%o4,%g0,%y
	sll	%o1,BSH27,%o1
	or	%o2,%o1,%o2

	udivcc	%o2,%o0,%o4
	umul	%o0,%o4,%o0
	subcc	%o2,%o0,%o0
	retl
	st	%o4,[%o3]

	.proc	14
	.global	dmar_27
dmar_27:
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
	.global	dqr_27
dqr_27:
	wr	%g0,%g0,%y
	udiv	%o0,%o1,%o3
	umul	%o1,%o3,%o1
	subcc	%o0,%o1,%o0

	retl
	st	%o3,[%o2]

	.seg	"data"
