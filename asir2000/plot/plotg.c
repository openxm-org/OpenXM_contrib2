/* $OpenXM: OpenXM/src/asir99/plot/plotg.c,v 1.1.1.1 1999/11/10 08:12:34 noro Exp $ */
#include "ca.h"
#include "parse.h"
#include "ox.h"
#include "ifplot.h"

int stream;

DISPLAY *display;
CURSOR normalcur,runningcur,errorcur;

Window rootwin;
GC drawGC,dashGC,hlGC,scaleGC,clearGC,xorGC,colorGC;
XFontStruct *sffs;

struct canvas *canvas[MAXCANVAS];
struct canvas *current_can;
