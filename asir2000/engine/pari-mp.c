/*
 * $OpenXM: $
*/
/* for f-itv.c */

#include "genpari.h"
#include "itv-pari.h"


GEN
PariAddDown(GEN x, GEN y)
{
  if(typ(x)==1) return (typ(y)==1) ? addii(x,y) : PariAddirDown(y,x);
  return (typ(y)==1) ? PariAddirDown(y,x) : PariAddrrDown(x,y);
}

GEN
PariAddirDown(GEN x, GEN y)
{
  long l,e,ly,av,i,l1;
  GEN z;
  
  if(!signe(x)) return rcopy(y);
  if(!signe(y))
  {
    l=lgef(x)-(expo(y)>>TWOPOTBITS_IN_LONG);if((l<3)||(l>32767)) err(adder3);
    z=cgetr(l);affir(x,z);return z;
  }
  else
  {
    e=expo(y)-expi(x);ly=lg(y);
    if(e>0)
    {
      l=ly-(e>>TWOPOTBITS_IN_LONG);if(l<=2) return rcopy(y);
    }
    else
    { 
      l=ly+((-e)>>TWOPOTBITS_IN_LONG)+1;if(l>32767) err(adder3);
    }
    av=avma;z=cgetr(l);affir(x,z);l1=av-avma;l=l1>>TWOPOTBYTES_IN_LONG;
    z=PariAddrrDown(z,y);
    for(i=lg(z)-1;i>=0;i--) z[i+l]=z[i];z+=l;avma+=l1;
  }
  return z;
}

