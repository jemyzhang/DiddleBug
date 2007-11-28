/*

(c) 2003, 2004 Peter Putzer <pputzer@users.sourceforge.net>

Licensed under the GNU General Public License (GPL).

*/

#include <PalmOS.h>

#define NOZLIBDEFS
#include "SysZLib.h"

#include "png.h"
#include "diddlebug.h"
#include "diddlebugRsc.h"
#include "diddlebugInlines.h"
#include "debug.h"

#define ChunkSize 1024

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
  UInt32 crc;   // CRC of type + data
} PNGChunk;
*/

typedef struct {
  UInt32 len;   /* the length of the data field */
  UInt32 type;  /* chunk type */
} PNGChunkHeader;

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
...
optional palette chunks
...
[IDAT]
...
optional tEXt chunks
...
[IEND]

*/

/* The keyword used for tEXt chunks containing our "note" */
static const Char noteKeyword[] = "Comment";
static const Char titleKeyword[] = "Title";

/* static functions */
static Err Read(MemPtr src, MemPtr buf, UInt32 size, UInt32* offset);
static Err Write(MemHandle dstH, UInt8** dst, UInt32* dstSize, const UInt8* src, UInt32 size, UInt32* offset) SUTIL3;
static Err DecodeIDATChunks(UInt16 ZLibRef, MemPtr stream, UInt32* offset,
			    IDATChunk* idat, UInt8* buffer, Boolean hires);
static Boolean DecodeNote(UInt16 ZLibRef, MemPtr stream, UInt32* offset,
			  PNGChunkHeader* chunk, MemHandle* note);
static void HandlePNGError(Err err);

/*
** Read specified number of bytes from src into buf and increase offset.
*/
static Err Read(MemPtr src, MemPtr buf, UInt32 size, UInt32* offset) {
  Err res;

  res = MemMove(buf, src + *offset, size);
  *offset += size;

  return res;
}

#define READ(A, B) Read(stream, A, B, &s->read)

/*
** Pop up the right error message
*/
static void HandlePNGError(Err err) {
  if (err) {
    switch (err) {
    case Z_STREAM_ERROR:
      FrmCustomAlert(GenericError, "Stream fucked up", "", "");
      break;
    case Z_BUF_ERROR:
      FrmCustomAlert(GenericError, "Buffer fucked up", "", "");
      break;
    case Z_DATA_ERROR:
      FrmCustomAlert(GenericError, "Data corrupted", "", "");
      break;
    case Z_NEED_DICT:
      FrmCustomAlert(GenericError, "Dictionary needed", "", "");
      break;
    case Z_MEM_ERROR:
      FrmCustomAlert(GenericError, "Out of Memory", "", "");
      break;
    case Z_VERSION_ERROR:
      FrmCustomAlert(GenericError, "Wrong ZLib Version " ZLIB_VERSION, "", "");
      break;
    case sysErrLibNotFound:
      FrmCustomAlert(GenericError, "ZLib not installed", "", "");
      break;

    default:
      FrmCustomAlert(GenericError, "Unknown error while decoding PNG file", "", "");
    }
  }
}

