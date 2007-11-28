/*
** diddlebug - a yellow-sticky-note-thingy
** Copyright (c) 2004 Peter Putzer <pputzer@users.sourceforge.net>.
**
** Licensed under the GNU GPL, version 2 or later
** See file "COPYING" that you should have received with this program
** or visit http://www.gnu.org/copyleft/gpl.html
*/

#include "diddlebug.h"
#include "diddlebugExtern.h"
#include "diddlebugInlines.h"
#include "globals.h"
#include "debug.h"
#include "clipboard.h"

/* Forward declarations for helper functions */
static void CopySelection(Coord x, Coord y, RectangleType* r) SUTIL3;
static void AnimateSelectionFinished(RectangleType* r) SUTIL3;

/*
** Clean up the doggie-doos left while placing the clipboard contents.
*/
void ClearOldPaste(void) {  /* Size the rectangle */
  RectangleType r;

  PasteRectangle(d.ox, d.oy, &r);
  MirrorPartialRealCanvas(&r, true);
}

/*
** Returns the bounding rectangle of the clipboard contents.
*/
void PasteRectangle(Coord x, Coord y, RectangleType* r) {
  if (!d.clipboard) {
    /* The clipboard is empty, so we return an empty rectangle */
    r->topLeft.x = 0;
    r->topLeft.y = 0;
    r->extent.x = 0;
    r->extent.y = 0;
  } else {
    /* Figure out the rectangle data */
    GetBounds(d.clipboard, r);
    r->topLeft.x = x;
    r->topLeft.y = y;
  }
}

/*
** A starting or continuing pen stroke while dragging a clipboard item.
*/
void DoPaste(Coord x, Coord y) {
  const IndexedColorType oldForeColor = WinSetForeColor(UIColorGetTableEntryIndex(UIObjectSelectedFill));
  const IndexedColorType oldBackColor = WinSetBackColor(ep.paperColor);
  const WinDrawOperation oldOp = WinSetDrawMode(winOverlay);
  WinHandle winH = WinGetDrawWindow();

  if (!d.clipboard)
    return; // FIXME

  SetDrawingAreaClip(winH);

  /* Draw the shape of the clipboard item */
    WinPaintBitmap(WinGetBitmap(d.clipboard), x, y);
  
  /* Save point for ClearOldShape */
  d.ox = x;
  d.oy = y;

  /* Clean up */
  WinSetDrawMode(oldOp);
  WinSetBackColor(oldBackColor);
  WinSetForeColor(oldForeColor);
  ResetDrawingAreaClip(winH);
}

/*
** Place the clipboard item when user lifts pen
*/
void EndPaste(Coord x, Coord y) {
  RectangleType r1, r2;
  WinHandle oldH = NULL;

  if (!d.clipboard)
    return; // FIXME

  PasteRectangle(x, y, &r1);
  GetBounds(d.clipboard, &r2);

  /* Set up environment so that background pixels are "off" */
  WinPushDrawState();
  WinSetBackColor(0);
  WinSetForeColor(1);

  /* Copy clipboard to sketch */
  oldH = WinSetDrawWindow(d.realCanvas);
  WinCopyRectangle(d.clipboard, d.realCanvas, &r2, 
		   r1.topLeft.x, r1.topLeft.y, winOverlay);
  WinSetDrawWindow(oldH);

  /* Clean up */
  WinPopDrawState();

  /* Mirror the changed area back to the screen */
  r2.topLeft = r1.topLeft;
  MirrorPartialRealCanvas(&r2, true);

  if (!(p.flags&PFLAGS_KEEP_DRAW_TOOL))
    SwitchToDoodleMode();
}

/*
** Clean up the old selection while doing "Cut" or "Copy".
*/
void ClearOldCutCopy(void) {  /* Size the rectangle */
  RectangleType r;

  CutCopyRectangle(d.ox, d.oy, &r);
  WinGetFramesRectangle(rectangleFrame, &r, &r);
  MirrorPartialRealCanvas(&r, true);
}

/*
** Returns the bounding rectangle of the current selection.
*/
void CutCopyRectangle(Coord x, Coord y, RectangleType* r) {
  /* Figure out the rectangle data */
  r->topLeft.x = Min(d.anchor.x, x);
  r->topLeft.y = Min(d.anchor.y, y);
  r->extent.x = Max(d.anchor.x, x) - r->topLeft.x;
  r->extent.y = Max(d.anchor.y, y) - r->topLeft.y;
  
  if (r->extent.x == 0)
    r->extent.x = 1;
  if (r->extent.y == 0)
    r->extent.y = 1;
}

