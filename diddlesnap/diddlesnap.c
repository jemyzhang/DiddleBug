/*
** DiddleSnap - standalone snapshot app for PalmOS 5
** Copyright (c) 2003, 2004 Peter Putzer <pputzer@users.sourceforge.net>
** Licensed under the GNU GPL, version 2 or later
** See file "COPYING" that you should have received with this program
** or visit http://www.gnu.org/copyleft/gpl.html
*/


#include <PalmOSGlue.h>
#include <PalmOS.h>

/* Sony Clie */
#include <SonyHRLib.h>
#include <SonySystemFtr.h>

#include "diddlesnap.h"
#include "diddlesnapRsc.h"

/* forward references */
static Boolean DoScreenCap(UInt16 prefFlags, Boolean clip);
static BitmapType* Convert(WinHandle src, Boolean hires, Boolean Clip);
static void MakeDateTimeStamp(Char* stamp);
static MemPtr CompressSketch(DmOpenRef dbR, UInt16 dbI, Char* rec_name, Char* rec_note, 
			     Int32 maxRecordSize, Int16 maxPixels,
			     Int16 thumbnailAdjustment, UInt16* size, UInt8* uPtr);

static void PrefsInit(FormPtr frm);
static void PrefsOK();
static MemPtr GetObjectPtr(UInt16 id);
static void SetList(UInt16 triggerid, UInt16 listid, UInt16 key);
static Err RomVersionCompatible(UInt32 requiredVersion, UInt16 launchFlags);
static Boolean CheckForDoubleDensity(void);
static void ProcessSoftKeyStroke(PointType* start, PointType* end);
static Boolean ProcessVirtualChar(WChar c);

// This is the code that will be called whenever ANY app calls EvtGetEvent,
// and myhack is enabled in HackMaster. HackMaster is not smart about the
// content of this file; it will simply jump to the first executable statement
// in this file. If you need helper functions, prototype them above, and
// define them AFTER the trap patch.
static void ProcessSoftKeyStroke(PointType* start, PointType* end) {
  const PenBtnInfoType* btninfo;
  UInt16 numbuttons = 0, i;
  UInt16 startbutton = 0, endbutton = 0;

  btninfo=EvtGetPenBtnList(&numbuttons);
  for (i=0; i<numbuttons; i++) {
    if (RctPtInRectangle(start->x, start->y, &btninfo[i].boundsR))
      startbutton=btninfo[i].asciiCode;
    if (RctPtInRectangle(end->x, end->y, &btninfo[i].boundsR))
      endbutton=btninfo[i].asciiCode;
  }

  if (startbutton!=endbutton) {
    MyPrefsType prefs;
    UInt16 prefssize=sizeof(prefs);

    if ((PrefGetAppPreferences('DBSC', 0, &prefs, &prefssize, true)
	 ==noPreferenceFound)||(prefssize<sizeof(prefs))) {
      prefs.strokestart=calcChr;
      prefs.strokeend=findChr;
      prefs.clipstart=findChr;
      prefs.clipend=calcChr;
      prefs.flags = 0x0000;
      prefs.flags |= DBSC_FLAGS_JUMP;
    }
    if ((startbutton==prefs.strokestart)&&(endbutton==prefs.strokeend)) {
      if (!DoScreenCap(prefs.flags, false)) SndPlaySystemSound(sndError);
    }
    if ((startbutton==prefs.clipstart)&&(endbutton==prefs.clipend)) {
      if (!DoScreenCap(prefs.flags, true)) SndPlaySystemSound(sndError);
    }
  }
}

