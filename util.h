#ifndef __UTIL_H_
#define __UTIL_H_

/*
** diddlebug - a yellow-sticky-note-thingy
** Copyright (c) 2001, 2002, 2004 Peter Putzer <pputzer@users.sourceforge.net>.
** Copyright (c) 1999, 2000 Mitch Blevins <mblevin@debian.org>.
** Licensed under the GNU GPL, version 2 or later
** See file "COPYING" that you should have received with this program
** or visit http://www.gnu.org/copyleft/gpl.html
**
** DiddleBug is based on Diddle, which is based on Doodle
** Doodle is Copyright 1997 Roger E Critchlow Jr., San Francisco, California.
** All rights reserved.  Fair use permitted.  Caveat emptor.  No warranty.
*/

#include "sections.h"
#include <PalmOS.h>

/*
** Structure for flash message
*/
typedef struct {
  WinHandle offscreen_handle;         /* Draw window before flash */
  WinHandle flash_handle;             /* The flash window handle */
  Coord x;                            /* X value in display coords */
  Coord y;                            /* Y value in display coords */
} FlashMessageType;

extern FlashMessageType* FlashMessage(UInt16 strID) SUTIL3;
extern void UnFlashMessage(FlashMessageType* flashP) SUTIL3;
extern void FlashWaitMessage(UInt16 strID) SUTIL3;

#endif // __UTIL_H_
