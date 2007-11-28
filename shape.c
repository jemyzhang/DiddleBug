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
#include "diddlebugInlines.h"
#include "diddlebugExtern.h"
#include "globals.h"
#include "debug.h"
#include "diddlebugRsc.h"
#include "shape.h"

/* Sony Clie */
#include <SonyHRLib.h>

/* static functions */
static Boolean WithinBounds(Coord x, Coord y) SUTIL3;
static Boolean GetPatternPixel(Coord x, Coord y, 
			       const CustomPatternType* pattern) SUTIL3;
static void MarkAndDrawPixel(Coord x, Coord y, const CustomPatternType* pattern,
			     WinHandle bufferH) SUTIL3;
static void AddToQueue(Coord x, Coord y, PointType** qWrite, PointType* qStart,
		       UInt32 qPointerSize, const CustomPatternType* pattern,
		       WinHandle bufferH) SUTIL3;

/*
** Load rectangle for drawing shapes.
**
** Returns the corner diameter.
*/
UInt16 LoadShapeRect(Coord x, Coord y, RectangleType* r) {
  Int16 side = 0;
  UInt16 corner = 0;

  /* Figure out the rectangle data */
  if (dbShapeCircle != p.shape_type) {
    r->topLeft.x =  Min(d.anchor.x, x);
    r->topLeft.y = Min(d.anchor.y, y);
    r->extent.x = Max(d.anchor.x, x) - r->topLeft.x;
    r->extent.y = Max(d.anchor.y, y) - r->topLeft.y;

    side = Min(r->extent.x, r->extent.y); /* small side */
  } else {
    const Coord xdiff = Max(x - d.anchor.x, d.anchor.x - x);
    const Coord ydiff = Max(y - d.anchor.y, d.anchor.y - y);

    side = Max(xdiff, ydiff); /* large side */

    r->topLeft.x = d.anchor.x - side;
    r->topLeft.y = d.anchor.y - side;
    r->extent.x = 2 * side;
    r->extent.y = 2 * side;
  }

  switch (p.shape_type) {
  case dbShapeRoundedRectangle:
    corner = side / 4;
    break;

  case dbShapeOval:
    corner = side / 2;
    break;

  case dbShapeCircle:
    corner = side;
    break;

  case dbShapeRectangle:
  default:
    corner = 0;
    break;
  }

  /* Ensure minimum size */
  r->extent.x = Max(r->extent.x, 1);
  r->extent.y = Max(r->extent.y, 1);

  PRINT("ShapeRect x %hd, y %hd, width %hd, height %hd",
	r->topLeft.x, r->topLeft.y, r->extent.x, r->extent.y);

  return corner;
}

/*
** Fix shape corner width on HandEra 330
**
** This is really kludgy stuff (avoiding floating point arithmetic).
*/
extern inline void FixCornerHandera(UInt16* corner) {
  *corner = ((*corner) * 2) / 3;
}

/*
** Removes the xor'd shape from the screen.
*/
void ClearOldShape(void) {
  RectangleType r;

  LoadShapeRect(d.ox, d.oy, &r);
  MirrorPartialRealCanvas(&r, true);
}

/*
** Draw the shape on the screen.
*/
void DoShape(Coord x, Coord y) {
  const IndexedColorType oldForeColor = WinSetForeColor(UIColorGetTableEntryIndex(UIObjectSelectedFill));
  const IndexedColorType oldBackColor = WinSetBackColor(ep.paperColor);
  RectangleType rect;
  WinHandle winH = WinGetDrawWindow();

  SetDrawingAreaClip(winH);

  /* Draw the shape */
  if (!d.sonyClie || !d.hires)
    WinDrawRectangle(&rect, LoadShapeRect(x, y, &rect));
  else
    HRWinDrawRectangle(d.sonyHRRefNum, &rect, LoadShapeRect(x, y, &rect));
  
  /* Save point for ClearOldShape */
  d.ox = x;
  d.oy = y;

  /* Clean up */
  WinSetBackColor(oldBackColor);
  WinSetForeColor(oldForeColor);
  ResetDrawingAreaClip(winH);
}

