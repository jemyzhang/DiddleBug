/*

Copyright (c) 2001, 2002, 2004 Peter Putzer <pputzer@users.sourceforge.net>.
Licensed under the GNU General Public License (GPL).

Portions copyright (c) 2000 Palm, Inc. or its subsidiaries.  All rights reserved.

*/

#include "thumbnails_details.h"
#include "listview.h"
#include "diddlebug.h"
#include "diddlebugExtern.h"
#include "diddlebugInlines.h"
#include "diddlebugRsc.h"
#include "globals.h"
#include "dynamic_buttons.h"
#include "alarm.h"
#include "debug.h"

#include <PalmOSGlue.h>

/* Palm 5-way navigator */
#include "PalmChars.h" 

/* Treo 600 5-way navigation */
#include <HsExt.h>
#include <HsNav.h> 

/* Number of records per page */
#define recordsPerPage 4

/* Number of records in a row */
#define recordsPerRow  1

/* Number of rows */
#define numberOfRows   recordsPerPage / recordsPerRow

/* local functions */
static void ThumbnailDetailViewLoadGadgets(FormType* frm) SUTIL3;
static UInt16 ThumbnailDetailViewSelectCategory(void) SUTIL3;
static void ThumbnailDetailViewPageScroll(WinDirectionType direction) SUTIL3;
static void ThumbnailDetailViewScroll(Int16 linesToScroll) SUTIL3;
static void ThumbnailDetailViewInit(FormPtr frm) SUTIL3;
static void ThumbnailDoCmdSecurity(void) SUTIL3;
static Boolean ThumbnailDetailGadgetEvent(FormGadgetTypeInCallback* gadgetP, UInt16 cmd, void* paramP) SUTIL3;
static void ThumbnailDetailViewUpdateScrollers(FormPtr frm) SUTIL3;
static Boolean ThumbnailLineEvent(FormGadgetTypeInCallback* gadgetP, UInt16 cmd, void* paramP) SUTIL3;
static Boolean ThumbnailDetailViewUpdateDisplay(UInt16 updateCode) SUTIL3;
static void GotoRecordField(UInt16 gadgetID, UInt16 field) SUTIL3;
static void LoadRecordData(void) SUTIL3;
static void ToggleButton(UInt16 offset, Boolean select) SUTIL3;
static void ThumbnailDetailMoveCursorUp(UInt16 old_dbI) SUTIL3;
static void ThumbnailDetailMoveCursorDown(UInt16 old_dbI) SUTIL3;
static Boolean ThumbnailDetailUnlockMaskedRecords(void) SUTIL3;
static Boolean ThumbnailDetailHandleFiveWayNavLeft(void) SUTIL3;
static Boolean ThumbnailDetailHandleFiveWayNavRight(void) SUTIL3;
static Boolean ThumbnailDetailHandleFiveWayNavSelect(void) SUTIL3;

/*
** Go to the record specified gadgetID and open the specified
** "field" (the note dialog, the details dialog, ...)
*/
static void GotoRecordField(UInt16 gadgetID, UInt16 field) {
  FormType* frm = FrmGetActiveForm();
  DynamicButtonType* btn = (DynamicButtonType*) FrmGetGadgetData(frm, FrmGetObjectIndex(frm, gadgetID));

  p.dbI = btn->value;
  LoadRecordData();

  switch (field) {
  case ffNote:
    DoNoteDialog(0, 0);
    break;

  case ffDetails:
    d.detailsWithSketch = false;
    FrmPopupForm(RecordDetailsForm);
    break;

  case ffAlarm:
    recordUnsetCountdown();
    FrmGotoForm(TimeForm);
    break;

  default:
    /* ignore */
  }
}

/***********************************************************************
 *
 * FUNCTION:    ThumbnailDetailViewUpdateDisplay
 *
 * DESCRIPTION: This routine update the display of the thumbnail details view
 *
 * PARAMETERS:  updateCode - a code that indicated what changes have been
 *                           made to the view.
 *
 * RETURNED:    Whether the event was handled, or not.
 *
 ***********************************************************************/
static Boolean ThumbnailDetailViewUpdateDisplay(UInt16 updateCode) {
  FormType* frm = FrmGetActiveForm();
  Boolean handled = false;

  switch (updateCode) {
  case fUpdateDisplayOptsChanged:
    SetTopVisibleRecord(0);
    /* Fall through */
  case fUpdateFontChanged:
    ThumbnailDetailViewLoadRecords(frm);
    handled = true;
    break;

  case fUpdateDeletedCurrent:
    p.dbI = noRecordSelected;
    /* fall through */
  case fUpdateDeletedAny:
    SetTopVisibleRecord(0);
    ThumbnailDetailViewLoadRecords(frm);
    handled = true;
    break;

  case fUpdateReloadRecords:
    ThumbnailDetailViewLoadRecords(frm);
    handled = true;
    break;
  }

  /* Have a valid selected record (for 5-way navigation) */
  p.dbI = d.top_visible_record;

  return handled;
}

