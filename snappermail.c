/*

Copyright (c) 2003, 2004 Peter Putzer <pputzer@users.sourceforge.net>.

Licensed under the GNU General Public License (GPL).

*/

#include <PalmOS.h>

#include "snappermail.h"
#include "diddlebug.h"
#include "diddlebugInlines.h"
#include "diddlebugExtern.h"
#include "diddlebugRsc.h"
#include "globals.h"
#include "png.h"
#include "alarm.h"
#include "debug.h"

#define ChunkSize 1024
#define mailExtension ".mail"
#define maximumNoStatusSize 20000

/*
** Switch to another application.
*/
void Switch(UInt32 crId) {
  UInt16 cardNo;
  LocalID dbID;
  DmSearchStateType searchInfo;

  /* get the application's card number and dbID */
  DmGetNextDatabaseByTypeCreator (true, &searchInfo, sysFileTApplication, crId, true, &cardNo, &dbID);

  /* Launch application created by crId */
  SysUIAppSwitch(cardNo, dbID, sysAppLaunchCmdNormalLaunch, NULL);
}

/*
** Strip -fix from string.
*/
static MemHandle StripFixFromString(const Char* str, Char lim, Boolean suffix) {
  MemHandle res = NULL;
  Char* resP = NULL;
  Int16 pos;
  Boolean found = false;
  const Int16 len = StrLen(str);

  if (suffix) {
    /* strip suffix */
    for (pos = len - 1; pos >= 0;  --pos) {
      if (str[pos] == lim) {
	found = true;
	break;
      }
    }

    if (found) {
      res = MemHandleNew(pos + 1);
      ASSERT(res);
      resP = MemHandleLock(res);
      StrNCopy(resP, str, pos);
      resP[pos] = 0;
      MemHandleUnlock(res);
    }
  } else {
    /* strip prefix */
    for (pos = 0; pos < len; ++pos) {
      if (str[pos] == lim) {
	found = true;
	break;
      }
    }

    if (found) {
      res = MemHandleNew(len - pos - 1);
      ASSERT(res);
      resP = MemHandleLock(res);
      StrNCopy(resP, str + pos + 1, len - pos - 1);
      resP[len - pos - 2] = 0;
      MemHandleUnlock(res);
    }
  }

  if (!found) {
    res = MemHandleNew(len + 1);
    ASSERT(res);
    StrCopy(MemHandleLock(res), str);
    MemHandleUnlock(res);
  }

  return res;
}

