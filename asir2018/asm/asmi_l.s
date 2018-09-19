.text
	.align 2

.globl dm
dm:
	pushl 	%ebp
	movl	%esp,%ebp
	pushl 	%ebx

	movl	8(%ebp),%eax
	mull	12(%ebp)
	movl 	16(%ebp),%ebx
	movl 	%edx,(%ebx)

	leal 	-4(%ebp),%esp
	popl 	%ebx
	leave
	ret

.globl dma
dma:
	pushl 	%ebp
	movl	%esp,%ebp
	pushl 	%ebx

	movl	8(%ebp),%eax
	mull	12(%ebp)
	addl	16(%ebp),%eax
	adcl	$0,%edx

	movl 	20(%ebp),%ebx
	movl 	%edx,(%ebx)

	leal 	-4(%ebp),%esp
	popl 	%ebx
	leave
	ret

.globl dmb
dmb:
	pushl 	%ebp
	movl	%esp,%ebp
	pushl 	%ebx

	movl	12(%ebp),%eax
	mull	16(%ebp)

	divl	8(%ebp)

	movl 	20(%ebp),%ebx
	movl 	%eax,(%ebx)
	movl 	%edx,%eax

	leal 	-4(%ebp),%esp
	popl 	%ebx
	leave
	ret

.globl dmab
dmab:
	pushl 	%ebp
	movl	%esp,%ebp
	pushl 	%ebx

	movl	12(%ebp),%eax
	mull	16(%ebp)
	addl	20(%ebp),%eax
	adcl	$0,%edx

	divl	8(%ebp)

	movl 	24(%ebp),%ebx
	movl 	%eax,(%ebx)
	movl 	%edx,%eax

	leal 	-4(%ebp),%esp
	popl 	%ebx
	leave
	ret

.globl dmar
dmar:
	pushl 	%ebp
	movl	%esp,%ebp

	movl	8(%ebp),%eax
	mull	12(%ebp)
	addl	16(%ebp),%eax
	adcl	$0,%edx

	divl	20(%ebp)

	movl 	%edx,%eax
	leave
	ret
