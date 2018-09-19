! $OpenXM$
#include "base.h"

	.seg	"text"			! [internal]
	.proc	14
	.global	dmb
dmb:
	umul	%o1,%o2,%o1 
	udivcc	%o1,%o0,%o2
	umul	%o0,%o2,%o0
	subcc	%o1,%o0,%o0
	retl
	st	%o2,[%o3]

	.proc	14
	.global	dmab
dmab:
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
	.global	dsar
dsar:
	wr	%o1,%g0,%y
	udivcc	%o2,%o0,%o4
	umul	%o0,%o4,%o0
	retl
	subcc	%o2,%o0,%o0

	.proc	14
	.global	dmar
dmar:
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
	.global	dqr
dqr:
	wr	%g0,%g0,%y
	udiv	%o0,%o1,%o3
	umul	%o1,%o3,%o1
	subcc	%o0,%o1,%o0

	retl
	st	%o3,[%o2]

	.seg	"data"
