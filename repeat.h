#ifndef REPEAT_H
#define REPEAT_H

/*

(c) 2001, 2002 Peter Putzer
Licensed under the GNU General Public License (GPL).

Portions copyright (c) 2000 Palm, Inc. or its subsidiaries.  All rights reserved.

*/

#include <PalmOS.h>
#include "sections.h"
#include "booger.h"
#include "diddlebug.h"

/*
** Functions for the Repeat dialog box
*/
extern void RepeatDrawDescription (FormPtr frm) SUTIL2;
extern void RepeatInit (FormPtr frm) SUTIL2;
extern Boolean RepeatHandleEvent (EventType* event) SUTIL2;
extern Boolean NextRepeat (DiddleBugRecordPtr rec, DateTimePtr dateP);

#endif // REPEAT_H
