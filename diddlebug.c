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
#include "diddlebugInlines.h"
#include "diddlebugRsc.h"
#include "globals.h"
#include "debug.h"
#include "util.h"
#include "xfer.h"
#include "shape.h"
#include "text.h"
#include "clipboard.h"
#include "details.h"
#include "dynamic_buttons.h"
#include "repeat.h"
#include "notes.h"
#include "listview.h"
#include "thumbnails.h"
#include "thumbnails_details.h"
#include "alarm.h"
#include "prefs.h"
#include "time.h"
#include "snappermail.h"

#include <PalmOSGlue.h>

/* Miscellaneous hardware information */
#define NON_PORTABLE
#include <Core/Hardware/HwrMiscFlags.h>
#undef NON_PORTABLE

/* Sony Clie */
#include <SonyHRLib.h>
#include <SonyHwrOEMIDs.h>
#include <SonyChars.h>
#include <SonySystemFtr.h>

/* Acer s50 and S60 */
#include <AcerHwrOEMIDs.h>

/* palmOne */
#include <palmOneNavigator.h>
#include <PalmResources.h>

/* Handspring/palmOne Treo */
#include <HsExt.h>
#include <HsKeyCodes.h>

/* forward declaration for beaming other stuff */
static void BeamSend(void) SUTIL2;
static Boolean ShapeEvent(EventPtr e) SUTIL1;
static void DialogButtonDrop(UInt16 ctlID) SUTIL2;
static void ChangeGeneralColor(void) SUTIL1;
static void LoadPen(Int16 selection) SUTIL1;
static void LoadInk(Int16 selection) SUTIL1;
static void SetPaintMode(Int16 mode, Int16 penSel, Int16 filtSel) SUTIL2;
static void LoadInkNoUpdate(Int16 selection) SUTIL1;
static Int16 GetListWidth(ListPtr) SUTIL1;
static UInt16 GetObjectIndexFromPointer(FormPtr, void*) SUTIL1;
static void GetFormattedDate(DateTimeType*, Char*, Boolean) SUTIL1;
static Boolean ModalEvent(EventPtr e) SUTIL2;
static void PriorModal(UInt16 formID) SUTIL2;
static void DoTextPopup(void) SUTIL2;
static void UpgradeRecordFormat(void) SUTIL3;
static Err OpenDatabases(void) SUTIL2;
static void DoDateSelect(Int16 x) SUTIL3;
static void DrawPenLabel(const UInt8 *pen, Int16 x, Int16 y) SUTIL2;
static void ClearImage(void) SUTIL3;
static void FillImage(void) SUTIL3;
static void ChangeIndexInCategory(UInt16 amount, Boolean forward) SUTIL2;
static Boolean DoPageList(void) SUTIL2;
static void NewScratchImage(void) SUTIL1;
static void LoadFromBuffer(void) SUTIL2;
static void SearchDraw (DiddleBugRecordType* rec, Char* record_name, Int16 x, Int16 y, Int16 width);
static Boolean DiddleMenuEvent(EventType* e) SUTIL2;
static Boolean DiddleSelectEvent(EventPtr e) SUTIL3;
static Boolean SpecialKeyDown(EventPtr e) SUTIL1;
static Boolean HandleHardwareButton(Int8 index) SUTIL1;
static void UpgradeRecordFormat2_15(void) SUTIL3;
static MemPtr FlushExtraData(void) SUTIL1;
static void PageListDrawFunc(Int16 item, RectangleType* b, Char** text) SUTIL2;
static void DrawPenLabelDynamic(WinHandle, UInt16, UInt16, UInt16, const UInt8*) SUTIL2;
static void DoCmdSecurity(void) SUTIL1;
static void DrawInkLabelDynamic(WinHandle, UInt16, UInt16, UInt16, const CustomPatternType*) SUTIL1;
static void RemoveImage(void) SUTIL3;
static void UpdateLockButton() SUTIL1;
static void DrawBitmapLabel(UInt16 bitmapID, Int16 x, Int16 y) SUTIL1;
static void LabelTitle(void) SUTIL1;
static Boolean SketchIsEmpty(MemHandle sketchHandle) SUTIL1;
static void ShortCutPop(ShortCutLocationType loc) SUTIL1;
static void ScrollXferField(WinDirectionType direction) SUTIL1;
static void SetFilterMode(Int16 mode) SUTIL1;
static Boolean CheckPageButtonDrag(PointType* orig, Int16 curX, Int16 curY) SUTIL3;
static void EventLoop(void) SUTIL1;
static Boolean DiddleEvent(EventPtr e) SUTIL1;
static Boolean LockAsk(void) SUTIL1;
static void LockWarn(void) SUTIL3;
static Boolean ShapeGadgetEvent(FormGadgetTypeInCallback* gadgetP, UInt16 cmd, void* paramP) SUTIL3;
static Boolean GadgetEvent(FormGadgetTypeInCallback* gadgetP, UInt16 cmd, void* paramP) SUTIL3;
static Boolean DoPenDown(EventPtr e) SUTIL2;
static void InvertImage(void) SUTIL3;
static void ToggleToolbar(Boolean on) SUTIL3;
static void StopApplication(void) SUTIL3;
static void SwitchBetweenViews(void) SUTIL3;
static Err RomVersionCompatible(UInt32 requiredVersion, UInt16 launchFlags);
static void SetUpMenu(void) SUTIL3;
static void UpgradeRecordFormatHiRes(void) SUTIL3;
static void CreateBitmaps(void) SUTIL1;
static UInt16 SwitchToHiRes(void) SUTIL2;
static void UpdateDateTimeStamp(void) SUTIL2;
static Boolean CheckForAcerS60(void) SUTIL3;
static Boolean CommandBar(EventPtr e) SUTIL2;
static Int32 TimeUntilNextTimePeriod(void) SUTIL2;
static void CloseDatabases(void) SUTIL3;
static void NewSketch(void) SUTIL3;
static void UpgradeRepeat(Int16 oldRepeatMode, DiddleBugRecordType* rec) SUTIL3;
static UInt16 SwitchToPreviousCoordinates(UInt16 oldCoordinateSystem) SUTIL2;
static void MIDIListDrawFunc(Int16 item, RectangleType* b, Char** text) SUTIL3;
static void NoMIDIListDrawFunc(Int16 item, RectangleType* b, Char** text) SUTIL3;
static void FixInkAndPaperColors(void) SUTIL2;
static void DoDeviceSetup(void) SUTIL2;
static Boolean CheckForPalmOne(void) SUTIL3;
static Boolean UnlockMaskedRecords(void) SUTIL3;
static Boolean TriggerGadgetEvent(FormGadgetTypeInCallback* gadgetP, 
                  UInt16 cmd, void* paramP) SUTIL3;
static void TrackTriggerPen(DynamicButtonType* btn, EventType* e) SUTIL3;
static Boolean CanRectangleBeDrawn(const RectangleType* shapeR,
                   const RectangleType* drawingAreaR) SUTIL3;
static void CountdownDrawFunc(Int16 item, RectangleType* b, Char** text) SUTIL3;
static Int16 GetAlarmSelectListWidth(void) SUTIL3;
static void CountdownStuff(UInt16 listnum, Boolean action, Char* text) SUTIL3;
static void EndStroke(void) SUTIL4;
static void DoStroke(Coord x, Coord y) SUTIL4;
static void DiddlePenEvent(Coord nx, Coord ny) SUTIL4;

/* Globals */

data d; /* initialized to zero */
pref p; /* real initialization done by GetPreferences */
extended_pref ep; /* real initialization done by GetExtendedPreferences */

/* Drop lists */
static const UInt16 smoothDropList[4] = { cmdRough, cmdSmooth, cmdSmoother, 0 };
static const UInt16 penDropList[8] =    { penFine, penMedium, penBold, penBroad,
                      penFineItalic, penMediumItalic, penBroadItalic, 0 };
static const UInt16 inkDropList[10] =   { inkWhite, inkDith12, inkDith25, inkDith37, inkDith50,
                      inkDith62, inkDith75, inkDith87, inkBlack, 0 };

/*
** Default penpixel set.
*/
static const UInt8 penpixFine[] =         { 0x88, 0 };
static const UInt8 penpixMedium[] =       { 0x88, 0x98, 0x89, 0x99, 0 };
static const UInt8 penpixBold[] =         { 0x77, 0x87, 0x97, 0x78, 0x98, 0x79, 0x89, 0x99, 0 };
static const UInt8 penpixBroad[] =        { 0x66, 0x76, 0x86, 0x96, 0xA6, 0x67, 0xA7, 0X68,
                        0xA8, 0x69, 0xA9, 0x6A, 0x7A, 0x8A, 0x9A, 0xAA, 0 };
static const UInt8 penpixFineItalic[] =   { 0x78, 0x88, 0x87, 0x97, 0 };
static const UInt8 penpixMediumItalic[] = { 0x69, 0x79, 0x78, 0x78, 0x88, 0x87, 0x97, 0x96, 0xA6, 0 };
static const UInt8 penpixBroadItalic[] =  { 0x5A, 0x6A, 0x69, 0x79, 0x78, 0x88, 0x87, 0x97, 0x96,
                        0xA6, 0xA5, 0xB5, 0 };

static const UInt8 penpixVeryBroad[] =    { 0x44, 0x54, 0x64, 0x74, 0x84, 0x94, 0xA4, 0xB4, 0xC4,
                        0x45,                                           0xC5,
                        0x46,                                           0xC6,
                        0x47,                                           0xC7,
                        0x48,                                           0xC8,
                        0x49,                                           0xC9, 
                        0x4A,                                           0xCA,
                        0x4B,                                           0xCB,
                        0x4C, 0x5C, 0x6C, 0x7C, 0x8C, 0x9C, 0xAC, 0xBC, 0xCC,
                        0 };
static const UInt8 penpixVeryBroadItalic[] = { 0x4B, 0x5B, 0x5A, 0x6A, 0x69, 0x79, 0x78, 0x88, 0x87,
                           0x97, 0x96, 0xA6, 0xA5, 0xB5, 0xB4, 0xC4, 0 };

