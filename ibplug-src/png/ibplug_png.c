/* Main code for Windows Bitmap */

#include <PalmOS.h>
#include <VFSMgr.h>
#include "booger.h"
#include "SysZLib.h"

#include "ibplug_pngRsc.h"
#include "ibplug_png.h"
#include "../../debug.h"

/* Numbers of pixels per line */
#define MAX_PIXELS 160
#define MAX_PIXELS_HIRES 320

/*** Plugin specific code ***/

/* The PNG file signature */
static const UInt8 pngSignature[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };

/*
** A PNG file consists of several chunks
*/

/*
typedef struct {
  UInt32 len;   // the length of the data field
  UInt32 type;  // chunk type
  UInt8 data[]; // chunk data
  UInt32 crc;   //CRC of type + data
} PNGChunk;
*/

/* The IHDR image header chunk */
typedef struct {
  UInt32 len;   /* 13 */
  UInt32 type;  /* 'IHDR' */

  UInt32 width;
  UInt32 height;
  UInt8 bitDepth;
  UInt8 colorType;
  UInt8 compressionMethod;
  UInt8 filterMethod;
  UInt8 interlaceMethod;

  UInt32 crc; /* CRC of type + data */
} IHDRChunk;

/* The possible color types */
typedef enum {
  grayscale = 0,      /* 1,2,4,8,16  Each pixel is a grayscale sample. */
  rgb = 2,            /* 8,16        Each pixel is an R,G,B triple. */
  palette = 3,        /* 1,2,4,8     Each pixel is a palette index; a PLTE chunk must appear. */
  grayscaleAlpha = 4, /* 8,16        Each pixel is a grayscale sample, followed by an alpha sample. */
  rgbAlpha = 6        /* 8,16        Each pixel is an R,G,B triple, followed by an alpha sample. */
} PNGColorType;

/* The available compression methods */
typedef enum {
  deflate = 0 /* deflate/inflate with a sliding window of at most 32768 bytes */
} PNGCompressionMethod;

/* The available filter methods */
typedef enum {
  adaptive5 = 0 /* adaptive filtering with five basic filter types */
} PNGFilterMethod;

/* The available interlace methods */
typedef enum {
  noInterlace = 0,
  adam7 = 1
} PNGInterlaceMethod;

/* The IEND chunk */
typedef struct {
  UInt32 len;  /* 0 */
  UInt32 type; /* 'IEND' */

  /* empty data field */

  UInt32 crc; /* CRC of type + data */
} IENDChunk;

/* The IEND chunk */
typedef struct {
  UInt32 len;
  UInt32 type; /* 'IDAT' */

  UInt8* data;

  UInt32 crc; /* CRC of type + data */
} IDATChunk;

static const IENDChunk iend = { 0, 'IEND', 0xae426082 };

