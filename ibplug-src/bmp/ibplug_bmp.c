/* Main code for Windows Bitmap */

#include <PalmOS.h>
#include <VFSMgr.h>
#include "booger.h"

#include "ibplug_bmpRsc.h"
#include "ibplug_bmp.h"

/* Numbers of pixels per line */
#define MAX_PIXELS 160
#define MAX_PIXELS_HIRES 320

/*** Plugin specific code ***/
typedef struct {
  UInt8 signature[2]; /* Must be always set to 'BM' */
  UInt32 fileSize;  /* File size in bytes */
  UInt16 reserved1; /* Unused (= 0) */
  UInt16 reserved2; /* Unused (= 0) */
  UInt32 offset;    /* Offset of the raster data */
} BitmapFileHeader;

typedef struct {
  UInt32 size;            /* sizeof(BitmapInfoHeader) */
  Int32 width;            /* Width in pixels */
  Int32 height;           /* Height in pixels */
  UInt16 planes;          /* Number of planes = 1 */
  UInt16 bitDepth;        /* Bits per pixel (1/4/8/16/24) */
  UInt32 compression;     /* => BitmapCompression */
  UInt32 imageSize;       /* (Compressed) size of image. It is valid to set this =0 if Compression = 0. */
  Int32 xPixelsPerM;     /* Horizontal resolution */
  Int32 yPixelsPerM;     /* Vertical resolution */
  UInt32 colorsUsed;      /* Actual number of colors used */
  UInt32 colorsImportant; /* Number of important colors */
} BitmapInfoHeader;

typedef enum {
  RGB,  /* No compression */
  RLE8, /* 8-bit RLE encoding */
  RLE4  /* 4-bit RLE encoding */
} BitmapCompression;

typedef struct {
  UInt8 blue;     /* Blue intensity */
  UInt8 green;    /* Green intensity */
  UInt8 red;      /* Red intensity */
  UInt8 reserved; /* Unused (= 0) */
} BitmapColor;

/*

Windows Bitmap (DIB) format:
===========================

[BitmapFileHeader]
[BitmapInfoHeader]
[BitmapColor] * Number of colors (if bitDepth <= 8)
[Raster data] in bottom-up format

*/

/* Write a single byte to the given file */
extern inline Err WriteByte(FileRef fileRef, UInt8 data, UInt32 *numBytesWrittenP) {
  return VFSFileWrite(fileRef, sizeof(data), &data, numBytesWrittenP);
}

/* Write a 16-bit word to the given file - in little-endian byteorder */
static Err WriteTwoBytes(FileRef fileRef, UInt16 data, UInt32 *numBytesWrittenP) {
  Err err;
  UInt32 dummy;

  err = WriteByte(fileRef, data & 0xff, &dummy);
  if (numBytesWrittenP) *numBytesWrittenP = dummy;

  if (err == errNone) {
    err = WriteByte(fileRef, (data >> 8) & 0xff, &dummy);
    if (numBytesWrittenP) *numBytesWrittenP += dummy;
  }

  return err;
}

/* Write a 32-bit dword to the given file - in little-endian byteorder */
static Err WriteFourBytes(FileRef fileRef, UInt32 data, UInt32 *numBytesWrittenP) {
  Err err;
  UInt32 dummy;

  err = WriteByte(fileRef, data & 0xff, &dummy);
  if (numBytesWrittenP) *numBytesWrittenP = dummy;

  if (err == errNone) {
    err = WriteByte(fileRef, (data >> 8) & 0xff, &dummy);
    if (numBytesWrittenP) *numBytesWrittenP += dummy;
  }

  if (err == errNone) {
    err = WriteByte(fileRef, (data >> 16) & 0xff, &dummy);
    if (numBytesWrittenP) *numBytesWrittenP += dummy;
  }

  if (err == errNone) {
    err = WriteByte(fileRef, (data >> 24) & 0xff, &dummy);
    if (numBytesWrittenP) *numBytesWrittenP += dummy;
  }

  return err;
}

/* Get preferences, open (or create) app database */
static UInt16
StartApplication(void)
{
    return (errNone);
}

/* Save preferences, close forms, close app database */
static void
StopApplication(void)
{
}