static Boolean ProcessVirtualChar(WChar c) {
  Boolean res = false;

  if (c == vchrRonamatic) {
    MyPrefsType prefs;
    UInt16 prefssize = sizeof(prefs);

    if ((PrefGetAppPreferences('DBSC', 0, &prefs, &prefssize, true)
	 ==noPreferenceFound)||(prefssize<sizeof(prefs))) {
      prefs.strokestart=calcChr;
      prefs.strokeend=findChr;
      prefs.clipstart=findChr;
      prefs.clipend=calcChr;
      prefs.flags = 0x0000;
      prefs.flags |= DBSC_FLAGS_JUMP;
    }

    if (!DoScreenCap(prefs.flags, true))
      SndPlaySystemSound(sndError);

    res = true;
  }

  return res;
}

static Boolean DoScreenCap(UInt16 prefFlags, Boolean clip) {
  struct StackFrame {
    Err err;
    DmSearchStateType searchstate;
    UInt16 dBugCardNo;
    LocalID dBugdbID;
    UInt32 width;
    UInt32 height;
    UInt32 depth;
    Boolean color;
    DmOpenRef dbR;
    MemHandle t;
    MemPtr ptr;
    DiddleBugRecordType record;
    Char name[32];
    Boolean hires;
    Int32 maxPixels;
    Int32 maxBytes;
    Int32 maxRecordSize;
    BitmapType* bmp;
  } *s;
  UInt16 cardno;
  LocalID dbid;
  GoToParamsPtr gotoP;
  UInt16 index;

  /* Reserve & initialize virtual stack frame */
  s = MemPtrNew(sizeof(struct StackFrame));
  MemSet(s, sizeof(struct StackFrame), 0);

  /* Set up our "constants" */
  if ((s->hires = CheckForDoubleDensity())) {
    s->maxPixels = MAX_PIXELS_HIRES;
    s->maxRecordSize = 3331 * 4;
  } else {
    s->maxPixels = MAX_PIXELS;
    s->maxRecordSize = 3331;
  }
  s->maxBytes = s->maxPixels * s->maxPixels / 8;

  /* Check to see if we support more than 1-bit depth */
  WinScreenMode(winScreenModeGet, &s->width, &s->height, &s->depth, &s->color);

  /* Check if DiddleBug is running */
  SysCurAppDatabase(&s->dBugCardNo, &s->dBugdbID);
  s->err = DmGetNextDatabaseByTypeCreator(true, &s->searchstate, 'appl', 'DIDB',
					  true, &cardno, &dbid);
  if (s->err) return false;
  if ((s->dBugCardNo == cardno) && (s->dBugdbID == dbid)) 
    return false;

  /* Play the starting sound */
  SndPlaySystemSound(sndClick);

  /* Open the DiddleBug database */
  s->dbR = DmOpenDatabaseByTypeCreator('Data', 'DIDB', dmModeReadWrite);
  if (s->dbR == NULL) return false;

  /* Create date/time stamp */
  MakeDateTimeStamp(s->name);

  /* Create a new record at highest index */
  index = dmMaxRecordIndex;
  s->t = DmNewRecord(s->dbR, &index, 
		     s->maxRecordSize + baseRecordSize + StrLen(s->name));
  if (!s->t) { DmCloseDatabase(s->dbR); return false; }
  DmReleaseRecord(s->dbR, index, true);

  /* Switch to hi-res */
  WinPushDrawState();
  WinSetCoordinateSystem(kCoordinatesDouble);

  /* Convert to 320 x 320 x 1 bpp */
  s->bmp = Convert(WinGetDisplayWindow(), s->hires, clip);

  /* Write sketch */
  s->ptr = CompressSketch(s->dbR, index, s->name, "", s->maxRecordSize, 
			  s->maxPixels, s->hires ? 2 : 1, &s->record.sketchLength,
			  BmpGetBits(s->bmp));

  /* Switch back to lo-res */
  WinPopDrawState();

  DmWrite(s->ptr, 0, &s->record, sizeof(DiddleBugRecordType));
  
  /* Clean up */
  BmpDelete(s->bmp);
  MemPtrUnlock(s->ptr);
  DmReleaseRecord(s->dbR, index, true);
  DmCloseDatabase(s->dbR);

  /* Clean up virtual stack frame */
  MemPtrFree(s);

  /* Launch DiddleBug at new sketch */
  if (prefFlags & DBSC_FLAGS_JUMP) {
    gotoP = MemPtrNew(sizeof(GoToParamsType));
    MemSet(gotoP, sizeof(GoToParamsType), 0);
    gotoP->recordNum = index;
    MemPtrSetOwner(gotoP, 0);
    SysUIAppSwitch(cardno, dbid, sysAppLaunchCmdGoTo, (MemPtr)gotoP);
  }

  return true;
}

