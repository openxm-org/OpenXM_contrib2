/* $OpenXM: OpenXM/src/asir99/asm/gf2m_mul_w.c,v 1.1.1.1 1999/11/10 08:12:25 noro Exp $ */
unsigned int NNgf2m_mul_11(unsigned int *h, unsigned int a, unsigned int b)
{
	__asm
	{
	push ebx
	mov ecx,a
	mov edx,b
GF2ML0:
	xor ebx,ebx
	xor eax,eax
	add edx,edx
	jnc GF2ML1
	xor eax,ecx
GF2ML1:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML2
	xor eax,ecx
GF2ML2:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML3
	xor eax,ecx
GF2ML3:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML4
	xor eax,ecx
GF2ML4:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML5
	xor eax,ecx
GF2ML5:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML6
	xor eax,ecx
GF2ML6:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML7
	xor eax,ecx
GF2ML7:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML8
	xor eax,ecx
GF2ML8:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML9
	xor eax,ecx
GF2ML9:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML10
	xor eax,ecx
GF2ML10:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML11
	xor eax,ecx
GF2ML11:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML12
	xor eax,ecx
GF2ML12:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML13
	xor eax,ecx
GF2ML13:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML14
	xor eax,ecx
GF2ML14:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML15
	xor eax,ecx
GF2ML15:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML16
	xor eax,ecx
GF2ML16:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML17
	xor eax,ecx
GF2ML17:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML18
	xor eax,ecx
GF2ML18:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML19
	xor eax,ecx
GF2ML19:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML20
	xor eax,ecx
GF2ML20:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML21
	xor eax,ecx
GF2ML21:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML22
	xor eax,ecx
GF2ML22:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML23
	xor eax,ecx
GF2ML23:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML24
	xor eax,ecx
GF2ML24:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML25
	xor eax,ecx
GF2ML25:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML26
	xor eax,ecx
GF2ML26:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML27
	xor eax,ecx
GF2ML27:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML28
	xor eax,ecx
GF2ML28:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML29
	xor eax,ecx
GF2ML29:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML30
	xor eax,ecx
GF2ML30:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML31
	xor eax,ecx
GF2ML31:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2ML32
	xor eax,ecx
GF2ML32:
	mov ecx,h
	mov DWORD PTR [ecx], ebx
	pop ebx
	}
}

unsigned int NNgf2m_mul_1h(unsigned int *h, unsigned int a, unsigned int b)
{
	__asm
	{
	push ebx
	mov ecx,a
	mov edx,b
	shl edx,0x10
GF2MLL0:
	xor ebx,ebx
	xor eax,eax
	add edx,edx
	jnc GF2MLL1
	xor eax,ecx
GF2MLL1:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2MLL2
	xor eax,ecx
GF2MLL2:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2MLL3
	xor eax,ecx
GF2MLL3:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2MLL4
	xor eax,ecx
GF2MLL4:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2MLL5
	xor eax,ecx
GF2MLL5:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2MLL6
	xor eax,ecx
GF2MLL6:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2MLL7
	xor eax,ecx
GF2MLL7:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2MLL8
	xor eax,ecx
GF2MLL8:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2MLL9
	xor eax,ecx
GF2MLL9:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2MLL10
	xor eax,ecx
GF2MLL10:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2MLL11
	xor eax,ecx
GF2MLL11:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2MLL12
	xor eax,ecx
GF2MLL12:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2MLL13
	xor eax,ecx
GF2MLL13:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2MLL14
	xor eax,ecx
GF2MLL14:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2MLL15
	xor eax,ecx
GF2MLL15:
	add eax,eax
	adc ebx,ebx
	add edx,edx
	jnc GF2MLL16
	xor eax,ecx
GF2MLL16:
	mov ecx,h
	mov DWORD PTR [ecx],ebx;
	pop ebx
	}
}