/*
** Draw the final shape on the screen
*/
void EndShape(Coord x, Coord y) {
  Coord dx, dy;
  Int16 i = 0;
  RectangleType rect;
  WinHandle oseh = NULL, osmh = NULL, oldH = NULL;
  Err error = errNone;
  BitmapType* bmp1 = NULL, *bmp2 = NULL;
  BitmapTypeV3* bmpV3 = NULL;
  /* Ensure that the right rectangle is used for hi- and lo-res... */
  const RectangleType* rightR = d.hires ? &d.r_hires : &p.r;
  /* ...and scale spacings for Sony Clie*/
  const Coord px8 = PixelOffset(8, d.hires, d.sonyClie);
  const Coord px16 = PixelOffset(16, d.hires, d.sonyClie);
  UInt16 corner = LoadShapeRect(x, y, &rect);
  const Coord origx = rect.topLeft.x;
  const Coord origy = rect.topLeft.y;
  const Coord adjx = Min(origx - rightR->topLeft.x, px8);
  const Coord adjy = Min(origy - rightR->topLeft.y, px8);

  /* Set up environment so that background pixels are "off" */
  WinPushDrawState();
  WinSetBackColor(0);
  WinSetForeColor(1);

  switch (p.shape_fill) {
  case dbsFillOutline:
  case dbsFillOutlineXOR:
    /* Create the offscreen window to hold rectangle */
    if (d.hires) {
      if (!d.sonyClie) {
	bmp1 = BmpCreate(rect.extent.x + px16, rect.extent.y + px16, 1, NULL, &error);
	if (error) abort();
	bmpV3 = BmpCreateBitmapV3(bmp1, kDensityDouble, BmpGetBits(bmp1), NULL);
	ASSERT(bmpV3);
	osmh = WinCreateBitmapWindow((BitmapType*)bmpV3, &error);
	ASSERT(osmh);
	if (error) abort();
      } else {
	bmp1 = HRBmpCreate(d.sonyHRRefNum, rect.extent.x + px16, rect.extent.y + px16, 1, NULL, &error);
	if (error) abort();
	osmh = HRWinCreateBitmapWindow(d.sonyHRRefNum, bmp1, &error);
	ASSERT(osmh);
	bmp1 = NULL; /* Bitmap should not be deleted twice */
      }
    } else {
      bmp1 = BmpCreate(rect.extent.x + px16, rect.extent.y + px16, 1, NULL, &error);
      if (error) abort();
      osmh = WinCreateBitmapWindow(bmp1, &error);
      ASSERT(osmh);
      if (error) abort();
      bmp1 = NULL; /* Bitmap should not be deleted twice */
    }

    ASSERT(error == errNone);
   
    /* Create second off-screen rectangle for pen-shaped shape */
    if (d.hires) {
      if (!d.sonyClie) {
	bmp2 = BmpCreate(rect.extent.x + px16, rect.extent.y + px16, 1, NULL, &error);
	if (error) abort();
	bmpV3 = BmpCreateBitmapV3(bmp2, kDensityDouble, BmpGetBits(bmp2), NULL);
	ASSERT(bmpV3);
	oseh = WinCreateBitmapWindow((BitmapType*)bmpV3, &error);
	if (error) abort();
      } else {
	bmp2 = HRBmpCreate(d.sonyHRRefNum, rect.extent.x + px16, rect.extent.y + px16, 1, NULL, &error);
	if (error) abort();
	oseh = HRWinCreateBitmapWindow(d.sonyHRRefNum, bmp2, &error);
	if (error) abort();
	bmp2 = NULL; /* Bitmap should not be deleted twice */
      }
    } else {
      bmp2 = BmpCreate(rect.extent.x + px16, rect.extent.y + px16, 1, NULL, &error);
      if (error) abort();
      oseh = WinCreateBitmapWindow(bmp2, &error);
      if (error) abort();
      bmp2 = NULL; /* Bitmap should not be deleted twice */
    }

    /* Draw a one-pixel width rectangle into the window */
    rect.topLeft.x = px8; 
    rect.topLeft.y = px8;
    oldH = WinSetDrawWindow(oseh);
    WinDrawRectangle(&rect, corner);
    if ((rect.extent.x > 2) && (rect.extent.y > 2)) {
      /* The only place where 1 and 2 is real pixels since I don't double the size of the pen. */
      rect.topLeft.x++; 
      rect.topLeft.y++;
      rect.extent.x -= 2; 
      rect.extent.y -= 2;
      WinEraseRectangle(&rect, Max(corner - 1, 0));
      rect.topLeft.x--; 
      rect.topLeft.y--;
      rect.extent.x += 2; 
      rect.extent.y += 2;
    }

    /* Copy the rect into drawable window as defined by the pen */
    for (; i < NPENPIX && p.penpix[i] != 0; ++i) {
      dx = dxFromPenpix(p.penpix[i]);
      dy = dyFromPenpix(p.penpix[i]);

      if (rect.topLeft.x < 0) 
	continue;
      if (rect.topLeft.y < 0) 
	continue;

      WinCopyRectangle(oseh, osmh, &rect, px8 + dx, px8 + dy, winOverlay);
    }
    
    rect.topLeft.x = px8 - adjx; 
    rect.topLeft.y = px8 - adjy;
    rect.extent.x += px8 + adjx; 
    rect.extent.y += px8 + adjy;
    
    /*
    ** This gets very complicated, but for some reason the
    ** scrAND function does not seem to work (Palm V) *shrug*
    */
    /* CopyNOT the mask to extra offscreen window (anti-mask) */
    WinCopyRectangle(osmh, oseh, &rect, px8 - adjx, px8 - adjy, winPaintInverse);
    /* then OR the mask onto the screen */
    /* or XOR it, if we're in outline-xor mode */
    if (p.shape_fill == dbsFillOutlineXOR)
      WinCopyRectangle(osmh, d.realCanvas, &rect, origx - adjx, origy - adjy, winInvert);
    else
      WinCopyRectangle(osmh, d.realCanvas, &rect, origx - adjx, origy - adjy, winOverlay);

    /* Draw a pattern over the mask */
    WinSetDrawWindow(osmh);
    WinFillRectangle(&rect, 0);
    /* OR the anti-mask onto the pattern */
    WinCopyRectangle(oseh, osmh, &rect, px8 - adjx, px8 - adjy, winOverlay);
    /* CopyNOT the anti mask onto the real mask */
    WinCopyRectangle(osmh, oseh, &rect, px8 - adjx, px8 - adjy, winPaintInverse);
    /* XOR the result onto the screen */
    WinCopyRectangle(oseh, d.realCanvas, &rect, origx - adjx, origy - adjy, winInvert);

    /* Whew... cleanup */
    bmpV3 = (BitmapTypeV3*) WinGetBitmap(osmh);
    WinDeleteWindow(osmh, false);
    if (bmp1) BmpDelete(bmp1);
    BmpDelete((BitmapType*) bmpV3);
    bmpV3 = (BitmapTypeV3*) WinGetBitmap(oseh);
    WinDeleteWindow(oseh, false);
    if (bmp2) BmpDelete(bmp2);
    BmpDelete((BitmapType*) bmpV3);
    break;

  case dbsFillFilled:
    oldH = WinSetDrawWindow(d.realCanvas);

    if (p.modeSelection == dbmSelectionErase)
      WinEraseRectangle(&rect, corner);
    else
      WinFillRectangle(&rect, corner);
    break;

  case dbsFillXOR:
    oldH = WinSetDrawWindow(d.realCanvas);
    WinInvertRectangle(&rect, corner);
    break;
  }

  /* Cleanup */
  WinPopDrawState();
  WinSetDrawWindow(oldH);
  MirrorRealCanvas();

  if (!(p.flags&PFLAGS_KEEP_DRAW_TOOL))
    SwitchToDoodleMode();
}