GEN
PariAddrrDown(GEN x, GEN y)
{
  long sx=signe(x),sy=signe(y),lx=lg(x),ly=lg(y),lz,ex=expo(x),ey=expo(y),sz;
  long av0=avma,e,l,i,d,m,flag,lp1,lp2,av,k,j,cex,f2;
  GEN z,p1,p2;
  
  if(!sy)
  {
    if(!sx) {e=(ex>=ey)?ex:ey;z=cgetr(3);z[2]=0;z[1]=e+HIGHEXPOBIT;return z;}
    e=ex-ey;
    if(e<0) {z=cgetr(3);z[2]=0;z[1]=ey+HIGHEXPOBIT;return z;}
    l=(e>>TWOPOTBITS_IN_LONG)+3;if(l>lx) l=lx;z=cgetr(l);
    for(i=1;i<l;i++) z[i]=x[i];return z;
  }
  e=ey-ex;
  if(!sx)
  {
    if(e<0) {z=cgetr(3);z[2]=0;z[1]=ex+HIGHEXPOBIT;return z;}
    l=(e>>TWOPOTBITS_IN_LONG)+3;if(l>ly) l=ly;z=cgetr(l);
    for(i=1;i<l;i++) z[i]=y[i];return z;
  }
  if(e)
  {
    if(e<0) {z=x;x=y;y=z;lz=lx;lx=ly;ly=lz;ey=ex;e= -e;sz=sx;sx=sy;sy=sz;}
    d=(e>>TWOPOTBITS_IN_LONG);m=e&(BITS_IN_LONG-1);
    if(d>=ly-2) return rcopy(y);
    l=d+lx;
    if(l>=ly)
    {
      flag=1;p1=cgetr(ly);lp1=ly;lp2=ly-d;
    }
    else
    {
      flag=0;p1=cgetr(l+1);lp2=lx+1;lp1=l+1;
    }
    av=avma;
    if(m)
    {
      p2=cgetr(lp2);m=BITS_IN_LONG-m;
      if(flag) {shiftl(x[lp2-1],m);k=hiremainder;}
      else k=0;
      for(i=lp2-1;i>=3;i--) 
      {
	p2[i]=shiftl(x[i-1],m)+k;k=hiremainder;
      }
      p2[2]=k;
    }
    else p2=x;
  }
  else
  {
    l=(lx>ly)?ly:lx;p1=cgetr(l);av=avma;lp2=lp1=l;flag=2;p2=x;m=0;
  }
  if(sx==sy)
  {
    overflow=0;
    if(m+flag) for(i=lp1-1,j=lp2-1;j>=2;i--,j--) p1[i]=addllx(p2[j],y[i]);
    else 
    {
      p1[lp1-1]=y[lp1-1];
      for(i=lp1-2,j=lp2-2;j>=2;i--,j--) p1[i]=addllx(p2[j],y[i]);
    }
    if(overflow)
    {
      for(;(i>=2)&&(y[i]==(long)MAXULONG);i--) p1[i]=0;
      if(i>=2) {cex=0;p1[i]=y[i]+1;while(i>=3) {i--;p1[i]=y[i];}}
      else 
      {
	cex=1;k=HIGHBIT;if(ey==(HIGHEXPOBIT-1)) err(adder4);
	for(i=2;i<lp1;i++) {p1[i]=shiftlr(p1[i],1)+k;k=hiremainder;}

        if ( sx < 0 && hiremainder ) { /* |p1| + 1 */
            overflow=1;
            for(j=lg(p1);(overflow) && j>=2;j--) p1[j]=addllx(p1[j],0);
        }

      }
    }
    else {cex=0;for(;i>=2;i--) p1[i]=y[i];}
    p1[1]=evalsigne(sx)+ey+cex+HIGHEXPOBIT;
    avma=av;return p1;
  }
  else 
  {
    if(!e) 
    {
      for(i=2;(i<l)&&(p2[i]==y[i]);i++);
      if(i==l)
      {
	e=ex-((l-2)<<TWOPOTBITS_IN_LONG)+HIGHEXPOBIT;if(e<0) err(adder5);
	if(e>EXPOBITS) err(adder4);
	avma=av0;z=cgetr(3);z[2]=0;z[1]=e;return z;
      }
      else
      {
	f2=(((ulong)y[i])>((ulong)p2[i]))?1:0;
      }
    }
    else f2=1;
    if(f2)
    {
      overflow=0;
      if(m+flag) for(i=lp1-1,j=lp2-1;j>=2;i--,j--) p1[i]=subllx(y[i],p2[j]);
      else 
      {
	p1[lp1-1]=y[lp1-1];
	for(i=lp1-2,j=lp2-2;j>=2;i--,j--) p1[i]=subllx(y[i],p2[j]);
      }
      if(overflow)
      {
	for(;(i>=2)&&(!y[i]);i--) p1[i]=(long)MAXULONG;
	p1[i]=y[i]-1;while(i>=3) {i--;p1[i]=y[i];}
      }
      else for(;i>=2;i--) p1[i]=y[i];
    }
    else
    {
      overflow=0;
      if(m+flag) for(i=lp1-1;i>=2;i--) p1[i]=subllx(p2[i],y[i]);
      else 
      {
	p1[lp1-1]=subllx(0,y[lp1-1]);
	for(i=lp1-2;i>=2;i--) p1[i]=subllx(p2[i],y[i]);
      }
    }
    for(i=2;!p1[i];i++);j=i-2;avma=av+(j<<TWOPOTBYTES_IN_LONG);p1[j]=p1[0]-j;p1+=j;
    m=bfffo(p1[2]);e=ey-(j<<TWOPOTBITS_IN_LONG)-m+HIGHEXPOBIT;
    if(e<0) err(adder5);
    p1[1]=f2 ? evalsigne(sy)+e : evalsigne(sx)+e;
    if(m)
    {
      k=0;for(i=lp1-1-j;i>=2;i--) {p1[i]=shiftl(p1[i],m)+k;k=hiremainder;}
    }
    return p1;
  }
}

GEN
PariSubDown(GEN x, GEN y)
{
  if(typ(x)==1) return (typ(y)==1) ? subii(x,y) : PariSubirDown(x,y);
  return (typ(y)==1) ? PariSubriDown(x,y) : PariSubrrDown(x,y);
}