/*
** Default ink pattern set.
*/
static const CustomPatternType patternWhite =  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const CustomPatternType patternDith12 = { 0x11, 0x00, 0x44, 0x00, 0x11, 0x00, 0x44, 0x00 };
static const CustomPatternType patternDith25 = { 0x22, 0x11, 0x44, 0x88, 0x22, 0x11, 0x44, 0x88 };
static const CustomPatternType patternDith37 = { 0xc8, 0x4c, 0x23, 0x31, 0x8c, 0xc4, 0x32, 0x13 };
static const CustomPatternType patternDith50 = { 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55 };
static const CustomPatternType patternDith62 = { 0x37, 0xb3, 0xdc, 0xce, 0x73, 0x3b, 0xcd, 0xec };
static const CustomPatternType patternDith75 = { 0xdd, 0xee, 0xbb, 0x77, 0xdd, 0xee, 0xbb, 0x77 };
static const CustomPatternType patternDith87 = { 0xee, 0xff, 0xbb, 0xff, 0xee, 0xff, 0xbb, 0xff };
static const CustomPatternType patternBlack =  { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

/* Mask pattern */
const CustomPatternType* maskPattern = &patternDith50;

/* Some important screen areas */
const RectangleType bottom_bar_rect =       { {0, 146}, {160, 14} };
const RectangleType title_bar_rect =        { {0, 0}, {160, 16} };
const RectangleType screen_above_bar_rect = { {0, 0}, {160, 146} };
static const RectangleType screen_rect =    { {0, 0}, {160, 160} };

/* Alarm sound initialization */
static const AlarmSoundInfoType defaultAlarm = { 0x00, -1 };

/* Some colors */
static const RGBColorType darkGrey = { 0, 0x55, 0x55, 0x55 };
static const RGBColorType middleGrey = { 0, 0x77, 0x77, 0x77 };
const RGBColorType black = { 0, 0, 0, 0 };

/*
** Signal a database error to the user and release the 
** corresponding record if necessary.
*/
void DatabaseError(DmOpenRef dbR, UInt16 dbI) {
  const UInt16 oldDbI = dbI;
  const Err err = DmGetLastErr();
  
  if (DmSeekRecordInCategory(dbR, &dbI, 0, dmSeekForward, dmAllCategories) == errNone &&
      dbI == oldDbI) {
    UInt16 attr = 0;
    
    if (DmRecordInfo(dbR, dbI, &attr, NULL, NULL) == errNone && attr & dmRecAttrBusy) {
      PRINT("Error %hx. Releasing %hd", err, dbI);
      DmReleaseRecord(dbR, dbI, false);
    }
  }
  
  PRINT("Error %hx", err);
  ErrThrow(err);
}

/*
** Switch to hi-res mode
*/
UInt16 SwitchToHiRes(void) {
  UInt16 oldCoordinateSystem = 0;

  if (!d.sonyClie) {
    oldCoordinateSystem = WinSetCoordinateSystem(kCoordinatesNative);
    
    if (oldCoordinateSystem == kCoordinatesStandard) {
      RctCopyRectangle(&p.r, &d.r_hires);
      WinScaleRectangle(&d.r_hires);
    }
  } else {
    RctCopyRectangle(&p.r, &d.r_hires);
    d.r_hires.topLeft.x *= HiResCoef;
    d.r_hires.topLeft.y *= HiResCoef;
    d.r_hires.extent.x *= HiResCoef;
    d.r_hires.extent.y *= HiResCoef;
  }

  return oldCoordinateSystem;
}

/*
** Switch back to standard mode 
*/
UInt16 SwitchToLoRes(void) {
  if (d.hires && !d.sonyClie)
    return WinSetCoordinateSystem(kCoordinatesStandard);
  else
    return 0;
}

/*
** Switch back previous mode 
*/
static UInt16 SwitchToPreviousCoordinates(UInt16 oldCoordinateSystem) {
  if (d.hires && !d.sonyClie)
    return WinSetCoordinateSystem(oldCoordinateSystem);
  else
    return 0;
}

/*
** Adjust spacing for hi-res devices
*/
static Int16 AdjustSpacing(Int16 pixels) {
  if (d.hires)
    return pixels * HiResCoef;
  else
    return pixels;
}

/*
** GetObjectIndexFromPointer - simulate FrmGetObjectIndexFromPtr (available in 4.0 only)
*/
static UInt16 GetObjectIndexFromPointer(FormPtr frm, void* obj) {
  UInt16 index;
  UInt16 objIndex = frmInvalidObjectId;
  const UInt16 numObjects = FrmGetNumberOfObjects(frm);

  for (index = 0; index < numObjects; index++) {
    if (FrmGetObjectPtr(frm, index) == obj) {
      /* Found it */
      objIndex = index;
      break;
    }
  }

  return objIndex;
}

/*
** Sort database by sketch title.
*/
Int16 SortByName(void *rec1, void *rec2, Int16 other,
              SortRecordInfoPtr rec1SortInfo, SortRecordInfoPtr rec2SortInfo,
              MemHandle appInfoH) {
  DiddleBugRecordType record1, record2;
  Int16 result = 0;
  const Char* name1 = NULL;
  const Char* name2 = NULL;
  UInt16 len1 = 0;
  UInt16 len2 = 0;

  /* Read the records */
  MemMove(&record1, rec1, sizeof(DiddleBugRecordType));
  MemMove(&record2, rec2, sizeof(DiddleBugRecordType));

  /* Read the names */
  name1 = ((Char*) rec1) + sketchDataOffset + record1.sketchLength;
  name2 = ((Char*) rec2) + sketchDataOffset + record2.sketchLength;
  len1 = StrLen(name1);
  len2 = StrLen(name2);

  /* Compare the two names */
  result = TxtCompare(name1, len1 + 1, NULL, name2, len2 + 1, NULL);

  /* Move records with empty names to the end */
  if (len1 == 0 && len2 > 0)
    result = 1;
  else if (len1 > 0 && len2 == 0)
    result = -1;

  if (!result) {
    /* Either both names are equal, or do not exist, sort by priority */
    if (record1.priority < record2.priority)
      result = -1;
    else if (record2.priority > record2.priority)
      result = 1;
    else
      result = 0;
  }

  return result;
}

/*
** Sort database by alarm time.
*/
Int16 SortByAlarmTime(void *rec1, void *rec2, Int16 other,
              SortRecordInfoPtr rec1SortInfo, SortRecordInfoPtr rec2SortInfo,
              MemHandle appInfoH) {
  DiddleBugRecordType record1, record2;
  UInt32 alarm1, alarm2;
  Int16 result = 0;
  const UInt32 now = TimGetSeconds();

  /* Read the records */
  MemMove(&record1, rec1, sizeof(DiddleBugRecordType));
  MemMove(&record2, rec2, sizeof(DiddleBugRecordType));

  /* Calculate alarm times */
  alarm1 = ApptGetAlarmTime(&record1, now);
  alarm2 = ApptGetAlarmTime(&record2, now);

  /* Sort records without alarms at the very end */
  if (!(record1.flags & RECORD_FLAG_ALARM_SET))
    alarm1 = 0;
  if (!(record2.flags & RECORD_FLAG_ALARM_SET))
    alarm2 = 0;

  if (!alarm1 && alarm2) /* the first record doesn't have an alarm set */
    result = 1;
  else if (alarm1 && !alarm2) /* the second record doesn't have an alarm set */
    result = -1;
  else if (alarm1 && alarm2 && alarm1 != alarm2) /* both alarms exist */
    result = alarm1 < alarm2 ? -1 : 1;
  else {
    /* either both alarms do NOT exist or the are equal, sort by priority */
    if (record1.priority < record2.priority)
      result = -1;
    else if (record2.priority > record2.priority)
      result = 1;
    else
      result = 0;
  }

  return result;
}

/*
** Mirror the real canvas onto the screen
*/
void MirrorRealCanvas(void) {
  RectangleType* r = NULL;

  if (d.hires) {
    if (!d.sonyClie && WinGetCoordinateSystem() == kCoordinatesStandard)
      r = &p.r;
    else
      r = &d.r_hires;
  } else {
    r = &p.r;
  }
  
  MirrorPartialRealCanvas(r, false);
}

/*
** GetListWidth
*/
static Int16 GetListWidth(ListPtr list) {
  UInt16 num_items;
  UInt16 i = 0;
  Char* list_textP = NULL;
  FontID oldfont;
  Int16 list_width = 0;

  if (!list)
    return 10;  /* real skinny list indicates an error */

  num_items = LstGetNumberOfItems(list);
  oldfont = FntSetFont(stdFont);
  for (; i < num_items; ++i) {
    list_textP = LstGetSelectionText(list, i);
    if (list_textP)
      list_width = Max(list_width, FntLineWidth(list_textP, StrLen(list_textP)));
  }

  FntSetFont(oldfont);
  list_width += 8;
  
  return list_width;
}

/*
** GetFormattedDate - Get the proper format for the alarm label
*/
static void GetFormattedDate(DateTimeType *date, Char* string, Boolean wc) {
  Char temp[15];
  const DateFormatType longFormat = PrefGetPreference(prefLongDateFormat);

  SysCopyStringResource(temp, DayOfWeek(date->month, date->day, date->year) + SunString);
  StrNCopy(string, temp, ALARM_STRING_LENGTH);
  StrNCat(string, ", ", ALARM_STRING_LENGTH);
  switch (longFormat) {
  case dfDMYLong:
  case dfDMYLongWithDot:
  case dfDMYLongNoDay:
  case dfDMYLongWithComma:
    /* Day comes first */
    StrIToA(temp, date->day);
    StrNCat(string, temp, ALARM_STRING_LENGTH);
    if (longFormat == dfDMYLongWithDot)
      StrNCat(string, ". ", ALARM_STRING_LENGTH);
    else StrNCat(string, " ", ALARM_STRING_LENGTH);
    DateToAscii(date->month, date->day, date->year, dfMDYLongWithComma, temp);
    temp[3] = nullChr;
    StrNCat(string, temp, ALARM_STRING_LENGTH);
    break;

  default:
    DateToAscii(date->month, date->day, date->year, dfMDYLongWithComma, temp);
    temp[3] = nullChr;
    StrNCat(string, temp, ALARM_STRING_LENGTH);
    StrNCat(string, " ", ALARM_STRING_LENGTH);
    StrIToA(temp, date->day);
    StrNCat(string, temp, ALARM_STRING_LENGTH);
    break;
  }

  if (wc) /* if with comma */
    StrNCat(string, ", ", ALARM_STRING_LENGTH);
}

/*
** FlushToBuffer
*/
void FlushToBuffer(void) {
  WinHandle oldH;

  if (recordIsDirty) {
    oldH = WinSetDrawWindow(d.winbufM);
    WinPushDrawState();
    WinSetBackColor(0);
    WinSetForeColor(1);

    if (!d.sonyClie || !d.hires) {
      SwitchToLoRes();
      WinCopyRectangle(d.realCanvas, NULL, &p.r, 0, 0, winPaint);
    } else {
      HRWinCopyRectangle(d.sonyHRRefNum, d.realCanvas, NULL, &d.r_hires, 0, 0, winPaint);
    }

    WinPopDrawState();
    WinSetDrawWindow(oldH);
    recordUnsetDirty();
    recordSetBufDirty();
  }
  d.undo_mark = false;
}

/*
** LoadFromBuffer
*/
static void LoadFromBuffer(void) {
  RectangleType tempRect;
  WinHandle oldH;

  /* Set up environment */
  oldH = WinSetDrawWindow(d.realCanvas);
  WinPushDrawState();
  WinSetBackColor(0);
  WinSetForeColor(1);

  if (!d.sonyClie || !d.hires) {
    SwitchToLoRes();
    RctCopyRectangle(&p.r, &tempRect);
    tempRect.topLeft.x = 0;
    tempRect.topLeft.y = 0;
    
    WinCopyRectangle(d.winbufM, d.realCanvas, &tempRect, p.r.topLeft.x,
             p.r.topLeft.y, winPaint);
  } else {
    RctCopyRectangle(&d.r_hires, &tempRect);
    tempRect.topLeft.x = 0;
    tempRect.topLeft.y = 0;
    
    HRWinCopyRectangle(d.sonyHRRefNum, d.winbufM, d.realCanvas, &tempRect,
               d.r_hires.topLeft.x, d.r_hires.topLeft.y, winPaint);
  }

  /* Clean up */
  WinPopDrawState();
  WinSetDrawWindow(oldH);

  /* Load image */
  MirrorRealCanvas();
  recordUnsetDirty();
  d.undo_mark = false;
}

/*
** CompressSketch - assumes uPtr is either 160x160 or 320x320 bits
**                  (depending on the "hires" parameter)
**
**                  returns a locked pointer to the current record
*/
MemPtr CompressSketch(Boolean hires, DmOpenRef dbR, UInt16 dbI,
              MemHandle rec_name, MemHandle rec_note,
              AlarmSoundInfoType* rec_sound, UInt16* extraLength,
              UInt16* size, UInt8* uPtr) {
  UInt8* dataPtr, *headPtr, *buffer, *cuPtr;
  MemHandle t;
  MemPtr ptr;
  UInt16 i, j, k;
  UInt8 mask, thumbByte;
  Int16 name_len, note_len;
  Char* name;
  Char* note;
  const Int16 maxPixels = hires ? MAX_PIXELS_HIRES : MAX_PIXELS;
  const Int16 thumbnailAdjustment = hires ? 2 : 1;
  const Int16 bytesPerRow = maxPixels / 8;
   
  /* Now the real stuff */
  buffer = MemPtrNew(MaxRecordSize(hires)); /* Big enough to hold any sketch */
  if (!buffer) NoMemoryError();

  (*size) = 0;
  headPtr = buffer + sketchThumbnailSize;
  dataPtr = headPtr;

  /* Fill the thumbnail buffer */
  for (i=0; i < sketchThumbnailSize; ++i) {
    buffer[i] = 0x00;
    ++(*size);
  }

  /* Load the seed */
  if (uPtr[0]) headPtr[0] = 0x00;
  else headPtr[0] = 0x80;
  (*size)++;

  /* Loop thru every byte in uPtr data */
  cuPtr = uPtr;
  for (i=0; i < bytesPerRow; i++) {
    for (j=0; j < 8; j++) {
      for (k=0; k < bytesPerRow; k++) {
        if (cuPtr[0]) {
      /* Set the bit in the thumbnail */
      thumbByte = (i*4/thumbnailAdjustment) + (k/8/thumbnailAdjustment);
      
      if (i % thumbnailAdjustment == 0) {
        switch (k/thumbnailAdjustment%8) {
        case 0: mask = 0x80; break;
        case 1: mask = 0x40; break;
        case 2: mask = 0x20; break;
        case 3: mask = 0x10; break;
        case 4: mask = 0x08; break;
        case 5: mask = 0x04; break;
        case 6: mask = 0x02; break;
        case 7: mask = 0x01; break;
        default: mask = 0x00;
        }
      
        buffer[thumbByte] |= mask;
      }

      if (cuPtr[0] == 0xff) {
        /* Black */
        if (headPtr[0]&0x80) {
          if (headPtr[0]&0x40) {
        /* headPtr is mixed */
        if (dataPtr[0] == 0xff) {
          /* Previous byte was also black */
          headPtr[0]--;
          headPtr = dataPtr; headPtr[0] = 0x81;
        }
        else {
          /* Previous byte was not black */
          if ((headPtr[0]&0x3f) < 63) {
            headPtr[0]++;
            dataPtr++; dataPtr[0] = 0xff; (*size)++;
          }
          else {
            headPtr = dataPtr+1; headPtr[0] = 0x80;
            (*size)++;
          }
        }
          }
          else {
        /* headPtr is black */
        if ((headPtr[0]&0x3f) < 63) headPtr[0]++;
        else { headPtr++; headPtr[0] = 0x80; (*size)++; }
          }
        }
        else {
          /* headPtr is white, create a new headPtr */
          headPtr++; headPtr[0] = 0x80; (*size)++;
        }
      }
      else {
        /* Mixed */
        if ((headPtr[0]&0xc0) == 0xc0) {
          /* Add to the mixture */
          if ((headPtr[0]&0x3f) < 63) {
        headPtr[0]++;
          }
          else {
        dataPtr++; headPtr = dataPtr; headPtr[0] = 0xc0;
        (*size)++;
          }
          dataPtr++; dataPtr[0] = cuPtr[0]; (*size)++;
        }
        else {
          /* Create new headPtr for mixture */
          headPtr++; headPtr[0] = 0xc0; (*size)++;
          dataPtr = headPtr+1; dataPtr[0] = cuPtr[0]; (*size)++;
        }
      }
        }
        else {
      /* White */
      if (headPtr[0]&0x80) {
        if (headPtr[0]&0x40) {
          /* headPtr is mixed */
          if (dataPtr[0]) {
        /* Previous byte was not white */
        if ((headPtr[0]&0x3f) < 63) {
          headPtr[0]++;
          dataPtr++; dataPtr[0] = 0x00; (*size)++;
        }
        else {
          headPtr = dataPtr+1; headPtr[0] = 0x00;
          (*size)++;
        }
          }
          else {
        /* Previous byte was also white */
        headPtr[0]--;
        headPtr = dataPtr; headPtr[0] = 0x01;
          }
        }
        else {
          /* headPtr is black, create a new headPtr */
          headPtr++; headPtr[0] = 0x00; (*size)++;
        }
      }
      else {
        /* headPtr is white */
        if (headPtr[0] < 127) headPtr[0]++;
        else { headPtr++; headPtr[0] = 0x00; (*size)++; }
      }
        }
        cuPtr++;
      }
    }
  }

  /* Save any text or other info above the sketch data */
  t = DmGetRecord(dbR, dbI);
  if (!t) {
    MemPtrFree(buffer);
    DatabaseError(dbR, dbI);
  }
  ptr = MemHandleLock(t);

  if (!rec_name || !rec_note) abort();

  /* Save the high data */
  name = (Char*) MemHandleLock(rec_name);
  note = (Char*) MemHandleLock(rec_note);
  name_len = StrLen(name) + 1;
  note_len = StrLen(note) + 1;

  MemHandleUnlock(t);

  /* Resize the record */
  t = DmResizeRecord(dbR, dbI,
             sketchDataOffset + (*size) + name_len + note_len + sizeof(AlarmSoundInfoType));
  if (!t) {
    MemPtrFree(buffer);
    DatabaseError(dbR, dbI);
  }
  ptr = MemHandleLock(t);

  /* Write the sketch data to disk */
  DmWrite(ptr, sketchDataOffset, buffer, (*size));
  /* Re-Write the high data to disk */
  DmStrCopy(ptr, sketchDataOffset + (*size), name);
  DmStrCopy(ptr, sketchDataOffset + (*size) + name_len, note);
  /* Re-Write the MIDI information */
  DmWrite(ptr, sketchDataOffset + (*size) + name_len + note_len, rec_sound, sizeof(AlarmSoundInfoType));
  ASSERTNOT(d.record_sound.cardNo);

  /* Update record */
  *extraLength = sizeof(AlarmSoundInfoType);

  /* Clean up */
  MemHandleUnlock(rec_name);
  MemHandleUnlock(rec_note);
  MemPtrFree(buffer);
  return ptr;
}

/*
** FlushExtraData - flushes the high data to disk
**                  returns a locked pointer to the current record
*/
static MemPtr FlushExtraData(void) {
  MemHandle t;
  MemPtr ptr, buffer;
  UInt32 size;
  Int16 name_len, note_len;
  Char* name;
  Char* note;

  /* Now the real stuff */
  buffer = MemPtrNew(MaxRecordSize(d.hires)); /* Big enough to hold any sketch */
  if (!buffer) NoMemoryError();

  /* Save any text or other info above the sketch data */
  t = DmGetRecord(d.dbR, p.dbI);
  if (!t) {
    MemPtrFree(buffer);
    DatabaseError(d.dbR, p.dbI);
  }
  ptr = MemHandleLock(t);

  /* Save the image data */
  MemMove(buffer, ptr + sketchDataOffset, d.record.sketchLength);

  if (!d.record_name || !d.record_note) abort();

  /* Save the high data */
  name = (Char*) MemHandleLock(d.record_name);
  note = (Char*) MemHandleLock(d.record_note);
  name_len = StrLen(name) + 1;
  note_len = StrLen(note) + 1;

  /* Resize the record */
  size = sketchDataOffset + d.record.sketchLength + name_len + note_len + sizeof(AlarmSoundInfoType);
  if (MemHandleSize(t) != size) {
    MemHandleUnlock(t);
    t = DmResizeRecord(d.dbR, p.dbI, size);
    if (!t) {
      MemPtrFree(buffer);
      DatabaseError(d.dbR, p.dbI);
    }
    ptr = MemHandleLock(t);
  }

  /* Write the sketch data to disk */
  DmWrite(ptr, sketchDataOffset, buffer, d.record.sketchLength);
  /* Re-Write the high data to disk */
  DmStrCopy(ptr, sketchDataOffset + d.record.sketchLength, name);
  DmStrCopy(ptr, sketchDataOffset + d.record.sketchLength + name_len, note);
  /* Re-Write the MIDI information */
  DmWrite(ptr, sketchDataOffset + d.record.sketchLength + name_len + note_len, &d.record_sound, sizeof(AlarmSoundInfoType));

  ASSERTNOT(d.record_sound.cardNo);

  /* Update record */
  d.record.extraLength = sizeof(AlarmSoundInfoType);

  /* Clean up */
  MemHandleUnlock(d.record_name);
  MemHandleUnlock(d.record_note);
  MemPtrFree(buffer);
  return ptr;
}

/*
** FlushToDisk
*/
void FlushToDisk(Boolean flushSketchData) {
  MemPtr ptr;

  /* Bail if invalid index */
  if (p.dbI == noRecordSelected) return;

  if (d.record_private) {
    /* 1. bail if hidden                   */
    /* 2. ignore flushSketchData if masked */
    /* 3. behave normally otherwise        */
    if (d.privateRecordStatus == hidePrivateRecords)
      return;
    else if (d.privateRecordStatus == maskPrivateRecords && !d.unmaskedCurrentRecord)
      flushSketchData = false;
  }

  if (flushSketchData) FlushToBuffer();

  /* Bail if index is out of range */
  if ((p.dbI+1) > DmNumRecords(d.dbR)) return;

  /* Also save the sketch data if requested and needed */
  if (flushSketchData && recordIsBufDirty) {
    recordUnsetBufDirty();

    /* ptr is locked on return */
    ptr = CompressSketch(d.hires, d.dbR, p.dbI, d.record_name, d.record_note,
             &d.record_sound, &d.record.extraLength,
             &d.record.sketchLength, BmpGetBits(WinGetBitmap(d.winbufM)));
  }
  else {
    ptr = FlushExtraData();
  }

  /* Write the header-type data */
  DmWrite(ptr, 0, &(d.record), sizeof(DiddleBugRecordType));

  /* Clean winUp */
  MemPtrUnlock(ptr);
  DmReleaseRecord(d.dbR, p.dbI, true);
}

/*
** UncompressSketch - assumes uPtr of 160x160 or 320x320 bits
*/
Boolean UncompressSketch(UInt8* cPtr, UInt8* uPtr, UInt32 size, Boolean hires) {
  UInt32 i, j = 0;
  UInt8 num_bytes;
  Int16 maxPixels, maxBytes;

  /* Set up our "constants" */
  if (hires) {
    maxPixels = MAX_PIXELS_HIRES;
  } else {
    maxPixels = MAX_PIXELS;
  }
  maxBytes = maxPixels * maxPixels / 8;

  for (i = 0; i < size; ++i) {
    if (cPtr[i] & 0x80) {
      num_bytes = cPtr[i] & 0x3f;  /* ~(0x40|0x80) */
      ++num_bytes;

      if (num_bytes > (maxBytes - j))
    return false; /* indicate an error to our caller */

      if (cPtr[i] & 0x40) {
    /* Mixed */
    MemMove(uPtr + j, cPtr + i + 1, num_bytes);
    i += num_bytes;
      } else {
    /* Black */
    MemSet(uPtr + j, num_bytes, 0xff);
      }
    } else {
      /* White */
      num_bytes = cPtr[i] & 0x7f; /* ~0x80 */
      ++num_bytes;

      if (num_bytes > (maxBytes - j))
    return false; /* indicate an error to our caller */

      MemSet(uPtr + j, num_bytes, 0x00);
    }
    j += num_bytes;
  }

  return true;
}

/*
** AllocImage - create a new image record.
*/
void AllocImage(void) {
  MemHandle t = NULL;
  MemPtr ptr = NULL;
  DiddleBugRecordType record;
  UInt16 attr = 0;
  UInt32 additionalSize = 0, comprBytesCount = 0;
  Char name[32];

  if (p.flags & PFLAGS_AUTO_DATETIME_STAMP) {
    MakeDateTimeStamp(TimGetSeconds(), name);
    additionalSize = StrLen(name);
  }

  t = DmNewRecord(d.dbR, &p.dbI, MinRecordSize(d.hires) + additionalSize);
  if (!t) 
    DatabaseError(d.dbR, p.dbI);
  ptr = MemHandleLock(t);
  MemSet(&record, sizeof(DiddleBugRecordType), 0);
  comprBytesCount = d.hires ? 25 * HiResCoef * HiResCoef : 25;
  record.sketchLength = sketchThumbnailSize + 1 + comprBytesCount;
  record.priority = ep.defaultPriority;
  DmSet(ptr, 0, MinRecordSize(d.hires) + additionalSize, 0);
  DmSet(ptr, sketchDataOffset + sketchThumbnailSize + 1, comprBytesCount, 0x7f);
  DmWrite(ptr, 0, &record, sizeof(DiddleBugRecordType));

  if (p.flags & PFLAGS_AUTO_DATETIME_STAMP) {
    /* Store name */
    DmStrCopy(ptr, sketchDataOffset + record.sketchLength, name);
  }

  MemHandleUnlock(t);
  DmReleaseRecord(d.dbR, p.dbI, true);

  /* Save category */
  DmRecordInfo(d.dbR, p.dbI, &attr, NULL, NULL);
  attr &= ~dmRecAttrCategoryMask;
  if (p.category != dmAllCategories)
    attr |= p.category;
  else
    attr |= 0;

  attr |= dmRecAttrDirty;
  DmSetRecordInfo(d.dbR, p.dbI, &attr, NULL);

  /* Update cached number of records */
  d.records_in_cat = DmNumRecordsInCategory(d.dbR, p.category);

  PRINT("AllocImage() finished. p.dbI = %hd", p.dbI);
}

/*
** Show or hide the lock resp. clear and delete buttons
** in the bottom toolbar.
*/
static void UpdateLockButton() {
  FormType* frm = FrmGetActiveForm();
  const UInt16 frmID = FrmGetActiveFormID();

  /* Bail out if we don't have a toolbar in the current form */
  if (frmID != DiddleForm && frmID != DiddleTForm) return;
  if (p.flags & PFLAGS_WITHOUT_TOOLBAR) return;

  if (recordIsLocked) {
    /* Show the lock button */
    HideObject(frm, MainClearButton);
    HideObject(frm, MainDeleteButton);
    ShowObject(frm, MainLockButton);
  } else {
    /* Hide the lock button */
    HideObject(frm, MainLockButton);
    ShowObject(frm, MainClearButton);
    ShowObject(frm, MainDeleteButton);
  }
}

/*
** Draw masked record.
*/
void DrawMaskedRecord(WinHandle winH, const CustomPatternType* pattern) {
  WinHandle oldH;
  RectangleType r;
  
  /* Set up draw state */
  oldH = WinSetDrawWindow(winH);
  WinPushDrawState();
  WinSetBackColor(0);
  
  /* Draw mask pattern */
  WinGetDrawWindowBounds(&r);
  WinSetPattern(pattern);
  WinFillRectangle(&r, 0);
  
  /* Clean up */
  WinPopDrawState();
  WinSetDrawWindow(oldH);
}

/*
** LoadFromDisk
*/
void LoadFromDisk(void) {
  MemHandle t = NULL;
  MemPtr ptr, sketchPtr;
  UInt32 highDataOffset;
  Int16 len;
  UInt16 attr;

  /* Clear unmasked flag */
  d.unmaskedCurrentRecord = false;

  /* Open and lock the record */
  t = DmQueryRecord(d.dbR, p.dbI);
  if (!t) {
    AllocImage();
    t = DmQueryRecord(d.dbR, p.dbI);
    if (!t) abort();
  }
  ptr = MemHandleLock(t);

  /* Is the record private? */
  DmRecordInfo(d.dbR, p.dbI, &attr, NULL, NULL);
  d.record_private = attr & dmRecAttrSecret;

  /* Read the header data */
  MemMove(&d.record, ptr, sizeof(DiddleBugRecordType));
  sketchPtr = ptr + sketchDataOffset + sketchThumbnailSize + 1;

  /* Mask if private */
  if (d.record_private && d.privateRecordStatus == maskPrivateRecords) {
    DrawMaskedRecord(d.winbufM, maskPattern);
  } else {
    /* Uncompress to the offscreen window */
    if (!UncompressSketch(sketchPtr, BmpGetBits(WinGetBitmap(d.winbufM)),
              d.record.sketchLength - sketchThumbnailSize - 1, d.hires)) {
      /* An error has occured. Either we get permission to delete */
      /* the current sketch, or we crash now.                     */
      if (FrmAlert(DeleteCorruptedSketchAlert) == 0) {
    MemHandleUnlock(t);
    RemoveImage();
    return;
      } else {
    MemHandleUnlock(t);
    ErrThrow(dbErrCorruptedSketch);
      }
    }
  }

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

  ASSERTNOT(d.record_sound.cardNo);

  /* Cleanup */
  MemHandleUnlock(t);
  LoadFromBuffer();
  d.nextPeriod = TimGetTicks()+100;

  /* Update menu */
  if (!d.os4)
    FrmSetMenu(FrmGetActiveForm(), DiddleMenu35);

  /* Update the lock button */
  UpdateLockButton();

  /* Update x/y button & note button*/
  if (FrmGetActiveFormID() == DiddleTForm) {
    UpdateNoteButton();
    UpdatePageButton();
  }
}

/*
** DrawBitmapLabel
*/
static void DrawBitmapLabel(UInt16 bitmapID, Int16 x, Int16 y) {
  MemHandle t;
  BitmapPtr bitmapP;

  t = DmGetResource('Tbmp', bitmapID);
  ASSERT(t);
  bitmapP = MemHandleLock(t);

  /* Should draw larger bitmap for HandEra, doesn't work though */
    WinDrawBitmap(bitmapP, x, y);

  MemHandleUnlock(t);
  DmReleaseResource(t);
}

/*
** DrawPenLabel
*/
static void DrawPenLabel(const UInt8 *pen, Coord x, Coord y) {
  UInt16 i = 0;
  UInt16 used = 0;
  RectangleType rect = { {x, y}, {handera(14), handera(10)} };
  PointType pts[NPENPIX];

  WinEraseRectangle(&rect, 4);

  /* Set up hi-res */
  WinPushDrawState();

  if (d.hires) {
    SwitchToHiRes();
    if (!d.sonyClie) {
      x = WinScaleCoord(x, true);
      y = WinScaleCoord(y, true);
    } else {
      x *= HiResCoef;
      y *= HiResCoef;
    }
  }

  for (; i < NPENPIX && pen[i] != 0; i += 1) {
    pts[used].x = x + AdjustSpacing(handera(6)) + dxFromPenpix(pen[i]);
    pts[used].y = y + AdjustSpacing(handera(4)) + dyFromPenpix(pen[i]);

    ++used;
  }

  /* WinPaintPixels uses the current pattern */
  WinSetPatternType(blackPattern);

  if (!d.sonyClie || !d.hires)
    WinPaintPixels(used, pts);
  else
    HRWinPaintPixels(d.sonyHRRefNum, used, pts);

  /* Clean up */
  WinPopDrawState();
}

/*
** DrawInkLabel
*/
static void DrawInkLabel(const CustomPatternType* ink, Int16 x, Int16 y) {
  RectangleType rect = { {x+1, y}, {13, 9} };

  WinSetPattern(ink);
  WinFillRectangle(&rect, 3);
  WinSetPattern((const CustomPatternType*) &p.inkpat);
}

/*
** LabelTitle - Put the current time into the title
*/
static void LabelTitle(void) {
  FormPtr frm;
  UInt32 curr_secs;
  Char title_text[9];
  DateTimeType date;

  if (d.doNotUpdateFormTitle || FrmGetActiveFormID() != DiddleTForm)
    return;

  frm = FrmGetActiveForm();
  curr_secs = TimGetSeconds();
  TimSecondsToDateTime(curr_secs, &date);

  TimeToAscii(date.hour, date.minute, d.shorttime, title_text);
  title_text[6] = nullChr;  /* Ensure it stays within limits */
  if (frm)
    FrmCopyTitle(frm, title_text);
}

/*
** LabelAlarm - put the correct alarm in the button bar
**              This function should only be called while on the DiddleForm
**              or the DiddleTForm.
*/
void LabelAlarm(void) {
  FormPtr frm = FrmGetActiveForm();
  const UInt16 formID = FrmGetActiveFormID();
  Char dateStr[ALARM_STRING_LENGTH];
  DateTimeType date, curr, cdown;
  DateType cdowndate;
  UInt32 cdowndays;
  UInt32 current_secs, real_alarm_secs = 0;
  DynamicButtonType* btn = NULL;

  /* Should we do something? */
  if (d.is_xfer_mode) return;
  if (!frm || (formID != DiddleForm && formID != DiddleTForm)) return;
  if (p.flags & PFLAGS_WITHOUT_TOOLBAR) return;

  /* Erase command bar if necessary */
  if (d.erase_menu) {
    MenuEraseStatus(NULL);
    d.erase_menu = false;
  }

  btn = (DynamicButtonType*) FrmGetGadgetData(frm, FrmGetObjectIndex(frm, MainDateSelTrig));
  if (!btn) return; /* the gadgets are not set up again after FrmGotoForm() */

  current_secs = TimGetSeconds();
  if (recordIsAlarmSet)
    real_alarm_secs = ApptGetAlarmTime(&d.record, current_secs);

  if (real_alarm_secs > current_secs) {
    TimSecondsToDateTime(real_alarm_secs, &date);

    if (recordIsCountdown || real_alarm_secs == d.record.snoozeSecs) {
      TimSecondsToDateTime(real_alarm_secs-current_secs, &cdown);
      cdowndate.day = cdown.day;
      cdowndate.month = cdown.month;
      cdowndate.year = cdown.year - firstYear;
      cdowndays = DateToDays(cdowndate);

      if (cdowndays == 0) {
    if (real_alarm_secs != d.record.snoozeSecs)
      SysCopyStringResource(dateStr, CountdownString);
    else
      SysCopyStringResource(dateStr, SnoozeString);
    StrNCopy(btn->content.text, dateStr, ALARM_STRING_LENGTH);
      } else if (cdowndays == 1) {
    SysCopyStringResource(dateStr, SingleDayString);
    StrNCopy(btn->content.text, dateStr, ALARM_STRING_LENGTH);
      } else {
    StrIToA(btn->content.text, cdowndays);
    SysCopyStringResource(dateStr, MultiDayString);
    StrNCat(btn->content.text, dateStr, ALARM_STRING_LENGTH);
      }

      StrIToA(dateStr, cdown.hour);
      if (StrLen(dateStr) == 1) {
    dateStr[2] = nullChr;
    dateStr[1] = dateStr[0];
    dateStr[0] = '0';
      }

      StrNCat(btn->content.text, dateStr, ALARM_STRING_LENGTH);
      StrNCat(btn->content.text, ":", ALARM_STRING_LENGTH);
      StrIToA(dateStr, cdown.minute);

      if (StrLen(dateStr) == 1) {
    dateStr[2] = nullChr;
    dateStr[1] = dateStr[0];
    dateStr[0] = '0';
      }

      StrNCat(btn->content.text, dateStr, ALARM_STRING_LENGTH);
      StrNCat(btn->content.text, ":", ALARM_STRING_LENGTH);
      StrIToA(dateStr, cdown.second);

      if (StrLen(dateStr) == 1) {
    dateStr[2] = nullChr;
    dateStr[1] = dateStr[0];
    dateStr[0] = '0';
      }

      StrNCat(btn->content.text, dateStr, ALARM_STRING_LENGTH);
    } else {
      TimSecondsToDateTime(current_secs, &curr);
      if ((curr.day == date.day) && (curr.month == date.month) &&
      (curr.year == date.year)) {
    SysCopyStringResource(btn->content.text, TodayString);
      } else if (((curr.day+1) == date.day) && (curr.month == date.month) &&
         (curr.year == date.year)) {
    SysCopyStringResource(btn->content.text, TomorrowString);
      } else {
    GetFormattedDate(&date, dateStr, true);
    StrNCopy(btn->content.text, dateStr, ALARM_STRING_LENGTH);
      }

      TimeToAscii(date.hour, date.minute, PrefGetPreference(prefTimeFormat), dateStr);
      StrNCat(btn->content.text, dateStr, ALARM_STRING_LENGTH);
    }
  } else {
    if (recordIsCountdown)
      SysCopyStringResource(btn->content.text, NoCountdownString);
    else
      SysCopyStringResource(btn->content.text, NoAlarmString);
    
    if (real_alarm_secs < current_secs) {
      recordUnsetAlarm();
      d.record.alarmSecs = 0;
      d.record.snoozeSecs = 0;
    }
  }
  
  DynBtnDraw(btn);
}

/*
** Draw current ink label in given window.
*/
static void DrawInkLabelDynamic(WinHandle w, UInt16 offset_x, UInt16 offset_y,
                UInt16 round, const CustomPatternType* ink) {
  RectangleType r;
  WinHandle oldH = WinSetDrawWindow(w);

  /* Save old state */
  WinPushDrawState();

  WinGetDrawWindowBounds(&r);
  WinSetBackColor(0); /* otherwise the pixels will not count as "off" */
  WinSetPattern(ink);
  WinFillRectangle(&r, round);

  /* Clean up */
  WinPopDrawState();
  WinSetDrawWindow(oldH);
}

/*
** Draw current pen label in given window.
*/
static void DrawPenLabelDynamic(WinHandle w, UInt16 offset_x, UInt16 offset_y,
                UInt16 round, const UInt8* pen) {
  UInt16 i = 0;
  UInt16 used = 0;
  RectangleType r;
  PointType pts[NPENPIX];
  WinHandle oldH = WinSetDrawWindow(w);
  Coord x, y;

  /* Save old state */
  WinPushDrawState();

  WinSetBackColor(0); /* otherwise the pixels will not count as "off" */

  WinGetDrawWindowBounds(&r);
  WinEraseRectangle(&r, round);

  /* Set up hi-res */
  if (d.hires) {
    SwitchToHiRes();
    if (!d.sonyClie) {
      offset_x = WinScaleCoord(offset_x, true);
      offset_y = WinScaleCoord(offset_y, true);
      WinScaleRectangle(&r);
    } else {
      HRWinGetWindowBounds(d.sonyHRRefNum, &r);
      offset_x *= HiResCoef;
      offset_y *= HiResCoef;
    }
  }

  /* Scale top left point for HandEra */
  x = handera(r.topLeft.x);
  y = handera(r.topLeft.y);

  for (i = 0; i < NPENPIX && pen[i] != 0; i += 1) {
    pts[used].x = x + offset_x + AdjustSpacing(6) + dxFromPenpix(pen[i]);
    pts[used].y = y + offset_y + AdjustSpacing(4) + dyFromPenpix(pen[i]);

    ++used;
  }
  
  /* WinPaintPixels uses the current pattern */
  WinSetPatternType(blackPattern);

  if (!d.sonyClie || !d.hires)
    WinPaintPixels(used, pts);
  else /* Sony Clie in hi-res mode */
    HRWinPaintPixels(d.sonyHRRefNum, used, pts);

  /* Clean up */
  WinPopDrawState();
  WinSetDrawWindow(oldH);
}

/*
** ClearImage - clear a new image record.
*/
static void ClearImage(void) {
  FlushToBuffer();
  WinSetDrawWindow(d.winbufM);

  /* Save old state */
  WinPushDrawState();

  WinSetBackColor(0);

  if (!d.sonyClie || !d.hires) {
    WinEraseRectangle(&screen_rect, 0);
  } else {
    RectangleType r;
    RctCopyRectangle(&screen_rect, &r);
    r.extent.x *= HiResCoef;
    r.extent.y *= HiResCoef;

    HRWinEraseRectangle(d.sonyHRRefNum, &r, 0);
  }

  /* Restore old state */
  WinPopDrawState();

  WinSetDrawWindow(d.winM);
  recordSetBufDirty();
  LoadFromBuffer();
}

/*
** SketchIsEmpty - Determine if a given sketch is empty or not
**                 Assumes a non-busy-bit record handle is passed
*/
static Boolean SketchIsEmpty(MemHandle sketchHandle) {
  DiddleBugRecordType record;

  MemMove(&record, MemHandleLock(sketchHandle), sizeof(DiddleBugRecordType));
  MemHandleUnlock(sketchHandle);

  PRINT("SketchIsEmpty %hd", p.dbI);

  /* Take advantage of the fact that all whitespace is 181 bytes */
  return ((record.sketchLength == EmptySketchSize(d.hires)) &&
      !(record.flags&RECORD_FLAG_ALARM_SET));
}

/*
** Update date/time stamp.
*/
static void UpdateDateTimeStamp(void) {  
  MemHandle t = NULL;
  MemPtr ptr = NULL;
  DiddleBugRecordType record;
  Char name[32];
  UInt32 additionalSize = 0;

  MakeDateTimeStamp(TimGetSeconds(), name);
  additionalSize = StrLen(name);
        
  t = DmGetRecord(d.dbR, p.dbI);
  if (!t) 
    DatabaseError(d.dbR, p.dbI);
  ptr = MemHandleLock(t);
  MemMove(&record, ptr, sizeof(DiddleBugRecordType));
  MemHandleUnlock(t);
  t = DmResizeRecord(d.dbR, p.dbI, MinRecordSize(d.hires) + additionalSize);
  if (!t)
    DatabaseError(d.dbR, p.dbI);
  ptr = MemHandleLock(t);
  
  /* Remember that custom MIDI sounds are discarded */
  record.extraLength = 0;
  DmWrite(ptr, 0, &record, sizeof(DiddleBugRecordType));

  /* Save the new name (date/time stamp) */
  DmSet(ptr, sketchDataOffset + record.sketchLength, 
    MinRecordSize(d.hires) + additionalSize - sketchDataOffset - record.sketchLength, 0);
  DmStrCopy(ptr, sketchDataOffset + record.sketchLength, name);

  /* Clean up */
  MemHandleUnlock(t);
  DmReleaseRecord(d.dbR, p.dbI, true);
}

/*
** NewScratchImage - create a new scratch image or go to last empty one
*/
static void NewScratchImage(void) {
  const UInt16 total = DmNumRecords(d.dbR);
  MemHandle t = NULL;  
  UInt16 oldIndex = 0;

  /* Check whether the current index is out of range */
  if (p.dbI >= total)
    p.dbI = 0;

  oldIndex = p.dbI;

  if (total) {
    /* See if current sketch is empty */
    t = DmQueryRecord(d.dbR, p.dbI);
    if (!t) abort();
    if (SketchIsEmpty(t)) {
      /* Update title before returning */
      if ((! (p.flags & PFLAGS_NEVER_OVERWRITE_TITLE))
      && p.flags & PFLAGS_AUTO_DATETIME_STAMP) {
    UpdateDateTimeStamp();  
      }
      return; /* we found a scratch image */
    }

    /* Get the highest numbered scratch sketch */
    p.dbI = total;
    while (p.dbI > 0) {
      --p.dbI;

      t = DmQueryRecord(d.dbR, p.dbI);

      if (SketchIsEmpty(t)) {
    /* Update title before returning */
    if ((! (p.flags & PFLAGS_NEVER_OVERWRITE_TITLE))
        && p.flags & PFLAGS_AUTO_DATETIME_STAMP) {
      UpdateDateTimeStamp();    
    }
    return; /* we found a scratch image */
      }
    }
  }

  /* Create a new sketch after the current one   */
  /* (or after the last one if the corresponding */
  /* preferences option is set) */
  if (p.flags & PFLAGS_CREATE_LAST_HARDBUTTON)
    p.dbI = total;
  else
    p.dbI = oldIndex + 1;
  AllocImage();
}

/*
** FillImage - fill a new image record
*/
static void FillImage(void) {
  FlushToBuffer();
  WinSetDrawWindow(d.winbufM);

  /* Save old state */
  WinPushDrawState();

  WinSetBackColor(0);
  WinSetPattern((const CustomPatternType*) &p.inkpat);
  
  if (!d.sonyClie || !d.hires) {
    WinFillRectangle(&screen_rect, 0);
  } else {
    RectangleType r;
    RctCopyRectangle(&screen_rect, &r);
    r.extent.x *= HiResCoef;
    r.extent.y *= HiResCoef;

    HRWinFillRectangle(d.sonyHRRefNum, &r, 0);  
  }

  /* Restore old state */
  WinPopDrawState();

  WinSetDrawWindow(d.winM);
  recordSetBufDirty();
  LoadFromBuffer();
}

/*
** InvertImage - XOR the current image, done on DB, rather than window
**               because window is sometimes obscured by titlebar
*/
static void InvertImage(void) {
  FlushToBuffer();
  WinSetDrawWindow(d.winbufM);

  if (!d.sonyClie || !d.hires) {
    WinInvertRectangle(&screen_rect, 0);
  } else {
    RectangleType r;
    RctCopyRectangle(&screen_rect, &r);
    r.extent.x *= HiResCoef;
    r.extent.y *= HiResCoef;

    HRWinInvertRectangle(d.sonyHRRefNum, &r, 0);
  }

  WinSetDrawWindow(d.winM);
  recordSetBufDirty();
  LoadFromBuffer();
}

/*
** RemoveImage
*/
static void RemoveImage(void) {
  UInt16 new_idx = p.dbI;
  const Boolean last_in_cat = DmPositionInCategory(d.dbR, p.dbI, p.category) + 1 == d.records_in_cat;
  Err err = errNone;

  PRINT("Removing image %hd of %hd", p.dbI, DmNumRecords(d.dbR));

  if (last_in_cat)
    err = DmSeekRecordInCategory(d.dbR, &new_idx, 1, dmSeekBackward, p.category);
  else {
    err = DmSeekRecordInCategory(d.dbR, &new_idx, 1, dmSeekForward, p.category);
    --new_idx; /* we remove an image before this one */
  }

  DmRemoveRecord(d.dbR, p.dbI);
  if (err != errNone) 
    AllocImage();
  else
    p.dbI = new_idx;

  /* Update cached number of records */
  if (d.records_in_cat > 1)
    --d.records_in_cat;

  if (d.formID == DiddleForm || d.formID == DiddleTForm)
    LoadFromDisk();
  RescheduleAlarms(d.dbR);
}

/*
** NewSketch - create a new sketch after the current p.dbI
**             Will remain on the current sketch if empty
*/
static void NewSketch(void) {
  MemHandle t;

  FlushToDisk(true);
  t = DmQueryRecord(d.dbR, p.dbI);
  if (!t) abort();

  if(!SketchIsEmpty(t)) {
    if (p.flags & PFLAGS_CREATE_LAST_ICON)
      p.dbI = DmNumRecords(d.dbR);
    else
      p.dbI += 1;
    AllocImage();
  }

  LoadFromDisk();
}

/*
** DuplicateSketch - create a new sketch after the current one and copy
**                   the current one to it. Will do nothing if current
**                   sketch is empty.
*/
static void DuplicateSketch() {
  MemHandle t;

  SndPlaySystemSound(sndClick);
  FlushToDisk(true);
  t = DmQueryRecord(d.dbR, p.dbI);
  if (!t) abort();

  if(!SketchIsEmpty(t)) {
    UInt16 attr_old, attr_new;
    UInt16 new_idx = p.dbI + 1;
    Int16 size = MemHandleSize(t);
    MemHandle target;
    MemPtr ptr_t, ptr_s;

    target = DmNewRecord(d.dbR, &new_idx, size);
    if (!target)
      DatabaseError(d.dbR, new_idx);
    ptr_t = MemHandleLock(target);
    ptr_s = MemHandleLock(t);

    DmWrite(ptr_t, 0, ptr_s, size);

    MemHandleUnlock(t);
    MemHandleUnlock(target);
    DmReleaseRecord(d.dbR, new_idx, true);

    /* Get old record attributes */
    DmRecordInfo(d.dbR, p.dbI, &attr_old, NULL, NULL);

    /* finally increase index */
    p.dbI += 1;

    /* Save category & private status */
    DmRecordInfo(d.dbR, p.dbI, &attr_new, NULL, NULL);
    attr_new &= ~dmRecAttrCategoryMask;
    attr_new |= (attr_old & dmRecAttrCategoryMask) | (attr_old & dmRecAttrSecret) | dmRecAttrDirty;
    DmSetRecordInfo(d.dbR, p.dbI, &attr_new, NULL);

    /* Update cached settings */
    d.records_in_cat++;
  }

  LoadFromDisk();
}

/*
** SendGotoEvent
*/
void SendGotoEvent(UInt16 recordNum, UInt16 field, UInt16 pos, UInt16 len) {
  EventType event;

  event.eType = frmGotoEvent;
  event.data.frmGoto.formID = d.formID;
  event.data.frmGoto.recordNum = recordNum;
  event.data.frmGoto.matchFieldNum = field;
  event.data.frmGoto.matchPos = pos;
  event.data.frmGoto.matchLen = len;
  EvtAddEventToQueue(&event);
}

/*
** ModalEvent
*/
static Boolean ModalEvent(EventPtr e) {
  Boolean handled = false;

  switch (e->eType) {
  case frmUpdateEvent:
    if (e->data.frmUpdate.updateCode == fUpdateDeletedCurrent) {
      EvtEnqueueKey(confirmChr, confirmChr, commandKeyMask);
      d.alarm_deleted_current = true;
      handled = true;
    }
    else if (e->data.frmUpdate.updateCode == fUpdateDeletedAny) {
      d.alarm_deleted_any = true;
      handled = true;
    }
    break;
  case frmGotoEvent:
    EvtEnqueueKey(confirmChr, confirmChr, commandKeyMask);
    d.alarm_deleted_current = true;
    p.dbI = e->data.frmGoto.recordNum;
    SwitchCategoryForGoto();
    e->eType = dbOpenRecordFieldEvent;
    EvtAddEventToQueue(e);
    handled = true;
    break;
  default:
    /* do nothing */
  }

  return handled;
}

/*
** PriorModal - called before modal dialogs in the main form
*/
static void PriorModal(UInt16 formID) {
  FlushToDisk(true);
  d.alarm_deleted_current = false;
  d.alarm_deleted_any = false;
  FrmSetEventHandler(FrmGetFormPtr(formID), ModalEvent);
  d.formID = formID;
}

/*
** PostModal - called after modal dialogs in the main form
*/
static Boolean PostModal(FormPtr frm) {
  Boolean bail = false;

  /* Check if sketch was deleted during the dialog */
  if (d.alarm_deleted_current) {
    if (frm) FrmDeleteForm(frm);
    LoadFromDisk();
    LabelAlarm();
    bail=true;
  }

  if (d.alarm_deleted_any) LabelAlarm();
  d.formID = FrmGetActiveFormID();
  return bail;
}

/*
** DoTextPopup - Pop up the Insert Text form
*/
static void DoTextPopup(void) {
  FormPtr frm = FrmInitForm(TextForm);
  UInt16 foi;     /* font pushbutton object index */
  ListPtr list;
  ControlPtr ctl;
  Char* str;
  UInt16 button_pushed;

  /* Set the font pushbutton */
  switch (p.txt_font) {
  case boldFont:
    foi = TextBoldFontPush;
    break;
  case largeFont:
    foi = TextLargeFontPush;
    break;
  case largeBoldFont:
    foi = TextLargeBoldFontPush;
    break;
  case stdFont: /* fall thru */
  default:
    foi = TextStdFontPush;
    break;
  }
  CtlSetValue(GetObjectPointer(frm, foi), true);
  /* Set the dropdown list selection */
  list = GetObjectPointer(frm, TextModeList);
  LstSetSelection(list, p.txt_mode);
  str = LstGetSelectionText(list, p.txt_mode);
  CtlSetLabel(GetObjectPointer(frm, TextModePop), str);
  /* Set the focus to the text field */
  FrmSetFocus(frm, FrmGetObjectIndex(frm, TextField));

  PriorModal(TextForm);
  button_pushed = FrmDoDialog(frm);
  if (PostModal(frm)) return;

  if (TextInsertButton == button_pushed) {
    /* Get the font to use */
    ctl = GetObjectPointer(frm, TextStdFontPush);
    if (CtlGetValue(ctl)) p.txt_font = stdFont;
    ctl = GetObjectPointer(frm, TextBoldFontPush);
    if (CtlGetValue(ctl)) p.txt_font = boldFont;
    ctl = GetObjectPointer(frm, TextLargeFontPush);
    if (CtlGetValue(ctl)) p.txt_font = largeFont;
    ctl = GetObjectPointer(frm, TextLargeBoldFontPush);
    if (CtlGetValue(ctl)) p.txt_font = largeBoldFont;

    /* Get the text insertion mode */
    list = GetObjectPointer(frm, TextModeList);
    p.txt_mode = LstGetSelection(list);

    /* Copy text to insert into d.InsertionText */
    str = FldGetTextPtr(GetObjectPointer(frm, TextField));
    d.InsertionText[0] = 0; /* just to make sure... */
    if (str) {
      StrNCopy(d.InsertionText, str, 31);
      /* Signal the new mode for the Pen Handler */
      if (StrLen(d.InsertionText) > 0) {
    d.drawmode = dbModeText;
    d.last_insert = d.drawmode;
      }
      d.txt_oldfont = FntSetFont(p.txt_font);
    }
    /* We rely on the Pen Queue being empty here */
    EvtFlushPenQueue();
  }

  /* Cleanup */
  FrmDeleteForm(frm);
}

/*
** Handles events for the Insert Shape popup
*/
static Boolean ShapeEvent(EventPtr e) {
  Boolean handled = false;
  FormPtr frm = FrmGetActiveForm();

  switch (e->eType) {
  case ctlSelectEvent:
    switch (e->data.ctlEnter.controlID) {
    case ShapeRectPush:
      CtlSetGraphics(GetObjectPointer(frm, ShapeOutlinePush), ShapeRectBitmap, NULL);
      CtlSetGraphics(GetObjectPointer(frm, ShapeFilledPush), ShapeRectFilledBitmap, NULL);
      CtlSetGraphics(GetObjectPointer(frm, ShapeFilledXorPush), ShapeRectFilledXorBitmap, NULL);
      CtlSetGraphics(GetObjectPointer(frm, ShapeOutlineXorPush), ShapeRectOutlineXorBitmap, NULL);
      break;
    case ShapeRoundRectPush:
      CtlSetGraphics(GetObjectPointer(frm, ShapeOutlinePush), ShapeRoundRectBitmap, NULL);
      CtlSetGraphics(GetObjectPointer(frm, ShapeFilledPush), ShapeRoundRectFilledBitmap, NULL);
      CtlSetGraphics(GetObjectPointer(frm, ShapeFilledXorPush), ShapeRoundRectFilledXorBitmap, NULL);
      CtlSetGraphics(GetObjectPointer(frm, ShapeOutlineXorPush), ShapeRoundRectOutlineXorBitmap, NULL);
      break;
    case ShapeCirclePush:
      CtlSetGraphics(GetObjectPointer(frm, ShapeOutlinePush), ShapeCircleBitmap, NULL);
      CtlSetGraphics(GetObjectPointer(frm, ShapeFilledPush), ShapeCircleFilledBitmap, NULL);
      CtlSetGraphics(GetObjectPointer(frm, ShapeFilledXorPush), ShapeCircleFilledXorBitmap, NULL);
      CtlSetGraphics(GetObjectPointer(frm, ShapeOutlineXorPush), ShapeCircleOutlineXorBitmap, NULL);
      break;
    case ShapeOvalPush:
      CtlSetGraphics(GetObjectPointer(frm, ShapeOutlinePush), ShapeOvalBitmap, NULL);
      CtlSetGraphics(GetObjectPointer(frm, ShapeFilledPush), ShapeOvalFilledBitmap, NULL);
      CtlSetGraphics(GetObjectPointer(frm, ShapeFilledXorPush), ShapeOvalFilledXorBitmap, NULL);
      CtlSetGraphics(GetObjectPointer(frm, ShapeOutlineXorPush), ShapeOvalOutlineXorBitmap, NULL);
      break;

    default:
      break;
    }
    break;

  default:
    break;
  }

  return handled;
}

/*
** Handles events for my custom gadgets in DoShape
*/
static Boolean ShapeGadgetEvent(FormGadgetTypeInCallback* gadgetP, UInt16 cmd, void* paramP) {
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
    {
      EventType* e = (EventType*) paramP;
      if (e->eType == frmGadgetEnterEvent) {
    if (DynBtnTrackPen(btn)) {
      /* Handle the selection */
      DialogButtonDrop(btn->id);

      if (btn->id == InkButton)
        DrawInkLabelDynamic(btn->content.bmpW, 0, 0, 0, d.inkList[d.tmp_ink]);
      else
        DrawPenLabelDynamic(btn->content.bmpW, 1, 2, 0, d.penpixList[d.tmp_pen]);

      DynBtnDraw(btn);
    }
      }

      handled = true;
    } break;
  }

  return handled;
}