/*
** Remove the xor'd line for rubber-banding
*/
void ClearOldLine(void) {
  RectangleType r;
  
  LineRectangle(d.ox, d.oy, &r);
  MirrorPartialRealCanvas(&r, true);
}

/*
** Returns the bounding rectangle of the line.
*/
void LineRectangle(Coord x, Coord y, RectangleType* r) {
  const Coord lx = Min(d.anchor.x, x);
  const Coord by = Max(d.anchor.y, y);
  const Coord rx = Max(d.anchor.x, x);
  const Coord ty = Min(d.anchor.y, y);
  
  r->topLeft.x = lx;
  r->topLeft.y = ty;
  r->extent.x = rx - lx + 1;
  r->extent.y = by - ty + 1;

  PRINT("LineRect x %hd, y %hd, width %hd, height %hd",
	r->topLeft.x, r->topLeft.y, r->extent.x, r->extent.y);
}

/*
** Draw rubber band line
*/
void DoLine(Coord x, Coord y) {
  const IndexedColorType oldColor = WinSetForeColor(UIColorGetTableEntryIndex(UIObjectSelectedFill));
  WinHandle winH = WinGetDrawWindow();

  SetDrawingAreaClip(winH);

  /* Draw the line */
  if (!d.sonyClie || !d.hires) 
    WinDrawLine(d.anchor.x, d.anchor.y, x, y);
  else
    HRWinDrawLine(d.sonyHRRefNum, d.anchor.x, d.anchor.y, x, y);

  /* Save point for ClearOldLine */
  d.ox = x;
  d.oy = y;

  /* Clean up */
  WinSetForeColor(oldColor);
  ResetDrawingAreaClip(winH);
}

