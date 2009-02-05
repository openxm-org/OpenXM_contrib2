#include "gc.h"
#include <time.h>

void error(char *);

int *StackBottom;

void *Risa_GC_malloc(size_t d)
{
	void *ret;

	ret = (void *)GC_malloc(d);
	if ( !ret )
		error("GC_malloc : failed to allocate memory");
	return ret;
}

void *Risa_GC_malloc_atomic(size_t d)
{
	void *ret;

	ret = (void *)GC_malloc_atomic(d);
	if ( !ret )
		error("GC_malloc_atomic : failed to allocate memory");
	return ret;
}

void *Risa_GC_realloc(void *p,size_t d)
{
	void *ret;

	ret = (void *)GC_realloc(p,d);
	if ( !ret )
		error("GC_realloc : failed to reallocate memory");
	return ret;
}

int get_heapsize()
{
	return GC_get_heap_size();
}

#if !defined(BYTES_TO_WORDS)
#define BYTES_TO_WORDS(x)   ((x)>>2)
#endif

long get_allocwords()
{
	size_t n = GC_get_total_bytes();
	return (long)BYTES_TO_WORDS(n); /* bytes to words */
}

double gctime;
static double gcstart,asir_start_time;

double get_clock(), get_rtime(), get_current_time();

void rtime_init()
{
#if defined(i386) && defined(linux)
	unsigned short cw;

#define fldcw(addr)             __asm("fldcw %0" : : "m" (*addr))
#define fnstcw(addr)            __asm("fnstcw %0" : "=m" (*addr) : "0" (*addr))
	fnstcw(&cw); cw &= 0xfeff; cw |= 0x0200; fldcw(&cw);
#endif
	asir_start_time = get_current_time();
}

double get_rtime()
{
	return get_current_time() - asir_start_time;
}

#if defined(THINK_C) || defined(__MWERKS__) || defined(VISUAL) || defined(MSWIN32)

#if defined(VISUAL)
#include <windows.h>

extern int recv_intr,doing_batch;
void send_intr();

BOOL set_ctrlc_flag(DWORD type)
{
	if ( doing_batch )
		send_intr();
	else
		recv_intr = 1;
	return TRUE;
}

void register_ctrlc_handler() {
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)set_ctrlc_flag,TRUE);
}

int mythreadid() {
	return GetCurrentThreadId();
}

double get_current_time()
{
//	return (double)clock()/(double)CLOCKS_PER_SEC;
	return ((double)GetTickCount())/1000.0;
}

double get_clock()
{
	static int initialized = 0;
	static int is_winnt = 0;
	static HANDLE curproc;

	if ( !initialized ) {
		OSVERSIONINFO vinfo;

		curproc = GetCurrentProcess();		
		vinfo.dwOSVersionInfoSize = sizeof(vinfo);
		GetVersionEx(&vinfo);
		if ( vinfo.dwPlatformId == VER_PLATFORM_WIN32_NT )
			is_winnt = 1;
		else
			is_winnt = 0;
	}
	if ( is_winnt ) {
		FILETIME c,e,k,u;

		GetProcessTimes(curproc,&c,&e,&k,&u);
		return ((double)k.dwLowDateTime+(double)u.dwLowDateTime
			+4294967296.0*((double)k.dwHighDateTime+(double)u.dwHighDateTime))/10000000.0;
	} else
		return get_current_time();
}
#else
double get_current_time()
{
	return get_clock();
}

double get_clock()
{
	clock_t c;

	c = clock();
	return (double)c/(double)CLOCKS_PER_SEC;
}
#endif

#else
#include <sys/time.h>

double get_current_time()
{
	struct timeval t;
	struct timezone z;

	gettimeofday(&t,&z);
	return (double)t.tv_sec + ((double)t.tv_usec)/((double)1000000);
}

#if defined(_PA_RISC1_1) || defined(__svr4__) || defined(__CYGWIN__)

#include <sys/times.h>
#include <limits.h>

double get_clock()
{
	struct tms buf;

	times(&buf);
	return (double)(buf.tms_utime+buf.tms_stime)/(double)CLK_TCK;
}
#else

#include <sys/time.h>
#include <sys/resource.h>

double get_clock()
{
	int tv_sec,tv_usec;
	struct rusage ru;

	getrusage(RUSAGE_SELF,&ru);
	tv_sec = ru.ru_utime.tv_sec + ru.ru_stime.tv_sec;
	tv_usec = ru.ru_utime.tv_usec + ru.ru_stime.tv_usec;
	return (double)tv_sec+(double)tv_usec/(double)1000000;
}
#endif
#endif

extern int GC_free_space_numerator;

void Risa_GC_get_adj(int *nm, int *dn) {
	*nm = GC_free_space_numerator;
	*dn = GC_free_space_divisor;
}

void Risa_GC_set_adj(int nm, int dn) {
	GC_free_space_numerator = nm;
	GC_free_space_divisor = dn;
}

void GC_timerstart() {
	gcstart = get_clock();
}

void GC_timerstop() {
	gctime += get_clock() - gcstart;
}

#if defined(MSWIN32) && !defined(VISUAL)
#include <signal.h>
void process_events() {
	if ( check_break() )
		raise(SIGINT);
}
#endif

#if defined(THINK_C) || defined(__MWERKS__)
#include <signal.h>
#include <Events.h>

int sigsetmask(int mask){ return 0; }

void process_events() {

	register EvQElPtr q;
#if 0
	extern void (*app_process_events)();
#endif

	for (q = (EvQElPtr) GetEventQueue()->qHead; q; q = (EvQElPtr) q->qLink)
		if (q->evtQWhat == keyDown && (char) q->evtQMessage == '.')
			if (q->evtQModifiers & cmdKey) {
				raise(SIGINT); break;
			}
#if 0
	if ( app_process_events )
		(*app_process_events)();
#endif
}
#endif

#if defined(VISUAL) && !defined(MSWIN32)
int sigsetmask(mask) int mask; { return 0; }

void process_events() {
int c;
        
	while ( c = read_cons() )
#if defined(GO32)
		if ( c == ('x' & 037 ) )
#else
		if ( c == ('c' & 037 ) )
#endif
			int_handler();
}
#endif