/*
** Load the record data (flags, ...) - but not the picture
*/
static void LoadRecordData(void) {
  MemHandle t = NULL;
  MemPtr ptr = NULL;
  UInt16 attr = 0;
  UInt32 highDataOffset = 0;
  Int16 len = 0;

  PRINT("Loading Record Data for %hd", p.dbI);

  /* Clear unmasked flag */
  d.unmaskedCurrentRecord = false;

  /* Open and lock the record */
  t = DmQueryRecord(d.dbR, p.dbI);
  if (!t) abort();
  ptr = MemHandleLock(t);

  /* Is the record private? */
  DmRecordInfo(d.dbR, p.dbI, &attr, NULL, NULL);
  d.record_private = attr & dmRecAttrSecret;

  /* Read the header data */
  MemMove(&d.record, ptr, sizeof(DiddleBugRecordType));

  /* Read the additional alarm info */
  highDataOffset = sketchDataOffset + d.record.sketchLength;

  len = StrLen((Char*)(ptr + highDataOffset)) + 1; /* +1 for null char */
  if (d.record_name) MemHandleFree(d.record_name);
  d.record_name = MemHandleNew(len);
  ASSERT(d.record_name);
  MemMove(MemHandleLock(d.record_name), ptr + highDataOffset, len);
  MemHandleUnlock(d.record_name);

  highDataOffset += len;

  len = StrLen((Char*)(ptr + highDataOffset)) + 1; /* +1 for null char */
  if (d.record_note) MemHandleFree(d.record_note);
  d.record_note = MemHandleNew(len);
  ASSERT(d.record_note);
  MemMove(MemHandleLock(d.record_note), ptr + highDataOffset, len);
  MemHandleUnlock(d.record_note);

  highDataOffset += len;

  /* Clear old data since there may not be an extra-data block yet */
  MemSet(&d.record_sound, sizeof(AlarmSoundInfoType), 0);
  d.record_sound.listIndex = -1; /* default */
  /* Read new extra-data (if it exists and is from a compatible version) */
  if (d.record.extraLength == sizeof(AlarmSoundInfoType))
    MemMove(&d.record_sound, ptr + highDataOffset, d.record.extraLength);

  /* Unlock record */
  MemHandleUnlock(t);
}

