










	.proc	4
	.global	edr_27
edr_27:
	tst %o1
	be,a .udr_27
	mov	%o2,%o1

	mov	%o0,%o3
	mov	%o1,%o0
	clr	%o1
	tst %o0

	sll	%o1,4,%o1
	
	bl	L.0.1.16
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.0.2.17
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.0.3.19
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.0.4.23
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.0
	add	%o1,15,%o1
L.0.4.23:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.0
	add	%o1,13,%o1
	

L.0.3.19:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.0.4.21
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.0
	add	%o1,11,%o1
L.0.4.21:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.0
	add	%o1,9,%o1
	


L.0.2.17:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.0.3.17
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.0.4.19
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.0
	add	%o1,7,%o1
L.0.4.19:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.0
	add	%o1,5,%o1
	

L.0.3.17:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.0.4.17
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.0
	add	%o1,3,%o1
L.0.4.17:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.0
	add	%o1,1,%o1
	



L.0.1.16:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.0.2.15
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.0.3.15
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.0.4.15
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.0
	add	%o1,-1,%o1
L.0.4.15:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.0
	add	%o1,-3,%o1
	

L.0.3.15:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.0.4.13
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.0
	add	%o1,-5,%o1
L.0.4.13:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.0
	add	%o1,-7,%o1
	


L.0.2.15:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.0.3.13
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.0.4.11
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.0
	add	%o1,-9,%o1
L.0.4.11:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.0
	add	%o1,-11,%o1
	

L.0.3.13:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.0.4.9
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.0
	add	%o1,-13,%o1
L.0.4.9:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.0
	add	%o1,-15,%o1
	




NEXT.0:
	sll	%o1,4,%o1
	
	bl	L.1.1.16
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.1.2.17
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.1.3.19
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.1.4.23
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.1
	add	%o1,15,%o1
L.1.4.23:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.1
	add	%o1,13,%o1
	

L.1.3.19:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.1.4.21
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.1
	add	%o1,11,%o1
L.1.4.21:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.1
	add	%o1,9,%o1
	


L.1.2.17:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.1.3.17
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.1.4.19
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.1
	add	%o1,7,%o1
L.1.4.19:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.1
	add	%o1,5,%o1
	

L.1.3.17:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.1.4.17
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.1
	add	%o1,3,%o1
L.1.4.17:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.1
	add	%o1,1,%o1
	



L.1.1.16:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.1.2.15
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.1.3.15
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.1.4.15
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.1
	add	%o1,-1,%o1
L.1.4.15:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.1
	add	%o1,-3,%o1
	

L.1.3.15:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.1.4.13
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.1
	add	%o1,-5,%o1
L.1.4.13:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.1
	add	%o1,-7,%o1
	


L.1.2.15:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.1.3.13
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.1.4.11
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.1
	add	%o1,-9,%o1
L.1.4.11:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.1
	add	%o1,-11,%o1
	

L.1.3.13:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.1.4.9
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.1
	add	%o1,-13,%o1
L.1.4.9:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.1
	add	%o1,-15,%o1
	




NEXT.1:
	sll	%o1,4,%o1
	
	bl	L.2.1.16
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.2.2.17
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.2.3.19
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.2.4.23
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.2
	add	%o1,15,%o1
L.2.4.23:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.2
	add	%o1,13,%o1
	

L.2.3.19:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.2.4.21
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.2
	add	%o1,11,%o1
L.2.4.21:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.2
	add	%o1,9,%o1
	


L.2.2.17:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.2.3.17
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.2.4.19
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.2
	add	%o1,7,%o1
L.2.4.19:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.2
	add	%o1,5,%o1
	

L.2.3.17:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.2.4.17
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.2
	add	%o1,3,%o1
L.2.4.17:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.2
	add	%o1,1,%o1
	



L.2.1.16:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.2.2.15
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.2.3.15
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.2.4.15
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.2
	add	%o1,-1,%o1
L.2.4.15:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.2
	add	%o1,-3,%o1
	

L.2.3.15:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.2.4.13
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.2
	add	%o1,-5,%o1
L.2.4.13:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.2
	add	%o1,-7,%o1
	


L.2.2.15:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.2.3.13
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.2.4.11
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.2
	add	%o1,-9,%o1
L.2.4.11:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.2
	add	%o1,-11,%o1
	

L.2.3.13:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.2.4.9
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.2
	add	%o1,-13,%o1
L.2.4.9:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.2
	add	%o1,-15,%o1
	




NEXT.2:
	sll	%o1,4,%o1
	
	bl	L.3.1.16
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.3.2.17
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.3.3.19
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.3.4.23
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.3
	add	%o1,15,%o1
L.3.4.23:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.3
	add	%o1,13,%o1
	

L.3.3.19:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.3.4.21
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.3
	add	%o1,11,%o1
