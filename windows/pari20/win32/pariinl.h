/*   This file contains declarations/definitions of functions that
 *   _can_ be inlined.
 */
/* $Id$ */

// #ifndef WINCE
#if defined(_M_IX86)
#  include "../pari-2.0.17.beta/src/kernel/ix86/level0.h"
#else
#  include "../pari-2.0.17.beta/src/kernel/none/level0.h"
#endif

#include "../pari-2.0.17.beta/src/kernel/none/level1.h"

