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
 * $OpenXM$
*/
#define DMB(base,a1,a2,u,l) (l)=dmb(base,a1,a2,&(u));
#define DMAB(base,a1,a2,a3,u,l) (l)=dmab(base,a1,a2,a3,&(u));
#define DMAR(a1,a2,a3,d,r) (r)=dmar(a1,a2,a3,d);
#define DM27(a1,a2,u,l) (l)=dm_27(a1,a2,&(u));
#define DMA27(a1,a2,a3,u,l) (l)=dma_27(a1,a2,a3,&(u));
#define DSAB27(base,a1,a2,u,l) (l)=dmab(base,a1,BASE27,a2,&(u));

#if defined(__GNUC__) || defined(__INTEL_COMPILER) || defined(__alpha) || defined(mips) || defined(_IBMR2)
#define DM(a1,a2,u,l)\
{UL _t;\
_t=(UL)(a1)*(UL)(a2);\
(u)=(unsigned int)(_t>>BSH);\
(l)=(unsigned int)(_t&BMASK);}

#define DMA(a1,a2,a3,u,l)\
{UL _t;\
_t=(UL)(a1)*(UL)(a2)+(UL)(a3);\
(u)=(unsigned int)(_t>>BSH);\
(l)=(unsigned int)(_t&BMASK);}

#define DMA2(a1,a2,a3,a4,u,l)\
{UL _t;\
_t=(UL)(a1)*(UL)(a2)+(UL)(a3)+(UL)(a4);\
(u)=(unsigned int)(_t>>BSH);\
(l)=(unsigned int)(_t&BMASK);}

#define DSAB(base,a1,a2,u,l)\
{UL _t;\
_t=(((UL)(a1))<<BSH)|((UL)(a2));\
(u)=(unsigned int)(_t/((UL)(base)));\
(l)=(unsigned int)(_t-((UL)(base)*(UL)(u)));}
#else
#define DM(a1,a2,u,l) (l)=dm(a1,a2,&(u));
#define DMA(a1,a2,a3,u,l) (l)=dma(a1,a2,a3,&(u));
#define DMA2(a1,a2,a3,a4,u,l) (l)=dma2(a1,a2,a3,a4,&(u));
#define DSAB(base,a1,a2,u,l) (l)=dsab(base,a1,a2,&(u));
#endif

#define DQR(a,b,q,r)\
(q)=(a)/(b);\
(r)=(a)-(b)*(q);

#if defined(sparc)
#undef DSAB
#undef DSAB27

#define DSAB(base,a1,a2,u,l)\
{unsigned int _t;\
asm volatile("wr %0,%%g0,%%y"   :       : "r"(a1)           );\
asm volatile("udiv %1,%2,%0"    :"=r"(_t): "r"(a2),"r"(base) );\
(l)=(unsigned int)(a2)-(unsigned int)(_t)*(unsigned int)(base);\
(u)=(_t);}

#define DSAB27(base,a1,a2,u,l) (l)=dsab_27(base,a1,a2,&(u));
#endif

#if defined(_M_IX86) && !defined(__MINGW32__)

#undef DM
#undef DMA
#undef DMA2
#undef DMB
#undef DMAB
#undef DMAR
#undef DSAB
#undef DM27
#undef DMA27
#undef DSAB27

#define DSAB(base,a1,a2,u,l)\
{\
   unsigned int _t1,_t2,_d;\
   _t1=a1;_t2=a2;_d=base;\
   __asm\
   {\
   __asm mov  edx,_t1\
   __asm mov  eax,_t2\
   __asm div  _d\
   __asm mov  _t1,eax\
   __asm mov  _t2,edx\
   }\
   u=_t1;l=_t2;\
}

#define DM(a1,a2,u,l)\
{\
   unsigned int _t1,_t2;\
   _t1=a1;_t2=a2;\
   __asm\
   {\
   __asm mov  eax,_t1\
   __asm mul  _t2\
   __asm mov  _t1,edx\
   __asm mov  _t2,eax\
   }\
   u=_t1;l=_t2;\
}

