.text
	.align 2

.globl _dm_27
_dm_27:
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

.globl _dma_27
_dma_27:
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

.globl _dmb_27
_dmb_27:
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

.globl _dmab_27
_dmab_27:
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

.globl _dmar_27
_dmar_27:
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