/*
** DoShapePopup - Popup the Insert Shape form
*/
static void DoShapePopup(void) {
  FormPtr frm;
  UInt16 button_pressed;
  Err err = 0;
  DynamicButtonType* btn;

  /* Initialize */
  frm = FrmInitForm(ShapeForm);

  /* Set the push button group selection for the shape */
  FrmSetControlGroupSelection(frm, 1, ShapeRectPush + p.shape_type);
  /* Set the push button group selection for the style */
  FrmSetControlGroupSelection(frm, 2, ShapeOutlinePush + p.shape_fill);

  /* Modified "PriorModal()" */
  FlushToDisk(true);
  d.alarm_deleted_current = false;
  d.alarm_deleted_any = false;
  FrmSetEventHandler(frm, ShapeEvent);
  d.formID = ShapeForm;

  /* Initialize buffers for pen & ink */
  d.tmp_pen = p.shape_pen;
  d.tmp_ink = p.shape_ink;

  /* Initialize gadgets */
  /* Setup PenButton    */
  btn = DynBtnInitGadget(PenButton, rectangleFrame, true, true, false, d.sonyClie,
             d.sonyHRRefNum, d.hires, dynBtnGraphical, 0, &err, frm, 
             ShapeGadgetEvent);
  if (err) abort();
  DrawPenLabelDynamic(btn->content.bmpW, 1, 2, 0, d.penpixList[d.tmp_pen]);

  /* Setup InkButton */
  btn = DynBtnInitGadget(InkButton, rectangleFrame, true, true, false, d.sonyClie,
             d.sonyHRRefNum, d.hires, dynBtnGraphical, 0, &err, frm, 
             ShapeGadgetEvent);
  if (err) abort();
  DrawInkLabelDynamic(btn->content.bmpW, 0, 0, 0, d.inkList[d.tmp_ink]);

  /* Show the form */
  button_pressed = FrmDoDialog(frm);
  if (PostModal(frm)) return;

  if (ShapeInsertButton == button_pressed) {
    /* Set shape info based on push button selection */
    p.shape_type = FrmGetControlGroupSelection(frm, 1) - FrmGetObjectIndex(frm, ShapeRectPush);
    if (p.shape_type == -1) p.shape_type = 0;
    /* Set fill info based on push button selection */
    p.shape_fill = FrmGetControlGroupSelection(frm, 2) - FrmGetObjectIndex(frm, ShapeOutlinePush);
    if (p.shape_fill == -1) p.shape_fill = 0;
    /* Enter shape mode */
    EvtFlushPenQueue();  /* Pen queue must be empty */
    d.drawmode = dbModeShape;
    d.last_insert = d.drawmode;

    /* temporary load selected pen/shape and store old values */
    p.shape_pen = d.tmp_pen;
    d.tmp_pen = p.penSelection;
    LoadPen(p.shape_pen);
    p.shape_ink = d.tmp_ink;
    d.tmp_ink = p.inkSelection;
    LoadInk(p.shape_ink);
  }

  /* Clean up */
  FrmDeleteForm(frm);
}

/*
** CustomListDrawFunc - draw the numbers in the custom time form
*/
static void CustomListDrawFunc(Int16 item, RectangleType* b, Char** text) {
  Char str[6];

  StrIToA(str, item);
  WinDrawChars(str, StrLen(str), b->topLeft.x, b->topLeft.y);
}

/*
** DoCustomTimePopup - Pop up the custom time for weird people
*/
static UInt32 DoCustomTimePopup(void) {
  FormPtr frm;
  ListPtr list;
  UInt16 button_pressed;
  UInt16 list_selection;
  UInt32 ret;

  frm = FrmInitForm(CustomTimeForm);

  /* Initialize the lists */
  list = GetObjectPointer(frm, CustomHourList);
  LstSetListChoices(list, NULL, 24);
  LstSetDrawFunction(list, CustomListDrawFunc);
  LstSetSelection(list, 0);
  list = GetObjectPointer(frm, CustomMinuteList);
  LstSetListChoices(list, NULL, 60);
  LstSetDrawFunction(list, CustomListDrawFunc);
  LstSetSelection(list, 0);
  list = GetObjectPointer(frm, CustomSecondList);
  LstSetListChoices(list, NULL, 60);
  LstSetDrawFunction(list, CustomListDrawFunc);
  LstSetSelection(list, 0);

  PriorModal(CustomTimeForm);
  button_pressed = FrmDoDialog(frm);
  if (PostModal(frm)) return 0;

  /* Get the values from the form */
  if (CustomOkButton == button_pressed) {
    ret = 0;
    list = GetObjectPointer(frm, CustomHourList);
    list_selection = LstGetSelection(list);
    if (list_selection != -1);
    ret += 60L * 60L * list_selection;
    list = GetObjectPointer(frm, CustomMinuteList);
    list_selection = LstGetSelection(list);
    if (list_selection != -1);
    ret += 60L * list_selection;
    list = GetObjectPointer(frm, CustomSecondList);
    list_selection = LstGetSelection(list);
    if (list_selection != -1);
    ret += list_selection;
  }
  else ret = 0;

  /* Cleanup */
  FrmDeleteForm(frm);
  return ret;
}

/*
** Load a pen pattern
*/
static void LoadPen(Int16 selection) {
  StrCopy(p.penpix, d.penpixList[selection]);
  if (p.modeSelection == dbmSelectionErase)
    p.erasePenSelection = selection;
  else
    p.penSelection = selection;
  d.undo_mark = true;

  /* Update button if necessary */
  if (d.formID == DiddleTForm) {
    DynamicButtonType* btn;
    FormType* frm = FrmGetFormPtr(DiddleTForm); if (!frm) return;

    btn = (DynamicButtonType*)FrmGetGadgetData(frm, FrmGetObjectIndex(frm, PenButton));
    DrawPenLabelDynamic(btn->content.bmpW, 0, 0, 4, p.penpix);
    DynBtnDraw(btn);
  }
}

/*
** Random integer in the range 0..(n-1).
*/
extern inline UInt16 RandomLessThan(UInt16 n) {
  return SysRandom(0) / (1+sysRandomMax/n);
}

/*
** Load an ink pattern, but don't update the buttons & stuff
*/
static void LoadInkNoUpdate(Int16 selection) {
  if (selection != dbInkRandom) {
    /* Load the specified ink pattern */
    p.inkSelection = selection;
    MemMove(&p.inkpat, d.inkList[selection], sizeof(CustomPatternType));
  } else {
    UInt16 i, id, im, ib, j, jd, jm, jb;

    SysRandom(((Int32 *)p.inkpat)[0]+((Int32 *)p.inkpat)[1]);
    for (i = 0; i < 64; i += 1) {
      j = i + RandomLessThan(64-i);
      ib = p.inkpat[id = i/8] & (im = 1<<(i%8));
      jb = p.inkpat[jd = j/8] & (jm = 1<<(j%8));
      if (ib)
    p.inkpat[jd] |= jm;
      else
    p.inkpat[jd] &= ~jm;
      if (jb)
    p.inkpat[id] |= im;
      else
    p.inkpat[id] &= ~im;
    }
  }

  /* Actually make the pattern active */
  WinSetPattern((const CustomPatternType*) &p.inkpat);
}

/*
** Load an ink pattern
*/
static void LoadInk(Int16 selection) {
  LoadInkNoUpdate(selection);
  if (p.flags&PFLAGS_INK_ARTIST && d.drawmode == dbModeDoodle)
    LoadInkNoUpdate(dbInkRandom);

  SetPaintMode(dbmSelectionPaint, p.penSelection, p.filtSelection);
  d.undo_mark = true;

  /* Update button if necessary */
  if (d.formID == DiddleTForm) {
    DynamicButtonType* btn;
    FormType* frm = FrmGetFormPtr(DiddleTForm); if (!frm) return;

    btn = (DynamicButtonType*)FrmGetGadgetData(frm, FrmGetObjectIndex(frm, InkButton));
    DrawInkLabelDynamic(btn->content.bmpW, 0, 0, 4, (const CustomPatternType*) &p.inkpat);
    DynBtnDraw(btn);
  }
}

/*
** Switches back to doodle mode and does the necessary cleanup
*/
void SwitchToDoodleMode(void) {
  /* Ensure we are not in the native coordinate system */
  WinPushDrawState();
  SwitchToLoRes();
  
  if (d.drawmode == dbModeShape) {
    LoadInk(d.tmp_ink);
    LoadPen(d.tmp_pen);
  }

  /* Reset the draw mode */
  d.drawmode = dbModeDoodle;

  /* Clean up */
  WinPopDrawState();
}

/*
** LockAsk - Ask the user if the sketch should be unlocked
*/
static Boolean LockAsk(void) {

  if (!recordIsLocked) return false;

  /* SavImage();  alarm insurance */
  FlushToBuffer();
  if (!FrmAlert(LockAskForm)) {
    recordUnlock();
    FlushToDisk(false);
    LabelAlarm();
    if (d.os4) FrmSetMenu(FrmGetActiveForm(), DiddleMenu);
    UpdateLockButton();
  }
  else SwitchToDoodleMode();
  d.lock_count = 0;

  return true;
}

/*
** LockWarn - Beep at the user and increment the lock count
*/
static void LockWarn(void) {
  if (d.lock_count < 2) {
    SndPlaySystemSound(sndWarning);
    d.lock_count++;
  }
  else {
    LockAsk();
    EvtFlushPenQueue();  /* Leftover penMoves and penUps */
  }
}

/*
** ShortCutPop - handle selection of shortcut menu
*/
static void ShortCutPop(ShortCutLocationType loc) {
  FormType* frm = FrmGetFormPtr(p.formID);
  const Int16 idx = FrmGetObjectIndex(frm, PopShortcutList);
  ListType* list = FrmGetObjectPtr(frm, idx);
  Int16 selection;
  RectangleType r;

  FrmGetObjectBounds(frm, idx, &r);
  r.extent.x = d.shortcut_width;
  FrmSetObjectBounds(frm, idx, &r);
  if (loc == scLow) LstSetPosition(list, 157 - d.shortcut_width, 97);
  else LstSetPosition(list, 3, 15);
  LstSetSelection(list, -1);

  selection = LstPopupList(list);
  if (-1 == selection) return;

  switch (selection) {
  case scLine:
    EvtFlushPenQueue();
    d.last_insert = d.drawmode = dbModeLine;
    return;

  case scRectangle:
    p.shape_type = dbShapeRectangle;
    break;

  case scRoundedRectangle:
    p.shape_type = dbShapeRoundedRectangle;
    break;

  case scOval:
    p.shape_type = dbShapeOval;
    break;

  case scCircle:
    p.shape_type = dbShapeCircle;
    break;

  case scFill:
    EvtFlushPenQueue();
    d.last_insert = d.drawmode = dbModeFloodFill;
    return;

  case scText:
    DoTextPopup();
    return;

  default:
    /* ignore */
  }

  /* Handle the common code for shape events here */
  EvtFlushPenQueue();  /* Pen queue must be empty */
  d.last_insert = d.drawmode = dbModeShape;

  /* temporary load selected pen/shape and store old values */
  d.tmp_pen = p.penSelection;
  d.tmp_ink = p.inkSelection;
}

/*
** DoCmdRemove
*/
void DoCmdRemove(void) {
  const UInt16 oldDbI = p.dbI;

  FlushToDisk(true);
  PRINT("DoCmdRemove... %hd of %hd", p.dbI, DmNumRecords(d.dbR));

  if (recordIsLocked) {
    if (!FrmAlert(ConfirmLockedDelete))
      RemoveImage();
  } else if (p.flags&PFLAGS_CONFIRM_DELETE) {
    if (SketchIsEmpty(DmQueryRecord(d.dbR, p.dbI)) ||
    (FrmAlert(ConfirmDeleteForm) == 0)) {
      p.dbI = oldDbI; /* Mysteriously p.dbI gets overwritten during FrmAlert when called from ThumbnailDetailForm */
      RemoveImage();
    } else {
      p.dbI = oldDbI; /* Mysteriously p.dbI gets overwritten during FrmAlert when called from ThumbnailDetailForm */
    }
  } else {
    RemoveImage();
  }
}

/*
** ScrollXferField
*/
static void ScrollXferField(WinDirectionType direction) {
  FormPtr frm;
  FieldPtr fld;

  frm = FrmGetActiveForm();
  fld = GetObjectPointer(frm, XferField);
  if (FldScrollable(fld, direction)) {
    FldScrollField(fld, 1, direction);
  }
  else {
    SndPlaySystemSound(sndWarning);
  }
}

/*
** Set a paint mode
*/
static void SetPaintMode(Int16 mode, Int16 penSel, Int16 filtSel) {
  p.modeSelection = mode;
  LoadPen(penSel);
  SetFilterMode(filtSel);

  FlushToBuffer();

  /* Update button if necessary */
  if (d.formID == DiddleTForm)
    CtlSetGraphics(GetObjectPointer(FrmGetFormPtr(DiddleTForm), ModeButton), CmdPaintBitmap + mode, NULL);
}

/*
** Set a filter mode
*/
static void SetFilterMode(Int16 mode) {
  switch (mode) {
  case dbFilterRough:
    p.filt = 1;
    break;

  case dbFilterSmooth:
    p.filt = 8;
    break;
  case dbFilterSmoother:
    p.filt = 16;
    break;

  default:
    abort(); /* should never happen */
  }

  if (p.modeSelection == dbmSelectionErase)
    ep.eraseFiltSelection = mode;
  else
    p.filtSelection = mode;

  FlushToBuffer();

  /* Update button if necessary */
  if (d.formID == DiddleTForm)
     CtlSetGraphics(GetObjectPointer(FrmGetFormPtr(DiddleTForm), SmoothButton), CmdRoughBitmap + mode, NULL);
}

/*
** (Conditionally) initialize MIDI-list
**
** The width of the bounds rectangle is adjusted as necessary,
** if the parameter is non-null.
*/
void InitMIDIList(RectangleType* bounds) {
  UInt16 i = 0;
  Int16 width = 0, offset = 2;

  if (!d.midi_list)
    SndCreateMidiList(0, true, &d.midi_count, &d.midi_list);

  /* Calculate width of MIDI popup list */
  if (bounds && d.midi_count > 0) {
    SndMidiListItemType* midi = MemHandleLock(d.midi_list);

    for (; i < d.midi_count; ++i)
      width = Max(width, FntCharsWidth(midi[i].name, StrLen(midi[i].name)));

    if (d.midi_count > 4) {
      const FontID font = FntSetFont(symbolFont);
      offset += FntCharWidth(symbolSmallUpArrow);
      FntSetFont(font);
    }

    bounds->extent.x = Min(width + offset, 120); /* max width of 120 */

    MemHandleUnlock(d.midi_list);
  }
}

/*
** Increase or decrease index WITHIN a category
*/
static void ChangeIndexInCategory(UInt16 amount, Boolean forward) {
  UInt16 n = DmNumRecords(d.dbR);
  Int8 i;
  UInt16 idx = p.dbI;
  Err err;

  for (i = 0; i < amount; ++i) {
    err = DmSeekRecordInCategory(d.dbR, &idx, 1,
                 forward ? dmSeekForward : dmSeekBackward, p.category);

    if (err == dmErrSeekFailed) {
      idx = forward ? 0 : n;
      err = DmSeekRecordInCategory(d.dbR, &idx, 0,
                   forward ? dmSeekForward : dmSeekBackward, p.category);
    }

    if (err == errNone)
      p.dbI = idx;
  }
}

/*
** Handle Security command
*/
void DoCmdSecurity(void) {
  Boolean wasHiding = d.privateRecordStatus == hidePrivateRecords;
  UInt16 mode;

  d.privateRecordStatus = SecSelectViewStatus();

  if (wasHiding ^ (d.privateRecordStatus == hidePrivateRecords)) {
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

    if (FrmGetActiveFormID() == DiddleTForm
    && (!d.record_private || d.privateRecordStatus != hidePrivateRecords)) {
      UpdatePageButton();
    }
  }

  if (d.record_private) {
    switch (d.privateRecordStatus) {
    case maskPrivateRecords:
    case showPrivateRecords:
      LoadFromDisk();
      break;

    case hidePrivateRecords:
      SetTopVisibleRecord(0);
      p.dbI = noRecordSelected;
      FrmGotoForm(ep.listFormID);
      break;
    }
  }
}

/*
** Set the drawing area rectangle correctly.
*/
void ResetDrawingAreaRectangle(Boolean withTitle, Boolean withToolbar) {
  if (withToolbar) {
    if (withTitle) {
      RctSetRectangle(&p.r,
              screen_above_bar_rect.topLeft.x,
              screen_above_bar_rect.topLeft.y + title_bar_rect.extent.y,
              screen_above_bar_rect.extent.x,
              screen_above_bar_rect.extent.y - title_bar_rect.extent.y);
    } else {
      RctCopyRectangle(&screen_above_bar_rect, &p.r);
    }
  } else {
    if (withTitle) {
      RctSetRectangle(&p.r,
              screen_above_bar_rect.topLeft.x,
              screen_above_bar_rect.topLeft.y + title_bar_rect.extent.y,
              screen_above_bar_rect.extent.x,
              screen_above_bar_rect.extent.y - title_bar_rect.extent.y + bottom_bar_rect.extent.y);
    } else {
      RctSetRectangle(&p.r,
              screen_above_bar_rect.topLeft.x,
              screen_above_bar_rect.topLeft.y,
              screen_above_bar_rect.extent.x,
              screen_above_bar_rect.extent.y + bottom_bar_rect.extent.y);
    }
  }

  /* Update hi-res rectangle */
  if (d.hires) {
    if (!d.sonyClie) {
      if (WinGetCoordinateSystem() != kCoordinatesStandard) {
    RctCopyRectangle(&p.r, &d.r_hires);
    WinScaleRectangle(&d.r_hires);
      }
    } else {
      RctCopyRectangle(&p.r, &d.r_hires);
      d.r_hires.extent.x *= HiResCoef;
      d.r_hires.extent.y *= HiResCoef;
      d.r_hires.topLeft.x *= HiResCoef;
      d.r_hires.topLeft.y *= HiResCoef;
    }
  }
}

/*
** Switch the bottom toolbar on or off.
*/
static void ToggleToolbar(Boolean on) {
  const UInt16 id = FrmGetActiveFormID();
  FormType* frm = FrmGetActiveForm();

  ToggleButtonBar(frm, on);

  /* Save drawing */
  FlushToDisk(true);

  ResetDrawingAreaRectangle(id == DiddleTForm, on);

  /* Update menu and screen */
  if (d.os4) FrmSetMenu(frm, DiddleMenu);
  LoadFromDisk();
}

