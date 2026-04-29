#include "ca.h"
#include "parse.h"
#include "ox.h"
#include "ifplot.h"

#define MEMORY_DRAWPOINT(a,len,x,y) (((a)[(len)*(y)+((x)>>3)]) |= (1<<((x)&7)))

void memory_if_print(double **tab,struct canvas *can,BYTEARRAY *bytes){
  int ix,iy,width,height;
  double *px,*px1,*px2;
  unsigned char *array;
  int scan_len;

  if ( can->mode==modeNO(CONPLOT) ) {
    memory_con_print(tab,can,bytes);
    return;
  }

  width = can->width;
  height = can->height;
  scan_len = (width+7)/8;
  MKBYTEARRAY(*bytes,scan_len*height);
  array = BDY(*bytes);
  for( ix=0; ix<width-1; ix++ )
    for(iy=0, px=tab[ix], px1 = tab[ix+1], px2 = px+1;
      iy<height-1 ;iy++, px++, px1++, px2++ )
      if ( ((*px >= 0) && ((*px1 <= 0) || (*px2 <= 0))) ||
         ((*px <= 0) && ((*px1 >= 0) || (*px2 >= 0))) ) {
        MEMORY_DRAWPOINT(array,scan_len,ix,height-iy-1);
      }
}

void memory_con_print(double **tab,struct canvas *can,BYTEARRAY *bytes){
  int ix,iy,iz,pas,pai,len;
  double zstep,z;
  double *px,*px1,*px2;
  POINT *pa,*pa1;
  struct pa *parray;
  unsigned char *array;
  int scan_len;

  zstep = (can->zmax-can->zmin)/can->nzstep;
  can->pa = (struct pa *)MALLOC((can->nzstep+1)*sizeof(struct pa));
  pas = can->width;
  pa = (POINT *)ALLOCA(pas*sizeof(POINT));
  for ( z = can->zmin, iz = 0; z <= can->zmax; z += zstep, iz++ ) {
    pai = 0;
    for( ix=0; ix<can->width-1; ix++ )
      for(iy=0, px=tab[ix], px1 = tab[ix+1], px2 = px+1;
        iy<can->height-1 ;iy++, px++, px1++, px2++ )
        if ( ((*px >= z) && ((*px1 <= z) || (*px2 <= z))) ||
            ((*px <= z) && ((*px1 >= z) || (*px2 >= z))) ) {
          if ( pai == pas ) {
            pa1 = (POINT *)ALLOCA(2*pas*sizeof(POINT));
            bcopy((char *)pa,(char *)pa1,pas*sizeof(POINT));
            pa = pa1;
            pas += pas;
          }
          XC(pa[pai]) = ix;
          YC(pa[pai]) = can->height-iy-1;
          pai++;
        }
    can->pa[iz].length = pai;
    if ( pai ) {
      pa1 = (POINT *)MALLOC(pai*sizeof(POINT));
      bcopy((char *)pa,(char *)pa1,pai*sizeof(POINT));
      can->pa[iz].pos = pa1;
    }
  }

  scan_len = (can->width+7)/8;
  MKBYTEARRAY(*bytes,scan_len*can->height);
  array = BDY(*bytes);
  for ( parray = can->pa, iz = 0; iz <= can->nzstep; iz++, parray++ )
    for ( pa = parray->pos, len = parray->length, ix = 0; ix < len; ix++ ) {
      MEMORY_DRAWPOINT(array,scan_len,XC(pa[ix]),YC(pa[ix]));
    }
}

void memory_print(struct canvas *can,BYTEARRAY *bytes){
  int len,scan_len,i;
  POINT *pa;
  char *array;

  scan_len = (can->width+7)/8;
  MKBYTEARRAY(*bytes,scan_len*can->height);
  array = (char *)BDY(*bytes);

  len = can->pa[0].length;
  pa = can->pa[0].pos;
  for ( i = 0; i < len; i++ ) {
    MEMORY_DRAWPOINT(array,scan_len,pa[i].x,pa[i].y);
  }
}