GEN
PariSubrrDown(GEN x, GEN y)
{
  long s=signe(y);
  GEN z;
  
  if(x==y)
  {
    z=cgetr(3);z[2]=0;z[1]=HIGHEXPOBIT-(lg(x)<<TWOPOTBITS_IN_LONG);return z;
  }
  setsigne(y,-s);z=PariAddrrDown(x,y);setsigne(y,s);return z;
}

GEN
PariSubirDown(GEN x, GEN y)
{
  long s=signe(y);
  GEN z;
  
  setsigne(y,-s);z=PariAddirDown(x,y);setsigne(y,s);return z;
}

GEN
PariSubriDown(GEN x, GEN y)
{
  long s=signe(y);
  GEN z;
  
  setsigne(y,-s);z=PariAddirDown(y,x);setsigne(y,s);return z;
}

GEN
PariMulDown(GEN x, GEN y)
{
  if(typ(x)==1) return (typ(y)==1) ? mulii(x,y) : PariMulirDown(x,y);
  return (typ(y)==1) ? PariMulirDown(y,x) : PariMulrrDown(x,y);
}

GEN
PariMulrrDown(GEN x, GEN y)
{
  long i,j,lx=lg(x),ly=lg(y),sx=signe(x),sy=signe(y),ex=expo(x),ey=expo(y);
  long e,flag,garde,p1,p2,lz;
  GEN z;
  
  e=ex+ey+HIGHEXPOBIT;if(e>=EXPOBITS) err(muler4);
  if(e<0) err(muler5);
  if((!sx)||(!sy)) {z=cgetr(3);z[2]=0;z[1]=e;return z;}
  if(sy<0) sx= -sx;
  if(lx>ly) {lz=ly;z=x;x=y;y=z;flag=1;} else {lz=lx;flag=(lx!=ly);}
  z=cgetr(lz);z[1]=evalsigne(sx)+e;
  if(flag) mulll(x[2],y[lz]);else hiremainder=0;
  if(lz==3)
  {
    garde=flag ? addmul(x[2],y[2]) : mulll(x[2],y[2]);
    if((long)hiremainder<0) {z[2]=hiremainder;z[1]++;}
    else {z[2]=(garde<0)?(hiremainder<<1)+1:(hiremainder<<1);}
    return z;
  }
  p1=x[lz-1];garde=hiremainder;
  if(p1)
  {
    mulll(p1,y[3]);p2=addmul(p1,y[2]);
    garde=addll(p2,garde);z[lz-1]=overflow+hiremainder;
  }
  else z[lz-1]=0;
  for(j=lz-2;j>=3;j--)
  {
    p1=x[j];
    if(p1)
    {
      mulll(p1,y[lz+2-j]);
      p2=addmul(p1,y[lz+1-j]);
      garde=addll(p2,garde);hiremainder+=overflow;
      for(i=lz-j;i>=2;i--)
      {
	p2=addmul(p1,y[i]);
	z[i+j-1]=addll(p2,z[i+j-1]);hiremainder+=overflow;
      }
      z[j]=hiremainder;
    }
    else z[j]=0;
  }
  p1=x[2];p2=mulll(p1,y[lz-1]);
  garde=addll(p2,garde);hiremainder+=overflow;
  for(i=lz-2;i>=2;i--)
  {
    p2=addmul(p1,y[i]);
    z[i+1]=addll(p2,z[i+1]);hiremainder+=overflow;
  }
  z[2]=hiremainder;
  if((long)hiremainder>0)
  {
    overflow=(garde<0)?1:0;
    for(i=lz-1;i>=2;i--) {p1=z[i];z[i]=addllx(p1,p1);}
  }
  else z[1]++;
  return z;
}

