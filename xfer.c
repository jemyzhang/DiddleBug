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

#include "diddlebug.h"
#include "diddlebugExtern.h"
#include "diddlebugInlines.h"
#include "globals.h"
#include "debug.h"
#include "diddlebugRsc.h"
#include "util.h"
#include "booger.h"
#include "linker.h"
#include "dynamic_buttons.h"

#include "xfer.h"

/* Prototypes for static functions, ensuring that they are in the right code section */
static void ToggleObject(FormType* frm, UInt16 objID, Boolean toggle_on) SUTIL1;
static void DrawBitmap(UInt16 bitmapID, Coord x, Coord y) SUTIL1;
static Int16 GetCurrentXferAppListIndex(void) SUTIL1;
static Int16 PlugIndex(Int16 index, Boolean atov) SUTIL1;
static void GetPlugString(Int16 plugindex, UInt16 rscID, Char* chrP) SUTIL1;
static void DrawPlugBitmap(Int16 plugindex, UInt16 rscID, Coord x, Coord y) SUTIL1;
static inline void SelectAndDrawButton(DynamicButtonType* btn, Boolean select) SUTIL1;
static void DoXferList(void) SUTIL1;
static void XferListDrawFunc(Int16 item, RectangleType* b, Char** text) SUTIL1;
static void TrackXferDone(DynamicButtonType* btn) SUTIL1;
static void XferDoneListDrawFunc(Int16 item, RectangleType* b, Char** text) SUTIL1;
static void ToggleXferBar(FormType* frm, Boolean toggle_on) SUTIL1;
static void InitXferList(void) SUTIL1;
static Char* GetLink(Char* orig_text) SUTIL1;

/* Necessary because of multi-segment constant data access */
static const RectangleType* screen = &screen_above_bar_rect;

/*
** ToggleObject
*/
static void ToggleObject(FormType* frm, UInt16 objID, Boolean toggle_on) {
  if (toggle_on)
    FrmShowObject(frm, FrmGetObjectIndex(frm, objID));
  else
    FrmHideObject(frm, FrmGetObjectIndex(frm, objID));
}

/*
** DrawBitmap
*/
static void DrawBitmap(UInt16 bitmapID, Coord x, Coord y) {
  const MemHandle bitmapH = DmGetResource(bitmapRsc, bitmapID);

  WinDrawBitmap(MemHandleLock(bitmapH), x, y);
  
  /* Clean up */
  MemHandleUnlock(bitmapH);
  DmReleaseResource(bitmapH);
}

/*
** GetCurrentXferAppListIndex - returns -1 for no current app
*/
static Int16 GetCurrentXferAppListIndex(void) {
  Int16 ret = -1;
  Int16 i = 0;
  SysDBListItemType *pluglistP = NULL;

  if (!d.xfer.pluglistH) {
    d.xfer.num_plugs = 0;
  } else {
    pluglistP = MemHandleLock(d.xfer.pluglistH);

    for (; i < d.xfer.num_plugs; i++) {
      /* Search for the currently selected plugin */
      if ((pluglistP[i].creator == p.xfer_current_plug) &&
	  (pluglistP[i].version&(IBVERSION_ORIG|IBVERSION_PICTURE))) {
	ret = i;
      }
    }

    if (ret == -1)
      p.xfer_current_plug = 0;
    
    /* Clean up */
    MemHandleUnlock(d.xfer.pluglistH);
  }
  
  return ret;
}

/*
** Convert between absolute and versioned indices.
*/
static Int16 PlugIndex(Int16 index, Boolean absoluteToVersioned) {
  SysDBListItemType *pluglistP = NULL;
  Int16 a_index = 0;
  Int16 v_index = 0;
  Int16 ret = 0;

  if (d.xfer.pluglistH) {
    pluglistP = MemHandleLock(d.xfer.pluglistH);

    for (; a_index < d.xfer.num_plugs; a_index++) {
      if ((pluglistP[a_index].version)&(IBVERSION_ORIG|IBVERSION_PICTURE)) {
	if (absoluteToVersioned && index == a_index) 
	  ret = v_index;
	
	if (!absoluteToVersioned && index == v_index)
	  ret = a_index;

	v_index++;
      }
    }

    MemHandleUnlock(d.xfer.pluglistH);

    if (!index && absoluteToVersioned)
      ret = v_index;
  }
  
  return ret;
}