/* Create new item in app database */
static UInt16
DoTheBoogie(KleenexPtr kleenexP, DmOpenRef dbR, UInt16 *index) {
  Int16 i;
  Err err = errNone;
  FileRef fileRef;
  UInt16 volRef;
  UInt32 volIterator = vfsIteratorStart;
  UInt32 vfsMgrVersion;
  UInt8* buffer;
  UInt8* topOfBuffer;
  Char* fileName = MemPtrNew(StrLen(kleenexP->text) + 4 + 1 + 1);
  BitmapFileHeader fileHeader;
  BitmapInfoHeader infoHeader;
  static const BitmapColor colorTable[2] = { {255, 255, 255, 0}, {0, 0, 0, 0} };
  UInt16 bytesPerRow;
  UInt16 bmpBytesPerRow;
  Int32 maxPixels, maxBytes;

  /* Set up our "constants" */
  if (kleenexP->flags & IBFLAG_HIRES) {
    maxPixels = MAX_PIXELS_HIRES;
  } else {
    maxPixels = MAX_PIXELS;
  }
  maxBytes = maxPixels * maxPixels / 8;
  bytesPerRow = maxPixels / 8;
  bmpBytesPerRow = (bytesPerRow + 3) / 4 * 4;

  /* Set up buffer */
  buffer = MemPtrNew(maxBytes);
  topOfBuffer = buffer + maxBytes;

  /* Check for not enough memory */
  if (!buffer) {abort(); return 1;}
  if (!fileName) { MemPtrFree(buffer); return 1; }
  
  /* Is the proposed filename OK? */
  if (StrLen(kleenexP->text)) {
    StrCopy(fileName, "/");
    StrCat(fileName, kleenexP->text);
    StrCat(fileName, ".bmp");
  } else {
    /* Empty filename */
    FrmAlert(alertID_noFileName);
    goto exit;
  }

  /* Check for VFS-Manager */
  err = FtrGet(sysFileCVFSMgr, vfsFtrIDVersion, &vfsMgrVersion);
  if (err) {
    /* VFS Manager not installed */
    FrmAlert(alertID_noVFSManager);
    goto exit;
  }

  /* Create file */
  err = VFSVolumeEnumerate(&volRef, &volIterator);
  if (err) {
    /* No volume inserted */
    FrmAlert(alertID_noVolume);
    goto exit;
  }

  err = VFSFileCreate(volRef, fileName);
  if (err) {
    /* Could not create file */
    StrCopy(fileName, kleenexP->text);
    StrCat(fileName, ".bmp");
    FrmCustomAlert(alertID_fileExists, fileName, "", "");
    goto exit;
  }

  err = VFSFileOpen(volRef, fileName, vfsModeWrite, &fileRef);
  if (err) {
    /* Could not open file */
    goto exit;
  }

  /* Fill in header information */
  fileHeader.signature[0] = 'B';
  fileHeader.signature[1] = 'M';
  fileHeader.fileSize = sizeof(fileHeader) + sizeof(infoHeader) + sizeof(colorTable) + maxPixels * bmpBytesPerRow;
  fileHeader.reserved1 = 0;
  fileHeader.reserved2 = 0;
  fileHeader.offset =  sizeof(fileHeader) + sizeof(infoHeader) + sizeof(colorTable);

  infoHeader.size = sizeof(infoHeader);
  infoHeader.width = maxPixels;
  infoHeader.height = maxPixels;
  infoHeader.planes = 1;
  infoHeader.bitDepth = 1;
  infoHeader.compression = (UInt32) RGB;
  infoHeader.imageSize = maxPixels * bmpBytesPerRow;
  infoHeader.xPixelsPerM = 0x0B13; /* default resolution */
  infoHeader.yPixelsPerM = 0x0B13; /* default resolution */
  infoHeader.colorsUsed = 2;
  infoHeader.colorsImportant = 0L; /* All colors are important */

  /* Write .bmp-header */
  err = WriteByte(fileRef, fileHeader.signature[0], NULL);
  if (err) goto close;
  err = WriteByte(fileRef, fileHeader.signature[1], NULL);
  if (err) goto close;
  err = WriteFourBytes(fileRef, fileHeader.fileSize, NULL);
  if (err) goto close;
  err = WriteTwoBytes(fileRef, fileHeader.reserved1, NULL);
  if (err) goto close;
  err = WriteTwoBytes(fileRef, fileHeader.reserved2, NULL);
  if (err) goto close;
  err = WriteFourBytes(fileRef, fileHeader.offset, NULL);
  if (err) goto close;

  err = WriteFourBytes(fileRef, infoHeader.size, NULL);
  err = WriteFourBytes(fileRef, infoHeader.width, NULL);
  err = WriteFourBytes(fileRef, infoHeader.height, NULL);
  err = WriteTwoBytes(fileRef, infoHeader.planes, NULL);
  err = WriteTwoBytes(fileRef, infoHeader.bitDepth, NULL);
  err = WriteFourBytes(fileRef, infoHeader.compression, NULL);
  err = WriteFourBytes(fileRef, infoHeader.imageSize, NULL);
  err = WriteFourBytes(fileRef, infoHeader.xPixelsPerM, NULL);
  err = WriteFourBytes(fileRef, infoHeader.yPixelsPerM, NULL);
  err = WriteFourBytes(fileRef, infoHeader.colorsUsed, NULL);
  err = WriteFourBytes(fileRef, infoHeader.colorsImportant, NULL);

  err = VFSFileWrite(fileRef, sizeof(colorTable), &colorTable, NULL);
  if (err) goto close;

  /* Turn upside down */
  for (i = 0; i < maxBytes; i += bytesPerRow)
    MemMove(topOfBuffer - i - bytesPerRow, kleenexP->data + i, bytesPerRow);

  /* Write data */
  err = VFSFileWrite(fileRef, MemPtrSize(buffer), buffer, NULL);
  if (err) goto close;

 close:
  if (err) FrmAlert(alertID_diskFull); /* most likely there's not enough space left */
  VFSFileClose(fileRef);

 exit:
  /* Clean up */
  MemPtrFree(buffer);
  MemPtrFree(fileName);

  return err;
}

