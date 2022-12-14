/*
 * Copyright 1988, 1989 Hans-J. Boehm, Alan J. Demers
 * Copyright (c) 1991-1995 by Xerox Corporation.  All rights reserved.
 * Copyright (c) 1996-1999 by Silicon Graphics.  All rights reserved.
 * Copyright (c) 1999 by Hewlett-Packard Company.  All rights reserved.
 *
 * THIS MATERIAL IS PROVIDED AS IS, WITH ABSOLUTELY NO WARRANTY EXPRESSED
 * OR IMPLIED.  ANY USE IS AT YOUR OWN RISK.
 *
 * Permission is hereby granted to use or copy this program
 * for any purpose,  provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 */

# include "private/gc_priv.h"

# if defined(LINUX) && !defined(POWERPC)
#   include <linux/version.h>
#   if (LINUX_VERSION_CODE <= 0x10400)
      /* Ugly hack to get struct sigcontext_struct definition.  Required      */
      /* for some early 1.3.X releases.  Will hopefully go away soon. */
      /* in some later Linux releases, asm/sigcontext.h may have to   */
      /* be included instead.                                         */
#     define __KERNEL__
#     include <asm/signal.h>
#     undef __KERNEL__
#   else
      /* Kernels prior to 2.1.1 defined struct sigcontext_struct instead of */
      /* struct sigcontext.  libc6 (glibc2) uses "struct sigcontext" in     */
      /* prototypes, so we have to include the top-level sigcontext.h to    */
      /* make sure the former gets defined to be the latter if appropriate. */
#     include <features.h>
#     if 2 <= __GLIBC__
#       if 2 == __GLIBC__ && 0 == __GLIBC_MINOR__
	  /* glibc 2.1 no longer has sigcontext.h.  But signal.h	*/
	  /* has the right declaration for glibc 2.1.			*/
#         include <sigcontext.h>
#       endif /* 0 == __GLIBC_MINOR__ */
#     else /* not 2 <= __GLIBC__ */
        /* libc5 doesn't have <sigcontext.h>: go directly with the kernel   */
        /* one.  Check LINUX_VERSION_CODE to see which we should reference. */
#       include <asm/sigcontext.h>
#     endif /* 2 <= __GLIBC__ */
#   endif
# endif
# if !defined(OS2) && !defined(PCR) && !defined(AMIGA) && !defined(MACOS) \
    && !defined(MSWINCE)
#   include <sys/types.h>
#   if !defined(MSWIN32) && !defined(SUNOS4)
#   	include <unistd.h>
#   endif
# endif

# include <stdio.h>
# if defined(MSWINCE)
#   define SIGSEGV 0 /* value is irrelevant */
# else
#   include <signal.h>
# endif

/* Blatantly OS dependent routines, except for those that are related 	*/
/* to dynamic loading.							*/

# if defined(HEURISTIC2) || defined(SEARCH_FOR_DATA_START)
#   define NEED_FIND_LIMIT
# endif

# if !defined(STACKBOTTOM) && defined(HEURISTIC2)
#   define NEED_FIND_LIMIT
# endif

# if (defined(SUNOS4) && defined(DYNAMIC_LOADING)) && !defined(PCR)
#   define NEED_FIND_LIMIT
# endif

# if (defined(SVR4) || defined(AUX) || defined(DGUX) \
      || (defined(LINUX) && defined(SPARC))) && !defined(PCR)
#   define NEED_FIND_LIMIT
# endif

#if defined(FREEBSD) && defined(I386)
#  include <machine/trap.h>
#  if !defined(PCR)
#    define NEED_FIND_LIMIT
#  endif
#endif

#ifdef NEED_FIND_LIMIT
#   include <setjmp.h>
#endif

#ifdef AMIGA
# define GC_AMIGA_DEF
# include "AmigaOS.c"
# undef GC_AMIGA_DEF
#endif

#if defined(MSWIN32) || defined(MSWINCE)
# define WIN32_LEAN_AND_MEAN
# define NOSERVICE
# include <windows.h>
#endif

#ifdef MACOS
# include <Processes.h>
#endif

#ifdef IRIX5
# include <sys/uio.h>
# include <malloc.h>   /* for locking */
#endif
#ifdef USE_MMAP
# include <sys/types.h>
# include <sys/mman.h>
# include <sys/stat.h>
#endif

#ifdef UNIX_LIKE
# include <fcntl.h>
#endif

#if defined(SUNOS5SIGS) || defined (HURD) || defined(LINUX)
# ifdef SUNOS5SIGS
#  include <sys/siginfo.h>
# endif
# undef setjmp
# undef longjmp
# define setjmp(env) sigsetjmp(env, 1)
# define longjmp(env, val) siglongjmp(env, val)
# define jmp_buf sigjmp_buf
#endif

#ifdef DARWIN
/* for get_etext and friends */
#include <mach-o/getsect.h>
#endif

#ifdef DJGPP
  /* Apparently necessary for djgpp 2.01.  May cause problems with	*/
  /* other versions.							*/
  typedef long unsigned int caddr_t;
#endif

#ifdef PCR
# include "il/PCR_IL.h"
# include "th/PCR_ThCtl.h"
# include "mm/PCR_MM.h"
#endif

#if !defined(NO_EXECUTE_PERMISSION)
# define OPT_PROT_EXEC PROT_EXEC
#else
# define OPT_PROT_EXEC 0
#endif

#if defined(LINUX) && \
    (defined(USE_PROC_FOR_LIBRARIES) || defined(IA64) || !defined(SMALL_CONFIG))

/* We need to parse /proc/self/maps, either to find dynamic libraries,	*/
/* and/or to find the register backing store base (IA64).  Do it once	*/
/* here.								*/

#define READ read

/* Repeatedly perform a read call until the buffer is filled or	*/
/* we encounter EOF.						*/
ssize_t GC_repeat_read(int fd, char *buf, size_t count)
{
    ssize_t num_read = 0;
    ssize_t result;
    
    while (num_read < count) {
	result = READ(fd, buf + num_read, count - num_read);
	if (result < 0) return result;
	if (result == 0) break;
	num_read += result;
    }
    return num_read;
}

/*
 * Apply fn to a buffer containing the contents of /proc/self/maps.
 * Return the result of fn or, if we failed, 0.
 */

word GC_apply_to_maps(word (*fn)(char *))
{
    int f;
    int result;
    int maps_size;
    char maps_temp[32768];
    char *maps_buf;

    /* Read /proc/self/maps	*/
        /* Note that we may not allocate, and thus can't use stdio.	*/
        f = open("/proc/self/maps", O_RDONLY);
        if (-1 == f) return 0;
	/* stat() doesn't work for /proc/self/maps, so we have to
	   read it to find out how large it is... */
	maps_size = 0;
	do {
	    result = GC_repeat_read(f, maps_temp, sizeof(maps_temp));
	    if (result <= 0) return 0;
	    maps_size += result;
	} while (result == sizeof(maps_temp));

	if (maps_size > sizeof(maps_temp)) {
	    /* If larger than our buffer, close and re-read it. */
	    close(f);
	    f = open("/proc/self/maps", O_RDONLY);
	    if (-1 == f) return 0;
	    maps_buf = alloca(maps_size);
	    if (NULL == maps_buf) return 0;
	    result = GC_repeat_read(f, maps_buf, maps_size);
	    if (result <= 0) return 0;
	} else {
	    /* Otherwise use the fixed size buffer */
	    maps_buf = maps_temp;
	}

	close(f);
        maps_buf[result] = '\0';
	
    /* Apply fn to result. */
	return fn(maps_buf);
}

#endif /* Need GC_apply_to_maps */

#if defined(LINUX) && (defined(USE_PROC_FOR_LIBRARIES) || defined(IA64))
//
//  GC_parse_map_entry parses an entry from /proc/self/maps so we can
//  locate all writable data segments that belong to shared libraries.
//  The format of one of these entries and the fields we care about
//  is as follows:
//  XXXXXXXX-XXXXXXXX r-xp 00000000 30:05 260537     name of mapping...\n
//  ^^^^^^^^ ^^^^^^^^ ^^^^          ^^
//  start    end      prot          maj_dev
//  0        9        18            32
//  
//  For 64 bit ABIs:
//  0	     17	      34	    56
//
//  The parser is called with a pointer to the entry and the return value
//  is either NULL or is advanced to the next entry(the byte after the
//  trailing '\n'.)
//
#if CPP_WORDSZ == 32
# define OFFSET_MAP_START   0
# define OFFSET_MAP_END     9
# define OFFSET_MAP_PROT   18
# define OFFSET_MAP_MAJDEV 32
# define ADDR_WIDTH 	    8
#endif

#if CPP_WORDSZ == 64
# define OFFSET_MAP_START   0
# define OFFSET_MAP_END    17
# define OFFSET_MAP_PROT   34
# define OFFSET_MAP_MAJDEV 56
# define ADDR_WIDTH 	   16
#endif

/*
 * Assign various fields of the first line in buf_ptr to *start, *end,
 * *prot_buf and *maj_dev.  Only *prot_buf may be set for unwritable maps.
 */
char *GC_parse_map_entry(char *buf_ptr, word *start, word *end,
                                char *prot_buf, unsigned int *maj_dev)
{
    int i;
    char *tok;

    if (buf_ptr == NULL || *buf_ptr == '\0') {
        return NULL;
    }

    memcpy(prot_buf, buf_ptr+OFFSET_MAP_PROT, 4);
    				/* do the protections first. */
    prot_buf[4] = '\0';

    if (prot_buf[1] == 'w') {/* we can skip all of this if it's not writable. */

        tok = buf_ptr;
        buf_ptr[OFFSET_MAP_START+ADDR_WIDTH] = '\0';
        *start = strtoul(tok, NULL, 16);

        tok = buf_ptr+OFFSET_MAP_END;
        buf_ptr[OFFSET_MAP_END+ADDR_WIDTH] = '\0';
        *end = strtoul(tok, NULL, 16);

        buf_ptr += OFFSET_MAP_MAJDEV;
        tok = buf_ptr;
        while (*buf_ptr != ':') buf_ptr++;
        *buf_ptr++ = '\0';
        *maj_dev = strtoul(tok, NULL, 16);
    }

    while (*buf_ptr && *buf_ptr++ != '\n');

    return buf_ptr;
}

#endif /* Need to parse /proc/self/maps. */	

#if defined(SEARCH_FOR_DATA_START)
  /* The I386 case can be handled without a search.  The Alpha case	*/
  /* used to be handled differently as well, but the rules changed	*/
  /* for recent Linux versions.  This seems to be the easiest way to	*/
  /* cover all versions.						*/

# ifdef LINUX
    /* Some Linux distributions arrange to define __data_start.  Some	*/
    /* define data_start as a weak symbol.  The latter is technically	*/
    /* broken, since the user program may define data_start, in which	*/
    /* case we lose.  Nonetheless, we try both, prefering __data_start.	*/
    /* We assume gcc-compatible pragmas.	*/
#   pragma weak __data_start
    extern int __data_start[];
#   pragma weak data_start
    extern int data_start[];
# endif /* LINUX */
  extern int _end[];

  ptr_t GC_data_start;

  void GC_init_linux_data_start()
  {
    extern ptr_t GC_find_limit();

#   ifdef LINUX
      /* Try the easy approaches first:	*/
      if ((ptr_t)__data_start != 0) {
	  GC_data_start = (ptr_t)(__data_start);
	  return;
      }
      if ((ptr_t)data_start != 0) {
	  GC_data_start = (ptr_t)(data_start);
	  return;
      }
#   endif /* LINUX */
    GC_data_start = GC_find_limit((ptr_t)(_end), FALSE);
  }
#endif

# ifdef ECOS

# ifndef ECOS_GC_MEMORY_SIZE
# define ECOS_GC_MEMORY_SIZE (448 * 1024)
# endif /* ECOS_GC_MEMORY_SIZE */

// setjmp() function, as described in ANSI para 7.6.1.1
#define setjmp( __env__ )  hal_setjmp( __env__ )

// FIXME: This is a simple way of allocating memory which is
// compatible with ECOS early releases.  Later releases use a more
// sophisticated means of allocating memory than this simple static
// allocator, but this method is at least bound to work.
static char memory[ECOS_GC_MEMORY_SIZE];
static char *brk = memory;

static void *tiny_sbrk(ptrdiff_t increment)
{
  void *p = brk;

  brk += increment;

  if (brk >  memory + sizeof memory)
    {
      brk -= increment;
      return NULL;
    }

  return p;
}
#define sbrk tiny_sbrk
# endif /* ECOS */

#if (defined(NETBSD) || defined(OPENBSD)) && defined(__ELF__)
  ptr_t GC_data_start;

  void GC_init_netbsd_elf()
  {
    extern ptr_t GC_find_limit();
    extern char **environ;
	/* This may need to be environ, without the underscore, for	*/
	/* some versions.						*/
    GC_data_start = GC_find_limit((ptr_t)&environ, FALSE);
  }
#endif

# ifdef OS2

# include <stddef.h>

# if !defined(__IBMC__) && !defined(__WATCOMC__) /* e.g. EMX */

struct exe_hdr {
    unsigned short      magic_number;
    unsigned short      padding[29];
    long                new_exe_offset;
};

#define E_MAGIC(x)      (x).magic_number
#define EMAGIC          0x5A4D  
#define E_LFANEW(x)     (x).new_exe_offset

struct e32_exe {
    unsigned char       magic_number[2]; 
    unsigned char       byte_order; 
    unsigned char       word_order; 
    unsigned long       exe_format_level;
    unsigned short      cpu;       
    unsigned short      os;
    unsigned long       padding1[13];
    unsigned long       object_table_offset;
    unsigned long       object_count;    
    unsigned long       padding2[31];
};

#define E32_MAGIC1(x)   (x).magic_number[0]
#define E32MAGIC1       'L'
#define E32_MAGIC2(x)   (x).magic_number[1]
#define E32MAGIC2       'X'
#define E32_BORDER(x)   (x).byte_order
#define E32LEBO         0
#define E32_WORDER(x)   (x).word_order
#define E32LEWO         0
#define E32_CPU(x)      (x).cpu
#define E32CPU286       1
#define E32_OBJTAB(x)   (x).object_table_offset
#define E32_OBJCNT(x)   (x).object_count

struct o32_obj {
    unsigned long       size;  
    unsigned long       base;
    unsigned long       flags;  
    unsigned long       pagemap;
    unsigned long       mapsize; 
    unsigned long       reserved;
};

#define O32_FLAGS(x)    (x).flags
#define OBJREAD         0x0001L
#define OBJWRITE        0x0002L
#define OBJINVALID      0x0080L
#define O32_SIZE(x)     (x).size
#define O32_BASE(x)     (x).base

# else  /* IBM's compiler */

/* A kludge to get around what appears to be a header file bug */
# ifndef WORD
#   define WORD unsigned short
# endif
# ifndef DWORD
#   define DWORD unsigned long
# endif

# define EXE386 1
# include <newexe.h>
# include <exe386.h>

# endif  /* __IBMC__ */

# define INCL_DOSEXCEPTIONS
# define INCL_DOSPROCESS
# define INCL_DOSERRORS
# define INCL_DOSMODULEMGR
# define INCL_DOSMEMMGR
# include <os2.h>


/* Disable and enable signals during nontrivial allocations	*/

void GC_disable_signals(void)
{
    ULONG nest;
    
    DosEnterMustComplete(&nest);
    if (nest != 1) ABORT("nested GC_disable_signals");
}

void GC_enable_signals(void)
{
    ULONG nest;
    
    DosExitMustComplete(&nest);
    if (nest != 0) ABORT("GC_enable_signals");
}


# else

#  if !defined(PCR) && !defined(AMIGA) && !defined(MSWIN32) \
      && !defined(MSWINCE) \
      && !defined(MACOS) && !defined(DJGPP) && !defined(DOS4GW) \
      && !defined(NOSYS) && !defined(ECOS)

#   if defined(sigmask) && !defined(UTS4) && !defined(HURD)
	/* Use the traditional BSD interface */
#	define SIGSET_T int
#	define SIG_DEL(set, signal) (set) &= ~(sigmask(signal))
#	define SIG_FILL(set)  (set) = 0x7fffffff
    	  /* Setting the leading bit appears to provoke a bug in some	*/
    	  /* longjmp implementations.  Most systems appear not to have	*/
    	  /* a signal 32.						*/
#	define SIGSETMASK(old, new) (old) = sigsetmask(new)
#   else
	/* Use POSIX/SYSV interface	*/
#	define SIGSET_T sigset_t
#	define SIG_DEL(set, signal) sigdelset(&(set), (signal))
#	define SIG_FILL(set) sigfillset(&set)
#	define SIGSETMASK(old, new) sigprocmask(SIG_SETMASK, &(new), &(old))
#   endif

static GC_bool mask_initialized = FALSE;

static SIGSET_T new_mask;

static SIGSET_T old_mask;

static SIGSET_T dummy;

#if defined(PRINTSTATS) && !defined(THREADS)
# define CHECK_SIGNALS
  int GC_sig_disabled = 0;
#endif

void GC_disable_signals()
{
    if (!mask_initialized) {
    	SIG_FILL(new_mask);

	SIG_DEL(new_mask, SIGSEGV);
	SIG_DEL(new_mask, SIGILL);
	SIG_DEL(new_mask, SIGQUIT);
#	ifdef SIGBUS
	    SIG_DEL(new_mask, SIGBUS);
#	endif
#	ifdef SIGIOT
	    SIG_DEL(new_mask, SIGIOT);
#	endif
#	ifdef SIGEMT
	    SIG_DEL(new_mask, SIGEMT);
#	endif
#	ifdef SIGTRAP
	    SIG_DEL(new_mask, SIGTRAP);
#	endif 
	mask_initialized = TRUE;
    }
#   ifdef CHECK_SIGNALS
	if (GC_sig_disabled != 0) ABORT("Nested disables");
	GC_sig_disabled++;
#   endif
    SIGSETMASK(old_mask,new_mask);
}

void GC_enable_signals()
{
#   ifdef CHECK_SIGNALS
	if (GC_sig_disabled != 1) ABORT("Unmatched enable");
	GC_sig_disabled--;
#   endif
    SIGSETMASK(dummy,old_mask);
}

#  endif  /* !PCR */

# endif /*!OS/2 */

/* Ivan Demakov: simplest way (to me) */
#if defined (DOS4GW)
  void GC_disable_signals() { }
  void GC_enable_signals() { }
#endif

/* Find the page size */
word GC_page_size;

# if defined(MSWIN32) || defined(MSWINCE)
  void GC_setpagesize()
  {
    GetSystemInfo(&GC_sysinfo);
    GC_page_size = GC_sysinfo.dwPageSize;
  }