/*
** Converts src to a b/w bitmap of the same size
**
** Returns NULL if an error occurs.
*/
static BitmapType* Convert(WinHandle srcH, Boolean hires, Boolean clip) {
  Coord width, height, x, y;
  BitmapType* src = WinGetBitmap(srcH);
  BitmapType* dest;
  BitmapTypeV3* destV3;
  WinHandle oldH, destH;
  Err err;
  IndexedColorType c;
  RGBColorType rgb;
  UInt8 depth;
  RectangleType rect;
  UInt8* srcPtr, *destPtr;
  UInt8 bufByte;
  UInt16 i, bytes;

  BmpGlueGetDimensions(src, &width, &height, NULL);
  
  /* Only copy the left-/topmost 320x320 pixels */
  if (width != height) {
    width = width > height ? height : width;
    height = height > width ? width: height;
  }

  dest = BmpCreate(width, height, 1, NULL, &err);
  if (err != errNone) return NULL;
  destV3 = BmpCreateBitmapV3(dest, kDensityDouble, BmpGetBits(dest), NULL);

  destH = WinCreateBitmapWindow((BitmapType*)destV3, &err);
  if (err != errNone) {
    BmpDelete(dest);
    BmpDelete((BitmapType*)destV3);
    return NULL;
  }

  /* Save old draw window state */
  oldH = WinSetDrawWindow(srcH);
  WinPushDrawState();

  /* Check for optimized cases */
  depth = BmpGlueGetBitDepth(src);
  switch (depth) {
  case 1:
    WinGetDrawWindowBounds(&rect); /* equiv. to WinGetBounds(srcH, &rect) */
    WinCopyRectangle(srcH, destH, &rect, 0, 0, winPaint);
    break;
    
/*   case 2: */
/*   case 4: */
/*     destPtr = BmpGetBits(dest); */
/*     srcPtr = BmpGetBits(src); */
/*     bytes = BmpBitsSize(dest); */

/*     if (depth == 2) { */
/*       for (i = 0; i < bytes; ++i) { */
/* 	bufByte = 0x00; */

/* 	if (srcPtr[0]&0x80) bufByte |= 0x80; */
/* 	if (srcPtr[0]&0x20) bufByte |= 0x40; */
/* 	if (srcPtr[0]&0x08) bufByte |= 0x20; */
/* 	if (srcPtr[0]&0x02) bufByte |= 0x10; */
/* 	srcPtr++; */
/* 	if (srcPtr[0]&0x80) bufByte |= 0x08; */
/* 	if (srcPtr[0]&0x20) bufByte |= 0x04; */
/* 	if (srcPtr[0]&0x08) bufByte |= 0x02; */
/* 	if (srcPtr[0]&0x02) bufByte |= 0x01; */

/* 	destPtr[i] = bufByte; */
/* 	srcPtr++; */
/*       } */
/*     } else { */
/*       for (i = 0; i < bytes; ++i) { */
/* 	bufByte = 0x00; */

/* 	if ((srcPtr[0]&0x80)||(srcPtr[0]&0x40)) bufByte |= 0x80; */
/* 	if ((srcPtr[0]&0x08)||(srcPtr[0]&0x04)) bufByte |= 0x40; */
/* 	srcPtr++; */
/* 	if ((srcPtr[0]&0x80)||(srcPtr[0]&0x40)) bufByte |= 0x20; */
/* 	if ((srcPtr[0]&0x08)||(srcPtr[0]&0x04)) bufByte |= 0x10; */
/* 	srcPtr++; */
/* 	if ((srcPtr[0]&0x80)||(srcPtr[0]&0x40)) bufByte |= 0x08; */
/* 	if ((srcPtr[0]&0x08)||(srcPtr[0]&0x04)) bufByte |= 0x04; */
/* 	srcPtr++; */
/* 	if ((srcPtr[0]&0x80)||(srcPtr[0]&0x40)) bufByte |= 0x02; */
/* 	if ((srcPtr[0]&0x08)||(srcPtr[0]&0x04)) bufByte |= 0x01; */

/* 	destPtr[i] = bufByte; */
/* 	srcPtr++; */
/*       } */
/*     } */
/*     break; */

  default:
    for (x = 0; x < width; ++x) {
      for (y = 0; y < height; ++y) {
	WinSetDrawWindow(srcH);
	c = WinGetPixel(x, y);
	WinIndexToRGB(c, &rgb);
	if (c && (30 * rgb.r + 59 * rgb.g + 11 * rgb.b) / 300 < 55) {
	  WinSetDrawWindow(destH);
	  WinPaintPixel(x, y);
	}
      }
    }
  }
  
  /* If necessary, clip the titlebar */
  if (clip) {
    const RectangleType r = { {0, 32}, {320, 288} };
    const RectangleType bottom_r = { {0, 288}, {320, 32} };

    WinCopyRectangle(destH, destH, &r, 0, 0, winPaint);
    WinSetDrawWindow(destH);
    WinFillRectangle(&bottom_r, 0);
  }

  /* Clean up */
  WinPopDrawState();
  WinSetDrawWindow(oldH);
  WinDeleteWindow(destH, false);
  BmpDelete(dest);

  return (BitmapType*) destV3;
}