/*
** Receive PNG file
*/
Err ReceiveData(ExgSocketPtr exgSocketP, Boolean appIsActive) {
  struct StackFrame {
    DmOpenRef dbR;
    UInt16 recIndex;
    MemHandle dataH;
    MemHandle t;
    MemPtr ptr;
    UInt16 size;
    UInt8 *dataP;
    Int16 len;
    UInt16 dataLen;
    UInt8* buffer;
    DiddleBugRecordType rec;
    AlarmSoundInfoType rec_sound;
    MemHandle rec_name;
    MemHandle tmp_name;
    MemHandle rec_note;
    MemHandle beamStringH;
    Boolean hires;
    UInt32 maxPixels;
    UInt32 bufferSize;
  } *s;
  Err err;

  /* Reserve & initialize virtual stack frame */
  s = MemPtrNew(sizeof(struct StackFrame));
  ASSERT(s);
  MemSet(s, sizeof(struct StackFrame), 0);
  s->rec_sound.listIndex = -1;

  /* Check for resolution */
  if (CheckROMVersion(sysVersion50))
    s->hires = CheckForDoubleDensity();
  else 
    CheckForSony(&s->hires);
  
  /* Set up buffer size */
  if (s->hires)
    s->maxPixels = MAX_PIXELS_HIRES;
  else
    s->maxPixels = MAX_PIXELS;
  s->bufferSize = s->maxPixels * s->maxPixels / 8;

  /* Open the database */
  if (appIsActive) s->dbR = d.dbR;
  else {
    s->dbR = DmOpenDatabaseByTypeCreator(DBType, AppType,
					 dmModeReadWrite|dmModeShowSecret);
    if (!s->dbR) {
      /* Clean up virtual stack frame */
      MemPtrFree(s);
      return -1;
    }
  }

  /* Create the new record */
  s->recIndex = dmMaxRecordIndex;
  s->t = DmNewRecord(s->dbR, &s->recIndex, MinRecordSize(s->hires));
  if (!s->t) {
    DmOpenRef dbR = s->dbR;
    UInt16 dbI = s->recIndex;

    MemPtrFree(s);
    DatabaseError(dbR, dbI);
  }
  DmSet(MemHandleLock(s->t), 0, MinRecordSize(s->hires), 0);
  MemHandleUnlock(s->t);

  if (exgSocketP->length)
    s->size = exgSocketP->length;
  else
    s->size = ChunkSize;

  s->dataH = MemHandleNew(s->size);
  ASSERT(s->dataH);
  if (!s->dataH) {
    /* Clean up virtual stack frame */
    MemPtrFree(s);
    return -1;
  }

  /* accept will open a progress dialog and wait for your receive commands */
  err = ExgAccept(exgSocketP);
  if (!err) {
    s->dataP = MemHandleLock(s->dataH);

    do {
      s->len = ExgReceive(exgSocketP,&s->dataP[s->dataLen], s->size - s->dataLen, &err);
      if (s->len && !err) {
	s->dataLen += s->len;
	/* resize block when we reach the limit of this one... */
	if (s->dataLen >= s->size) {
	  MemHandleUnlock(s->dataH);
	  err = MemHandleResize(s->dataH, s->size + ChunkSize);
	  s->dataP = MemHandleLock(s->dataH);
	  if (!err) s->size += ChunkSize;
	}
      }
    } while (s->len && !err);

    /* Guess if we are receiving a beamed sketch in the old format */
    s->beamStringH = DmGetResource(strRsc, BeamContentString);
    if (!StrCompare(exgSocketP->name, MemHandleLock(s->beamStringH))) {
      UInt16 category = exgSocketP->appData;

      /* Store the sketch "verbatim" */
      s->t = DmResizeRecord(s->dbR, s->recIndex, s->dataLen);
      s->ptr = MemHandleLock(s->t);
      DmWrite(s->ptr, 0, s->dataP, s->dataLen);
      MemPtrUnlock(s->ptr);
      DmReleaseRecord(s->dbR, s->recIndex, true);

      /* Store record in the previously selected category */
      if (category != dmUnfiledCategory) {
	UInt16 attr;
	err = DmRecordInfo(s->dbR, s->recIndex, &attr, NULL, NULL);

	/* Set the category to the one the user  */
	/* specified, and mark the record dirty. */
	if ((attr & dmRecAttrCategoryMask) != category) {
	  attr &= ~dmRecAttrCategoryMask;
	  attr |= category | dmRecAttrDirty;
	  err = DmSetRecordInfo(s->dbR, s->recIndex, &attr, NULL);
	}
      }
    } else {
      /* Decode PNG file */
      s->buffer = MemPtrNew(s->bufferSize);
      if (!s->buffer) {
	MemPtrFree(s);
	NoMemoryError();
      }
      err = DecodePNG(s->dataP, s->size, s->buffer, &s->rec_note, s->hires);

      if (!err) {
	/* Store decoded sketch */
	MemSet(&s->rec, sizeof(DiddleBugRecordType), 0);

	/* Decode name */
	s->tmp_name = StripFixFromString(exgSocketP->name, ':', false);
	s->rec_name = StripFixFromString(MemHandleLock(s->tmp_name), '.', true);
	MemHandleUnlock(s->tmp_name);
	MemHandleFree(s->tmp_name);

	/* Handle potentially missing note */
	if (!s->rec_note) {
	  s->rec_note = MemHandleNew(1); /* no note */
	  ASSERT(s->rec_note);
	  MemSet(MemHandleLock(s->rec_note), 1, 0);
	  MemHandleUnlock(s->rec_note);
	}

	/* Release record */
	DmReleaseRecord(s->dbR, s->recIndex, true);

	/* Compress sketch */
	s->ptr = CompressSketch(s->hires, s->dbR, s->recIndex, s->rec_name, 
				s->rec_note, &s->rec_sound, &s->rec.extraLength,
				&s->rec.sketchLength, s->buffer);

	/* Write the header-type data */
	DmWrite(s->ptr, 0, &s->rec, sizeof(DiddleBugRecordType));
	MemPtrUnlock(s->ptr);;
	DmReleaseRecord(s->dbR, s->recIndex, true);
      }
    }

    /* Intermediate clean-up */
    MemHandleUnlock(s->beamStringH);
    DmReleaseResource(s->beamStringH);
    MemHandleUnlock(s->dataH);
    ExgDisconnect(exgSocketP, err); /* closes transfer dialog */

    if (!err) {
      /* Jump to the record */
      UInt16 cardNo;
      LocalID dbID;

      RescheduleAlarms(s->dbR);
      DmOpenDatabaseInfo(s->dbR, &dbID, NULL, NULL, &cardNo, NULL);

      exgSocketP->goToCreator = AppType;
      exgSocketP->goToParams.dbCardNo = cardNo;
      exgSocketP->goToParams.dbID = dbID;
      exgSocketP->goToParams.recordNum = s->recIndex;
    } else {
      DmRemoveRecord(s->dbR, s->recIndex);
    }
  }

  /* Clean up */
  if (s->buffer) MemPtrFree(s->buffer);
  if (s->rec_name) MemHandleFree(s->rec_name);
  if (s->rec_note) MemHandleFree(s->rec_note);
  if (s->dataH) MemHandleFree(s->dataH);

  /* Close the database */
  if (!appIsActive) DmCloseDatabase(s->dbR);

  /* Clean up virtual stack frame */
  MemPtrFree(s);

  return err;
}

