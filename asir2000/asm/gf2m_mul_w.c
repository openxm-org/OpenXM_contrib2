/*
 * Copyright (c) 1994-2000 FUJITSU LABORATORIES LIMITED 
 * All rights reserved.
 * 
 * FUJITSU LABORATORIES LIMITED ("FLL") hereby grants you a limited,
 * non-exclusive and royalty-free license to use, copy, modify and
 * redistribute, solely for non-commercial and non-profit purposes, the
 * computer program, "Risa/Asir" ("SOFTWARE"), subject to the terms and
 * conditions of this Agreement. For the avoidance of doubt, you acquire
 * only a limited right to use the SOFTWARE hereunder, and FLL or any
 * third party developer retains all rights, including but not limited to
 * copyrights, in and to the SOFTWARE.
 * 
 * (1) FLL does not grant you a license in any way for commercial
 * purposes. You may use the SOFTWARE only for non-commercial and
 * non-profit purposes only, such as academic, research and internal
 * business use.
 * (2) The SOFTWARE is protected by the Copyright Law of Japan and
 * international copyright treaties. If you make copies of the SOFTWARE,
 * with or without modification, as permitted hereunder, you shall affix
 * to all such copies of the SOFTWARE the above copyright notice.
 * (3) An explicit reference to this SOFTWARE and its copyright owner
 * shall be made on your publication or presentation in any form of the
 * results obtained by use of the SOFTWARE.
 * (4) In the event that you modify the SOFTWARE, you shall notify FLL by
 * e-mail at risa-admin@sec.flab.fujitsu.co.jp of the detailed specification
 * for such modification or the source code of the modified part of the
 * SOFTWARE.
 * 
 * THE SOFTWARE IS PROVIDED AS IS WITHOUT ANY WARRANTY OF ANY KIND. FLL
 * MAKES ABSOLUTELY NO WARRANTIES, EXPRESSED, IMPLIED OR STATUTORY, AND
 * EXPRESSLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT OF THIRD PARTIES'
 * RIGHTS. NO FLL DEALER, AGENT, EMPLOYEES IS AUTHORIZED TO MAKE ANY
 * MODIFICATIONS, EXTENSIONS, OR ADDITIONS TO THIS WARRANTY.
 * UNDER NO CIRCUMSTANCES AND UNDER NO LEGAL THEORY, TORT, CONTRACT,
 * OR OTHERWISE, SHALL FLL BE LIABLE TO YOU OR ANY OTHER PERSON FOR ANY
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, PUNITIVE OR CONSEQUENTIAL
 * DAMAGES OF ANY CHARACTER, INCLUDING, WITHOUT LIMITATION, DAMAGES
 * ARISING OUT OF OR RELATING TO THE SOFTWARE OR THIS AGREEMENT, DAMAGES
 * FOR LOSS OF GOODWILL, WORK STOPPAGE, OR LOSS OF DATA, OR FOR ANY
 * DAMAGES, EVEN IF FLL SHALL HAVE BEEN INFORMED OF THE POSSIBILITY OF
 * SUCH DAMAGES, OR FOR ANY CLAIM BY ANY OTHER PARTY. EVEN IF A PART
 * OF THE SOFTWARE HAS BEEN DEVELOPED BY A THIRD PARTY, THE THIRD PARTY
 * DEVELOPER SHALL HAVE NO LIABILITY IN CONNECTION WITH THE USE,
 * PERFORMANCE OR NON-PERFORMANCE OF THE SOFTWARE.
 *
 * $OpenXM: OpenXM_contrib2/asir2000/asm/gf2m_mul_w.c,v 1.2 2000/08/21 08:31:17 noro Exp $ 
*/
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