/*
** GetPlugString
*/
static void GetPlugString(Int16 plugindex, UInt16 rscID, Char* chrP) {
  SysDBListItemType* pluglistP = MemHandleLock(d.xfer.pluglistH);
  DmOpenRef dbR = DmOpenDatabase(pluglistP[plugindex].cardNo,
				 pluglistP[plugindex].dbID, dmModeReadOnly);
  MemHandle plugH = DmGet1Resource('BooG', rscID);

  ASSERT(dbR);
  ASSERT(plugH);

  if (!plugH) {
    StrCopy(chrP, "");
  } else {
    StrNCopy(chrP, MemHandleLock(plugH), 48); // FIXME constant should be symbolic
    MemHandleUnlock(plugH);
  }

  /* Clean up */
  MemHandleUnlock(d.xfer.pluglistH);
  DmCloseDatabase(dbR);
}

/*
** DrawPlugBitmap
*/
static void DrawPlugBitmap(Int16 plugindex, UInt16 rscID, Coord x, Coord y) {
  SysDBListItemType *pluglistP = MemHandleLock(d.xfer.pluglistH);
  DmOpenRef dbR = DmOpenDatabase(pluglistP[plugindex].cardNo,
				 pluglistP[plugindex].dbID, dmModeReadOnly);
  MemHandle plugH = DmGet1Resource('BooG', rscID);

  ASSERT(dbR);
  ASSERT(plugH);

  WinDrawBitmap(MemHandleLock(plugH), x, y);

  /* Clean up */
  MemHandleUnlock(d.xfer.pluglistH);
  MemHandleUnlock(plugH);
  DmReleaseResource(plugH);
  DmCloseDatabase(dbR);
}

/*
** DrawXferDoneButton
*/
void DrawXferDoneButton(DynamicButtonType* btn) {
  WinHandle oldH = WinSetDrawWindow(btn->content.bmpW);
  Int16 bitmapID = 0;
  const IndexedColorType oldCol = WinSetBackColor(0); /* otherwise the pixels will not count as "off" */

  if (((p.flags&PFLAGS_XFER_GOTO) || xferGotoIsAlways) && !(xferGotoIsNever))
    bitmapID = XferDoneGotoBitmap;
  else
    bitmapID = XferDoneBitmap;
  DrawBitmap(bitmapID, 0, 0);
  
  if (p.flags&PFLAGS_XFER_DELETE)
    WinEraseLine(12, 2, 12, 10);
  if ((d.xfer.complete || xferCompleteIsAlways) && !(xferCompleteIsNever))
    WinEraseLine(10, 7, 15, 7);
  if (d.linker_available && p.flags & PFLAGS_XFER_BACKLINK)
    WinEraseLine(9, 2, 9, 10);

  /* Restore old draw window & state */
  WinSetDrawWindow(oldH);
  WinSetBackColor(oldCol);
}

/*
** DrawPlugButton
*/
void DrawPlugButton(DynamicButtonType* btn) {
  Char str[48], str2[48];
  Int16 i = -1;
  WinHandle oldH = WinSetDrawWindow(btn->content.bmpW);

  /* Set up draw state */
  WinPushDrawState();
  WinSetBackColor(0); /* otherwise the pixels will not count as "off" */

  if (d.xfer.pluglistH && ((i = GetCurrentXferAppListIndex()) != -1)) {
    BitmapType* bmp = NULL;
    BitmapTypeV3* bmpV3 = NULL;
    WinHandle tmpH = NULL;
    Coord x, y;
    UInt32 depth;
    Err err = errNone;

    /* Hi-res 1-bpp-bug-workaround, part 1 */
    if (d.hires) {
      /* Create a new temporary bitmap with screen depth */
      BmpGetDimensions(btn->bmp, &x, &y, NULL);
      WinScreenMode(winScreenModeGet, NULL, NULL, &depth, NULL);
      bmp = BmpCreate(x, y, depth, NULL, &err);
      if (err) abort();

      /* ...make it a double density bitmap */
      bmpV3 = BmpCreateBitmapV3(bmp, kDensityDouble, BmpGetBits(bmp), NULL);

      /* ...and create a temporary window */
      tmpH = WinCreateBitmapWindow((BitmapType*) bmpV3, &err);
      if (err) abort();

      /* Draw to temporary window for the time being... */
      WinSetDrawWindow(tmpH);

      /* ...and set the right foreground color */
      WinSetForeColor(WinRGBToIndex(&black));
    }

    /* Get the plugin icon and draw it */
    DrawPlugBitmap(i, boogerID_button, 0, 0);

    /* Hi-res 1-bpp-bug-workaround, part 2 */
    if (d.hires) {
      /* Copy the icon to the actual button */
      WinSetDrawWindow(btn->content.bmpW);
      WinPaintBitmap(WinGetBitmap(tmpH), 0, 0);

      /* Clean up */
      if (tmpH) WinDeleteWindow(tmpH, false);
      if (bmpV3) BmpDelete((BitmapType*) bmpV3);
      if (bmp) BmpDelete(bmp);
    }

    /* Load "goto" behavior */
    GetPlugString(i, boogerID_gotobehavior, str);
    if (StrChr(str, 'n')) 
      xferGotoSetNever();
    else if (StrChr(str, 'a')) 
      xferGotoSetAlways();
    else
      xferGotoClear();

    /* Load "completion" behaviour ("completion" is never on by default (yet)) */
    GetPlugString(i, boogerID_completebehavior, str2);
    if (StrChr(str2, 'a')) 
      xferCompleteSetAlways();
    else if (StrChr(str2, 'o')) 
      xferCompleteClear();
    else
      xferCompleteSetNever();
  } else {
    DrawBitmap(XferNoAppBitmap, 0, 0);
    xferGotoClear();
    xferCompleteClear();
  }

  /* Restore old draw window & state */
  WinPopDrawState();
  WinSetDrawWindow(oldH);
}