/*
** Act on a key (or menu event) received.
*/
Boolean KeyDown(UInt16 chr) {
  Boolean res = false;

  /* Do not capture keystrokes when in transfer mode */
  if (d.is_xfer_mode) {
    switch (chr) {
    case pageUpChr:
      ScrollXferField(winUp);
      return true;
      break;
    case pageDownChr:
      ScrollXferField(winDown);
      return true;
      break;
    }
    return false;
  }

  /* Map command character to lower case */
  if (chr >= 'A' && chr <= 'Z')
    chr += 'a'-'A';

  /* Dispatch on character */
  switch (chr) {

  case penFine:
    LoadPen(dbPenFine);
    res = true;
    break;
  case penMedium:
    LoadPen(dbPenMedium);
    res = true;
    break;
  case penBold:
    LoadPen(dbPenBold);
    res = true;
    break;
  case penBroad:
    LoadPen(dbPenBroad);
    res = true;
    break;
  case penFineItalic:
    LoadPen(dbPenFineItalic);
    res = true;
    break;
  case penMediumItalic:
    LoadPen(dbPenMediumItalic);
    res = true;
    break;
  case penBroadItalic:
    LoadPen(dbPenBroadItalic);
    res = true;
    break;

  case inkWhite:
    LoadInk(dbInkWhite);
    res = true;
    break;
  case inkDith12:
    LoadInk(dbInk1_8);
    res = true;
    break;
  case inkDith25:
    LoadInk(dbInk2_8);
    res = true;
    break;
  case inkDith37:
    LoadInk(dbInk3_8);
    res = true;
    break;
  case inkDith50:
    LoadInk(dbInk4_8);
    res = true;
    break;
  case inkDith62:
    LoadInk(dbInk5_8);
    res = true;
    break;
  case inkDith75:
    LoadInk(dbInk6_8);
    res = true;
    break;
  case inkDith87:
    LoadInk(dbInk7_8);
    res = true;
    break;
  case inkBlack:
    LoadInk(dbInkBlack);
    res = true;
    break;
  case inkRandom:
    LoadInk(dbInkRandom);
    res = true;
    break;

  case modePaint:
    SetPaintMode(dbmSelectionPaint, p.penSelection, p.filtSelection);
    res = true;
    break;
  case modeErase:
    SetPaintMode(dbmSelectionErase, p.erasePenSelection, ep.eraseFiltSelection);
    res = true;
    break;

  case cmdRough:
    SetFilterMode(dbFilterRough);
    res = true;
    break;
  case cmdSmooth:
    SetFilterMode(dbFilterSmooth);
    res = true;
    break;
  case cmdSmoother:
    SetFilterMode(dbFilterSmoother);
    res = true;
    break;

  case cmdPref:
    FrmGotoForm(PrefForm);
    res = true;
    break;

  case cmdExtPref:
    FrmGotoForm(PrefExtForm);
    res = true;
    break;

  case cmdHWPref:
    FrmGotoForm(PrefHWForm);
    res = true;
    break;

  case cmdAlarmPref:
    DoAlarmPreferences();
    res = true;
    break;

  case cmdHelp:
    {
      UInt16 id;

      switch(p.formID) {
      case DiddleListForm:
    id = DiddleListHelpString;
    break;

      case DiddleThumbnailForm:
    id = DiddleThumbnailHelpString;
    break;

      case DiddleThumbnailDetailForm:
    id = DiddleThumbnailDetailHelpString;
    break;

      default:
    id = DiddleBugHelpString;
      }

      FrmHelp(id);
    }
    res = true;
    break;

  case cmdTitle:
    p.flags ^= PFLAGS_WITH_TITLEBAR;
    FrmGotoForm(p.flags & PFLAGS_WITH_TITLEBAR ? DiddleTForm : DiddleForm);
    res = true;
    break;

  case cmdToolbar:
    p.flags ^= PFLAGS_WITHOUT_TOOLBAR;
    ToggleToolbar(p.flags & PFLAGS_WITHOUT_TOOLBAR ? false : true);
    res = true;
    break;

  case cmdClear:
    LockAsk();
    if (!recordIsLocked && (!(p.flags&PFLAGS_CONFIRM_CLEAR) || !FrmAlert(ConfirmClearForm)))
      ClearImage();
    res = true;
    break;

  case cmdFill:
    LockAsk();
    if (!recordIsLocked && (!(p.flags&PFLAGS_CONFIRM_CLEAR) || !FrmAlert(ConfirmFillForm)))
      FillImage();
    res = true;
    break;

  case cmdInvert:
    LockAsk();
    if (!recordIsLocked)
      InvertImage();
    res = true;
    break;

  case cmdSketchCut:
    LockAsk();
    if (!recordIsLocked) {
      EvtFlushPenQueue();
      d.drawmode = dbModeCut;
    }
    res = true;
    break;

  case cmdSketchCopy:
    EvtFlushPenQueue();
    d.drawmode = dbModeCopy;
    res = true;
    break;

  case cmdSketchPaste:
    LockAsk();
    if (!recordIsLocked) {
      EvtFlushPenQueue();
      d.drawmode = dbModePaste;
      d.last_insert = d.drawmode;
    }
    res = true;
    break;

  case cmdSketchUndo:
    if (recordIsDirty) {
      LockAsk();
      if (!recordIsLocked && (!(p.flags&PFLAGS_CONFIRM_CLEAR) || !FrmAlert(ConfirmUndoForm)))
    LoadFromBuffer();
    }
    else
      FlashWaitMessage(NoUndoString);
    res = true;
    break;

  case cmdText:
    LockAsk();
    if (!recordIsLocked)
      DoTextPopup();
    res = true;
    break;

  case cmdLine:
    LockAsk();
    if (!recordIsLocked) {
      EvtFlushPenQueue();
      d.drawmode = dbModeLine;
      d.last_insert = d.drawmode;
    }
    res = true;
    break;

  case cmdShape:
    LockAsk();
    if (!recordIsLocked) {
      FlushToBuffer();
      DoShapePopup();
    }
    res = true;
    break;

  case cmdInsertLast:
    LockAsk();
    if (!recordIsLocked) {
      EvtFlushPenQueue();
      d.drawmode = d.last_insert;

      switch (d.drawmode) {
      case dbModeShape:
    /* temporary load selected pen/shape and store old values */
    d.tmp_pen = p.penSelection;
    LoadPen(p.shape_pen);
    d.tmp_ink = p.inkSelection;
    LoadInk(p.shape_ink);
    break;

      case dbModeText:
    d.txt_oldfont = FntSetFont(p.txt_font);
    break;

      default:
    /* nothing */
      }

      SndPlaySystemSound(sndClick);
    }
    res = true;
    break;

  case cmdNew:
    NewSketch();
    res = true;
    break;

  case cmdDuplicate:
    DuplicateSketch();
    res = true;
    break;

  case cmdDetails:
    if (d.record_private && d.privateRecordStatus == maskPrivateRecords && !UnlockMaskedRecords())
      break;

    FlushToDisk(true);
    d.detailsWithSketch = true;
    FrmPopupForm(RecordDetailsForm);
    //    DoDetailsDialog(true);
    res = true;
    break;

  case cmdListView:
    FrmGotoForm(ep.listFormID);
    res = true;
    break;

  case cmdSecurity:
    DoCmdSecurity();
    res = true;
    break;

  case cmdFloodFill:
    d.drawmode = dbModeFloodFill;
    d.last_insert = d.drawmode;
    res = true;
    break;

  case cmdLock:
    SndPlaySystemSound(sndClick);
    if (!recordIsLocked) {
      recordLock();
      d.lock_count = 0;
      FlushToDisk(true);
      if (d.os4) FrmSetMenu(FrmGetActiveForm(), DiddleMenu);
      UpdateLockButton();
    } else {
      LockAsk();
    }
    res = true;
    break;

  case cmdRemove:
    if (!d.record_private || d.privateRecordStatus == showPrivateRecords ||
    d.unmaskedCurrentRecord || UnlockMaskedRecords())
      DoCmdRemove();
    res = true;
    break;

  case chrBackspace:
  case cmdPrevious:
  case prevFieldChr:
  case pageUpChr:
    SndPlaySystemSound(sndClick);
    FlushToDisk(true);
    ChangeIndexInCategory(1, false);
    LoadFromDisk();
    res = true;
    break;

  case chrSpace:
  case cmdNext:
  case nextFieldChr:
  case pageDownChr:
    SndPlaySystemSound(sndClick);
    FlushToDisk(true);
    ChangeIndexInCategory(1, true);
    LoadFromDisk();
    res = true;
    break;

  case cmdNextCategory:
    {
      UInt16 old_cat = p.category;
      p.category = CategoryGetNext(d.dbR, p.category);

      SndPlaySystemSound(sndClick);
      SwitchCategory(old_cat, FrmGetActiveForm());
    }
    res = true;
    break;

  case cmdTransfer:
    if (!d.record_private || d.privateRecordStatus == showPrivateRecords || UnlockMaskedRecords()) {
      SndPlaySystemSound(sndClick);
      StartXferMode();
    }
    res = true;
    break;

  case cmdPopupCountdownList:
    {
      RectangleType r;
      FormType* frm = FrmGetActiveForm();

      FrmGetObjectBounds(frm, FrmGetObjectIndex(frm, MainDateSelTrig), &r);

      DoDateSelect(r.topLeft.x + r.extent.x / 2);
    }
    res = true;
    break;

  case sendDataChr:
  case cmdBeam:
    BeamSend();
    res = true;
    break;

  case cmdSend:
    SendCurrentRecord();
    res = true;
    break;
  }

  LabelAlarm();

  return res;
}

/*
** BDropDrawFunc
*/
static void BDropDrawFunc(Int16 item, RectangleType* b, Char** text) {

  if (d.dropList.type == DROPLIST_TYPE_BITMAP)
    DrawBitmapLabel(d.dropList.iconRsc+item, b->topLeft.x, b->topLeft.y+1);
  else if (d.dropList.type == DROPLIST_TYPE_PEN)
    DrawPenLabel(d.penpixList[item], b->topLeft.x, b->topLeft.y+1);
  else if (d.dropList.type == DROPLIST_TYPE_INK)
    DrawInkLabel(d.inkList[item], b->topLeft.x, b->topLeft.y+1);

  /* Draw the text label on correct pos (* 1.5 for HandEra) */
  WinDrawChars(text[item], StrLen(text[item]), b->topLeft.x + handera(17), b->topLeft.y);
}

/*
** ButtonDrop - show drop-down list for titlebar buttons
*/
static Boolean ButtonDrop(UInt16 ctlID) {
  FormPtr frm = FrmGetActiveForm();
  ListPtr list;
  UInt16 button_num;
  Int16 list_selection;
  Int16 list_left_edge;
  Int16 current_selection;
  const UInt16* droplist;
  UInt16 listRsc;
  RectangleType r;

  switch (ctlID) {
  case SmoothButton:
    listRsc = MainSmoothList;
    d.dropList.type = DROPLIST_TYPE_BITMAP;
    d.dropList.iconRsc = CmdRoughBitmap;
    droplist = smoothDropList;
    if (p.modeSelection == dbmSelectionErase)
      current_selection = ep.eraseFiltSelection;
    else
      current_selection = p.filtSelection;
    button_num = 1;
    break;
  case PenButton:
    listRsc = MainPenList;
    d.dropList.type = DROPLIST_TYPE_PEN;
    droplist = penDropList;
    if (p.modeSelection == dbmSelectionErase)
      current_selection = p.erasePenSelection;
    else
      current_selection = p.penSelection;
    button_num = 2;
    break;
  case InkButton:
    listRsc = MainInkList;
    d.dropList.type = DROPLIST_TYPE_INK;
    droplist = inkDropList;
    current_selection = p.inkSelection;
    button_num = 3;
    break;
  default:
    return false;
    break;
  }
  /* Set the list width, location and selection */
  list = GetObjectPointer(frm, listRsc);
  LstSetDrawFunction(list, BDropDrawFunc);
  FrmGetObjectBounds(frm, FrmGetObjectIndex(frm, ctlID), &r);
  list_left_edge = r.topLeft.x - 2;
  if (!d.dropList.dropListWidth[button_num])
    d.dropList.dropListWidth[button_num] = GetListWidth(list)+15;
  if (d.dropList.dropListWidth[button_num] > (159-list_left_edge))
    list_left_edge = 159-d.dropList.dropListWidth[button_num];
  FrmGetObjectBounds(frm, GetObjectIndexFromPointer(frm, list), &r);
  r.extent.x = d.dropList.dropListWidth[button_num];
  FrmSetObjectBounds(frm, GetObjectIndexFromPointer(frm, list), &r);
  LstSetPosition(list, list_left_edge, 14);
  LstSetSelection(list, current_selection);

  /* Popup the list and handle the result */
  list_selection = LstPopupList(list);
  if (list_selection != -1) {
    return KeyDown(droplist[list_selection]);
  }

  return false;
}

/*
** DialogButtonDrop - show drop-winDown list for dialog buttons
*/
static void DialogButtonDrop(UInt16 ctlID) {
  FormPtr frm = FrmGetActiveForm();
  ListPtr list;
  UInt16 button_num;
  Int16 list_selection;
  Int16 list_left_edge;
  Int16 current_selection;
  const UInt16* droplist;
  UInt16 listRsc;
  RectangleType r;

  switch (ctlID) {
  case PenButton:
    listRsc = MainPenList;
    d.dropList.type = DROPLIST_TYPE_PEN;
    droplist = penDropList;
    current_selection = d.tmp_pen;
    button_num = 2;
    break;
  case InkButton:
    listRsc = MainInkList;
    d.dropList.type = DROPLIST_TYPE_INK;
    droplist = inkDropList;
    current_selection = d.tmp_ink;
    button_num = 3;
    break;
  default:
    return;
  }
  /* Set the list width, location and selection */
  list = GetObjectPointer(frm, listRsc);
  LstSetDrawFunction(list, BDropDrawFunc);
  FrmGetObjectBounds(frm, FrmGetObjectIndex(frm, ctlID), &r);
  list_left_edge = r.topLeft.x;
  if (!d.dropList.dropListWidth[button_num])
    d.dropList.dropListWidth[button_num] = GetListWidth(list)+15;
  if (d.dropList.dropListWidth[button_num] > (159-list_left_edge))
    list_left_edge = 159-d.dropList.dropListWidth[button_num];
  FrmGetObjectBounds(frm, GetObjectIndexFromPointer(frm, list), &r);
  r.extent.x = d.dropList.dropListWidth[button_num];
  FrmSetObjectBounds(frm, GetObjectIndexFromPointer(frm, list), &r);
  LstSetPosition(list, list_left_edge, r.topLeft.y);
  LstSetSelection(list, current_selection);
  /* Popup the list and handle the result */
  list_selection = LstPopupList(list);
  if (list_selection != -1) {
    switch (droplist[list_selection]) {
    case penFine:       d.tmp_pen=dbPenFine; break;
    case penMedium:     d.tmp_pen=dbPenMedium; break;
    case penBold:       d.tmp_pen=dbPenBold; break;
    case penBroad:      d.tmp_pen=dbPenBroad; break;
    case penFineItalic:     d.tmp_pen=dbPenFineItalic; break;
    case penMediumItalic:   d.tmp_pen=dbPenMediumItalic; break;
    case penBroadItalic:    d.tmp_pen=dbPenBroadItalic; break;

    case inkWhite:  d.tmp_ink=dbInkWhite; break;
    case inkDith12: d.tmp_ink=dbInk1_8; break;
    case inkDith25: d.tmp_ink=dbInk2_8; break;
    case inkDith37: d.tmp_ink=dbInk3_8; break;
    case inkDith50: d.tmp_ink=dbInk4_8; break;
    case inkDith62: d.tmp_ink=dbInk5_8; break;
    case inkDith75: d.tmp_ink=dbInk6_8; break;
    case inkDith87: d.tmp_ink=dbInk7_8; break;
    case inkBlack:  d.tmp_ink=dbInkBlack; break;
    case inkRandom: d.tmp_ink=dbInkRandom; break;
    }
  }
}


/*
** Clear the pen state.
*/
extern inline void DiddlePenClear(void) {
  d.nwts = d.wx = d.wy = 0;
  MemSet(d.wts, sizeof(d.wts), 0);
}

/*
** Mirror the given rectangle of the real canvas onto the screen
** using the user-specified ink and paper color.
*/
void MirrorPartialRealCanvas(const RectangleType* r, Boolean clip) {
  const IndexedColorType oldForeColor = WinSetForeColor(ep.inkColor);
  const IndexedColorType oldBackColor = WinSetBackColor(ep.paperColor);
  
  if (clip)
    SetDrawingAreaClip(d.winM);

  if (!d.sonyClie || !d.hires)
    WinCopyRectangle(d.realCanvas, d.winM, r, r->topLeft.x, r->topLeft.y, winPaint);
  else
    HRWinCopyRectangle(d.sonyHRRefNum, d.realCanvas, d.winM, (RectangleType*) r,
               r->topLeft.x, r->topLeft.y, winPaint);

  /* Clean up */
  WinSetBackColor(oldBackColor);
  WinSetForeColor(oldForeColor);
  if (clip)
    ResetDrawingAreaClip(d.winM);
}

/*
** Act on a diddle pen event.
*/
static void DiddlePenEvent(Coord nx, Coord ny) {
  register Int16 i;
  register Coord dx, dy, lx, rx, ty, by;
  register Coord x1, y1, x2, y2;
  RectangleType r;
  WinHandle oldH;
  RectangleType* my_r;
  UInt16 oldCoords = 0;
  WinLineType lines[NPENPIX];
  UInt16 used = 0;

  /* Save old draw window */
  oldH = WinSetDrawWindow(d.realCanvas);

  /* Save old state */
  WinPushDrawState();

  /* Set correct background color so that pixels are off */
  WinSetBackColor(0);

  /* Switch to hi-res mode */
  if (d.hires) {
    my_r = &d.r_hires;
    oldCoords = SwitchToHiRes();
  } else {
    my_r = &p.r;
  }

  rx = lx = d.ox;
  ty = by = d.oy;

  /* Draw the lines defined by the pen */
  for (i = 0; i < NPENPIX && p.penpix[i] != 0; ++i) {
    dx = dxFromPenpix(p.penpix[i]);
    dy = dyFromPenpix(p.penpix[i]);

    x1 = d.ox + dx;
    y1 = d.oy + dy;
    x2 = nx + dx;
    y2 = ny + dy;

    if (!RctPtInRectangle(x1, y1, my_r))
      continue;
    if (!RctPtInRectangle(x2, y2, my_r))
      continue;

    /* Add to lines to draw */
    lines[used].x1 = x1;
    lines[used].y1 = y1;
    lines[used].x2 = x2;
    lines[used].y2 = y2;
    ++used;

    /* Calculate leftmost and rightmost x coordinate */
    lx = Min(lx, Min(x1, x2));
    rx = Max(rx, Max(x1, x2));

    /* Calculate top and bottom y coordinate */
    ty = Min(ty, Min(y1, y2));
    by = Max(by, Max(y1, y2));
  }
  
  /* Do the actual drawing */
  if (!d.sonyClie || !d.hires) 
    WinPaintLines(used, lines);
  else 
    HRWinPaintLines(d.sonyHRRefNum, used, lines);
  
  /* Show the "pen" while in eraser mode */
  if (p.modeSelection == dbmSelectionErase) {
    WinSetDrawWindow(d.winM);
    WinSetForeColor(UIColorGetTableEntryIndex(UIObjectSelectedFill));
    WinSetPatternType(blackPattern);

    if (!d.sonyClie || !d.hires) 
      WinPaintLines(used, lines);
    else 
      HRWinPaintLines(d.sonyHRRefNum, used, lines);
    WinSetDrawWindow(d.realCanvas);
  }

  /* Store new offsets */
  d.ox = nx;
  d.oy = ny;

  /* Restore old draw window - otherwise the palette handling gets in the way */
  WinSetDrawWindow(oldH);

  /* Mirror to screen */
  RctSetRectangle(&r, lx, ty, rx - lx + 1, by - ty + 1); 
  MirrorPartialRealCanvas(&r, false);

  /* Restore previous coordinate System */
  if (oldCoords) SwitchToPreviousCoordinates(oldCoords);

  /* Clean up */
  WinPopDrawState();
}

/*
** DoStroke - a starting or continuing pen stroke while doodling
*/
static void DoStroke(Coord x, Coord y) {
  UInt16 n;

  /* Randomize ink if in 'Artist' mode */
  if (p.modeSelection == dbmSelectionPaint && p.flags&PFLAGS_INK_ARTIST)
    LoadInkNoUpdate(dbInkRandom);

  /* Update sum of coordinates */
  n = (d.nwts-p.filt)%NPENWTS;
  d.wx += x - d.wts[n].x;
  d.wy += y - d.wts[n].y;

  /* Update array of coordinates */
  n = d.nwts%NPENWTS;
  d.wts[n].x = x;
  d.wts[n].y = y;

  /* Send averaged coordinate */
  n = Min(p.filt, d.nwts + 1);
  DiddlePenEvent(d.wx/n, d.wy/n);

  /* Update count of coordinates */
  d.nwts += 1;
}

/*
** EndStroke - end a pen stroke while doodling
*/
static void EndStroke(void) {
  Int16 i;
  UInt16 n;

  /* Drain queue */
  for (i = Min(d.nwts, p.filt); --i > 0; ) {
    n = (d.nwts-i-1)%NPENWTS;
    d.wx -= d.wts[n].x;
    d.wy -= d.wts[n].y;
    DiddlePenEvent(d.wx/i, d.wy/i);
  }

  /* Update the buttons if in 'Artist' mode */
  if (p.modeSelection == dbmSelectionPaint && p.flags&PFLAGS_INK_ARTIST)
    LoadInk(dbInkRandom);

  /* Clear state */
  DiddlePenClear();
}

/*
** CountdownStuff - Load the countdown list, or execute an action
*/
static void CountdownStuff(UInt16 listnum, Boolean action, Char* text) {
  UInt32 future_seconds = 0;
  UInt16 minutes = 0;
  UInt16 hours = 0;
  UInt32 days = 0; /* Big only to prevent compiler warning of integer overflow */
  Char tempStr[15];

  switch (listnum) {

  case 0: /* OFF */
    if (action) {
      d.record.alarmSecs = 0;
      d.record.snoozeSecs = 0;
      d.record.repeatInfo.repeatType = repeatNone;
      DateToInt(d.record.repeatInfo.repeatEndDate) = apptNoEndDate;
      d.record.repeatInfo.repeatFrequency = 0;
      d.record.repeatInfo.repeatOn = 0;

      if (recordIsAlarmSet) {
    recordUnsetAlarm();
    FlushToDisk(false);
    RescheduleAlarms(d.dbR);
      }
      /*       recordUnsetAlarm(); */
      LabelAlarm();
    }
    else SysCopyStringResource(text, OffString);
    break;
  case 1: /* Absolute... */
    if (action) {
      recordUnsetCountdown();
      FrmGotoForm(TimeForm);
    }
    else SysCopyStringResource(text, AbsoluteString);
    break;
  case 2: /* Custom... */
    if (action) {
      future_seconds = DoCustomTimePopup();
    }
    else SysCopyStringResource(text, CustomString);
    break;
  case 3: /* 1 min */
  case 4: /* 2 min */
  case 5: /* 3 min */
  case 6: /* 4 min */
  case 7: /* 5 min */
    minutes = listnum - 2;
    break;
  case 8: /* 10 min */
  case 9: /* 15 min */
  case 10: /* 20 min */
    minutes = 5 * (listnum - 6);
    break;
  case 11: /* 30 min */
    minutes = 30;
    break;
  case 12: /* 45 min */
    minutes = 45;
    break;
  case 13: /* 1 hr */
    hours = 1;
    break;
  case 14: /* 1.5 hr */
    if (action) minutes =  90;
    else {
      StrCopy(text, "1.5");
      SysCopyStringResource(tempStr, HoursString);
      StrNCat(text, tempStr, 14);
    }
    break;
  case 15: /* 2 hr */
  case 16: /* 3 hr */
  case 17: /* 4 hr */
  case 18: /* 5 hr */
    hours = listnum - 13;
    break;
  case 19: /* 8 hr */
    hours = 8;
    break;
  case 20: /* 10 hr */
    hours = 10;
    break;
  case 21: /* 12 hr */
    hours = 12;
    break;
  case 22: /* 1 day */
  case 23: /* 2 days */
  case 24: /* 3 days */
  case 25: /* 4 days */
  case 26: /* 5 days */
  case 27: /* 6 days */
    days = listnum - 21;
    break;
  case 28: /* 1 week */
  case 29: /* 2 weeks */
    if (action) days = (listnum - 27) * 7;
    else {
      if (listnum == 28) SysCopyStringResource(tempStr, WeekString);
      else SysCopyStringResource(tempStr, WeeksString);
      StrIToA(text, (listnum-27));
      StrNCat(text, tempStr, 14);
    }
    break;
  }
  if (minutes) {
    if (action) future_seconds = minutesInSeconds * minutes;
    else {
      StrIToA(text, minutes);
      if (minutes == 1) SysCopyStringResource(tempStr, MinString);
      else SysCopyStringResource(tempStr, MinsString);
      StrNCat(text, tempStr, 14);
    }
  }
  else if (hours) {
    if (action) future_seconds = hoursInSeconds * hours;
    else {
      StrIToA(text, hours);
      if (hours == 1) SysCopyStringResource(tempStr, HourString);
      else SysCopyStringResource(tempStr, HoursString);
      StrNCat(text, tempStr, 14);
    }
  }
  else if (days) {
    if (action) future_seconds = days * daysInSeconds;
    else {
      StrIToA(text, days);
      if (days == 1) SysCopyStringResource(tempStr, DayString);
      else SysCopyStringResource(tempStr, DaysString);
      StrNCat(text, tempStr, 14);
    }
  }
  if (action && future_seconds) {
    /* Set the alarm for future_seconds into the future */
    recordSetAlarm();
    recordSetCountdown(); /* belt and suspenders */
    d.record.repeatInfo.repeatType = repeatNone;
    DateToInt(d.record.repeatInfo.repeatEndDate) = apptNoEndDate;
    d.record.repeatInfo.repeatFrequency = 0;
    d.record.repeatInfo.repeatOn = 0;
    d.record.alarmSecs = TimGetSeconds() + future_seconds;
    d.record.snoozeSecs = 0;
    if (recordIsDirty)
      FlushToDisk(true);
    else
      FlushToDisk(false);
    RescheduleAlarms(d.dbR);
    LabelAlarm();
  }
  if (recordIsAlarmSet && recordIsCountdown) {
    /* Go into high battery, constant update mode */
    d.nextPeriod = TimGetTicks() + 100;
  }
}

/*
** GetAlarmSelectListWidth
*/
static Int16 GetAlarmSelectListWidth(void) {
  UInt16 i = 0;
  FontID oldfont;
  Int16 list_width = 0;
  Char str[20];

  oldfont = FntSetFont(stdFont);

  for (; i < 30; ++i) {
    CountdownStuff(i, false, str);
    if (str)
      list_width = Max(list_width, FntLineWidth(str, StrLen(str)));
  }

  FntSetFont(oldfont);
  list_width += 4;
  
  return list_width;
}

/*
** CountdownDrawFunc - draw the countdown items in the list
*/
static void CountdownDrawFunc(Int16 item, RectangleType* b, Char** text) {
  Char str[20];

  CountdownStuff(item, false, str);
  WinDrawChars(str, StrLen(str), b->topLeft.x, b->topLeft.y);
}

/*
** DoDateSelect - handle selection of the Alarm Selection Trigger
*/
static void DoDateSelect(Int16 x) {
  FormPtr frm = FrmGetActiveForm();
  ListPtr list = GetObjectPointer(frm, PopCountdownList);
  Int16 list_choice;
  Int16 halfwidth = d.alarm_select_width / 2;
  Int16 position = x - halfwidth;
  RectangleType r;

  /* Pop winUp the quicklist for relative times */
  LstSetListChoices(list, NULL, 30);
  if (position < 38) position = 38;
  if (position > (147 - d.alarm_select_width))
    position = 147 - d.alarm_select_width;
  LstSetPosition(list, position, 24);

  /* Calculate and set bounds */
  FrmGetObjectBounds(frm, FrmGetObjectIndex(frm, PopCountdownList), &r);
  r.extent.x = d.alarm_select_width;
  FrmSetObjectBounds(frm, FrmGetObjectIndex(frm, PopCountdownList), &r);

  LstSetTopItem(list,0);
  LstSetSelection(list, 0);
  LstSetDrawFunction(list, CountdownDrawFunc);
  list_choice = LstPopupList(list);

  /* Handle result */
  if (list_choice != -1)
    CountdownStuff(list_choice, true, NULL);
}

/*
** Correctly label a midi file
**
** Returns the modified list_index (if it's larger than the number of MIDI files)
*/
Int16 LabelMIDITrigger(Int16 list_index, Char* string, UInt16 string_len) {

  if (d.midi_count == 0 || list_index < 0 || list_index > d.midi_count) {
    MemHandle h = DmGetResource(strRsc, MIDINotSelectedString);
    StrNCopy(string, MemHandleLock(h), string_len);
    MemHandleUnlock(h);
    DmReleaseResource(h);
    list_index = -1;
  } else {
    SndMidiListItemType* midi = MemHandleLock(d.midi_list);
    StrNCopy(string, midi[list_index].name, sndMidiNameLength);
    MemPtrUnlock(midi);
  }

  return list_index;
}

/*
** Truncate left aligned popuptrigger
*/
void TruncatePopupTriggerText(FormType* frm, UInt16 id, Char* str) {
  RectangleType r1, r2;

  FrmGetObjectBounds(frm, FrmGetObjectIndex(frm, id), &r1);
  FrmGetFormBounds(frm, &r2);
  TxtGlueTruncateString(str, r2.extent.x - r1.topLeft.x - 20);
}