# else
#   if defined(MPROTECT_VDB) || defined(PROC_VDB) || defined(USE_MMAP) \
       || defined(USE_MUNMAP)
	void GC_setpagesize()
	{
	    GC_page_size = GETPAGESIZE();
	}
#   else
	/* It's acceptable to fake it. */
	void GC_setpagesize()
	{
	    GC_page_size = HBLKSIZE;
	}
#   endif
# endif

/* 
 * Find the base of the stack. 
 * Used only in single-threaded environment.
 * With threads, GC_mark_roots needs to know how to do this.
 * Called with allocator lock held.
 */
# if defined(MSWIN32) || defined(MSWINCE)
# define is_writable(prot) ((prot) == PAGE_READWRITE \
			    || (prot) == PAGE_WRITECOPY \
			    || (prot) == PAGE_EXECUTE_READWRITE \
			    || (prot) == PAGE_EXECUTE_WRITECOPY)
/* Return the number of bytes that are writable starting at p.	*/
/* The pointer p is assumed to be page aligned.			*/
/* If base is not 0, *base becomes the beginning of the 	*/
/* allocation region containing p.				*/
word GC_get_writable_length(ptr_t p, ptr_t *base)
{
    MEMORY_BASIC_INFORMATION buf;
    word result;
    word protect;
    
    result = VirtualQuery(p, &buf, sizeof(buf));
    if (result != sizeof(buf)) ABORT("Weird VirtualQuery result");
    if (base != 0) *base = (ptr_t)(buf.AllocationBase);
    protect = (buf.Protect & ~(PAGE_GUARD | PAGE_NOCACHE));
    if (!is_writable(protect)) {
        return(0);
    }
    if (buf.State != MEM_COMMIT) return(0);
    return(buf.RegionSize);
}

ptr_t GC_get_stack_base()
{
    int dummy;
    ptr_t sp = (ptr_t)(&dummy);
    ptr_t trunc_sp = (ptr_t)((word)sp & ~(GC_page_size - 1));
    word size = GC_get_writable_length(trunc_sp, 0);
   
    return(trunc_sp + size);
}


# endif /* MS Windows */

# ifdef BEOS
# include <kernel/OS.h>
ptr_t GC_get_stack_base(){
	thread_info th;
	get_thread_info(find_thread(NULL),&th);
	return th.stack_end;
}
# endif /* BEOS */


# ifdef OS2

ptr_t GC_get_stack_base()
{
    PTIB ptib;
    PPIB ppib;
    
    if (DosGetInfoBlocks(&ptib, &ppib) != NO_ERROR) {
    	GC_err_printf0("DosGetInfoBlocks failed\n");
    	ABORT("DosGetInfoBlocks failed\n");
    }
    return((ptr_t)(ptib -> tib_pstacklimit));
}

# endif /* OS2 */

# ifdef AMIGA
#   define GC_AMIGA_SB
#   include "AmigaOS.c"
#   undef GC_AMIGA_SB
# endif /* AMIGA */

# if defined(NEED_FIND_LIMIT) || defined(UNIX_LIKE)

#   ifdef __STDC__
	typedef void (*handler)(int);
#   else
	typedef void (*handler)();
#   endif

#   if defined(SUNOS5SIGS) || defined(IRIX5) || defined(OSF1) || defined(HURD)
	static struct sigaction old_segv_act;
#	if defined(_sigargs) /* !Irix6.x */ || defined(HPUX) || defined(HURD)
	    static struct sigaction old_bus_act;
#	endif
#   else
        static handler old_segv_handler, old_bus_handler;
#   endif
    
#   ifdef __STDC__
      void GC_set_and_save_fault_handler(handler h)
#   else
      void GC_set_and_save_fault_handler(h)
      handler h;
#   endif
    {
#	if defined(SUNOS5SIGS) || defined(IRIX5)  \
        || defined(OSF1) || defined(HURD)
	  struct sigaction	act;

	  act.sa_handler	= h;
#	  ifdef SUNOS5SIGS
            act.sa_flags          = SA_RESTART | SA_NODEFER;
#         else
            act.sa_flags          = SA_RESTART;
#	  endif
          /* The presence of SA_NODEFER represents yet another gross    */
          /* hack.  Under Solaris 2.3, siglongjmp doesn't appear to     */
          /* interact correctly with -lthread.  We hide the confusion   */
          /* by making sure that signal handling doesn't affect the     */
          /* signal mask.                                               */

	  (void) sigemptyset(&act.sa_mask);
#	  ifdef GC_IRIX_THREADS
		/* Older versions have a bug related to retrieving and	*/
		/* and setting a handler at the same time.		*/
	        (void) sigaction(SIGSEGV, 0, &old_segv_act);
	        (void) sigaction(SIGSEGV, &act, 0);
#	  else
	        (void) sigaction(SIGSEGV, &act, &old_segv_act);
#		if defined(IRIX5) && defined(_sigargs) /* Irix 5.x, not 6.x */ \
		   || defined(HPUX) || defined(HURD)
		    /* Under Irix 5.x or HP/UX, we may get SIGBUS.	*/
		    /* Pthreads doesn't exist under Irix 5.x, so we	*/
		    /* don't have to worry in the threads case.		*/
		    (void) sigaction(SIGBUS, &act, &old_bus_act);
#		endif
#	  endif	/* GC_IRIX_THREADS */
#	else
    	  old_segv_handler = signal(SIGSEGV, h);
#	  ifdef SIGBUS
	    old_bus_handler = signal(SIGBUS, h);
#	  endif
#	endif
    }
# endif /* NEED_FIND_LIMIT || UNIX_LIKE */

# ifdef NEED_FIND_LIMIT
  /* Some tools to implement HEURISTIC2	*/
#   define MIN_PAGE_SIZE 256	/* Smallest conceivable page size, bytes */
    /* static */ jmp_buf GC_jmp_buf;
    
    /*ARGSUSED*/
    void GC_fault_handler(sig)
    int sig;
    {
        longjmp(GC_jmp_buf, 1);
    }

    void GC_setup_temporary_fault_handler()
    {
	GC_set_and_save_fault_handler(GC_fault_handler);
    }
    
    void GC_reset_fault_handler()
    {
#       if defined(SUNOS5SIGS) || defined(IRIX5) \
	   || defined(OSF1) || defined(HURD)
	  (void) sigaction(SIGSEGV, &old_segv_act, 0);
#	  if defined(IRIX5) && defined(_sigargs) /* Irix 5.x, not 6.x */ \
	     || defined(HPUX) || defined(HURD)
	      (void) sigaction(SIGBUS, &old_bus_act, 0);
#	  endif
#       else
  	  (void) signal(SIGSEGV, old_segv_handler);
#	  ifdef SIGBUS
	    (void) signal(SIGBUS, old_bus_handler);
#	  endif
#       endif
    }

    /* Return the first nonaddressible location > p (up) or 	*/
    /* the smallest location q s.t. [q,p) is addressable (!up).	*/
    /* We assume that p (up) or p-1 (!up) is addressable.	*/
    ptr_t GC_find_limit(p, up)
    ptr_t p;
    GC_bool up;
    {
        static VOLATILE ptr_t result;
    		/* Needs to be static, since otherwise it may not be	*/
    		/* preserved across the longjmp.  Can safely be 	*/
    		/* static since it's only called once, with the		*/
    		/* allocation lock held.				*/


	GC_setup_temporary_fault_handler();
	if (setjmp(GC_jmp_buf) == 0) {
	    result = (ptr_t)(((word)(p))
			      & ~(MIN_PAGE_SIZE-1));
	    for (;;) {
 	        if (up) {
		    result += MIN_PAGE_SIZE;
 	        } else {
		    result -= MIN_PAGE_SIZE;
 	        }
		GC_noop1((word)(*result));
	    }
	}
	GC_reset_fault_handler();
 	if (!up) {
	    result += MIN_PAGE_SIZE;
 	}
	return(result);
    }
# endif

#if defined(ECOS) || defined(NOSYS)
  ptr_t GC_get_stack_base()
  {
    return STACKBOTTOM;
  }
#endif

#ifdef LINUX_STACKBOTTOM

#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>

# define STAT_SKIP 27   /* Number of fields preceding startstack	*/
			/* field in /proc/self/stat			*/

# pragma weak __libc_stack_end
  extern ptr_t __libc_stack_end;

# ifdef IA64
    /* Try to read the backing store base from /proc/self/maps.	*/
    /* We look for the writable mapping with a 0 major device,  */
    /* which is	as close to our frame as possible, but below it.*/
    static word backing_store_base_from_maps(char *maps)
    {
      char prot_buf[5];
      char *buf_ptr = maps;
      word start, end;
      unsigned int maj_dev;
      word current_best = 0;
      word dummy;
  
      for (;;) {
        buf_ptr = GC_parse_map_entry(buf_ptr, &start, &end, prot_buf, &maj_dev);
	if (buf_ptr == NULL) return current_best;
	if (prot_buf[1] == 'w' && maj_dev == 0) {
	    if (end < (word)(&dummy) && start > current_best) current_best = start;
	}
      }
      return current_best;
    }

    static word backing_store_base_from_proc(void)
    {
        return GC_apply_to_maps(backing_store_base_from_maps);
    }

#   pragma weak __libc_ia64_register_backing_store_base
    extern ptr_t __libc_ia64_register_backing_store_base;

    ptr_t GC_get_register_stack_base(void)
    {
      if (0 != &__libc_ia64_register_backing_store_base
	  && 0 != __libc_ia64_register_backing_store_base) {
	/* Glibc 2.2.4 has a bug such that for dynamically linked	*/
	/* executables __libc_ia64_register_backing_store_base is 	*/
	/* defined but uninitialized during constructor calls.  	*/
	/* Hence we check for both nonzero address and value.		*/
	return __libc_ia64_register_backing_store_base;
      } else {
	word result = backing_store_base_from_proc();
	if (0 == result) {
	  /* Use dumb heuristics.  Works only for default configuration. */
	  result = (word)GC_stackbottom - BACKING_STORE_DISPLACEMENT;
	  result += BACKING_STORE_ALIGNMENT - 1;
	  result &= ~(BACKING_STORE_ALIGNMENT - 1);
	  /* Verify that it's at least readable.  If not, we goofed. */
	  GC_noop1(*(word *)result); 
	}
	return (ptr_t)result;
      }
    }
# endif

  ptr_t GC_linux_stack_base(void)
  {
    /* We read the stack base value from /proc/self/stat.  We do this	*/
    /* using direct I/O system calls in order to avoid calling malloc   */
    /* in case REDIRECT_MALLOC is defined.				*/ 
#   define STAT_BUF_SIZE 4096
#   define STAT_READ read
	  /* Should probably call the real read, if read is wrapped.	*/
    char stat_buf[STAT_BUF_SIZE];
    int f;
    char c;
    word result = 0;
    size_t i, buf_offset = 0;

    /* First try the easy way.  This should work for glibc 2.2	*/
      if (0 != &__libc_stack_end) {
#       ifdef IA64
	  /* Some versions of glibc set the address 16 bytes too	*/
	  /* low while the initialization code is running.		*/
	  if (((word)__libc_stack_end & 0xfff) + 0x10 < 0x1000) {
	    return __libc_stack_end + 0x10;
	  } /* Otherwise it's not safe to add 16 bytes and we fall	*/
	    /* back to using /proc.					*/
#	else 
	  return __libc_stack_end;
#	endif
      }
    f = open("/proc/self/stat", O_RDONLY);
    if (f < 0 || STAT_READ(f, stat_buf, STAT_BUF_SIZE) < 2 * STAT_SKIP) {
	ABORT("Couldn't read /proc/self/stat");
    }
    c = stat_buf[buf_offset++];
    /* Skip the required number of fields.  This number is hopefully	*/
    /* constant across all Linux implementations.			*/
      for (i = 0; i < STAT_SKIP; ++i) {
	while (isspace(c)) c = stat_buf[buf_offset++];
	while (!isspace(c)) c = stat_buf[buf_offset++];
      }
    while (isspace(c)) c = stat_buf[buf_offset++];
    while (isdigit(c)) {
      result *= 10;
      result += c - '0';
      c = stat_buf[buf_offset++];
    }
    close(f);
    if (result < 0x10000000) ABORT("Absurd stack bottom value");
    return (ptr_t)result;
  }

#endif /* LINUX_STACKBOTTOM */

#ifdef FREEBSD_STACKBOTTOM

/* This uses an undocumented sysctl call, but at least one expert 	*/
/* believes it will stay.						*/

#include <unistd.h>
#include <sys/types.h>
#include <sys/sysctl.h>

  ptr_t GC_freebsd_stack_base(void)
  {
    int nm[2] = {CTL_KERN, KERN_USRSTACK};
    ptr_t base;
    size_t len = sizeof(ptr_t);
    int r = sysctl(nm, 2, &base, &len, NULL, 0);
    
    if (r) ABORT("Error getting stack base");

    return base;
  }

#endif /* FREEBSD_STACKBOTTOM */

#if !defined(BEOS) && !defined(AMIGA) && !defined(MSWIN32) \
    && !defined(MSWINCE) && !defined(OS2)

ptr_t GC_get_stack_base()
{
#   if defined(HEURISTIC1) || defined(HEURISTIC2) || \
       defined(LINUX_STACKBOTTOM) || defined(FREEBSD_STACKBOTTOM)
    word dummy;
    ptr_t result;
#   endif

#   define STACKBOTTOM_ALIGNMENT_M1 ((word)STACK_GRAN - 1)

#   ifdef STACKBOTTOM
	return(STACKBOTTOM);
#   else
#	ifdef HEURISTIC1
#	   ifdef STACK_GROWS_DOWN
	     result = (ptr_t)((((word)(&dummy))
	     		       + STACKBOTTOM_ALIGNMENT_M1)
			      & ~STACKBOTTOM_ALIGNMENT_M1);
#	   else
	     result = (ptr_t)(((word)(&dummy))
			      & ~STACKBOTTOM_ALIGNMENT_M1);
#	   endif
#	endif /* HEURISTIC1 */
#	ifdef LINUX_STACKBOTTOM
	   result = GC_linux_stack_base();
#	endif
#	ifdef FREEBSD_STACKBOTTOM
	   result = GC_freebsd_stack_base();
#	endif
#	ifdef HEURISTIC2
#	    ifdef STACK_GROWS_DOWN
		result = GC_find_limit((ptr_t)(&dummy), TRUE);
#           	ifdef HEURISTIC2_LIMIT
		    if (result > HEURISTIC2_LIMIT
		        && (ptr_t)(&dummy) < HEURISTIC2_LIMIT) {
		            result = HEURISTIC2_LIMIT;
		    }
#	        endif
#	    else
		result = GC_find_limit((ptr_t)(&dummy), FALSE);
#           	ifdef HEURISTIC2_LIMIT
		    if (result < HEURISTIC2_LIMIT
		        && (ptr_t)(&dummy) > HEURISTIC2_LIMIT) {
		            result = HEURISTIC2_LIMIT;
		    }
#	        endif
#	    endif

#	endif /* HEURISTIC2 */
#	ifdef STACK_GROWS_DOWN
	    if (result == 0) result = (ptr_t)(signed_word)(-sizeof(ptr_t));
#	endif
    	return(result);
#   endif /* STACKBOTTOM */
}

# endif /* ! AMIGA, !OS 2, ! MS Windows, !BEOS */

/*
 * Register static data segment(s) as roots.
 * If more data segments are added later then they need to be registered
 * add that point (as we do with SunOS dynamic loading),
 * or GC_mark_roots needs to check for them (as we do with PCR).
 * Called with allocator lock held.
 */

# ifdef OS2

void GC_register_data_segments()
{
    PTIB ptib;
    PPIB ppib;
    HMODULE module_handle;
#   define PBUFSIZ 512
    UCHAR path[PBUFSIZ];
    FILE * myexefile;
    struct exe_hdr hdrdos;	/* MSDOS header.	*/
    struct e32_exe hdr386;	/* Real header for my executable */
    struct o32_obj seg;	/* Currrent segment */
    int nsegs;
    
    
    if (DosGetInfoBlocks(&ptib, &ppib) != NO_ERROR) {
    	GC_err_printf0("DosGetInfoBlocks failed\n");
    	ABORT("DosGetInfoBlocks failed\n");
    }
    module_handle = ppib -> pib_hmte;
    if (DosQueryModuleName(module_handle, PBUFSIZ, path) != NO_ERROR) {
    	GC_err_printf0("DosQueryModuleName failed\n");
    	ABORT("DosGetInfoBlocks failed\n");
    }
    myexefile = fopen(path, "rb");
    if (myexefile == 0) {
        GC_err_puts("Couldn't open executable ");
        GC_err_puts(path); GC_err_puts("\n");
        ABORT("Failed to open executable\n");
    }
    if (fread((char *)(&hdrdos), 1, sizeof hdrdos, myexefile) < sizeof hdrdos) {
        GC_err_puts("Couldn't read MSDOS header from ");
        GC_err_puts(path); GC_err_puts("\n");
        ABORT("Couldn't read MSDOS header");
    }
    if (E_MAGIC(hdrdos) != EMAGIC) {
        GC_err_puts("Executable has wrong DOS magic number: ");
        GC_err_puts(path); GC_err_puts("\n");
        ABORT("Bad DOS magic number");
    }
    if (fseek(myexefile, E_LFANEW(hdrdos), SEEK_SET) != 0) {
        GC_err_puts("Seek to new header failed in ");
        GC_err_puts(path); GC_err_puts("\n");
        ABORT("Bad DOS magic number");
    }
    if (fread((char *)(&hdr386), 1, sizeof hdr386, myexefile) < sizeof hdr386) {
        GC_err_puts("Couldn't read MSDOS header from ");
        GC_err_puts(path); GC_err_puts("\n");
        ABORT("Couldn't read OS/2 header");
    }
    if (E32_MAGIC1(hdr386) != E32MAGIC1 || E32_MAGIC2(hdr386) != E32MAGIC2) {
        GC_err_puts("Executable has wrong OS/2 magic number:");
        GC_err_puts(path); GC_err_puts("\n");
        ABORT("Bad OS/2 magic number");
    }
    if ( E32_BORDER(hdr386) != E32LEBO || E32_WORDER(hdr386) != E32LEWO) {
        GC_err_puts("Executable %s has wrong byte order: ");
        GC_err_puts(path); GC_err_puts("\n");
        ABORT("Bad byte order");
    }
    if ( E32_CPU(hdr386) == E32CPU286) {
        GC_err_puts("GC can't handle 80286 executables: ");
        GC_err_puts(path); GC_err_puts("\n");
        EXIT();
    }
    if (fseek(myexefile, E_LFANEW(hdrdos) + E32_OBJTAB(hdr386),
    	      SEEK_SET) != 0) {
        GC_err_puts("Seek to object table failed: ");
        GC_err_puts(path); GC_err_puts("\n");
        ABORT("Seek to object table failed");
    }
    for (nsegs = E32_OBJCNT(hdr386); nsegs > 0; nsegs--) {
      int flags;
      if (fread((char *)(&seg), 1, sizeof seg, myexefile) < sizeof seg) {
        GC_err_puts("Couldn't read obj table entry from ");
        GC_err_puts(path); GC_err_puts("\n");
        ABORT("Couldn't read obj table entry");
      }
      flags = O32_FLAGS(seg);
      if (!(flags & OBJWRITE)) continue;
      if (!(flags & OBJREAD)) continue;
      if (flags & OBJINVALID) {
          GC_err_printf0("Object with invalid pages?\n");
          continue;
      } 
      GC_add_roots_inner(O32_BASE(seg), O32_BASE(seg)+O32_SIZE(seg), FALSE);
    }
}

