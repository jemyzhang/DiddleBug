/*

Copyright (c) 2001, 2002, 2004 Peter Putzer <pputzer@users.sourceforge.net>.
Licensed under the GNU General Public License (GPL).

Portions copyright (c) 2000 Palm, Inc. or its subsidiaries.  All rights reserved.

*/

#include "notes.h"
#include "globals.h"
#include "diddlebug.h"
#include "diddlebugRsc.h"
#include "diddlebugInlines.h"
#include "debug.h"

#include <palmOneNavigator.h> /* Palm 5-way navigator */

static void NoteViewLoadRecord(void) SUTIL2;
static void NoteViewSave(void) SUTIL2;
static Boolean NoteViewDeleteNote(void) SUTIL2;
static Boolean NoteViewDoCommand(UInt16 command) SUTIL2;
static void NoteViewScroll(Int16 linesToScroll, Boolean updateScrollbar) SUTIL2;
static void NoteViewPageScroll(WinDirectionType direction) SUTIL2;
static FontID SelectFont(FontID currFontID) SUTIL2;

/***********************************************************************
 *
 * FUNCTION:    SelectFont
 *
 * DESCRIPTION: This routine handles selection of a font in the List
 *              View.
 *
 * PARAMETERS:  currFontID - ID of current font
 *
 * RETURNED:    id of new font
 *
 ***********************************************************************/
static FontID SelectFont (FontID currFontID) {
  UInt16 formID = FrmGetActiveFormID();
  FontID fontID = FontSelect(currFontID); /* Call the OS font selector to get the id of a font. */
  
  if (fontID != currFontID)
    FrmUpdateForm(formID, fUpdateFontChanged);

  return (fontID);
}

/***********************************************************************
 *
 * FUNCTION:    NoteViewDrawTitleAndForm
 *
 * DESCRIPTION: This routine draws the form and title of the note view.
 *
 * PARAMETERS:  frm, pointer to the form to draw
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
void NoteViewDrawTitleAndForm(FormType* frm) {
  Coord x;
  Coord maxWidth;
  Coord formWidth;
  RectangleType r;
  FontID curFont;
  RectangleType drawRect;
  UInt16 descLen, ellipsisWidth;
  Char* linefeedP;
  Int16 descWidth;
  IndexedColorType curForeColor;
  IndexedColorType curBackColor;
  IndexedColorType curTextColor;
  UInt8* lockedWinP;
  Char* desc = MemHandleLock(d.record_name); /* Get current record name */

  /* "Lock" the screen so that all drawing occurs offscreen to avoid     */
  /* the anamolies associated with drawing the Form's title then drawing */
  /* the NoteView title.  We REALLY need to make a variant for doing     */
  /* this in a more official way!                                        */
  lockedWinP = WinScreenLock(winLockCopy);

  FrmDrawForm(frm);

  /* Perform initial set up */
  FrmGetFormBounds(frm, &r);
  formWidth = r.extent.x;
  maxWidth = formWidth - 8;

  /* Clear garbage */
  WinEraseWindow();
  CtlDrawControl(GetObjectPointer(frm, NoteDoneButton));
  CtlDrawControl(GetObjectPointer(frm, NoteDeleteButton));
  FldDrawField(GetObjectPointer(frm, NoteField));
  SclDrawScrollBar(GetObjectPointer(frm, NoteScrollBar));

  linefeedP = StrChr (desc, linefeedChr);
  descLen = (linefeedP == NULL ? StrLen (desc) : linefeedP - desc);
  ellipsisWidth = 0;

  RctSetRectangle (&drawRect, 0, 0, formWidth, FntLineHeight()+2);

  /* Save/Set window colors and font.  Do this after FrmDrawForm() is called */
  /* because FrmDrawForm() leaves the fore/back colors in a state that we    */
  /* don't want here.                                                        */
  curForeColor = WinSetForeColor(UIColorGetTableEntryIndex(UIFormFrame));
  curBackColor = WinSetBackColor(UIColorGetTableEntryIndex(UIFormFill));
  curTextColor = WinSetTextColor(UIColorGetTableEntryIndex(UIFormFrame));
  curFont = FntSetFont(noteTitleFont);

  /* Erase the Form's title area and draw the NoteView's. */
  WinDrawRectangle(&drawRect, 3);

  if (FntWidthToOffset(desc, descLen, maxWidth, NULL, &descWidth) != descLen) {
    ellipsisWidth = FntCharWidth(chrEllipsis);
    descLen = FntWidthToOffset(desc, descLen, maxWidth - ellipsisWidth, NULL, &descWidth);
  }
  
  x = (formWidth - descWidth - ellipsisWidth + 1) / 2;

  WinDrawInvertedChars(desc, descLen, x, 1);
  if (ellipsisWidth != 0) {
    Char buf[maxCharBytes + sizeOf7BitChar(chrNull)];

    buf[TxtSetNextChar(buf, 0, chrEllipsis)] = chrNull;
    WinDrawInvertedChars(buf, StrLen(buf), x + descWidth, 1);
  }

  /* Now that we've drawn everything, blast it all back on the screen at once */
  if (lockedWinP)
    WinScreenUnlock();

  /* Restore window colors and font */
  WinSetForeColor (curForeColor);
  WinSetBackColor (curBackColor);
  WinSetTextColor (curTextColor);
  FntSetFont (curFont);

  /* Clean up */
  MemHandleUnlock(d.record_name);
}