/*
** ToggleButtonBar - disable or enable the objects in the button bar
*/
void ToggleButtonBar(FormType* frm, Boolean toggle_on) {
  UInt16 i, skip1, skip2;

  if (recordIsLocked) {
    skip1 = MainClearButton; 
    skip2 = MainDeleteButton;
  } else {
    skip1 = skip2 = MainLockButton;
  }

  for (i = BUTTONBAR_START; i <= BUTTONBAR_END; i++) {
    if (toggle_on && (skip1 == i || skip2 == i)) 
      continue;

    ToggleObject(frm, i, toggle_on);
  }
}

/*
** ToggleXferBar - disable or enable the objects in the transfer bar
*/
static void ToggleXferBar(FormType* frm, Boolean toggle_on) {
  ToggleObject(frm, XferDoneButton, toggle_on);
  ToggleObject(frm, XferDetailsButton, toggle_on);
  ToggleObject(frm, XferField, toggle_on);

  /* Set the insertion point in the field if toggling on */
  if(!toggle_on)
    FldFreeMemory(GetObjectPointer(frm, XferField));
}

/*
** Initializes the transfer group structures.
*/
static void InitXferList(void) {
  SysDBListItemType* pluglistP = NULL;
  Char str[48];
  FormType* frm = FrmGetActiveForm();
  Int16 i = 0;
  Int16 width = 0;
  Int16 n = 0;

  /* Return if we've already initialized the plugin list */
  if (d.xfer.pluglistH) 
    return;

  /* Return if there are no plugins */
  if (!SysCreateDataBaseList('BooG', 0, &d.xfer.num_plugs,
			     &d.xfer.pluglistH, true)) {
    /* No databases found */
    d.xfer.pluglistH = NULL;
    d.xfer.num_plugs = 0;
    return;
  }

  /* This will unset the p.xfer_current_plug if it has been deleted */
  GetCurrentXferAppListIndex();
  /* Update the plug button */
  DrawPlugButton((DynamicButtonType*)FrmGetGadgetData(frm, FrmGetObjectIndex(frm, XferDetailsButton)));
  /* Update XferDone button */
  DrawXferDoneButton((DynamicButtonType*)FrmGetGadgetData(frm, FrmGetObjectIndex(frm, XferDoneButton)));

  /* Find the menu width and set it */
  pluglistP = MemHandleLock(d.xfer.pluglistH);
  for (; i < d.xfer.num_plugs; i++) {
    GetPlugString(i, boogerID_plugname, str);
    n = FntCharsWidth(str, StrLen(str));

    if ((pluglistP[i].version) & (IBVERSION_ORIG|IBVERSION_PICTURE))
      width = Max(width, n);
  }

  d.xfer.plug_menu_width = width + 25;

  /* Clean up */
  MemHandleUnlock(d.xfer.pluglistH);
}

