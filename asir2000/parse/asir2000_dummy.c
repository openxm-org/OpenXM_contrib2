#if defined(VISUAL) || defined(__MINGW32__) || defined(__MINGW64__)
#include <stdio.h>
#include <stdarg.h>
int w_printf(char *format, ...)
{
    int ret;
    va_list ap;
    va_start(ap, format);
    ret = vprintf(format, ap);
    va_end(ap);
    return ret;
}
int w_fprintf(FILE *fp, char *format, ...)
{
    int ret;
    va_list ap;
    va_start(ap, format);
    ret = vfprintf(fp, format, ap);
    va_end(ap);
    return ret;
}
int w_fflush(FILE *fp)
{
    return fflush(fp);
}
int w_fgetc(FILE *fp)
{
    return fgetc(fp);
}
char *w_fgets(char *s, int n, FILE *fp)
{
    return fgets(s, n, fp);
}
int w_fputc(int c, FILE *fp)
{
    return fputc(c, fp);
}
int w_fputs(char *s, FILE *fp)
{
    return fputs(s, fp);
}
int w_ungetc(int c, FILE *fp)
{
    return ungetc(c, fp);
}
void send_progress(short per,char *msg)
{
}
void SendHeapSize()
{
}
#endif