/******************************************************************************
 *
 * Workaround for OS3.5 exchange manager bug where the extension is determined
 * by the first dot, not the last.
 *
 * Escape dots in a filename and append ".mail". For example:
 *
 *    e_equals_mc^2_proof.txt -> e_equals_mc^^2_proof^_txt.mail
 *
 *  -> filename - A chrNull terminated string containing a filename.
 *
 * RETURNS: A locked handle containing the escaped filename. The caller is
 *          resposible for freeing this locked handle using MemPtrFree, or
 *          MemPtrRecoverHandle, MemHandleUnlock, and MemHandleFree. May return
 *          NULL if out of memory.
 *
 *****************************************************************************/
Char* EscapeDotsInFilename(const Char* filename) {
  Char *pos;
  UInt16 extraChars = 0;
  MemHandle handle;
  Char *escapedFilename = NULL;
  Char current;

  ErrFatalDisplayIf(NULL == filename, "NULL filename");

  // Count the extra characters required to store the escaped filename.
  // One for every dot and caret, except the last dot.
  pos = (Char *)filename;
  while (chrNull != *pos) {
    if (chrFullStop == *pos || chrCircumflexAccent == *pos) {
      extraChars++;
    }

    pos++;
  }

  // Plus six for .mail and chrNull
  handle = MemHandleNew(StrLen(filename) + extraChars + 6);
  ASSERT(handle);

  if (handle) {
    escapedFilename = MemHandleLock(handle);

    // Copy the filename escaping carets and dots.
    pos = escapedFilename;
    while (chrNull != *filename) {
      current = *filename++;

      if (chrFullStop == current || chrCircumflexAccent == current) {
	*pos++ = chrCircumflexAccent;

	if (chrFullStop == current) {
	  current = chrLowLine;
	}
      }

      *pos++ = current;
    }

    *pos = chrNull;
    StrCat(escapedFilename, mailExtension);
  }

  // The caller must free this locked handle using MemPtrFree, or
  // MemPtrRecoverHandle, MemHandleUnlock, and MemHandleFree.
  return escapedFilename;
}