/*
** Draw transfer list.
*/
static void XferListDrawFunc(Int16 item, RectangleType* b, Char** itemsText) {
  const Int16 absolute_item = PlugIndex(item, false);
  Char str[48];
  
  DrawPlugBitmap(absolute_item, boogerID_lilbutton, 
		 b->topLeft.x, b->topLeft.y + 1);
  GetPlugString(absolute_item, boogerID_plugname, str);
  WinDrawChars(str, StrLen(str), b->topLeft.x + 19, b->topLeft.y);
}

/*
** Popup the transfer details list.
*/
static void DoXferList(void) {
  const Int16 num_plugs = PlugIndex(NULL, true);

  if (!d.xfer.pluglistH || !num_plugs) {
    FrmAlert(NoPluginInstalled);
  } else {
    RectangleType rect;
    Int16 list_selection = -1;
    FormType* frm = FrmGetActiveForm();
    ListPtr list = GetObjectPointer(frm, XferList);

    /* Set up plugin list control */
    LstSetDrawFunction(list, XferListDrawFunc);
    LstSetListChoices(list, NULL, num_plugs);
    LstSetHeight(list, Min(num_plugs, 10));
    FrmGetObjectBounds(frm, FrmGetObjectIndex(frm, XferList), &rect);
    rect.topLeft.x = 16;
    rect.topLeft.y = 144 - rect.extent.y;
    rect.extent.x = d.xfer.plug_menu_width;
    FrmSetObjectBounds(frm, FrmGetObjectIndex(frm, XferList), &rect);

    /* Pop up the list */
    LstSetSelection(list, GetCurrentXferAppListIndex());
    list_selection = LstPopupList(list);

    if (list_selection != -1) {
      SysDBListItemType* pluglistP = MemHandleLock(d.xfer.pluglistH);
      
      /* Store the current plugin */
      p.xfer_current_plug = pluglistP[PlugIndex(list_selection, false)].creator;

      /* Clean up */
      MemHandleUnlock(d.xfer.pluglistH);
    }
  }
}

/*
** Initiates transfer mode.
*/
void StartXferMode(void) {
  FormType* frm = FrmGetActiveForm();
  FieldType* fld =  GetObjectPointer(frm, XferField);
  const UInt32 len = MemHandleSize(d.record_name);
  MemHandle textH = MemHandleNew(len);

  ASSERT(textH);

  FlushToBuffer();
  FrmSetMenu(frm, XferMenu);
  d.is_xfer_mode = true;
  ToggleButtonBar(frm, false);
  ResetDrawingAreaRectangle(p.formID == DiddleTForm, true);
  FrmUpdateForm(p.formID, 0);
  InitXferList();
  ToggleXferBar(frm, true);
  FrmSetFocus(frm, FrmGetObjectIndex(frm, XferField));

  /* Init field with record title */
  MemMove(MemHandleLock(textH), MemHandleLock(d.record_name), len);
  MemHandleUnlock(textH);
  MemHandleUnlock(d.record_name);
  FldSetTextHandle(fld, textH);
}

/*
** Cancels the transfer mode of operation.
*/
void CancelXferMode(void) {
  FormType* frm = NULL;

  if (p.formID == DiddleTForm)
    frm = FrmGetFormPtr(DiddleTForm);
  else if (p.formID == DiddleForm)
    frm = FrmGetFormPtr(DiddleForm);

  if (frm) {
    FrmSetMenu(frm, DiddleMenu);
    d.is_xfer_mode = false;
    d.drawmode = dbModeDoodle;
    ToggleXferBar(frm, false);

    if (!(p.flags & PFLAGS_WITHOUT_TOOLBAR))
      ToggleButtonBar(frm, true);
    else {
      ResetDrawingAreaRectangle(p.formID == DiddleTForm, false);
      MirrorRealCanvas();
    }

    LabelAlarm();
  }
}

