.text
	.align 2

.globl dm_27
dm_27:
	pushl 	%ebp
	movl	%esp,%ebp
	pushl 	%ebx

	movl	8(%ebp),%eax
	mull	12(%ebp)

	shll 	$5,%edx
	movl 	%eax,%ecx
	shrl 	$27,%ecx
	orl 	%ecx,%edx
	andl 	$134217727,%eax

	movl 	16(%ebp),%ebx
	movl 	%edx,(%ebx)

	leal 	-4(%ebp),%esp
	popl 	%ebx
	leave
	ret

.globl dma_27
dma_27:
	pushl 	%ebp
	movl	%esp,%ebp
	pushl 	%ebx

	movl	8(%ebp),%eax
	mull	12(%ebp)
	addl	16(%ebp),%eax
	adcl	$0,%edx

	shll 	$5,%edx
	movl 	%eax,%ecx
	shrl 	$27,%ecx
	orl 	%ecx,%edx
	andl 	$134217727,%eax

	movl 	20(%ebp),%ebx
	movl 	%edx,(%ebx)

	leal 	-4(%ebp),%esp
	popl 	%ebx
	leave
	ret

.globl dmb_27
dmb_27:
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

.globl dmab_27
dmab_27:
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

.globl dmar_27
dmar_27:
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
