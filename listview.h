#ifndef LISTVIEW_H
#define LISTVIEW_H

/*

(c) 2001, 2002, 2004 Peter Putzer
Licensed under the GNU General Public License (GPL).

Portions copyright (c) 2000 Palm, Inc. or its subsidiaries.  All rights reserved.

*/

#include <PalmOS.h>
#include "sections.h"

extern Boolean ListViewHandleEvent(EventPtr event) SUTIL2;
extern void ListViewNextCategory(void) SUTIL1;
extern void ListViewDisplayMask(RectanglePtr bounds) SUTIL1;
extern Boolean SeekRecord(UInt16* indexP, UInt16 offset, Int16 direction) SUTIL1;

#endif
