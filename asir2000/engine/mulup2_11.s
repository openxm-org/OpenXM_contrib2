	/* $OpenXM: OpenXM/src/asir99/engine/mulup2_11.s,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
	.file	"z.c"
gcc2_compiled.:
___gnu_compiled_c:
.text
	.align 2
.globl _mulup2_11
	.type	 _mulup2_11,@function
_mulup2_11:
	pushl %ebx
	pushl %ecx
	movl 12(%esp),%eax
	movl 16(%esp),%ebx
L0:
	xorl %ecx,%ecx
	xorl %edx,%edx
	addl %ebx,%ebx
	jnc L1
	xorl %eax,%edx
L1:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L2
	xorl %eax,%edx
L2:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L3
	xorl %eax,%edx
L3:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L4
	xorl %eax,%edx
L4:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L5
	xorl %eax,%edx
L5:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L6
	xorl %eax,%edx
L6:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L7
	xorl %eax,%edx
L7:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L8
	xorl %eax,%edx
L8:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L9
	xorl %eax,%edx
L9:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L10
	xorl %eax,%edx
L10:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L11
	xorl %eax,%edx
L11:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L12
	xorl %eax,%edx
L12:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L13
	xorl %eax,%edx
L13:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L14
	xorl %eax,%edx
L14:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L15
	xorl %eax,%edx
L15:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L16
	xorl %eax,%edx
L16:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L17
	xorl %eax,%edx
L17:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L18
	xorl %eax,%edx
L18:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L19
	xorl %eax,%edx
L19:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L20
	xorl %eax,%edx
L20:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L21
	xorl %eax,%edx
L21:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L22
	xorl %eax,%edx
L22:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L23
	xorl %eax,%edx
L23:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L24
	xorl %eax,%edx
L24:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L25
	xorl %eax,%edx
L25:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L26
	xorl %eax,%edx
L26:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L27
	xorl %eax,%edx
L27:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L28
	xorl %eax,%edx
L28:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L29
	xorl %eax,%edx
L29:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L30
	xorl %eax,%edx
L30:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L31
	xorl %eax,%edx
L31:
	shldl $1,%edx,%ecx
	shll $1,%edx
	addl %ebx,%ebx
	jnc L32
	xorl %eax,%edx
L32:
	movl 20(%esp),%eax
	movl %ecx,(%eax)
	movl 24(%esp),%eax
	movl %edx,(%eax)
	popl %ecx
	popl %ebx
	ret
Lfe1:
	.size	 _mulup2_11,Lfe1-_mulup2_11