/*
** Sends the current record encoded as a PNG file via SnapperMail.
*/
void SendCurrentRecord(void) {
  UInt32 bytes_written = 0;
  MemPtr buffer = NULL;
  Char* name = NULL;
  Char* rec_name = NULL;
  UInt16 len = 0;
  Err err = errNone;
  MemHandle png = MemHandleNew(ChunkSize * 2);

  if (!png)
    NoMemoryError();

  ErrTry {
    /* Save the current data */
    FlushToDisk(true);
  }
  ErrCatch(inErr) {
    MemHandleFree(png);
    ErrThrow(inErr);
  } ErrEndCatch;

  err = EncodePNG(BmpGetBits(WinGetBitmap(d.winbufM)), png, &bytes_written, 
	    d.record_note, d.record_name, d.hires);
  if (err != errNone) {
    MemHandleFree(png);
    return;
  }

  buffer = MemHandleLock(png);

  /* Create filename */
  rec_name = MemHandleLock(d.record_name);
  len = StrLen(rec_name);
  if (len) {
    name = MemPtrNew(len + 5); /* ".png" + 0-byte */
    if (!name) {
      MemHandleUnlock(png);
      MemHandleFree(png);
      NoMemoryError();
    }
    StrCopy(name, rec_name);
  } else {
    name = MemPtrNew(32);
    if (!name) {
      MemHandleUnlock(png);
      MemHandleFree(png);
      NoMemoryError();
    }
    SysCopyStringResource(name, BeamTypeString);
  }
  StrCat(name, ".png");
  MemHandleUnlock(d.record_name);

  Send("image/png", name, buffer, bytes_written);

  /* Clean up */
  MemPtrFree(name);
  MemHandleUnlock(png);
  MemHandleFree(png);
}

/******************************************************************************
 *
 * Send data using SnapperMail.
 *
 * For OS4.0 it uses the transport scheme of "_mail".
 * NOTE: The "_mail" scheme is a self-defined scheme by SnapperMail. It will
 *       only work with the SnapperMail exchange library installed.
 *
 *
 * For OS3.0 through OS3.5 transport schemes are not available so we use the
 * exchange manager set to local mode transfer, originally a feature designed for
 * developmental beam testing. This is made to work by hacking the object name
 * (with dots escaped and .mail appended). SnapperMail registers for the .mail
 * extension, receives the object and recovers the original filename.
 *
 *****************************************************************************/
Err Send(Char type[], Char filename[], MemPtr buffer, UInt32 bufferSize) {
  Char *url = NULL;
  UInt32 romVersion;
  Err err = memErrNotEnoughSpace;
  ExgSocketType* socket = MemPtrNew(sizeof(ExgSocketType));

  ASSERT(socket);

  if (socket) {
    MemSet(socket, sizeof(ExgSocketType), 0);

    /* Check for extended Exchange Manager (OS 4.0.0) */
    FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);
    if (romVersion >= sysVersion40) {
      // type holds the MIME type, e.g. "image/jpeg" or NULL.
      // we only populate the MIME type in 4.0 or above
      // in 3.5 the MIME type will override our .mail extension
      socket->type = type;

      url = MemPtrNew(StrLen(filename) + StrLen(exgSendBeamPrefix) + 1);
      ASSERT(url);
      if (url) {
	StrCopy(url, exgSendBeamPrefix);
	StrCat(url, filename);
      }
    } else {
      socket->localMode = 1;

      // This allocates memory for us.
      url = EscapeDotsInFilename(filename);
    }

    if (url) {
      socket->libraryRef = 0;
      socket->count = 1; // We are sending one object.
      socket->length = bufferSize; // Set the size of the object.
      // If you want the SnapperMail to switch back to your app.
      socket->appData = AppType;
      socket->name = url;
      socket->description = filename; // Displayed in status form.
      if (bufferSize < maximumNoStatusSize) {
	socket->noStatus = true;
      }

      err = ExgPut(socket);

      if (!err) {
	ExgSend(socket, buffer, bufferSize, &err);
	ExgDisconnect(socket, err);
      }
    }
  }

  /* Clean up */
  if (socket)
    MemPtrFree(socket);

  if (url)
    MemPtrFree(url);

  return err;
}