/*
** MakeDateTimeStamp - make sure that "stamp" is big enough for the resulting string
*/
static void MakeDateTimeStamp(Char* stamp) {
  DateTimeType dateTime;
  Char tempStr[timeStringLength];

  TimSecondsToDateTime(TimGetSeconds(), &dateTime);
  DateToAscii(dateTime.month, dateTime.day, dateTime.year,
	      PrefGetPreference(prefLongDateFormat), stamp);
  StrNCat(stamp, ", ", 31);
  TimeToAscii(dateTime.hour, dateTime.minute,
	      PrefGetPreference(prefTimeFormat), tempStr);
  StrNCat(stamp, tempStr, 31);
}

/*
** CompressSketch - assumes uPtr is either 160x160 or 320x320 bits
**                  (depending on the "hires" parameter)
**
**                  returns a locked pointer to the current record
*/
MemPtr CompressSketch(DmOpenRef dbR, UInt16 dbI, Char* rec_name, Char* rec_note, 
		      Int32 maxRecordSize, Int16 maxPixels, Int16 thumbnailAdjustment,
		      UInt16* size, UInt8* uPtr) {
  UInt8* dataPtr, *headPtr, *buffer, *cuPtr;
  MemHandle t;
  MemPtr ptr;
  UInt16 i, j, k;
  UInt8 mask, thumbByte;
  Int16 name_len, note_len;
  const Int16 bytesPerRow = maxPixels / 8;
  
  /* Now the real stuff */
  buffer = MemPtrNew(maxRecordSize); /* Big enough to hold any sketch */
  if (!buffer) abort();

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
	  thumbByte = (i*4/thumbnailAdjustment) + (k/8/thumbnailAdjustment); //FIXME
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
  ptr = MemHandleLock(t);

  if (!rec_name || !rec_note) abort();

  /* Save the high data */
  name_len = StrLen(rec_name) + 1;
  note_len = StrLen(rec_note) + 1;

  MemHandleUnlock(t);

  /* Resize the record */
  t = DmResizeRecord(dbR, dbI,
		     sketchDataOffset + (*size) + name_len + note_len);
  ptr = MemHandleLock(t);

  /* Write the sketch data to disk */
  DmWrite(ptr, sketchDataOffset, buffer, (*size));
  /* Re-Write the high data to disk */
  DmStrCopy(ptr, sketchDataOffset + (*size), rec_name);
  DmStrCopy(ptr, sketchDataOffset + (*size) + name_len, rec_note);

  /* Clean up */
  MemPtrFree(buffer);
  return ptr;
}