#define DMA(a1,a2,a3,u,l)\
{\
   unsigned int _t1,_t2,_t3;\
   _t1=a1;_t2=a2;_t3=a3;\
   __asm\
   {\
   __asm mov  eax,_t1\
   __asm mul  _t2\
   __asm add  eax,_t3\
   __asm adc  edx,0\
   __asm mov  _t1,edx\
   __asm mov  _t2,eax\
   }\
   u=_t1;l=_t2;\
}

#define DMA2(a1,a2,a3,a4,u,l)\
{\
   unsigned int _t1,_t2,_t3,_t4;\
   _t1=a1;_t2=a2;_t3=a3;_t4=a4;\
   __asm\
   {\
   __asm mov  eax,_t1\
   __asm mul  _t2\
   __asm add  eax,_t3\
   __asm adc  edx,0\
   __asm add  eax,_t4\
   __asm adc  edx,0\
   __asm mov  _t1,edx\
   __asm mov  _t2,eax\
   }\
   u=_t1;l=_t2;\
}

#define DMB(base,a1,a2,u,l)\
{\
   unsigned int _t1,_t2,_d;\
   _t1=a1;_t2=a2;_d=base;\
   __asm\
   {\
   __asm mov  eax,_t1\
   __asm mul  _t2\
   __asm div  _d\
   __asm mov  _t1,eax\
   __asm mov  _t2,edx\
   }\
   u=_t1;l=_t2;\
}

#define DMAB(base,a1,a2,a3,u,l)\
{\
   unsigned int _t1,_t2,_t3,_d;\
   _t1=a1;_t2=a2;_t3=a3;_d=base;\
   __asm\
   {\
   __asm mov  eax,_t1\
   __asm mul  _t2\
   __asm add  eax,_t3\
   __asm adc  edx,0\
   __asm div  _d\
   __asm mov  _t1,eax\
   __asm mov  _t2,edx\
   }\
   u=_t1;l=_t2;\
}

#define DMAR(a1,a2,a3,d,r)\
{\
   unsigned int _t1,_t2,_t3,_d;\
   _t1=a1;_t2=a2;_t3=a3,_d=d;\
   __asm\
   {\
   __asm mov  eax,_t1\
   __asm mul  _t2\
   __asm add  eax,_t3\
   __asm adc  edx,0\
   __asm div  _d\
   __asm mov  _t1,edx\
   }\
   r=_t1;\
}

#define DSAB27(base,a1,a2,u,l)\
{\
   unsigned int _t1,_t2,_d;\
   _t1=a1;_t2=a2;_d=base;\
   __asm\
   {\
   __asm mov  edx,_t1\
   __asm mov  eax,_t2\
   __asm mov  ecx,edx\
   __asm shl  ecx,27\
   __asm or    eax,ecx\
   __asm shr  edx,5\
   __asm div  _d\
   __asm mov  _t1,eax\
   __asm mov  _t2,edx\
   }\
   u=_t1;l=_t2;\
}

#define DM27(a1,a2,u,l)\
{\
   unsigned int _t1,_t2;\
   _t1=a1;_t2=a2;\
   __asm\
   {\
   __asm mov  eax,_t1\
   __asm mul  _t2\
   __asm shl  edx,5\
   __asm mov  ecx,eax\
   __asm shr  ecx,27\
   __asm or    edx,ecx\
   __asm and  eax,134217727\
   __asm mov  _t1,edx\
   __asm mov  _t2,eax\
   }\
   u=_t1;l=_t2;\
}

#define DMA27(a1,a2,a3,u,l)\
{\
   unsigned int _t1,_t2,_t3;\
   _t1=a1;_t2=a2;_t3=a3;\
   __asm\
   {\
   __asm mov  eax,_t1\
   __asm mul  _t2\
   __asm add  eax,_t3\
   __asm adc  edx,0\
   __asm shl  edx,5\
   __asm mov  ecx,eax\
   __asm shr  ecx,27\
   __asm or    edx,ecx\
   __asm and  eax,134217727\
   __asm mov  _t1,edx\
   __asm mov  _t2,eax\
   }\
   u=_t1;l=_t2;\
}
#endif