/*
** Add the Linker link to the plugin text. The calling
** function must free the returned pointer.
*/
static Char* GetLink(Char* orig_text) {
  UInt16 length = 0;
  Char* linked_text = NULL;
  LinkerCPB command_block;
  UInt32 result;

  /* Bail if there is no text */
  if (!orig_text)
    return 0;

  length = StrLen(orig_text) + 1;
  if (p.flags & PFLAGS_XFER_BACKLINK && d.linker_available) 
    length += 5;

  linked_text = MemPtrNew(length);
  ASSERT(linked_text);
  StrCopy(linked_text, orig_text);

  if (!(p.flags & PFLAGS_XFER_BACKLINK && d.linker_available))
    return linked_text;

  MemSet(&command_block, sizeof(LinkerCPB), 0);
  command_block.creator = 'Linx'; /* maybe this is overkill, but .... */
  command_block.message = LinkerNewLinkMessage;
  /* command_block.linkTag is empty and to be filled by Linker */
  command_block.gotoTargetAppCreator = AppType; /* DiddleBug is the target */
  DmOpenDatabaseInfo(d.dbR, &(command_block.gotoLinkParams.dbID), NULL, NULL,
		     &(command_block.gotoLinkParams.dbCardNo), NULL);
  command_block.gotoLinkParams.recordNum = p.dbI;
  
  /* Signal Linker */
  SysAppLaunch(d.cardNo, d.dbID, 0, linkerAppLaunchCmdSignalLinker,
	       &command_block, &result);

  /* Add the link text */
  command_block.linkTag[5] = 0x00; /* just to be sure */
  StrCat(linked_text, command_block.linkTag);

  return linked_text;
}

/*
** FinishXferMode
*/
void FinishXferMode(void) {
  KleenexType kleenex;
  DtbkRepeatInfoType repeat;
  FormType* frm = FrmGetActiveForm();
  SysDBListItemType* pluglistP = NULL;
  Int16 current_plug = -1;
  UInt32 result;
  Boolean is_goto = false;
  Err err = errNone;
  Char cat_name[dmCategoryLength];
  Char* note = NULL;
  UInt16 attr;

  /* Zero the kleenex */
  MemSet(&kleenex, sizeof(KleenexType), 0);

  /* Bail if no plugins installed */
  if (!d.xfer.pluglistH) {
    FrmAlert(NoPluginInstalled);
    return;
  }

  /* Lock the plugin list */
  pluglistP = MemHandleLock(d.xfer.pluglistH);
  current_plug = GetCurrentXferAppListIndex();
  if (current_plug == -1) {
    FrmAlert(NoPluginSelected);
    MemHandleUnlock(d.xfer.pluglistH);
    return;
  }

  /* Set the version */
  if (pluglistP[current_plug].version & IBVERSION_PICTURE)
    kleenex.version = IBVERSION_PICTURE;
  else if (pluglistP[current_plug].version & IBVERSION_ORIG)
    kleenex.version = IBVERSION_ORIG;
  else
    abort();
  /* Add flag notifier */
  kleenex.version |= IBVERSION_FLAGS;

  /* Set flags */
  if (d.hires) 
    kleenex.flags = IBFLAG_HIRES;
  
  /* Set the current record index */
  kleenex.sketchRecordNum = p.dbI;

  /* Set the pick text */
  kleenex.text = GetLink(FldGetTextPtr(GetObjectPointer(frm, XferField)));
  if (!kleenex.text) {
    FlashWaitMessage(XferNoTextString);
    MemHandleUnlock(d.xfer.pluglistH);
    return;
  }
  
  /* Set the title text */
  kleenex.title = MemHandleLock(d.record_name);

  /* Set the category text */
  DmRecordInfo(d.dbR, p.dbI, &attr, NULL, NULL);
  CategoryGetName(d.dbR, attr & dmRecAttrCategoryMask, cat_name);
  kleenex.category = cat_name;

  /* Set the note */
  note = MemHandleLock(d.record_note);
  if (StrLen(note))
    kleenex.note = note;

  /* Set the seconds for the alarm */
  if (recordIsAlarmSet) 
    kleenex.alarm_secs = d.record.alarmSecs;

  /* Set the repeat information */
  kleenex.repeat = &repeat;
  switch (d.record.repeatInfo.repeatType) {
  case repeatDaily:
  case repeatWeekly:
  case repeatMonthlyByDay:
  case repeatMonthlyByDate:
  case repeatYearly:
    repeat.repeatType = d.record.repeatInfo.repeatType - 1; /* fix offset caused by repeatHourly */
    repeat.repeatFrequency = d.record.repeatInfo.repeatFrequency;
    repeat.repeatEndDate = d.record.repeatInfo.repeatEndDate;
    repeat.repeatOn = d.record.repeatInfo.repeatOn;
    repeat.repeatStartOfWeek = d.record.repeatInfo.repeatStartOfWeek;
    break;
  case repeatNone:
  case repeatHourly:
  default:
    kleenex.repeat = NULL;
  }

  /* Set the priority */
  kleenex.priority = d.record.priority + 1; /* DiddleBug priority is zero-based */

  /* Set the completion flag */
  if ((d.xfer.complete || (xferCompleteIsAlways)) && !(xferCompleteIsNever)) 
    kleenex.is_complete = 1;
  else
    kleenex.is_complete = 0;

  /* Load the sketch data */
  if (kleenex.version & IBVERSION_PICTURE) {
      kleenex.data = BmpGetBits(WinGetBitmap(d.winbufM));
      kleenex.data_size = BmpBitsSize(WinGetBitmap(d.winbufM));
  }

  /* Call the plugin */
  err = SysAppLaunch(pluglistP[current_plug].cardNo,
		     pluglistP[current_plug].dbID, 0,
		     boogerPlugLaunchCmdBlowNose, (MemPtr)&kleenex, &result);

  /* Clean up some stuff */
  MemHandleUnlock(d.xfer.pluglistH);
  MemPtrFree(kleenex.text);
  if (err || result) {
    FrmAlert(PluginError);
    return;
  }

  /* Just to save some typing */
  is_goto = (((p.flags&PFLAGS_XFER_GOTO) || (xferGotoIsAlways)) &&
	     !(xferGotoIsNever));

  if (!is_goto)
    FlashWaitMessage(XferSavingString);
  
  /* Clean up some more */
  MemHandleUnlock(d.record_name);
  MemHandleUnlock(d.record_note);

  /* Delete the sketch if selected */
  if (p.flags&PFLAGS_XFER_DELETE)
    DoCmdRemove();

  /* Goto the new sketch if required */
  if (is_goto) {
    /* Clean up */
    MemHandleFree(d.xfer.pluglistH);
    d.xfer.pluglistH = NULL;
    
    /* Switch to other application */
    err = SysUIAppSwitch(kleenex.booger.cardNo, kleenex.booger.dbID,
			 kleenex.booger.cmd, kleenex.booger.cmdPBP);
    
    /* Clean up on error */
    if (err && kleenex.booger.cmdPBP) 
      MemPtrFree(kleenex.booger.cmdPBP);
  } else {
    if (kleenex.booger.cmdPBP) 
      MemPtrFree(kleenex.booger.cmdPBP);
  }

  CancelXferMode();
}

