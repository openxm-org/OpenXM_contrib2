/* $OpenXM: OpenXM/src/asir99/asm/asmiw.c,v 1.1.1.1 1999/11/10 08:12:25 noro Exp $ */
unsigned int dm(a1,a2,u)
unsigned int a1,a2,*u;
{
	__asm
	{
	mov		eax,a1
	mul		a2
	mov 	ebx,u
	mov 	DWORD PTR [ebx],edx
	}
}

unsigned int dma(a1,a2,a3,u)
unsigned int a1,a2,a3,*u;
{
	__asm
	{
	mov		eax,a1
	mul		a2
	add		eax,a3
	adc		edx,0
	mov 	ebx,u
	mov 	DWORD PTR [ebx],edx
	}
}

unsigned int dma2(a1,a2,a3,a4,u)
unsigned int a1,a2,a3,a4,*u;
{
	__asm
	{
	mov		eax,a1
	mul		a2
	add		eax,a3
	adc		edx,0
	add		eax,a4
	adc		edx,0
	mov 	ebx,u
	mov 	DWORD PTR [ebx],edx
	}
}

unsigned int dmb(base,a1,a2,u)
unsigned int base,a1,a2,*u;
{
	__asm
	{
	mov		eax,a1
	mul		a2
	div		base
	mov 	ebx,u
	mov 	DWORD PTR [ebx],eax
	mov 	eax,edx
	}
}

unsigned int dmab(base,a1,a2,a3,u)
unsigned int base,a1,a2,a3,*u;
{
	__asm
	{
	mov		eax,a1
	mul		a2
	add		eax,a3
	adc		edx,0
	div		base
	mov 	ebx,u
	mov 	DWORD PTR [ebx],eax
	mov 	eax,edx
	}
}

unsigned int dsab(base,a1,a2,u)
unsigned int base,a1,a2,*u;
{
	__asm
	{
	mov		edx,a1
	mov		eax,a2
	div		base
	mov 	ebx,u
	mov 	DWORD PTR [ebx],eax
	mov 	eax,edx
	}
}

unsigned int dmar(a1,a2,a3,d)
unsigned int a1,a2,a3,d;
{
	__asm
	{
	mov		eax,a1
	mul		a2
	add		eax,a3
	adc		edx,0
	div		d
	mov 	eax,edx
	}
}
