/*
 * $OpenXM: OpenXM_contrib2/asir2018/builtin/mattran.c,v 1.1 2018/09/19 05:45:06 noro Exp $
 */

#include "ca.h"
#include "parse.h"

static void MatFund(NODE, Obj *);
static void RowMul(NODE, Obj *);
static void RowExh(NODE, Obj *);
static void RowMulAdd(NODE, Obj *);
static void BodyRowMul(MAT, int, pointer);
static void BodyRowExh(MAT, int, int);
static void BodyRowMulAdd(MAT, int, int, pointer);
static void ColMul(NODE, Obj *);
static void ColExh(NODE, Obj *);
static void ColMulAdd(NODE, Obj *);
static void BodyColMul(MAT, int, pointer);
static void BodyColExh(MAT, int, int);
static void BodyColMulAdd(MAT, int, int, pointer);

struct ftab mat_tab[] = {
  {"mfund",MatFund,-5},
  {"rowm",RowMul,3},
  {"rowx",RowExh,3},
  {"rowa",RowMulAdd,4},
  {"colm",ColMul,3},
  {"colx",ColExh,3},
  {"cola",ColMulAdd,4},
  {0,0,0},
};

static void
MatFund(arg, rp)
NODE arg;
Obj *rp;
{
  MAT mat;
  pointer *coe;
  Q rowq, row2q, colq, col2q;
  int sw, row, row2, col, col2;
  sw = ZTOS( (Q)ARG0(arg) );
  mat = (MAT)ARG1(arg);
  switch (sw) {
    case 1:
      rowq = (Q)ARG2(arg);
      row = ZTOS( rowq );
      coe = (pointer)ARG3(arg);
      BodyRowMul( mat, row, coe );
      *rp = (Obj)mat;
      return;
    case 2:
      rowq = (Q)ARG2(arg);
      row = ZTOS( rowq );
      row2q = (Q)ARG3(arg);
      row2 = ZTOS( row2q );
      BodyRowExh( mat, row, row2 );
      *rp = (Obj)mat;
      return;
    case 3:
      rowq = (Q)ARG2(arg);
      row = ZTOS( rowq );
      row2q = (Q)ARG3(arg);
      row2 = ZTOS( row2q );
      coe = (pointer)ARG3(arg);
      BodyRowMulAdd( mat, row, row2, coe );
      *rp = (Obj)mat;
      return;
    case 4:
      colq = (Q)ARG2(arg);
      col = ZTOS( colq );
      coe = (pointer)ARG3(arg);
      BodyColMul( mat, col, coe );
      *rp = (Obj)mat;
      return;
    case 5:
      colq = (Q)ARG2(arg);
      col = ZTOS( colq );
      col2q = (Q)ARG3(arg);
      col2 = ZTOS( col2q );
      BodyColExh( mat, col, col2 );
      *rp = (Obj)mat;
      return;
    case 6:
      colq = (Q)ARG2(arg);
      col = ZTOS( colq );
      col2q = (Q)ARG3(arg);
      col2 = ZTOS( col2q );
      coe = (pointer)ARG3(arg);
      BodyColMulAdd( mat, col, col2, coe );
      *rp = (Obj)mat;
    default:
      return;
      *rp = 0;
  }
}

static void
RowMul(arg, rp)
NODE arg;
Obj *rp;
{
  MAT mat;
  Q rowq;
  pointer *coe;
  int row;

  mat = (MAT)ARG0(arg);
  rowq = (Q)ARG1(arg);
  row = ZTOS( rowq );
  coe = (pointer)ARG2(arg);
  BodyRowMul( mat, row , coe );
  *rp = (Obj)mat;
}

static void
RowExh(arg, rp)
NODE arg;
Obj *rp;
{
  MAT mat;
  Q QIndexA, QIndexB;
  int IndexA, IndexB;

  mat = (MAT)ARG0(arg);
  QIndexA = (Q)ARG1(arg);
  QIndexB = (Q)ARG2(arg);
  IndexA = ZTOS( QIndexA );
  IndexB = ZTOS( QIndexB );
  BodyRowExh( mat, IndexA, IndexB );
  *rp = (Obj)mat;
}

static void
RowMulAdd(arg, rp)
NODE arg;
Obj *rp;
{
  MAT mat;
  Q QIndexA, QIndexB;
  int IndexA, IndexB;
  pointer *coe;

  mat = (MAT)ARG0(arg);
  QIndexA = (Q)ARG1(arg);
  QIndexB = (Q)ARG2(arg);
  coe = (pointer)ARG3(arg);
  IndexA = ZTOS( QIndexA );
  IndexB = ZTOS( QIndexB );
  BodyRowMulAdd( mat, IndexA, IndexB, coe );
  *rp = (Obj)mat;
}