/*
** Handles events for gadgets acting as dynamic FormBitmaps.
*/
Boolean BitmapGadgetEvent(FormGadgetTypeInCallback* gadgetP, UInt16 cmd, void* paramP) {
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
  }

  return handled;
}

/*
** Copies an icon/bitmap resource of a given application into the window.
*/
void CopyAppBitmapToWindow(UInt32 creator, DmResType resType, DmResID resID,
               WinHandle win, Coord x, Coord y) {
  BitmapType* bmp;
  MemHandle h;
  WinHandle oldH;
  DmOpenRef db;

  /* Fix if don't have draw window yet */
  if (!WinGetDrawWindow())
    WinSetDrawWindow(WinGetDisplayWindow());

  WinPushDrawState();
  if (d.hires && !d.sonyClie)
    SwitchToHiRes();

  db = DmOpenDatabaseByTypeCreator(sysFileTApplication, creator, dmModeReadOnly);
  h = DmGetResource(resType, resID);
  bmp = (BitmapType*) MemHandleLock(h);

  oldH = WinSetDrawWindow(win);
  WinDrawBitmap(bmp, x, y);
  WinSetDrawWindow(oldH);

  MemHandleUnlock(h);
  DmReleaseResource(h);
  DmCloseDatabase(db);
  WinPopDrawState();
}

/*
** MIDIListDrawFunc - draw the MIDI file names in the selection list
*/
static void MIDIListDrawFunc(Int16 item, RectangleType* b, Char** text) {
  SndMidiListItemType* ptr = (SndMidiListItemType*) MemHandleLock(d.midi_list);

  WinDrawTruncChars(ptr[item].name, StrLen(ptr[item].name), b->topLeft.x, b->topLeft.y, b->extent.x);

  MemHandleUnlock(d.midi_list);
}

/*
** NoMIDIListDrawFunc - draw a single string indication "No MIDIs"
*/
static void NoMIDIListDrawFunc(Int16 item, RectangleType* b, Char** text) {
  MemHandle h = DmGetResource(strRsc, MIDINotSelectedString);
  Char* str = (Char*) MemHandleLock(h);

  WinDrawChars(str, StrLen(str), b->topLeft.x, b->topLeft.y);

  MemHandleUnlock(h);
  DmReleaseResource(h);
}

/*
** Set up signal list
*/
void SetupSignalList(FormType* frm, UInt32 signalList, UInt32 signalPop, Int32 signal, Char* label) {
  UInt16 idx = FrmGetObjectIndex(frm, signalList);
  ListType* list = FrmGetObjectPtr(frm, idx);
  RectangleType r;/* , r1; */

  /* Set up the MIDI list */
  FrmGetObjectBounds(frm, idx, &r);
  InitMIDIList(&r);
  FrmSetObjectBounds(frm, idx, &r);

  if (d.midi_count > 0) {
    LstSetDrawFunction(list, MIDIListDrawFunc);
    LstSetListChoices(list, NULL, d.midi_count);
    LstSetSelection(list, signal > -1 && signal < d.midi_count ? signal : 0);
    LstMakeItemVisible(list, LstGetSelection(list));
  } else {
    MemHandle h = DmGetResource(strRsc, MIDINotSelectedString);
    Char* str = (Char*) MemHandleLock(h);

    FrmGetObjectBounds(frm, idx, &r);
    r.extent.x = FntCharsWidth(str, StrLen(str)) + 3;
    r.extent.y = 11;
    FrmSetObjectBounds(frm, idx, &r);

    MemHandleUnlock(h);
    DmReleaseResource(h);

    LstSetDrawFunction(list, NoMIDIListDrawFunc);
    LstSetListChoices(list, NULL, 1);
    LstSetSelection(list, 0);
  }

  /* Correctly label popup trigger */
  LabelMIDITrigger(LstGetSelection(list), label, 31);
  TruncatePopupTriggerText(frm, signalPop, label);
  CtlSetLabel(GetObjectPointer(frm, signalPop), label);
}

/*
** Set up signal dialog - also used for Alarm Preferences form
*/
void SetupSignalDialog(FormType* frm, Boolean led, Boolean vibrate,
               Int32 signal) {
  UInt32 capabilities;
  UInt32 sonyDeviceID = 0x0000;
  Boolean hasLED = false;
  Boolean hasVibrate = false;

  SetupSignalList(frm, SignalList, SignalPop, signal, d.midi_label);

  /* Check device capabilities */
  FtrGet(kAttnFtrCreator, kAttnFtrCapabilities, &capabilities);
  hasVibrate = capabilities & kAttnFlagsHasVibrate ? true : false;
  hasLED = capabilities & kAttnFlagsHasLED ? true : false;

  /* Special handling for Sony Clie */
  if (d.sonyClie) {
    FtrGet(sysFtrCreator, sysFtrNumOEMHALID, &sonyDeviceID);
    
    switch (sonyDeviceID) {
    case sonyHwrOEMDeviceIDVenice:
    case sonyHwrOEMDeviceIDModena:
    case sonyHwrOEMHALIDNaples:
      hasVibrate = true;
      hasLED = true;
      break;
    default:
      /* nothing */
    }
  }

  /* Special handling for Treo 600 */
  if (d.treo600) {
    hasVibrate = true;
    hasLED = true;
  }

  /* Set up vibrate controls */
  if (!hasVibrate) {
    HideObject(frm, VibrateCheck);
    CtlSetEnabled(GetObjectPointer(frm, VibrateCheck), false);
    ShowObject(frm, VibrateNotSupported);
  } else {
    /* Set the right value */
    CtlSetValue(GetObjectPointer(frm, VibrateCheck), vibrate);
  }

  /* Set up LED controls */
  if (!hasLED) {
    HideObject(frm, LEDCheck);
    CtlSetEnabled(GetObjectPointer(frm, LEDCheck), false);
    ShowObject(frm, LEDNotSupported);
  } else {
    /* Set the right value */
    CtlSetValue(GetObjectPointer(frm, LEDCheck), led);
  }
}

/*
** Pop up and handle "Note" dialog for current record
*/
void DoNoteDialog(UInt16 select_start, UInt16 select_length) {
  FormType* frm = FrmInitForm(NewNoteView);
  const UInt16 idx = FrmGetObjectIndex(frm, NoteField);
  const UInt16 oldDbI = p.dbI;
  const UInt16 mainFormID = FrmGetActiveFormID();

  FrmSetEventHandler(frm, NoteViewHandleEvent);
  NoteViewInit(frm);
  NoteViewDrawTitleAndForm(frm);
  NoteViewUpdateScrollBar();
  FrmSetFocus(frm, idx);

  /* Select search string if necessary */
  if (select_length)
    FldSetSelection(FrmGetObjectPtr(frm, idx), select_start, select_start + select_length);

  /* Execute dialog & save results*/
  FrmDoDialog(frm);
  p.dbI = oldDbI; /* Workaround for mysterious change in p.dbI when called from
             ThumbnailDetailView */
  FlushToDisk(false);

  /* Clean up */
  FrmDeleteForm(frm);
  if (mainFormID == DiddleThumbnailDetailForm)
    FrmUpdateForm(mainFormID, fUpdateReloadRecords);
}

/*
** Switch category in case of a GoTo event
*/
void SwitchCategoryForGoto(void) {
  FormType* frm = FrmGetActiveForm();
  UInt16 attr = 0;

  if (p.dbI >= 0 && p.category != dmAllCategories) {
    DmRecordInfo(d.dbR, p.dbI, &attr, NULL, NULL);
    if (p.category != (attr & dmRecAttrCategoryMask)) {
      ChangeCategory(dmAllCategories);

      if (frm && FrmGetFormId(frm) == DiddleTForm)
    CtlSetGraphics(GetObjectPointer(frm, CategoryButton), AllCategoriesBitmap, NULL);
    }
  }
}
/*
** Can the shape/line/text be drawn wholly inside the current drawing area?
*/
static Boolean CanRectangleBeDrawn(const RectangleType* shapeR,
                   const RectangleType* drawingAreaR) {
  RectangleType r;

  RctGetIntersection(shapeR, drawingAreaR, &r);

  return (r.extent.x != 0 && r.extent.y != 0);
}

/*
** Sets the clip rectangle of given window to the visible
** drawing area. The clip rectangle has to be reset with
** ResetDrawingAreaClip().
*/
void SetDrawingAreaClip(WinHandle h) {
  WinHandle oldH = WinSetDrawWindow(h);

  if (!d.sonyClie || !d.hires) {
    if (!d.hires || WinGetCoordinateSystem() == kCoordinatesStandard)
      WinSetClip(&p.r);
    else
      WinSetClip(&d.r_hires);
  } else {
    WinSetClip(&d.r_hires);
  }

  /* Clean up */
  WinSetDrawWindow(oldH);
}

/*
** Resets the clip rectangle of the given window.
*/
void ResetDrawingAreaClip(WinHandle h) {
  WinHandle oldH = WinSetDrawWindow(h);

  WinResetClip();

  /* Clean up */
  WinSetDrawWindow(oldH);
}

/*
** DoPenDown - handle penDown events in the main doodling form
*/
static Boolean DoPenDown(EventPtr e) {
  Boolean handled = false;
  Boolean penDown;
  Coord x, y;
  FlashMessageType* msg;
  RectangleType* r;
  void (*do_func)(Coord x, Coord y);
  void (*clear_func)();
  void (*end_func)(Coord x, Coord y);
  void (*rec_func)(Coord x, Coord y, RectangleType* r);
  const WinHandle drawWindow = WinGetDrawWindow();
  UInt16 oldCoords = 0;

  /* Set up draw state */
  WinPushDrawState();
  WinSetPattern(p.modeSelection == dbmSelectionErase ? &patternWhite : (const CustomPatternType*) &p.inkpat);

  /* Set up hi-res mode */
  if (d.hires) {
    oldCoords = SwitchToHiRes();
    if (!d.sonyClie) {
      e->screenX = WinScaleCoord(e->screenX, false);
      e->screenY = WinScaleCoord(e->screenY, false);
    } else {
      e->screenX *= HiResCoef;
      e->screenY *= HiResCoef;
    }
    
    r = &d.r_hires;
  } else {
    r = &p.r;
  }

  if (!d.drawmode /* dbModeDoodle */) {
    d.ox = e->screenX;
    d.oy = e->screenY;

    if (d.undo_mark) FlushToBuffer();
    DoStroke(e->screenX, e->screenY);

    do {
      if (d.hires && !d.sonyClie && !d.acer) {
    EvtGetPenNative(drawWindow, &x, &y, &penDown);
      } else {
    EvtGetPen(&x, &y, &penDown);

    if ((d.sonyClie || d.acer) && d.hires) {
      x *= HiResCoef;
      y *= HiResCoef;
    }
      }

      if ((x == d.ox) && (y == d.oy)) continue;
      
      DoStroke(x, y);
    } while (penDown && RctPtInRectangle(x, y, r));

    /* Restore previous coordinate System */
    if (oldCoords) SwitchToPreviousCoordinates(oldCoords);

    EndStroke();
    recordSetDirty();
    handled = true;
  } else {
    RectangleType shapeR;

    switch (d.drawmode) {
    case dbModeText:
      do_func = DoTextInsert;
      clear_func = ClearOldText;
      end_func = EndTextInsert;
      rec_func = TextRectangle;
      break;

    case dbModeLine:
      do_func = DoLine;
      clear_func = ClearOldLine;
      end_func = EndLine;
      rec_func = LineRectangle;
      break;

    case dbModeShape:
      do_func = DoShape;
      clear_func = ClearOldShape;
      end_func = EndShape;
      rec_func = (void (*)(Coord x, Coord y, RectangleType* r)) LoadShapeRect;
      break;

    case dbModeFloodFill:
      EvtFlushPenQueue();
      FlushToBuffer();
      msg = FlashMessage(FloodFillMessageString);
      FloodFill(e->screenX, e->screenY, (const CustomPatternType*) &p.inkpat);
      UnFlashMessage(msg);

      /* Mark for undo */
      recordSetDirty();
      d.undo_mark = true;

      /* Clean up */
      MirrorRealCanvas();
      SwitchToDoodleMode();
      WinPopDrawState();
      
      return true;
      break;

    case dbModeCut:
      do_func = DoCutCopy;
      clear_func = ClearOldCutCopy;
      end_func = EndCut;
      rec_func = CutCopyRectangle;
      break;

    case dbModeCopy:      
      do_func = DoCutCopy;
      clear_func = ClearOldCutCopy;
      end_func = EndCopy;
      rec_func = CutCopyRectangle;
      break;

    case dbModePaste:
      do_func = DoPaste;
      clear_func = ClearOldPaste;
      end_func = EndPaste;
      rec_func = PasteRectangle;
      break;

    default:   
      /* Clean up */
      WinPopDrawState();
      return false;
      break;
    }

    d.anchor.x = d.ox = e->screenX;
    d.anchor.y = d.oy = e->screenY;
    FlushToBuffer();

    (*rec_func)(e->screenX, e->screenY, &shapeR);
    if (CanRectangleBeDrawn(&shapeR, r))
      (*do_func)(e->screenX, e->screenY);

    if (d.hires && !d.sonyClie)
      oldCoords = WinSetCoordinateSystem(kCoordinatesNative);

    do {
      if (d.hires && !d.sonyClie && !d.acer) {
    EvtGetPenNative(drawWindow, &x, &y, &penDown);
      } else {
    EvtGetPen(&x, &y, &penDown);

    if ((d.sonyClie || d.acer) && d.hires) {
      x *= HiResCoef;
      y *= HiResCoef;
    }
      }
 
      if ((x == d.ox) && (y == d.oy))
    continue;

      (*rec_func)(d.ox, d.oy, &shapeR);
      if (CanRectangleBeDrawn(&shapeR, r))
    (*clear_func)();
      (*rec_func)(x, y, &shapeR);
      if (CanRectangleBeDrawn(&shapeR, r))
    (*do_func)(x, y);
    } while (penDown && CanRectangleBeDrawn(&shapeR, r));
    
    (*rec_func)(d.ox, d.oy, &shapeR);
    if (CanRectangleBeDrawn(&shapeR, r))
      (*clear_func)();
    
    (*rec_func)(x, y, &shapeR);
    if (CanRectangleBeDrawn(&shapeR, r)) {
      (*end_func)(x, y);
      recordSetDirty();
      d.undo_mark = true;
    } else {
      if (d.drawmode == dbModeText) 
    FntSetFont(d.txt_oldfont);
      SwitchToDoodleMode();
    }

    /* Restore previous coordinate System */
    if (oldCoords)
      SwitchToPreviousCoordinates(oldCoords);

    handled = true;
  }

  /* Clean up */
  WinPopDrawState();

  return handled;
}

/*
** CheckPageButtonDrag
*/
static Boolean CheckPageButtonDrag(PointType* orig, Coord curX, Coord curY) {
  RectangleType button_bounds;
  Coord upper_limit, lower_limit;
  FormType* frm = FrmGetActiveForm();

  /* Check for the right form */
  if (!frm || ((FrmGetActiveFormID() != DiddleForm) && (FrmGetActiveFormID() != DiddleTForm)))
    return false;

  FrmGetObjectBounds(frm, FrmGetObjectIndex(frm, PageButton), &button_bounds);
  if (!RctPtInRectangle(orig->x, orig->y, &button_bounds))
    return false;

  upper_limit = button_bounds.topLeft.y;
  lower_limit = button_bounds.topLeft.y + button_bounds.extent.y;

  /* The upper_limit is numerically lower, the lower_limit numerically higher */
  if (curY >= upper_limit && curY <= lower_limit) {
    if (orig->x + p.min_swipe < curX) {
      KeyDown(chrSpace); /* move to next page */
      return true;
    } else if (curX < orig->x - p.min_swipe) {
      KeyDown(chrBackspace); /* move to previous page */
      return true;
    }
  }

  return false;
}

/*
** CustomListDrawFunc - draw the numbers in the custom time form
*/
static void PageListDrawFunc(Int16 item, RectangleType* b, Char** text) {
  UInt16 attr;

  DmRecordInfo(d.dbR, d.pos_to_index[item], &attr, NULL, NULL);

  if (!((attr & dmRecAttrSecret) && d.privateRecordStatus == maskPrivateRecords))
    WinDrawTruncChars(text[item], StrLen(text[item]), b->topLeft.x, b->topLeft.y, b->extent.x);
  else {
    b->extent.x -= 2;
    ListViewDisplayMask(b);
  }
}

/*
** Open a list containing all the pages
*/
static Boolean DoPageList(void) {
  Int16 res, width = 0, current = -1;
  FormType* frm = FrmGetActiveForm();
  Char** choices = NULL;
  MemHandle t = NULL, template = NULL;
  UInt16 idx = 0;
  UInt16 len;
  UInt16 i = 0;
  const UInt16 lidx = FrmGetObjectIndex(frm, PageList);
  ListType* list = GetObjectPointer(frm, PageList);
  MemPtr ptr = NULL;
  DiddleBugRecordType rec;
  RectangleType r;
  Char num_str1[maxStrIToALen], num_str2[maxStrIToALen];
  
  ASSERT(d.records_in_cat);

  choices = MemPtrNew(sizeof(Char*) * d.records_in_cat);
  ASSERT(choices);
  d.pos_to_index = MemPtrNew(sizeof(UInt16) * d.records_in_cat);
  ASSERT(d.pos_to_index);

  for (; i < d.records_in_cat; ++i) {
    if (DmSeekRecordInCategory(d.dbR, &idx, 0, dmSeekForward, p.category) != errNone)
      abort();

    t = DmQueryRecord(d.dbR, idx);
    ASSERT(t);
    
    if (idx != p.dbI) {
      ptr = MemHandleLock(t);
      MemMove(&rec, ptr, sizeof(DiddleBugRecordType));
      ptr += sketchDataOffset + rec.sketchLength;
    } else {
      t = d.record_name;
      ptr = MemHandleLock(t);
      current = i;
    }
    
    /* save mapping */
    d.pos_to_index[i] = idx;
    
    len = StrLen(ptr) + 1; /* +1 for NULL char */
    if (len > 1) {
      choices[i] = MemPtrNew(sizeof(Char) * len);
      ASSERT(choices[i]);
      MemMove(choices[i], ptr, len);
    } else {
      template = DmGetResource(strRsc, UntitledNoteTitleString);
      choices[i] = TxtParamString(MemHandleLock(template),
                  StrIToA(num_str1, i + 1), StrIToA(num_str2, d.records_in_cat), NULL, NULL);
      ASSERT(choices[i]);
      len = StrLen(choices[i]) + 1;
      MemHandleUnlock(template);
      DmReleaseResource(template);
    }
    
    /* Calculate width of list */
    width = Max(width, FntCharsWidth(choices[i], len - 1));
    
    /* Clean up */
    MemHandleUnlock(t);
    ++idx;
  }

  /* Calculate list size */
  width = Min(width, 120); /* max width for list */
  FrmGetObjectBounds(frm, lidx, &r);
  r.extent.y = 11 * Min(d.records_in_cat, 9); /* don't show more than 9 items at once */
  r.extent.x = width + 4 + (d.records_in_cat > 9 ? 10 : 0); /* extra width for arrow if necessary */

  FrmSetObjectBounds(frm, lidx, &r);
  LstSetDrawFunction(list, PageListDrawFunc);
  LstSetListChoices(list, choices, d.records_in_cat);
  LstSetSelection(list, current);
  if (current > -1)
    LstMakeItemVisible(list, current);
  res = LstPopupList(list);

  if (res != -1) {
    ErrTry {
      FlushToDisk(true);
    }
    ErrCatch(inErr) {
      /* Clean up */
      for (i = 0; i < d.records_in_cat; ++i)
    MemPtrFree(choices[i]);
      MemPtrFree(choices);
      MemPtrFree(d.pos_to_index);
      d.pos_to_index = NULL;
      
      ErrThrow(inErr);
    } ErrEndCatch;
    
    idx = 0;
    if (DmSeekRecordInCategory(d.dbR, &idx, res, dmSeekForward, p.category) != errNone)
      abort();
    p.dbI = idx;

    LoadFromDisk();
    LabelAlarm();
  }

  /* Clean up */
  for (i = 0; i < d.records_in_cat; ++i)
    MemPtrFree(choices[i]);
  MemPtrFree(choices);
  MemPtrFree(d.pos_to_index);
  d.pos_to_index = NULL;
  
  return true;
}

/*
** Handles events for my custom gadgets
*/
static Boolean GadgetEvent(FormGadgetTypeInCallback* gadgetP, UInt16 cmd, void* paramP) {
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
    {
      EventType* e = (EventType*) paramP;
      if (e->eType == frmGadgetEnterEvent) {
    if (DynBtnTrackPen(btn)) {
      /* Handle the selection */
      ButtonDrop(btn->id);

      if (btn->id == InkButton)
        DrawInkLabelDynamic(btn->content.bmpW, 0, 0, 4, (const CustomPatternType*) &p.inkpat);
      else
        DrawPenLabelDynamic(btn->content.bmpW, 0, 0, 4, p.penpix);

      DynBtnDraw(btn);
    }
      }

      handled = true;
    } break;
  }

  return handled;
}

/*
** Enables or disables the note button,
** depending on the existance of an attached note.
**
** This function assumes that there is a NoteButton
** in the active form!
*/
void UpdateNoteButton(void) {
  FormType* frm = FrmGetActiveForm();
  const UInt16 idx = FrmGetObjectIndex(frm, NoteButton);
  const UInt16 idx_filled = FrmGetObjectIndex(frm, FilledNoteButton);

  if (StrLen((Char*) MemHandleLock(d.record_note)) != 0) {
    FrmHideObject(frm, idx);
    FrmShowObject(frm, idx_filled);
  } else {
    FrmHideObject(frm, idx_filled);
    FrmShowObject(frm, idx);
  }

  /* Clean up */
  MemHandleUnlock(d.record_note);
}

/*
** Updates the page button (the "3/10" one)
**
** This function assumes that there is a PageButton in
** the active form!
*/
void UpdatePageButton(void) {
  Char temp[8];
  Char temp2[4];
  const UInt16 pos = DmPositionInCategory(d.dbR, p.dbI, p.category);

  /* Show current page number */
  StrIToA(temp, pos + 1);
  StrNCat(temp, "/", 7);
  StrIToA(temp2, d.records_in_cat);
  StrNCat(temp, temp2, 7);

  StrNCopy(d.bpText, temp, 7);
  CtlSetLabel(GetObjectPointer(FrmGetActiveForm(), PageButton), d.bpText);
}
/*
** MakeDateTimeStamp - make sure that "stamp" is big enough for the resulting string
*/
void MakeDateTimeStamp(UInt32 secs, Char* stamp) {
  DateTimeType dateTime;
  Char tempStr[timeStringLength];

  TimSecondsToDateTime(secs, &dateTime);
  DateToAscii(dateTime.month, dateTime.day, dateTime.year, PrefGetPreference(prefLongDateFormat), stamp);
  StrNCat(stamp, ", ", 31);
  TimeToAscii(dateTime.hour, dateTime.minute, PrefGetPreference(prefTimeFormat), tempStr);
  StrNCat(stamp, tempStr, 31);
}

/*
** Tracks pen movement in the dynamic trigger button
*/
static void TrackTriggerPen(DynamicButtonType* btn, EventType* e) {
  UInt16 x, y;
  Coord left_limit, right_limit;
  Boolean penDown, oldSel;

  /* Redraw button */
  btn->selected = true;
  DynBtnDraw(btn);

  left_limit = e->screenX - p.min_swipe;
  right_limit = e->screenX + p.min_swipe;

  do {
    EvtGetPen(&x, &y, &penDown);

    oldSel = btn->selected;
    btn->selected = RctPtInRectangle(x, y, &btn->frameRect);
    if (oldSel != btn->selected) DynBtnDraw(btn);

    /* Drag text if in drawing area */
    if (y < btn->frameRect.topLeft.y && !recordIsLocked
    && !(d.record_private && d.privateRecordStatus == maskPrivateRecords)) {
      EventType eDate;
      Int16 old_drawmode;

      eDate.screenX = x; eDate.screenY = y;
      old_drawmode = d.drawmode; d.drawmode = dbModeText;
      d.txt_oldfont = FntSetFont(p.txt_font);
      /* Copy current date and time to d.InsertionText */
      MakeDateTimeStamp(TimGetSeconds(), d.InsertionText);

      /* Insert the text */
      DoPenDown(&eDate);

      d.drawmode = old_drawmode;
      FntSetFont(d.txt_oldfont);
      EvtGetPen(&x, &y, &penDown);
    }
  } while (penDown);

  /* Redraw button */
  btn->selected = false;
  DynBtnDraw(btn);

  /* The upper_limit is numerically lower, the lower_limit numerically higher */
  if (y >= btn->frameRect.topLeft.y) {
    if (x < left_limit) {
      KeyDown(cmdTransfer);
    } else if (x > right_limit) {
      if (recordIsLocked && recordIsAlarmSet) {
    LockAsk();
      } else {
    recordUnsetCountdown();
    FrmGotoForm(TimeForm);
      }
    } else if (RctPtInRectangle(x, y, &btn->frameRect)) {
      if (recordIsLocked && recordIsAlarmSet)
    LockAsk();
      else
    DoDateSelect(x);
    }
  }
}

/*
** Handles events for my custum trigger button
*/
static Boolean TriggerGadgetEvent(FormGadgetTypeInCallback* gadgetP, UInt16 cmd, void* paramP) {
  Boolean handled = false;
  DynamicButtonType* btn = (DynamicButtonType*)gadgetP->data;

  switch (cmd) {
  case formGadgetDeleteCmd:
    DynBtnDelete(btn);
    handled = true;
    break;

  case formGadgetEraseCmd:
    WinEraseRectangleFrame(btn->frame, &btn->frameRect);
    gadgetP->attr.visible = false;
    gadgetP->attr.usable = false;
    handled = true;
    break;

  case formGadgetDrawCmd:
    DynBtnDraw(btn);
    gadgetP->attr.visible = true;
    handled = true;
    break;

  case formGadgetHandleEventCmd: {
    EventType* e = (EventType*) paramP;
    if (e->eType == frmGadgetEnterEvent) {
      if (!d.record_private || d.privateRecordStatus == showPrivateRecords ||
      d.unmaskedCurrentRecord)
    TrackTriggerPen(btn, e);
      else
    UnlockMaskedRecords();
    }
    
    handled = true;
  } break;
  }

  return handled;
}

/*
** Display 'About' dialog
*/
void DoAboutDialog(void) {
  FormType* frm;
  DynamicButtonType* btn;
  Err err;
 
  if (d.formID != DiddleListForm
      && d.formID != DiddleThumbnailForm
      && d.formID != DiddleThumbnailDetailForm) {
    FlushToDisk(true);
  }
  frm = FrmInitForm(AboutForm);
  btn = DynBtnInitGadget(AboutBitmapGadget, noFrame, false, true, true, d.sonyClie,
             d.sonyHRRefNum, d.hires && !d.sonyClie, dynBtnGraphical, 0, &err, frm,
             BitmapGadgetEvent);
  if (err) abort();
  CopyAppBitmapToWindow(AppType, iconType, 1000, btn->content.bmpW, 0, 0);

  FrmDoDialog(frm);
  FrmDeleteForm(frm);
}

/*
** Update everything after category switch
*/
void SwitchCategory(UInt16 old_cat, FormType* frm) {
  UInt16 idx = p.dbI;
  UInt16 bmpID = 0;
  
  PRINT("Switching from category %hd, d.records_in_cat = %hd", old_cat, d.records_in_cat);
  PRINT("to category %hd, p.dbI = %hd, d.record_in_cat = %hd, total # records = %hd",
    p.category, p.dbI, DmNumRecordsInCategory(d.dbR, p.category), DmNumRecords(d.dbR));

  /* update cached values */
  d.records_in_cat = DmNumRecordsInCategory(d.dbR, p.category);
  SetTopVisibleRecord(0);

  if (FrmGetFormId(frm) != DiddleThumbnailDetailForm) {
    /* Save old image */
    FlushToDisk(true);

    /* Delete active empty sketch */
    if (old_cat != p.category && old_cat != dmAllCategories
    && SketchIsEmpty(DmQueryRecord(d.dbR, p.dbI)))
      RemoveImage();

    if (p.category == dmAllCategories) {
      bmpID = AllCategoriesBitmap;
    } else {
      bmpID = CategoryBitmap;
      
      /* switch to a record within the category */
      if (DmSeekRecordInCategory(d.dbR, &idx, 0, dmSeekBackward, p.category) == errNone
      || DmSeekRecordInCategory(d.dbR, &idx, 0, dmSeekForward, p.category) == errNone)
    p.dbI = idx;
      else
    AllocImage();
      
      LoadFromDisk();
    }
  } else {
    /* Display the new category. */
    ThumbnailDetailViewLoadRecords(frm);
  }

  PRINT("Switched to category %hd, p.dbI = %hd, d.records_in_cat = %hd, total # records = %hd",
    p.category, p.dbI, d.records_in_cat, DmNumRecords(d.dbR));
  
  if (FrmGetFormId(frm) == DiddleTForm) {
    CtlSetGraphics(GetObjectPointer(frm, CategoryButton), bmpID, NULL);
    UpdatePageButton();
    UpdateNoteButton();
  }
}