/*** End of plugin specific code ***/


/* Finds and opens database,
 * calls plugin specific code to create new item,
 * then fills in GoTo parameters.
 */
static UInt16
PickBooger(KleenexPtr kleenexP)
{
  Err err;

  /* Check for the correct version */
  if (!((kleenexP->version) & IBVERSION_PICTURE) ||
      !((kleenexP->version) & IBVERSION_FLAGS))
    return (boogerErrorVersionMismatch);

  /* Call plugin specific routine to create item in database */
  err = DoTheBoogie(kleenexP, NULL, NULL);

  return err;
}

/* Fills in 'booger' structure for return to calling app */
static UInt16
BlowNose(KleenexPtr kleenexP)
{
    return errNone;
}


/* Jumps to newly created item using app specified in 'booger' structure */
static UInt16
Sneeze(KleenexPtr kleenexP)
{
    UInt16 err;
    LocalID dbID;
    UInt16 cardNo;

    // Free up parameters in 'kleenex' structure
    if (kleenexP->note)      MemPtrFree(kleenexP->note);
    if (kleenexP->data)      MemPtrFree(kleenexP->data);
    if (kleenexP->text)      MemPtrFree(kleenexP->text);
    if (kleenexP->title)     MemPtrFree(kleenexP->title);
    if (kleenexP->category)  MemPtrFree(kleenexP->category);
    if (kleenexP->repeat)    MemPtrFree(kleenexP->repeat);

    // Jump to default app if not specified
    if (!kleenexP->booger.dbID) {
        if ((err = BlowNose(kleenexP)))
            return (err);
    }

    if ((kleenexP->booger.cmd == sysAppLaunchCmdNormalLaunch) ||
        (kleenexP->booger.cmd == boogerPlugLaunchCmdLaunchLauncher))
    {
        // Don't use the GoTo params
        if (kleenexP->booger.cmdPBP) {
            MemPtrFree(kleenexP->booger.cmdPBP);
            kleenexP->booger.cmdPBP = NULL;
        }

    } else if (kleenexP->booger.cmd == sysAppLaunchCmdOpenDB) {
        // Change GoTo params to OpenDB params
        cardNo = ((GoToParamsType *)(kleenexP->booger.cmdPBP))->dbCardNo;
        dbID = ((GoToParamsType *)(kleenexP->booger.cmdPBP))->dbID;
        ((SysAppLaunchCmdOpenDBType *)(kleenexP->booger.cmdPBP))->cardNo = cardNo;
        ((SysAppLaunchCmdOpenDBType *)(kleenexP->booger.cmdPBP))->dbID = dbID;
    }

    // Achoo!
    if (SysUIAppSwitch(kleenexP->booger.cardNo, kleenexP->booger.dbID,
                       kleenexP->booger.cmd, kleenexP->booger.cmdPBP))
    {
        if (kleenexP->booger.cmdPBP)
            MemPtrFree(kleenexP->booger.cmdPBP);
        return (1);
    }

    return (errNone);
}


/* Start here! */
UInt32
PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags)
{
    UInt16 err;

    // Called as a subroutine
    if (cmd == boogerPlugLaunchCmdBlowNose) {
        if ((err = StartApplication()))
            return (err);

        if (!(err = PickBooger((KleenexPtr)cmdPBP)))
            err = BlowNose((KleenexPtr)cmdPBP);

        StopApplication();
        return (err);
    }

    // Running as an application
    if (cmd == boogerPlugLaunchCmdSneeze) {
        if ((err = StartApplication()))
            return (err);

        if (!(err = PickBooger((KleenexPtr)cmdPBP)))
            err = Sneeze((KleenexPtr)cmdPBP);

        StopApplication();
        return (err);
    }

    return (sysErrParamErr);
}

// EOF
