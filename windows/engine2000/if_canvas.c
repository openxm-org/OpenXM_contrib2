#include "gmp.h"
#include "ca.h"
#include <windows.h>
#include "ifplot.h"
#include "if_canvas.h"

void canvas_toggle_noaxis(struct canvas *can)
{
	can->noaxis = !can->noaxis;
}

void canvas_toggle_wide(struct canvas *can)
{
	can->wide = !can->wide;
}

void canvas_toggle_precise(struct canvas *can)
{
	can->precise = !can->precise;	
}

int canvas_percentage(struct canvas *can)
{
    return can->percentage;
}

int canvas_height(struct canvas *can)
{
    return can->height;
}

int canvas_width(struct canvas *can)
{
    return can->width;
}

int canvas_index(struct canvas *can)
{
    return can->index;
}

char canvas_precise(struct canvas *can)
{
    return can->precise;
}

char canvas_wide(struct canvas *can)
{
    return can->wide;
}

char canvas_noaxis(struct canvas *can)
{
    return can->noaxis;
}

char *canvas_prefix(struct canvas *can)
{
    return can->prefix;
}

char *canvas_wname(struct canvas *can)
{
    return can->wname;
}

void *canvas_window(struct canvas *can)
{
    return can->window;
}

double canvas_xmin(struct canvas *can)
{
	return can->xmin;
}

double canvas_xmax(struct canvas *can)
{
	return can->xmax;
}

double canvas_ymin(struct canvas *can)
{
	return can->ymin;
}

double canvas_ymax(struct canvas *can)
{
	return can->ymax;
}

HDC canvas_pix(struct canvas *can)
{
    return can->pix;
}

struct pa* canvas_pa(struct canvas *can)
{
	return can->pa;
}

void canvas_bitblt(struct canvas *can, HDC dest,int dx, int dy)
{
    BitBlt(dest,dx,dy,can->width,can->height,can->pix,0,0,SRCCOPY);
}

int canvas_is_plot(struct canvas *can)
{
    int mode = can->mode;
    return mode == modeNO(PLOT);
}

int canvas_is_ifplot(struct canvas *can)
{
    int mode = can->mode;
    return mode == modeNO(IFPLOT) || mode == modeNO(CONPLOT);
}

int canvas_is_interactive(struct canvas *can)
{
    int mode = can->mode;
    return mode == modeNO(INTERACTIVE);
}

void canvas_set_handle(struct canvas *can, HWND hwnd)
{
    can->hwnd = hwnd;
}

void canvas_set_window(struct canvas *can, void *window)
{
    can->window= window;
}

void **canvas_history(struct canvas *can, int *len)
{
    void **p = NULL;
    int i;
    NODE n;
    for ( i=0, n=can->history; n; i++, n=NEXT(n) ) {
    }
    *len = i;
    if( *len ) {
		p = (void **)MALLOC(sizeof(void *)*(*len));
		for ( i=0, n=can->history; n; i++, n=NEXT(n) ) {
			p[i] = n->body;
		}
	}
    return p;
}