/*
** DiddleMenuEvent - handle menu events for doodling form.
*/
static Boolean DiddleMenuEvent(EventType* e) {
  Boolean handled = false;
  Char chr = 0;

  if (d.is_xfer_mode) {
    switch(e->data.menu.itemID) {
    case menuitemID_XferDone:
      FinishXferMode();
      handled = true;
      break;
    case menuitemID_XferPhoneLookup: 
      {
        FormType* frm = FrmGetActiveForm();
    FieldPtr fld = GetObjectPointer(frm, XferField);
    PhoneNumberLookup(fld);
    handled = true;
      }
      break;
    }
  }

  if (!handled) {
    switch(e->data.menu.itemID) {
    case menuitemID_PenFine: chr=penFine; break;
    case menuitemID_PenMedium: chr=penMedium; break;
    case menuitemID_PenBold: chr=penBold; break;
    case menuitemID_PenBroad: chr=penBroad; break;
    case menuitemID_PenFineItalic: chr=penFineItalic; break;
    case menuitemID_PenMediumItalic: chr=penMediumItalic; break;
    case menuitemID_PenBroadItalic: chr=penBroadItalic; break;
    case menuitemID_InkWhite: chr=inkWhite; break;
    case menuitemID_InkDith12: chr=inkDith12; break;
    case menuitemID_InkDith25: chr=inkDith25; break;
    case menuitemID_InkDith37: chr=inkDith37; break;
    case menuitemID_InkDith50: chr=inkDith50; break;
    case menuitemID_InkDith62: chr=inkDith62; break;
    case menuitemID_InkDith75: chr=inkDith75; break;
    case menuitemID_InkDith87: chr=inkDith87; break;
    case menuitemID_InkBlack: chr=inkBlack; break;
    case menuitemID_InkRandom: chr=inkRandom; break;
    case menuitemID_ModePaint: chr=modePaint; break;
    case menuitemID_ModeErase: chr=modeErase; break;
    case menuitemID_CmdRough: chr=cmdRough; break;
    case menuitemID_CmdSmooth: chr=cmdSmooth; break;
    case menuitemID_CmdSmoother: chr=cmdSmoother; break;
    case menuitemID_CmdAbout:
      DoAboutDialog();
      handled = true;
      break;
    case menuitemID_CmdPref: chr=cmdPref; break;
    case menuitemID_CmdExtPref: chr=cmdExtPref; break;
    case menuitemID_CmdHWPref: chr=cmdHWPref; break;
    case menuitemID_CmdAlarmPref: chr=cmdAlarmPref; break;
    case menuitemID_CmdFill: chr=cmdFill; break;
    case menuitemID_CmdInvert: chr=cmdInvert; break;
    case menuitemID_CmdSketchUndo: chr=cmdSketchUndo; break;
    case menuitemID_CmdSketchCopy: chr=cmdSketchCopy; break;
    case menuitemID_CmdSketchPaste: chr=cmdSketchPaste; break;
    case menuitemID_CmdSketchCut: chr=cmdSketchCut; break;
    case menuitemID_CmdRemove: chr=cmdRemove; break;
    case menuitemID_CmdTitleOff: /* fall through */
    case menuitemID_CmdTitleOn: chr=cmdTitle; break;
    case menuitemID_CmdToolbarOff: /* fall through */
    case menuitemID_CmdToolbarOn: chr=cmdToolbar; break;
    case menuitemID_CmdNew: chr=cmdNew; break;
    case menuitemID_CmdPrevious: chr=cmdPrevious; break;
    case menuitemID_CmdNext: chr=cmdNext; break;
    case menuitemID_CmdText: chr=cmdText; break;
    case menuitemID_CmdLine: chr=cmdLine; break;
    case menuitemID_CmdShape: chr=cmdShape; break;
    case menuitemID_CmdLock: chr=cmdLock; break;
    case menuitemID_CmdTransfer: chr=cmdTransfer; break;
    case menuitemID_CmdBeam: chr=cmdBeam; break;
    case menuitemID_CmdSend: chr=cmdSend; break;
    case menuitemID_CmdUnlock: chr=cmdLock; break;
    case menuitemID_CmdHelp: chr=cmdHelp; break;
    case menuitemID_CmdDuplicate: chr=cmdDuplicate; break;
    case menuitemID_CmdDetails: chr=cmdDetails; break;
    case menuitemID_CmdListView: chr=cmdListView; break;
    case menuitemID_CmdSecurity: chr=cmdSecurity; break;
    case menuitemID_CmdFloodFill: chr=cmdFloodFill; break;
    }
  }

  if (!handled)
    handled = KeyDown(chr);

  return handled;
}

/*
** Handle ctlSelectEvents on doodling form
*/
static Boolean DiddleSelectEvent(EventPtr e) {
  Boolean handled = false;

  SwitchToDoodleMode();
  switch (e->data.ctlEnter.controlID) {
  case NoteButton:
  case FilledNoteButton:
    if (!d.record_private || d.privateRecordStatus == showPrivateRecords || UnlockMaskedRecords()) {
      DoNoteDialog(0, 0);
      UpdateNoteButton();
    }
    handled = true;
    break;
  case ModeButton:
    if (p.modeSelection == dbmSelectionPaint)
      SetPaintMode(dbmSelectionErase, p.erasePenSelection, ep.eraseFiltSelection);
    else
      SetPaintMode(dbmSelectionPaint, p.penSelection, p.filtSelection);
    handled = true;
    break;
  case SmoothButton:
    handled = ButtonDrop(SmoothButton);
    if (FrmGetActiveFormID() == DiddleTForm) {
      Int16 offset;

      if (p.modeSelection == dbmSelectionErase)
    offset = ep.eraseFiltSelection;
      else
    offset = p.filtSelection;

      CtlSetGraphics(GetObjectPointer(FrmGetActiveForm(), SmoothButton), CmdRoughBitmap+offset, NULL);
    }
    break;
  case PenButton:
    handled = ButtonDrop(PenButton);
    break;
  case InkButton:
    handled = ButtonDrop(InkButton);
    break;
  case PageButton:
    handled = CheckPageButtonDrag(&d.pbPoint, e->screenX, e->screenY);
    d.pbPoint.x = 0;
    d.pbPoint.y = 0;

    if (!handled)
      handled = DoPageList();
    break;
  case MainNewButton:
    handled = KeyDown(cmdNew);
    break;
  case MainClearButton:
    handled = KeyDown(cmdClear);
    break;
  case MainDeleteButton:
    handled = KeyDown(cmdRemove);
    break;
  case MainLockButton:
    LockAsk();
    handled = true;
    break;
  case MainSwitchButton:
    if (recordIsCountdown) 
      recordUnsetCountdown();
    else 
      recordSetCountdown();
    LabelAlarm();
    handled = true;
    break;
  case CategoryButton:
  case CategoryPop:
    {
      Char categoryName [dmCategoryLength];
      FormType* frm = FrmGetActiveForm();
      UInt16 old_cat = p.category;

      CategoryGetName(d.dbR, p.category, categoryName);
      CategorySelect(d.dbR, frm, CategoryPop, CategoryList, true,
             &p.category, categoryName, 1, categoryDefaultEditCategoryString);

      SwitchCategory(old_cat, frm);
    }
    handled = true;
    break;
  }

  return handled;
}

/*
** Check password and unlock masked record
*/
static Boolean UnlockMaskedRecords(void) {
  if (SecVerifyPW(showPrivateRecords)) {
    d.privateRecordStatus = showPrivateRecords;

    if (d.record_private)
      LoadFromDisk();
    
    /* We only want to unmask this one record, so restore the preference. */
    PrefSetPreference(prefShowPrivateRecords, maskPrivateRecords);
    d.privateRecordStatus = maskPrivateRecords;

    d.unmaskedCurrentRecord = true;

    return true;
  }

  return false;
}

/*
** Update menu according to current state
*/
static void SetUpMenu(void) {
  if (d.os4) {
    if (p.formID == DiddleTForm) {
      MenuHideItem(menuitemID_CmdTitleOn);
      MenuShowItem(menuitemID_CmdTitleOff);
    } else {
      MenuHideItem(menuitemID_CmdTitleOff);
      MenuShowItem(menuitemID_CmdTitleOn);
    }

    if (p.flags & PFLAGS_WITHOUT_TOOLBAR) {
      MenuHideItem(menuitemID_CmdToolbarOff);
      MenuShowItem(menuitemID_CmdToolbarOn);
    } else {
      MenuHideItem(menuitemID_CmdToolbarOn);
      MenuShowItem(menuitemID_CmdToolbarOff);
    }

    if (recordIsLocked) {
      MenuHideItem(menuitemID_CmdLock);
      MenuShowItem(menuitemID_CmdUnlock);
    } else {
      MenuHideItem(menuitemID_CmdUnlock);
      MenuShowItem(menuitemID_CmdLock);
    }
  }
}

/*
** HandleNonKeyCommandCode - handles commands that don't have a shortcut key.
*/
Boolean HandleNonKeyCommandCode(NonKeyCommandCodeType c) {
  Boolean res = false;

  switch (c) {
  case nkCmdHardwareNew:
    FlushToDisk(true);
    NewScratchImage();
    LoadFromDisk();
    LabelAlarm();
    SndPlaySystemSound(sndClick);
    res = true;
    break;
    
  case nkCmdNextCategory:
    switch (d.formID) {
    case DiddleForm:
    case DiddleTForm:
      KeyDown(cmdNextCategory);
      break;

    case DiddleListForm:
      ListViewNextCategory();
      break;

    case DiddleThumbnailForm:
      ThumbnailViewNextCategory();
      break;
      
    case DiddleThumbnailDetailForm:
      ThumbnailDetailViewNextCategory();
      break;
    }
    res = true;
    break;

  default:
    /* ignore */
  }

  return res;
}

/*
** DiddleEvent - handle events on the doodling form.
*/
static Boolean DiddleEvent(EventPtr e) {
  Boolean handled = false;
  DynamicButtonType* btn = NULL;

  /* Bail out if we're exiting due to an error */
  if (d.exiting) 
    return false;

  ErrTry {
    switch (e->eType) {
      
    case frmOpenEvent:
      {
    FormType* frm = FrmGetActiveForm();
    Err err = 0;
    const Boolean titlebar = p.formID == DiddleTForm;
    const Boolean toolbar = p.flags & PFLAGS_WITHOUT_TOOLBAR ? false : true;
    /* Switch to standard coordinate system */
    if (!WinGetDrawWindow())
      WinSetDrawWindow(WinGetDisplayWindow());
    WinPushDrawState();
    SwitchToLoRes();

    d.winM = FrmGetWindowHandle(frm);
    ResetDrawingAreaRectangle(titlebar, toolbar);
    
    if (!toolbar)
      ToggleButtonBar(frm, false);
    
    if (titlebar) {
      /* Set up PenButton */
      btn = DynBtnInitGadget(PenButton, noFrame, false, false, false, d.sonyClie,
                 d.sonyHRRefNum, d.hires, dynBtnGraphical, 0,
                 &err, frm, GadgetEvent);
      if (err) abort();
      DrawPenLabelDynamic(btn->content.bmpW, 0, 0, 4, p.penpix);
      
      /* Set up InkButton */
      btn = DynBtnInitGadget(InkButton, roundFrame, false, true, false, d.sonyClie,
                 d.sonyHRRefNum, d.hires, dynBtnGraphical, 0,
                 &err, frm, GadgetEvent);
      if (err) abort();
      DrawInkLabelDynamic(btn->content.bmpW, 0, 0, 4, (const CustomPatternType*) &p.inkpat);
      
      /* Update normal buttons */
      CtlSetGraphics(GetObjectPointer(frm, ModeButton), CmdPaintBitmap+p.modeSelection, NULL);
      if (p.modeSelection == dbmSelectionErase)
        CtlSetGraphics(GetObjectPointer(FrmGetActiveForm(), SmoothButton), CmdRoughBitmap+ep.eraseFiltSelection, NULL);
      else
        CtlSetGraphics(GetObjectPointer(FrmGetActiveForm(), SmoothButton), CmdRoughBitmap+p.filtSelection, NULL);
      
      /* Update category button */
      if (p.category == dmAllCategories)
        CtlSetGraphics(GetObjectPointer(frm, CategoryButton), AllCategoriesBitmap, NULL);
      else
        CtlSetGraphics(GetObjectPointer(frm, CategoryButton), CategoryBitmap, NULL);
    }
    
    /* Setup alarm trigger */
    btn = DynBtnInitGadget(MainDateSelTrig, rectangleFrame, true, false, false,
                   d.sonyClie, d.sonyHRRefNum, d.hires && !d.sonyClie, dynBtnText, 
                   ALARM_STRING_LENGTH, &err, frm, TriggerGadgetEvent);
    if (err) abort();

    /* Setup transfer buttons */
    btn = DynBtnInitGadget(XferDoneButton, noFrame, false, true, false, d.sonyClie,
                   d.sonyHRRefNum, d.hires && !d.sonyClie, dynBtnGraphical, 0,
                   &err, frm, XferDoneGadgetEvent);
    if (err) abort();
    DrawXferDoneButton(btn);
    btn = DynBtnInitGadget(XferDetailsButton, noFrame, false, true, false, 
                   d.sonyClie, d.sonyHRRefNum, d.hires && !d.sonyClie, 
                   dynBtnGraphical, 0, &err, frm, XferDetailsGadgetEvent);
    if (err) abort();
    DrawPlugButton(btn);

    LabelTitle();

    /* Switch back to previous coordinate system */
    WinPopDrawState();

    FrmDrawForm(frm);
    LoadFromDisk();
    LabelAlarm();
    d.nextPeriod = TimGetTicks() + 100;
    handled = true;
      }
      break;

    case appStopEvent:
    case frmCloseEvent:
      if (d.is_xfer_mode) CancelXferMode();
      FlushToDisk(true);
      break;

    case frmUpdateEvent:
      switch (e->data.frmUpdate.updateCode) {
      case fUpdateDeletedCurrent:
    if (d.is_xfer_mode) CancelXferMode();
    LoadFromDisk();
    LabelAlarm();
    if (e->data.frmUpdate.formID == DiddleTForm)
      UpdatePageButton();
    break;

      case fUpdateDeletedAny:
    LabelAlarm();
    if (e->data.frmUpdate.formID == DiddleTForm)
      UpdatePageButton();
    break;

      case fUpdateEraseToolbar:
    WinEraseRectangle(&bottom_bar_rect, 0); 
    break;

      default:
    FrmDrawForm(FrmGetFormPtr(e->data.frmUpdate.formID));
    MirrorRealCanvas();
      }
      handled = true;
      break;

    case frmGotoEvent:
      if (d.is_xfer_mode) CancelXferMode();
      FlushToDisk(true);
      p.dbI = e->data.frmGoto.recordNum;
      SwitchCategoryForGoto();
      LoadFromDisk();
      e->eType = dbOpenRecordFieldEvent;
      EvtAddEventToQueue(e);
      LabelAlarm();
      handled = true;
      break;

    case dbOpenRecordFieldEvent:
      switch (e->data.frmGoto.matchFieldNum) {
      case ffNote:
    DoNoteDialog(e->data.frmGoto.matchPos, e->data.frmGoto.matchLen);
    break;

      case ffDetails:
    d.detailsWithSketch = true;
    FrmPopupForm(RecordDetailsForm);
    break;

      case ffAlarm:
    recordUnsetCountdown();
    FrmGotoForm(TimeForm);
    break;

      default:
    /* ignore */
      }

      /* Update x/y button & note button*/
      if (FrmGetActiveFormID() == DiddleTForm)
    UpdateNoteButton();

      handled = true;
      break;

    case dbNonKeyCommandEvent:
      handled = HandleNonKeyCommandCode(e->data.menu.itemID);
      break;

    case menuEvent:
      /*     SwitchToDoodleMode(); */
      handled = DiddleMenuEvent(e);
      break;

    case keyDownEvent:
      /*     SwitchToDoodleMode(); */
      if (e->data.keyDown.chr == alarmChr) {
    FlushToDisk(true);
    handled = true;
      }
      else handled = KeyDown(e->data.keyDown.chr);
      break;

    case ctlEnterEvent:
      switch (e->data.ctlEnter.controlID) {
      case PageButton:
    d.pbPoint.x = e->screenX;
    d.pbPoint.y = e->screenY;
    break;
      }
      break;

    case ctlExitEvent:
      switch (e->data.ctlEnter.controlID) {
      case PageButton:
    handled = CheckPageButtonDrag(&d.pbPoint, e->screenX, e->screenY);
    d.pbPoint.x = 0;
    d.pbPoint.y = 0;
    break;
      }
      break;

    case ctlSelectEvent:
      handled = DiddleSelectEvent(e);
      break;

    case penDownEvent:
      DiddlePenClear();
      if (d.is_xfer_mode) {
    handled = XferPenDown(e);
      }
      else if (RctPtInRectangle(e->screenX, e->screenY, &p.r)) {
    if (recordIsLocked) {
      LockWarn();
      handled = true;
    }
    else if (d.record_private && d.privateRecordStatus == maskPrivateRecords && !d.unmaskedCurrentRecord) {
      UnlockMaskedRecords();
      handled = true;
    } else {
      d.drawing = true;
      handled = DoPenDown(e);
    }
      }
      break;

    case penMoveEvent:
      if (d.is_xfer_mode) {
    handled = XferPenDown(e);
      }
      else if (RctPtInRectangle(e->screenX, e->screenY, &p.r)) {
    if (d.record_private && d.privateRecordStatus == maskPrivateRecords && !d.unmaskedCurrentRecord) {
      UnlockMaskedRecords();
      handled = true;
    } else if (!recordIsLocked) {
      if (d.drawing)
        handled = DoPenDown(e);
    } else {
      //LockWarn();
      handled = true;
    }
      }
      break;

    case penUpEvent:
      d.drawing = false;
      /*   if (recordIsLocked && RctPtInRectangle(e->screenX, e->screenY, &p.r)) { */
      /*       LockWarn(); */
      /*       handled = true; */
      /*     } */
      break;
   

    case frmTitleEnterEvent:
      {
    Boolean penDown;

    EvtGetPen(&d.title_enter_pos.x, &d.title_enter_pos.y, &penDown);
      }
      break;

    case frmTitleSelectEvent:
      {
    PointType newPos;
    Boolean penDown;

    EvtGetPen(&newPos.x, &newPos.y, &penDown);

    if (newPos.x < (d.title_enter_pos.x - p.min_swipe)
        || newPos.x > (d.title_enter_pos.x + p.min_swipe)) {
      if (!(p.flags & PFLAGS_SWITCH_TITLE_ACTION)) {
        ShortCutPop(scHigh);
        handled = true;
      }
    } else if (p.flags & PFLAGS_SWITCH_TITLE_ACTION) {
      ShortCutPop(scHigh);
      handled = true;
    }
      }
      break;

    case menuOpenEvent:
      SetUpMenu();
      break;
  
    case winExitEvent:
      /* We may not update the title of the form because it is obscured by some
     other window (possibly by a menu). */
      if (e->data.winExit.exitWindow == (WinHandle) FrmGetFormPtr(DiddleTForm)) {
    d.doNotUpdateFormTitle = true;
      }
      break;

    case winEnterEvent:
      /* We can start updating the form title again */
      if (e->data.winEnter.enterWindow == (WinHandle) FrmGetFormPtr(DiddleTForm) 
      && e->data.winEnter.enterWindow == (WinHandle) FrmGetFirstForm()) { 
    d.doNotUpdateFormTitle = false; 
      }
      break;

    default:
      /* do nothing */
    }
  }

  ErrCatch(inErr) {
    if (!d.exiting) {
      EventType e;
      e.eType = appStopEvent;
      d.exiting = true;
      ErrAlert(inErr); /* Show dialog */

#ifdef DIDDLEDEBUG
      p.category = dmAllCategories;
      p.dbI = 0;
      Switch(AppType);
#else
      EvtAddEventToQueue(&e);
#endif /* DIDDLEDEBUG */

      PRINT("*** EXITING from DiddleEvent: p.dbI = %hd", p.dbI);
    }
  } ErrEndCatch;

  return handled;
}

/*
** SwitchBetweenViews - Act on Switch View button
*/
static void SwitchBetweenViews(void) {
  SwitchToDoodleMode();
  switch (d.formID) {
  case DiddleForm:
  case DiddleTForm:
    KeyDown(cmdListView);
    break;
  case DiddleListForm:
  case DiddleThumbnailForm:
  case DiddleThumbnailDetailForm:
    SwitchCategoryForGoto();
    p.dbI = d.top_visible_record;
    FrmGotoForm(p.flags & PFLAGS_WITH_TITLEBAR ? DiddleTForm : DiddleForm);
    break;
  default:
    /* ignore */
  }
}

/*
** PushKeyDownEvent - Push a keyDown event for a specific character onto the queue.
*/
void PushKeyDownEvent(WChar c) {
  EventType e;

  e.eType = keyDownEvent;
  e.data.keyDown.chr = c;

  EvtAddEventToQueue(&e);
}

/*
** PushNonKeyCommandEvent - Push a special event for commands that do not have a specific 
**                          shortcut key defined.
*/
void PushNonKeyCommandEvent(NonKeyCommandCodeType c) {
  EventType e;

  /* First, push a null KeyDownEvent to close any open menus */
  PushKeyDownEvent(nullChr);

  /* Now push the non-key command event */
  e.eType = dbNonKeyCommandEvent;
  e.data.menu.itemID = c;

  EvtAddEventToQueue(&e);
}

/*
** HandleHardwareButton - Act on a hardware key
*/
static Boolean HandleHardwareButton(Int8 index) {
  LocalID dbID;
  UInt16 cardNo;
  DmSearchStateType searchstate;

  if (d.is_xfer_mode) CancelXferMode();

  /* Calculate our index */
  switch (p.hardware_action[index]) {
  case hwNoAction:
    return false; /* ignore */
    break;

  case hwNewRecord:
    SwitchToDoodleMode();
    switch (d.formID) {
    case DiddleForm:
    case DiddleTForm:
      PushNonKeyCommandEvent(nkCmdHardwareNew);
      break;
    case DiddleListForm:
    case DiddleThumbnailForm:
    case DiddleThumbnailDetailForm:
    case TimeForm:
      if (p.dbI == noRecordSelected) p.dbI = 0;
      NewScratchImage();
      FrmGotoForm(p.flags & PFLAGS_WITH_TITLEBAR ? DiddleTForm : DiddleForm);
      SndPlaySystemSound(sndClick);
      break;
    }
    EvtFlushPenQueue();
    break;

  case hwNextRecord:
    if (d.formID == DiddleForm || d.formID == DiddleTForm) {
      SwitchToDoodleMode();
      PushKeyDownEvent(cmdNext);
    }
    break;

  case hwNextCategory:
    SwitchToDoodleMode();
    PushNonKeyCommandEvent(nkCmdNextCategory);
    break;

  case hwListView:
    SwitchBetweenViews();
    break;

  case hwDetails:
    if (d.formID == DiddleForm || d.formID == DiddleTForm)
      PushKeyDownEvent(cmdDetails);
    break;

  default:
    if (DmGetNextDatabaseByTypeCreator(true, &searchstate, sysFileTApplication,
                       p.hardware_action[index] - HW_MAX_ACTIONS,
                       true, &cardNo, &dbID))
      return true;

    SndPlaySystemSound(sndClick);
    SysUIAppSwitch(cardNo, dbID, sysAppLaunchCmdNormalLaunch, 0);
  }

  return true;
}

/*
** SpecialKeyDown - Act on a hardware or special key
*/
static Boolean SpecialKeyDown(EventPtr e) {
  if (e->eType != keyDownEvent) return false;

  if (e->data.keyDown.modifiers & commandKeyMask) {
    const UInt16 chr = e->data.keyDown.chr;

/*     if (e->data.keyDown.modifiers&autoRepeatKeyMask) */
/*       return false; */
    if (e->data.keyDown.modifiers & poweredOnKeyMask)
      return false;
    
    /* Detect menu */
    if (chr == vchrMenu) {
      /* Do not update form title while a menu is visible */
      /* This is a kludge because the canonical way (winEnter/winExit events)
     does not work */
      d.doNotUpdateFormTitle = true;
      return false;
    }

    if (chr == vchrHard1 || chr == vchrHard2 ||
    chr == vchrHard3 || chr == vchrHard4)
      return HandleHardwareButton(chr - vchrHard1);

    if (d.formID == DiddleForm || d.formID == DiddleTForm) {
      /* Support the 5-way navigator */
      if (chr == vchrNavChange) {
    if (NavDirectionPressed(e, Left)) {
      return KeyDown(pageUpChr);
    } else if (NavDirectionPressed(e, Right)) {
      return KeyDown(pageDownChr);
    } else if (NavSelectPressed(e)) {
      KeyDown(cmdListView);
      return true;
    }
      }
      
      /* Support for Sony jog-dial */
      if (chr == vchrJogUp) {
    return false; /* Let the OS handle this */
      } else if (chr == vchrJogDown) {
    return false; /* Let the OS handle this */
      } else if (chr == vchrJogPress) {
    PushKeyDownEvent(cmdListView);
    return true;
      }

      /* Support Handspring special keys */
      if (chr == keyDelete) {
    PushKeyDownEvent(cmdRemove);
    return true;
      } else if (chr == vchrRockerCenter && !d.doNotUpdateFormTitle) {
    KeyDown(cmdListView);
    return true;
      }
    
      /* No softkeys in transfer mode */
      if (d.is_xfer_mode) return false;
    
      if (chr == keyboardAlphaChr) {
    PushKeyDownEvent(cmdTitle);
    return true;
      } else if (chr == keyboardNumericChr) {
    SwitchToDoodleMode();
    ShortCutPop(scLow);
    return true;
      }
    }
  }
   
  return false;
}

/***********************************************************************
 *
 * FUNCTION:    SearchDraw
 *
 * DESCRIPTION: This routine draws the description, date, and time of
 *              an appointment found by the text search routine.
 *
 * PARAMETERS:  apptRecP - pointer to an appointment record.
 *              x        - draw position
 *              y        - draw position
 *              width    - maximum width to draw.
 *
 * RETURNED:     nothing
 *
 ***********************************************************************/

// FIXME - these should be based on the screen width, not hard coded.

#define maxSearchRepeatDescWidth               100
#define maxSearchDateWidth          42
#define maxSearchTimeWidth          39

static void SearchDraw (DiddleBugRecordType* rec, Char* record_name, Int16 x, Int16 y, Int16 width) {
  Char timeStr [timeStringLength];
  Char dateStr [dateStringLength];
  UInt16 len;
  UInt16 maxDescWidth;
  Coord drawX;
  Char* ptr;
  Char* rscP;
  MemHandle rscH;
  DateTimeType when;
  DateFormatType dateFormat;
  TimeFormatType timeFormat;

  if (rec->repeatInfo.repeatType != repeatNone)
    maxDescWidth = maxSearchRepeatDescWidth;
  else
    maxDescWidth = width - maxSearchDateWidth - maxSearchTimeWidth;

  /* Draw the records desciption */
  ptr = StrChr(record_name, linefeedChr);
  len = (ptr == NULL ? StrLen(record_name) : (UInt16)(ptr - record_name));
  WinDrawTruncChars (record_name, len, x, y, maxDescWidth);

  /* If the alarm is repeating, draw the repeat type */
  if (rec->repeatInfo.repeatType != repeatNone) {
    rscH = DmGetResource (strRsc, repeatTypesNoneString + rec->repeatInfo.repeatType);
    rscP = MemHandleLock (rscH);
    x += (width - FntCharsWidth (rscP, StrLen (rscP)));
    WinDrawChars (rscP, StrLen (rscP), x, y);
    MemHandleUnlock (rscH);
  }

  /* Draw the alarm date and time */
  else {
    /* Get time and date formats from the system preferences */
    dateFormat = (DateFormatType)PrefGetPreference(prefDateFormat);
    timeFormat = (TimeFormatType)PrefGetPreference(prefTimeFormat);

    TimSecondsToDateTime(rec->alarmSecs, &when);

    if (rec->alarmSecs != 0) {

      TimeToAscii (when.hour, when.minute, timeFormat, timeStr);
      len = StrLen (timeStr);
      drawX = x + (width - FntCharsWidth (timeStr, len));
      WinDrawChars (timeStr, len, drawX, y);

      DateToAscii (when.month, when.day, when.year, dateFormat, dateStr);
      len = StrLen (dateStr);
      drawX = x + (width - FntCharsWidth (dateStr, len) - maxSearchTimeWidth);
      WinDrawChars (dateStr, len, drawX, y);
    }
  }
}