L.3.4.21:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.3
	add	%o1,9,%o1
	


L.3.2.17:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.3.3.17
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.3.4.19
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.3
	add	%o1,7,%o1
L.3.4.19:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.3
	add	%o1,5,%o1
	

L.3.3.17:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.3.4.17
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.3
	add	%o1,3,%o1
L.3.4.17:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.3
	add	%o1,1,%o1
	



L.3.1.16:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.3.2.15
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.3.3.15
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.3.4.15
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.3
	add	%o1,-1,%o1
L.3.4.15:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.3
	add	%o1,-3,%o1
	

L.3.3.15:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.3.4.13
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.3
	add	%o1,-5,%o1
L.3.4.13:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.3
	add	%o1,-7,%o1
	


L.3.2.15:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.3.3.13
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.3.4.11
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.3
	add	%o1,-9,%o1
L.3.4.11:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.3
	add	%o1,-11,%o1
	

L.3.3.13:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.3.4.9
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.3
	add	%o1,-13,%o1
L.3.4.9:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.3
	add	%o1,-15,%o1
	




NEXT.3:
	sll	%o1,4,%o1
	
	bl	L.4.1.16
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.4.2.17
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.4.3.19
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.4.4.23
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.4
	add	%o1,15,%o1
L.4.4.23:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.4
	add	%o1,13,%o1
	

L.4.3.19:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.4.4.21
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.4
	add	%o1,11,%o1
L.4.4.21:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.4
	add	%o1,9,%o1
	


L.4.2.17:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.4.3.17
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.4.4.19
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.4
	add	%o1,7,%o1
L.4.4.19:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.4
	add	%o1,5,%o1
	

L.4.3.17:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.4.4.17
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.4
	add	%o1,3,%o1
L.4.4.17:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.4
	add	%o1,1,%o1
	



L.4.1.16:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.4.2.15
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.4.3.15
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.4.4.15
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.4
	add	%o1,-1,%o1
L.4.4.15:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.4
	add	%o1,-3,%o1
	

L.4.3.15:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.4.4.13
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.4
	add	%o1,-5,%o1
L.4.4.13:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.4
	add	%o1,-7,%o1
	


L.4.2.15:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.4.3.13
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.4.4.11
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.4
	add	%o1,-9,%o1
L.4.4.11:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.4
	add	%o1,-11,%o1
	

L.4.3.13:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.4.4.9
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.4
	add	%o1,-13,%o1
L.4.4.9:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.4
	add	%o1,-15,%o1
	




NEXT.4:
	sll	%o1,4,%o1
	
	bl	L.5.1.16
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.5.2.17
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.5.3.19
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.5.4.23
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.5
	add	%o1,15,%o1
L.5.4.23:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.5
	add	%o1,13,%o1
	

L.5.3.19:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.5.4.21
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.5
	add	%o1,11,%o1
L.5.4.21:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.5
	add	%o1,9,%o1
	


L.5.2.17:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.5.3.17
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.5.4.19
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.5
	add	%o1,7,%o1
L.5.4.19:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.5
	add	%o1,5,%o1
	

L.5.3.17:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.5.4.17
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.5
	add	%o1,3,%o1
L.5.4.17:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.5
	add	%o1,1,%o1
	



L.5.1.16:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.5.2.15
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.5.3.15
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.5.4.15
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.5
	add	%o1,-1,%o1
L.5.4.15:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.5
	add	%o1,-3,%o1
	

L.5.3.15:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.5.4.13
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.5
	add	%o1,-5,%o1
L.5.4.13:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.5
	add	%o1,-7,%o1
	


L.5.2.15:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.5.3.13
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.5.4.11
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.5
	add	%o1,-9,%o1
L.5.4.11:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.5
	add	%o1,-11,%o1
	

L.5.3.13:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.5.4.9
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.5
	add	%o1,-13,%o1
L.5.4.9:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.5
	add	%o1,-15,%o1
	




NEXT.5:
	sll	%o1,4,%o1
	
	bl	L.6.1.16
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.6.2.17
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.6.3.19
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.6.4.23
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.6
	add	%o1,15,%o1
L.6.4.23:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.6
	add	%o1,13,%o1
	

L.6.3.19:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.6.4.21
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.6
	add	%o1,11,%o1
L.6.4.21:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.6
	add	%o1,9,%o1
	


L.6.2.17:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.6.3.17
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.6.4.19
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.6
	add	%o1,7,%o1
L.6.4.19:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.6
	add	%o1,5,%o1
	

L.6.3.17:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.6.4.17
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.6
	add	%o1,3,%o1
L.6.4.17:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.6
	add	%o1,1,%o1
	



L.6.1.16:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.6.2.15
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.6.3.15
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.6.4.15
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.6
	add	%o1,-1,%o1
L.6.4.15:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.6
	add	%o1,-3,%o1
	