#if !defined(VISUAL) && defined(i386) && (defined(linux) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__CYGWIN__) || defined(__DARWIN__))

#if 0
#undef DMA
#define DMA(a1,a2,a3,u,l)\
asm volatile("movl  %0,%%eax"  :     : "g" (a1)  : "ax");\
asm volatile("mull  %0"    :     : "g" (a2)  : "ax","dx");\
asm volatile("addl  %0,%%eax"  :     : "g" (a3)  : "ax"  );\
asm volatile("adcl  $0,%%edx"  :     :     : "dx"  );\
asm volatile("movl  %%edx,%0"  : "=g" (u)  :    : "ax","dx");\
asm volatile("movl  %%eax,%0"  : "=g" (l)  :    : "ax"  );
#endif

#undef DM
#undef DMA
#undef DMB
#undef DMAB
#undef DMAR
#undef DSAB
#undef DM27
#undef DMA27

#define DM27(a1,a2,u,l)\
asm volatile(" movl   %2,%%eax; mull  %3; shll  $5,%%edx; movl  %%eax,%%ecx; shrl  $27,%%ecx; orl    %%ecx,%%edx; andl  $134217727,%%eax; movl  %%edx,%0; movl  %%eax,%1" :"=g"(u),"=g"(l) :"g"(a1),"g"(a2) :"ax","bx","cx","dx");

#define DMA27(a1,a2,a3,u,l)\
asm volatile(" movl  %2,%%eax; mull  %3; addl  %4,%%eax; adcl  $0,%%edx; shll  $5,%%edx; movl  %%eax,%%ecx; shrl  $27,%%ecx; orl    %%ecx,%%edx; andl  $134217727,%%eax; movl  %%edx,%0; movl  %%eax,%1" :"=g"(u),"=g"(l) :"g"(a1),"g"(a2),"g"(a3) :"ax","bx","cx","dx");

#define DSAB(base,a1,a2,u,l)\
asm volatile(" movl  %2,%%edx; movl  %3,%%eax; divl  %4; movl  %%edx,%0; movl  %%eax,%1" :"=g"(l),"=g"(u) :"g"(a1),"g"(a2),"g"(base) :"ax","dx");

#define DM(a1,a2,u,l)\
asm volatile(" movl  %2,%%eax; mull  %3; movl  %%edx,%0; movl  %%eax,%1" :"=g"(u),"=g"(l) :"g"(a1),"g"(a2) :"ax","dx");

#define DMA(a1,a2,a3,u,l)\
asm volatile("movl  %2,%%eax; mull  %3; addl    %4,%%eax; adcl  $0,%%edx; movl    %%edx,%0; movl %%eax,%1" :"=g"(u), "=g"(l) :"g"(a1),"g"(a2),"g"(a3) :"ax","dx");

#define DMB(base,a1,a2,u,l)\
asm volatile(" movl  %2,%%eax;" mull  %3;" divl  %4;" movl  %%edx,%0;" movl  %%eax,%0" :"=g"(l),"=g"(u) :"g"(a1),"g"(a2),"g"(base) :"ax","dx");

#define DMAB(base,a1,a2,a3,u,l)\
asm volatile("movl  %2,%%eax; mull  %3; addl  %4,%%eax; adcl  $0,%%edx; divl  %5; movl  %%edx,%0; movl  %%eax,%1" :"=g"(l),"=g"(u) :"g"(a1),"g"(a2),"g"(a3),"g"(base) :"ax","dx");

#define DMAR(a1,a2,a3,d,r)\
asm volatile("movl  %1,%%eax; mull  %2; addl  %3,%%eax; adcl  $0,%%edx; divl  %4; movl  %%edx,%0" :"=g"(r) :"g"(a1),"g"(a2),"g"(a3),"g"(d) :"ax","dx");
#endif

