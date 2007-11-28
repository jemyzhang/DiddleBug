#ifndef SHAPE_H
#define SHAPE_H

/*
** diddlebug - a yellow-sticky-note-thingy
** Copyright (c) 2001,2002,2004 Peter Putzer <pputzer@users.sourceforge.net>
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

extern void ClearOldShape(void) SUTIL3;
extern void DoShape(Coord x, Coord y) SUTIL3;
extern void EndShape(Coord x, Coord y) SUTIL3;
extern UInt16 LoadShapeRect(Coord x, Coord y, RectangleType* r) SUTIL3;
extern void ClearOldLine(void) SUTIL3;
extern void DoLine(Coord x, Coord y) SUTIL3;
extern void EndLine(Coord x, Coord y) SUTIL3;
extern void LineRectangle(Coord x, Coord y, RectangleType* r) SUTIL3;
extern void FloodFill(Coord x, Coord y, const CustomPatternType* pattern) SUTIL3;

#endif
