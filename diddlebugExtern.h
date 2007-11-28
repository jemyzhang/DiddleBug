#ifndef DIDDLEBUG_EXTERN_H
#define DIDDLEBUG_EXTERN_H

/*
** diddlebug - a yellow-sticky-note-thingy
** Copyright (c) 2001, 2002, 2003, 2004 Peter Putzer <pputzer@users.sourceforge.net>
** Copyright (c) 1999, 2000 Mitch Blevins <mblevin@debian.org>.
** Licensed under the GNU GPL, version 2 or later
** See file "COPYING" that you should have received with this program
** or visit http://www.gnu.org/copyleft/gpl.html
**
** DiddleBug is based on Diddle, which is based on Doodle
** Doodle is Copyright 1997 Roger E Critchlow Jr., San Francisco, California.
** All rights reserved.  Fair use permitted.  Caveat emptor.  No warranty.
*/

/* extern function prototypes */

#include "sections.h"

/*
** Switches back to doodle mode and does the necessary cleanup
*/
extern void SwitchToDoodleMode(void) SUTIL1;

/*
** Display 'About' dialog
*/
extern void DoAboutDialog(void) SUTIL1;

/*
** Check whether DiddleBug is running on a Sony Clie
*/
extern Boolean CheckForSony(Boolean*);

extern void DoCmdRemove(void) SUTIL1;
extern void FlushToBuffer(void) SUTIL1;
extern void LabelAlarm(void) SUTIL1;
extern void AllocImage(void) SUTIL2;
extern Boolean KeyDown(UInt16 chr) SUTIL1;
extern void SwitchCategoryForGoto(void) SUTIL2;
extern Boolean UncompressSketch(UInt8* cPtr, UInt8* uPtr, UInt32 size, Boolean hires);
extern Boolean GetPreferences(pref* prefs, UInt16* size, Int16* oldVersion, Boolean hires);
extern void SendGotoEvent(UInt16 recordNum, UInt16 field, UInt16 pos, UInt16 len);
extern Int16 LabelMIDITrigger(Int16 list_index, Char* string, UInt16 string_len) SUTIL2;
extern void TruncatePopupTriggerText(FormType* frm, UInt16 id, Char* str) SUTIL2;
extern void InitMIDIList(RectangleType* bounds) SUTIL2;
extern void FlushToDisk(Boolean flushSketchData) SUTIL1;
extern void SetupSignalDialog(FormType* frm, Boolean led, Boolean vibrate, Int32 signal) SUTIL3;
extern void SetupSignalList(FormType* frm, UInt32 signalList, UInt32 signalPop, Int32 signal, Char* label) SUTIL2;
extern void CopyAppBitmapToWindow(UInt32 creator, DmResType resType, DmResID resID, 
				  WinHandle win, Coord x, Coord y) SUTIL1;
extern void MakeDateTimeStamp(UInt32 secs, Char* stamp) SUTIL1;
extern void SwitchCategory(UInt16 old_cat, FormType* frm) SUTIL1;
extern void UpdatePageButton() SUTIL1;
extern void LoadFromDisk(void) SUTIL1;
extern void UpdateNoteButton(void) SUTIL1;

/*
** Retrieve extended preferences (and initialize if no or wrong preferences were found)
**
** Returns: error
*/
extern Boolean GetExtendedPreferences(extended_pref* prefs, UInt16* size, Int16* oldVersion);

/*
** Handles events for gadgets acting as dynamic FormBitmaps.
*/
extern Boolean BitmapGadgetEvent(FormGadgetTypeInCallback* gadgetP, UInt16 cmd, void* paramP) SUTIL2;

/*
** Sort database by sketch title.
*/
Int16 SortByName(void *rec1, void *rec2, Int16 other,
		 SortRecordInfoPtr rec1SortInfo, SortRecordInfoPtr rec2SortInfo,
		 MemHandle appInfoH) SUTIL3;

/*
** Sort database by alarm time.
*/
extern Int16 SortByAlarmTime(void *rec1, void *rec2, Int16 other,
			     SortRecordInfoPtr rec1SortInfo, SortRecordInfoPtr rec2SortInfo,
			     MemHandle appInfoH) SUTIL3;

/*
** Open note dialog for current record
*/
extern void DoNoteDialog(UInt16 select_start, UInt16 select_length) SUTIL2;

/*
** CompressSketch - assumes uPtr is 160x160 bits or 320x320 bits
**                  (depending on the "hires" parameter)
**
**                  returns a locked pointer to the current record
*/
extern MemPtr CompressSketch(Boolean hires, DmOpenRef dbR, UInt16 dbI,
			     MemHandle rec_name, MemHandle rec_note,
			     AlarmSoundInfoType* rec_sound, UInt16* extraLength,
			     UInt16* size, UInt8* uPtr);

/*
** Mirror the real canvas onto the screen
*/
extern void MirrorRealCanvas(void) SUTIL1;

/*
** Set up hi-res mode for Sony Clies
**
** Also stores the refNum of the HR library.
*/
extern void SetUpSony(UInt16* refNum, Boolean* loadedHRLib);

/*
** Checks for (at least) a certain PalmOS version.
*/
extern Boolean CheckROMVersion(UInt32 requiredVersion);

/*
** Is this a hi-res double-density device?
*/
extern Boolean CheckForDoubleDensity(void);

/*
** Switch back to standard mode 
*/
extern UInt16 SwitchToLoRes(void);

/*
** PushKeyDownEvent - Push a keyDown event for a specific character onto the queue.
*/
extern void PushKeyDownEvent(WChar c) SUTIL2;

/*
** PushNonKeyCommandEvent - Push a special event for commands that do not have a specific 
**                          shortcut key defined.
*/
extern void PushNonKeyCommandEvent(NonKeyCommandCodeType code) SUTIL2;

/*
** HandleNonKeyCommandCode - handles commands that don't have a shortcut key.
*/
extern Boolean HandleNonKeyCommandCode(NonKeyCommandCodeType c) SUTIL3;

/*
** Check whether we're running on a Treo 600 smartphone.
*/
extern Boolean CheckForTreo600(void);

/*
** Draw masked record.
*/
extern void DrawMaskedRecord(WinHandle winH, const CustomPatternType* pattern);

/*
** Mirror the given rectangle of the real canvas onto the screen
** using the user-specified ink and paper color.
*/
extern void MirrorPartialRealCanvas(const RectangleType* r, Boolean clip) SUTIL3;

/*
** Sets the clip rectangle of given window to the visible
** drawing area. The clip rectangle has to be reset with
** ResetDrawingAreaClip().
*/
extern void SetDrawingAreaClip(WinHandle h) SUTIL3;

/*
** Resets the clip rectangle of the given window.
*/
extern void ResetDrawingAreaClip(WinHandle h) SUTIL3;

/*
** Free a window and its associated bitmap.
*/
extern void FreeWindowAndBitmap(WinHandle h, BitmapType* bmp) SUTIL3;

/*
** Signal a database error to the user and release the 
** corresponding record if necessary.
*/
extern void DatabaseError(DmOpenRef dbR, UInt16 dbI);

/*
** Set the drawing area rectangle correctly.
*/
void ResetDrawingAreaRectangle(Boolean withTitle, Boolean withToolbar) SUTIL3;

#endif // DIDDLEBUG_EXTERN_H
