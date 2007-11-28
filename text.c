/*
** diddlebug - a yellow-sticky-note-thingy
** Copyright (c) 2001, 2002, 2003, 2004 Peter Putzer <pputzer@users.sourceforge.net>.
** Copyright (c) 1999, 2000 Mitch Blevins <mblevin@debian.org>.
** Licensed under the GNU GPL, version 2 or later
** See file "COPYING" that you should have received with this program
** or visit http://www.gnu.org/copyleft/gpl.html
**
** DiddleBug is based on Diddle, which is based on Doodle
** Doodle is Copyright 1997 Roger E Critchlow Jr., San Francisco, California.
** All rights reserved.  Fair use permitted.  Caveat emptor.  No warranty.
*/

#include "diddlebug.h"
#include "diddlebugExtern.h"
#include "globals.h"
#include "debug.h"
#include "text.h"

/* Sony Clie */
#include <SonyHRLib.h>

/*
** Clean up the textual doggie-doos left while placing text.
*/
void ClearOldText(void) {  /* Size the rectangle */
  RectangleType r;
  HRFontID oldFont = 0;

  if (d.sonyClie && d.hires)
    oldFont = HRFntSetFont(d.sonyHRRefNum, HRFntGetFont(d.sonyHRRefNum) + 8);
  
  TextRectangle(d.ox, d.oy, &r);
  MirrorPartialRealCanvas(&r, true);

  /* Clean up */
  if (d.sonyClie && d.hires)
    HRFntSetFont(d.sonyHRRefNum, oldFont);
}

/*
** Returns the bounding rectangle of the text.
*/
void TextRectangle(Coord x, Coord y, RectangleType* r) {
  HRFontID oldFont = 0;

  if (d.sonyClie && d.hires)
    oldFont = HRFntSetFont(d.sonyHRRefNum, HRFntGetFont(d.sonyHRRefNum) + 8);

  /* Set up area to restore */
  r->topLeft.x = x;
  r->topLeft.y = y;
  r->extent.x = FntCharsWidth(d.InsertionText, StrLen(d.InsertionText));
  r->extent.y = FntCharHeight();

  /* Clean up */
  if (d.sonyClie && d.hires)
    HRFntSetFont(d.sonyHRRefNum, oldFont);
}

/*
** A starting or continuing pen stroke while dragging text.
*/
void DoTextInsert(Coord x, Coord y) {
  const WinDrawOperation oldOp = WinSetDrawMode(winOverlay);
  const IndexedColorType oldTextColor = WinSetTextColor(UIColorGetTableEntryIndex(UIObjectSelectedFill));
  const IndexedColorType oldBackColor = WinSetBackColor(ep.paperColor);

  SetDrawingAreaClip(d.winM);

  if (!d.sonyClie || !d.hires) {
    WinPaintChars(d.InsertionText, StrLen(d.InsertionText), x, y);
  } else {
    /* Use the hi-res version of the current font*/
    const HRFontID oldFont = HRFntSetFont(d.sonyHRRefNum, HRFntGetFont(d.sonyHRRefNum) + 8); 
    HRWinPaintChars(d.sonyHRRefNum, d.InsertionText, StrLen(d.InsertionText), x, y);

    /* Clean up */
    HRFntSetFont(d.sonyHRRefNum, oldFont);
  }

  /* Save point for ClearOldText */
  d.ox = x;
  d.oy = y;

  /* Clean up */
  WinSetBackColor(oldBackColor);
  WinSetTextColor(oldTextColor);
  WinSetDrawMode(oldOp);
  ResetDrawingAreaClip(d.winM);
}

/*
** Place the final text when user lifts pen
*/
void EndTextInsert(Coord x, Coord y) {
  RectangleType rect;
  WinHandle oldH = NULL;

  /* Set right font on Sony Clie */
  if (d.sonyClie && d.hires) {
    /* Use the hi-res version of the current font*/
    HRFntSetFont(d.sonyHRRefNum, HRFntGetFont(d.sonyHRRefNum) + 8);
  }

  /* Size the rectangle */
  rect.topLeft.x = x - 3;
  rect.topLeft.y = y;
  rect.extent.x = FntCharsWidth(d.InsertionText, StrLen(d.InsertionText)) + 6;
  rect.extent.y = FntCharHeight();

  /* Set draw window to real canvas */
  oldH = WinSetDrawWindow(d.realCanvas);

  /* Store old state */
  WinPushDrawState();

  /* Set up environment so that background pixels are "off" */
  WinSetBackColor(0);
  WinSetTextColor(1);
  WinSetDrawMode(winOverlay);

  /* Draw the text background first */
  switch (p.txt_mode) {
  case dbTextBlackOnWhite:
    WinEraseRectangle(&rect, 3);
    break;
  case dbTextWhiteOnBlack:
    WinDrawRectangle(&rect, 3);
    WinSetBackColor(1);    
    WinSetTextColor(0);
    if (!d.os5)
      WinSetDrawMode(winErase);
    break;
  case dbTextBlack:
    /* We don't need to do anything */
    break;
  case dbTextXOR:
    WinSetDrawMode(winInvert);
    break;
  default:
    ASSERT(false); /* Should never be reached */
    break;
  }

  /* Now draw the text */
  if (!d.sonyClie || !d.hires)
    WinPaintChars(d.InsertionText, StrLen(d.InsertionText), x, y);
  else
    HRWinPaintChars(d.sonyHRRefNum, d.InsertionText, StrLen(d.InsertionText), x, y);

  /* Clean up */
  SwitchToDoodleMode();
  FntSetFont(d.txt_oldfont);
  WinPopDrawState();
  WinSetDrawWindow(oldH);
  MirrorRealCanvas();
}
