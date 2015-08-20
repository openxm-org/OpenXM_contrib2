#if !defined(__IF_CANVAS_H__)
#define __IF_CANVAS_H__

#include <windows.h>

void canvas_toggle_noaxis(struct canvas *can);
void canvas_toggle_wide(struct canvas *can);
void canvas_toggle_precise(struct canvas *can);
int canvas_percentage(struct canvas *can);
int canvas_height(struct canvas *can);
int canvas_width(struct canvas *can);
int canvas_index(struct canvas *can);
char canvas_precise(struct canvas *can);
char canvas_wide(struct canvas *can);
char canvas_noaxis(struct canvas *can);
char *canvas_prefix(struct canvas *can);
char *canvas_wname(struct canvas *can);
void *canvas_window(struct canvas *can);
double canvas_xmin(struct canvas *can);
double canvas_xmax(struct canvas *can);
double canvas_ymin(struct canvas *can);
double canvas_ymax(struct canvas *can);
HDC canvas_pix(struct canvas *can);
struct pa* canvas_pa(struct canvas *can);
void canvas_bitblt(struct canvas *can, HDC dest,int dx, int dy);
int canvas_is_plot(struct canvas *can);
int canvas_is_ifplot(struct canvas *can);
int canvas_is_interactive(struct canvas *can);
void canvas_set_handle(struct canvas *can, HWND hwnd);
void canvas_set_window(struct canvas *can, void *window);
void **canvas_history(struct canvas *can, int *len);

#endif