extern inline void SelectAndDrawButton(DynamicButtonType* btn, Boolean select) {
  Boolean oldSelect = btn->selected;
  btn->selected = select;

  if (oldSelect != select)
    DynBtnDraw(btn);
}

/*
** Draw transfer option list.
*/
#define TRACKXFERDONE_CHECKED 0x01
#define TRACKXFERDONE_NEVER   0x02
#define TRACKXFERDONE_ALWAYS  0x04
static void XferDoneListDrawFunc(Int16 item, RectangleType* b, Char** itemsText) {
  Char str[48];
  const FontID old_font = FntSetFont(symbol11Font);

  if (d.xfer.status[d.xfer.choice_map[item]] & TRACKXFERDONE_ALWAYS) {
    FntSetFont(symbolFont);
    WinDrawChar('\23', b->topLeft.x + 3, b->topLeft.y); /* diamond */
  } else if (d.xfer.status[d.xfer.choice_map[item]] & TRACKXFERDONE_CHECKED) {
    WinDrawChar('\01', b->topLeft.x, b->topLeft.y); /* completed checkbox */
  } else {
    WinDrawChar('\00', b->topLeft.x, b->topLeft.y); /* empty checkbox */
  }

  FntSetFont(old_font);
  SysCopyStringResource(str, XferMenuOptionsStrings + d.xfer.choice_map[item]);

  /* Draw the text label on the right pos (* 1.5 for HandEra) */
  WinDrawChars(str, StrLen(str), b->topLeft.x + 15, b->topLeft.y);
}