/***********************************************************************
 *
 * FUNCTION:    Global Find
 *
 * DESCRIPTION: This routine searchs the database for records
 *              containing the string passed (in name or note field).
 *
 * PARAMETERS:  findParams - text search parameter block
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
static void GlobalFind(FindParamsType* params)
{
  UInt16 pos;
  UInt16 recordNum;
  MemHandle recordH, t;
  RectangleType r;
  Boolean done;
  Boolean match;
  DmOpenRef dbR;
  DmSearchStateType searchState;
  Err err;
  UInt16 cardNo = 0;
  LocalID dbID;
  UInt32 longPos;
  UInt16 matchLength;
  Char* record_name;
  Char* record_note;
  MemPtr ptr;
  DiddleBugRecordType record;
  UInt16 total;
  UInt16 attr;
  FindFieldsType field;

  /* Find the application's data file. */
  err = DmGetNextDatabaseByTypeCreator (true, &searchState, DBType, AppType, true, &cardNo, &dbID);
  if (err) {
    params->more = false;
    return;
  }

  /* Open the DiddleBug record database. */
  dbR = DmOpenDatabase(cardNo, dbID, params->dbAccesMode);
  if (!dbR) {
    params->more = false;
    return;
  }

  /* Draw header */
  t = DmGetResource(strRsc, FindHeaderString);
  done = FindDrawHeader(params, MemHandleLock(t));
  MemHandleUnlock(t);
  DmReleaseResource(t);
  if (done) goto Exit;

  /* Search the records for the "find" string. */
  recordNum = params->recordNum;
  total = DmNumRecords(dbR);
  while (true) {
    // Because applications can take a long time to finish a find when
    // the result may be on the screen or for other reasons, users like
    // to be able to stop the find.  Stop the find if an event is pending.
    // This stops if the user does something with the device.  Because
    // this call slows down the search we perform it every so many
    // records instead of every record.  The response time should still
    // be short without introducing much extra work to the search.

    // Note that in the implementation below, if the next 16th record is
    // secret the check doesn't happen.  Generally this shouldn't be a
    // problem since if most of the records are secret then the search
    // won't take long anyways!
    if ((recordNum & 0x000f) == 0 && EvtSysEventAvail(true)) {
      /* Stop the search process */
      params->more = true;
      break;
    }

    recordH = DmQueryNextInCategory (dbR, &recordNum, dmAllCategories);

    /* Have we run out of records? */
    if (!recordH) {
      params->more = false;
      break;
    }

    /* Read record */
    ptr = MemHandleLock(recordH);
    MemMove(&record, ptr, sizeof(DiddleBugRecordType));

    /* Should private records be shown? */
    DmRecordInfo(dbR, recordNum, &attr, NULL, NULL);
    if (attr & dmRecAttrSecret)
      goto Cont;

    /* Read the additional alarm info */
    record_name = ptr + sketchDataOffset + record.sketchLength;
    record_note = record_name + StrLen(record_name) + 1;

    /* Search for the string passed, if it's found display the title of the record. */
    match = TxtFindString(record_name, params->strToFind, &longPos, &matchLength);
    field = ffName;

    if (!match) {
      match = TxtFindString(record_note, params->strToFind, &longPos, &matchLength);
      field = ffNote;
    }

    pos = longPos;

    if (match) {
      /* Add the match to the find parameter block, if there is no room to */
      /* display the match the following function will return true.        */
      done = FindSaveMatch (params, recordNum, pos, field, matchLength, cardNo, dbID);

      if (!done) {
    /* Get the bounds of the region where we will draw the results. */
    FindGetLineBounds (params, &r);

    /* Display the title of the description. */
    SearchDraw(&record, record_name, r.topLeft.x+1, r.topLeft.y, r.extent.x-2);

    params->lineNumber++;
      }
    }

  Cont:
    MemHandleUnlock(recordH);
    if (done) break;

    recordNum++;
  }

 Exit:
  DmCloseDatabase(dbR);
}

/*
** Retrieve preferences (and initialize if no or wrong preferences were found)
**
** Returns: error
*/
Boolean GetPreferences(pref* prefs, UInt16* size, Int16* oldVersion, Boolean hires) {
  Int16 version = 0;
  Boolean error = false;
  UInt16 mySize = sizeof(pref);

  MemSet(prefs, sizeof(pref), 0);
  version = PrefGetAppPreferences(AppType, dbsNormalAppPrefs, prefs, &mySize, true);
  if (version == noPreferenceFound || mySize != sizeof(pref) || prefs->version != PrefVersion) {
    /* Save old version */
    if (oldVersion) {
      if (version != noPreferenceFound)
    *oldVersion = prefs->version;
      else
    *oldVersion = noPreferenceFound;
    }

    /* Set up defaults */
    prefs->version = PrefVersion;
    prefs->dbI = 0;
    prefs->formID = DiddleTForm;
    prefs->flags = PFLAGS_CONFIRM_CLEAR | PFLAGS_CONFIRM_DELETE /* flags - confirm clear, confirm delete */
      | PFLAGS_XFER_GOTO | PFLAGS_WITH_TITLEBAR; /* flags - xfer goto, titlebar on */
    RctSetRectangle(&prefs->r,
            screen_above_bar_rect.topLeft.x + title_bar_rect.extent.x,
            screen_above_bar_rect.topLeft.y,
            screen_above_bar_rect.extent.x,
            screen_above_bar_rect.extent.y - title_bar_rect.extent.y);
    prefs->modeSelection = dbmSelectionPaint;
    prefs->filtSelection = dbFilterRough;
    prefs->penSelection = dbPenMedium;
    prefs->erasePenSelection = dbPenBroad;
    prefs->inkSelection = dbInkBlack;
    prefs->filt = 4;
    if (!hires)
      MemMove(&prefs->penpix, &penpixMedium, sizeof(penpixMedium));
    else
      MemMove(&prefs->penpix, &penpixBold, sizeof(penpixBold));
    MemMove(&prefs->inkpat, &patternBlack, sizeof(patternBlack));
    prefs->txt_font = stdFont;
    prefs->txt_mode = dbTextBlack;
    prefs->shape_type = dbShapeRectangle;
    prefs->shape_fill = dbsFillOutline;
    prefs->category = dmAllCategories;
    prefs->xfer_current_plug = 0;
    prefs->hardware_action[0] = hwNoAction;
    prefs->hardware_action[1] = hwNoAction;
    prefs->hardware_action[2] = hwNoAction;
    prefs->hardware_action[3] = hwNoAction;
    prefs->min_swipe = 12;
    prefs->shape_ink = dbInkBlack;
    prefs->shape_pen = dbPenMedium;
    prefs->std_alarm = defaultAlarm;
    prefs->remind_alarm = defaultAlarm;
    prefs->alarm_repeat_times = 1;
    prefs->alarm_auto_snooze_delay = 5; /* in minutes */
    prefs->alarm_auto_snooze_times = 3;

    error = true;
  } else {
    /* ...but we still have to check for the obsolete "smear" mode */
    if (prefs->modeSelection == dbmSelectionSmear)
      prefs->modeSelection = dbmSelectionPaint;
  }

  /* Return the preferences size, if wanted by the caller */
  if (size) *size = mySize;

  return error;
}

/*
** Retrieve extended preferences (and initialize if no or wrong preferences were found)
**
** Returns: error
*/
Boolean GetExtendedPreferences(extended_pref* prefs, UInt16* size, Int16* oldVersion) {
  Int16 version;
  Boolean error;

  MemSet(prefs, sizeof(extended_pref), 0);
  *size = sizeof(extended_pref);
  version = PrefGetAppPreferences(AppType, dbsExtendedAppPrefs, prefs, size, true);
  if (version == noPreferenceFound || *size != sizeof(extended_pref)
      || prefs->version != ExtendedPrefVersion) {
    /* Save old version */
    if (oldVersion) {
      if (version != noPreferenceFound)
    *oldVersion = prefs->version;
      else
    *oldVersion = noPreferenceFound;
    }

    /* Set up defaults */
    prefs->version = ExtendedPrefVersion;
    prefs->eraseFiltSelection = dbFilterRough;
    prefs->listFormID = DiddleListForm;
    prefs->defaultPriority = 2; /* Priority is zero-based */
    prefs->paperColor = 0; /* white */
    prefs->inkColor = WinRGBToIndex(&black); /* black */
    error = true;
  }
  else {
    /* Everything OK... */
    error = false;

    /* ...but we still need to check for correct pen and paper colors */
    if (prefs->inkColor == prefs->paperColor && prefs->inkColor == 0) {
      prefs->paperColor = 0; /* white */
      prefs->inkColor = WinRGBToIndex(&black); /* black */
    }
  }

  return error;
}

/*
** CloseDatabases - you know what it does
*/
static void CloseDatabases(void) {
  DmCloseDatabase(d.dbR);
}

/*
** Upgrade repeat format
*/
static void UpgradeRepeat(Int16 oldRepeatMode, DiddleBugRecordType* rec) {
  DateTimeType date;

  switch (oldRepeatMode) {
  case REPEAT_NONE:
    rec->repeatInfo.repeatType = repeatNone;
    break;
  case REPEAT_HOURLY:
    rec->repeatInfo.repeatType = repeatHourly;
    rec->repeatInfo.repeatFrequency = 1;
    break;
  case REPEAT_DAILY:
    rec->repeatInfo.repeatType = repeatDaily;
    rec->repeatInfo.repeatFrequency = 1;
    break;
  case REPEAT_WEEKDAYS:
    rec->repeatInfo.repeatType = repeatWeekly;
    rec->repeatInfo.repeatOn = REPEATBITS_WEEKDAYS;
    rec->repeatInfo.repeatFrequency = 1;
    break;
  case REPEAT_WEEKLY:
  case REPEAT_BIWEEKLY:
    TimSecondsToDateTime(rec->alarmSecs, &date);
    rec->repeatInfo.repeatType = repeatWeekly;
    rec->repeatInfo.repeatOn = (1 << date.weekDay);

    if (oldRepeatMode == REPEAT_WEEKLY)
      rec->repeatInfo.repeatFrequency = 1;
    else
      rec->repeatInfo.repeatFrequency = 2;
    break;
  }

  /* upgraded record don't have an end date set */
  DateToInt(rec->repeatInfo.repeatEndDate) = apptNoEndDate;
}

/*
** UpgradeRecordFormat
*/
static void UpgradeRecordFormat(void) {
  UInt16 i, num_records;
  MemHandle t;
  MemPtr ptr, winPtr;
  DeprecatedAlarmType alarm;
  DiddleBugRecordType record;
  UInt16 sketchSize;

  num_records = DmNumRecords(d.dbR);
  for (i=0; i<num_records; i++) {
    /* Open and lock the old record */
    t = DmQueryRecord(d.dbR, i);
    if (!t) abort();
    ptr = MemHandleLock(t);
    /* Read the old alarm data */
    MemMove(&alarm, ptr+3200, sizeof(DeprecatedAlarmType));
    /* Copy the old sketch data to the buffer */
    winPtr = BmpGetBits(WinGetBitmap(d.winbufM));
    MemMove(winPtr, ptr, 3200);
    /* Close the old record */
    MemHandleUnlock(t);
    /* The compression routine uses p.dbI */
    sketchSize = 0;
    ptr = CompressSketch(d.hires, d.dbR, i, d.record_name, d.record_note,
             &d.record_sound, &d.record.extraLength,
             &sketchSize, winPtr);
    /* Write the high data to disk */
    DmSet(ptr, sketchDataOffset+sketchSize, 2, 0);
    /* Fill the record structure */
    MemSet(&record, sizeof(DiddleBugRecordType), 0);
    record.sketchLength = sketchSize;
    record.alarmSecs = alarm.alarm_secs;
    UpgradeRepeat(alarm.repeat_mode, &record);
    if (alarm.is_alarm_set) record.flags |= RECORD_FLAG_ALARM_SET;
    if (alarm.is_countdown) record.flags |= RECORD_FLAG_COUNTDOWN;
    if(DmWrite(ptr, 0, &record, sizeof(DiddleBugRecordType)))
      abort();
    MemPtrUnlock(ptr);
    DmReleaseRecord(d.dbR, i, true);
  }

  /* Back to normal */
  p.dbI = 0;
}

/*
** Checks for (at least) a certain PalmOS version.
*/
Boolean CheckROMVersion(UInt32 requiredVersion) {
  UInt32 romVersion;

  /* See if we're on in minimum required version of the ROM or later */
  FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);
  if (romVersion >= requiredVersion)
    return true;
  else
    return false;
}

/***********************************************************************
 *
 * FUNCTION:    RomVersionCompatible
 *
 * DESCRIPTION: This routine checks that a ROM version is meet your
 *              minimum requirement.
 *
 * PARAMETERS:  requiredVersion - minimum rom version required
 *                                (see sysFtrNumROMVersion in SystemMgr.h
 *                                for format)
 *              launchFlags     - flags that indicate if the application
 *                                UI is initialized.
 *
 * RETURNED:    error code or zero if rom is compatible
 *
 ***********************************************************************/
static Err RomVersionCompatible(UInt32 requiredVersion, UInt16 launchFlags) {
  UInt32 romVersion;

  /* See if we're on in minimum required version of the ROM or later */
  FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);
  if (romVersion < requiredVersion) {
    if ((launchFlags & (sysAppLaunchFlagNewGlobals | sysAppLaunchFlagUIApp)) ==
    (sysAppLaunchFlagNewGlobals | sysAppLaunchFlagUIApp)) {
      FrmAlert (RomIncompatibleAlert);

      /* Palm OS 1.0 will continuously relaunch this app unless we switch to */
      /* another safe one.                                                   */
      if (romVersion < sysVersion20) {
    AppLaunchWithCommand(sysFileCDefaultApp, sysAppLaunchCmdNormalLaunch, NULL);
      }
    }

    return sysErrRomIncompatible;
  }

  return errNone;
}

/*
** UpgradeRecordFormat from 2.15 to 2.50
*/
static void UpgradeRecordFormat2_15(void) {
  /* We need to upgrade the repeat format */
  UInt16 i, num_records;
  MemHandle t;
  MemPtr ptr;
  DiddleBugRecordType record;

  num_records = DmNumRecords(d.dbR);
  for (i = 0; i < num_records; ++i) {
    /* Open and lock the old record */
    t = DmQueryRecord(d.dbR, i);
    if (!t) abort();
    ptr = MemHandleLock(t);

    /* Read the old record */
    MemMove(&record, ptr, sizeof(DiddleBugRecordType));

    /* Upgrade repeat mode */
    UpgradeRepeat(record.repeatMode, &record);
    if(DmWrite(ptr, 0, &record, sizeof(DiddleBugRecordType)))
      abort();

    /* Zero out the name & note */
    DmSet(ptr, sketchDataOffset + record.sketchLength, 2, 0x00);

    /* Close the old record */
    MemHandleUnlock(t);
    DmReleaseRecord(d.dbR, i, true);
  }
}

/*
** UpgradeRecordFormat to hi-res
*/
static void UpgradeRecordFormatHiRes(void) {
  UInt16 i, num_records;
  MemHandle t1 = NULL, t2 = NULL;
  MemPtr ptr1 = NULL, ptr2 = NULL;
  DiddleBugRecordType record;
  DmSearchStateType search;
  UInt16 cardNo;
  LocalID dbID, dbIDNew;
  DmOpenRef newDBRef, oldDBRef;
  Char dbName[dmDBNameLength];
  UInt16 mode = dmModeReadWrite | dmModeShowSecret;
  Err err;
  BitmapType* srcBmp = NULL;
  BitmapType* dstBmp = NULL;
  BitmapType* tmpBmp = NULL;
  BitmapTypeV3* tmpBmpV3 = NULL;
  WinHandle dstWin = NULL, oldH = NULL;
  UInt32 additionalSize = 0;
  UInt32 highDataOffset;
  Int16 len;
  MemHandle name = NULL, note = NULL;
  AlarmSoundInfoType rec_sound;
  LocalID appInfoID1;
  UInt16 attr;

  /* Should we upgrade the old database? */
  err = DmGetNextDatabaseByTypeCreator(true, &search, DBType, AppType, true, 
                       &cardNo, &dbID);
  if (err) return;
  err = DmDatabaseInfo(cardNo, dbID, dbName, NULL, NULL, NULL, NULL, 
               NULL, NULL, NULL, NULL, NULL, NULL);
  if (err) return; /* abort? */
  if (!StrCompare(dbName, DBNameHR) || DmFindDatabase(cardNo, DBNameHR))
    return;

  /* Close lo-res database */
  DmCloseDatabase(d.dbR);
  d.dbR = NULL;

  /* Explicitely re-open old lo-res database */
  oldDBRef = DmOpenDatabase(cardNo, dbID, mode);

  /* Create new hi-res database */
  if ((err = DmCreateDatabase(0, DBNameHR, AppType, DBType, false)))
    abort();
  dbIDNew = DmFindDatabase(cardNo, DBNameHR);
  newDBRef = DmOpenDatabase(cardNo, dbIDNew, mode);

  /* Set up the relevant bitmaps and windows */
  srcBmp = BmpCreate(MAX_PIXELS, MAX_PIXELS, 1, NULL, &err);
  if (err) abort();
  if (d.hires) {
    if (!d.sonyClie) {
      tmpBmp = BmpCreate(MAX_PIXELS_HIRES, MAX_PIXELS_HIRES, 1, NULL, &err);
      
      if (err) abort();
      tmpBmpV3 = BmpCreateBitmapV3(tmpBmp, kDensityDouble, 
                   BmpGetBits(tmpBmp), NULL);
      dstBmp = (BitmapType*) tmpBmpV3;
      dstWin = WinCreateBitmapWindow(dstBmp, &err);
      if (err) abort();
    } else {
      tmpBmp = HRBmpCreate(d.sonyHRRefNum, MAX_PIXELS_HIRES, MAX_PIXELS_HIRES,
               1, NULL, &err);
      if (err) abort();
      dstBmp = tmpBmp;
      dstWin = HRWinCreateBitmapWindow(d.sonyHRRefNum, dstBmp, &err);
      if (err) abort();
    }
  }

  /* Set up draw state */
  oldH = WinSetDrawWindow(dstWin);
  WinPushDrawState();
  WinSetBackColor(0);
  WinSetForeColor(1);

  num_records = DmNumRecords(oldDBRef);
  for (i = 0; i < num_records; ++i) {
    /* Open and lock the old record */
    t1 = DmQueryRecord(oldDBRef, i);
    if (!t1) abort();
    ptr1 = MemHandleLock(t1);

    /* Read the old record */
    MemMove(&record, ptr1, sizeof(DiddleBugRecordType));

    /* Read lo-res sketch */
    if (!UncompressSketch(ptr1 + sketchDataOffset + sketchThumbnailSize + 1,
              BmpGetBits(srcBmp),
              record.sketchLength - sketchThumbnailSize - 1, false)) {
      /* An error occured while uncompressing the sketch */
      abort();
    }

    /* Read the additional alarm info */
    highDataOffset = sketchDataOffset + record.sketchLength;

    len = StrLen((Char*)(ptr1 + highDataOffset)) + 1; /* +1 for null char */
    if (name) MemHandleFree(name);
    name = MemHandleNew(len);
    ASSERT(name);
    MemMove(MemHandleLock(name), ptr1 + highDataOffset, len);
    MemHandleUnlock(name);

    highDataOffset += len;

    len = StrLen((Char*)(ptr1 + highDataOffset)) + 1; /* +1 for null char */
    if (note) MemHandleFree(note);
    note = MemHandleNew(len);
    ASSERT(note);
    MemMove(MemHandleLock(note), ptr1 + highDataOffset, len);
    MemHandleUnlock(note);

    highDataOffset += len;

    /* Clear old data since there may not be an extra-data block yet */
    MemSet(&rec_sound, sizeof(AlarmSoundInfoType), 0);
    rec_sound.listIndex = -1; /* default */
    /* Read new extra-data (if it exists and is from a compatible version) */
    if (record.extraLength == sizeof(AlarmSoundInfoType))
      MemMove(&rec_sound, ptr1 + highDataOffset, record.extraLength);

    /* Copy it to the hi-res bitmap */
    if (!d.sonyClie) {
      WinDrawBitmap(srcBmp, 0, 0);
    } else {
      Coord x, y;
      PointType pts[4];
      WinHandle srcWin = WinCreateBitmapWindow(srcBmp, &err);
      if (err) abort();

      /* Clear destination window */
      WinSetDrawWindow(dstWin);
      WinEraseWindow();

      for (x = 0; x < MAX_PIXELS; ++x) {
    for (y = 0; y < MAX_PIXELS; ++y) {
      WinSetDrawWindow(srcWin);
      if (WinGetPixel(x, y)) {
        const Coord x2 = x * HiResCoef;
        const Coord y2 = y * HiResCoef;
        
        pts[0].x = pts[1].x = x2;
        pts[2].x = pts[3].x = x2 + 1;
        pts[0].y = pts[2].y = y2;
        pts[1].y = pts[3].y = y2 + 1;
        
        WinSetDrawWindow(dstWin);
        HRWinPaintPixels(d.sonyHRRefNum, 4, pts);
      }
    }
      }

      /* Clean up */
      WinDeleteWindow(srcWin, false);
    }

    /* Create new record in hi-res database */
    t2 = DmNewRecord(newDBRef, &i, MinRecordSize(true) + additionalSize);
    if (!t2) 
      DatabaseError(newDBRef, i);
    DmReleaseRecord(newDBRef, i, true);

    /* Write hi-res sketch */
    ptr2 = CompressSketch(true, newDBRef, i, name, note, &rec_sound, &record.extraLength, &record.sketchLength, BmpGetBits(dstBmp));

    /* Write basic record information */
    if(DmWrite(ptr2, 0, &record, sizeof(DiddleBugRecordType)))
      abort();

    /* Close the old & new record */
    MemHandleUnlock(t1);
    MemPtrUnlock(ptr2);
    DmReleaseRecord(newDBRef, i, true);

    /* Copy the record attributes (category, private status, ...) */
    DmRecordInfo(oldDBRef, i, &attr, NULL, NULL);
    DmSetRecordInfo(newDBRef, i, &attr, NULL);
  } 

  /* Restore old draw state */ 
  WinPopDrawState();
  WinSetDrawWindow(oldH);

  /* Clean up windows... */
  if (dstWin) WinDeleteWindow(dstWin, false);
  /* ...and bitmaps... */
  if (srcBmp) BmpDelete(srcBmp);
  if (tmpBmp) BmpDelete(tmpBmp);
  if (tmpBmpV3) BmpDelete((BitmapType*) tmpBmpV3);
  /* ...and miscellaneous memory */
  if (name) MemHandleFree(name);
  if (note) MemHandleFree(note);
  
  /* Copy AppInfo block if it exists */
  appInfoID1 = DmGetAppInfoID(oldDBRef);
  if (appInfoID1 != 0) {
    LocalID appInfoID2;
    MemHandle appInfoH2;
    AppInfoPtr appInfoP1, appInfoP2;

    appInfoH2 = DmNewHandle(newDBRef, sizeof(AppInfoType));
    if (!appInfoH2) abort();
    appInfoID2 = MemHandleToLocalID(appInfoH2);
    DmSetDatabaseInfo(0, dbIDNew, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
              &appInfoID2, NULL, NULL, NULL);
    appInfoP1 = MemLocalIDToLockedPtr(appInfoID1, 0);
    appInfoP2 = MemLocalIDToLockedPtr(appInfoID2, 0);
    DmWrite(appInfoP2, 0, appInfoP1, sizeof(AppInfoType));

    /* Clean up */
    MemPtrUnlock(appInfoP1);
    MemPtrUnlock(appInfoP2);
  }

  /* Close databases */
  err = DmCloseDatabase(oldDBRef);
  if (err) abort();
  err = DmCloseDatabase(newDBRef);
  if (err) abort();

  /* Delete old database */
  err = DmDeleteDatabase(cardNo, dbID);
  if (err) abort();

  /* Set the "real" read mode */
  mode = dmModeReadWrite;
  /* Get the setting for private records */
  d.privateRecordStatus = PrefGetPreference(prefShowPrivateRecords);
  if (d.privateRecordStatus != hidePrivateRecords)
    mode |= dmModeShowSecret;
  /* Re-open database */
  d.dbR = DmOpenDatabaseByTypeCreator(DBType, AppType, mode);
  if (!d.dbR) abort();
}

/*
** Create bitmaps
*/
static void CreateBitmaps(void) {
  Err err = errNone;
  BitmapTypeV3* enhancedBmp;

  /* Create the offscreen buffer */
  if (d.hires) {
    if (!d.sonyClie) {
      d.winbufBmp = BmpCreate(MAX_PIXELS_HIRES, MAX_PIXELS_HIRES, 1, NULL, &err);
      if (err) abort();
      enhancedBmp = BmpCreateBitmapV3(d.winbufBmp, kDensityDouble, BmpGetBits(d.winbufBmp), NULL);
      d.winbufM = WinCreateBitmapWindow((BitmapType*) enhancedBmp, &err);
      if (err) abort();
    } else {
      d.winbufBmp = HRBmpCreate(d.sonyHRRefNum, MAX_PIXELS_HIRES, MAX_PIXELS_HIRES, 1, NULL, &err);
      if (err) abort();
      d.winbufM = HRWinCreateBitmapWindow(d.sonyHRRefNum, d.winbufBmp, &err);
      if (err) abort();
      d.winbufBmp = NULL; /* Otherwise the bitmap would be deleted twice */
    }
  } else {
      d.winbufBmp = BmpCreate(MAX_PIXELS, MAX_PIXELS, 1, NULL, &err);
      if (err) abort();
      d.winbufM = WinCreateBitmapWindow(d.winbufBmp, &err);
      if (err) abort();
      d.winbufBmp = NULL; /* Otherwise the bitmap would be deleted twice */
  }

  /* Create the conversion bitmap */
  if (d.hires) {
    if (!d.sonyClie) {
      d.realCanvasBmp = BmpCreate(MAX_PIXELS_HIRES, MAX_PIXELS_HIRES, 1, NULL, &err);
      if (err) abort();
      enhancedBmp = BmpCreateBitmapV3(d.realCanvasBmp, kDensityDouble, BmpGetBits(d.realCanvasBmp), NULL);
      d.realCanvas = WinCreateBitmapWindow((BitmapType*) enhancedBmp, &err);
      if (err) abort();
    } else {
      d.realCanvasBmp = HRBmpCreate(d.sonyHRRefNum, MAX_PIXELS_HIRES, MAX_PIXELS_HIRES, 1, NULL, &err);
      if (err) abort();
      d.realCanvas = HRWinCreateBitmapWindow(d.sonyHRRefNum, d.realCanvasBmp, &err);
      if (err) abort();
      d.realCanvasBmp = NULL; /* Otherwise the bitmap would be deleted twice */
    }
  } else {
    d.realCanvasBmp = BmpCreate(MAX_PIXELS, MAX_PIXELS, 1, NULL, &err);
    if (err) abort();
    d.realCanvas = WinCreateBitmapWindow(d.realCanvasBmp, &err);
    if (err) abort();
    d.realCanvasBmp = NULL; /* Otherwise the bitmap would be deleted twice */
  }
}

