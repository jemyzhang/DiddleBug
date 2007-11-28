#ifndef DIDDLEBUG_GLOBALS_H
#define DIDDLEBUG_GLOBALS_H

/*
** diddlebug - a yellow-sticky-note-thingy
** Copyright (c) 2001,2002,2003 Peter Putzer <pputzer@users.sourceforge.net>
** Copyright (c) 1999,2000 Mitch Blevins <mblevin@debian.org>.
** Licensed under the GNU GPL, version 2 or later
** See file "COPYING" that you should have received with this program
** or visit http://www.gnu.org/copyleft/gpl.html
**
** DiddleBug is based on Diddle, which is based on Doodle
** Doodle is Copyright 1997 Roger E Critchlow Jr., San Francisco, California.
** All rights reserved.  Fair use permitted.  Caveat emptor.  No warranty.
*/

/* Global variables... */

extern pref p;
extern extended_pref ep;
extern data d;
extern const CustomPatternType* maskPattern;

/* ... and some global constants */
extern const RectangleType bottom_bar_rect;  /* Rectangle covering the bottom bar */
extern const RectangleType title_bar_rect;   /* Rectangle covering the title bar */
extern const RectangleType screen_above_bar_rect; /* Rectangle above the bottom bar */
extern const RGBColorType black; /* The color black */

#endif // DIDDLEBUG_GLOBALS_H