GEN
PariMulirDown(GEN x, GEN y)
{
  long sx=signe(x),sy,av,lz,ey,e,garde,p1,p2,i,j;
  GEN z,temp;
  
  if(!sx) return gzero;
  sy=signe(y);ey=expo(y);
  if(!sy)
  {
    z=cgetr(3);z[2]=0;e=expi(x)+ey+HIGHEXPOBIT;if(e>EXPOBITS) err(muler6);
    z[1]=e;return z;
  }
  lz=lg(y);if(sy<0) sx= -sx;
  z=cgetr(lz);av=avma;
  temp=cgetr(lz+1);affir(x,temp);x=y;y=temp;
  e=expo(y)+ey+HIGHEXPOBIT;if(e>=EXPOBITS) err(muler4);
  if(e<0) err(muler5);
  z[1]=evalsigne(sx)+e;
  mulll(x[2],y[lz]);
  if(lz==3)
  {
    garde=addmul(x[2],y[2]);
    if((long)hiremainder<0) {z[2]=hiremainder;z[1]++;}
    else {z[2]=(garde<0)?(hiremainder<<1)+1:(hiremainder<<1);}
    avma=av;return z;
  }
  garde=hiremainder;
  p1=x[lz-1];mulll(p1,y[3]);p2=addmul(p1,y[2]);
  garde=addll(p2,garde);z[lz-1]=overflow+hiremainder;
  for(j=lz-2;j>=3;j--)
  {
    p1=x[j];mulll(p1,y[lz+2-j]);
    p2=addmul(p1,y[lz+1-j]);
    garde=addll(p2,garde);hiremainder+=overflow;
    for(i=lz-j;i>=2;i--)
    {
      p2=addmul(p1,y[i]);
      z[i+j-1]=addll(p2,z[i+j-1]);hiremainder+=overflow;
    }
    z[j]=hiremainder;
  }
  p1=x[2];p2=mulll(p1,y[lz-1]);
  garde=addll(p2,garde);hiremainder+=overflow;
  for(i=lz-2;i>=2;i--)
  {
    p2=addmul(p1,y[i]);
    z[i+1]=addll(p2,z[i+1]);hiremainder+=overflow;
  }
  z[2]=hiremainder;
  if((long)hiremainder>0)
  {
    overflow=(garde<0)?1:0;
    for(i=lz-1;i>=2;i--) {p1=z[i];z[i]=addllx(p1,p1);}
  }
  else z[1]++;
  avma=av;return z;
}

#ifdef LONG_IS_32BIT
#define DIVCONVI 14
#endif

#ifdef LONG_IS_64BIT
#define DIVCONVI 7
#endif

GEN
PariDivDown(GEN x, GEN y)
{
  if(typ(x)==1) return (typ(y)==1) ? divii(x,y) : PariDivirDown(x,y);
  return (typ(y)==1) ? PariDivriDown(x,y) : PariDivrrDown(x,y);
}

GEN
PariDivirDown(GEN x, GEN y)
{
  GEN xr,z;
  long av,ly;
  
  if(!signe(y)) err(diver5);
  if(!signe(x)) return gzero;
  ly=lg(y);z=cgetr(ly);av=avma;affir(x,xr=cgetr(ly+1));
  xr=PariDivrrDown(xr,y);affrr(xr,z);avma=av;return z;
}

GEN
PariDivriDown(GEN x, GEN y)
{
  GEN yr,z;
  long av,lx,ex,s=signe(y);

  if(!s) err(diver8);
  if(!signe(x))
  {
    ex=expo(x)-expi(y)+HIGHEXPOBIT;
    if(ex<0) err(diver12);
    z=cgetr(3);z[1]=ex;z[2]=0;return z;
  }
  if((lg(y)==3)&&(y[2]>0)) return (s>0) ? divrs(x,y[2]) : divrs(x,-y[2]);
  lx=lg(x);z=cgetr(lx);av=avma;affir(y,yr=cgetr(lx+1));
  yr=PariDivrrDown(x,yr);affrr(yr,z);avma=av;return z;
}