/***********************************************************************
 *
 * FUNCTION:    ThumbnailDetailViewUpdateScrollers
 *
 * DESCRIPTION: This routine draws or erases the thumbnail view scroll arrow
 *              buttons.
 *
 * PARAMETERS:  frm          -  pointer to the to do thumbnail form
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
void ThumbnailDetailViewUpdateScrollers(FormPtr frm) {
  UInt16 pos, maxValue;

  if (d.records_in_cat > recordsPerPage) {
    pos = d.top_row_pos_in_cat / recordsPerRow;
    maxValue = (d.records_in_cat - recordsPerPage) / recordsPerRow;
    if ((d.records_in_cat - recordsPerPage) % recordsPerRow)
      maxValue++;
  } else {
    pos = 0;
    maxValue = 0;
  }

  SclSetScrollBar(GetObjectPointer(frm, ScrollBar), pos, 0, maxValue,
		  recordsPerPage / recordsPerRow);
}

/***********************************************************************
 *
 * FUNCTION:    ThumbnailDetailViewLoadGadgets
 *
 * DESCRIPTION: This routine loads sketches into the thumbnail view form
 *              thumbnail gadgets.
 *
 * PARAMETERS:  recordNum index of the first record to display.
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
static void ThumbnailDetailViewLoadGadgets(FormType* frm) {
  UInt16 row;
  MemHandle recordH;
  DynamicButtonType* btnThumb, *btnName, *btnNameMasked, *btnAlarm;
  MemPtr ptr;
  UInt16 attr;
  Char* record_name, *record_note;
  DiddleBugRecordType record;
  FontID font;
  UInt32 alarmSecs;
  UInt16 recordNum = d.top_visible_record;
  const UInt16 max = Min(recordsPerPage, d.records_in_cat - d.top_row_pos_in_cat);
  Boolean private = false;

  for (row = 0; row < max; row++) {
    /* Get the next record in the current category. */
    recordH = DmQueryNextInCategory (d.dbR, &recordNum, p.category);

    if(row == 0) {
      /* store the position of the first row so we can use */
      /* d.top_row_pos_in_cat+row when drawing             */
      d.top_row_pos_in_cat = recordH ? DmPositionInCategory(d.dbR, recordNum, p.category) : 0;
    }

    btnThumb = (DynamicButtonType*) FrmGetGadgetData(frm, FrmGetObjectIndex(frm, Thumb1 + row));
    btnName = (DynamicButtonType*) FrmGetGadgetData(frm, FrmGetObjectIndex(frm, Thumb1Name + row));
    btnNameMasked = (DynamicButtonType*) FrmGetGadgetData(frm, FrmGetObjectIndex(frm, Thumb1NameMasked + row));
    btnAlarm = (DynamicButtonType*) FrmGetGadgetData(frm, FrmGetObjectIndex(frm, Thumb1Alarm + row));

    /* Store record number */
    btnThumb->value = recordNum;
    btnName->value = recordNum;
    btnNameMasked->value = recordNum;
    btnAlarm->value = recordNum;

    /* Clear old internal values */
    btnThumb->selected = false;

    /* Read record attributes */
    DmRecordInfo(d.dbR, recordNum, &attr, NULL, NULL);
    private = attr & dmRecAttrSecret && d.privateRecordStatus == maskPrivateRecords;
    
    /* Get a pointer to the record */
    ptr = MemHandleLock(recordH);

    if (private) {
      DrawMaskedRecord(btnThumb->content.bmpW, maskPattern);
    } else {
      /* Uncompress thumbnail */
      MemMove(BmpGetBits(WinGetBitmap(btnThumb->content.bmpW)), ptr + sketchDataOffset, sketchThumbnailSize);
    }

    /* Read the additional alarm info */
    MemMove(&record, ptr, sizeof(DiddleBugRecordType));
    record_name = ptr + sketchDataOffset + record.sketchLength;
    record_note = record_name + StrLen(record_name) + 1;
    
    /* Initialize detail gadgets' content */
    if (!private) {
      /* Sketch name */
      StrNCopy(btnName->content.text, record_name, TITLE_MAX_LEN);
      font = FntGetFont();
      FntSetFont(btnThumb->font);
      TxtGlueTruncateString(btnName->content.text, btnName->contentRect.extent.x);
      FntSetFont(font);
    }

    /* Note button */
    if (StrLen(record_note))
      FrmShowObject(frm, FrmGetObjectIndex(frm, Thumb1NoteButton + row));
    else
      FrmHideObject(frm, FrmGetObjectIndex(frm, Thumb1NoteButton + row));
    /* Alarm string */
    alarmSecs = ApptGetAlarmTime(&record, TimGetSeconds());
    if (record.flags & RECORD_FLAG_ALARM_SET && alarmSecs) {
      MakeDateTimeStamp(alarmSecs, btnAlarm->content.text);
      FrmShowObject(frm, FrmGetObjectIndex(frm, Thumb1Alarm + row));
    } else {
      btnAlarm->content.text[0] = 0x00;
      FrmHideObject(frm, FrmGetObjectIndex(frm, Thumb1Alarm + row));
    }

    /* Clean up */
    MemHandleUnlock(recordH);

    recordNum++;

    /* Show the right gadgets... */
    FrmShowObject(frm, FrmGetObjectIndex(frm, Thumb1 + row));
    if (!private) {
      FrmHideObject(frm, FrmGetObjectIndex(frm, Thumb1NameMasked + row));
      FrmShowObject(frm, FrmGetObjectIndex(frm, Thumb1Name + row));
    } else {
      FrmHideObject(frm, FrmGetObjectIndex(frm, Thumb1Name + row));
      FrmShowObject(frm, FrmGetObjectIndex(frm, Thumb1NameMasked + row));
    }
    FrmShowObject(frm, FrmGetObjectIndex(frm, Thumb1AlarmButton + row));
    FrmShowObject(frm, FrmGetObjectIndex(frm, Thumb1TrashButton + row));
    FrmShowObject(frm, FrmGetObjectIndex(frm, Line1 + row));
  }

  /* ... and hide the rest */
  for (; row < recordsPerPage; row++) {
    FrmHideObject(frm, FrmGetObjectIndex(frm, Thumb1 + row));
    FrmHideObject(frm, FrmGetObjectIndex(frm, Thumb1Name + row));
    FrmHideObject(frm, FrmGetObjectIndex(frm, Thumb1NameMasked + row));
    FrmHideObject(frm, FrmGetObjectIndex(frm, Thumb1NoteButton + row));
    FrmHideObject(frm, FrmGetObjectIndex(frm, Thumb1AlarmButton + row));
    FrmHideObject(frm, FrmGetObjectIndex(frm, Thumb1Alarm + row));
    FrmHideObject(frm, FrmGetObjectIndex(frm, Thumb1TrashButton + row));
    FrmHideObject(frm, FrmGetObjectIndex(frm, Line1 + row));
  }

  /* Update the scroll arrows. */
  ThumbnailDetailViewUpdateScrollers (frm);
}

/***********************************************************************
 *
 * FUNCTION:    ThumbnailDetailViewLoadRecords
 *
 * DESCRIPTION: This routine loads sketches into the thumbnails view form.
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
void ThumbnailDetailViewLoadRecords (FormType* frm) {
  UInt16 recordNum;

  if (p.dbI != noRecordSelected) {
    /* Is the current record before the first visible record? */
    if (d.top_visible_record > p.dbI)
      SetTopVisibleRecord(p.dbI);

    /* Is the current record after the last visible record? */
    else {
      recordNum = d.top_visible_record;
      DmSeekRecordInCategory(d.dbR, &recordNum, numberOfRows - 1,
			     dmSeekForward, p.category);
      if (recordNum < p.dbI)
	SetTopVisibleRecord(p.dbI);
    }
  }

  /* Make sure we show a full display of records. */
  if (d.records_in_cat) {
    recordNum = dmMaxRecordIndex;
    DmSeekRecordInCategory(d.dbR, &recordNum, recordsPerPage - 1, dmSeekBackward, p.category);
    SetTopVisibleRecord(Min(d.top_visible_record, recordNum));

    /* Ensure that the top visible record actually is part of this category */
    recordNum = d.top_visible_record;
    DmSeekRecordInCategory(d.dbR, &recordNum, 0, dmSeekForward, p.category);
    d.top_visible_record = recordNum;
  } else
    SetTopVisibleRecord(0);

  ThumbnailDetailViewLoadGadgets(frm);
}

/***********************************************************************
 *
 * FUNCTION:    ThumbnailDetailViewSelectCategory
 *
 * DESCRIPTION: This routine handles selection, creation and deletion of
 *              categories form the Details Dialog.
 *
 * PARAMETERS:  nothing
 *
 * RETURNED:    The index of the new category.
 *
 *              The following global variables are modified:
 *			p.category
 *			d.show_all_categories
 *			d.categoryName
 *
 ***********************************************************************/