// This is the code that will be called whenever the user taps myhack's "+"
// button in the HackMaster list. HackMaster is not smart about the content of
// this file; it will simply jump to the first executable statement in this
// file. If you need helper functions, prototype them above, and define them
// AFTER the trap patch.
Boolean PrefsHandler(EventPtr event)
{
   FormPtr         frm;
   Boolean handled=false;

   switch (event->eType) {
   case frmOpenEvent:
      frm = FrmGetActiveForm();
      PrefsInit(frm);
      FrmDrawForm(frm);
      handled=1;
      break;

   case ctlSelectEvent:
/*       switch (event->data.ctlEnter.controlID) { */
/*       case PrefsOKButton: */
/* 	 PrefsOK(); */
/* 	 // FALL THRU */

/*       case PrefsCancelButton: */
/* 	 FrmGotoForm(9000);  // return to HackMaster control panel */
/* 	 handled=true; */
/* 	 break; */
/*       } */
      break;

   case frmCloseEvent:
     PrefsOK();
     break;

   default:
      break;
   }

   return handled;
}

static void PrefsInit(FormPtr frm)
{
   MyPrefsType prefs;
   UInt16 prefssize=sizeof(prefs);

   MemSet(&prefs, sizeof(MyPrefsType), 0);
   if ((PrefGetAppPreferences('DBSC', 0, &prefs, &prefssize, true)
	==noPreferenceFound)||(prefssize<sizeof(prefs))) {
      prefs.strokestart=calcChr;
      prefs.strokeend=findChr;
      prefs.clipstart=findChr;
      prefs.clipend=calcChr;
      prefs.flags |= DBSC_FLAGS_JUMP;
   }

   SetList(StrokeFromTrigger, StrokeFromList, prefs.strokestart);
   SetList(StrokeToTrigger, StrokeToList, prefs.strokeend);
   SetList(ClipFromTrigger, ClipFromList, prefs.clipstart);
   SetList(ClipToTrigger, ClipToList, prefs.clipend);
   CtlSetValue(GetObjectPtr(JumpCheckBox), prefs.flags&DBSC_FLAGS_JUMP);
}

static void SetList(UInt16 triggerid, UInt16 listid, UInt16 key)
{
   UInt16 selection;
   Char* text;

   switch (key) {
   case launchChr:
      selection=0;
      text="Applications";
      break;

   case menuChr:
      selection=1;
      text="Menu";
      break;

   case calcChr:
      selection=2;
      text="Calculator";
      break;

   case findChr:
      selection=3;
      text="Find";
      break;

   default:
      selection=4;
      text="Disabled";
      break;
   };
   LstSetSelection(GetObjectPtr(listid), selection);
   CtlSetLabel(GetObjectPtr(triggerid), text);
}

static void PrefsOK()
{
   UInt16 key[5];
   MyPrefsType prefs;
   UInt16 prefssize=sizeof(prefs);

   key[0]=launchChr;
   key[1]=menuChr;
   key[2]=calcChr;
   key[3]=findChr;
   key[4]=0;

   prefs.strokestart=key[LstGetSelection(GetObjectPtr(StrokeFromList))];
   prefs.strokeend=key[LstGetSelection(GetObjectPtr(StrokeToList))];
   prefs.clipstart=key[LstGetSelection(GetObjectPtr(ClipFromList))];
   prefs.clipend=key[LstGetSelection(GetObjectPtr(ClipToList))];
   if (CtlGetValue(GetObjectPtr(JumpCheckBox)))
       prefs.flags |= DBSC_FLAGS_JUMP;
   else
       prefs.flags &= ~DBSC_FLAGS_JUMP;

   PrefSetAppPreferences('DBSC', 0, 0, &prefs, prefssize, true);
}