/***********************************************************************
 *
 * FUNCTION:    NoteViewUpdateScrollBar
 *
 * DESCRIPTION: This routine update the scroll bar.
 *
 * PARAMETERS:  nothing
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
void NoteViewUpdateScrollBar (void) {
  UInt16 scrollPos;
  UInt16 textHeight;
  UInt16 fieldHeight;
  Int16 maxValue;
  FormType* frm = FrmGetFormPtr(NewNoteView);
  FieldPtr fld = GetObjectPointer(frm, NoteField);
  ScrollBarPtr bar = GetObjectPointer(frm, NoteScrollBar);

  FldGetScrollValues(fld, &scrollPos, &textHeight,  &fieldHeight);

  if (textHeight > fieldHeight) {
    // On occasion, such as after deleting a multi-line selection of text,
    // the display might be the last few lines of a field followed by some
    // blank lines.  To keep the current position in place and allow the user
    // to "gracefully" scroll out of the blank area, the number of blank lines
    // visible needs to be added to max value.  Otherwise the scroll position
    // may be greater than maxValue, get pinned to maxvalue in SclSetScrollBar
    // resulting in the scroll bar and the display being out of sync.
    maxValue = textHeight - fieldHeight + FldGetNumberOfBlankLines(fld);
  } else {
    maxValue = scrollPos;
  }

  SclSetScrollBar(bar, scrollPos, 0, maxValue, fieldHeight-1);
}

/***********************************************************************
 *
 * FUNCTION:    NoteViewLoadRecord
 *
 * DESCRIPTION: This routine loads the note db field into the note edit
 *              object.
 *
 * PARAMETERS:  frm - pointer to the Edit View form
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
static void NoteViewLoadRecord(void) {
  FieldType* fld = GetObjectPointer(FrmGetFormPtr(NewNoteView), NoteField);
  const UInt32 len = MemHandleSize(d.record_note);
  MemHandle textH = MemHandleNew(len);

  ASSERT(textH);

  /* Set the font used in the note field. */
  FldSetFont(fld, d.note_font);
  MemMove(MemHandleLock(textH), MemHandleLock(d.record_note), len);
  MemHandleUnlock(d.record_note);
  MemHandleUnlock(textH);
  FldSetTextHandle(fld, textH);
}