/*
** Place the final line on the drawing
*/
void EndLine(Coord x, Coord y) {
  Coord dx, dy;
  Int16 i = 0;
  const WinHandle oldH = WinSetDrawWindow(d.realCanvas);
  const RectangleType* r = d.hires ? &d.r_hires : &p.r;

  PRINT("EndLine from (%hd, %hd) to (%hd, %hd)", d.anchor.x, d.anchor.y, x, y);

  /* Store old state */
  WinPushDrawState();

  /* Ensure background pixels off */
  WinSetBackColor(0);

  /* Draw the lines defined by the pen */
  for (; i < NPENPIX && p.penpix[i] != 0; ++i) {
    dx = dxFromPenpix(p.penpix[i]);
    dy = dyFromPenpix(p.penpix[i]);

    if (!RctPtInRectangle(d.anchor.x + dx, d.anchor.y + dy, r))
      continue;
    if (p.modeSelection == dbmSelectionErase)
      WinEraseLine(d.anchor.x + dx, d.anchor.y + dy, x + dx, y + dy);
    else
      WinFillLine(d.anchor.x + dx, d.anchor.y + dy, x + dx, y + dy);
  }

  /* Cleanup */
  WinPopDrawState();
  WinSetDrawWindow(oldH);
  MirrorRealCanvas();

  if (!(p.flags&PFLAGS_KEEP_DRAW_TOOL))
    SwitchToDoodleMode();
}

/*
** Check boundaries for flood fill.
*/
static Boolean WithinBounds(Coord x, Coord y) {
  if (!d.sonyClie || !d.hires) {
    const Coord max = MAX_PIXELS - 1;
    
    if ((x < 0)||(x > max)||(y < 0)||( y > max))
      return false;
    else
      return WinGetPixel(x, y) == 0;
  } else {
    const Coord max = MAX_PIXELS_HIRES - 1;
    
    if ((x < 0)||(x > max)||(y < 0)||( y > max))
      return false;
    else
      return HRWinGetPixel(d.sonyHRRefNum, x, y) == 0;
  }
}

