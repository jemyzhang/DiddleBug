#ifndef __XFER_H_
#define __XFER_H_

/*
** diddlebug - a yellow-sticky-note-thingy
** Copyright (c) 2001,2002 Peter Putzer <pputzer@users.sourceforge.net>
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
#include "dynamic_buttons.h"

/* Transfer flags */
#define XFER_GOTONEVER      0x01
#define XFER_GOTOALWAYS     0x02
#define XFER_COMPLETEALWAYS 0x04
#define XFER_COMPLETENEVER  0x08

#define xferGotoClear() d.xfer.flags &= ~(XFER_GOTONEVER|XFER_GOTOALWAYS)
#define xferGotoIsNever   d.xfer.flags&XFER_GOTONEVER
#define xferGotoIsAlways  d.xfer.flags&XFER_GOTOALWAYS
#define xferGotoIsClear   !(d.xfer.flags&(XFER_GOTONEVER|XFER_GOTOALWAYS))
#define xferGotoSetNever() d.xfer.flags = \
            (d.xfer.flags|XFER_GOTONEVER)& ~XFER_GOTOALWAYS
#define xferGotoSetAlways() d.xfer.flags = \
            (d.xfer.flags|XFER_GOTOALWAYS)& ~XFER_GOTONEVER
#define xferCompleteClear() d.xfer.flags &= ~(XFER_COMPLETENEVER|XFER_COMPLETEALWAYS)
#define xferCompleteIsNever d.xfer.flags&XFER_COMPLETENEVER
#define xferCompleteIsAlways d.xfer.flags&XFER_COMPLETEALWAYS
#define xferCompleteIsClear !(d.xfer.flags&(XFER_COMPLETENEVER|XFER_COMPLETEALWAYS))
#define xferCompleteSetNever() d.xfer.flags = \
            (d.xfer.flags|XFER_COMPLETENEVER)& ~XFER_COMPLETEALWAYS
#define xferCompleteSetAlways() d.xfer.flags = \
            (d.xfer.flags|XFER_COMPLETEALWAYS)& ~XFER_COMPLETENEVER

extern void StartXferMode(void) SUTIL2;
extern void CancelXferMode(void) SUTIL2;
extern void FinishXferMode(void) SUTIL1;
extern void DrawXferDoneButton(DynamicButtonType* btn) SUTIL2;
extern void DrawPlugButton(DynamicButtonType* btn) SUTIL2;
extern Boolean XferPenDown(EventPtr e) SUTIL2;
extern Boolean XferDoneGadgetEvent(FormGadgetTypeInCallback* gadgetP, UInt16 cmd, void* paramP) SUTIL2;
extern Boolean XferDetailsGadgetEvent(FormGadgetTypeInCallback* gadgetP, UInt16 cmd, void* paramP) SUTIL2;
extern void ToggleButtonBar(FormPtr frm, Boolean toggle_on) SUTIL2;

#endif /* __XFER_H_ */