/*

PNG format:
===========

[IHDR]
[IDAT]
[IEND]

*/

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
  Err err = errNone;
  FileRef fileRef;
  UInt16 volRef;
  UInt32 volIterator = vfsIteratorStart;
  UInt32 vfsMgrVersion;
  Char* fileName = MemPtrNew(StrLen(kleenexP->text) + 4 + 1 + 1);
  IHDRChunk ihdr;
  IDATChunk idat;
  UInt16 ZLibRef = 0;
  Int16 i;
  z_stream zstrm;
  UInt8* buffer = NULL;
  UInt16 bytesPerRow;
  UInt16 pngBytesPerRow;
  Int32 maxPixels, maxBytes;

  /* Set up our "constants" */
  if (kleenexP->flags & IBFLAG_HIRES) {
    maxPixels = MAX_PIXELS_HIRES;
  } else {
    maxPixels = MAX_PIXELS;
  }
  maxBytes = maxPixels * maxPixels / 8;
  bytesPerRow = maxPixels / 8;
  pngBytesPerRow = bytesPerRow + 1; /* for the filter method */

  /* Check for not enough memory */
  if (!fileName) return 1;

  /* Is the proposed filename OK? */
  if (StrLen(kleenexP->text)) {
    StrCopy(fileName, "/");
    StrCat(fileName, kleenexP->text);
    StrCat(fileName, ".png");
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

  /* Set up zlib */
  ZLSetup;

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
    StrCat(fileName, ".png");
    FrmCustomAlert(alertID_fileExists, fileName, "", "");
    goto exit;
  }

  err = VFSFileOpen(volRef, fileName, vfsModeWrite, &fileRef);
  if (err) {
    /* Could not open file */
    goto exit;
  }

  /* Fill in header information */

  /* Write .png-header */
  err = VFSFileWrite(fileRef, sizeof(pngSignature), &pngSignature, NULL);
  if (err) goto close;

  ihdr.len = 13;
  ihdr.type = 'IHDR';
  ihdr.width = maxPixels;
  ihdr.height = maxPixels;
  ihdr.bitDepth = 1;
  ihdr.colorType = grayscale;
  ihdr.compressionMethod = deflate;
  ihdr.filterMethod = adaptive5;
  ihdr.interlaceMethod = noInterlace;

  ihdr.crc = crc32(crc32(0, Z_NULL, 0), (UInt8*) &ihdr + 4, 17);

  err = VFSFileWrite(fileRef, sizeof(ihdr.len), &ihdr.len, NULL);
  err = VFSFileWrite(fileRef, sizeof(ihdr.type), &ihdr.type, NULL);
  err = VFSFileWrite(fileRef, sizeof(ihdr.width), &ihdr.width, NULL);
  err = VFSFileWrite(fileRef, sizeof(ihdr.height), &ihdr.height, NULL);
  err = VFSFileWrite(fileRef, sizeof(ihdr.bitDepth), &ihdr.bitDepth, NULL);
  err = VFSFileWrite(fileRef, sizeof(ihdr.colorType), &ihdr.colorType, NULL);
  err = VFSFileWrite(fileRef, sizeof(ihdr.compressionMethod), &ihdr.compressionMethod, NULL);
  err = VFSFileWrite(fileRef, sizeof(ihdr.filterMethod), &ihdr.filterMethod, NULL);
  err = VFSFileWrite(fileRef, sizeof(ihdr.interlaceMethod), &ihdr.interlaceMethod, NULL);
  err = VFSFileWrite(fileRef, sizeof(ihdr.crc), &ihdr.crc, NULL);

  if (err) goto close;

  /* Generate data */
  buffer = MemPtrNew(maxPixels * pngBytesPerRow);
  idat.data = MemPtrNew((maxPixels * pngBytesPerRow) * 2);

  /* Invert pixels */
  for (i = 0; i < maxBytes; i += 4) {
    * ((UInt32*) (kleenexP->data + i)) =  ~(* ((UInt32*) (kleenexP->data + i)));
  }

  for (i = 0; i < maxPixels; ++i) {
    *(buffer + i * pngBytesPerRow) = 0; /* no filtering */
    MemMove(buffer + i * pngBytesPerRow + 1, kleenexP->data + i * bytesPerRow, bytesPerRow);
  }

  zstrm.next_in = buffer;
  zstrm.avail_in = maxPixels * pngBytesPerRow;
  zstrm.next_out = idat.data;
  zstrm.avail_out = (maxPixels * pngBytesPerRow) * 2;

  zstrm.zalloc = Z_NULL;
  zstrm.zfree = Z_NULL;
  zstrm.opaque = NULL;

  deflateInit(&zstrm, 6);
  if (deflate(&zstrm, Z_FINISH) != Z_STREAM_END) abort();

  /* Write data */
  idat.len = zstrm.total_out;
  idat.type = 'IDAT';
  idat.crc = crc32(crc32(0, Z_NULL, 0), (UInt8*) &idat + 4, 4);
  idat.crc = crc32(idat.crc, idat.data, idat.len);

  deflateEnd(&zstrm);

  err = VFSFileWrite(fileRef, sizeof(idat.len), &idat.len, NULL);
  err = VFSFileWrite(fileRef, sizeof(idat.type), &idat.type, NULL);
  err = VFSFileWrite(fileRef, idat.len, idat.data, NULL);
  err = VFSFileWrite(fileRef, sizeof(idat.crc), &idat.crc, NULL);

  /* Write IEND chunk */
  err = VFSFileWrite(fileRef, sizeof(iend), &iend, NULL);
  if (err) goto close;

 close:
  if (err) FrmAlert(alertID_diskFull); /* most likely there's not enough space left */
  VFSFileClose(fileRef);

 exit:
  /* Clean up */
  MemPtrFree(fileName);
  if (idat.data) MemPtrFree(idat.data);
  if (buffer) MemPtrFree(buffer);
  ZLTeardown;

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