static UInt16 ThumbnailDetailViewSelectCategory(void) {
  FormPtr frm = FrmGetActiveForm();
  UInt16 category = p.category; 
  Boolean categoryEdited = false;

  /* Process the category popup list. */
  categoryEdited = CategorySelect(d.dbR, frm, CategoryPop,
				  CategoryList, true, &category, d.categoryName, 1,
				  categoryDefaultEditCategoryString);

  if (categoryEdited || (category != p.category)) {
    ChangeCategory(category);

    /* Disable 5-way navigation */
    d.fiveWayNavigation = false;
    p.dbI = noRecordSelected;

    /* Display the new category. */
    ThumbnailDetailViewLoadRecords(frm);
  }

  return (category);
}

/***********************************************************************
 *
 * FUNCTION:    ThumbnailDetailViewNextCategory
 *
 * DESCRIPTION: This routine display the next category,  if the last
 *              category isn't being displayed
 *
 * PARAMETERS:  nothing
 *
 * RETURNED:    nothing
 *
 *              The following global variables are modified:
 *			p.category
 *			d.categoryName
 *
 ***********************************************************************/
void ThumbnailDetailViewNextCategory(void) {
  UInt16 category = CategoryGetNext(d.dbR, p.category);

  if (category != p.category) {
    FormPtr frm = FrmGetActiveForm();
    ControlPtr ctl = GetObjectPointer(frm, CategoryPop);

    ChangeCategory(category);

    /* Set the label of the category trigger. */
    CategoryGetName(d.dbR, p.category, d.categoryName);
    CategorySetTriggerLabel(ctl, d.categoryName);

    /* Display the new category. */
    SetTopVisibleRecord(0);
    ThumbnailDetailViewLoadGadgets(frm);
  }
}

/***********************************************************************
 *
 * FUNCTION:    ThumbnailDetailViewPageScroll
 *
 * DESCRIPTION: This routine scrolls the list of of memo titles
 *              in the direction specified.
 *
 * PARAMETERS:  direction - winUp or down
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
static void ThumbnailDetailViewPageScroll (WinDirectionType direction) {
  UInt16 newTopVisibleRecord;

  newTopVisibleRecord = d.top_visible_record;
  p.dbI = noRecordSelected;

  /* Scroll the table winDown a page (less one row). */
  if (direction == winDown) {
    /* Try going forward one page */
    if (!SeekRecord(&newTopVisibleRecord, recordsPerPage - 1, dmSeekForward)) {
      /* Try going backwards one page from the last record */
      newTopVisibleRecord = dmMaxRecordIndex;
      if (!SeekRecord(&newTopVisibleRecord, recordsPerPage - 1, dmSeekBackward)) {
	/* Not enough records to fill one page.  Start with the first record */
	newTopVisibleRecord = 0;
	SeekRecord(&newTopVisibleRecord, 0, dmSeekForward);
      }
    }
  } else {
    /* Scroll up a page (less one row). */
    if (!SeekRecord(&newTopVisibleRecord, recordsPerPage - 1, dmSeekBackward)) {
      /* Not enough records to fill one page.  Start with the first record */
      newTopVisibleRecord = 0;
      SeekRecord(&newTopVisibleRecord, 0, dmSeekForward);
    }
  }

  /* Avoid redraw if no change */
  if (d.top_visible_record != newTopVisibleRecord) {
    SetTopVisibleRecord(newTopVisibleRecord);
    ThumbnailDetailViewLoadRecords(FrmGetActiveForm());
  }
}

/***********************************************************************
 *
 * FUNCTION:    ThumbnailDetailViewScroll
 *
 * DESCRIPTION: This routine scrolls the list of of memo titles
 *              in the direction specified.
 *
 * PARAMETERS:  direction - winUp or down
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
static void ThumbnailDetailViewScroll(Int16 linesToScroll) {
  UInt16 newTopVisibleRecord = d.top_visible_record;
  FormType* frm = FrmGetActiveForm();

/*   p.dbI = noRecordSelected; */

  /* Scroll down. */
  if (linesToScroll > 0)
    SeekRecord (&newTopVisibleRecord, linesToScroll * recordsPerRow, dmSeekForward);

  /* Scroll up. */
  else if (linesToScroll < 0)
    SeekRecord (&newTopVisibleRecord, -linesToScroll * recordsPerRow, dmSeekBackward);

  ErrFatalDisplayIf (d.top_visible_record == newTopVisibleRecord, "Invalid scroll value");

  SetTopVisibleRecord(newTopVisibleRecord);

  ThumbnailDetailViewLoadGadgets(frm);
}

/*
** Check password and unlock masked record
*/
static Boolean ThumbnailDetailUnlockMaskedRecords(void) {
  const UInt16 oldDbI = p.dbI;

  PRINT("Unlocking %hd", p.dbI);
  
  if (SecVerifyPW(showPrivateRecords)) {
    p.dbI = oldDbI; /* Mysteriously p.dbI gets overwritten during SecVerifyPW when called from ThumbnailDetailForm */
    PRINT("Still unlocking %hd", p.dbI);

   /* We only want to unmask this one record, so restore the preference. */
    PrefSetPreference(prefShowPrivateRecords, maskPrivateRecords);

    PRINT("Done unlocking %hd", p.dbI);

    return true;
  }
  
  p.dbI = oldDbI; /* Mysteriously p.dbI gets overwritten during SecVerifyPW when called from ThumbnailDetailForm */
  PRINT("Not unlocked %hd", p.dbI);

  return false;
}