/*
** Decode IDAT chunks.
*/
static Err DecodeIDATChunks(UInt16 ZLibRef, MemPtr stream, UInt32* offset,
			    IDATChunk* idat, UInt8* buffer, Boolean hires) {
  struct StackFrame {
    UInt32 read;
    UInt32 tmp_crc;
    UInt8* dec_buffer;
    z_stream zstrm;
    UInt32 dec_buffer_size;
    UInt32 bytesPerRow;
    UInt32 pngBytesPerRow;
    UInt32 totalBytes;
    UInt32 maxPixels;
  } *s;

  Err err = errNone;
  Int16 i;

  /* Reserve & initialize virtual stack frame */
  s = MemPtrNew(sizeof(struct StackFrame));
  ASSERT(s);
  MemSet(s, sizeof(struct StackFrame), 0);
  s->read = *offset;

  /* Set up our "constants" */
  if (hires) {
    s->maxPixels = MAX_PIXELS_HIRES;
  } else {
    s->maxPixels = MAX_PIXELS;
  }
  s->totalBytes = s->maxPixels * s->maxPixels / 8;
  s->bytesPerRow = s->maxPixels / 8;
  s->pngBytesPerRow = s->maxPixels / 8 + 1; /* for the filter method */

  /* Get IDAT data */
  idat->data = MemPtrNew(idat->len);
  ASSERT(idat->data);
  err = READ(idat->data, idat->len);
  if (err) goto exit;
  err = READ(&idat->crc, sizeof(idat->crc));
  s->tmp_crc = crc32(crc32(0, Z_NULL, 0), (UInt8*) idat + 4, 4);
  if (err || idat->crc != crc32(s->tmp_crc, idat->data, idat->len))
    goto exit;

  /* Inflate bitmap data */
  s->zstrm.next_in = idat->data;
  s->zstrm.avail_in = idat->len;
  s->zstrm.avail_out = s->dec_buffer_size = s->totalBytes + 400; /* Should always be enough */
  s->zstrm.next_out = s->dec_buffer = MemPtrNew(s->dec_buffer_size);
  ASSERT(s->dec_buffer);

  s->zstrm.zalloc = Z_NULL;
  s->zstrm.zfree = Z_NULL;
  s->zstrm.opaque = Z_NULL;

  err = inflateInit(&s->zstrm);
  if (err != Z_OK) goto exit;

  err = inflate(&s->zstrm, Z_SYNC_FLUSH);
  while (err != Z_STREAM_END) {
    if (err == Z_OK) {
      if (s->zstrm.avail_out == 0) {
	/* Make more room */
	UInt32 old_size = s->dec_buffer_size;
	s->dec_buffer_size += ChunkSize;
	err = MemPtrResize(s->dec_buffer, s->dec_buffer_size);
	if (err) goto exit;

	/* Update zlib-stream */
	s->zstrm.avail_out = s->dec_buffer_size - old_size;
	s->zstrm.next_out = s->dec_buffer + old_size;
      } else {
	/* Provide more input */

	/* Clean up old data */
	MemPtrFree(idat->data);
	idat->data = NULL;

	/* Read next chunk header */
	err = READ(&idat->len, sizeof(idat->len));
	if (err) goto exit;
	err = READ(&idat->type, sizeof(idat->type));
	if (err || idat->type != 'IDAT') goto exit;

	/* Get IDAT data */
	idat->data = MemPtrNew(idat->len);
	ASSERT(idat->data);
	err = READ(idat->data, idat->len);
	if (err) goto exit;
	err = READ(&idat->crc, sizeof(idat->crc));
	s->tmp_crc = crc32(crc32(0, Z_NULL, 0), (UInt8*) idat + 4, 4);
	if (err || idat->crc != crc32(s->tmp_crc, idat->data, idat->len))
	  goto exit;

	s->zstrm.next_in = idat->data;
	s->zstrm.avail_in = idat->len;
      }
    } else {
      /* Some error occured */
      goto exit;
    }

    /* Go on */
    err = inflate(&s->zstrm, Z_SYNC_FLUSH);
  }

  err = inflateEnd(&s->zstrm);
  if (err != Z_OK) goto exit;

  /* Remove filter-type byte */
  for (i = 0; i < s->maxPixels; ++i)
    MemMove(buffer + i * s->bytesPerRow, s->dec_buffer + i * s->pngBytesPerRow + 1, s->bytesPerRow);

  /* Invert pixels */
  for (i = 0; i < s->totalBytes; i += 4)
    * ((UInt32*) (buffer + i)) =  ~(* ((UInt32*) (buffer + i)));

 exit:
  /* Provide error message */
  HandlePNGError(err);

  /* Clean up */
  if (idat->data) MemPtrFree(idat->data);
  if (s->dec_buffer) MemPtrFree(s->dec_buffer);

  /* Give back number of read bytes */
  *offset = s->read;

  /* Clean up virtual stack frame */
  MemPtrFree(s);

  return err;
}