/*
** Is the selected pixel in the pattern on?
*/
static Boolean GetPatternPixel(Coord x, Coord y, const CustomPatternType* pat) {
  Boolean res = false;
  const UInt8* pattern = (const UInt8*) pat; /* access the rows of the pattern */

  ASSERT(pat);

  x = x % 8;
  y = y % 8;

  switch (x) {
  case 0:
    res = pattern[y] & 0x01;
    break;
  case 1:
    res = pattern[y] & 0x02;
    break;
  case 2:
    res = pattern[y] & 0x04;
    break;
  case 3:
    res = pattern[y] & 0x08;
    break;
  case 4:
    res = pattern[y] & 0x10;
    break;
  case 5:
    res = pattern[y] & 0x20;
    break;
  case 6:
    res = pattern[y] & 0x40;
    break;
  case 7:
    res = pattern[y] & 0x80;
    break;
  default:
    res = false;
  }

  return res;
}

/*
** Mark pixel as used in the buffer and draw it on the canvas if
** necessary as according to the pattern.
*/
static void MarkAndDrawPixel(Coord x, Coord y, const CustomPatternType* pattern,
			     WinHandle bufferH) {
  ASSERT(pattern);
  ASSERT(bufferH);

  if (!d.sonyClie || !d.hires) {
    WinDrawPixel(x, y);
    if (GetPatternPixel(x, y, pattern)) {
      WinSetDrawWindow(d.realCanvas);
      WinDrawPixel(x, y);
      WinSetDrawWindow(bufferH);
    }
  } else {
    HRWinDrawPixel(d.sonyHRRefNum, x, y);
    if (GetPatternPixel(x, y, pattern)) {
      WinSetDrawWindow(d.realCanvas);
      HRWinDrawPixel(d.sonyHRRefNum, x, y);
      WinSetDrawWindow(bufferH);
    }
  }
}

/*
** Store point in queue for flood fill.
*/
extern inline void StorePoint(Coord x, Coord y, PointType** queue) {
  ASSERT(queue);
  ASSERT(*queue);

  (*queue)->x = x;
  (*queue)->y = y;
  (*queue)++;
}

/*
** Add point to queue for floodfill.
*/
static void AddToQueue(Coord x, Coord y, PointType** qWrite, PointType* qStart,
		       UInt32 qPointerSize, const CustomPatternType* pattern,
		       WinHandle bufferH) {
  if(WithinBounds(x, y)) {
    StorePoint(x, y, qWrite);
    MarkAndDrawPixel(x, y, pattern, bufferH);
    
    if (*qWrite >= (qStart + qPointerSize))
      *qWrite = qStart; /* Loop back to beginning of queue */
  }
}

