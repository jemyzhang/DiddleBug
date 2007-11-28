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

#include "util.h"
#include "diddlebug.h"
#include "diddlebugExtern.h"
#include "debug.h"

/*
** Show a "message box" containing the given string resource.
**
** The "message box" needs to be erased by UnFlashMessage().
*/
FlashMessageType* FlashMessage(UInt16 strID) {
  RectangleType bounds;
  FlashMessageType* ret = NULL;
  WinHandle oldH = NULL;
  Char message[48];
  Err error = errNone;

  SysCopyStringResource(message, strID);

  /* Allocate memory for FlashMessage data structure */
  ret = (FlashMessageType*)MemPtrNew(sizeof(FlashMessageType));
  if (!ret) return NULL;

  /* Ensure standard coordinates */
  WinPushDrawState();
  SwitchToLoRes();

  /* Set new font */
  FntSetFont(boldFont);

  /* Calculate bounds for "message box" */
  bounds.extent.x = FntLineWidth(message, StrLen(message));
  bounds.extent.y = FntLineHeight();
  RctInsetRectangle(&bounds, -4);
  if (bounds.extent.x > MAX_PIXELS) {
    MemPtrFree(ret);
    WinPopDrawState();
    ASSERT(false);
    return NULL;
  }
  bounds.topLeft.x = (MAX_PIXELS - bounds.extent.x) / 2;
  bounds.topLeft.y = (MAX_PIXELS - bounds.extent.y) / 2;
  RctInsetRectangle(&bounds, -2);
  ret->x = bounds.topLeft.x; 
  ret->y = bounds.topLeft.y;

  /* Save previous screen */
  ret->offscreen_handle = WinSaveBits(&bounds, &error);
  if (error) {
    MemPtrFree(ret);
    WinPopDrawState();
    ASSERT(false);
    return NULL;
  }

  RctInsetRectangle(&bounds, 2);

  /* Create "message box" window */
  ret->flash_handle = WinCreateWindow(&bounds, boldRoundFrame,
				      false, false, &error);
  if(error) {
    WinDeleteWindow(ret->offscreen_handle, false);
    MemPtrFree(ret);
    WinPopDrawState();
    ASSERT(false);
    return NULL;
  }

  /* Draw "message box" */
  oldH = WinSetDrawWindow(ret->flash_handle);
  WinSetBackColor(UIColorGetTableEntryIndex(UIAlertFill));
  WinEraseWindow();
  WinSetForeColor(UIColorGetTableEntryIndex(UIAlertFrame));
  WinDrawWindowFrame();
  WinSetTextColor(UIColorGetTableEntryIndex(UIObjectForeground));
  WinDrawChars(message, StrLen(message), 4, 4);

  /* Clean up */
  WinSetDrawWindow(oldH);
  WinPopDrawState();

  return ret;
}

/*
** Erase the "message box" and restore the background.
*/ 
void UnFlashMessage(FlashMessageType* flashP) {
  if (!flashP) return;

  /* Ensure standard coordinates */
  WinPushDrawState();
  SwitchToLoRes();

  /* Erase "message box" */
  WinDeleteWindow(flashP->flash_handle, false);
  WinRestoreBits(flashP->offscreen_handle, flashP->x, flashP->y);

  /* Clean up */
  WinPopDrawState();
  MemPtrFree(flashP);
}

/*
** Show a "message box" for a few seconds.
*/
void FlashWaitMessage(UInt16 strID) {
  FlashMessageType* flshP = FlashMessage(strID);
  ASSERT(flshP);
  SysTaskDelay(SysTicksPerSecond() / 2);
  UnFlashMessage(flshP);
}
