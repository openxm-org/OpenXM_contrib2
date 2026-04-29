#include "ca.h"
#include "parse.h"
#include "ox.h"
#include "ifplot.h"

#include <string.h>

static void build_bitmap_result(int width,int height,BYTEARRAY barray,LIST *bytes)
{
  NODE n;
  Z qw,qh;

  STOZ(width,qw);
  STOZ(height,qh);
  n = mknode(3,qw,qh,barray);
  MKLIST(*bytes,n);
}

int memory_plot(NODE arg,LIST *bytes){
  NODE n;
  struct canvas tmp_can;
  struct canvas *can;
  P formula;
  LIST xrange,yrange,zrange,wsize;
  double **tabe;
  int i;
  BYTEARRAY barray;

  formula=(P)ARG0(arg);
  xrange=(LIST)ARG1(arg);
  yrange=(LIST)ARG2(arg);
  zrange=(LIST)ARG3(arg);
  wsize=(LIST)ARG4(arg);

  memset(&tmp_can,0,sizeof(tmp_can));
  can=&tmp_can;
  n=BDY(xrange); can->vx=VR((P)BDY(n)); n=NEXT(n);
  can->qxmin=(Q)BDY(n); n=NEXT(n); can->qxmax=(Q)BDY(n);
  can->xmin=ToReal(can->qxmin); can->xmax=ToReal(can->qxmax);
  if(yrange){
    n=BDY(yrange); can->vy=VR((P)BDY(n)); n=NEXT(n);
    can->qymin=(Q)BDY(n); n=NEXT(n); can->qymax=(Q)BDY(n);
    can->ymin=ToReal(can->qymin); can->ymax=ToReal(can->qymax);
    if(zrange){
      n=NEXT(BDY(zrange));
      can->zmin=ToReal(BDY(n)); n=NEXT(n); can->zmax=ToReal(BDY(n));
      n=NEXT(n);
      can->nzstep=n?ZTOS((Q)BDY(n)):MAXGC;
      can->mode=modeNO(CONPLOT);
    } else
      can->mode=modeNO(IFPLOT);
  } else
    can->mode=modeNO(PLOT);

  if(!wsize){
    can->width=DEFAULTWIDTH;
    can->height=DEFAULTHEIGHT;
  } else {
    can->width=ZTOS((Q)BDY(BDY(wsize)));
    can->height=ZTOS((Q)BDY(NEXT(BDY(wsize))));
  }
  can->formula=formula;

  if(can->mode==modeNO(PLOT)){
    can->prec=argc(arg)==6 ? ZTOS((Q)ARG5(arg)) : 0;
    plotcalc(can);
    memory_print(can,&barray);
  } else {
    tabe=(double **)ALLOCA(can->width*sizeof(double *));
    for(i=0;i<can->width;i++)
      tabe[i]=(double *)ALLOCA(can->height*sizeof(double));
    calc(tabe,can,1);
    memory_if_print(tabe,can,&barray);
  }
  build_bitmap_result(can->width,can->height,barray,bytes);
  return 0;
}

int memory_polarplot(NODE arg,LIST *bytes){
  NODE n;
  struct canvas tmp_can;
  struct canvas *can;
  P formula;
  LIST zrange,wsize;
  BYTEARRAY barray;

  formula=(P)ARG0(arg);
  zrange=(LIST)ARG3(arg);
  wsize=(LIST)ARG4(arg);

  memset(&tmp_can,0,sizeof(tmp_can));
  can=&tmp_can;
  n=BDY(zrange); can->vx=VR((P)BDY(n)); n=NEXT(n);
  can->zmin=ToReal(BDY(n)); n=NEXT(n); can->zmax=ToReal(BDY(n));
  n=NEXT(n);
  can->nzstep=n?ZTOS((Q)BDY(n)):DEFAULTPOLARSTEP;

  if(!wsize){
    can->width=DEFAULTWIDTH;
    can->height=DEFAULTHEIGHT;
  } else {
    can->width=ZTOS((Q)BDY(BDY(wsize)));
    can->height=ZTOS((Q)BDY(NEXT(BDY(wsize))));
  }
  can->formula=formula;
  can->mode=modeNO(POLARPLOT);

  polarcalc(can);
  memory_print(can,&barray);
  build_bitmap_result(can->width,can->height,barray,bytes);
  return 0;
}