/*
** TrackXferDone
*/
static void TrackXferDone(DynamicButtonType* btn) {
  RectangleType bounds[5], frame, popFrame, popShadowFrame;
  Boolean penDown, on_button;
  Int16 x, y, clicked_on = 0;
  Int16 state = 1;
  Int16 i = 0;
  WinHandle offscreenH = NULL;
  FormType* frm = FrmGetActiveForm();
  const UInt16 listIdx = FrmGetObjectIndex(frm, XferDoneList);
  ListType* list = FrmGetObjectPtr(frm, listIdx);
  UInt16 width = 0, choices = 0;
  Err err = errNone;
  Char str[48];
  Int16 n = 0;

  /* Save the old drawing context */
  WinPushDrawState();

  /* This should match the XferDoneButton bounds */
  FrmGetObjectBounds(frm, FrmGetObjectIndex(frm, btn->id), &bounds[0]);

  /* Invert the done button */
  SelectAndDrawButton(btn, true);
  SndPlaySystemSound(sndClick);

  /* Set the status of each menu pick */
  for (; i < 4; i++)
    d.xfer.status[i] = 0x00;
  
  if (xferGotoIsAlways)
    d.xfer.status[0] = TRACKXFERDONE_ALWAYS;
  else if (xferGotoIsNever)
    d.xfer.status[0] = TRACKXFERDONE_NEVER;
  else if (p.flags&PFLAGS_XFER_GOTO)
    d.xfer.status[0] = TRACKXFERDONE_CHECKED;

  if (xferCompleteIsAlways)
    d.xfer.status[1] = TRACKXFERDONE_ALWAYS;
  else if (xferCompleteIsNever)
    d.xfer.status[1] = TRACKXFERDONE_NEVER;
  else if (d.xfer.complete)
    d.xfer.status[1] = TRACKXFERDONE_CHECKED;

  if (!d.linker_available)
    d.xfer.status[2] = TRACKXFERDONE_NEVER;
  else if (p.flags&PFLAGS_XFER_BACKLINK)
    d.xfer.status[2] = TRACKXFERDONE_CHECKED;
  if (p.flags&PFLAGS_XFER_DELETE)
    d.xfer.status[3] = TRACKXFERDONE_CHECKED;

  for (i = 0; i < 4; ++i) {
    if (!(d.xfer.status[i] & TRACKXFERDONE_NEVER)) {
      d.xfer.choice_map[choices] = i;
      ++choices;

      /* calculate list width */
      SysCopyStringResource(str, XferMenuOptionsStrings + i);
      n = FntCharsWidth(str, StrLen(str));
      width = Max(width, n);
    }
  }

  LstSetDrawFunction(list, XferDoneListDrawFunc);
  LstSetListChoices(list, 0, choices);
  LstSetHeight(list, Min(choices, 10));
  FrmGetObjectBounds(frm, listIdx, &frame);
  frame.topLeft.y = 144 - frame.extent.y - 1; /* -1 to compensate for white border */
  frame.extent.x = width + 15 + 6;
  FrmSetObjectBounds(frm, listIdx, &frame);
  WinGetFramesRectangle(popupFrame, &frame, &popFrame);
  WinGetFramesRectangle(rectangleFrame, &popFrame, &popShadowFrame);

  for (i = 0; i < choices; ++i)
    RctSetRectangle(&bounds[i+1], frame.topLeft.x, frame.topLeft.y + 10 * i, 
		    frame.extent.x, 10);

  /* Save the bits of the whole menu */
  offscreenH = WinSaveBits(&popShadowFrame, &err);
  if (err) abort();

  /* None selected */
  LstSetSelection(list, -1);

  FrmShowObject(frm, listIdx);
  WinEraseRectangle(&popShadowFrame, 0);
  LstDrawList(list);
  WinEraseRectangleFrame(rectangleFrame, &frame);
  WinDrawRectangleFrame(popupFrame, &frame);

  do {
    EvtGetPen(&x, &y, &penDown);
    if (!state || !RctPtInRectangle(x, y, &bounds[state-1])) {
      on_button = false;
      for (i = 1; i <= choices + 1; i++) {
	if ((state != i) && RctPtInRectangle(x, y, &bounds[i-1])) {
	  /* Invert the new state */
	  LstSetSelection(list, i - 2);
	  LstDrawList(list);
	  WinEraseRectangleFrame(rectangleFrame, &frame);
	  WinDrawRectangleFrame(popupFrame, &frame);
	  
	  SelectAndDrawButton(btn, i == 1);
	  
	  state = i;
	  on_button = true;
	}
      }

      if (state && !on_button) {
	/* Moved off the current button */
	LstSetSelection(list, -1);
	LstDrawList(list);
	WinEraseRectangleFrame(rectangleFrame, &frame);
	WinDrawRectangleFrame(popupFrame, &frame);

	if (state == 1) 
	  SelectAndDrawButton(btn, false);
	state = 0;
      }
    }
  } while (penDown);

  FrmHideObject(frm, listIdx);
  LstEraseList(list);

  /* Restore the framed rect */
  WinRestoreBits(offscreenH, popShadowFrame.topLeft.x, popShadowFrame.topLeft.y);

  /* Unselect the button */
  SelectAndDrawButton(btn, false);

  /* Finish up if we just tapped the button */
  if (RctPtInRectangle(x, y, &bounds[0])) {
    FinishXferMode();
    
    /* Restore the old draw state */
    WinPopDrawState();
    return;
  }

  /* Change the setting for goto or delete */
  for (i = 1; i <= choices; i++) {
    if (RctPtInRectangle(x, y, &bounds[i])) {
      clicked_on = i;
      break;
    }
  }

  if (clicked_on) 
    clicked_on = d.xfer.choice_map[clicked_on-1] + 1;

  switch (clicked_on) {
  case 1: /* Goto */
    if (d.xfer.status[0] & TRACKXFERDONE_CHECKED)
      p.flags &= ~PFLAGS_XFER_GOTO;
    else if (!d.xfer.status[0])
      p.flags |= PFLAGS_XFER_GOTO;
    break;
  case 2: /* Complete */
    if (d.xfer.status[1] & TRACKXFERDONE_CHECKED)
      d.xfer.complete = false;
    else if (!d.xfer.status[1])
      d.xfer.complete = true;
    break;
  case 3: /* BackLink */
    if (d.xfer.status[2] & TRACKXFERDONE_CHECKED)
      p.flags &= ~PFLAGS_XFER_BACKLINK;
    else if (!d.xfer.status[2]) {
      p.flags |= PFLAGS_XFER_BACKLINK;
      p.flags &= ~PFLAGS_XFER_DELETE; /* No delete if backlink */
    }
    break;
  case 4: /* Delete */
    if (d.xfer.status[3] & TRACKXFERDONE_CHECKED)
      p.flags &= ~PFLAGS_XFER_DELETE;
    else if (!d.xfer.status[3]) {
      p.flags |= PFLAGS_XFER_DELETE;
      p.flags &= ~PFLAGS_XFER_BACKLINK; /* No backlink if delete */
    }
    break;
  }

  /* Click and redraw the button */
  if (clicked_on) {
    DrawXferDoneButton(btn);
    DynBtnDraw(btn);
    
    if (d.xfer.status[clicked_on-1] & TRACKXFERDONE_ALWAYS)
      SndPlaySystemSound(sndWarning);
    else
      SndPlaySystemSound(sndClick);
  }

  /* Restore the old draw state */
  WinPopDrawState();
}