# else /* !OS2 */

# if defined(MSWIN32) || defined(MSWINCE)

# ifdef MSWIN32
  /* Unfortunately, we have to handle win32s very differently from NT, 	*/
  /* Since VirtualQuery has very different semantics.  In particular,	*/
  /* under win32s a VirtualQuery call on an unmapped page returns an	*/
  /* invalid result.  Under NT, GC_register_data_segments is a noop and	*/
  /* all real work is done by GC_register_dynamic_libraries.  Under	*/
  /* win32s, we cannot find the data segments associated with dll's.	*/
  /* We register the main data segment here.				*/
  GC_bool GC_no_win32_dlls = FALSE;	 
  	/* This used to be set for gcc, to avoid dealing with		*/
  	/* the structured exception handling issues.  But we now have	*/
  	/* assembly code to do that right.				*/
  
  void GC_init_win32()
  {
    /* if we're running under win32s, assume that no DLLs will be loaded */
    DWORD v = GetVersion();
    GC_no_win32_dlls |= ((v & 0x80000000) && (v & 0xff) <= 3);
  }

  /* Return the smallest address a such that VirtualQuery		*/
  /* returns correct results for all addresses between a and start.	*/
  /* Assumes VirtualQuery returns correct information for start.	*/
  ptr_t GC_least_described_address(ptr_t start)
  {  
    MEMORY_BASIC_INFORMATION buf;
    DWORD result;
    LPVOID limit;
    ptr_t p;
    LPVOID q;
    
    limit = GC_sysinfo.lpMinimumApplicationAddress;
    p = (ptr_t)((word)start & ~(GC_page_size - 1));
    for (;;) {
    	q = (LPVOID)(p - GC_page_size);
    	if ((ptr_t)q > (ptr_t)p /* underflow */ || q < limit) break;
    	result = VirtualQuery(q, &buf, sizeof(buf));
    	if (result != sizeof(buf) || buf.AllocationBase == 0) break;
    	p = (ptr_t)(buf.AllocationBase);
    }
    return(p);
  }
# endif

# ifndef REDIRECT_MALLOC
  /* We maintain a linked list of AllocationBase values that we know	*/
  /* correspond to malloc heap sections.  Currently this is only called */
  /* during a GC.  But there is some hope that for long running		*/
  /* programs we will eventually see most heap sections.		*/

  /* In the long run, it would be more reliable to occasionally walk 	*/
  /* the malloc heap with HeapWalk on the default heap.  But that	*/
  /* apparently works only for NT-based Windows. 			*/ 

  /* In the long run, a better data structure would also be nice ...	*/
  struct GC_malloc_heap_list {
    void * allocation_base;
    struct GC_malloc_heap_list *next;
  } *GC_malloc_heap_l = 0;

  /* Is p the base of one of the malloc heap sections we already know	*/
  /* about?								*/
  GC_bool GC_is_malloc_heap_base(ptr_t p)
  {
    struct GC_malloc_heap_list *q = GC_malloc_heap_l;

    while (0 != q) {
      if (q -> allocation_base == p) return TRUE;
      q = q -> next;
    }
    return FALSE;
  }

  void *GC_get_allocation_base(void *p)
  {
    MEMORY_BASIC_INFORMATION buf;
    DWORD result = VirtualQuery(p, &buf, sizeof(buf));
    if (result != sizeof(buf)) {
      ABORT("Weird VirtualQuery result");
    }
    return buf.AllocationBase;
  }

  size_t GC_max_root_size = 100000;	/* Appr. largest root size.	*/

  void GC_add_current_malloc_heap()
  {
    struct GC_malloc_heap_list *new_l =
                 malloc(sizeof(struct GC_malloc_heap_list));
    void * candidate = GC_get_allocation_base(new_l);

    if (new_l == 0) return;
    if (GC_is_malloc_heap_base(candidate)) {
      /* Try a little harder to find malloc heap.			*/
	size_t req_size = 10000;
	do {
	  void *p = malloc(req_size);
	  if (0 == p) { free(new_l); return; }
 	  candidate = GC_get_allocation_base(p);
	  free(p);
	  req_size *= 2;
	} while (GC_is_malloc_heap_base(candidate)
	         && req_size < GC_max_root_size/10 && req_size < 500000);
	if (GC_is_malloc_heap_base(candidate)) {
	  free(new_l); return;
	}
    }
#   ifdef CONDPRINT
      if (GC_print_stats)
	  GC_printf1("Found new system malloc AllocationBase at 0x%lx\n",
                     candidate);
#   endif
    new_l -> allocation_base = candidate;
    new_l -> next = GC_malloc_heap_l;
    GC_malloc_heap_l = new_l;
  }
# endif /* REDIRECT_MALLOC */
  
  /* Is p the start of either the malloc heap, or of one of our */
  /* heap sections?						*/
  GC_bool GC_is_heap_base (ptr_t p)
  {
     
     unsigned i;
     
#    ifndef REDIRECT_MALLOC
       static word last_gc_no = -1;
     
       if (last_gc_no != GC_gc_no) {
	 GC_add_current_malloc_heap();
	 last_gc_no = GC_gc_no;
       }
       if (GC_root_size > GC_max_root_size) GC_max_root_size = GC_root_size;
       if (GC_is_malloc_heap_base(p)) return TRUE;
#    endif
     for (i = 0; i < GC_n_heap_bases; i++) {
         if (GC_heap_bases[i] == p) return TRUE;
     }
     return FALSE ;
  }

# ifdef MSWIN32
  void GC_register_root_section(ptr_t static_root)
  {
      MEMORY_BASIC_INFORMATION buf;
      DWORD result;
      DWORD protect;
      LPVOID p;
      char * base;
      char * limit, * new_limit;
    
      if (!GC_no_win32_dlls) return;
      p = base = limit = GC_least_described_address(static_root);
      while (p < GC_sysinfo.lpMaximumApplicationAddress) {
        result = VirtualQuery(p, &buf, sizeof(buf));
        if (result != sizeof(buf) || buf.AllocationBase == 0
            || GC_is_heap_base(buf.AllocationBase)) break;
        new_limit = (char *)p + buf.RegionSize;
        protect = buf.Protect;
        if (buf.State == MEM_COMMIT
            && is_writable(protect)) {
            if ((char *)p == limit) {
                limit = new_limit;
            } else {
                if (base != limit) GC_add_roots_inner(base, limit, FALSE);
                base = p;
                limit = new_limit;
            }
        }
        if (p > (LPVOID)new_limit /* overflow */) break;
        p = (LPVOID)new_limit;
      }
      if (base != limit) GC_add_roots_inner(base, limit, FALSE);
  }
#endif
  
  void GC_register_data_segments()
  {
#     ifdef MSWIN32
      static char dummy;
      GC_register_root_section((ptr_t)(&dummy));
#     endif
  }

# else /* !OS2 && !Windows */

# if (defined(SVR4) || defined(AUX) || defined(DGUX) \
      || (defined(LINUX) && defined(SPARC))) && !defined(PCR)
ptr_t GC_SysVGetDataStart(max_page_size, etext_addr)
int max_page_size;
int * etext_addr;
{
    word text_end = ((word)(etext_addr) + sizeof(word) - 1)
    		    & ~(sizeof(word) - 1);
    	/* etext rounded to word boundary	*/
    word next_page = ((text_end + (word)max_page_size - 1)
    		      & ~((word)max_page_size - 1));
    word page_offset = (text_end & ((word)max_page_size - 1));
    VOLATILE char * result = (char *)(next_page + page_offset);
    /* Note that this isnt equivalent to just adding		*/
    /* max_page_size to &etext if &etext is at a page boundary	*/
    
    GC_setup_temporary_fault_handler();
    if (setjmp(GC_jmp_buf) == 0) {
    	/* Try writing to the address.	*/
    	*result = *result;
        GC_reset_fault_handler();
    } else {
        GC_reset_fault_handler();
    	/* We got here via a longjmp.  The address is not readable.	*/
    	/* This is known to happen under Solaris 2.4 + gcc, which place	*/
    	/* string constants in the text segment, but after etext.	*/
    	/* Use plan B.  Note that we now know there is a gap between	*/
    	/* text and data segments, so plan A bought us something.	*/
    	result = (char *)GC_find_limit((ptr_t)(DATAEND), FALSE);
    }
    return((ptr_t)result);
}
# endif

// # if defined(FREEBSD) && defined(I386) && !defined(PCR)
# if defined(FREEBSD) && ( defined(I386) || defined(X86_64) ) && !defined(PCR)
/* Its unclear whether this should be identical to the above, or 	*/
/* whether it should apply to non-X86 architectures.			*/
/* For now we don't assume that there is always an empty page after	*/
/* etext.  But in some cases there actually seems to be slightly more.  */
/* This also deals with holes between read-only data and writable data.	*/
ptr_t GC_FreeBSDGetDataStart(max_page_size, etext_addr)
int max_page_size;
int * etext_addr;
{
    word text_end = ((word)(etext_addr) + sizeof(word) - 1)
		     & ~(sizeof(word) - 1);
	/* etext rounded to word boundary	*/
    VOLATILE word next_page = (text_end + (word)max_page_size - 1)
			      & ~((word)max_page_size - 1);
    VOLATILE ptr_t result = (ptr_t)text_end;
    GC_setup_temporary_fault_handler();
    if (setjmp(GC_jmp_buf) == 0) {
	/* Try reading at the address.				*/
	/* This should happen before there is another thread.	*/
	for (; next_page < (word)(DATAEND); next_page += (word)max_page_size)
	    *(VOLATILE char *)next_page;
	GC_reset_fault_handler();
    } else {
	GC_reset_fault_handler();
	/* As above, we go to plan B	*/
	result = GC_find_limit((ptr_t)(DATAEND), FALSE);
    }
    return(result);
}

# endif


#ifdef AMIGA

#  define GC_AMIGA_DS
#  include "AmigaOS.c"
#  undef GC_AMIGA_DS

#else /* !OS2 && !Windows && !AMIGA */

void GC_register_data_segments()
{
#   if !defined(PCR) && !defined(SRC_M3) && !defined(MACOS)
#     if defined(REDIRECT_MALLOC) && defined(GC_SOLARIS_THREADS)
	/* As of Solaris 2.3, the Solaris threads implementation	*/
	/* allocates the data structure for the initial thread with	*/
	/* sbrk at process startup.  It needs to be scanned, so that	*/
	/* we don't lose some malloc allocated data structures		*/
	/* hanging from it.  We're on thin ice here ...			*/
        extern caddr_t sbrk();

	GC_add_roots_inner(DATASTART, (char *)sbrk(0), FALSE);
#     else
	GC_add_roots_inner(DATASTART, (char *)(DATAEND), FALSE);
#       if defined(DATASTART2)
         GC_add_roots_inner(DATASTART2, (char *)(DATAEND2), FALSE);
#       endif
#     endif
#   endif
#   if defined(MACOS)
    {
#   if defined(THINK_C)
	extern void* GC_MacGetDataStart(void);
	/* globals begin above stack and end at a5. */
	GC_add_roots_inner((ptr_t)GC_MacGetDataStart(),
			   (ptr_t)LMGetCurrentA5(), FALSE);
#   else
#     if defined(__MWERKS__)
#       if !__POWERPC__
	  extern void* GC_MacGetDataStart(void);
	  /* MATTHEW: Function to handle Far Globals (CW Pro 3) */
#         if __option(far_data)
	  extern void* GC_MacGetDataEnd(void);
#         endif
	  /* globals begin above stack and end at a5. */
	  GC_add_roots_inner((ptr_t)GC_MacGetDataStart(),
          		     (ptr_t)LMGetCurrentA5(), FALSE);
	  /* MATTHEW: Handle Far Globals */          		     
#         if __option(far_data)
      /* Far globals follow he QD globals: */
	  GC_add_roots_inner((ptr_t)LMGetCurrentA5(),
          		     (ptr_t)GC_MacGetDataEnd(), FALSE);
#         endif
#       else
	  extern char __data_start__[], __data_end__[];
	  GC_add_roots_inner((ptr_t)&__data_start__,
	  		     (ptr_t)&__data_end__, FALSE);
#       endif /* __POWERPC__ */
#     endif /* __MWERKS__ */
#   endif /* !THINK_C */
    }
#   endif /* MACOS */

    /* Dynamic libraries are added at every collection, since they may  */
    /* change.								*/
}

# endif  /* ! AMIGA */
# endif  /* ! MSWIN32 && ! MSWINCE*/
# endif  /* ! OS2 */

/*
 * Auxiliary routines for obtaining memory from OS.
 */

# if !defined(OS2) && !defined(PCR) && !defined(AMIGA) \
	&& !defined(MSWIN32) && !defined(MSWINCE) \
	&& !defined(MACOS) && !defined(DOS4GW)

# ifdef SUNOS4
    extern caddr_t sbrk();
# endif
# ifdef __STDC__
#   define SBRK_ARG_T ptrdiff_t
# else
#   define SBRK_ARG_T int
# endif


# ifdef RS6000
/* The compiler seems to generate speculative reads one past the end of	*/
/* an allocated object.  Hence we need to make sure that the page 	*/
/* following the last heap page is also mapped.				*/
ptr_t GC_unix_get_mem(bytes)
word bytes;
{
    caddr_t cur_brk = (caddr_t)sbrk(0);
    caddr_t result;
    SBRK_ARG_T lsbs = (word)cur_brk & (GC_page_size-1);
    static caddr_t my_brk_val = 0;
    
    if ((SBRK_ARG_T)bytes < 0) return(0); /* too big */
    if (lsbs != 0) {
        if((caddr_t)(sbrk(GC_page_size - lsbs)) == (caddr_t)(-1)) return(0);
    }
    if (cur_brk == my_brk_val) {
    	/* Use the extra block we allocated last time. */
        result = (ptr_t)sbrk((SBRK_ARG_T)bytes);
        if (result == (caddr_t)(-1)) return(0);
        result -= GC_page_size;
    } else {
        result = (ptr_t)sbrk(GC_page_size + (SBRK_ARG_T)bytes);
        if (result == (caddr_t)(-1)) return(0);
    }
    my_brk_val = result + bytes + GC_page_size;	/* Always page aligned */
    return((ptr_t)result);
}

#else  /* Not RS6000 */

#if defined(USE_MMAP)
/* Tested only under Linux, IRIX5 and Solaris 2 */

#ifdef USE_MMAP_FIXED
#   define GC_MMAP_FLAGS MAP_FIXED | MAP_PRIVATE
	/* Seems to yield better performance on Solaris 2, but can	*/
	/* be unreliable if something is already mapped at the address.	*/
#else
#   define GC_MMAP_FLAGS MAP_PRIVATE
#endif

#ifndef HEAP_START
#   define HEAP_START 0
#endif

ptr_t GC_unix_get_mem(bytes)
word bytes;
{
    void *result;
    static ptr_t last_addr = HEAP_START;

#   ifndef USE_MMAP_ANON
      static GC_bool initialized = FALSE;
      static int fd;

      if (!initialized) {
	  fd = open("/dev/zero", O_RDONLY);
	  fcntl(fd, F_SETFD, FD_CLOEXEC);
	  initialized = TRUE;
      }
#   endif

    if (bytes & (GC_page_size -1)) ABORT("Bad GET_MEM arg");
#   ifdef USE_MMAP_ANON
      result = mmap(last_addr, bytes, PROT_READ | PROT_WRITE | OPT_PROT_EXEC,
		    GC_MMAP_FLAGS | MAP_ANON, -1, 0/* offset */);
#   else
      result = mmap(last_addr, bytes, PROT_READ | PROT_WRITE | OPT_PROT_EXEC,
		    GC_MMAP_FLAGS, fd, 0/* offset */);
#   endif
    if (result == MAP_FAILED) return(0);
    last_addr = (ptr_t)result + bytes + GC_page_size - 1;
    last_addr = (ptr_t)((word)last_addr & ~(GC_page_size - 1));
#   if !defined(LINUX)
      if (last_addr == 0) {
        /* Oops.  We got the end of the address space.  This isn't	*/
	/* usable by arbitrary C code, since one-past-end pointers	*/
	/* don't work, so we discard it and try again.			*/
	munmap(result, (size_t)(-GC_page_size) - (size_t)result);
			/* Leave last page mapped, so we can't repeat. */
	return GC_unix_get_mem(bytes);
      }
#   else
      GC_ASSERT(last_addr != 0);
#   endif
    return((ptr_t)result);
}

#else /* Not RS6000, not USE_MMAP */
ptr_t GC_unix_get_mem(bytes)
word bytes;
{
  ptr_t result;
# ifdef IRIX5
    /* Bare sbrk isn't thread safe.  Play by malloc rules.	*/
    /* The equivalent may be needed on other systems as well. 	*/
    __LOCK_MALLOC();
# endif
  {
    ptr_t cur_brk = (ptr_t)sbrk(0);
    SBRK_ARG_T lsbs = (word)cur_brk & (GC_page_size-1);
    
    if ((SBRK_ARG_T)bytes < 0) return(0); /* too big */
    if (lsbs != 0) {
        if((ptr_t)sbrk(GC_page_size - lsbs) == (ptr_t)(-1)) return(0);
    }
    result = (ptr_t)sbrk((SBRK_ARG_T)bytes);
    if (result == (ptr_t)(-1)) result = 0;
  }
# ifdef IRIX5
    __UNLOCK_MALLOC();
# endif
  return(result);
}

#endif /* Not USE_MMAP */
#endif /* Not RS6000 */

