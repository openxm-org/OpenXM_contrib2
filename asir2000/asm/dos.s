.text
	.align 2

.globl _read_cons
_read_cons:
	pushl %ebp
	movl %esp,%ebp
	pushl %ebx
	pushl %esi
	pushl %edi
	pushf
	movb $6,%ah
	movb $0xff,%dl
	int $0x21
	movzbl %al,%eax
	popf
	popl %edi
	popl %esi
	popl %ebx
	leave
	ret

.globl _read_cons_blocking
_read_cons_blocking:
	pushl %ebp
	movl %esp,%ebp
	pushl %ebx
	pushl %esi
	pushl %edi
	pushf
	movb $7,%ah
	int $0x21
	movzbl %al,%eax
	popf
	popl %edi
	popl %esi
	popl %ebx
	leave
	ret

.globl _set_pb
_set_pb:
	pushl %ebp
	movl %esp,%ebp
	pushl %ebx
	pushl %esi
	pushl %edi
	pushf
	movl 8(%ebp),%eax
	movl 12(%ebp),%edx
	movl %edx,(%eax)
	movw $0,4(%eax)
	movl 16(%ebp),%edx
	movl %edx,6(%eax)
	movw %es,%dx
	movw %dx,10(%eax)
	movb $0,12(%eax)
	popf
	popl %edi
	popl %esi
	popl %ebx
	leave
	ret
	
.globl _disable_ctrl_c
_disable_ctrl_c:
	pushl %ebp
	movl %esp,%ebp
	pushl %ebx
	pushl %esi
	pushl %edi
	pushf
	movw $0x2506,%ax
	pushw %ds
	movw %cs,%dx
	movw %dx,%ds
	movb $0x23,%cl
	movl $_dummy_ctrl_c,%edx
	int $0x21
	popw %ds
	popf
	popl %edi
	popl %esi
	popl %ebx
	leave
	ret

.globl _dummy_ctrl_c
_dummy_ctrl_c:
	iret
