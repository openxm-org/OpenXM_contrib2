/* $OpenXM: OpenXM_contrib2/asir2000/include/inline.h,v 1.1.1.1 1999/12/03 07:39:11 noro Exp $ */
#define DMB(base,a1,a2,u,l) (l)=dmb(base,a1,a2,&(u));
#define DMAB(base,a1,a2,a3,u,l) (l)=dmab(base,a1,a2,a3,&(u));
#define DMAR(a1,a2,a3,d,r) (r)=dmar(a1,a2,a3,d);

#define DM27(a1,a2,u,l) (l)=dm_27(a1,a2,&(u));
#define DMA27(a1,a2,a3,u,l) (l)=dma_27(a1,a2,a3,&(u));
#define DSAB27(base,a1,a2,u,l) (l)=dmab(base,a1,BASE27,a2,&(u));

#if defined(__GNUC__) || defined(__alpha) || defined(mips)
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

#if defined(VISUAL)

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
   __asm mov	edx,_t1\
   __asm mov	eax,_t2\
   __asm div	_d\
   __asm mov	_t1,eax\
   __asm mov	_t2,edx\
   }\
   u=_t1;l=_t2;\
}

#define DM(a1,a2,u,l)\
{\
   unsigned int _t1,_t2;\
   _t1=a1;_t2=a2;\
   __asm\
   {\
   __asm mov	eax,_t1\
   __asm mul	_t2\
   __asm mov	_t1,edx\
   __asm mov	_t2,eax\
   }\
   u=_t1;l=_t2;\
}

#define DMA(a1,a2,a3,u,l)\
{\
   unsigned int _t1,_t2,_t3;\
   _t1=a1;_t2=a2;_t3=a3;\
   __asm\
   {\
   __asm mov	eax,_t1\
   __asm mul	_t2\
   __asm add	eax,_t3\
   __asm adc	edx,0\
   __asm mov	_t1,edx\
   __asm mov	_t2,eax\
   }\
   u=_t1;l=_t2;\
}

#define DMA2(a1,a2,a3,a4,u,l)\
{\
   unsigned int _t1,_t2,_t3,_t4;\
   _t1=a1;_t2=a2;_t3=a3;_t4=a4;\
   __asm\
   {\
   __asm mov	eax,_t1\
   __asm mul	_t2\
   __asm add	eax,_t3\
   __asm adc	edx,0\
   __asm add	eax,_t4\
   __asm adc	edx,0\
   __asm mov	_t1,edx\
   __asm mov	_t2,eax\
   }\
   u=_t1;l=_t2;\
}

#define DMB(base,a1,a2,u,l)\
{\
   unsigned int _t1,_t2,_d;\
   _t1=a1;_t2=a2;_d=base;\
   __asm\
   {\
   __asm mov	eax,_t1\
   __asm mul	_t2\
   __asm div	_d\
   __asm mov	_t1,eax\
   __asm mov	_t2,edx\
   }\
   u=_t1;l=_t2;\
}

#define DMAB(base,a1,a2,a3,u,l)\
{\
   unsigned int _t1,_t2,_t3,_d;\
   _t1=a1;_t2=a2;_t3=a3;_d=base;\
   __asm\
   {\
   __asm mov	eax,_t1\
   __asm mul	_t2\
   __asm add	eax,_t3\
   __asm adc	edx,0\
   __asm div	_d\
   __asm mov	_t1,eax\
   __asm mov	_t2,edx\
   }\
   u=_t1;l=_t2;\
}

#define DMAR(a1,a2,a3,d,r)\
{\
   unsigned int _t1,_t2,_t3,_d;\
   _t1=a1;_t2=a2;_t3=a3,_d=d;\
   __asm\
   {\
   __asm mov	eax,_t1\
   __asm mul	_t2\
   __asm add	eax,_t3\
   __asm adc	edx,0\
   __asm div	_d\
   __asm mov	_t1,edx\
   }\
   r=_t1;\
}

#define DSAB27(base,a1,a2,u,l)\
{\
   unsigned int _t1,_t2,_d;\
   _t1=a1;_t2=a2;_d=base;\
   __asm\
   {\
   __asm mov	edx,_t1\
   __asm mov	eax,_t2\
   __asm mov	ecx,edx\
   __asm shl	ecx,27\
   __asm or		eax,ecx\
   __asm shr	edx,5\
   __asm div	_d\
   __asm mov	_t1,eax\
   __asm mov	_t2,edx\
   }\
   u=_t1;l=_t2;\
}