/*
** OpenDatabases - Open databases and set backup bit
*/
static Err OpenDatabases(void) {
  Err err;
  MemHandle h;
  LocalID dbID;
  LocalID appInfoID;
  UInt16 dbAttrs;
  UInt16 cardNo;
  DmSearchStateType searchstate;
  AppInfoPtr appInfoP;
  UInt16 prefsSize;
  UInt16 oldVersion;
  FormPtr frm;
  ListPtr list;
  UInt32 linkerparam;
  UInt16 mode = dmModeReadWrite;
  const TimeFormatType timeFormat = PrefGetPreference(prefTimeFormat);

  /* Check for at least PalmOS 3.5 done on startup */

#ifdef DIDDLEDEBUG
  HostTraceInit();
#endif /* DIDDLEDEBUG */

  d.handera = false;

  /* Create buffer bitmaps */
  CreateBitmaps();

  /* Get the setting for private records */
  d.privateRecordStatus = PrefGetPreference(prefShowPrivateRecords);
  if (d.privateRecordStatus != hidePrivateRecords)
    mode |= dmModeShowSecret;

  /* Open scratch database and get preferences */
  if ((d.dbR = DmOpenDatabaseByTypeCreator(DBType, AppType, mode)) != NULL) {
    if (GetPreferences(&p, &prefsSize, &oldVersion, d.hires)) {
      /* Upgrade records for versions before 2.6 */
      if (!oldVersion == noPreferenceFound) {
    if (oldVersion < 6)
      UpgradeRecordFormat();
    /* or before 2.50 */
    else if (oldVersion < 7)
      UpgradeRecordFormat2_15();
      }

      /* Check if we need to upgrade the database format */
      if (d.hires)
    UpgradeRecordFormatHiRes();

      /* Correctly set PFLAGS_WITH_TITLEBAR */
      if (p.formID == DiddleTForm)
    p.flags |= PFLAGS_WITH_TITLEBAR;
    }

    /* Also load extended preferences */
    if (GetExtendedPreferences(&ep, &prefsSize, &oldVersion)) {
      if (!oldVersion == noPreferenceFound) {
    /* Do any necessary upgrades here */
      }
    }
  } else {
    if (d.hires) {
      if ((err = DmCreateDatabase(0, DBNameHR, AppType, DBType, false)))
    return err;
    } else {
      if ((err = DmCreateDatabase(0, DBName, AppType, DBType, false)))
    return err;
    }
    d.dbR = DmOpenDatabaseByTypeCreator(DBType, AppType, mode);

    GetPreferences(&p, &prefsSize, NULL, d.hires);

    /* Also load extended preferences */
    GetExtendedPreferences(&ep, &prefsSize, NULL);
  }

  /* Set backup bits */
  if ((err = DmOpenDatabaseInfo(d.dbR, &dbID, NULL, NULL, &cardNo, NULL))) {
    CloseDatabases();
    return err;
  }
  if ((err = DmDatabaseInfo(0, dbID, NULL, &dbAttrs, NULL, NULL, NULL,
                NULL, NULL, NULL, NULL, NULL, NULL))) {
    CloseDatabases();
    return err;
  }
  dbAttrs |= dmHdrAttrBackup;

  /* Create AppInfo block if necessary */
  appInfoID = DmGetAppInfoID(d.dbR);
  if (appInfoID == 0) {
    h = DmNewHandle(d.dbR, sizeof(AppInfoType));
    if (!h) return dmErrMemError;
    appInfoID = MemHandleToLocalID(h);
    DmSetDatabaseInfo(0, dbID, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
              &appInfoID, NULL, NULL, NULL);
    appInfoP = MemLocalIDToLockedPtr(appInfoID, 0);
    DmSet(appInfoP, 0, sizeof(AppInfoType), 0);
    CategoryInitialize(appInfoP, DefaultCategories);
    MemPtrUnlock(appInfoP);
  }
  if ((err = DmSetDatabaseInfo(0, dbID, NULL, &dbAttrs, NULL, NULL,
                   NULL, NULL, NULL, NULL, NULL, NULL,
                   NULL))) {
    CloseDatabases();
    return err;
  }

  /* Get the cardNo and dbID of the application (for alarms) */
  if ((err = DmGetNextDatabaseByTypeCreator(true, &searchstate,
                        sysFileTApplication, AppType, true,
                        &(d.cardNo), &(d.dbID)))) {
    CloseDatabases();
    return err;
  }
  
  /* Get the localized preferences */
  switch (timeFormat) {
  case tfColon24h:
  case tfHours24h:
    d.shorttime = tfColon24h;
    break;
  case tfDot:
  case tfDotAMPM:
    d.shorttime = tfDot;
    break;
  case tfDot24h:
    d.shorttime = tfDot24h;
    break;
  case tfComma24h:
    d.shorttime = tfComma24h;
    break;
  case tfColon:
  case tfColonAMPM:
  case tfHoursAMPM:
  default:
    d.shorttime = tfColon;
    break;
  }

  /* Set the ShortCut popup list width */
  frm = FrmInitForm(DiddleTForm);
  list = GetObjectPointer(frm, PopShortcutList);
  d.shortcut_width = GetListWidth(list);
  FrmDeleteForm(frm);

  /* Set the Coutdown popup list width */
  d.alarm_select_width = GetAlarmSelectListWidth();

  /* Set category cache for list view */
  d.records_in_cat = DmNumRecordsInCategory(d.dbR, p.category);
  if (d.records_in_cat)
    SetTopVisibleRecord(0);
  else
    d.top_visible_record = d.top_row_pos_in_cat = 0;

  /* We "initialize" this here because the new egcs compiler is
   * flakey about initializing pointers */
  if (d.hires) {
    d.penpixList[0] = penpixMedium;
    d.penpixList[1] = penpixBold;
    d.penpixList[2] = penpixBroad;
    d.penpixList[3] = penpixVeryBroad;
    d.penpixList[4] = penpixMediumItalic;
    d.penpixList[5] = penpixBroadItalic;
    d.penpixList[6] = penpixVeryBroadItalic;
  } else {
    d.penpixList[0] = penpixFine;
    d.penpixList[1] = penpixMedium;
    d.penpixList[2] = penpixBold;
    d.penpixList[3] = penpixBroad;
    d.penpixList[4] = penpixFineItalic;
    d.penpixList[5] = penpixMediumItalic;
    d.penpixList[6] = penpixBroadItalic;
  }
  d.inkList[0] = &patternWhite;
  d.inkList[1] = &patternDith12;
  d.inkList[2] = &patternDith25;
  d.inkList[3] = &patternDith37;
  d.inkList[4] = &patternDith50;
  d.inkList[5] = &patternDith62;
  d.inkList[6] = &patternDith75;
  d.inkList[7] = &patternDith87;
  d.inkList[8] = &patternBlack;

  /* Check to see if Linker is available */
  if (!FtrGet('Linx', 1001, &linkerparam)) {
    d.linker_available = true;
  }

  /* Initialize alarm mechanism */
  AlarmInit();

  /* Might as well do a sanity check that the correct alarm is set */
  RescheduleAlarms(d.dbR);

  return 0;
}

/*
** TimeUntilNextTimePeriod - return the number of ticks to wait
*/
static Int32 TimeUntilNextTimePeriod(void) {
  const UInt32 realAlarmSecs = ApptGetAlarmTime(&d.record, TimGetSeconds());
  const UInt32 ticks = TimGetTicks();
  Int32 timeRemaining = d.nextPeriod - ticks;

  if (d.is_xfer_mode) 
    return evtWaitForever;

  if (timeRemaining < 0)
    timeRemaining = 0;

  if ((d.formID == DiddleForm) || (d.formID == DiddleTForm)) {
    if (!timeRemaining) {
      if (d.formID == DiddleTForm) LabelTitle();
      if ((recordIsCountdown || realAlarmSecs == d.record.snoozeSecs) &&
      recordIsAlarmSet) {
    LabelAlarm();
    d.nextPeriod = ticks + 100;
      } else {
    d.nextPeriod = ticks + 6000;
      }
    } else if ((recordIsCountdown|| realAlarmSecs == d.record.snoozeSecs) &&
           recordIsAlarmSet && (d.nextPeriod > (ticks + 100))) {
      d.nextPeriod = ticks+100;
    }
  } else {
    timeRemaining = evtWaitForever;
  }
  
  return timeRemaining;
}

/*
** Handle the new (OS 3.5) command bar
*/
static Boolean CommandBar(EventPtr e) {
  if (e->eType != menuCmdBarOpenEvent || d.is_xfer_mode)
    return false;

  switch (d.formID) {
  case DiddleForm:
  case DiddleTForm:
    MenuCmdBarAddButton(menuCmdBarOnRight, BarInfoBitmap,
            menuCmdBarResultMenuItem, menuitemID_CmdDetails, NULL);
    MenuCmdBarAddButton(menuCmdBarOnRight, BarBeamBitmap,
            menuCmdBarResultMenuItem, menuitemID_CmdBeam, NULL);
    MenuCmdBarAddButton(menuCmdBarOnRight, BarSecureBitmap,
            menuCmdBarResultMenuItem, menuitemID_CmdSecurity, NULL);

    if (recordIsDirty) {
      MenuCmdBarAddButton(menuCmdBarOnRight, BarUndoBitmap,
              menuCmdBarResultMenuItem, menuitemID_CmdSketchUndo, NULL);
    }
    break;

  case DiddleListForm:
  case DiddleThumbnailForm:
  case DiddleThumbnailDetailForm:
    /* do nothing */
    break;

  default:
    /* do nothing */
  }
  
  /* Signal that the command bar may need to be erased */
  d.erase_menu = true;
  
  return false; /* this event MUST fall through to the system */
}

/*
** EventLoop - handle events
*/
static void EventLoop(void) {
  ErrTry {
    EventType e;
    Err error = errNone;
    FormPtr frm = NULL;
    
    do {
      EvtGetEvent(&e, TimeUntilNextTimePeriod());

      if (SpecialKeyDown(&e)) continue;
      
      if (SysHandleEvent(&e)) continue;
      
      if (MenuHandleEvent(NULL, &e, &error)) continue;
      
      if (CommandBar(&e)) continue;
      
      if ((e.eType == frmLoadEvent) && (d.formID != e.data.frmLoad.formID)) {
    frm = FrmInitForm(d.formID = e.data.frmLoad.formID);
    FrmSetActiveForm(frm);
    switch (d.formID) {
    case DiddleForm:
    case DiddleTForm:
      p.formID = d.formID;
      FrmSetEventHandler(frm, DiddleEvent);
      break;
    case DiddleListForm:
      p.formID = ep.listFormID = d.formID;
      FrmSetEventHandler(frm, ListViewHandleEvent);
      break;
    case DiddleThumbnailForm:
      p.formID = ep.listFormID = d.formID;
      FrmSetEventHandler(frm, ThumbnailViewHandleEvent);
      break;
    case DiddleThumbnailDetailForm:
      p.formID = ep.listFormID = d.formID;
      FrmSetEventHandler(frm, ThumbnailDetailViewHandleEvent);
      break;
    case TimeForm:
      FrmSetEventHandler(frm, TimeEvent);
      break;
    case PrefForm:
      FrmSetEventHandler(frm, PrefEvent);
      break;
    case PrefExtForm:
      FrmSetEventHandler(frm, PrefExtEvent);
      break;
    case PrefHWForm:
      FrmSetEventHandler(frm, PrefHWEvent);
      break;
    case RecordDetailsForm:
      FrmSetEventHandler(frm, RecordDetailsEvent);
      break;
    }
    continue;
      }
      
      FrmDispatchEvent(&e);
      
    } while (e.eType != appStopEvent);
  }
  ErrCatch(inErr) {
    if (!d.exiting) {
      EventType e;
      e.eType = appStopEvent;
      d.exiting = true;
      ErrAlert(inErr); /* Show dialog */

#ifdef DIDDLEDEBUG
      p.category = dmAllCategories;
      p.dbI = 0;
      Switch(AppType);
#else 
      EvtAddEventToQueue(&e);
#endif /* DIDDLEDEBUG */

      PRINT("*** EXITING from EventLoop: p.dbI = %hd", p.dbI);
    }
  } ErrEndCatch;
}

/*
** Free a window and its associated bitmap.
*/
void FreeWindowAndBitmap(WinHandle h, BitmapType* bmp) {
  BitmapType* bmp2;

  if (h) {
    bmp2 = WinGetBitmap(h);
    WinDeleteWindow(h, false);
    BmpDelete(bmp2);

    if (bmp)
      BmpDelete(bmp);
  }
}

/*
** StopApplication
*/
static void StopApplication(void) {
  FrmCloseAllForms();
  PrefSetAppPreferences(AppType, dbsNormalAppPrefs, 1, &p, sizeof(pref), true);
  PrefSetAppPreferences(AppType, dbsExtendedAppPrefs, 1, &ep, sizeof(extended_pref), true);

  /* Free memory */
  if (d.record_name) MemHandleFree(d.record_name);
  if (d.record_note) MemHandleFree(d.record_note);
  if (d.midi_list) MemHandleFree(d.midi_list);
  if (d.xfer.pluglistH) MemHandleFree(d.xfer.pluglistH);

  FreeWindowAndBitmap(d.winbufM, d.winbufBmp);
  FreeWindowAndBitmap(d.realCanvas, d.realCanvasBmp);
  FreeWindowAndBitmap(d.clipboard, d.clipboardBmp);

  /* Clean up HR library we're running on a Sony */
  if (d.sonyClie && d.hires) {
    HRClose(d.sonyHRRefNum);

    if (d.sonyLoadedHRLib)
      SysLibRemove(d.sonyHRRefNum);
  }

#ifdef DIDDLEDEBUG
  HostTraceClose();
#endif /* DIDDLEDEBUG */

  CloseDatabases();
}

/*
** BeamSend
*/
static void BeamSend(void) {
  ExgSocketType exgSock;
  MemHandle t;
  MemPtr ptr;
  UInt16 pos, bytes_to_send, bytes_sent;
  UInt16 recSize;
  UInt8 buf[BEAM_BUF_SIZE];
  Char tempString[32];
  UInt8 tempSize;
  Char* tmp;
  Err err;

  /* Save the current data */
  FlushToDisk(true);

  /* Zero the exchange socket */
  MemSet(&exgSock, sizeof(ExgSocketType), 0);

  /* Set the target */
  exgSock.target = AppType;

  /* Set the name and description */
  SysCopyStringResource(tempString, BeamContentString);
  tempSize = StrLen(tempString) + 1;
  exgSock.name = MemPtrNew(tempSize);
  ASSERT(exgSock.name);
  StrNCopy(exgSock.name, tempString, tempSize);
  SysCopyStringResource(tempString, BeamTypeString);
  tempSize = StrLen(tempString) + 1;
  exgSock.description = MemPtrNew(tempSize);
  ASSERT(exgSock.description);
  StrNCopy(exgSock.description, tempString, tempSize);

  /* Open the record */
  t = DmQueryRecord(d.dbR, p.dbI);
  ptr = MemHandleLock(t);

  /* Set the length */
  recSize = sizeof(DiddleBugRecordType);
  recSize += ((DiddleBugRecordPtr)ptr)->sketchLength;
  recSize += ((DiddleBugRecordPtr)ptr)->extraLength;
  tmp = MemHandleLock(d.record_name);
  recSize += StrLen(tmp) + 1; /* Null-terminated string */
  MemHandleUnlock(d.record_name);
  tmp = MemHandleLock(d.record_note);
  recSize += StrLen(tmp) + 1; /* Null-terminated string */
  MemHandleUnlock(d.record_note);
  exgSock.length = recSize;

  /* Do the beaming */
  err = ExgPut(&exgSock);
  if (!err) {
    pos = 0;
    while (1) {
      bytes_to_send = Min(recSize - pos, BEAM_BUF_SIZE);
      if (!bytes_to_send) break;
      MemMove(buf, ptr+pos, bytes_to_send);
      bytes_sent = ExgSend(&exgSock, buf, bytes_to_send, &err);
      if (err) break;
      pos += bytes_sent;
    }
    ExgDisconnect(&exgSock, err);
  }

  /* Cleanup */
  MemHandleUnlock(t);
  MemPtrFree(exgSock.name);
  MemPtrFree(exgSock.description);
}

/*
** Check whether DiddleBug is running on a Sony Clie
*/
Boolean CheckForSony(Boolean* hires) {
  SonySysFtrSysInfoP sonySysFtrSysInfoP;

  if (!FtrGet(sonySysFtrCreator, sonySysFtrNumSysInfoP, (UInt32*)&sonySysFtrSysInfoP)) {
    /* Running on a Sony... */ 
    if (sonySysFtrSysInfoP->libr & sonySysFtrSysInfoLibrHR) {
      /*... and hi-res library available */
      *hires = true;
    }

    return true;
  }
  
  *hires = false;

  return false;
}

/*
** Set up hi-res mode for Sony Clies
**
** Also stores the refNum of the HR library.
*/
extern void SetUpSony(UInt16* refNum, Boolean* loadedHRLib) {
  Err error = 0;

  if ((error = SysLibFind(sonySysLibNameHR, refNum))) {
    if (error == sysErrLibNotFound) {
      /* couldn't find lib */
      error = SysLibLoad('libr', sonySysFileCHRLib, refNum);
      *loadedHRLib = true;
    }
  } if (!error) {
    /* Now we can use HR lib */

    UInt32 width = hrWidth;
    UInt32 height = hrHeight;

    /* Set compatibility mode */
    HROpen(*refNum);
    HRWinScreenMode(*refNum, winScreenModeSet, &width, &height, NULL, NULL);
  }
}

/*
** Is this a hi-res double-density device?
*/
Boolean CheckForDoubleDensity(void) {
  UInt32 version, attr;
  Err err = FtrGet(sysFtrCreator, sysFtrNumWinVersion, &version);

  if (err == errNone && version >= 4) {
    WinScreenGetAttribute(winScreenDensity, &attr);

    if (attr == kDensityDouble)
      return true;
  }

  return false;
}

/*
** Check if we are running on an Acer s50 or s60
*/
static Boolean CheckForAcerS60(void) {
  Err err;
  UInt32 deviceID, manufacturerID;

  err = FtrGet(sysFtrCreator, sysFtrNumOEMCompanyID, &manufacturerID);
  if (err == errNone && manufacturerID == acerHwrOEMCompanyID_Sony) {
    err = FtrGet(sysFtrCreator, sysFtrNumOEMDeviceID, &deviceID);
    if (err == errNone && deviceID == sonyHwrOEMDeviceIDColor)
      return true;
  }

  return false;
}

/*
** ChangeGeneralColor:
** Change the screen depth. Display some UI controls in grey and
** change the color of several item in the UI interface.
**
** Originally taken from CryptoPad 4.06 (c) 2001 Maxime Labelle & Jean-Baptiste Labelle
*/
static void ChangeGeneralColor(void) {
  UInt32 depth;

  /* Retrieve and store the original screen settings */
  WinScreenMode(winScreenModeGet, &d.oldScreen.width, &d.oldScreen.height,
        &d.oldScreen.depth, &d.oldScreen.color);

  /* Retrieve the supported screen depth. */
  WinScreenMode(winScreenModeGetSupportedDepths, NULL, NULL, &depth, NULL);

  if (depth == 0x808B)
    depth = 16;
  else if (depth == 0x8B)
    depth = 8;
  else if (depth == 0x0B)
    depth = 4;
  else if (depth == 0x03)
    depth = 2;
  else
    depth = 1;

  if (depth <= 4) {
    /* Set the screen depth. */
    WinScreenMode(winScreenModeSet, NULL, NULL, &depth, NULL);

    UIColorSetTableEntry (UIMenuSelectedFill, &middleGrey);

    UIColorSetTableEntry (UIObjectSelectedFill, &middleGrey);

    UIColorSetTableEntry (UIFieldTextLines, &middleGrey);
    UIColorSetTableEntry (UIFieldCaret, &darkGrey);
  }
}

/* 
** Fix ink and paper colors when running on black & white devices.
*/
static void FixInkAndPaperColors(void) {
  UInt32 depth = 0;

  /* Retrieve the current screen settings */
  WinScreenMode(winScreenModeGet, NULL, NULL, &depth, NULL);

  switch (depth) {
  case 1:
    ep.paperColor = 0; /* White */
    ep.inkColor = 1; /* black */
    break;

  case 2:
  case 4:
    if (ep.inkColor == 1)
      ep.inkColor = WinRGBToIndex(&black); /* black */
    break;

  default:
    /* do nothing */
  }
}

/*
** Check whether we're running on a Treo 600 smartphone.
*/
Boolean CheckForTreo600(void) {
  UInt32 company = 0;
  UInt32 device = 0;
  Boolean res = false;

  FtrGet(sysFtrCreator, sysFtrNumOEMCompanyID, &company);
  if (company == hwrOEMCompanyIDHandspring || company == 'Palm' || company == 'palm') {
    FtrGet(sysFtrCreator, sysFtrNumOEMDeviceID, &device);
    if (device == kPalmOneDeviceIDTreo600 || device == kPalmOneDeviceIDTreo600Sim) 
	res = true;
    if (device == kPalmOneDeviceIDTreo650 || device == kPalmOneDeviceIDTreo650Sim ) 
    {
        d.treo650 = true;
	res = true;
    }
    if (device == 'D053' )/* 'D053' is the ID for treo680*/
    {
        d.treo680 = true;
	res = true;
    }
  }

  return res;
}

/*
** Check whether we're running on a palmOne device.
*/
static Boolean CheckForPalmOne(void) {
  UInt32 company = 0;
  UInt32 device = 0;
  Boolean res = false;

  FtrGet(sysFtrCreator, sysFtrNumOEMCompanyID, &company);

  switch (company) {
  case kPalmCompanyIDPalm:
  case kPalmCompanyIDOldPalm:
    FtrGet(sysFtrCreator, sysFtrNumOEMDeviceID, &device);

    switch (device) {
    case kPalmOneDeviceIDZire:
    case kPalmOneDeviceIDTungstenT:
    case kPalmOneDeviceIDTungstenW:
    case kPalmOneDeviceIDTungstenC:
    case kPalmOneDeviceIDZire71:
    case kPalmOneDeviceIDTungstenT2:
    case kPalmOneDeviceIDTungstenT3:
    case kPalmOneDeviceIDTungstenTE:
    case kPalmOneDeviceIDZire21:
      res = true;
      break;
      
    default:
      /* do nothing */
    }
    break;

  default:
    /* do nothing */
  }

  return res;
}

/*
** Find out everything we need to know about the hardware and the OS.
*/
static void DoDeviceSetup(void) {
  /* Check for various OS versions */
  d.os4 = CheckROMVersion(sysVersion40);
  d.os5 = CheckROMVersion(sysVersion50);

  /* Check for OS5 double density screen or Sony Clie hi-res */
  if (d.os5) {
    d.hires = CheckForDoubleDensity();
      
    /* The Treo 600 smartphone needs special handling */
    d.treo600 = CheckForTreo600();
  } else if ((d.sonyClie = CheckForSony(&d.hires)))
    SetUpSony(&d.sonyHRRefNum, &d.sonyLoadedHRLib);
  else if (d.os4) { /* Palm Tungsten W is an OS4 device with double density */
    d.hires = CheckForDoubleDensity();
      
    /* Acer s50/s60 are as well and need special handling */
    d.acer = CheckForAcerS60();
  }

  /* Check for palmOne devices (exluding the Treo 600) */
  d.palmOne = CheckForPalmOne();

  /* Read the preferences before deciding whether to change the color mode */
  GetPreferences(&p, NULL, NULL, d.hires);

  /* Switch to supported gray-scale mode */
  if (!(p.flags & PFLAGS_CHANGE_DEPTH))
    ChangeGeneralColor();
}

/*
** PilotMain - handle the toplevel command and event dispatch.
*/
UInt32 PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags) {
  Err err;
  Boolean appIsActive;
  Char name[dmCategoryLength];

  /* Check for required ROM version (3.5.0) */
  err = RomVersionCompatible (sysVersion35, launchFlags);
  if (err) return (err);

  switch (cmd) {
  case sysAppLaunchCmdNormalLaunch:
    DoDeviceSetup();
    
    /* Open databases */
    if((err = OpenDatabases())) return err;

    /* Fix ink and paper colors if necessary */
    FixInkAndPaperColors();

    /* The file may have been synchronized since the last time we used it, */
    /* check that the currrent category still exist.                       */
    CategoryGetName (d.dbR, p.category, name);
    if (*name == NULL)
      p.category = dmAllCategories;

    /* If necessary, open a new record for immediate scribbling */
    if (p.flags & PFLAGS_OPEN_NEW_RECORD) {
      ErrTry {
    NewScratchImage();
      }
      ErrCatch(inErr) {
    ErrAlert(inErr);
    goto cleanup;
      } ErrEndCatch;

      FrmGotoForm(p.flags & PFLAGS_WITH_TITLEBAR ? DiddleTForm : DiddleForm);
    } else {
      Boolean found = false;
      UInt16 attr;
      UInt32 uniqueID;

      /* Check that the current record still exists.           */
      /* If secret records are being hidden, also check if the */
      /* the current record is marked secret.                  */
      if (DmQueryRecord(d.dbR, p.dbI) != NULL) {
    DmRecordInfo (d.dbR, p.dbI, &attr, &uniqueID, NULL);
    found = /* (uniqueID == currentRecordID) && */
      ((d.privateRecordStatus != hidePrivateRecords) || (!(attr & dmRecAttrSecret)));
      }

      if (!found && DmNumRecords(d.dbR) != 0) {
    SetTopVisibleRecord(0);
    p.dbI = noRecordSelected;
    FrmGotoForm(ep.listFormID);
      } else {
    FrmGotoForm(p.formID);
      }
    }

    /* Continue with normal startup */
    EventLoop();
    
  cleanup:
    /* Clean up */
    StopApplication();

    /* Get back to the previous screen state */
    if (d.oldScreen.width && d.oldScreen.height)
      WinScreenMode(winScreenModeSet, &d.oldScreen.width, &d.oldScreen.height,
            &d.oldScreen.depth, &d.oldScreen.color);
    break;

  case sysAppLaunchCmdAlarmTriggered:
    appIsActive = launchFlags & sysAppLaunchFlagSubCall;
    AlarmTriggered((SysAlarmTriggeredParamType*) cmdPBP, appIsActive);
    break;

  case sysAppLaunchCmdDisplayAlarm:
    appIsActive = launchFlags & sysAppLaunchFlagSubCall;
    DisplayAlarm(appIsActive);
    break;
      
  case sysAppLaunchCmdSystemReset:
    if (!((SysAppLaunchCmdSystemResetType*)cmdPBP)->hardReset) {
      AlarmInit();
      AlarmReset(false);
    }

    /* Check for extended Exchange Manager (OS 4.0.0) */
    {
      UInt32 romVersion;

      FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);
      if (romVersion < sysVersion40) {
    ExgRegisterData(AppType, exgRegTypeID, "image/png");
    ExgRegisterData(AppType, exgRegExtensionID, "PNG");
      } else {
    ExgRegisterDatatype(AppType, exgRegTypeID, "image/png", "image", 0);
    ExgRegisterDatatype(AppType, exgRegExtensionID, " PNG", "image", 0);
      }
    }
    break;

  case sysAppLaunchCmdTimeChange:
  case sysAppLaunchCmdSyncNotify:
    AlarmInit();
    AlarmReset(false);

    /* Check for extended Exchange Manager (OS 4.0.0) */
    {
      UInt32 romVersion;

      FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);
      if (romVersion < sysVersion40) {
    ExgRegisterData(AppType, exgRegTypeID, "image/png");
    ExgRegisterData(AppType, exgRegExtensionID, "PNG");
      } else {
    ExgRegisterDatatype(AppType, exgRegTypeID, "image/png", "image", 0);
    ExgRegisterDatatype(AppType, exgRegExtensionID, " PNG", "image", 0);
      }
    }
    break;

    /*   case sysAppLaunchCmdInitDatabase: */
    /*     break; */

    /*   case sysAppLaunchCmdSaveData: */
    /* Save all data before find */
    /*     break; */

  case sysAppLaunchCmdFind:
    GlobalFind((FindParamsType*)cmdPBP);
    break;

  case sysAppLaunchCmdGoTo:
    {
      GoToParamsPtr par = (GoToParamsPtr) cmdPBP;

      if (launchFlags&sysAppLaunchFlagNewGlobals) {
    DoDeviceSetup();

    if ((err = OpenDatabases())) return err;
    p.dbI = par->recordNum;
    if ((p.dbI < 0) || (p.dbI >= DmNumRecords(d.dbR))) {
      StopApplication();
      return 0;
    }

    /* Fix ink and paper colors if necessary */
    FixInkAndPaperColors();

    /* Change category to "All" if necessary */
    SwitchCategoryForGoto();

    FrmGotoForm(p.flags & PFLAGS_WITH_TITLEBAR ? DiddleTForm : DiddleForm);
    {
      /* Open note dialog if necessary */
      EventType e;

      e.eType = dbOpenRecordFieldEvent;
      e.data.frmGoto.matchFieldNum = par->matchFieldNum;
      e.data.frmGoto.matchPos = par->matchPos;
      e.data.frmGoto.matchLen = par->matchCustom;

      EvtAddEventToQueue(&e);
    }
    
    EventLoop();

    /* Clean up */
    StopApplication();

    /* Get back to the previous screen state */
    if (d.oldScreen.width && d.oldScreen.height)
      WinScreenMode(winScreenModeSet, &d.oldScreen.width, &d.oldScreen.height,
            &d.oldScreen.depth, &d.oldScreen.color);
      }
      else {
    SendGotoEvent(par->recordNum, par->matchFieldNum, par->matchPos, par->matchCustom);
      }
    }
    break;

  case sysAppLaunchCmdExgAskUser:
    {
      ExgAskParamType* par = (ExgAskParamType*) cmdPBP;
      ExgDialogInfoType info;
      Err err;

      info.version = 0;
      info.db = DmOpenDatabaseByTypeCreator(DBType, AppType, dmModeReadOnly);
      info.categoryIndex = dmUnfiledCategory;

      if (ExgDoDialog(par->socketP, &info, &err))
    par->result = exgAskOk;
      else
    par->result = exgAskCancel;

      /* Save chosen category */
      par->socketP->appData = info.categoryIndex;
    }
    break;

  case sysAppLaunchCmdExgReceiveData:
    appIsActive = launchFlags & sysAppLaunchFlagSubCall;

    ErrTry {
      ReceiveData((ExgSocketPtr) cmdPBP, appIsActive);  
    }
    ErrCatch(inErr) {
      ErrAlert(inErr); /* Show dialog */
      PRINT("Error %hx while receiving data.", inErr);
    } ErrEndCatch;
    break;
  }

  return 0;
}
