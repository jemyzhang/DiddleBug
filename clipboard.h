#ifndef CLIPBOARD_H
#define CLIPBOARD_H

/*
** diddlebug - a yellow-sticky-note-thingy
** Copyright (c) 2004 Peter Putzer <pputzer@users.sourceforge.net>
**
** Licensed under the GNU GPL, version 2 or later
** See file "COPYING" that you should have received with this program
** or visit http://www.gnu.org/copyleft/gpl.html
*/

#include "sections.h"

extern void ClearOldPaste(void) SUTIL3;
extern void DoPaste(Coord x, Coord y) SUTIL3;
extern void EndPaste(Coord x, Coord y) SUTIL3;
extern void PasteRectangle(Coord x, Coord y, RectangleType* r) SUTIL3;
extern void ClearOldCutCopy(void) SUTIL3;
extern void DoCutCopy(Coord x, Coord y) SUTIL3;
extern void EndCopy(Coord x, Coord y) SUTIL3;
extern void CutCopyRectangle(Coord x, Coord y, RectangleType* r) SUTIL3;
extern void EndCut(Coord x, Coord y) SUTIL3;

#endif