/*
** Flood fill with starting point.
**
** The algorithm assumes that the given point is valid.
** Uses the given pattern.
*/
void FloodFill(Coord x, Coord y, const CustomPatternType* pattern) {
  WinHandle bufferH = NULL;
  BitmapType* bufferB = NULL;
  BitmapTypeV3* bufferBV3 = NULL;
  MemHandle queueH = NULL;
  PointType* qRead = NULL;
  PointType* qWrite = NULL;
  PointType* qStart = NULL;
  static const UInt32 qPointerSize = 640;
  Coord xt, yt; /* Temporary x and y values */
  Err err = errNone;
  RectangleType r;
  const Coord max = d.hires ? MAX_PIXELS_HIRES : MAX_PIXELS;
  const WinHandle oldH = WinSetDrawWindow(d.realCanvas);

  ASSERT(x >= 0 && x < max);
  ASSERT(y >= 0 && y < max);
  ASSERT(pattern);

  /* Save old state */
  WinPushDrawState();

  /* Set correct background color so that pixels are off */
  WinSetBackColor(0);
  WinSetForeColor(1);
  
  /* Unscale coordinates */
  if (d.hires) {
    if (!d.sonyClie) {
      x = WinUnscaleCoord(x, true);
      y = WinUnscaleCoord(y, true);
    }

    /* Switch to lo-res mode - the pattern should be drawn normally */
    SwitchToLoRes();
  }

  /* Initialize buffer window */
  if (d.hires) {
    if (!d.sonyClie) {
      bufferB = BmpCreate(max, max, 1, NULL, &err);
      if (err != errNone) abort();
      bufferBV3 = BmpCreateBitmapV3(bufferB, kCoordinatesDouble, BmpGetBits(bufferB), NULL);
      ASSERT(bufferBV3);
      bufferH = WinCreateBitmapWindow((BitmapType*) bufferBV3, &err);
      if (err != errNone) abort();
      WinGetDrawWindowBounds(&r);
      WinSetDrawWindow(bufferH);
      WinCopyRectangle(d.realCanvas, NULL, &r, 0, 0, winPaint);
    } else {
      bufferB = HRBmpCreate(d.sonyHRRefNum, max, max, 1, NULL, &err);
      if (err != errNone) abort();
      bufferH = HRWinCreateBitmapWindow(d.sonyHRRefNum, bufferB, &err);
      if (err != errNone) abort();
      HRWinGetWindowBounds(d.sonyHRRefNum, &r);
      WinSetDrawWindow(bufferH);
      HRWinCopyRectangle(d.sonyHRRefNum, d.realCanvas, NULL, &r, 0, 0, winPaint);    
    }
  } else {
    bufferB = BmpCreate(max, max, 1, NULL, &err);
    if (err != errNone) abort();
    bufferH = WinCreateBitmapWindow(bufferB, &err);
    if (err != errNone) abort();
    WinGetDrawWindowBounds(&r);
    WinSetDrawWindow(bufferH);
    WinCopyRectangle(d.realCanvas, NULL, &r, 0, 0, winPaint);  
  }

  /* Initialize queue */
  queueH = MemHandleNew(qPointerSize * sizeof(PointType));
  ASSERT(queueH);
  qStart = qRead = qWrite = MemHandleLock(queueH);
  MemSet(qStart, qPointerSize * sizeof(PointType), 0);

  /* Draw initial point */
  StorePoint(xt = x, yt = y, &qWrite);
  MarkAndDrawPixel(xt, yt, pattern, bufferH);

  /* Do the actual flood fill */
  while (qRead != qWrite) {
    /* Add new members to queue */
    /* Above current pixel */
    AddToQueue(xt, yt - 1, &qWrite, qStart, qPointerSize, pattern, bufferH);
    /* Below current pixel */
    AddToQueue(xt, yt + 1, &qWrite, qStart, qPointerSize, pattern, bufferH);
    /* Left of current pixel */
    AddToQueue(xt - 1, yt, &qWrite, qStart, qPointerSize, pattern, bufferH);
    /* Right of current pixel */
    AddToQueue(xt + 1, yt, &qWrite, qStart, qPointerSize, pattern, bufferH);

    /* Retrieve current queue member */
    qRead++;
    if (qRead >= (qStart + qPointerSize))
      qRead = qStart; /* Loop back to the beginning */

    xt = qRead->x;
    yt = qRead->y;
  }

  /* Restore old draw state */
  WinPopDrawState();

  /* Restore old draw window */
  WinSetDrawWindow(oldH);

  /* Clean up */
  MemHandleUnlock(queueH);
  MemHandleFree(queueH);
  WinDeleteWindow(bufferH, false);
  if (bufferBV3) BmpDelete((BitmapType*) bufferBV3);
  BmpDelete(bufferB);
}