L.6.3.15:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.6.4.13
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.6
	add	%o1,-5,%o1
L.6.4.13:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.6
	add	%o1,-7,%o1
	


L.6.2.15:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.6.3.13
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.6.4.11
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.6
	add	%o1,-9,%o1
L.6.4.11:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.6
	add	%o1,-11,%o1
	

L.6.3.13:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.6.4.9
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.6
	add	%o1,-13,%o1
L.6.4.9:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.6
	add	%o1,-15,%o1
	




NEXT.6:
	sll	%o1,4,%o1
	
	bl	L.7.1.16
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.7.2.17
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.7.3.19
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.7.4.23
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.7
	add	%o1,15,%o1
L.7.4.23:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.7
	add	%o1,13,%o1
	

L.7.3.19:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.7.4.21
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.7
	add	%o1,11,%o1
L.7.4.21:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.7
	add	%o1,9,%o1
	


L.7.2.17:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.7.3.17
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.7.4.19
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.7
	add	%o1,7,%o1
L.7.4.19:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.7
	add	%o1,5,%o1
	

L.7.3.17:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.7.4.17
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.7
	add	%o1,3,%o1
L.7.4.17:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.7
	add	%o1,1,%o1
	



L.7.1.16:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.7.2.15
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.7.3.15
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.7.4.15
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.7
	add	%o1,-1,%o1
L.7.4.15:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.7
	add	%o1,-3,%o1
	

L.7.3.15:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.7.4.13
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.7
	add	%o1,-5,%o1
L.7.4.13:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.7
	add	%o1,-7,%o1
	


L.7.2.15:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.7.3.13
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	
	bl	L.7.4.11
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.7
	add	%o1,-9,%o1
L.7.4.11:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.7
	add	%o1,-11,%o1
	

L.7.3.13:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	
	bl	L.7.4.9
	addcc %o3,%o3,%o3
	addx %o0,%o0,%o0
	subcc	%o0,%o2,%o0
	b	NEXT.7
	add	%o1,-13,%o1
L.7.4.9:
	addx %o0,%o0,%o0
	addcc	%o0,%o2,%o0
	b	NEXT.7
	add	%o1,-15,%o1
	




NEXT.7:
	










END:
	tst %o0
	bl,a	A1
	add %o0,%o2,%o0

	retl
	nop

A1:
	retl
	sub	%o1,1,%o1

	.proc	14
	.global	.udr_27
.udr_27:
	clr	%g1
	orcc	%o1, %g0, %o5
	bne	L0x38
	mov	%o0, %o3
	ba	L0x2a0
	nop
L0x38:	cmp	%o3, %o5
	blu	L0x28c
	clr	%o2
	sethi	%hi(0x8000000),	%g2
	cmp	%o3, %g2
	blu	L0xe8
	clr	%o4
L0x54:	cmp	%o5, %g2
L0x58:	bgeu	L0x8c
	mov	0x1, %g3
L0x60:	sll	%o5, 0x4, %o5
L0x64:	ba	L0x54
	add	%o4, 0x1, %o4
L0x6c:	addcc	%o5, %o5, %o5
L0x70:	bgeu	L0x8c
	add	%g3, 0x1, %g3
L0x78:	sll	%g2, 0x4, %g2
L0x7c:	srl	%o5, 0x1, %o5
L0x80:	add	%o5, %g2, %o5
L0x84:	ba	L0xa0
	sub	%g3, 0x1, %g3
L0x8c:	cmp	%o5, %o3
L0x90:	blu	L0x6c
	nop
L0x98:	be	L0xa0
	nop
L0xa0:	subcc	%g3, 0x1, %g3
L0xa4:	bl	L0x278
	nop
L0xac:	sub	%o3, %o5, %o3
L0xb0:	mov	0x1, %o2
L0xb4:	ba,a	L0xd8
L0xb8:	sll	%o2, 0x1, %o2
L0xbc:	bl	L0xd0
	srl	%o5, 0x1, %o5
L0xc4:	sub	%o3, %o5, %o3
L0xc8:	ba	L0xd8
	add	%o2, 0x1, %o2
L0xd0:	add	%o3, %o5, %o3
L0xd4:	sub	%o2, 0x1, %o2
L0xd8:	subcc	%g3, 0x1, %g3
L0xdc:	bge	L0xb8
	orcc	%g0, %o3, %g0
L0xe4:	ba,a	L0x278
L0xe8:	sll	%o5, 0x4, %o5
L0xec:	cmp	%o5, %o3
L0xf0:	bleu	L0xe8
	addcc	%o4, 0x1, %o4
L0xf8:	be	L0x28c
	sub	%o4, 0x1, %o4
L0x100:	orcc	%g0, %o3, %g0
L0x104:	sll	%o2, 0x4, %o2
L0x108:	bl	L0x1c4
	srl	%o5, 0x1, %o5