static void
ColMul(arg, rp)
NODE arg;
Obj *rp;
{
  MAT mat;
  Q QIndex;
  pointer *coe;
  int Index;

  mat = (MAT)ARG0(arg);
  QIndex = (Q)ARG1(arg);
  Index = ZTOS( QIndex );
  coe = (pointer)ARG2(arg);
  BodyColMul( mat, Index , coe );
  *rp = (Obj)mat;
}

static void
ColExh(arg, rp)
NODE arg;
Obj *rp;
{
  MAT mat;
  Q QIndexA, QIndexB;
  int IndexA, IndexB;

  mat = (MAT)ARG0(arg);
  QIndexA = (Q)ARG1(arg);
  QIndexB = (Q)ARG2(arg);
  IndexA = ZTOS( QIndexA );
  IndexB = ZTOS( QIndexB );
  BodyColExh( mat, IndexA, IndexB );
  *rp = (Obj)mat;
}

static void
ColMulAdd(arg, rp)
NODE arg;
Obj *rp;
{
  MAT mat;
  Q QIndexA, QIndexB;
  int IndexA, IndexB;
  pointer *coe;

  mat = (MAT)ARG0(arg);
  QIndexA = (Q)ARG1(arg);
  QIndexB = (Q)ARG2(arg);
  coe = (pointer)ARG3(arg);
  IndexA = ZTOS( QIndexA );
  IndexB = ZTOS( QIndexB );
  BodyColMulAdd( mat, IndexA, IndexB, coe );
  *rp = (Obj)mat;
}

static void
BodyRowMul( mat, row, coe )
MAT mat;
int row;
pointer coe;
{
  int size, i;
  pointer *t, *matrow;

  size = mat->col;
  matrow = BDY(mat)[row];
  for ( i = 0; i < size; i++ ) {
    mulr(CO,(Obj)matrow[i],(Obj)coe,(Obj *)&t);
    matrow[i]=(Obj)t;
  }
}

static void
BodyRowExh( mat, IndexA, IndexB )
MAT mat;
int IndexA, IndexB;
{
  int i, size;
  pointer *t, *PRowA, *PRowB;

  size = mat->col;
  PRowA = BDY(mat)[IndexA];
  PRowB = BDY(mat)[IndexB];
  for ( i = 0; i < size; i++ ) {
    t = PRowA[i];
    PRowA[i] = PRowB[i];
    PRowB[i] = t;
  }
}

static void
BodyRowMulAdd( mat, IndexA, IndexB, coe )
MAT mat;
int IndexA, IndexB;
pointer coe;
{
  int i, size;
  pointer *t, *PRowA, *PRowB;

  size = mat->col;
  PRowA = BDY(mat)[IndexA];
  PRowB = BDY(mat)[IndexB];

  for ( i = 0; i < size; i++ ) {
    mulr( CO, (Obj)PRowB[i], (Obj)coe, (Obj *)&t );
    addr( CO, (Obj)PRowA[i], (Obj)t, (Obj *)&t );
    PRowA[i] = t;
  }
}

static void
BodyColMul( mat, Index, coe )
MAT mat;
int Index;
pointer coe;
{
  int size, i;

  size = mat->row;
  for ( i = 0; i < size; i++ ) {
    mulr(CO, BDY(mat)[i][Index], (Obj)coe, (Obj *)&BDY(mat)[i][Index]);
  }
}

static void
BodyColExh( mat, IndexA, IndexB )
MAT mat;
int IndexA, IndexB;
{
  int i, size;
  pointer *t;

  size = mat->row;
  for ( i = 0; i < size; i++ ) {
    t = BDY(mat)[i][IndexA];
    BDY(mat)[i][IndexA] = BDY(mat)[i][IndexB];
    BDY(mat)[i][IndexB] = t;
  }
}

static void
BodyColMulAdd( mat, IndexA, IndexB, coe )
MAT mat;
int IndexA, IndexB;
pointer coe;
{
  int i, size;
  pointer *t;

  size = mat->row;
  for ( i = 0; i < size; i++ ) {
    mulr( CO, BDY(mat)[i][IndexB], coe, (Obj *)&t );
    addr( CO, BDY(mat)[i][IndexA], (Obj)t, (Obj *)&BDY(mat)[i][IndexA]);
  }
}
