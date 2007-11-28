#ifndef DIDDLEBUG_INLINES_H
#define DIDDLEBUG_INLINES_H

/*

Copyright (c) 2001, 2002, 2004 Peter Putzer <pputzer@users.sourceforge.net>.
Licensed under the GNU General Public License (GPL).

Portions copyright (c) 2000 Palm, Inc. or its subsidiaries.  All rights reserved.

*/

#include "diddlebug.h"
#include "globals.h"

/*
** GetObjectPointer - return an object pointer given a form and objID
*/
extern inline void* GetObjectPointer(FormPtr frm, UInt16 objID) {
  return FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, objID));
}

/*
** HideObject
*/
extern inline void HideObject(FormPtr frm, UInt16 objID) {
  FrmHideObject(frm, FrmGetObjectIndex(frm, objID));
}

/*
** ShowObject
*/
extern inline void ShowObject(FormPtr frm, UInt16 objID) {
  FrmShowObject(frm, FrmGetObjectIndex(frm, objID));
}

/*
** A max() function
*/
extern inline Int16 Max(Int16 a, Int16 b) {
  return a > b ? a : b;
}

/*
** A min() function
*/
extern inline Int16 Min(Int16 a, Int16 b) {
  return a < b ? a : b;
}

/***********************************************************************
 *
 * FUNCTION:    ChangeCategory
 *
 * DESCRIPTION: This routine updates the global varibles that keep track
 *              of category information.
 *
 * PARAMETERS:  category  - new category (index)
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
extern inline void ChangeCategory(UInt16 category) {
  d.records_in_cat = DmNumRecordsInCategory(d.dbR, category);
  p.category = category;
  d.top_visible_record = 0;
}

/*
** Set the top-most record visible and update cached position.
*/
extern inline void SetTopVisibleRecord(UInt16 top) {
  d.top_visible_record = top;
  d.top_row_pos_in_cat = DmPositionInCategory(d.dbR, top, p.category);
}

/* Map flags to Boolean value */
#define CHECK_FLAG(FLAG) (p.flags & FLAG ? true : false)

/*
** Returns the minimum record size for this device:
**
** 25 bytes for the zero data, 80 bytes for thumbnail, 2 bytes for text nulls
*/
extern inline UInt32 MinRecordSize(Boolean hires) {
  return hires ? sketchDataOffset + sketchThumbnailSize + 25 * HiResCoef * HiResCoef + 2 + 1
    : sketchDataOffset + sketchThumbnailSize + 25 + 2 + 1;
}

/*
** Returns the maximum size of a DiddleBug record for this device.
*/
extern inline UInt32 MaxRecordSize(Boolean hires) {
  return hires ? 3331 * HiResCoef * HiResCoef : 3331;
}

/*
** Returns the sketchLength of an empty (all pixels white) sketch.
*/
extern inline UInt32 EmptySketchSize(Boolean hires) {
  return hires ? 181 : 106;
}

/*
** Returns the pixel offset to be used in EndShape.
*/
extern inline Coord PixelOffset(Coord px, Boolean hires, Boolean sonyClie) {
  return (hires && sonyClie) ? px * HiResCoef : px;
}

/*
** WinGetBounds for PalmOS 5.3 devices.
*/
extern inline void GetBounds(WinHandle h, RectangleType* r) {
  WinHandle oldH = WinSetDrawWindow(h);

  /* Get bounds */
  WinGetDrawWindowBounds(r);

  /* Clean up */
  WinSetDrawWindow(oldH);
}

/*
** Throw an out-of-memory exception.
*/
extern inline void NoMemoryError(void) {
  ErrThrow(memErrNotEnoughSpace);
}

/*
** Should we enable the vibrate alarm?
*/
extern inline Boolean ShouldTriggerVibrate(UInt16 flags, pref* prefs) {
  if (flags & RECORD_FLAG_CUSTOM_SIGNAL)
    return flags & RECORD_FLAG_VIBRATE ? true : false;
  else
    return prefs->flags & PFLAGS_DEFAULT_VIBRATE ? true : false;
}

/*
** Should we enable the LED alarm?
*/
extern inline Boolean ShouldTriggerLED(UInt16 flags, pref* prefs) {
  if (flags & RECORD_FLAG_CUSTOM_SIGNAL)
    return flags & RECORD_FLAG_LED ? true : false;
  else
    return prefs->flags & PFLAGS_DEFAULT_LED ? true : false;
}

#endif /* DIDDLEBUG_INLINES_H */