/*
** Read note from "Comment" tEXt chunk.
*/
static Boolean DecodeNote(UInt16 ZLibRef, MemPtr stream, UInt32* offset,
			  PNGChunkHeader* chunk, MemHandle* note) {
  struct StackFrame {
    Char keyword[80];
    UInt32 read;
    UInt16 len;
    Char* noteP;
    UInt32 calc_crc;
    UInt32 crc;
    UInt16 max_index;
  } *s;

  Boolean res = false;

  /* Reserve & initialize virtual stack frame */
  s = MemPtrNew(sizeof(struct StackFrame));
  ASSERT(s);
  MemSet(s, sizeof(struct StackFrame), 0);
  s->read = *offset;
  s->max_index = Min(chunk->len, 80);

  /* Read the (longest possible) keyword */
  READ(s->keyword, s->max_index);
  s->keyword[s->max_index] = 0; /* Just in case the stream has been corrupted */

  if (!StrCompare(s->keyword, noteKeyword)) {
    res = true;
    s->len = StrLen(s->keyword) + 1;  /* don't forget the '\0' byte */
    s->read = *offset + s->len;

    if (*note) MemHandleFree(*note);
    *note = MemHandleNew(chunk->len - s->len + 1);
    ASSERT(*note);
    s->noteP = MemHandleLock(*note);
    READ(s->noteP, chunk->len - s->len);
    s->noteP[chunk->len - s->len] = 0; /* Properly terminate the string */

    s->calc_crc = crc32(crc32(0L, NULL, 0), (MemPtr) &chunk->type, sizeof(chunk->type));
    s->calc_crc = crc32(s->calc_crc, noteKeyword, sizeof(noteKeyword)); /* include '\0' */
    s->calc_crc = crc32(s->calc_crc, s->noteP, chunk->len - s->len); /* do not include '\0' */

    READ(&s->crc, sizeof(s->crc));

    /* Clean up */
    MemHandleUnlock(*note);

    if (s->calc_crc != s->crc) {
      res = false;
      MemHandleFree(*note);
      *note = NULL;
    }
  } else {
    /* Skip this chunk */
    s->read += chunk->len + sizeof(UInt32); /* also skip CRC field */
  }

  /* Give back number of read bytes */
  *offset = s->read;

  /* Clean up virtual stack frame */
  MemPtrFree(s);

  return res;
}

