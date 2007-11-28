#ifndef NOTES_H
#define NOTES_H

/*

Copyright (c) 2001, 2002, 2004 Peter Putzer <pputzer@users.sourceforge.net>.
Licensed under the GNU General Public License (GPL).

Portions copyright (c) 2000 Palm, Inc. or its subsidiaries.  All rights reserved.

*/

#include "sections.h"
#include "diddlebug.h"

#define noteTitleFont                                   boldFont
#define maxNoteTitleLen                                 40

extern Boolean NoteViewHandleEvent (EventType* event) SUTIL1;
extern void NoteViewInit (FormPtr frm) SUTIL1;
extern void NoteViewDrawTitleAndForm (FormPtr frm) SUTIL1;
extern void NoteViewUpdateScrollBar (void) SUTIL1;

#endif
