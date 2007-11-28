#ifndef TEXT_H
#define TEXT_H

/*
** diddlebug - a yellow-sticky-note-thingy
** Copyright (c) 2001,2002,2003,2004 Peter Putzer <pputzer@users.sourceforge.net>
** Copyright (c) 1999,2000 Mitch Blevins <mblevin@debian.org>.
** Licensed under the GNU GPL, version 2 or later
** See file "COPYING" that you should have received with this program
** or visit http://www.gnu.org/copyleft/gpl.html
**
** DiddleBug is based on Diddle, which is based on Doodle
** Doodle is Copyright 1997 Roger E Critchlow Jr., San Francisco, California.
** All rights reserved.  Fair use permitted.  Caveat emptor.  No warranty.
*/

#include "sections.h"

extern void ClearOldText(void) SUTIL3;
extern void DoTextInsert(Coord x, Coord y) SUTIL3;
extern void EndTextInsert(Coord x, Coord y) SUTIL3;
extern void TextRectangle(Coord x, Coord y, RectangleType* r) SUTIL3;

#endif
