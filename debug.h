#ifndef DIDDLEBUG_DEBUG_H
#define DIDDLEBUG_DEBUG_H

/*
** diddlebug - a yellow-sticky-note-thingy
** Copyright (c) 2001,2002 Peter Putzer <pputzer@users.sourceforge.net>
** Copyright (c) 1999,2000 Mitch Blevins <mblevin@debian.org>.
** Licensed under the GNU GPL, version 2 or later
** See file "COPYING" that you should have received with this program
** or visit http://www.gnu.org/copyleft/gpl.html
**
** DiddleBug is based on Diddle, which is based on Doodle
** Doodle is Copyright 1997 Roger E Critchlow Jr., San Francisco, California.
** All rights reserved.  Fair use permitted.  Caveat emptor.  No warranty.
*/

#include <hostcontrol.h>

/* Error messages */
#define abort()	ErrDisplayFileLineMsg(__FILE__, __LINE__, "")

#ifdef DIDDLEDEBUG

#define ASSERT(a) if(!(a)) ErrDisplayFileLineMsg(__FILE__, __LINE__, "Assert")
#define ASSERTNOT(a) if(a) ErrDisplayFileLineMsg(__FILE__, __LINE__, "Assert")

#define PRINT(...) HostTraceOutputTL(appErrorClass, __VA_ARGS__)

#else  /* DIDDLEDEBUG */

#define ASSERT(a)
#define ASSERTNOT(a)

#define PRINT(...)

#endif /* DIDDLEDEBUG */



#endif /* DIDDLEBUG_DEBUG_H */