# endif /* UN*X */

# ifdef OS2

void * os2_alloc(size_t bytes)
{
    void * result;

    if (DosAllocMem(&result, bytes, PAG_EXECUTE | PAG_READ |
    				    PAG_WRITE | PAG_COMMIT)
		    != NO_ERROR) {
	return(0);
    }
    if (result == 0) return(os2_alloc(bytes));
    return(result);
}

# endif /* OS2 */


# if defined(MSWIN32) || defined(MSWINCE)
SYSTEM_INFO GC_sysinfo;
# endif

# ifdef MSWIN32

# ifdef USE_GLOBAL_ALLOC
#   define GLOBAL_ALLOC_TEST 1
# else
#   define GLOBAL_ALLOC_TEST GC_no_win32_dlls
# endif

word GC_n_heap_bases = 0;

ptr_t GC_win32_get_mem(bytes)
word bytes;
{
    ptr_t result;

    if (GLOBAL_ALLOC_TEST) {
    	/* VirtualAlloc doesn't like PAGE_EXECUTE_READWRITE.	*/
    	/* There are also unconfirmed rumors of other		*/
    	/* problems, so we dodge the issue.			*/
        result = (ptr_t) GlobalAlloc(0, bytes + HBLKSIZE);
        result = (ptr_t)(((word)result + HBLKSIZE) & ~(HBLKSIZE-1));
    } else {
	/* VirtualProtect only works on regions returned by a	*/
	/* single VirtualAlloc call.  Thus we allocate one 	*/
	/* extra page, which will prevent merging of blocks	*/
	/* in separate regions, and eliminate any temptation	*/
	/* to call VirtualProtect on a range spanning regions.	*/
	/* This wastes a small amount of memory, and risks	*/
	/* increased fragmentation.  But better alternatives	*/
	/* would require effort.				*/
        result = (ptr_t) VirtualAlloc(NULL, bytes + 1,
    				      MEM_COMMIT | MEM_RESERVE,
    				      PAGE_EXECUTE_READWRITE);
    }
    if (HBLKDISPL(result) != 0) ABORT("Bad VirtualAlloc result");
    	/* If I read the documentation correctly, this can	*/
    	/* only happen if HBLKSIZE > 64k or not a power of 2.	*/
    if (GC_n_heap_bases >= MAX_HEAP_SECTS) ABORT("Too many heap sections");
    GC_heap_bases[GC_n_heap_bases++] = result;
    return(result);			  
}

void GC_win32_free_heap ()
{
    if (GC_no_win32_dlls) {
 	while (GC_n_heap_bases > 0) {
 	    GlobalFree (GC_heap_bases[--GC_n_heap_bases]);
 	    GC_heap_bases[GC_n_heap_bases] = 0;
 	}
    }
}
# endif

#ifdef AMIGA
# define GC_AMIGA_AM
# include "AmigaOS.c"
# undef GC_AMIGA_AM
#endif


# ifdef MSWINCE
word GC_n_heap_bases = 0;

ptr_t GC_wince_get_mem(bytes)
word bytes;
{
    ptr_t result;
    word i;

    /* Round up allocation size to multiple of page size */
    bytes = (bytes + GC_page_size-1) & ~(GC_page_size-1);

    /* Try to find reserved, uncommitted pages */
    for (i = 0; i < GC_n_heap_bases; i++) {
	if (((word)(-(signed_word)GC_heap_lengths[i])
	     & (GC_sysinfo.dwAllocationGranularity-1))
	    >= bytes) {
	    result = GC_heap_bases[i] + GC_heap_lengths[i];
	    break;
	}
    }

    if (i == GC_n_heap_bases) {
	/* Reserve more pages */
	word res_bytes = (bytes + GC_sysinfo.dwAllocationGranularity-1)
			 & ~(GC_sysinfo.dwAllocationGranularity-1);
	/* If we ever support MPROTECT_VDB here, we will probably need to	*/
	/* ensure that res_bytes is strictly > bytes, so that VirtualProtect	*/
	/* never spans regions.  It seems to be OK for a VirtualFree argument	*/
	/* to span regions, so we should be OK for now.				*/
	result = (ptr_t) VirtualAlloc(NULL, res_bytes,
    				      MEM_RESERVE | MEM_TOP_DOWN,
    				      PAGE_EXECUTE_READWRITE);
	if (HBLKDISPL(result) != 0) ABORT("Bad VirtualAlloc result");
    	    /* If I read the documentation correctly, this can	*/
    	    /* only happen if HBLKSIZE > 64k or not a power of 2.	*/
	if (GC_n_heap_bases >= MAX_HEAP_SECTS) ABORT("Too many heap sections");
	GC_heap_bases[GC_n_heap_bases] = result;
	GC_heap_lengths[GC_n_heap_bases] = 0;
	GC_n_heap_bases++;
    }

    /* Commit pages */
    result = (ptr_t) VirtualAlloc(result, bytes,
				  MEM_COMMIT,
    				  PAGE_EXECUTE_READWRITE);
    if (result != NULL) {
	if (HBLKDISPL(result) != 0) ABORT("Bad VirtualAlloc result");
	GC_heap_lengths[i] += bytes;
    }

    return(result);			  
}
# endif

#ifdef USE_MUNMAP

/* For now, this only works on Win32/WinCE and some Unix-like	*/
/* systems.  If you have something else, don't define		*/
/* USE_MUNMAP.							*/
/* We assume ANSI C to support this feature.			*/

#if !defined(MSWIN32) && !defined(MSWINCE)

#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#endif

/* Compute a page aligned starting address for the unmap 	*/
/* operation on a block of size bytes starting at start.	*/
/* Return 0 if the block is too small to make this feasible.	*/
ptr_t GC_unmap_start(ptr_t start, word bytes)
{
    ptr_t result = start;
    /* Round start to next page boundary.       */
        result += GC_page_size - 1;
        result = (ptr_t)((word)result & ~(GC_page_size - 1));
    if (result + GC_page_size > start + bytes) return 0;
    return result;
}

/* Compute end address for an unmap operation on the indicated	*/
/* block.							*/
ptr_t GC_unmap_end(ptr_t start, word bytes)
{
    ptr_t end_addr = start + bytes;
    end_addr = (ptr_t)((word)end_addr & ~(GC_page_size - 1));
    return end_addr;
}

/* Under Win32/WinCE we commit (map) and decommit (unmap)	*/
/* memory using	VirtualAlloc and VirtualFree.  These functions	*/
/* work on individual allocations of virtual memory, made	*/
/* previously using VirtualAlloc with the MEM_RESERVE flag.	*/
/* The ranges we need to (de)commit may span several of these	*/
/* allocations; therefore we use VirtualQuery to check		*/
/* allocation lengths, and split up the range as necessary.	*/

/* We assume that GC_remap is called on exactly the same range	*/
/* as a previous call to GC_unmap.  It is safe to consistently	*/
/* round the endpoints in both places.				*/
void GC_unmap(ptr_t start, word bytes)
{
    ptr_t start_addr = GC_unmap_start(start, bytes);
    ptr_t end_addr = GC_unmap_end(start, bytes);
    word len = end_addr - start_addr;
    if (0 == start_addr) return;
#   if defined(MSWIN32) || defined(MSWINCE)
      while (len != 0) {
          MEMORY_BASIC_INFORMATION mem_info;
	  GC_word free_len;
	  if (VirtualQuery(start_addr, &mem_info, sizeof(mem_info))
	      != sizeof(mem_info))
	      ABORT("Weird VirtualQuery result");
	  free_len = (len < mem_info.RegionSize) ? len : mem_info.RegionSize;
	  if (!VirtualFree(start_addr, free_len, MEM_DECOMMIT))
	      ABORT("VirtualFree failed");
	  GC_unmapped_bytes += free_len;
	  start_addr += free_len;
	  len -= free_len;
      }
#   else
      if (munmap(start_addr, len) != 0) ABORT("munmap failed");
      GC_unmapped_bytes += len;
#   endif
}


void GC_remap(ptr_t start, word bytes)
{
    static int zero_descr = -1;
    ptr_t start_addr = GC_unmap_start(start, bytes);
    ptr_t end_addr = GC_unmap_end(start, bytes);
    word len = end_addr - start_addr;
    ptr_t result;

#   if defined(MSWIN32) || defined(MSWINCE)
      if (0 == start_addr) return;
      while (len != 0) {
          MEMORY_BASIC_INFORMATION mem_info;
	  GC_word alloc_len;
	  if (VirtualQuery(start_addr, &mem_info, sizeof(mem_info))
	      != sizeof(mem_info))
	      ABORT("Weird VirtualQuery result");
	  alloc_len = (len < mem_info.RegionSize) ? len : mem_info.RegionSize;
	  result = VirtualAlloc(start_addr, alloc_len,
				MEM_COMMIT,
				PAGE_EXECUTE_READWRITE);
	  if (result != start_addr) {
	      ABORT("VirtualAlloc remapping failed");
	  }
	  GC_unmapped_bytes -= alloc_len;
	  start_addr += alloc_len;
	  len -= alloc_len;
      }
#   else
      if (-1 == zero_descr) zero_descr = open("/dev/zero", O_RDWR);
      fcntl(zero_descr, F_SETFD, FD_CLOEXEC);
      if (0 == start_addr) return;
      result = mmap(start_addr, len, PROT_READ | PROT_WRITE | OPT_PROT_EXEC,
		    MAP_FIXED | MAP_PRIVATE, zero_descr, 0);
      if (result != start_addr) {
	  ABORT("mmap remapping failed");
      }
      GC_unmapped_bytes -= len;
#   endif
}

/* Two adjacent blocks have already been unmapped and are about to	*/
/* be merged.  Unmap the whole block.  This typically requires		*/
/* that we unmap a small section in the middle that was not previously	*/
/* unmapped due to alignment constraints.				*/
void GC_unmap_gap(ptr_t start1, word bytes1, ptr_t start2, word bytes2)
{
    ptr_t start1_addr = GC_unmap_start(start1, bytes1);
    ptr_t end1_addr = GC_unmap_end(start1, bytes1);
    ptr_t start2_addr = GC_unmap_start(start2, bytes2);
    ptr_t end2_addr = GC_unmap_end(start2, bytes2);
    ptr_t start_addr = end1_addr;
    ptr_t end_addr = start2_addr;
    word len;
    GC_ASSERT(start1 + bytes1 == start2);
    if (0 == start1_addr) start_addr = GC_unmap_start(start1, bytes1 + bytes2);
    if (0 == start2_addr) end_addr = GC_unmap_end(start1, bytes1 + bytes2);
    if (0 == start_addr) return;
    len = end_addr - start_addr;
#   if defined(MSWIN32) || defined(MSWINCE)
      while (len != 0) {
          MEMORY_BASIC_INFORMATION mem_info;
	  GC_word free_len;
	  if (VirtualQuery(start_addr, &mem_info, sizeof(mem_info))
	      != sizeof(mem_info))
	      ABORT("Weird VirtualQuery result");
	  free_len = (len < mem_info.RegionSize) ? len : mem_info.RegionSize;
	  if (!VirtualFree(start_addr, free_len, MEM_DECOMMIT))
	      ABORT("VirtualFree failed");
	  GC_unmapped_bytes += free_len;
	  start_addr += free_len;
	  len -= free_len;
      }
#   else
      if (len != 0 && munmap(start_addr, len) != 0) ABORT("munmap failed");
      GC_unmapped_bytes += len;
#   endif
}

#endif /* USE_MUNMAP */

/* Routine for pushing any additional roots.  In THREADS 	*/
/* environment, this is also responsible for marking from 	*/
/* thread stacks. 						*/
#ifndef THREADS
void (*GC_push_other_roots)() = 0;
#else /* THREADS */

# ifdef PCR
PCR_ERes GC_push_thread_stack(PCR_Th_T *t, PCR_Any dummy)
{
    struct PCR_ThCtl_TInfoRep info;
    PCR_ERes result;
    
    info.ti_stkLow = info.ti_stkHi = 0;
    result = PCR_ThCtl_GetInfo(t, &info);
    GC_push_all_stack((ptr_t)(info.ti_stkLow), (ptr_t)(info.ti_stkHi));
    return(result);
}

/* Push the contents of an old object. We treat this as stack	*/
/* data only becasue that makes it robust against mark stack	*/
/* overflow.							*/
PCR_ERes GC_push_old_obj(void *p, size_t size, PCR_Any data)
{
    GC_push_all_stack((ptr_t)p, (ptr_t)p + size);
    return(PCR_ERes_okay);
}


void GC_default_push_other_roots GC_PROTO((void))
{
    /* Traverse data allocated by previous memory managers.		*/
	{
	  extern struct PCR_MM_ProcsRep * GC_old_allocator;
	  
	  if ((*(GC_old_allocator->mmp_enumerate))(PCR_Bool_false,
	  					   GC_push_old_obj, 0)
	      != PCR_ERes_okay) {
	      ABORT("Old object enumeration failed");
	  }
	}
    /* Traverse all thread stacks. */
	if (PCR_ERes_IsErr(
                PCR_ThCtl_ApplyToAllOtherThreads(GC_push_thread_stack,0))
              || PCR_ERes_IsErr(GC_push_thread_stack(PCR_Th_CurrThread(), 0))) {
              ABORT("Thread stack marking failed\n");
	}
}

# endif /* PCR */

# ifdef SRC_M3

# ifdef ALL_INTERIOR_POINTERS
    --> misconfigured
# endif

void GC_push_thread_structures GC_PROTO((void))
{
    /* Not our responsibibility. */
}

extern void ThreadF__ProcessStacks();

void GC_push_thread_stack(start, stop)
word start, stop;
{
   GC_push_all_stack((ptr_t)start, (ptr_t)stop + sizeof(word));
}

/* Push routine with M3 specific calling convention. */
GC_m3_push_root(dummy1, p, dummy2, dummy3)
word *p;
ptr_t dummy1, dummy2;
int dummy3;
{
    word q = *p;
    
    GC_PUSH_ONE_STACK(q, p);
}

/* M3 set equivalent to RTHeap.TracedRefTypes */
typedef struct { int elts[1]; }  RefTypeSet;
RefTypeSet GC_TracedRefTypes = {{0x1}};

void GC_default_push_other_roots GC_PROTO((void))
{
    /* Use the M3 provided routine for finding static roots.	 */
    /* This is a bit dubious, since it presumes no C roots.	 */
    /* We handle the collector roots explicitly in GC_push_roots */
      	RTMain__GlobalMapProc(GC_m3_push_root, 0, GC_TracedRefTypes);
	if (GC_words_allocd > 0) {
	    ThreadF__ProcessStacks(GC_push_thread_stack);
	}
	/* Otherwise this isn't absolutely necessary, and we have	*/
	/* startup ordering problems.					*/
}

# endif /* SRC_M3 */

# if defined(GC_SOLARIS_THREADS) || defined(GC_PTHREADS) || \
     defined(GC_WIN32_THREADS)

extern void GC_push_all_stacks();

void GC_default_push_other_roots GC_PROTO((void))
{
    GC_push_all_stacks();
}

# endif /* GC_SOLARIS_THREADS || GC_PTHREADS */

void (*GC_push_other_roots) GC_PROTO((void)) = GC_default_push_other_roots;

#endif /* THREADS */

/*
 * Routines for accessing dirty  bits on virtual pages.
 * We plan to eventually implement four strategies for doing so:
 * DEFAULT_VDB:	A simple dummy implementation that treats every page
 *		as possibly dirty.  This makes incremental collection
 *		useless, but the implementation is still correct.
 * PCR_VDB:	Use PPCRs virtual dirty bit facility.
 * PROC_VDB:	Use the /proc facility for reading dirty bits.  Only
 *		works under some SVR4 variants.  Even then, it may be
 *		too slow to be entirely satisfactory.  Requires reading
 *		dirty bits for entire address space.  Implementations tend
 *		to assume that the client is a (slow) debugger.
 * MPROTECT_VDB:Protect pages and then catch the faults to keep track of
 *		dirtied pages.  The implementation (and implementability)
 *		is highly system dependent.  This usually fails when system
 *		calls write to a protected page.  We prevent the read system
 *		call from doing so.  It is the clients responsibility to
 *		make sure that other system calls are similarly protected
 *		or write only to the stack.
 */
GC_bool GC_dirty_maintained = FALSE;

# ifdef DEFAULT_VDB

/* All of the following assume the allocation lock is held, and	*/
/* signals are disabled.					*/

/* The client asserts that unallocated pages in the heap are never	*/
/* written.								*/

/* Initialize virtual dirty bit implementation.			*/
void GC_dirty_init()
{
#   ifdef PRINTSTATS
      GC_printf0("Initializing DEFAULT_VDB...\n");
#   endif
    GC_dirty_maintained = TRUE;
}

/* Retrieve system dirty bits for heap to a local buffer.	*/
/* Restore the systems notion of which pages are dirty.		*/
void GC_read_dirty()
{}

/* Is the HBLKSIZE sized page at h marked dirty in the local buffer?	*/
/* If the actual page size is different, this returns TRUE if any	*/
/* of the pages overlapping h are dirty.  This routine may err on the	*/
/* side of labelling pages as dirty (and this implementation does).	*/
/*ARGSUSED*/
GC_bool GC_page_was_dirty(h)
struct hblk *h;
{
    return(TRUE);
}

/*
 * The following two routines are typically less crucial.  They matter
 * most with large dynamic libraries, or if we can't accurately identify
 * stacks, e.g. under Solaris 2.X.  Otherwise the following default
 * versions are adequate.
 */
 
/* Could any valid GC heap pointer ever have been written to this page?	*/
/*ARGSUSED*/
GC_bool GC_page_was_ever_dirty(h)
struct hblk *h;
{
    return(TRUE);
}

/* Reset the n pages starting at h to "was never dirty" status.	*/
void GC_is_fresh(h, n)
struct hblk *h;
word n;
{
}

/* A call that:						*/
/* I) hints that [h, h+nblocks) is about to be written.	*/
/* II) guarantees that protection is removed.		*/
/* (I) may speed up some dirty bit implementations.	*/
/* (II) may be essential if we need to ensure that	*/
/* pointer-free system call buffers in the heap are 	*/
/* not protected.					*/
/*ARGSUSED*/
void GC_remove_protection(h, nblocks, is_ptrfree)
struct hblk *h;
word nblocks;
GC_bool is_ptrfree;
{
}

# endif /* DEFAULT_VDB */


# ifdef MPROTECT_VDB

/*
 * See DEFAULT_VDB for interface descriptions.
 */

