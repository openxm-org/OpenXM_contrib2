.text
	.align 2
.globl NNgf2m_mul_11
	.type	 NNgf2m_mul_11,@function
NNgf2m_mul_11:
	pushl %ebx
	movl 12(%esp),%ecx
	movl 16(%esp),%edx
L0:
	xorl %ebx,%ebx
	xorl %eax,%eax
	addl %edx,%edx
	jnc L1
	xorl %ecx,%eax
L1:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L2
	xorl %ecx,%eax
L2:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L3
	xorl %ecx,%eax
L3:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L4
	xorl %ecx,%eax
L4:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L5
	xorl %ecx,%eax
L5:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L6
	xorl %ecx,%eax
L6:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L7
	xorl %ecx,%eax
L7:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L8
	xorl %ecx,%eax
L8:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L9
	xorl %ecx,%eax
L9:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L10
	xorl %ecx,%eax
L10:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L11
	xorl %ecx,%eax
L11:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L12
	xorl %ecx,%eax
L12:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L13
	xorl %ecx,%eax
L13:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L14
	xorl %ecx,%eax
L14:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L15
	xorl %ecx,%eax
L15:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L16
	xorl %ecx,%eax
L16:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L17
	xorl %ecx,%eax
L17:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L18
	xorl %ecx,%eax
L18:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L19
	xorl %ecx,%eax
L19:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L20
	xorl %ecx,%eax
L20:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L21
	xorl %ecx,%eax
L21:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L22
	xorl %ecx,%eax
L22:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L23
	xorl %ecx,%eax
L23:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L24
	xorl %ecx,%eax
L24:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L25
	xorl %ecx,%eax
L25:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L26
	xorl %ecx,%eax
L26:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L27
	xorl %ecx,%eax
L27:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L28
	xorl %ecx,%eax
L28:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L29
	xorl %ecx,%eax
L29:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L30
	xorl %ecx,%eax
L30:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L31
	xorl %ecx,%eax
L31:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc L32
	xorl %ecx,%eax
L32:
	movl 8(%esp),%ecx
	movl %ebx,(%ecx)
	popl %ebx
	ret
Lfe1:
	.size	 NNgf2m_mul_11,Lfe1-NNgf2m_mul_11

.text
	.align 2
.globl NNgf2m_mul_1h
	.type	 NNgf2m_mul_1h,@function
NNgf2m_mul_1h:
	pushl %ebx
	movl 12(%esp),%ecx
	movl 16(%esp),%edx
	shll $16,%edx
LL0:
	xorl %ebx,%ebx
	xorl %eax,%eax
	addl %edx,%edx
	jnc LL1
	xorl %ecx,%eax
LL1:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc LL2
	xorl %ecx,%eax
LL2:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc LL3
	xorl %ecx,%eax
LL3:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc LL4
	xorl %ecx,%eax
LL4:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc LL5
	xorl %ecx,%eax
LL5:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc LL6
	xorl %ecx,%eax
LL6:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc LL7
	xorl %ecx,%eax
LL7:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc LL8
	xorl %ecx,%eax
LL8:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc LL9
	xorl %ecx,%eax
LL9:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc LL10
	xorl %ecx,%eax
LL10:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc LL11
	xorl %ecx,%eax
LL11:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc LL12
	xorl %ecx,%eax
LL12:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc LL13
	xorl %ecx,%eax
LL13:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc LL14
	xorl %ecx,%eax
LL14:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc LL15
	xorl %ecx,%eax
LL15:
	addl %eax,%eax
	adcl %ebx,%ebx
	addl %edx,%edx
	jnc LL16
	xorl %ecx,%eax
LL16:
	movl 8(%esp),%ecx
	movl %ebx,(%ecx)
	popl %ebx
	ret
LLfe1:
	.size	 NNgf2m_mul_1h,LLfe1-NNgf2m_mul_1h