/*
** Handles events for my custom thumbnail gadgets.
*/
static Boolean ThumbnailDetailGadgetEvent(FormGadgetTypeInCallback* gadgetP, UInt16 cmd, void* paramP) {
  Boolean handled = false;
  DynamicButtonType* btn = (DynamicButtonType*)gadgetP->data;

  switch (cmd) {
  case formGadgetDeleteCmd:
    DynBtnDelete(btn);
    handled = true;
    break;

  case formGadgetDrawCmd:
    DynBtnDraw(btn);
    gadgetP->attr.visible = true;
    handled = true;
    break;

  case formGadgetEraseCmd:
    WinEraseRectangleFrame(btn->frame, &btn->contentRect);
    break;

  case formGadgetHandleEventCmd:
    {
      EventType* e = (EventType*) paramP;

      if (e->eType == frmGadgetEnterEvent) {
	if (DynBtnTrackPen(btn)) {
	  UInt16 attr;

	  /* A thumbnail was selected, display it. */
	  p.dbI = btn->value;

	  /* Get the category and secret attribute of the current record. */
	  DmRecordInfo(d.dbR, p.dbI, &attr, NULL, NULL);

	  /* If this is a "private" record, then determine what is to be shown. */
	  if (!(attr & dmRecAttrSecret) || d.privateRecordStatus != hidePrivateRecords) {
	    switch (btn->id) {
	    case Thumb1Name:
	    case Thumb2Name:
	    case Thumb3Name:
	    case Thumb4Name:
	      GotoRecordField(btn->id, ffDetails);
	      break;

	    case Thumb1NameMasked:
	    case Thumb2NameMasked:
	    case Thumb3NameMasked:
	    case Thumb4NameMasked:
	      if (ThumbnailDetailUnlockMaskedRecords())
		GotoRecordField(btn->id, ffDetails);
	      break;

	    case Thumb1Alarm:
	    case Thumb2Alarm:
	    case Thumb3Alarm:
	    case Thumb4Alarm:
	      if (d.privateRecordStatus != maskPrivateRecords || ThumbnailDetailUnlockMaskedRecords())
		GotoRecordField(btn->id, ffAlarm);
	      break;

	    case Thumb1:
	    case Thumb2:
	    case Thumb3:
	    case Thumb4:
	      FrmGotoForm(p.flags & PFLAGS_WITH_TITLEBAR ? DiddleTForm : DiddleForm);
	      break;
	    }
	  }
	}
      }

      handled = true;
    } break;
  }

  return handled;
}

/*
** Handles events for line gadgets.
*/
static Boolean ThumbnailLineEvent(FormGadgetTypeInCallback* gadgetP, UInt16 cmd, void* paramP) {
  Boolean handled = false;
  DynamicButtonType* btn = (DynamicButtonType*)gadgetP->data;

  switch (cmd) {
  case formGadgetDeleteCmd:
    DynBtnDelete(btn);
    handled = true;
    break;

  case formGadgetDrawCmd:
    DynBtnDraw(btn);
    gadgetP->attr.visible = true;
    handled = true;
    break;

  case formGadgetHandleEventCmd:
    /* ignore */
    handled = true;
    break;
  }

  return handled;
}

/***********************************************************************
 *
 * FUNCTION:    ThumbnailDetailViewInit
 *
 * DESCRIPTION: This routine initializes the "Thumbnail View".
 *
 * PARAMETERS:  event  - a pointer to an EventType structure
 *
 * RETURNED:    nothing.
 *
 ***********************************************************************/
static void ThumbnailDetailViewInit (FormPtr frm) {
  ControlPtr ctl;
  UInt16 i;
  Err err;
  DynamicButtonType* btn;
  WinHandle oldH;

  for (i = 0; i < recordsPerPage; ++i) {
    btn = DynBtnInitGadget(Thumb1 + i, rectangleFrame, true, true, false,false, dynBtnGraphical, 0, &err, frm,
			   ThumbnailDetailGadgetEvent);
    if (err != errNone) abort();

    btn = DynBtnInitGadget(Thumb1Name + i, noFrame, false, true, false,d.hires, dynBtnText, TITLE_MAX_LEN, &err,
			   frm, ThumbnailDetailGadgetEvent);
    if (err != errNone) abort();
    /*     btn->font = stdFont; */

    btn = DynBtnInitGadget(Thumb1NameMasked + i, noFrame, false, true, false, 
			   d.hires,
			   dynBtnGraphical, 0, &err, frm,
			   ThumbnailDetailGadgetEvent);
    if (err != errNone) abort();
    DrawMaskedRecord(btn->content.bmpW, maskPattern);

    btn = DynBtnInitGadget(Thumb1Alarm + i, noFrame, false, true, false, d.hires, dynBtnText, 31, &err, frm,
			   ThumbnailDetailGadgetEvent);
    if (err != errNone) abort();
    btn->font = stdFont;

    btn = DynBtnInitGadget(Line1 + i, noFrame, false, true, false, d.hires, dynBtnGraphical, 0, &err, frm,
			   ThumbnailLineEvent);
    if (err != errNone) abort();

    /* Set up drawing context */
    oldH = WinSetDrawWindow(btn->content.bmpW);
    WinPushDrawState();

    WinDrawGrayLine(0, 1, btn->contentRect.extent.x, 1);

    /* Clean up */
    WinPopDrawState();
    WinSetDrawWindow(oldH);
  }

  ThumbnailDetailViewLoadRecords(frm);

  /* Set the label of the category trigger. */
  ctl = GetObjectPointer(frm, CategoryPop);
  CategoryGetName(d.dbR, p.category, d.categoryName);
  CategorySetTriggerLabel(ctl, d.categoryName);

  /* Update scroll bar variables */
  ThumbnailDetailViewUpdateScrollers(frm);
}