/*
** A starting or continuing pen stroke while selecting
** a part of the current sketch for "Cut" and "Copy".
*/
void DoCutCopy(Coord x, Coord y) {
  const IndexedColorType oldForeColor = WinSetForeColor(UIColorGetTableEntryIndex(UIObjectSelectedFill));
  const IndexedColorType oldBackColor = WinSetBackColor(ep.paperColor);
  RectangleType rect;
  WinHandle winH = WinGetDrawWindow();

  SetDrawingAreaClip(winH);

  /* Draw the shape */
  CutCopyRectangle(x, y, &rect);
    WinDrawRectangleFrame(rectangleFrame, &rect);
  
  /* Save point for ClearOldShape */
  d.ox = x;
  d.oy = y;

  /* Clean up */
  WinSetBackColor(oldBackColor);
  WinSetForeColor(oldForeColor);
  ResetDrawingAreaClip(winH);
}

/*
** Draw inverted selection and beep.
*/
static void AnimateSelectionFinished(RectangleType* r) {
  WinHandle winH = WinGetDrawWindow();
  RectangleType r2 = *r;

  /* Set up drawing state */
  WinPushDrawState();
  SetDrawingAreaClip(winH);
  WinSetForeColor(UIColorGetTableEntryIndex(UIObjectSelectedFill));
  WinSetBackColor(ep.paperColor);
  
    WinDrawRectangle(r, 0);
  
  r2.topLeft.x = 0;
  r2.topLeft.y = 0;
  WinCopyRectangle(d.clipboard, d.winM, &r2, r->topLeft.x, r->topLeft.y, winMask);

  /* Beep */
  SndPlaySystemSound(sndClick);

  /* Wait for a quarter second */
  SysTaskDelay(SysTicksPerSecond() / 4);

  /* Restore sketch */
  MirrorPartialRealCanvas(r, true);
  
  /* Clean up */
  ResetDrawingAreaClip(winH);
  WinPopDrawState();
}

/* 
** Common code for "Cut" and "Copy".
*/
static void CopySelection(Coord x, Coord y, RectangleType* r) {
  BitmapTypeV3* enhancedBmp = NULL;
  Err err = errNone;
  WinHandle oldH = NULL;

  /* Delete old clipboard content */
  if (d.clipboard) {
    FreeWindowAndBitmap(d.clipboard, d.clipboardBmp);
    d.clipboard = NULL;
    d.clipboardBmp = NULL;
  }

  CutCopyRectangle(x, y, r);

  /* Create the conversion bitmap */
  if (d.hires) {
      d.clipboardBmp = BmpCreate(r->extent.x, r->extent.y, 1, NULL, &err);
      if (err) abort();
      enhancedBmp = BmpCreateBitmapV3(d.clipboardBmp, kDensityDouble, BmpGetBits(d.clipboardBmp), NULL);
      d.clipboard = WinCreateBitmapWindow((BitmapType*) enhancedBmp, &err);
      if (err) abort();
  } else {
    d.clipboardBmp = BmpCreate(r->extent.x, r->extent.y, 1, NULL, &err);
    if (err) abort();
    d.clipboard = WinCreateBitmapWindow(d.clipboardBmp, &err);
    if (err) abort();
    d.clipboardBmp = NULL; /* Otherwise the bitmap would be deleted twice */
  }
  if (err) abort();

  /* Set up environment so that background pixels are "off" */
  WinPushDrawState();
  WinSetBackColor(0);
  WinSetForeColor(1);
  
  /* Copy selection to clipboard */
  oldH = WinSetDrawWindow(d.clipboard);
  WinCopyRectangle(d.realCanvas, d.clipboard, r, 0, 0, winOverlay);
  WinSetDrawWindow(oldH);
 
  /* Draw selection and beep */
  AnimateSelectionFinished(r);

  /* Clean up */
  WinPopDrawState();
}

/*
** Copy selection with the final position and size.
*/
void EndCopy(Coord x, Coord y) {
  RectangleType r;

  CopySelection(x, y, &r);

  if (!(p.flags&PFLAGS_KEEP_DRAW_TOOL))
    SwitchToDoodleMode();
}

/*
** Cut selection with the final position and size.
*/
void EndCut(Coord x, Coord y) {
  RectangleType r;
  WinHandle oldH = NULL;
  IndexedColorType oldCol;

  CopySelection(x, y, &r);

  /* Erase selection from canvas */
  oldH = WinSetDrawWindow(d.realCanvas);
  oldCol = WinSetBackColor(0);
  WinEraseRectangle(&r, 0);

  /* Clean up */
  WinSetBackColor(oldCol);
  WinSetDrawWindow(oldH);

  /* Mirror the changed area back to the screen */
  MirrorPartialRealCanvas(&r, true);

  if (!(p.flags&PFLAGS_KEEP_DRAW_TOOL))
    SwitchToDoodleMode();
}