/***********************************************************************
 *
 * FUNCTION:    NoteViewSave
 *
 * DESCRIPTION: This routine removed any empty space that the field
 *              routines may a add to the note,  the field object
 *              modifies the note db field in place.  The current
 *              record is also mark dirty.
 *
 * PARAMETERS:  nothing
 *
 * RETURNED:    nothing
 *
 * REVISION HISTORY:
 *          Name    Date        Description
 *          ----    ----        -----------
 *          art 2/21/95 Initial Revision
 *
 ***********************************************************************/
static void NoteViewSave (void) {
  FieldPtr fld = GetObjectPointer(FrmGetFormPtr(NewNoteView), NoteField);
  UInt32 len = 0;
  MemHandle textH = NULL;

  /* Was the note string modified by the user. */
  if (FldDirty(fld)) {
    /* Release any free space in the note field. */
    FldCompactText(fld);
  }

  textH = FldGetTextHandle(fld);
  len = MemHandleSize(textH);
  if (MemHandleSize(d.record_note) != len) {
    const Err err = MemHandleResize(d.record_note, len);
    if (err != errNone)
      ErrThrow(err);
  }
  MemMove(MemHandleLock(d.record_note), MemHandleLock(textH), len);
  MemHandleUnlock(d.record_note);
  MemHandleUnlock(textH);
}

/***********************************************************************
 *
 * FUNCTION:    NoteViewDeleteNote
 *
 * DESCRIPTION: This routine deletes a the note field from an appointment
 *              record.
 *
 * PARAMETERS:  nothing
 *
 * RETURNED:    true if the note was deleted.
 *
 ***********************************************************************/
static Boolean NoteViewDeleteNote (void) {
  /* Confirm the operation. */
  if (FrmAlert(DeleteNoteAlert) != 0)
    return false;

  /* Empty note */
  MemHandleFree(d.record_note);
  d.record_note = MemHandleNew(1);
  ASSERT(d.record_note);
  MemSet(MemHandleLock(d.record_note), 1, 0x00);
  MemHandleUnlock(d.record_note);

  return true;
}

/***********************************************************************
 *
 * FUNCTION:    NoteViewDoCommand
 *
 * DESCRIPTION: This routine preforms the menu command specified.
 *
 * PARAMETERS:  command  - menu item id
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
static Boolean NoteViewDoCommand(UInt16 command) {
  FieldPtr fld;
  Boolean handled = true;

  switch (command) {
  case newNoteFontCmd:
    d.note_font = SelectFont(d.note_font);
    break;

  case newNotePhoneLookupCmd:
    fld = GetObjectPointer(FrmGetFormPtr(NewNoteView), NoteField);
    d.in_phone_lookup = true;
    PhoneNumberLookup (fld);
    d.in_phone_lookup = false;
    break;

  default:
    handled = false;
  }

  return handled;
}

/***********************************************************************
 *
 * FUNCTION:    NoteViewScroll
 *
 * DESCRIPTION: This routine scrolls the Note View by the specified
 *                   number of lines.
 *
 * PARAMETERS:  linesToScroll - the number of lines to scroll,
 *                      positive for down,
 *                      negative for up
 *                   updateScrollbar - force a scrollbar update?
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
static void NoteViewScroll(Int16 linesToScroll, Boolean updateScrollbar) {
  FieldPtr fld = GetObjectPointer(FrmGetFormPtr(NewNoteView), NoteField);
  UInt16 blankLines = FldGetNumberOfBlankLines (fld);

  if (linesToScroll < 0)
    FldScrollField(fld, -linesToScroll, winUp);
  else if (linesToScroll > 0)
    FldScrollField(fld, linesToScroll, winDown);

  /*
  ** If there were blank lines visible at the end of the field
  ** then we need to update the scroll bar.
  */
  if ((blankLines && linesToScroll < 0) || updateScrollbar)
    NoteViewUpdateScrollBar();
}