/*
** Handle Security command
*/
static void ThumbnailDoCmdSecurity(void) {
  FormType* frm = FrmGetActiveForm();
  const privateRecordViewEnum oldPrivateRecordStatus = d.privateRecordStatus;
  UInt16 mode = 0;

  d.privateRecordStatus = SecSelectViewStatus();

  if ((oldPrivateRecordStatus == hidePrivateRecords) ^ (d.privateRecordStatus == hidePrivateRecords)) {
    /* We have to close the database first */
    if (DmCloseDatabase(d.dbR) != errNone) abort();

    mode = dmModeReadWrite;
    if (d.privateRecordStatus != hidePrivateRecords)
      mode |= dmModeShowSecret;

    /* Re-open the database */
    if ((d.dbR = DmOpenDatabaseByTypeCreator(DBType, AppType, mode)) == NULL)
      abort();

    /* Update cached values */
    d.records_in_cat = DmNumRecordsInCategory(d.dbR, p.category);
    if (d.privateRecordStatus == hidePrivateRecords)
      SetTopVisibleRecord(0);

    ThumbnailDetailViewLoadRecords(frm);
  } else if ((oldPrivateRecordStatus == maskPrivateRecords) ^ (d.privateRecordStatus == maskPrivateRecords)) {
    ThumbnailDetailViewLoadRecords(frm);
  }
}

/*
** Move the cursor up one step and scroll if necessary.
*/
static void ThumbnailDetailMoveCursorUp(UInt16 old_dbI) {
  UInt16 curPos, oldPos;

  /* Get the positions corresponding to the record indices */
  oldPos = DmPositionInCategory(d.dbR, old_dbI, p.category);
  if (DmGetLastErr() != errNone) abort();
  curPos = DmPositionInCategory(d.dbR, p.dbI, p.category);
  if (DmGetLastErr() != errNone) abort();

  /* Deselect previous item */
  ToggleButton(oldPos - d.top_row_pos_in_cat, false);

  /* Select new item, scroll if necessary */
   if (oldPos - d.top_row_pos_in_cat == 0)
     ThumbnailDetailViewScroll(-1);
  ToggleButton(curPos - d.top_row_pos_in_cat, true);
}

/*
** Move the cursor down one step and scroll if necessary.
*/
static void ThumbnailDetailMoveCursorDown(UInt16 old_dbI) {
  UInt16 curPos, oldPos;
       
  /* Get the positions corresponding to the record indices */
  oldPos = DmPositionInCategory(d.dbR, old_dbI, p.category);
  if (DmGetLastErr() != errNone) abort();
  curPos = DmPositionInCategory(d.dbR, p.dbI, p.category);
  if (DmGetLastErr() != errNone) abort();

  /* Deselect previous item */
  ToggleButton(oldPos - d.top_row_pos_in_cat, false);

  /* Select new item, scroll if necessary */
  if (curPos - d.top_row_pos_in_cat >= numberOfRows)
    ThumbnailDetailViewScroll(1);
  ToggleButton(curPos - d.top_row_pos_in_cat, true);
}

static void ToggleButton(UInt16 offset, Boolean select) {
  FormType* frm = FrmGetActiveForm();
  DynamicButtonType* btn = FrmGetGadgetData(frm, FrmGetObjectIndex(frm, Thumb1 + offset));

  btn->selected = select;
  DynBtnDraw(btn);
}

/*
** Handle 5-way navigation (left).
*/
static Boolean ThumbnailDetailHandleFiveWayNavLeft(void) {
  d.fiveWayNavigation = false;
  FrmGotoForm(DiddleThumbnailForm);

  return true;
}

/*
** Handle 5-way navigation (right).
*/
static Boolean ThumbnailDetailHandleFiveWayNavRight(void) {
  d.fiveWayNavigation = false;
  FrmGotoForm(DiddleListForm);

  return true;
}

/*
** Handle 5-way navigation select.
*/
static Boolean ThumbnailDetailHandleFiveWayNavSelect(void) {
  if (!d.fiveWayNavigation) {
    UInt16 pos;
    
    if (p.dbI == noRecordSelected && d.records_in_cat)
      p.dbI = d.top_visible_record;
    
    if (p.dbI != noRecordSelected) {
      pos = DmPositionInCategory(d.dbR, p.dbI, p.category);
      if (DmGetLastErr() != errNone) abort();
      
	      d.fiveWayNavigation = true;
	      ToggleButton(pos - d.top_row_pos_in_cat, true);
    }
  } else {
    d.fiveWayNavigation = false;
    
    FrmGotoForm(p.flags & PFLAGS_WITH_TITLEBAR ? DiddleTForm : DiddleForm);
  }
  
  return true;
}