/*
** Decode a PNG stream into "buffer".
**
** "buffer" has to be sized x * y / 8 bytes.
*/
Err DecodePNG(MemPtr stream, UInt32 size, UInt8* buffer, MemHandle* note,
	      Boolean hires) {
  struct StackFrame {
    IHDRChunk ihdr;
    IDATChunk idat;
    UInt8 sig[8];
    Boolean found;
    PNGChunkHeader chunk;
    UInt32 read;
    UInt32 maxPixels;
  } *s;
  Err err = errNone;
  UInt16 ZLibRef = 0;

  /* Reserve & initialize virtual stack frame */
  s = MemPtrNew(sizeof(struct StackFrame));
  ASSERT(s);
  MemSet(s, sizeof(struct StackFrame), 0);

  /* Set up our "constants" */
  if (hires) {
    s->maxPixels = MAX_PIXELS_HIRES;
  } else {
    s->maxPixels = MAX_PIXELS;
  }

  ASSERT(buffer);

  /* Set up zlib */
  err = ZLSetup;
  if (err != errNone) goto exit;

  /* Read header information */
  err = READ(&s->sig, sizeof(s->sig));
  if (err) goto exit;

  /* Check signature */
  err = MemCmp(&s->sig, &pngSignature, sizeof(s->sig));
  if (err) goto exit;

  /* Read & check IHDR chunk */
  err = READ(&s->ihdr.len, sizeof(s->ihdr.len));
  if (err || s->ihdr.len != 13) goto exit;
  err = READ(&s->ihdr.type, sizeof(s->ihdr.type));
  if (err || s->ihdr.type != 'IHDR') goto exit;
  err = READ(&s->ihdr.width, sizeof(s->ihdr.width));
  if (err || s->ihdr.width != s->maxPixels) goto exit;
  err = READ(&s->ihdr.height, sizeof(s->ihdr.height));
  if (err || s->ihdr.height != s->maxPixels) goto exit;
  err = READ(&s->ihdr.bitDepth, sizeof(s->ihdr.bitDepth));
  if (err || s->ihdr.bitDepth != 1) goto exit;
  err = READ(&s->ihdr.colorType, sizeof(s->ihdr.colorType));
  if (err || s->ihdr.colorType != grayscale) goto exit;
  err = READ(&s->ihdr.compressionMethod, sizeof(s->ihdr.compressionMethod));
  if (err || s->ihdr.compressionMethod != deflate) goto exit;
  err = READ(&s->ihdr.filterMethod, sizeof(s->ihdr.filterMethod));
  if (err || s->ihdr.filterMethod != adaptive5) goto exit;
  err = READ(&s->ihdr.interlaceMethod, sizeof(s->ihdr.interlaceMethod));
  if (err || s->ihdr.interlaceMethod != noInterlace) goto exit;
  err = READ(&s->ihdr.crc, sizeof(s->ihdr.crc));
  if (err || s->ihdr.crc != crc32(crc32(0, Z_NULL, 0), (UInt8*) &s->ihdr + 4, 17))
    goto exit;

  /* Skip non-IDAT chunks */
  while (!s->found) {
    err = READ(&s->chunk.len, sizeof(s->chunk.len));
    if (err) goto exit;

    err = READ(&s->chunk.type, sizeof(s->chunk.type));
    if (err) goto exit;

    if (s->chunk.type == 'IDAT') {
      s->found = true;
      s->idat.type = s->chunk.type;
      s->idat.len = s->chunk.len;
    }
  }

  if (!s->found) {
    err = -1;
    goto exit;
  }

  /* Read IDAT chunk */
  err = DecodeIDATChunks(ZLibRef, stream, &s->read, &s->idat, buffer, hires);
  if (err) goto exit;

  /* See if there is a tEXT chunk (containing the note) */
  while (s->chunk.type != 'IEND' && s->read < size) {
    err = READ(&s->chunk.len, sizeof(s->chunk.len));
    if (err) goto exit;

    err = READ(&s->chunk.type, sizeof(s->chunk.type));
    if (err || s->chunk.type == 'IEND') goto exit;

    if (s->chunk.type == 'tEXt') {
      DecodeNote(ZLibRef, stream, &s->read, &s->chunk, note);
    } else {
      /* Skip this chunk */
      s->read += s->chunk.len + sizeof(UInt32); /* also skip CRC field */
    }
  }

 exit:
  ZLTeardown;

  if (err) {
    FrmCustomAlert(GenericError, "Error decoding PNG", "", "");
  }

  /* Clean up virtual stack frame */
  MemPtrFree(s);

  return err;
}

/*
** Write specified number of bytes to dst and increase offset.
*/
static Err Write(MemHandle dstH, UInt8** dst, UInt32* dstSize, const UInt8* src, UInt32 size, UInt32* offset) {
  Err res;

  if ((*offset + size) > *dstSize) {
    MemHandleUnlock(dstH);
    res = MemHandleResize(dstH, *dstSize + ChunkSize);
    *dstSize += ChunkSize;

    *dst = MemHandleLock(dstH);
  }

  res = MemMove(*dst + *offset, src, size);
  *offset += size;

  return res;
}

#define WRITE(A, B) Write(stream, &stream_buf, &stream_size, (const UInt8*) A, B, bytes_written)

