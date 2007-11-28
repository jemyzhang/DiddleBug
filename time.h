#ifndef DIDDLEBUG_TIME_H
#define DIDDLEBUG_TIME_H

/*

(c) 2001, 2002 Peter Putzer <pputzer@users.sourceforge.net>
(c) 1999, 2000 Mitch Blevins <mblevin@debian.org>.

Licensed under the GNU General Public License (GPL).

*/

#include "sections.h"

extern Boolean TimeEvent(EventPtr e) SUTIL2;
extern UInt32 DoAbsoluteSnoozeDialog(void);

#endif /* DIDDLEBUG_TIME_H */