L0x110:	subcc	%o3, %o5, %o3
L0x114:	bl	L0x170
	srl	%o5, 0x1, %o5
L0x11c:	subcc	%o3, %o5, %o3
L0x120:	bl	L0x14c
	srl	%o5, 0x1, %o5
L0x128:	subcc	%o3, %o5, %o3
L0x12c:	bl	L0x140
	srl	%o5, 0x1, %o5
L0x134:	subcc	%o3, %o5, %o3
L0x138:	ba	L0x278
	add	%o2, 0xf, %o2
L0x140:	addcc	%o3, %o5, %o3
L0x144:	ba	L0x278
	add	%o2, 0xd, %o2
L0x14c:	addcc	%o3, %o5, %o3
L0x150:	bl	L0x164
	srl	%o5, 0x1, %o5
L0x158:	subcc	%o3, %o5, %o3
L0x15c:	ba	L0x278
	add	%o2, 0xb, %o2
L0x164:	addcc	%o3, %o5, %o3
L0x168:	ba	L0x278
	add	%o2, 0x9, %o2
L0x170:	addcc	%o3, %o5, %o3
L0x174:	bl	L0x1a0
	srl	%o5, 0x1, %o5
L0x17c:	subcc	%o3, %o5, %o3
L0x180:	bl	L0x194
	srl	%o5, 0x1, %o5
L0x188:	subcc	%o3, %o5, %o3
L0x18c:	ba	L0x278
	add	%o2, 0x7, %o2
L0x194:	addcc	%o3, %o5, %o3
L0x198:	ba	L0x278
	add	%o2, 0x5, %o2
L0x1a0:	addcc	%o3, %o5, %o3
L0x1a4:	bl	L0x1b8
	srl	%o5, 0x1, %o5
L0x1ac:	subcc	%o3, %o5, %o3
L0x1b0:	ba	L0x278
	add	%o2, 0x3, %o2
L0x1b8:	addcc	%o3, %o5, %o3
L0x1bc:	ba	L0x278
	add	%o2, 0x1, %o2
L0x1c4:	addcc	%o3, %o5, %o3
L0x1c8:	bl	L0x224
	srl	%o5, 0x1, %o5
L0x1d0:	subcc	%o3, %o5, %o3
L0x1d4:	bl	L0x200
	srl	%o5, 0x1, %o5
L0x1dc:	subcc	%o3, %o5, %o3
L0x1e0:	bl	L0x1f4
	srl	%o5, 0x1, %o5
L0x1e8:	subcc	%o3, %o5, %o3
L0x1ec:	ba	L0x278
	add	%o2, -0x1, %o2
L0x1f4:	addcc	%o3, %o5, %o3
L0x1f8:	ba	L0x278
	add	%o2, -0x3, %o2
L0x200:	addcc	%o3, %o5, %o3
L0x204:	bl	L0x218
	srl	%o5, 0x1, %o5
L0x20c:	subcc	%o3, %o5, %o3
L0x210:	ba	L0x278
	add	%o2, -0x5, %o2
L0x218:	addcc	%o3, %o5, %o3
L0x21c:	ba	L0x278
	add	%o2, -0x7, %o2
L0x224:	addcc	%o3, %o5, %o3
L0x228:	bl	L0x254
	srl	%o5, 0x1, %o5
L0x230:	subcc	%o3, %o5, %o3
L0x234:	bl	L0x248
	srl	%o5, 0x1, %o5
L0x23c:	subcc	%o3, %o5, %o3
L0x240:	ba	L0x278
	add	%o2, -0x9, %o2
L0x248:	addcc	%o3, %o5, %o3
L0x24c:	ba	L0x278
	add	%o2, -0xb, %o2
L0x254:	addcc	%o3, %o5, %o3
L0x258:	bl	L0x26c
	srl	%o5, 0x1, %o5
L0x260:	subcc	%o3, %o5, %o3
L0x264:	ba	L0x278
	add	%o2, -0xd, %o2
L0x26c:	addcc	%o3, %o5, %o3
L0x270:	ba	L0x278
	add	%o2, -0xf, %o2
L0x278:	subcc	%o4, 0x1, %o4
L0x27c:	bge	L0x104
	orcc	%g0, %o3, %g0
	bge	L0x28c
	nop

	sub	%o2, 0x1, %o2
	add	%o3, %o1, %o3

L0x28c:	orcc	%g0, %g1, %g0
	bge	Lcfo
	nop

	sub	%g0, %o2, %o2
	sub	%g0, %o3, %o3

Lcfo:	mov	%o2, %o1
	jmp	%o7 + 0x8
	mov	%o3, %o0

L0x2a0:	ta	0x2
	clr %o0
	jmp	%o7 + 0x8
	clr	%o1