/*
** Encode "buffer" into a PNG stream
**
** "stream" must be an unlocked handle of at least 1024 bytes size.
*/
Err EncodePNG(UInt8* orig_buffer, MemHandle stream, UInt32* bytes_written, MemHandle note, MemHandle title, Boolean hires) {
  Err err = errNone;
  IHDRChunk ihdr;
  IDATChunk idat = { 0, 0, NULL, 0 };
  UInt16 ZLibRef = 0;
  Int16 i;
  z_stream zstrm;
  UInt8* buffer = NULL;
  UInt32 stream_size = MemHandleSize(stream);
  UInt8* stream_buf = MemHandleLock(stream);
  PNGChunkHeader chunk;
  Char *noteP, *titleP;
  UInt16 note_len, title_len;
  UInt32 data_size;
  UInt32 crc; /* CRC for "Comment" tEXt chunk */
  UInt32 bytesPerRow;
  UInt32 pngBytesPerRow;
  UInt32 totalBytes;
  UInt32 maxPixels;

  /* Set up our "constants" */
  if (hires) {
    maxPixels = MAX_PIXELS_HIRES;
  } else {
    maxPixels = MAX_PIXELS;
  }
  totalBytes = maxPixels * maxPixels / 8;
  bytesPerRow = maxPixels / 8;
  pngBytesPerRow = maxPixels / 8 + 1; /* for the filter method */
  data_size = maxPixels * pngBytesPerRow * 2;

  /* Set up zlib */
  err = ZLSetup;
  if (err != errNone) goto exit;

  /* Initialize number of bytes that have been written */
  *bytes_written = 0;

  /* Fill in header information */

  /* Write .png-header */
  err = WRITE(&pngSignature, sizeof(pngSignature));
  if (err) goto exit;

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

  err = WRITE(&ihdr.len, sizeof(ihdr.len));
  if (err) goto exit;
  err = WRITE(&ihdr.type, sizeof(ihdr.type));
  if (err) goto exit;
  err = WRITE(&ihdr.width, sizeof(ihdr.width));
  if (err) goto exit;
  err = WRITE(&ihdr.height, sizeof(ihdr.height));
  if (err) goto exit;
  err = WRITE(&ihdr.bitDepth, sizeof(ihdr.bitDepth));
  if (err) goto exit;
  err = WRITE(&ihdr.colorType, sizeof(ihdr.colorType));
  if (err) goto exit;
  err = WRITE(&ihdr.compressionMethod, sizeof(ihdr.compressionMethod));
  if (err) goto exit;
  err = WRITE(&ihdr.filterMethod, sizeof(ihdr.filterMethod));
  if (err) goto exit;
  err = WRITE(&ihdr.interlaceMethod, sizeof(ihdr.interlaceMethod));
  if (err) goto exit;
  err = WRITE(&ihdr.crc, sizeof(ihdr.crc));
  if (err) goto exit;

  /* Generate data */
  buffer = MemPtrNew(maxPixels * pngBytesPerRow);
  ASSERT(buffer);
  idat.data = MemPtrNew(data_size);
  ASSERT(idat.data);

  /* Invert pixels */
  for (i = 0; i < totalBytes; i += 4) {
    * ((UInt32*) (orig_buffer + i)) =  ~(* ((UInt32*) (orig_buffer + i)));
  }

  for (i = 0; i < maxPixels; ++i) {
    *(buffer + i * pngBytesPerRow) = 0; /* no filtering */
    MemMove(buffer + i * pngBytesPerRow + 1, orig_buffer + i * bytesPerRow, bytesPerRow);
  }

  zstrm.next_in = buffer;
  zstrm.avail_in = maxPixels * pngBytesPerRow;
  zstrm.next_out = idat.data;
  zstrm.avail_out = data_size;

  zstrm.zalloc = Z_NULL;
  zstrm.zfree = Z_NULL;
  zstrm.opaque = NULL;

  err = deflateInit(&zstrm, 6);
  if (err != Z_OK) goto exit;

  err = deflate(&zstrm, Z_FINISH);
  while (err != Z_STREAM_END) {
    if (err == Z_OK) {
      if (zstrm.avail_out == 0) {
	/* Make more room */
	UInt32 old_size = data_size;
	data_size += ChunkSize;
	err = MemPtrResize(idat.data, data_size);
	if (err) goto exit;

	/* Update zlib-stream */
	zstrm.avail_out = data_size - old_size;
	zstrm.next_out = idat.data + old_size;
      } else {
	/* we need more input - which is impossible */
	goto exit;
      }
    } else {
      /* Some error occured */
      goto exit;
    }

    /* Go on */
    err = deflate(&zstrm, Z_FINISH);
  }

  /* Write data */
  idat.len = zstrm.total_out;
  idat.type = 'IDAT';
  idat.crc = crc32(crc32(0, Z_NULL, 0), (UInt8*) &idat + 4, 4);
  idat.crc = crc32(idat.crc, idat.data, idat.len);

  deflateEnd(&zstrm);

  err = WRITE(&idat.len, sizeof(idat.len));
  if (err) goto exit;
  err = WRITE(&idat.type, sizeof(idat.type));
  if (err) goto exit;
  err = WRITE(idat.data, idat.len);
  if (err) goto exit;
  err = WRITE(&idat.crc, sizeof(idat.crc));
  if (err) goto exit;

  /* Write tEXt chunk with "Comment" (note) */
  noteP = MemHandleLock(note);
  note_len = StrLen(noteP);
  chunk.len = sizeof(noteKeyword) + note_len; /* "Comment" + '\0' + note */
  chunk.type = 'tEXt';
  err = WRITE(&chunk.len, sizeof(chunk.len));
  if (err) goto exit;
  err = WRITE(&chunk.type, sizeof(chunk.type));
  if (err) goto exit;
  err = WRITE(noteKeyword, sizeof(noteKeyword)); /* including terminating '\0' */
  if (err) goto exit;
  err = WRITE(noteP, note_len); /* /not/ terminated! */
  if (err) goto exit;

  /* Calculate CRC for tEXt chunk */
  crc = crc32(crc32(0L, NULL, 0), (MemPtr) &chunk.type, sizeof(chunk.type));
  crc = crc32(crc, noteKeyword, sizeof(noteKeyword));
  crc = crc32(crc, noteP, note_len);

  /* Write CRC for tEXt chunk */
  err = WRITE(&crc, sizeof(crc));
  if (err) goto exit;

  /* For compatibility with NotePad: Write tEXt chunk with "Title" */
  titleP = MemHandleLock(title);
  title_len = StrLen(titleP);
  chunk.len = sizeof(titleKeyword) + title_len; /* "Comment" + '\0' + title */
  chunk.type = 'tEXt';
  err = WRITE(&chunk.len, sizeof(chunk.len));
  if (err) goto exit;
  err = WRITE(&chunk.type, sizeof(chunk.type));
  if (err) goto exit;
  err = WRITE(titleKeyword, sizeof(titleKeyword)); /* including terminating '\0' */
  if (err) goto exit;
  err = WRITE(titleP, title_len); /* /not/ terminated! */
  if (err) goto exit;

  /* Calculate CRC for tEXt chunk */
  crc = crc32(crc32(0L, NULL, 0), (MemPtr) &chunk.type, sizeof(chunk.type));
  crc = crc32(crc, titleKeyword, sizeof(titleKeyword));
  crc = crc32(crc, titleP, title_len);

  /* Write CRC for tEXt chunk */
  err = WRITE(&crc, sizeof(crc));
  if (err) goto exit;

  /* Write IEND chunk */
  err = WRITE(&iend, sizeof(iend));
  if (err) goto exit;

 exit:
  /* Provide error message */
  HandlePNGError(err);

  /* Clean up */
  if (idat.data) MemPtrFree(idat.data);
  if (buffer) MemPtrFree(buffer);
  MemHandleUnlock(stream);
  ZLTeardown;

  return err;
}