/*
 * This implementation maintains dirty bits itself by catching write
 * faults and keeping track of them.  We assume nobody else catches
 * SIGBUS or SIGSEGV.  We assume no write faults occur in system calls.
 * This means that clients must ensure that system calls don't write
 * to the write-protected heap.  Probably the best way to do this is to
 * ensure that system calls write at most to POINTERFREE objects in the
 * heap, and do even that only if we are on a platform on which those
 * are not protected.  Another alternative is to wrap system calls
 * (see example for read below), but the current implementation holds
 * a lock across blocking calls, making it problematic for multithreaded
 * applications. 
 * We assume the page size is a multiple of HBLKSIZE.
 * We prefer them to be the same.  We avoid protecting POINTERFREE
 * objects only if they are the same.
 */

# if !defined(MSWIN32) && !defined(MSWINCE) && !defined(DARWIN)

#   include <sys/mman.h>
#   include <signal.h>
#   include <sys/syscall.h>

#   define PROTECT(addr, len) \
    	  if (mprotect((caddr_t)(addr), (size_t)(len), \
    	      	       PROT_READ | OPT_PROT_EXEC) < 0) { \
    	    ABORT("mprotect failed"); \
    	  }
#   define UNPROTECT(addr, len) \
    	  if (mprotect((caddr_t)(addr), (size_t)(len), \
    	  	       PROT_WRITE | PROT_READ | OPT_PROT_EXEC ) < 0) { \
    	    ABORT("un-mprotect failed"); \
    	  }
    	  
# else

# ifdef DARWIN
    /* Using vm_protect (mach syscall) over mprotect (BSD syscall) seems to
       decrease the likelihood of some of the problems described below. */
    #include <mach/vm_map.h>
    extern mach_port_t GC_task_self;
    #define PROTECT(addr,len) \
        if(vm_protect(GC_task_self,(vm_address_t)(addr),(vm_size_t)(len), \
                FALSE,VM_PROT_READ) != KERN_SUCCESS) { \
            ABORT("vm_portect failed"); \
        }
    #define UNPROTECT(addr,len) \
        if(vm_protect(GC_task_self,(vm_address_t)(addr),(vm_size_t)(len), \
                FALSE,VM_PROT_READ|VM_PROT_WRITE) != KERN_SUCCESS) { \
            ABORT("vm_portect failed"); \
        }
# else
    
#   ifndef MSWINCE
#     include <signal.h>
#   endif

    static DWORD protect_junk;
#   define PROTECT(addr, len) \
	  if (!VirtualProtect((addr), (len), PAGE_EXECUTE_READ, \
	  		      &protect_junk)) { \
	    DWORD last_error = GetLastError(); \
	    GC_printf1("Last error code: %lx\n", last_error); \
	    ABORT("VirtualProtect failed"); \
	  }
#   define UNPROTECT(addr, len) \
	  if (!VirtualProtect((addr), (len), PAGE_EXECUTE_READWRITE, \
	  		      &protect_junk)) { \
	    ABORT("un-VirtualProtect failed"); \
	  }
# endif /* !DARWIN */
# endif /* MSWIN32 || MSWINCE || DARWIN */

#if defined(SUNOS4) || defined(FREEBSD)
    typedef void (* SIG_PF)();
#endif /* SUNOS4 || FREEBSD */

#if defined(SUNOS5SIGS) || defined(OSF1) || defined(LINUX) \
    || defined(HURD)
# ifdef __STDC__
    typedef void (* SIG_PF)(int);
# else
    typedef void (* SIG_PF)();
# endif
#endif /* SUNOS5SIGS || OSF1 || LINUX || HURD */

#if defined(MSWIN32)
    typedef LPTOP_LEVEL_EXCEPTION_FILTER SIG_PF;
#   undef SIG_DFL
#   define SIG_DFL (LPTOP_LEVEL_EXCEPTION_FILTER) (-1)
#endif
#if defined(MSWINCE)
    typedef LONG (WINAPI *SIG_PF)(struct _EXCEPTION_POINTERS *);
#   undef SIG_DFL
#   define SIG_DFL (SIG_PF) (-1)
#endif

#if defined(IRIX5) || defined(OSF1) || defined(HURD)
    typedef void (* REAL_SIG_PF)(int, int, struct sigcontext *);
#endif /* IRIX5 || OSF1 || HURD */

#if defined(SUNOS5SIGS)
# ifdef HPUX
#   define SIGINFO __siginfo
# else
#   define SIGINFO siginfo
# endif
# ifdef __STDC__
    typedef void (* REAL_SIG_PF)(int, struct SIGINFO *, void *);
# else
    typedef void (* REAL_SIG_PF)();
# endif
#endif /* SUNOS5SIGS */

#if defined(LINUX)
#   if __GLIBC__ > 2 || __GLIBC__ == 2 && __GLIBC_MINOR__ >= 2
      typedef struct sigcontext s_c;
#   else  /* glibc < 2.2 */
#     include <linux/version.h>
#     if (LINUX_VERSION_CODE >= 0x20100) && !defined(M68K) || defined(ALPHA) || defined(ARM32)
        typedef struct sigcontext s_c;
#     else
        typedef struct sigcontext_struct s_c;
#     endif
#   endif  /* glibc < 2.2 */
#   if defined(ALPHA) || defined(M68K)
      typedef void (* REAL_SIG_PF)(int, int, s_c *);
#   else
#     if defined(IA64) || defined(HP_PA)
        typedef void (* REAL_SIG_PF)(int, siginfo_t *, s_c *);
#     else
        typedef void (* REAL_SIG_PF)(int, s_c);
#     endif
#   endif
#   ifdef ALPHA
    /* Retrieve fault address from sigcontext structure by decoding	*/
    /* instruction.							*/
    char * get_fault_addr(s_c *sc) {
        unsigned instr;
	word faultaddr;

	instr = *((unsigned *)(sc->sc_pc));
	faultaddr = sc->sc_regs[(instr >> 16) & 0x1f];
	faultaddr += (word) (((int)instr << 16) >> 16);
	return (char *)faultaddr;
    }
#   endif /* !ALPHA */
# endif /* LINUX */

#ifndef DARWIN
SIG_PF GC_old_bus_handler;
SIG_PF GC_old_segv_handler;	/* Also old MSWIN32 ACCESS_VIOLATION filter */
#endif /* !DARWIN */

#if defined(THREADS)
/* We need to lock around the bitmap update in the write fault handler	*/
/* in order to avoid the risk of losing a bit.  We do this with a 	*/
/* test-and-set spin lock if we know how to do that.  Otherwise we	*/
/* check whether we are already in the handler and use the dumb but	*/
/* safe fallback algorithm of setting all bits in the word.		*/
/* Contention should be very rare, so we do the minimum to handle it	*/
/* correctly.								*/
#ifdef GC_TEST_AND_SET_DEFINED
  static VOLATILE unsigned int fault_handler_lock = 0;
  void async_set_pht_entry_from_index(VOLATILE page_hash_table db, int index) {
    while (GC_test_and_set(&fault_handler_lock)) {}
    /* Could also revert to set_pht_entry_from_index_safe if initial	*/
    /* GC_test_and_set fails.						*/
    set_pht_entry_from_index(db, index);
    GC_clear(&fault_handler_lock);
  }
#else /* !GC_TEST_AND_SET_DEFINED */
  /* THIS IS INCORRECT! The dirty bit vector may be temporarily wrong,	*/
  /* just before we notice the conflict and correct it. We may end up   */
  /* looking at it while it's wrong.  But this requires contention	*/
  /* exactly when a GC is triggered, which seems far less likely to	*/
  /* fail than the old code, which had no reported failures.  Thus we	*/
  /* leave it this way while we think of something better, or support	*/
  /* GC_test_and_set on the remaining platforms.			*/
  static VOLATILE word currently_updating = 0;
  void async_set_pht_entry_from_index(VOLATILE page_hash_table db, int index) {
    unsigned int update_dummy;
    currently_updating = (word)(&update_dummy);
    set_pht_entry_from_index(db, index);
    /* If we get contention in the 10 or so instruction window here,	*/
    /* and we get stopped by a GC between the two updates, we lose!	*/
    if (currently_updating != (word)(&update_dummy)) {
	set_pht_entry_from_index_safe(db, index);
	/* We claim that if two threads concurrently try to update the	*/
	/* dirty bit vector, the first one to execute UPDATE_START 	*/
	/* will see it changed when UPDATE_END is executed.  (Note that	*/
	/* &update_dummy must differ in two distinct threads.)  It	*/
	/* will then execute set_pht_entry_from_index_safe, thus 	*/
	/* returning us to a safe state, though not soon enough.	*/
    }
  }
#endif /* !GC_TEST_AND_SET_DEFINED */
#else /* !THREADS */
# define async_set_pht_entry_from_index(db, index) \
	set_pht_entry_from_index(db, index)
#endif /* !THREADS */

/*ARGSUSED*/
#if !defined(DARWIN)
# if defined (SUNOS4) || defined(FREEBSD)
    void GC_write_fault_handler(sig, code, scp, addr)
    int sig, code;
    struct sigcontext *scp;
    char * addr;
#   ifdef SUNOS4
#     define SIG_OK (sig == SIGSEGV || sig == SIGBUS)
#     define CODE_OK (FC_CODE(code) == FC_PROT \
              	    || (FC_CODE(code) == FC_OBJERR \
              	       && FC_ERRNO(code) == FC_PROT))
#   endif
#   ifdef FREEBSD
#     define SIG_OK (sig == SIGBUS)
#     define CODE_OK (code == BUS_PAGE_FAULT)
#   endif
# endif /* SUNOS4 || FREEBSD */

# if defined(IRIX5) || defined(OSF1) || defined(HURD)
#   include <errno.h>
    void GC_write_fault_handler(int sig, int code, struct sigcontext *scp)
#   ifdef OSF1
#     define SIG_OK (sig == SIGSEGV)
#     define CODE_OK (code == 2 /* experimentally determined */)
#   endif
#   ifdef IRIX5
#     define SIG_OK (sig == SIGSEGV)
#     define CODE_OK (code == EACCES)
#   endif
#   ifdef HURD
#     define SIG_OK (sig == SIGBUS || sig == SIGSEGV) 	
#     define CODE_OK  TRUE
#   endif
# endif /* IRIX5 || OSF1 || HURD */

# if defined(LINUX)
#   if defined(ALPHA) || defined(M68K)
      void GC_write_fault_handler(int sig, int code, s_c * sc)
#   else
#     if defined(IA64) || defined(HP_PA)
        void GC_write_fault_handler(int sig, siginfo_t * si, s_c * scp)
#     else
#       if defined(ARM32)
          void GC_write_fault_handler(int sig, int a2, int a3, int a4, s_c sc)
#       else
          void GC_write_fault_handler(int sig, s_c sc)
#       endif
#     endif
#   endif
#   define SIG_OK (sig == SIGSEGV)
#   define CODE_OK TRUE
	/* Empirically c.trapno == 14, on IA32, but is that useful?     */
	/* Should probably consider alignment issues on other 		*/
	/* architectures.						*/
# endif /* LINUX */

# if defined(SUNOS5SIGS)
#  ifdef __STDC__
    void GC_write_fault_handler(int sig, struct SIGINFO *scp, void * context)
#  else
    void GC_write_fault_handler(sig, scp, context)
    int sig;
    struct SIGINFO *scp;
    void * context;
#  endif
#   ifdef HPUX
#     define SIG_OK (sig == SIGSEGV || sig == SIGBUS)
#     define CODE_OK (scp -> si_code == SEGV_ACCERR) \
		     || (scp -> si_code == BUS_ADRERR) \
		     || (scp -> si_code == BUS_UNKNOWN) \
		     || (scp -> si_code == SEGV_UNKNOWN) \
		     || (scp -> si_code == BUS_OBJERR)
#   else
#     define SIG_OK (sig == SIGSEGV)
#     define CODE_OK (scp -> si_code == SEGV_ACCERR)
#   endif
# endif /* SUNOS5SIGS */

# if defined(MSWIN32) || defined(MSWINCE)
    LONG WINAPI GC_write_fault_handler(struct _EXCEPTION_POINTERS *exc_info)
#   define SIG_OK (exc_info -> ExceptionRecord -> ExceptionCode == \
			STATUS_ACCESS_VIOLATION)
#   define CODE_OK (exc_info -> ExceptionRecord -> ExceptionInformation[0] == 1)
			/* Write fault */
# endif /* MSWIN32 || MSWINCE */
{
    register unsigned i;
#   if defined(HURD) 
	char *addr = (char *) code;
#   endif
#   ifdef IRIX5
	char * addr = (char *) (size_t) (scp -> sc_badvaddr);
#   endif
#   if defined(OSF1) && defined(ALPHA)
	char * addr = (char *) (scp -> sc_traparg_a0);
#   endif
#   ifdef SUNOS5SIGS
	char * addr = (char *) (scp -> si_addr);
#   endif
#   ifdef LINUX
#     if defined(I386) || defined (X86_64)
	char * addr = (char *) (sc.cr2);
#     else
#	if defined(M68K)
          char * addr = NULL;

	  struct sigcontext *scp = (struct sigcontext *)(sc);

	  int format = (scp->sc_formatvec >> 12) & 0xf;
	  unsigned long *framedata = (unsigned long *)(scp + 1); 
	  unsigned long ea;

	  if (format == 0xa || format == 0xb) {
	  	/* 68020/030 */
	  	ea = framedata[2];
	  } else if (format == 7) {
	  	/* 68040 */
	  	ea = framedata[3];
	  	if (framedata[1] & 0x08000000) {
	  		/* correct addr on misaligned access */
	  		ea = (ea+4095)&(~4095);
		}
	  } else if (format == 4) {
	  	/* 68060 */
	  	ea = framedata[0];
	  	if (framedata[1] & 0x08000000) {
	  		/* correct addr on misaligned access */
	  		ea = (ea+4095)&(~4095);
	  	}
	  }	
	  addr = (char *)ea;
#	else
#	  ifdef ALPHA
            char * addr = get_fault_addr(sc);
#	  else
#	    if defined(IA64) || defined(HP_PA)
	      char * addr = si -> si_addr;
	      /* I believe this is claimed to work on all platforms for	*/
	      /* Linux 2.3.47 and later.  Hopefully we don't have to	*/
	      /* worry about earlier kernels on IA64.			*/
#	    else
#             if defined(POWERPC)
                char * addr = (char *) (sc.regs->dar);
#	      else
#               if defined(ARM32)
                  char * addr = (char *)sc.fault_address;
#               else
		  --> architecture not supported
#               endif
#	      endif
#	    endif
#	  endif
#	endif
#     endif
#   endif
#   if defined(MSWIN32) || defined(MSWINCE)
	char * addr = (char *) (exc_info -> ExceptionRecord
				-> ExceptionInformation[1]);
#	define sig SIGSEGV
#   endif
    
    if (SIG_OK && CODE_OK) {
        register struct hblk * h =
        		(struct hblk *)((word)addr & ~(GC_page_size-1));
        GC_bool in_allocd_block;
        
#	ifdef SUNOS5SIGS
	    /* Address is only within the correct physical page.	*/
	    in_allocd_block = FALSE;
            for (i = 0; i < divHBLKSZ(GC_page_size); i++) {
              if (HDR(h+i) != 0) {
                in_allocd_block = TRUE;
              }
            }
#	else
	    in_allocd_block = (HDR(addr) != 0);
#	endif
        if (!in_allocd_block) {
	    /* Heap blocks now begin and end on page boundaries */
            SIG_PF old_handler;
            
            if (sig == SIGSEGV) {
            	old_handler = GC_old_segv_handler;
            } else {
                old_handler = GC_old_bus_handler;
            }
            if (old_handler == SIG_DFL) {
#		if !defined(MSWIN32) && !defined(MSWINCE)
		    GC_err_printf1("Segfault at 0x%lx\n", addr);
                    ABORT("Unexpected bus error or segmentation fault");
#		else
		    return(EXCEPTION_CONTINUE_SEARCH);
#		endif
            } else {
#		if defined (SUNOS4) || defined(FREEBSD)
		    (*old_handler) (sig, code, scp, addr);
		    return;
#		endif
#		if defined (SUNOS5SIGS)
		    (*(REAL_SIG_PF)old_handler) (sig, scp, context);
		    return;
#		endif
#		if defined (LINUX)
#		    if defined(ALPHA) || defined(M68K)
		        (*(REAL_SIG_PF)old_handler) (sig, code, sc);
#		    else 
#		      if defined(IA64) || defined(HP_PA)
		        (*(REAL_SIG_PF)old_handler) (sig, si, scp);
#		      else
		        (*(REAL_SIG_PF)old_handler) (sig, sc);
#		      endif
#		    endif
		    return;
#		endif
#		if defined (IRIX5) || defined(OSF1) || defined(HURD)
		    (*(REAL_SIG_PF)old_handler) (sig, code, scp);
		    return;
#		endif
#		ifdef MSWIN32
		    return((*old_handler)(exc_info));
#		endif
            }
        }
        UNPROTECT(h, GC_page_size);
	/* We need to make sure that no collection occurs between	*/
	/* the UNPROTECT and the setting of the dirty bit.  Otherwise	*/
	/* a write by a third thread might go unnoticed.  Reversing	*/
	/* the order is just as bad, since we would end up unprotecting	*/
	/* a page in a GC cycle during which it's not marked.		*/
	/* Currently we do this by disabling the thread stopping	*/
	/* signals while this handler is running.  An alternative might	*/
	/* be to record the fact that we're about to unprotect, or	*/
	/* have just unprotected a page in the GC's thread structure,	*/
	/* and then to have the thread stopping code set the dirty	*/
	/* flag, if necessary.						*/
        for (i = 0; i < divHBLKSZ(GC_page_size); i++) {
            register int index = PHT_HASH(h+i);
            
            async_set_pht_entry_from_index(GC_dirty_pages, index);
        }
#	if defined(OSF1)
	    /* These reset the signal handler each time by default. */
	    signal(SIGSEGV, (SIG_PF) GC_write_fault_handler);
#	endif
    	/* The write may not take place before dirty bits are read.	*/
    	/* But then we'll fault again ...				*/
#	if defined(MSWIN32) || defined(MSWINCE)
	    return(EXCEPTION_CONTINUE_EXECUTION);
#	else
	    return;
#	endif
    }
#if defined(MSWIN32) || defined(MSWINCE)
    return EXCEPTION_CONTINUE_SEARCH;
#else
    GC_err_printf1("Segfault at 0x%lx\n", addr);
    ABORT("Unexpected bus error or segmentation fault");
#endif
}
#endif /* !DARWIN */

