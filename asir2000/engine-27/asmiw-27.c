/* $OpenXM: OpenXM/src/asir99/engine-27/asmiw-27.c,v 1.1.1.1 1999/11/10 08:12:27 noro Exp $ */
unsigned int dm_27(a1,a2,u)
unsigned int a1,a2,*u;
{
	__asm
	{
	mov		eax,a1
	mul		a2
	shl		edx,5
	mov		ecx,eax
	shr		ecx,27
	or		edx,ecx
	and		eax,134217727
	mov 	ebx,u
	mov 	DWORD PTR [ebx],edx
	}
}

unsigned int dma_27(a1,a2,a3,u)
unsigned int a1,a2,a3,*u;
{
	__asm
	{
	mov		eax,a1
	mul		a2
	add		eax,a3
	adc		edx,0
	shl		edx,5
	mov		ecx,eax
	shr		ecx,27
	or		edx,ecx
	and		eax,134217727
	mov 	ebx,u
	mov 	DWORD PTR [ebx],edx
	}
}