static MemPtr GetObjectPtr(UInt16 id)
{
   FormPtr frm;
   frm=FrmGetActiveForm();
   return FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, id));
}

/*
** EventLoop - handle events
*/
static void EventLoop(void) {
  EventType e;
  Err error;
  FormPtr frm;

  do {
    EvtGetEvent(&e, evtWaitForever);

    if (SysHandleEvent(&e)) continue;

    if (MenuHandleEvent(NULL, &e, &error)) continue;

    if (e.eType == frmLoadEvent) {
      frm = FrmInitForm(e.data.frmLoad.formID);
      FrmSetActiveForm(frm);
      switch (e.data.frmLoad.formID) {
      case PrefsForm:
	FrmSetEventHandler(frm, PrefsHandler);
	break;
      }
      continue;
    }

    FrmDispatchEvent(&e);

  } while (e.eType != appStopEvent);
}

/*
** StopApplication
*/
static void StopApplication(void) {  
  UInt16 cardno;
  LocalID dbid;
  DmSearchStateType searchstate;

  FrmCloseAllForms();

  DmGetNextDatabaseByTypeCreator(true, &searchstate, 'appl', 'DBSC', true,
				 &cardno, &dbid);
  SysNotifyRegister(cardno, dbid, sysNotifyProcessPenStrokeEvent, NULL,
		    sysNotifyNormalPriority, NULL);
  SysNotifyRegister(cardno, dbid, sysNotifyVirtualCharHandlingEvent, NULL,
		    sysNotifyNormalPriority, NULL);

  /* Free memory */
}

/*
** Is this a hi-res double-density device?
*/
static Boolean CheckForDoubleDensity(void) {
  UInt32 version, attr;
  Err err = FtrGet(sysFtrCreator, sysFtrNumWinVersion, &version);

  if (err == errNone && version >= 4) {
    WinScreenGetAttribute(winScreenDensity, &attr);

    if (attr == kDensityDouble)
      return true;
  }

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
** PilotMain - handle the toplevel command and event dispatch.
*/
UInt32 PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags) {
  Err err;
/*   Boolean appIsActive; */

  /* Check for required ROM version (5.0.0) */
  err = RomVersionCompatible (sysVersion50, launchFlags);
  if (err) return (err);

  switch (cmd) {
  case sysAppLaunchCmdNormalLaunch:
    FrmGotoForm(PrefsForm);

    /* Continue with normal startup */
    EventLoop();
    StopApplication();

    break;

  case sysAppLaunchCmdSystemReset:
    if (!((SysAppLaunchCmdSystemResetType*)cmdPBP)->hardReset) {
/*       AlarmInit(); */
/*       AlarmReset(false); */
    }
  
    break;

  case sysAppLaunchCmdTimeChange:
  case sysAppLaunchCmdSyncNotify:
/*     AlarmInit(); */
/*     AlarmReset(false); */
    break;

  case sysAppLaunchCmdNotify:
    {
      SysNotifyParamType* notifyPBP = (SysNotifyParamType*) cmdPBP;

      if (notifyPBP->notifyType == sysNotifyProcessPenStrokeEvent) {
	SysNotifyPenStrokeType* penInfo = notifyPBP->notifyDetailsP;

	ProcessSoftKeyStroke(&penInfo->startPt, &penInfo->endPt);
      } else if (notifyPBP->notifyType == sysNotifyVirtualCharHandlingEvent) {
	SysNotifyVirtualCharHandlingType* keyInfo = notifyPBP->notifyDetailsP;

	notifyPBP->handled = ProcessVirtualChar(keyInfo->keyDown.chr);
      }
    }
    break;
  }

  return 0;
}