/*
 * We hold the allocation lock.  We expect block h to be written
 * shortly.  Ensure that all pages containing any part of the n hblks
 * starting at h are no longer protected.  If is_ptrfree is false,
 * also ensure that they will subsequently appear to be dirty.
 */
void GC_remove_protection(h, nblocks, is_ptrfree)
struct hblk *h;
word nblocks;
GC_bool is_ptrfree;
{
    struct hblk * h_trunc;  /* Truncated to page boundary */
    struct hblk * h_end;    /* Page boundary following block end */
    struct hblk * current;
    GC_bool found_clean;
    
    if (!GC_dirty_maintained) return;
    h_trunc = (struct hblk *)((word)h & ~(GC_page_size-1));
    h_end = (struct hblk *)(((word)(h + nblocks) + GC_page_size-1)
	                    & ~(GC_page_size-1));
    found_clean = FALSE;
    for (current = h_trunc; current < h_end; ++current) {
        int index = PHT_HASH(current);
            
        if (!is_ptrfree || current < h || current >= h + nblocks) {
            async_set_pht_entry_from_index(GC_dirty_pages, index);
        }
    }
    UNPROTECT(h_trunc, (ptr_t)h_end - (ptr_t)h_trunc);
}

#if !defined(DARWIN)
void GC_dirty_init()
{
#   if defined(SUNOS5SIGS) || defined(IRIX5) || defined(LINUX) || \
       defined(OSF1) || defined(HURD)
      struct sigaction	act, oldact;
      /* We should probably specify SA_SIGINFO for Linux, and handle 	*/
      /* the different architectures more uniformly.			*/
#     if defined(IRIX5) || defined(LINUX) || defined(OSF1) || defined(HURD)
    	act.sa_flags	= SA_RESTART;
        act.sa_handler  = (SIG_PF)GC_write_fault_handler;
#     else
    	act.sa_flags	= SA_RESTART | SA_SIGINFO;
        act.sa_sigaction = GC_write_fault_handler;
#     endif
      (void)sigemptyset(&act.sa_mask);
#     ifdef SIG_SUSPEND
        /* Arrange to postpone SIG_SUSPEND while we're in a write fault	*/
        /* handler.  This effectively makes the handler atomic w.r.t.	*/
        /* stopping the world for GC.					*/
        (void)sigaddset(&act.sa_mask, SIG_SUSPEND);
#     endif /* SIG_SUSPEND */
#    endif
#   ifdef PRINTSTATS
	GC_printf0("Inititalizing mprotect virtual dirty bit implementation\n");
#   endif
    GC_dirty_maintained = TRUE;
    if (GC_page_size % HBLKSIZE != 0) {
        GC_err_printf0("Page size not multiple of HBLKSIZE\n");
        ABORT("Page size not multiple of HBLKSIZE");
    }
#   if defined(SUNOS4) || defined(FREEBSD)
      GC_old_bus_handler = signal(SIGBUS, GC_write_fault_handler);
      if (GC_old_bus_handler == SIG_IGN) {
        GC_err_printf0("Previously ignored bus error!?");
        GC_old_bus_handler = SIG_DFL;
      }
      if (GC_old_bus_handler != SIG_DFL) {
#	ifdef PRINTSTATS
          GC_err_printf0("Replaced other SIGBUS handler\n");
#	endif
      }
#   endif
#   if defined(SUNOS4)
      GC_old_segv_handler = signal(SIGSEGV, (SIG_PF)GC_write_fault_handler);
      if (GC_old_segv_handler == SIG_IGN) {
        GC_err_printf0("Previously ignored segmentation violation!?");
        GC_old_segv_handler = SIG_DFL;
      }
      if (GC_old_segv_handler != SIG_DFL) {
#	ifdef PRINTSTATS
          GC_err_printf0("Replaced other SIGSEGV handler\n");
#	endif
      }
#   endif
#   if defined(SUNOS5SIGS) || defined(IRIX5) || defined(LINUX) \
       || defined(OSF1) || defined(HURD)
      /* SUNOS5SIGS includes HPUX */
#     if defined(GC_IRIX_THREADS)
      	sigaction(SIGSEGV, 0, &oldact);
      	sigaction(SIGSEGV, &act, 0);
#     else
	{
	  int res = sigaction(SIGSEGV, &act, &oldact);
	  if (res != 0) ABORT("Sigaction failed");
 	}
#     endif
#     if defined(_sigargs) || defined(HURD) || !defined(SA_SIGINFO)
	/* This is Irix 5.x, not 6.x.  Irix 5.x does not have	*/
	/* sa_sigaction.					*/
	GC_old_segv_handler = oldact.sa_handler;
#     else /* Irix 6.x or SUNOS5SIGS or LINUX */
        if (oldact.sa_flags & SA_SIGINFO) {
          GC_old_segv_handler = (SIG_PF)(oldact.sa_sigaction);
        } else {
          GC_old_segv_handler = oldact.sa_handler;
        }
#     endif
      if (GC_old_segv_handler == SIG_IGN) {
	     GC_err_printf0("Previously ignored segmentation violation!?");
	     GC_old_segv_handler = SIG_DFL;
      }
      if (GC_old_segv_handler != SIG_DFL) {
#       ifdef PRINTSTATS
	  GC_err_printf0("Replaced other SIGSEGV handler\n");
#       endif
      }
#   endif
#   if defined(HPUX) || defined(LINUX) || defined(HURD)
      sigaction(SIGBUS, &act, &oldact);
      GC_old_bus_handler = oldact.sa_handler;
      if (GC_old_bus_handler == SIG_IGN) {
	     GC_err_printf0("Previously ignored bus error!?");
	     GC_old_bus_handler = SIG_DFL;
      }
      if (GC_old_bus_handler != SIG_DFL) {
#       ifdef PRINTSTATS
	  GC_err_printf0("Replaced other SIGBUS handler\n");
#       endif
      }
#   endif /* HPUX || LINUX || HURD */
#   if defined(MSWIN32)
      GC_old_segv_handler = SetUnhandledExceptionFilter(GC_write_fault_handler);
      if (GC_old_segv_handler != NULL) {
#	ifdef PRINTSTATS
          GC_err_printf0("Replaced other UnhandledExceptionFilter\n");
#	endif
      } else {
          GC_old_segv_handler = SIG_DFL;
      }
#   endif
}
#endif /* !DARWIN */

int GC_incremental_protection_needs()
{
    if (GC_page_size == HBLKSIZE) {
	return GC_PROTECTS_POINTER_HEAP;
    } else {
	return GC_PROTECTS_POINTER_HEAP | GC_PROTECTS_PTRFREE_HEAP;
    }
}

#define HAVE_INCREMENTAL_PROTECTION_NEEDS

#define IS_PTRFREE(hhdr) ((hhdr)->hb_descr == 0)

#define PAGE_ALIGNED(x) !((word)(x) & (GC_page_size - 1))
void GC_protect_heap()
{
    ptr_t start;
    word len;
    struct hblk * current;
    struct hblk * current_start;  /* Start of block to be protected. */
    struct hblk * limit;
    unsigned i;
    GC_bool protect_all = 
	  (0 != (GC_incremental_protection_needs() & GC_PROTECTS_PTRFREE_HEAP));
    for (i = 0; i < GC_n_heap_sects; i++) {
        start = GC_heap_sects[i].hs_start;
        len = GC_heap_sects[i].hs_bytes;
	if (protect_all) {
          PROTECT(start, len);
	} else {
	  GC_ASSERT(PAGE_ALIGNED(len))
	  GC_ASSERT(PAGE_ALIGNED(start))
	  current_start = current = (struct hblk *)start;
	  limit = (struct hblk *)(start + len);
	  while (current < limit) {
            hdr * hhdr;
	    word nhblks;
	    GC_bool is_ptrfree;

	    GC_ASSERT(PAGE_ALIGNED(current));
	    GET_HDR(current, hhdr);
	    if (IS_FORWARDING_ADDR_OR_NIL(hhdr)) {
	      /* This can happen only if we're at the beginning of a 	*/
	      /* heap segment, and a block spans heap segments.		*/
	      /* We will handle that block as part of the preceding	*/
	      /* segment.						*/
	      GC_ASSERT(current_start == current);
	      current_start = ++current;
	      continue;
	    }
	    if (HBLK_IS_FREE(hhdr)) {
	      GC_ASSERT(PAGE_ALIGNED(hhdr -> hb_sz));
	      nhblks = divHBLKSZ(hhdr -> hb_sz);
	      is_ptrfree = TRUE;	/* dirty on alloc */
	    } else {
	      nhblks = OBJ_SZ_TO_BLOCKS(hhdr -> hb_sz);
	      is_ptrfree = IS_PTRFREE(hhdr);
	    }
	    if (is_ptrfree) {
	      if (current_start < current) {
		PROTECT(current_start, (ptr_t)current - (ptr_t)current_start);
	      }
	      current_start = (current += nhblks);
	    } else {
	      current += nhblks;
	    }
	  } 
	  if (current_start < current) {
	    PROTECT(current_start, (ptr_t)current - (ptr_t)current_start);
	  }
	}
    }
}

/* We assume that either the world is stopped or its OK to lose dirty	*/
/* bits while this is happenning (as in GC_enable_incremental).		*/
void GC_read_dirty()
{
    BCOPY((word *)GC_dirty_pages, GC_grungy_pages,
          (sizeof GC_dirty_pages));
    BZERO((word *)GC_dirty_pages, (sizeof GC_dirty_pages));
    GC_protect_heap();
}

GC_bool GC_page_was_dirty(h)
struct hblk * h;
{
    register word index = PHT_HASH(h);
    
    return(HDR(h) == 0 || get_pht_entry_from_index(GC_grungy_pages, index));
}

/*
 * Acquiring the allocation lock here is dangerous, since this
 * can be called from within GC_call_with_alloc_lock, and the cord
 * package does so.  On systems that allow nested lock acquisition, this
 * happens to work.
 * On other systems, SET_LOCK_HOLDER and friends must be suitably defined.
 */

static GC_bool syscall_acquired_lock = FALSE;	/* Protected by GC lock. */
 
void GC_begin_syscall()
{
    if (!I_HOLD_LOCK()) {
	LOCK();
	syscall_acquired_lock = TRUE;
    }
}

void GC_end_syscall()
{
    if (syscall_acquired_lock) {
	syscall_acquired_lock = FALSE;
	UNLOCK();
    }
}

void GC_unprotect_range(addr, len)
ptr_t addr;
word len;
{
    struct hblk * start_block;
    struct hblk * end_block;
    register struct hblk *h;
    ptr_t obj_start;
    
    if (!GC_dirty_maintained) return;
    obj_start = GC_base(addr);
    if (obj_start == 0) return;
    if (GC_base(addr + len - 1) != obj_start) {
        ABORT("GC_unprotect_range(range bigger than object)");
    }
    start_block = (struct hblk *)((word)addr & ~(GC_page_size - 1));
    end_block = (struct hblk *)((word)(addr + len - 1) & ~(GC_page_size - 1));
    end_block += GC_page_size/HBLKSIZE - 1;
    for (h = start_block; h <= end_block; h++) {
        register word index = PHT_HASH(h);
        
        async_set_pht_entry_from_index(GC_dirty_pages, index);
    }
    UNPROTECT(start_block,
    	      ((ptr_t)end_block - (ptr_t)start_block) + HBLKSIZE);
}

#if 0

/* We no longer wrap read by default, since that was causing too many	*/
/* problems.  It is preferred that the client instead avoids writing	*/
/* to the write-protected heap with a system call.			*/
/* This still serves as sample code if you do want to wrap system calls.*/

#if !defined(MSWIN32) && !defined(MSWINCE) && !defined(GC_USE_LD_WRAP)
/* Replacement for UNIX system call.					  */
/* Other calls that write to the heap should be handled similarly.	  */
/* Note that this doesn't work well for blocking reads:  It will hold	  */
/* the allocation lock for the entire duration of the call. Multithreaded */
/* clients should really ensure that it won't block, either by setting 	  */
/* the descriptor nonblocking, or by calling select or poll first, to	  */
/* make sure that input is available.					  */
/* Another, preferred alternative is to ensure that system calls never 	  */
/* write to the protected heap (see above).				  */
# if defined(__STDC__) && !defined(SUNOS4)
#   include <unistd.h>
#   include <sys/uio.h>
    ssize_t read(int fd, void *buf, size_t nbyte)
# else
#   ifndef LINT
      int read(fd, buf, nbyte)
#   else
      int GC_read(fd, buf, nbyte)
#   endif
    int fd;
    char *buf;
    int nbyte;
# endif
{
    int result;
    
    GC_begin_syscall();
    GC_unprotect_range(buf, (word)nbyte);
#   if defined(IRIX5) || defined(GC_LINUX_THREADS)
	/* Indirect system call may not always be easily available.	*/
	/* We could call _read, but that would interfere with the	*/
	/* libpthread interception of read.				*/
	/* On Linux, we have to be careful with the linuxthreads	*/
	/* read interception.						*/
	{
	    struct iovec iov;

	    iov.iov_base = buf;
	    iov.iov_len = nbyte;
	    result = readv(fd, &iov, 1);
	}
#   else
#     if defined(HURD)	
	result = __read(fd, buf, nbyte);
#     else
 	/* The two zero args at the end of this list are because one
 	   IA-64 syscall() implementation actually requires six args
 	   to be passed, even though they aren't always used. */
     	result = syscall(SYS_read, fd, buf, nbyte, 0, 0);
#     endif /* !HURD */
#   endif
    GC_end_syscall();
    return(result);
}
#endif /* !MSWIN32 && !MSWINCE && !GC_LINUX_THREADS */

#if defined(GC_USE_LD_WRAP) && !defined(THREADS)
    /* We use the GNU ld call wrapping facility.			*/
    /* This requires that the linker be invoked with "--wrap read".	*/
    /* This can be done by passing -Wl,"--wrap read" to gcc.		*/
    /* I'm not sure that this actually wraps whatever version of read	*/
    /* is called by stdio.  That code also mentions __read.		*/
#   include <unistd.h>
    ssize_t __wrap_read(int fd, void *buf, size_t nbyte)
    {
 	int result;

	GC_begin_syscall();
    	GC_unprotect_range(buf, (word)nbyte);
	result = __real_read(fd, buf, nbyte);
	GC_end_syscall();
	return(result);
    }

    /* We should probably also do this for __read, or whatever stdio	*/
    /* actually calls.							*/
#endif

#endif /* 0 */

/*ARGSUSED*/
GC_bool GC_page_was_ever_dirty(h)
struct hblk *h;
{
    return(TRUE);
}

/* Reset the n pages starting at h to "was never dirty" status.	*/
/*ARGSUSED*/
void GC_is_fresh(h, n)
struct hblk *h;
word n;
{
}

# endif /* MPROTECT_VDB */

# ifdef PROC_VDB

/*
 * See DEFAULT_VDB for interface descriptions.
 */
 
/*
 * This implementaion assumes a Solaris 2.X like /proc pseudo-file-system
 * from which we can read page modified bits.  This facility is far from
 * optimal (e.g. we would like to get the info for only some of the
 * address space), but it avoids intercepting system calls.
 */

#include <errno.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/fault.h>
#include <sys/syscall.h>
#include <sys/procfs.h>
#include <sys/stat.h>

#define INITIAL_BUF_SZ 4096
word GC_proc_buf_size = INITIAL_BUF_SZ;
char *GC_proc_buf;

#ifdef GC_SOLARIS_THREADS
/* We don't have exact sp values for threads.  So we count on	*/
/* occasionally declaring stack pages to be fresh.  Thus we 	*/
/* need a real implementation of GC_is_fresh.  We can't clear	*/
/* entries in GC_written_pages, since that would declare all	*/
/* pages with the given hash address to be fresh.		*/
#   define MAX_FRESH_PAGES 8*1024	/* Must be power of 2 */
    struct hblk ** GC_fresh_pages;	/* A direct mapped cache.	*/
    					/* Collisions are dropped.	*/

#   define FRESH_PAGE_SLOT(h) (divHBLKSZ((word)(h)) & (MAX_FRESH_PAGES-1))
#   define ADD_FRESH_PAGE(h) \
	GC_fresh_pages[FRESH_PAGE_SLOT(h)] = (h)
#   define PAGE_IS_FRESH(h) \
	(GC_fresh_pages[FRESH_PAGE_SLOT(h)] == (h) && (h) != 0)
#endif

/* Add all pages in pht2 to pht1 */
void GC_or_pages(pht1, pht2)
page_hash_table pht1, pht2;
{
    register int i;
    
    for (i = 0; i < PHT_SIZE; i++) pht1[i] |= pht2[i];
}

int GC_proc_fd;

void GC_dirty_init()
{
    int fd;
    char buf[30];

    GC_dirty_maintained = TRUE;
    if (GC_words_allocd != 0 || GC_words_allocd_before_gc != 0) {
    	register int i;
    
        for (i = 0; i < PHT_SIZE; i++) GC_written_pages[i] = (word)(-1);
#       ifdef PRINTSTATS
	    GC_printf1("Allocated words:%lu:all pages may have been written\n",
	    	       (unsigned long)
	    	      		(GC_words_allocd + GC_words_allocd_before_gc));
#	endif       
    }
    sprintf(buf, "/proc/%d", getpid());
    fd = open(buf, O_RDONLY);
    if (fd < 0) {
    	ABORT("/proc open failed");
    }
    GC_proc_fd = syscall(SYS_ioctl, fd, PIOCOPENPD, 0);
    close(fd);
    syscall(SYS_fcntl, GC_proc_fd, F_SETFD, FD_CLOEXEC);
    if (GC_proc_fd < 0) {
    	ABORT("/proc ioctl failed");
    }
    GC_proc_buf = GC_scratch_alloc(GC_proc_buf_size);
#   ifdef GC_SOLARIS_THREADS
	GC_fresh_pages = (struct hblk **)
	  GC_scratch_alloc(MAX_FRESH_PAGES * sizeof (struct hblk *));
	if (GC_fresh_pages == 0) {
	    GC_err_printf0("No space for fresh pages\n");
	    EXIT();
	}
	BZERO(GC_fresh_pages, MAX_FRESH_PAGES * sizeof (struct hblk *));
#   endif
}

/* Ignore write hints. They don't help us here.	*/
/*ARGSUSED*/
void GC_remove_protection(h, nblocks, is_ptrfree)
struct hblk *h;
word nblocks;
GC_bool is_ptrfree;
{
}

#ifdef GC_SOLARIS_THREADS
#   define READ(fd,buf,nbytes) syscall(SYS_read, fd, buf, nbytes)
#else
#   define READ(fd,buf,nbytes) read(fd, buf, nbytes)
#endif