GEN
PariDivrrDown(GEN x, GEN y)
{
  long sx=signe(x),sy=signe(y),lx,ly,lz,ex,ex1,i,z0;
  ulong ldif,y0,y1,si,saux,qp,k,k3,k4,j;
  GEN z;
  if(!sy) err(diver9);
  ex=expo(x)-expo(y)+HIGHEXPOBIT;
  if(ex<=0) err(diver10);
  if(ex>EXPOBITS) err(diver11);
  if(!sx)
  {
    z=cgetr(3);z[1]=ex;z[2]=0;return z;
  }
  lx=lg(x);ly=lg(y);lz=(lx<=ly)?lx:ly;
  z=cgetr(lz);if(sy<0) sx= -sx;
  ex1=evalsigne(sx)+ex;
  if(ly==3)
  {
    i=x[2];si=(lx>3)?x[3]:0;
    if((ulong)i<(ulong)y[2])
    {
      hiremainder=i;z[2]=divll(si,y[2]);
      z[1]=ex1-1;return z;
    }
    else
    {
      hiremainder=((ulong)i)>>1;
      z[2]=(i&1)?divll((((ulong)si)>>1)|(HIGHBIT),y[2]):divll(((ulong)si)>>1,y[2]);
      z[1]=ex1;return z;
    }
  }
  z0= *z;*z=0;
  for(i=2;i<=lz-1;i++) z[i-1]=x[i];
  z[lz-1]=(lx>lz) ? x[lz] : 0;
  ldif=ly-lz;if(!ldif) {y0=y[lz];y[lz]=0;}
  if(ldif<=1) {y1=y[lz+1];y[lz+1]=0;}
  si=y[2];saux=y[3];
  for(i=0;i<lz-1;i++)
  {
    if(z[i]!=si)
    {
      if((ulong)z[i]>si)
      {
	overflow=0;
	for(j=lz-i+1;j>=2;j--) z[i+j-2]=subllx(z[i+j-2],y[j]);
	{z[i-1]++;for(j=i-1;j&&(!z[j]);j--) z[j-1]++;}
      }
      hiremainder=z[i];qp=divll(z[i+1],si);
      overflow=0;k=hiremainder;
    }
    else
    {
      qp=(long)MAXULONG;k=addll(si,z[i+1]);
    }
    if(!overflow)
    {
      k3=subll(mulll(qp,saux),z[i+2]);k4=subllx(hiremainder,k);
      while((!overflow)&&k4) {qp--;k3=subll(k3,saux);k4=subllx(k4,si);}
    }
    mulll(qp,y[lz+1-i]);
    for(j=lz-i;j>=2;j--)
    {
      z[i+j-1]=subll(z[i+j-1],addmul(qp,y[j]));hiremainder+=overflow;
    }
    if((ulong)z[i]!=(ulong)hiremainder)
    {
      if((ulong)z[i]<(ulong)hiremainder)
      {
	overflow=0;qp--;
	for(j=lz-i;j>=2;j--) z[i+j-1]=addllx(z[i+j-1],y[j]);
      }
      else
      {
	z[i]-=hiremainder;
	while(z[i])
	{
	  overflow=0;qp++;
	  if(!qp) {z[i-1]++;for(j=i-1;j&&(!z[j]);j--) z[j-1]++;}
	  for(j=lz-i;j>=2;j--) z[i+j-1]=subllx(z[i+j-1],y[j]);
	  z[i]-=overflow;
	}
      }
    }
    z[i]=qp;
  }
  if(!ldif) y[lz]=y0;if(ldif<=1) y[lz+1]=y1;
  for(j=lz-1;j>=2;j--) z[j]=z[j-1];
  if(*z)
  {
    k=HIGHBIT;
    for(j=2;j<lz;j++) {z[j]=shiftlr(z[j],1)+k;k=hiremainder;}
  }
  else ex1--;
  z[1]=ex1;*z=z0;return z;
}

