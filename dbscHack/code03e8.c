/* code03e8.c - EvtProcessSoftKeyStroke handler

Copyright © 2001, 2002  Peter Putzer
Copyright © 2000 Mitch Blevins
Copyright © 1999 Ben Darnell

This file is a part of DiddleBug ScreenShot Hack,
which is distributed under the GNU GPL.

Based on SampleHack by Roger Chaplin <foursquaredev@att.net>
*/

#include <PalmOS.h>
#include <FeatureMgr.h>
#include <SysEvtMgr.h>
#include <PalmOSGlue.h>

/* Support for special hardware */
#include <SonyHRLib.h>
#include <SonySystemFtr.h>

#include "dbscHack.h"
#include "dbscHackRsc.h"

/* forward references */
static Boolean DoScreenCap(UInt16 prefFlags, Boolean clip);
static BitmapType* Convert(WinHandle src, UInt16 sonyHRRefNum, Boolean Clip,
			   Coord width, Coord height);
static void MakeDateTimeStamp(Char* stamp);
static void SetUpSony(UInt16* refNum, Boolean* loadedHRLib);
static MemPtr CompressSketch(DmOpenRef dbR, UInt16 dbI, Char* rec_name, Char* rec_note, 
			     Int32 maxRecordSize, Int16 maxPixels,
			     Int16 thumbnailAdjustment, UInt16* size, UInt8* uPtr);

/* Numbers of pixels per line */
#define MAX_PIXELS 160
#define MAX_PIXELS_HIRES 320