void GC_read_dirty()
{
    unsigned long ps, np;
    int nmaps;
    ptr_t vaddr;
    struct prasmap * map;
    char * bufp;
    ptr_t current_addr, limit;
    int i;
int dummy;

    BZERO(GC_grungy_pages, (sizeof GC_grungy_pages));
    
    bufp = GC_proc_buf;
    if (READ(GC_proc_fd, bufp, GC_proc_buf_size) <= 0) {
#	ifdef PRINTSTATS
            GC_printf1("/proc read failed: GC_proc_buf_size = %lu\n",
            	       GC_proc_buf_size);
#	endif       
        {
            /* Retry with larger buffer. */
            word new_size = 2 * GC_proc_buf_size;
            char * new_buf = GC_scratch_alloc(new_size);
            
            if (new_buf != 0) {
                GC_proc_buf = bufp = new_buf;
                GC_proc_buf_size = new_size;
            }
            if (syscall(SYS_read, GC_proc_fd, bufp, GC_proc_buf_size) <= 0) {
                WARN("Insufficient space for /proc read\n", 0);
                /* Punt:	*/
        	memset(GC_grungy_pages, 0xff, sizeof (page_hash_table));
		memset(GC_written_pages, 0xff, sizeof(page_hash_table));
#		ifdef GC_SOLARIS_THREADS
		    BZERO(GC_fresh_pages,
		    	  MAX_FRESH_PAGES * sizeof (struct hblk *)); 
#		endif
		return;
            }
        }
    }
    /* Copy dirty bits into GC_grungy_pages */
    	nmaps = ((struct prpageheader *)bufp) -> pr_nmap;
	/* printf( "nmaps = %d, PG_REFERENCED = %d, PG_MODIFIED = %d\n",
		     nmaps, PG_REFERENCED, PG_MODIFIED); */
	bufp = bufp + sizeof(struct prpageheader);
	for (i = 0; i < nmaps; i++) {
	    map = (struct prasmap *)bufp;
	    vaddr = (ptr_t)(map -> pr_vaddr);
	    ps = map -> pr_pagesize;
	    np = map -> pr_npage;
	    /* printf("vaddr = 0x%X, ps = 0x%X, np = 0x%X\n", vaddr, ps, np); */
	    limit = vaddr + ps * np;
	    bufp += sizeof (struct prasmap);
	    for (current_addr = vaddr;
	         current_addr < limit; current_addr += ps){
	        if ((*bufp++) & PG_MODIFIED) {
	            register struct hblk * h = (struct hblk *) current_addr;
	            
	            while ((ptr_t)h < current_addr + ps) {
	                register word index = PHT_HASH(h);
	                
	                set_pht_entry_from_index(GC_grungy_pages, index);
#			ifdef GC_SOLARIS_THREADS
			  {
			    register int slot = FRESH_PAGE_SLOT(h);
			    
			    if (GC_fresh_pages[slot] == h) {
			        GC_fresh_pages[slot] = 0;
			    }
			  }
#			endif
	                h++;
	            }
	        }
	    }
	    bufp += sizeof(long) - 1;
	    bufp = (char *)((unsigned long)bufp & ~(sizeof(long)-1));
	}
    /* Update GC_written_pages. */
        GC_or_pages(GC_written_pages, GC_grungy_pages);
#   ifdef GC_SOLARIS_THREADS
      /* Make sure that old stacks are considered completely clean	*/
      /* unless written again.						*/
	GC_old_stacks_are_fresh();
#   endif
}

#undef READ

GC_bool GC_page_was_dirty(h)
struct hblk *h;
{
    register word index = PHT_HASH(h);
    register GC_bool result;
    
    result = get_pht_entry_from_index(GC_grungy_pages, index);
#   ifdef GC_SOLARIS_THREADS
	if (result && PAGE_IS_FRESH(h)) result = FALSE;
	/* This happens only if page was declared fresh since	*/
	/* the read_dirty call, e.g. because it's in an unused  */
	/* thread stack.  It's OK to treat it as clean, in	*/
	/* that case.  And it's consistent with 		*/
	/* GC_page_was_ever_dirty.				*/
#   endif
    return(result);
}

GC_bool GC_page_was_ever_dirty(h)
struct hblk *h;
{
    register word index = PHT_HASH(h);
    register GC_bool result;
    
    result = get_pht_entry_from_index(GC_written_pages, index);
#   ifdef GC_SOLARIS_THREADS
	if (result && PAGE_IS_FRESH(h)) result = FALSE;
#   endif
    return(result);
}

/* Caller holds allocation lock.	*/
void GC_is_fresh(h, n)
struct hblk *h;
word n;
{

    register word index;
    
#   ifdef GC_SOLARIS_THREADS
      register word i;
      
      if (GC_fresh_pages != 0) {
        for (i = 0; i < n; i++) {
          ADD_FRESH_PAGE(h + i);
        }
      }
#   endif
}

# endif /* PROC_VDB */


# ifdef PCR_VDB

# include "vd/PCR_VD.h"

# define NPAGES (32*1024)	/* 128 MB */

PCR_VD_DB  GC_grungy_bits[NPAGES];

ptr_t GC_vd_base;	/* Address corresponding to GC_grungy_bits[0]	*/
			/* HBLKSIZE aligned.				*/

void GC_dirty_init()
{
    GC_dirty_maintained = TRUE;
    /* For the time being, we assume the heap generally grows up */
    GC_vd_base = GC_heap_sects[0].hs_start;
    if (GC_vd_base == 0) {
   	ABORT("Bad initial heap segment");
    }
    if (PCR_VD_Start(HBLKSIZE, GC_vd_base, NPAGES*HBLKSIZE)
	!= PCR_ERes_okay) {
	ABORT("dirty bit initialization failed");
    }
}

void GC_read_dirty()
{
    /* lazily enable dirty bits on newly added heap sects */
    {
        static int onhs = 0;
        int nhs = GC_n_heap_sects;
        for( ; onhs < nhs; onhs++ ) {
            PCR_VD_WriteProtectEnable(
                    GC_heap_sects[onhs].hs_start,
                    GC_heap_sects[onhs].hs_bytes );
        }
    }


    if (PCR_VD_Clear(GC_vd_base, NPAGES*HBLKSIZE, GC_grungy_bits)
        != PCR_ERes_okay) {
	ABORT("dirty bit read failed");
    }
}

GC_bool GC_page_was_dirty(h)
struct hblk *h;
{
    if((ptr_t)h < GC_vd_base || (ptr_t)h >= GC_vd_base + NPAGES*HBLKSIZE) {
	return(TRUE);
    }
    return(GC_grungy_bits[h - (struct hblk *)GC_vd_base] & PCR_VD_DB_dirtyBit);
}

/*ARGSUSED*/
void GC_remove_protection(h, nblocks, is_ptrfree)
struct hblk *h;
word nblocks;
GC_bool is_ptrfree;
{
    PCR_VD_WriteProtectDisable(h, nblocks*HBLKSIZE);
    PCR_VD_WriteProtectEnable(h, nblocks*HBLKSIZE);
}

# endif /* PCR_VDB */

#if defined(MPROTECT_VDB) && defined(DARWIN)
/* The following sources were used as a *reference* for this exception handling
   code:
      1. Apple's mach/xnu documentation
      2. Timothy J. Wood's "Mach Exception Handlers 101" post to the
         omnigroup's macosx-dev list. 
         www.omnigroup.com/mailman/archive/macosx-dev/2000-June/002030.html
      3. macosx-nat.c from Apple's GDB source code.
*/
   
/* There seem to be numerous problems with darwin's mach exception handling.
   I'm pretty sure they are not problems in my code. Search for 
   BROKEN_EXCEPTION_HANDLING for more information. */
#define BROKEN_EXCEPTION_HANDLING
   
#include <mach/mach.h>
#include <mach/mach_error.h>
#include <mach/thread_status.h>
#include <mach/exception.h>
#include <mach/task.h>
#include <pthread.h>

/* These are not defined in any header, although they are documented */
extern boolean_t exc_server(mach_msg_header_t *,mach_msg_header_t *);
extern kern_return_t exception_raise(
    mach_port_t,mach_port_t,mach_port_t,
    exception_type_t,exception_data_t,mach_msg_type_number_t);
extern kern_return_t exception_raise_state(
    mach_port_t,mach_port_t,mach_port_t,
    exception_type_t,exception_data_t,mach_msg_type_number_t,
    thread_state_flavor_t*,thread_state_t,mach_msg_type_number_t,
    thread_state_t,mach_msg_type_number_t*);
extern kern_return_t exception_raise_state_identity(
    mach_port_t,mach_port_t,mach_port_t,
    exception_type_t,exception_data_t,mach_msg_type_number_t,
    thread_state_flavor_t*,thread_state_t,mach_msg_type_number_t,
    thread_state_t,mach_msg_type_number_t*);


#define MAX_EXCEPTION_PORTS 16

static mach_port_t GC_task_self;

static struct {
    mach_msg_type_number_t count;
    exception_mask_t      masks[MAX_EXCEPTION_PORTS];
    exception_handler_t   ports[MAX_EXCEPTION_PORTS];
    exception_behavior_t  behaviors[MAX_EXCEPTION_PORTS];
    thread_state_flavor_t flavors[MAX_EXCEPTION_PORTS];
} GC_old_exc_ports;

static struct {
    mach_port_t exception;
#if defined(THREADS)
    mach_port_t reply;
#endif
} GC_ports;

typedef struct {
    mach_msg_header_t head;
} GC_msg_t;

typedef enum {
    GC_MP_NORMAL, GC_MP_DISCARDING, GC_MP_STOPPED
} GC_mprotect_state_t;

/* FIXME: 1 and 2 seem to be safe to use in the msgh_id field,
   but it isn't  documented. Use the source and see if they
   should be ok. */
#define ID_STOP 1
#define ID_RESUME 2

/* These values are only used on the reply port */
#define ID_ACK 3

#if defined(THREADS)

GC_mprotect_state_t GC_mprotect_state;

/* The following should ONLY be called when the world is stopped  */
static void GC_mprotect_thread_notify(mach_msg_id_t id) {
    struct {
        GC_msg_t msg;
        mach_msg_trailer_t trailer;
    } buf;
    mach_msg_return_t r;
    /* remote, local */
    buf.msg.head.msgh_bits = 
        MACH_MSGH_BITS(MACH_MSG_TYPE_MAKE_SEND,0);
    buf.msg.head.msgh_size = sizeof(buf.msg);
    buf.msg.head.msgh_remote_port = GC_ports.exception;
    buf.msg.head.msgh_local_port = MACH_PORT_NULL;
    buf.msg.head.msgh_id = id;
            
    r = mach_msg(
        &buf.msg.head,
        MACH_SEND_MSG|MACH_RCV_MSG|MACH_RCV_LARGE,
        sizeof(buf.msg),
        sizeof(buf),
        GC_ports.reply,
        MACH_MSG_TIMEOUT_NONE,
        MACH_PORT_NULL);
    if(r != MACH_MSG_SUCCESS)
	ABORT("mach_msg failed in GC_mprotect_thread_notify");
    if(buf.msg.head.msgh_id != ID_ACK)
        ABORT("invalid ack in GC_mprotect_thread_notify");
}

/* Should only be called by the mprotect thread */
static void GC_mprotect_thread_reply() {
    GC_msg_t msg;
    mach_msg_return_t r;
    /* remote, local */
    msg.head.msgh_bits = 
        MACH_MSGH_BITS(MACH_MSG_TYPE_MAKE_SEND,0);
    msg.head.msgh_size = sizeof(msg);
    msg.head.msgh_remote_port = GC_ports.reply;
    msg.head.msgh_local_port = MACH_PORT_NULL;
    msg.head.msgh_id = ID_ACK;
            
    r = mach_msg(
        &msg.head,
        MACH_SEND_MSG,
        sizeof(msg),
        0,
        MACH_PORT_NULL,
        MACH_MSG_TIMEOUT_NONE,
        MACH_PORT_NULL);
    if(r != MACH_MSG_SUCCESS)
	ABORT("mach_msg failed in GC_mprotect_thread_reply");
}

void GC_mprotect_stop() {
    GC_mprotect_thread_notify(ID_STOP);
}
void GC_mprotect_resume() {
    GC_mprotect_thread_notify(ID_RESUME);
}

#else /* !THREADS */
/* The compiler should optimize away any GC_mprotect_state computations */
#define GC_mprotect_state GC_MP_NORMAL
#endif

static void *GC_mprotect_thread(void *arg) {
    mach_msg_return_t r;
    /* These two structures contain some private kernel data. We don't need to
       access any of it so we don't bother defining a proper struct. The
       correct definitions are in the xnu source code. */
    struct {
        mach_msg_header_t head;
        char data[256];
    } reply;
    struct {
        mach_msg_header_t head;
        mach_msg_body_t msgh_body;
        char data[1024];
    } msg;

    mach_msg_id_t id;
    
    for(;;) {
        r = mach_msg(
            &msg.head,
            MACH_RCV_MSG|MACH_RCV_LARGE|
                (GC_mprotect_state == GC_MP_DISCARDING ? MACH_RCV_TIMEOUT : 0),
            0,
            sizeof(msg),
            GC_ports.exception,
            GC_mprotect_state == GC_MP_DISCARDING ? 0 : MACH_MSG_TIMEOUT_NONE,
            MACH_PORT_NULL);
        
        id = r == MACH_MSG_SUCCESS ? msg.head.msgh_id : -1;
        
#if defined(THREADS)
        if(GC_mprotect_state == GC_MP_DISCARDING) {
            if(r == MACH_RCV_TIMED_OUT) {
                GC_mprotect_state = GC_MP_STOPPED;
                GC_mprotect_thread_reply();
                continue;
            }
            if(r == MACH_MSG_SUCCESS && (id == ID_STOP || id == ID_RESUME))
                ABORT("out of order mprotect thread request");
        }
#endif
        
        if(r != MACH_MSG_SUCCESS) {
            GC_err_printf2("mach_msg failed with %d %s\n", 
                (int)r,mach_error_string(r));
            ABORT("mach_msg failed");
        }
        
        switch(id) {
#if defined(THREADS)
            case ID_STOP:
                if(GC_mprotect_state != GC_MP_NORMAL)
                    ABORT("Called mprotect_stop when state wasn't normal");
                GC_mprotect_state = GC_MP_DISCARDING;
                break;
            case ID_RESUME:
                if(GC_mprotect_state != GC_MP_STOPPED)
                    ABORT("Called mprotect_resume when state wasn't stopped");
                GC_mprotect_state = GC_MP_NORMAL;
                GC_mprotect_thread_reply();
                break;
#endif /* THREADS */
            default:
	            /* Handle the message (calls catch_exception_raise) */
    	        if(!exc_server(&msg.head,&reply.head))
                    ABORT("exc_server failed");
                /* Send the reply */
                r = mach_msg(
                    &reply.head,
                    MACH_SEND_MSG,
                    reply.head.msgh_size,
                    0,
                    MACH_PORT_NULL,
                    MACH_MSG_TIMEOUT_NONE,
                    MACH_PORT_NULL);
	        if(r != MACH_MSG_SUCCESS) {
	        	/* This will fail if the thread dies, but the thread shouldn't
	        	   die... */
	        	#ifdef BROKEN_EXCEPTION_HANDLING
    	        	GC_err_printf2(
                        "mach_msg failed with %d %s while sending exc reply\n",
                        (int)r,mach_error_string(r));
    	        #else
    	        	ABORT("mach_msg failed while sending exception reply");
    	        #endif
        	}
        } /* switch */
    } /* for(;;) */
    /* NOT REACHED */
    return NULL;
}

/* All this SIGBUS code shouldn't be necessary. All protection faults should
   be going throught the mach exception handler. However, it seems a SIGBUS is
   occasionally sent for some unknown reason. Even more odd, it seems to be
   meaningless and safe to ignore. */
#ifdef BROKEN_EXCEPTION_HANDLING

typedef void (* SIG_PF)();
static SIG_PF GC_old_bus_handler;

/* Updates to this aren't atomic, but the SIGBUSs seem pretty rare.
   Even if this doesn't get updated property, it isn't really a problem */
static int GC_sigbus_count;

static void GC_darwin_sigbus(int num,siginfo_t *sip,void *context) {
    if(num != SIGBUS) ABORT("Got a non-sigbus signal in the sigbus handler");
    
    /* Ugh... some seem safe to ignore, but too many in a row probably means
       trouble. GC_sigbus_count is reset for each mach exception that is
       handled */
    if(GC_sigbus_count >= 8) {
        ABORT("Got more than 8 SIGBUSs in a row!");
    } else {
        GC_sigbus_count++;
        GC_err_printf0("GC: WARNING: Ignoring SIGBUS.\n");
    }
}
#endif /* BROKEN_EXCEPTION_HANDLING */

void GC_dirty_init() {
    kern_return_t r;
    mach_port_t me;
    pthread_t thread;
    pthread_attr_t attr;
    exception_mask_t mask;
    
#   ifdef PRINTSTATS
        GC_printf0("Inititalizing mach/darwin mprotect virtual dirty bit "
            "implementation\n");
#   endif  
#	ifdef BROKEN_EXCEPTION_HANDLING
        GC_err_printf0("GC: WARNING: Enabling workarounds for various darwin "
            "exception handling bugs.\n");
#	endif
    GC_dirty_maintained = TRUE;
    if (GC_page_size % HBLKSIZE != 0) {
        GC_err_printf0("Page size not multiple of HBLKSIZE\n");
        ABORT("Page size not multiple of HBLKSIZE");
    }
    
    GC_task_self = me = mach_task_self();
    
    r = mach_port_allocate(me,MACH_PORT_RIGHT_RECEIVE,&GC_ports.exception);
    if(r != KERN_SUCCESS) ABORT("mach_port_allocate failed (exception port)");
    
    r = mach_port_insert_right(me,GC_ports.exception,GC_ports.exception,
    	MACH_MSG_TYPE_MAKE_SEND);
    if(r != KERN_SUCCESS)
    	ABORT("mach_port_insert_right failed (exception port)");

    #if defined(THREADS)
        r = mach_port_allocate(me,MACH_PORT_RIGHT_RECEIVE,&GC_ports.reply);
        if(r != KERN_SUCCESS) ABORT("mach_port_allocate failed (reply port)");
    #endif

    /* The exceptions we want to catch */  
    mask = EXC_MASK_BAD_ACCESS;

    r = task_get_exception_ports(
        me,
        mask,
        GC_old_exc_ports.masks,
        &GC_old_exc_ports.count,
        GC_old_exc_ports.ports,
        GC_old_exc_ports.behaviors,
        GC_old_exc_ports.flavors
    );
    if(r != KERN_SUCCESS) ABORT("task_get_exception_ports failed");
        
    r = task_set_exception_ports(
        me,
        mask,
        GC_ports.exception,
        EXCEPTION_DEFAULT,
        MACHINE_THREAD_STATE
    );
    if(r != KERN_SUCCESS) ABORT("task_set_exception_ports failed");

    if(pthread_attr_init(&attr) != 0) ABORT("pthread_attr_init failed");
    if(pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED) != 0) 
        ABORT("pthread_attr_setdetachedstate failed");