/*
** Track pen and do the appropriate thing.
*/
Boolean XferPenDown(EventPtr e) {
  Boolean handled = false;

  if (RctPtInRectangle(e->screenX, e->screenY, screen)	/* This cancels the xfer mode */
      && !FrmPointInTitle(FrmGetActiveForm(), e->screenX, e->screenY)) {/* unless we should just popdown the menu */
    SndPlaySystemSound(sndWarning);
    CancelXferMode();
    handled = true;
  }

  return handled;
}

/*
** Handles events for my custom gadgets (the transfer details button in this case).
*/
Boolean XferDetailsGadgetEvent(FormGadgetTypeInCallback* gadgetP, 
			       UInt16 cmd, void* paramP) {
  Boolean handled = false;
  FormType* frm = FrmGetActiveForm();
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
    {
      EventType* e = (EventType*) paramP;
      if (e->eType == frmGadgetEnterEvent) {
	if (DynBtnTrackPen(btn)) {
	  DynamicButtonType* btn2 = FrmGetGadgetData(frm, FrmGetObjectIndex(frm, XferDoneButton));
	  
	  DoXferList();
	  DrawPlugButton(btn);

	  /* Update XferDone button */
	  DrawXferDoneButton(btn2);
	  DynBtnDraw(btn2);
	}

	DynBtnDraw(btn);
	handled = true;
      }
    }
    break;
  }

  return handled;
}

/*
** Handles events for my custom gadgets (the transfer done button in this case).
*/
Boolean XferDoneGadgetEvent(FormGadgetTypeInCallback* gadgetP, UInt16 cmd, void* paramP) {
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
    gadgetP->attr.usable = false; /* Workaround for PalmOS >= 5.0 bug */
    break;

  case formGadgetHandleEventCmd:
    {
      EventType* e = (EventType*) paramP;
      if (e->eType == frmGadgetEnterEvent) {
	TrackXferDone(btn);
	handled = true;
      }
    }
    break;
  }

  return handled;
}