// This is the code that will be called whenever ANY app calls EvtGetEvent,
// and myhack is enabled in HackMaster. HackMaster is not smart about the
// content of this file; it will simply jump to the first executable statement
// in this file. If you need helper functions, prototype them above, and
// define them AFTER the trap patch.
Err EvtProcessSoftKeyStroke(PointType* start, PointType* end)
{
  UInt32 temp;
  Err (*oldTrap)(PointType*, PointType*);
  const PenBtnInfoType* btninfo;
  UInt16 numbuttons = 0, i;
  UInt16 startbutton = 0, endbutton = 0;

  // get the address of the old EvtGetEvent() from HackMaster
  FtrGet(MYCRID, 0x3e8, &temp);
  // set the function pointer to the old trap
  oldTrap = (Err (*)(PointType*, PointType*)) temp;

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

  return oldTrap(start, end);
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
    SonySysFtrSysInfoP sonySysFtrSysInfoP;
    UInt16 sonyHRRefNum;
    Boolean sonyLoadedHRLib;
    Int32 maxPixels;
    Int32 maxBytes;
    Int32 maxRecordSize;
  } *s;
  UInt16 cardno;
  LocalID dbid;
  GoToParamsPtr gotoP;
  UInt16 index;

  /* Reserve & initialize virtual stack frame */
  s = MemPtrNew(sizeof(struct StackFrame));
  MemSet(s, sizeof(struct StackFrame), 0);

  /* Handle Sony hi-res devices */
  if (!FtrGet(sonySysFtrCreator, sonySysFtrNumSysInfoP, (UInt32*)&s->sonySysFtrSysInfoP)) {
    /* Running on a Sony... */ 
    if (s->sonySysFtrSysInfoP->libr & sonySysFtrSysInfoLibrHR) {
      /*... and hi-res library available */
      SetUpSony(&s->sonyHRRefNum, &s->sonyLoadedHRLib);
    }
  }

  /* Set up our "constants" */
  if (s->sonyHRRefNum) {
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

  /* Write sketch */
  s->ptr = CompressSketch(s->dbR, index, s->name, "", s->maxRecordSize, 
			  s->maxPixels, s->sonyHRRefNum ? 2 : 1, &s->record.sketchLength,
			  BmpGetBits(Convert(WinGetDisplayWindow(), s->sonyHRRefNum, 
					     clip, s->maxPixels, s->maxPixels)));

  DmWrite(s->ptr, 0, &s->record, sizeof(DiddleBugRecordType));
  
  /* Clean up */
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
static BitmapType* Convert(WinHandle srcH, UInt16 sonyHRRefNum, Boolean clip, 
			   Coord width, Coord height) {
  Coord x, y;
  BitmapType* src = WinGetBitmap(srcH);
  BitmapType* dest;
  WinHandle oldH, destH;
  Err err;
  IndexedColorType c;
  RGBColorType rgb;
  UInt8 depth;
  RectangleType rect;
  UInt8* srcPtr, *destPtr;
  UInt8 bufByte;
  UInt16 i, bytes;

  if (sonyHRRefNum)
    dest = HRBmpCreate(sonyHRRefNum, width, height, 1, NULL, &err);
  else
    dest = BmpCreate(width, height, 1, NULL, &err);
  if (err != errNone) return NULL;

  if (sonyHRRefNum)
    destH = HRWinCreateBitmapWindow(sonyHRRefNum, dest, &err);
  else
    destH = WinCreateBitmapWindow(dest, &err);
  if (err != errNone) {
    BmpDelete(dest);
    return NULL;
  }

  /* Save old draw window state */
  oldH = WinSetDrawWindow(srcH);
  WinPushDrawState();

  /* Check for optimized cases */
  depth = BmpGlueGetBitDepth(src);
  switch (depth) {

  case 1:
    if (sonyHRRefNum) {
      HRWinGetWindowBounds(sonyHRRefNum, &rect);
      HRWinCopyRectangle(sonyHRRefNum, srcH, destH, &rect, 0, 0, winPaint);
    } else {
      WinGetDrawWindowBounds(&rect); /* equiv. to WinGetBounds(srcH, &rect) */
      WinCopyRectangle(srcH, destH, &rect, 0, 0, winPaint);
    }
    break;

  case 2:
  case 4:
    destPtr = BmpGetBits(dest);
    srcPtr = BmpGetBits(src);
    if (sonyHRRefNum)
      bytes = HRBmpBitsSize(sonyHRRefNum, dest);
    else
      bytes = BmpBitsSize(dest);

    if (depth == 2) {
      for (i = 0; i < bytes; ++i) {
	bufByte = 0x00;

	if (srcPtr[0]&0x80) bufByte |= 0x80;
	if (srcPtr[0]&0x20) bufByte |= 0x40;
	if (srcPtr[0]&0x08) bufByte |= 0x20;
	if (srcPtr[0]&0x02) bufByte |= 0x10;
	srcPtr++;
	if (srcPtr[0]&0x80) bufByte |= 0x08;
	if (srcPtr[0]&0x20) bufByte |= 0x04;
	if (srcPtr[0]&0x08) bufByte |= 0x02;
	if (srcPtr[0]&0x02) bufByte |= 0x01;

	destPtr[i] = bufByte;
	srcPtr++;
      }
    } else {
      for (i = 0; i < bytes; ++i) {
	bufByte = 0x00;

	if ((srcPtr[0]&0x80)||(srcPtr[0]&0x40)) bufByte |= 0x80;
	if ((srcPtr[0]&0x08)||(srcPtr[0]&0x04)) bufByte |= 0x40;
	srcPtr++;
	if ((srcPtr[0]&0x80)||(srcPtr[0]&0x40)) bufByte |= 0x20;
	if ((srcPtr[0]&0x08)||(srcPtr[0]&0x04)) bufByte |= 0x10;
	srcPtr++;
	if ((srcPtr[0]&0x80)||(srcPtr[0]&0x40)) bufByte |= 0x08;
	if ((srcPtr[0]&0x08)||(srcPtr[0]&0x04)) bufByte |= 0x04;
	srcPtr++;
	if ((srcPtr[0]&0x80)||(srcPtr[0]&0x40)) bufByte |= 0x02;
	if ((srcPtr[0]&0x08)||(srcPtr[0]&0x04)) bufByte |= 0x01;

	destPtr[i] = bufByte;
	srcPtr++;
      }
    }
    break;

  default:
    if (sonyHRRefNum) {
      for (x = 0; x < width; ++x) {
	for (y = 0; y < height; ++y) {
	  WinSetDrawWindow(srcH);
	  c = HRWinGetPixel(sonyHRRefNum, x, y);
	  WinIndexToRGB(c, &rgb);
	  if (c && (30 * rgb.r + 59 * rgb.g + 11 * rgb.b) / 300 < 55) {
	    WinSetDrawWindow(destH);
	    HRWinPaintPixel(sonyHRRefNum, x, y);
	  }
	}
      }
    } else {
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
  }
  
  /* If necessary, clip the titlebar */
  if (clip) {
    if (sonyHRRefNum) {
      RectangleType r = { {0, 32}, {320, 288} };
      RectangleType bottom_r = { {0, 288}, {320, 32} };
      
      HRWinCopyRectangle(sonyHRRefNum, destH, destH, &r, 0, 0, winPaint);
      WinSetDrawWindow(destH);
      HRWinFillRectangle(sonyHRRefNum, &bottom_r, 0);
    } else {
      const RectangleType r = { {0, 16}, {160, 144} };
      const RectangleType bottom_r = { {0, 144}, {160, 16} };
      
      WinCopyRectangle(destH, destH, &r, 0, 0, winPaint);
      WinSetDrawWindow(destH);
      WinFillRectangle(&bottom_r, 0);
    }
  }

  /* Clean up */
  WinPopDrawState();
  WinSetDrawWindow(oldH);
  WinDeleteWindow(destH, false);

  return dest;
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
** Set up hi-res mode for Sony Clies
**
** Also stores the refNum of the HR library.
*/
static void SetUpSony(UInt16* refNum, Boolean* loadedHRLib) {
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