#	undef pthread_create
    /* This will call the real pthread function, not our wrapper */
    if(pthread_create(&thread,&attr,GC_mprotect_thread,NULL) != 0)
        ABORT("pthread_create failed");
    pthread_attr_destroy(&attr);
    
    /* Setup the sigbus handler for ignoring the meaningless SIGBUSs */
    #ifdef BROKEN_EXCEPTION_HANDLING 
    {
        struct sigaction sa, oldsa;
        sa.sa_handler = (SIG_PF)GC_darwin_sigbus;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART|SA_SIGINFO;
        if(sigaction(SIGBUS,&sa,&oldsa) < 0) ABORT("sigaction");
        GC_old_bus_handler = (SIG_PF)oldsa.sa_handler;
        if (GC_old_bus_handler != SIG_DFL) {
#       	ifdef PRINTSTATS
                GC_err_printf0("Replaced other SIGBUS handler\n");
#       	endif
        }
    }
    #endif /* BROKEN_EXCEPTION_HANDLING  */
}
 
/* The source code for Apple's GDB was used as a reference for the exception
   forwarding code. This code is similar to be GDB code only because there is 
   only one way to do it. */
static kern_return_t GC_forward_exception(
        mach_port_t thread,
        mach_port_t task,
        exception_type_t exception,
        exception_data_t data,
        mach_msg_type_number_t data_count
) {
    int i;
    kern_return_t r;
    mach_port_t port;
    exception_behavior_t behavior;
    thread_state_flavor_t flavor;
    
    thread_state_data_t thread_state;
    mach_msg_type_number_t thread_state_count = THREAD_STATE_MAX;
        
    for(i=0;i<GC_old_exc_ports.count;i++)
        if(GC_old_exc_ports.masks[i] & (1 << exception))
            break;
    if(i==GC_old_exc_ports.count) ABORT("No handler for exception!");
    
    port = GC_old_exc_ports.ports[i];
    behavior = GC_old_exc_ports.behaviors[i];
    flavor = GC_old_exc_ports.flavors[i];

    if(behavior != EXCEPTION_DEFAULT) {
        r = thread_get_state(thread,flavor,thread_state,&thread_state_count);
        if(r != KERN_SUCCESS)
            ABORT("thread_get_state failed in forward_exception");
    }
    
    switch(behavior) {
        case EXCEPTION_DEFAULT:
            r = exception_raise(port,thread,task,exception,data,data_count);
            break;
        case EXCEPTION_STATE:
            r = exception_raise_state(port,thread,task,exception,data,
                data_count,&flavor,thread_state,thread_state_count,
                thread_state,&thread_state_count);
            break;
        case EXCEPTION_STATE_IDENTITY:
            r = exception_raise_state_identity(port,thread,task,exception,data,
                data_count,&flavor,thread_state,thread_state_count,
                thread_state,&thread_state_count);
            break;
        default:
            r = KERN_FAILURE; /* make gcc happy */
            ABORT("forward_exception: unknown behavior");
            break;
    }
    
    if(behavior != EXCEPTION_DEFAULT) {
        r = thread_set_state(thread,flavor,thread_state,thread_state_count);
        if(r != KERN_SUCCESS)
            ABORT("thread_set_state failed in forward_exception");
    }
    
    return r;
}

#define FWD() GC_forward_exception(thread,task,exception,code,code_count)

/* This violates the namespace rules but there isn't anything that can be done
   about it. The exception handling stuff is hard coded to call this */
kern_return_t
catch_exception_raise(
   mach_port_t exception_port,mach_port_t thread,mach_port_t task,
   exception_type_t exception,exception_data_t code,
   mach_msg_type_number_t code_count
) {
    kern_return_t r;
    char *addr;
    struct hblk *h;
    int i;
#ifdef POWERPC
    thread_state_flavor_t flavor = PPC_EXCEPTION_STATE;
    mach_msg_type_number_t exc_state_count = PPC_EXCEPTION_STATE_COUNT;
    ppc_exception_state_t exc_state;
#else
#	error FIXME for non-ppc darwin
#endif

    
    if(exception != EXC_BAD_ACCESS || code[0] != KERN_PROTECTION_FAILURE) {
        #ifdef DEBUG_EXCEPTION_HANDLING
        /* We aren't interested, pass it on to the old handler */
        GC_printf3("Exception: 0x%x Code: 0x%x 0x%x in catch....\n",
            exception,
            code_count > 0 ? code[0] : -1,
            code_count > 1 ? code[1] : -1); 
        #endif
        return FWD();
    }

    r = thread_get_state(thread,flavor,
        (natural_t*)&exc_state,&exc_state_count);
    if(r != KERN_SUCCESS) {
        /* The thread is supposed to be suspended while the exception handler
           is called. This shouldn't fail. */
        #ifdef BROKEN_EXCEPTION_HANDLING
            GC_err_printf0("thread_get_state failed in "
                "catch_exception_raise\n");
            return KERN_SUCCESS;
        #else
            ABORT("thread_get_state failed in catch_exception_raise");
        #endif
    }
    
    /* This is the address that caused the fault */
    addr = (char*) exc_state.dar;
        
    if((HDR(addr)) == 0) {
        /* Ugh... just like the SIGBUS problem above, it seems we get a bogus 
           KERN_PROTECTION_FAILURE every once and a while. We wait till we get
           a bunch in a row before doing anything about it. If a "real" fault 
           ever occurres it'll just keep faulting over and over and we'll hit
           the limit pretty quickly. */
        #ifdef BROKEN_EXCEPTION_HANDLING
            static char *last_fault;
            static int last_fault_count;
            
            if(addr != last_fault) {
                last_fault = addr;
                last_fault_count = 0;
            }
            if(++last_fault_count < 32) {
                if(last_fault_count == 1)
                    GC_err_printf1(
                        "GC: WARNING: Ignoring KERN_PROTECTION_FAILURE at %p\n",
                        addr);
                return KERN_SUCCESS;
            }
            
            GC_err_printf1("Unexpected KERN_PROTECTION_FAILURE at %p\n",addr);
            /* Can't pass it along to the signal handler because that is
               ignoring SIGBUS signals. We also shouldn't call ABORT here as
               signals don't always work too well from the exception handler. */
            GC_err_printf0("Aborting\n");
            exit(EXIT_FAILURE);
        #else /* BROKEN_EXCEPTION_HANDLING */
            /* Pass it along to the next exception handler 
               (which should call SIGBUS/SIGSEGV) */
            return FWD();
        #endif /* !BROKEN_EXCEPTION_HANDLING */
    }

    #ifdef BROKEN_EXCEPTION_HANDLING
        /* Reset the number of consecutive SIGBUSs */
        GC_sigbus_count = 0;
    #endif
    
    if(GC_mprotect_state == GC_MP_NORMAL) { /* common case */
        h = (struct hblk*)((word)addr & ~(GC_page_size-1));
        UNPROTECT(h, GC_page_size);	
        for (i = 0; i < divHBLKSZ(GC_page_size); i++) {
            register int index = PHT_HASH(h+i);
            async_set_pht_entry_from_index(GC_dirty_pages, index);
        }
    } else if(GC_mprotect_state == GC_MP_DISCARDING) {
        /* Lie to the thread for now. No sense UNPROTECT()ing the memory
           when we're just going to PROTECT() it again later. The thread
           will just fault again once it resumes */
    } else {
        /* Shouldn't happen, i don't think */
        GC_printf0("KERN_PROTECTION_FAILURE while world is stopped\n");
        return FWD();
    }
    return KERN_SUCCESS;
}
#undef FWD

/* These should never be called, but just in case...  */
kern_return_t catch_exception_raise_state(mach_port_name_t exception_port,
    int exception, exception_data_t code, mach_msg_type_number_t codeCnt,
    int flavor, thread_state_t old_state, int old_stateCnt,
    thread_state_t new_state, int new_stateCnt)
{
    ABORT("catch_exception_raise_state");
    return(KERN_INVALID_ARGUMENT);
}
kern_return_t catch_exception_raise_state_identity(
    mach_port_name_t exception_port, mach_port_t thread, mach_port_t task,
    int exception, exception_data_t code, mach_msg_type_number_t codeCnt,
    int flavor, thread_state_t old_state, int old_stateCnt, 
    thread_state_t new_state, int new_stateCnt)
{
    ABORT("catch_exception_raise_state_identity");
    return(KERN_INVALID_ARGUMENT);
}


#endif /* DARWIN && MPROTECT_VDB */

# ifndef HAVE_INCREMENTAL_PROTECTION_NEEDS
  int GC_incremental_protection_needs()
  {
    return GC_PROTECTS_NONE;
  }
# endif /* !HAVE_INCREMENTAL_PROTECTION_NEEDS */

/*
 * Call stack save code for debugging.
 * Should probably be in mach_dep.c, but that requires reorganization.
 */

/* I suspect the following works for most X86 *nix variants, so 	*/
/* long as the frame pointer is explicitly stored.  In the case of gcc,	*/
/* compiler flags (e.g. -fomit-frame-pointer) determine whether it is.	*/
#if defined(I386) && defined(LINUX) && defined(SAVE_CALL_CHAIN)
#   include <features.h>

    struct frame {
	struct frame *fr_savfp;
	long	fr_savpc;
        long	fr_arg[NARGS];  /* All the arguments go here.	*/
    };
#endif

#if defined(SPARC)
#  if defined(LINUX)
#    include <features.h>

     struct frame {
	long	fr_local[8];
	long	fr_arg[6];
	struct frame *fr_savfp;
	long	fr_savpc;
#       ifndef __arch64__
	  char	*fr_stret;
#       endif
	long	fr_argd[6];
	long	fr_argx[0];
     };
#  else
#    if defined(SUNOS4)
#      include <machine/frame.h>
#    else
#      if defined (DRSNX)
#	 include <sys/sparc/frame.h>
#      else
#	 if defined(OPENBSD) || defined(NETBSD)
#	   include <frame.h>
#	 else
#	   include <sys/frame.h>
#	 endif
#      endif
#    endif
#  endif
#  if NARGS > 6
	--> We only know how to to get the first 6 arguments
#  endif
#endif /* SPARC */

#ifdef  NEED_CALLINFO
/* Fill in the pc and argument information for up to NFRAMES of my	*/
/* callers.  Ignore my frame and my callers frame.			*/

#ifdef LINUX
#   include <unistd.h>
#endif

#endif /* NEED_CALLINFO */

#ifdef SAVE_CALL_CHAIN

#if NARGS == 0 && NFRAMES % 2 == 0 /* No padding */ \
    && defined(GC_HAVE_BUILTIN_BACKTRACE)

#include <execinfo.h>

void GC_save_callers (info) 
struct callinfo info[NFRAMES];
{
  void * tmp_info[NFRAMES + 1];
  int npcs, i;
# define IGNORE_FRAMES 1
  
  /* We retrieve NFRAMES+1 pc values, but discard the first, since it	*/
  /* points to our own frame.						*/
  GC_ASSERT(sizeof(struct callinfo) == sizeof(void *));
  npcs = backtrace((void **)tmp_info, NFRAMES + IGNORE_FRAMES);
  BCOPY(tmp_info+IGNORE_FRAMES, info, (npcs - IGNORE_FRAMES) * sizeof(void *));
  for (i = npcs - IGNORE_FRAMES; i < NFRAMES; ++i) info[i].ci_pc = 0;
}

#else /* No builtin backtrace; do it ourselves */

#if (defined(OPENBSD) || defined(NETBSD)) && defined(SPARC)
#  define FR_SAVFP fr_fp
#  define FR_SAVPC fr_pc
#else
#  define FR_SAVFP fr_savfp
#  define FR_SAVPC fr_savpc
#endif

#if defined(SPARC) && (defined(__arch64__) || defined(__sparcv9))
#   define BIAS 2047
#else
#   define BIAS 0
#endif

void GC_save_callers (info) 
struct callinfo info[NFRAMES];
{
  struct frame *frame;
  struct frame *fp;
  int nframes = 0;
# ifdef I386
    /* We assume this is turned on only with gcc as the compiler. */
    asm("movl %%ebp,%0" : "=r"(frame));
    fp = frame;
# else
    frame = (struct frame *) GC_save_regs_in_stack ();
    fp = (struct frame *)((long) frame -> FR_SAVFP + BIAS);
#endif
  
   for (; (!(fp HOTTER_THAN frame) && !(GC_stackbottom HOTTER_THAN (ptr_t)fp)
	   && (nframes < NFRAMES));
       fp = (struct frame *)((long) fp -> FR_SAVFP + BIAS), nframes++) {
      register int i;
      
      info[nframes].ci_pc = fp->FR_SAVPC;
#     if NARGS > 0
        for (i = 0; i < NARGS; i++) {
	  info[nframes].ci_arg[i] = ~(fp->fr_arg[i]);
        }
#     endif /* NARGS > 0 */
  }
  if (nframes < NFRAMES) info[nframes].ci_pc = 0;
}

#endif /* No builtin backtrace */

#endif /* SAVE_CALL_CHAIN */

#ifdef NEED_CALLINFO

/* Print info to stderr.  We do NOT hold the allocation lock */
void GC_print_callers (info)
struct callinfo info[NFRAMES];
{
    register int i;
    static int reentry_count = 0;
    GC_bool stop = FALSE;

    LOCK();
      ++reentry_count;
    UNLOCK();
    
#   if NFRAMES == 1
      GC_err_printf0("\tCaller at allocation:\n");
#   else
      GC_err_printf0("\tCall chain at allocation:\n");
#   endif
    for (i = 0; i < NFRAMES && !stop ; i++) {
     	if (info[i].ci_pc == 0) break;
#	if NARGS > 0
	{
	  int j;

     	  GC_err_printf0("\t\targs: ");
     	  for (j = 0; j < NARGS; j++) {
     	    if (j != 0) GC_err_printf0(", ");
     	    GC_err_printf2("%d (0x%X)", ~(info[i].ci_arg[j]),
     	    				~(info[i].ci_arg[j]));
     	  }
	  GC_err_printf0("\n");
	}
# 	endif
        if (reentry_count > 1) {
	    /* We were called during an allocation during	*/
	    /* a previous GC_print_callers call; punt.		*/
     	    GC_err_printf1("\t\t##PC##= 0x%lx\n", info[i].ci_pc);
	    continue;
	}
	{
#	  ifdef LINUX
	    FILE *pipe;
#	  endif
#	  if defined(GC_HAVE_BUILTIN_BACKTRACE)
	    char **sym_name =
	      backtrace_symbols((void **)(&(info[i].ci_pc)), 1);
	    char *name = sym_name[0];
#	  else
	    char buf[40];
	    char *name = buf;
     	    sprintf(buf, "##PC##= 0x%lx", info[i].ci_pc);
#	  endif
#	  if defined(LINUX) && !defined(SMALL_CONFIG)
	    /* Try for a line number. */
	    {
#	        define EXE_SZ 100
		static char exe_name[EXE_SZ];
#		define CMD_SZ 200
		char cmd_buf[CMD_SZ];
#		define RESULT_SZ 200
		static char result_buf[RESULT_SZ];
		size_t result_len;
		static GC_bool found_exe_name = FALSE;
		static GC_bool will_fail = FALSE;
		int ret_code;
		/* Try to get it via a hairy and expensive scheme.	*/
		/* First we get the name of the executable:		*/
		if (will_fail) goto out;
		if (!found_exe_name) { 
		  ret_code = readlink("/proc/self/exe", exe_name, EXE_SZ);
		  if (ret_code < 0 || ret_code >= EXE_SZ
		      || exe_name[0] != '/') {
		    will_fail = TRUE;	/* Dont try again. */
		    goto out;
		  }
		  exe_name[ret_code] = '\0';
		  found_exe_name = TRUE;
		}
		/* Then we use popen to start addr2line -e <exe> <addr>	*/
		/* There are faster ways to do this, but hopefully this	*/
		/* isn't time critical.					*/
		sprintf(cmd_buf, "/usr/bin/addr2line -f -e %s 0x%lx", exe_name,
				 (unsigned long)info[i].ci_pc);
		pipe = popen(cmd_buf, "r");
		if (pipe == NULL
		    || (result_len = fread(result_buf, 1, RESULT_SZ - 1, pipe))
		       == 0) {
		  if (pipe != NULL) pclose(pipe);
		  will_fail = TRUE;
		  goto out;
		}
		if (result_buf[result_len - 1] == '\n') --result_len;
		result_buf[result_len] = 0;
		if (result_buf[0] == '?'
		    || result_buf[result_len-2] == ':' 
		       && result_buf[result_len-1] == '0') {
		    pclose(pipe);
		    goto out;
		}
		/* Get rid of embedded newline, if any.  Test for "main" */
		{
		   char * nl = strchr(result_buf, '\n');
		   if (nl != NULL && nl < result_buf + result_len) {
		     *nl = ':';
		   }
		   if (strncmp(result_buf, "main", nl - result_buf) == 0) {
		     stop = TRUE;
		   }
		}
		if (result_len < RESULT_SZ - 25) {
		  /* Add in hex address	*/
		    sprintf(result_buf + result_len, " [0x%lx]",
			  (unsigned long)info[i].ci_pc);
		}
		name = result_buf;
		pclose(pipe);
		out:;
	    }
#	  endif /* LINUX */
	  GC_err_printf1("\t\t%s\n", name);
#	  if defined(GC_HAVE_BUILTIN_BACKTRACE)
	    free(sym_name);  /* May call GC_free; that's OK */
#         endif
	}
    }
    LOCK();
      --reentry_count;
    UNLOCK();
}

#endif /* NEED_CALLINFO */



#if defined(LINUX) && defined(__ELF__) && !defined(SMALL_CONFIG)

/* Dump /proc/self/maps to GC_stderr, to enable looking up names for
   addresses in FIND_LEAK output. */

static word dump_maps(char *maps)
{
    GC_err_write(maps, strlen(maps));
    return 1;
}

void GC_print_address_map()
{
    GC_err_printf0("---------- Begin address map ----------\n");
    GC_apply_to_maps(dump_maps);
    GC_err_printf0("---------- End address map ----------\n");
}

#endif