#define DM27(a1,a2,u,l)\
{\
   unsigned int _t1,_t2;\
   _t1=a1;_t2=a2;\
   __asm\
   {\
   __asm mov	eax,_t1\
   __asm mul	_t2\
   __asm shl	edx,5\
   __asm mov	ecx,eax\
   __asm shr	ecx,27\
   __asm or		edx,ecx\
   __asm and	eax,134217727\
   __asm mov	_t1,edx\
   __asm mov	_t2,eax\
   }\
   u=_t1;l=_t2;\
}

#define DMA27(a1,a2,a3,u,l)\
{\
   unsigned int _t1,_t2,_t3;\
   _t1=a1;_t2=a2;_t3=a3;\
   __asm\
   {\
   __asm mov	eax,_t1\
   __asm mul	_t2\
   __asm add	eax,_t3\
   __asm adc	edx,0\
   __asm shl	edx,5\
   __asm mov	ecx,eax\
   __asm shr	ecx,27\
   __asm or		edx,ecx\
   __asm and	eax,134217727\
   __asm mov	_t1,edx\
   __asm mov	_t2,eax\
   }\
   u=_t1;l=_t2;\
}
#endif

#if !defined(VISUAL) && defined(i386) && (defined(linux) || defined(__FreeBSD__) || defined(__NetBSD__))

#if 0
#undef DMA
#define DMA(a1,a2,a3,u,l)\
asm volatile("movl	%0,%%eax"	: 		: "m" (a1)	: "ax");\
asm volatile("mull	%0"		: 		: "m" (a2)	: "ax","dx");\
asm volatile("addl	%0,%%eax"	: 		: "m" (a3)	: "ax"	);\
asm volatile("adcl	$0,%%edx"	: 		: 		: "dx"	);\
asm volatile("movl	%%edx,%0"	: "=m" (u)	:		: "ax","dx");\
asm volatile("movl	%%eax,%0"	: "=m" (l)	:		: "ax"	);
#endif

#undef DM
#undef DMB
#undef DMAB
#undef DMAR
#undef DSAB
#undef DM27
#undef DMA27

#define DM27(a1,a2,u,l)\
asm volatile(" movl 	%2,%%eax; mull	%3; shll	$5,%%edx; movl	%%eax,%%ecx; shrl	$27,%%ecx; orl		%%ecx,%%edx; andl	$134217727,%%eax; movl	%%edx,%0; movl	%%eax,%1" :"=m"(u),"=m"(l) :"m"(a1),"m"(a2) :"ax","bx","cx","dx");

#define DMA27(a1,a2,a3,u,l)\
asm volatile(" movl	%2,%%eax; mull	%3; addl	%4,%%eax; adcl	$0,%%edx; shll	$5,%%edx; movl	%%eax,%%ecx; shrl	$27,%%ecx; orl		%%ecx,%%edx; andl	$134217727,%%eax; movl	%%edx,%0; movl	%%eax,%1" :"=m"(u),"=m"(l) :"m"(a1),"m"(a2),"m"(a3) :"ax","bx","cx","dx");

#define DSAB(base,a1,a2,u,l)\
asm volatile(" movl	%2,%%edx; movl	%3,%%eax; divl	%4; movl	%%edx,%0; movl	%%eax,%1" :"=m"(l),"=m"(u) :"m"(a1),"m"(a2),"m"(base) :"ax","dx");

#define DM(a1,a2,u,l)\
asm volatile(" movl	%2,%%eax; mull	%3; movl	%%edx,%0; movl	%%eax,%1" :"=m"(u),"=m"(l) :"m"(a1),"m"(a2) :"ax","dx");

#define DMB(base,a1,a2,u,l)\
asm volatile(" movl	%2,%%eax;" mull	%3;" divl	%4;" movl	%%edx,%0;" movl	%%eax,%0" :"=m"(l),"=m"(u) :"m"(a1),"m"(a2),"m"(base) :"ax","dx");

#define DMAB(base,a1,a2,a3,u,l)\
asm volatile("movl	%2,%%eax; mull	%3; addl	%4,%%eax; adcl	$0,%%edx; divl	%5; movl	%%edx,%0; movl	%%eax,%1" :"=m"(l),"=m"(u) :"m"(a1),"m"(a2),"m"(a3),"m"(base) :"ax","dx");

#define DMAR(a1,a2,a3,d,r)\
asm volatile("movl	%1,%%eax; mull	%2; addl	%3,%%eax; adcl	$0,%%edx; divl	%4; movl	%%edx,%0" :"=m"(r) :"m"(a1),"m"(a2),"m"(a3),"m"(d) :"ax","dx");
#endif

