#ifndef PREFS_H
#define PREFS_H

/*

(c) 2001, 2002 Peter Putzer <pputzer@users.sourceforge.net>
(c) 1999, 2000 Mitch Blevins <mblevin@debian.org>.

Licensed under the GNU General Public License (GPL).

*/

#include <PalmOS.h>
#include "sections.h"

extern Boolean PrefExtEvent(EventPtr e) SUTIL2;
extern Boolean PrefEvent(EventPtr e) SUTIL2;
extern Boolean PrefHWEvent(EventPtr e) SUTIL2;
extern Boolean PrefAlarmEvent(EventPtr e) SUTIL1;
extern void DoAlarmPreferences(void) SUTIL2;

#endif