/***********************************************************************
 *
 * FUNCTION:    ThumbnailDetailViewHandleEvent
 *
 * DESCRIPTION: This routine is the event handler for the "Thumbnail View"
 *              of the Record application.
 *
 * PARAMETERS:  event  - a pointer to an EventType structure
 *
 * RETURNED:    true if the event was handled and should not be passed
 *              to a higher level handler.
 *
 ***********************************************************************/
Boolean ThumbnailDetailViewHandleEvent (EventPtr event) {
  Boolean handled = false;

  switch (event->eType) {
  case keyDownEvent:
    /* Hardware button pressed? */
    if (TxtCharIsHardKey(event->data.keyDown.modifiers, event->data.keyDown.chr)) {
      if (!(event->data.keyDown.modifiers & poweredOnKeyMask))
	ThumbnailDetailViewNextCategory();
      handled = true;
    } else if (EvtKeydownIsVirtual(event)) {
      switch (event->data.keyDown.chr) {
      case vchrPageUp:
      case vchrRockerUp:
	if (!d.fiveWayNavigation) {
	  ThumbnailDetailViewPageScroll(winUp);
	} else {
	  Int16 old_dbI = p.dbI;
	  
	  /* Try going backward one record */
	  if (!SeekRecord(&p.dbI, 1, dmSeekBackward))
	    p.dbI = old_dbI; /* Nowhere to go, that's OK */
	  else
	    ThumbnailDetailMoveCursorUp(old_dbI);
	}
	handled = true;
	break;

      case vchrPageDown:
      case vchrRockerDown:
	if (!d.fiveWayNavigation) {
	  ThumbnailDetailViewPageScroll(winDown);
	} else {
	  Int16 old_dbI = p.dbI;

	  /* Try going forward one record */
	  if (!SeekRecord(&p.dbI, 1, dmSeekForward))
	    p.dbI = old_dbI; /* Nowhere to go, that's OK */
	  else
	    ThumbnailDetailMoveCursorDown(old_dbI);
	}
	handled = true;
	break;

	/* Treo 600 5-way navigation */
      case vchrRockerCenter:
	handled = ThumbnailDetailHandleFiveWayNavSelect();
	break;
      case vchrRockerLeft:
	handled = ThumbnailDetailHandleFiveWayNavLeft();
	break;
      case vchrRockerRight:
	handled = ThumbnailDetailHandleFiveWayNavRight();
	break;
	
	/* Tungsten 5-way navigation */
      case vchrNavChange:
	if (NavDirectionPressed(event, Left))
	  handled = ThumbnailDetailHandleFiveWayNavLeft();
	else if (NavDirectionPressed(event, Right))
	  handled = ThumbnailDetailHandleFiveWayNavRight();
	else if (NavSelectPressed(event))
	  handled = ThumbnailDetailHandleFiveWayNavSelect();
	break;

/*       case vchrSendData: */
/* 	ThumbnailDetailViewDoCommand(ThumbnailRecordBeamCategoryCmd); */
/* 	handled = true; */
/* 	break; */

      default:
	/* ignore */
      }
    }
    break;

  case ctlSelectEvent:
    switch (event->data.ctlSelect.controlID) {
    case NewButton:
      /* Create new sketch after all existing ones */
      p.dbI = DmNumRecords(d.dbR);

      AllocImage();
      FrmGotoForm(p.flags & PFLAGS_WITH_TITLEBAR ? DiddleTForm : DiddleForm);
      handled = true;
      break;

    case CategoryPop:
      ThumbnailDetailViewSelectCategory();
      handled = true;
      break;

    case TextListButton:
      FrmGotoForm(DiddleListForm);
      handled = true;
      break;

    case ThumbnailListButton:
      FrmGotoForm(DiddleThumbnailForm);
      handled = true;
      break;

    case Thumb1NoteButton:
    case Thumb2NoteButton:
    case Thumb3NoteButton:
    case Thumb4NoteButton:
      {
	const UInt16 thumbID = Thumb1 + event->data.ctlSelect.controlID - Thumb1NoteButton;	
	FormType* frm = FrmGetActiveForm();
	DynamicButtonType* btn = (DynamicButtonType*) FrmGetGadgetData(frm, FrmGetObjectIndex(frm, thumbID));
	UInt16 attr = 0;
	
	/* Check if this is a private record */
	DmRecordInfo(d.dbR, btn->value, &attr, NULL, NULL);
	if (!(attr & dmRecAttrSecret) || 
	    d.privateRecordStatus != maskPrivateRecords ||
	    ThumbnailDetailUnlockMaskedRecords()) {
	  GotoRecordField(thumbID, ffNote);
	}
	handled = true;
      }
      break;

    case Thumb1AlarmButton:
    case Thumb2AlarmButton:
    case Thumb3AlarmButton:
    case Thumb4AlarmButton:
      {
	const UInt16 thumbID = Thumb1 + event->data.ctlSelect.controlID - Thumb1AlarmButton;	
	FormType* frm = FrmGetActiveForm();
	DynamicButtonType* btn = (DynamicButtonType*) FrmGetGadgetData(frm, FrmGetObjectIndex(frm, thumbID));
	UInt16 attr = 0;
	
	/* Check if this is a private record */
	DmRecordInfo(d.dbR, btn->value, &attr, NULL, NULL);
	if (!(attr & dmRecAttrSecret) || 
	    d.privateRecordStatus != maskPrivateRecords ||
	    ThumbnailDetailUnlockMaskedRecords()) {
	  GotoRecordField(thumbID, ffAlarm);
	}
	handled = true;
      }
      break;

    case Thumb1TrashButton:
    case Thumb2TrashButton:
    case Thumb3TrashButton:
    case Thumb4TrashButton: 
      {
	FormType* frm = FrmGetActiveForm();
	DynamicButtonType* btn = (DynamicButtonType*) FrmGetGadgetData(frm, FrmGetObjectIndex(frm, Thumb1 + event->data.ctlSelect.controlID - Thumb1TrashButton));
	
	p.dbI = btn->value;
	LoadRecordData();
      
	/* If this is a "private" record, show password dialog if necessary */
	if (!d.record_private || d.privateRecordStatus != maskPrivateRecords ||
	    ThumbnailDetailUnlockMaskedRecords())
	  DoCmdRemove();

	p.dbI = noRecordSelected;
	d.fiveWayNavigation = false;
	ThumbnailDetailViewLoadRecords(frm);
	
	handled = true;
      } 
      break;
    }
    break;

  case frmGotoEvent:
    p.dbI = event->data.frmGoto.recordNum;
    SwitchCategoryForGoto();
    FrmGotoForm(p.flags & PFLAGS_WITH_TITLEBAR ? DiddleTForm : DiddleForm);
    event->eType = dbOpenRecordFieldEvent;
    EvtAddEventToQueue(event);

    handled = true;
    break;

  case menuEvent:
    {
      Char chr = 0;

      switch(event->data.menu.itemID) {
      case menuitemID_CmdAbout:
	DoAboutDialog();
	handled = true;
	break;

      case menuitemID_CmdSecurity:
	ThumbnailDoCmdSecurity();
	handled = true;
	break;

      case menuitemID_CmdSortByAlarm:      
      case menuitemID_CmdSortByName:
	if (event->data.menu.itemID == menuitemID_CmdSortByAlarm)
	  DmInsertionSort(d.dbR, &SortByAlarmTime, 0);
	else
	  DmInsertionSort(d.dbR, &SortByName, 0);

	SetTopVisibleRecord(0);
	ThumbnailDetailViewLoadRecords(FrmGetActiveForm());
	handled = true;
	break;

      case menuitemID_CmdPref: chr=cmdPref; break;
      case menuitemID_CmdExtPref: chr=cmdExtPref; break;
      case menuitemID_CmdHWPref: chr=cmdHWPref; break;
      case menuitemID_CmdAlarmPref: chr=cmdAlarmPref; break;
      case menuitemID_CmdHelp: chr=cmdHelp; break;
      }

      if (!handled)
	handled = KeyDown(chr);
    }
    break;

  case frmOpenEvent:
    {
      FormType* frm = FrmGetActiveForm ();
      ThumbnailDetailViewInit (frm);
      FrmDrawForm (frm);
      
      /* Disable 5-way navigation initially */
      d.fiveWayNavigation = false;
      
      /* Clean up */
      if (d.record_name) MemHandleFree(d.record_name);
      d.record_name = NULL;
      if (d.record_note)MemHandleFree(d.record_note);
      d.record_note = NULL;
      
      handled = true;
    }
    break;

  case frmUpdateEvent:
    handled = ThumbnailDetailViewUpdateDisplay(event->data.frmUpdate.updateCode);
    break;

  case menuCmdBarOpenEvent:
    MenuCmdBarAddButton(menuCmdBarOnLeft, BarSecureBitmap,
			menuCmdBarResultMenuItem, menuitemID_CmdSecurity, 0);

    /* Tell the field package to not add buttons automatically; */
    /* we've done it all ourselves.                             */
    event->data.menuCmdBarOpen.preventFieldButtons = true;

    /* don't set handled to true; this event must fall through to the system. */
    break;

  case sclRepeatEvent:
    ThumbnailDetailViewScroll (event->data.sclRepeat.newValue - event->data.sclRepeat.value);
    break;

  case dbNonKeyCommandEvent:
    handled = HandleNonKeyCommandCode(event->data.menu.itemID);
    break;

/*   case frmObjectFocusTakeEvent: */
/*     { */
/*       RectangleType r; */
/*       FormType* frm = FrmGetFormPtr(event->data.frmObjectFocusTake.formID); */
/*       const UInt16 id = event->data.frmObjectFocusTake.objectID; */
/*       const UInt16 idx = FrmGetObjectIndex(frm, id); */

/*       if ((id >= Thumb1 && id <= Thumb4) || */
/* 	  (id >= Thumb1Name && id <= Thumb4Name) || */
/* 	  (id >= Thumb1NameMasked && id <= Thumb4NameMasked)) { */
/* 	FrmGetObjectBounds(frm, idx, &r); */
/* 	FrmSetFocus(frm, idx); */
/* 	HsNavDrawFocusRing(frm, id, hsNavFocusRingNoExtraInfo, &r, */
/* 			   hsNavFocusRingStyleObjectTypeDefault, true); */
/* 	handled = true; */
/*       } */
/*     } */
/*     break; */

  default:
    /* do nothing */
  }

  return handled;
}