/***********************************************************************
 *
 * FUNCTION:    NoteViewPageScroll
 *
 * DESCRIPTION: This routine scrolls the message a page up or down.
 *
 * PARAMETERS:   direction     up or down
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
static void NoteViewPageScroll(WinDirectionType direction) {
  UInt16 linesToScroll;
  FieldPtr fld = GetObjectPointer(FrmGetFormPtr(NewNoteView), NoteField);

  if (FldScrollable(fld, direction)) {
    linesToScroll = FldGetVisibleLines (fld) - 1;

    if (direction == winUp)
      linesToScroll = -linesToScroll;

    NoteViewScroll(linesToScroll, true);
  }
}

/***********************************************************************
 *
 * FUNCTION:    NoteViewInit
 *
 * DESCRIPTION: This routine initializes the Note View form.
 *
 * PARAMETERS:  frm - pointer to the Note View form.
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
void NoteViewInit(FormType* frm) {
  FieldPtr fld = GetObjectPointer(frm, NoteField);
  FieldAttrType attr;

  // AttnIndicatorEnable(false); // Custom title doesn't support attention indicator.
  NoteViewLoadRecord();

  /* Have the field send events to maintain the scroll bar. */
  FldGetAttributes (fld, &attr);
  attr.hasScrollBar = true;
  FldSetAttributes (fld, &attr);
}


/***********************************************************************
 *
 * FUNCTION:    NoteViewHandleEvent
 *
 * DESCRIPTION: This routine is the event handler for the "Note View".
 *
 * PARAMETERS:  event  - a pointer to an EventType structure
 *
 * RETURNED:    true if the event has handled and should not be passed
 *              to a higher level handler.
 *
 ***********************************************************************/
Boolean NoteViewHandleEvent (EventType* event) {
  Boolean handled = false;

  if (d.exiting)
    return handled;

  switch (event->eType) {
  case keyDownEvent:
    if (EvtKeydownIsVirtual(event)) {
      if (event->data.keyDown.chr == vchrPageUp) {
    NoteViewPageScroll(winUp);
    handled = true;
      } else if (event->data.keyDown.chr == vchrPageDown) {
    NoteViewPageScroll(winDown);
    handled = true;
      } else if (event->data.keyDown.chr == vchrNavChange) {
    if (NavSelectPressed(event)) {
      CtlHitControl(GetObjectPointer(FrmGetActiveForm(), NoteDoneButton));
      handled = true;
    }
      }
    }
    break;

  case ctlSelectEvent:
    switch (event->data.ctlSelect.controlID) {
    case NoteDoneButton:
      NoteViewSave();
      /* Fall through */
      break;

    case NoteDeleteButton:
      if (!NoteViewDeleteNote())
    handled = true;
      /* else */
      /* fall through */
      break;
    }
    break;

  case menuEvent:
    handled = NoteViewDoCommand(event->data.menu.itemID);
    break;

  case fldChangedEvent:
    NoteViewUpdateScrollBar();
    handled = true;
    break;

  case frmUpdateEvent:
    if (event->data.frmUpdate.updateCode & fUpdateFontChanged) {
      FldSetFont(GetObjectPointer(FrmGetFormPtr(NewNoteView), NoteField), d.note_font);
      NoteViewUpdateScrollBar ();
    } else {
      NoteViewDrawTitleAndForm(FrmGetFormPtr(NewNoteView));
    }

    handled = true;
    break;

  case appStopEvent:
  case frmCloseEvent:
    // AttnIndicatorEnable(true); // Custom title doesn't support attention indicator.
    if (FldGetTextHandle(GetObjectPointer(FrmGetFormPtr(NewNoteView), NoteField)))
      NoteViewSave();
    break;

  case sclRepeatEvent:
    NoteViewScroll(event->data.sclRepeat.newValue - event->data.sclRepeat.value, false);
    break;

  default:
    /* nothing */
  }
  
  return handled;
}
