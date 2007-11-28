/* diddlesnap.h

   Copyright © 2002, 2003 Peter Putzer
   Copyright © 1999 Mitch Blevins
   Licensed under the GNU GPL - http://www.gnu.org/copyleft/gpl.html
*/

// Define the minimum OS version we support (3.5), as
// well as other OS versions so we do the right thing everywhere
// Note that you should use < or >= in these comparisons, in nearly
// every case, because 3.0.1 should be treated the same way as 3.0.
// If you say "if vers <= 3.0" you'd often do the wrong thing.
#define sysVersion20    sysMakeROMVersion(2,0,0,sysROMStageRelease,0)
#define sysVersion35	sysMakeROMVersion(3,5,0,sysROMStageRelease,0)
#define sysVersion40	sysMakeROMVersion(4,0,0,sysROMStageDevelopment,0)
#define sysVersion50    sysMakeROMVersion(5,0,0,sysROMStageDevelopment,0)

// The hack's creator ID -- make sure you reserve one at the Palm Developer
// Zone.
#define MYCRID          'DBSC'

/* Numbers of pixels per line */
#define MAX_PIXELS 160
#define MAX_PIXELS_HIRES 320

// The struct that defines the app preferences for the hack.
typedef struct
{
   UInt16 strokestart, strokeend;
   UInt16 clipstart, clipend;
   UInt16 flags;
} MyPrefsType;
// flags
#define DBSC_FLAGS_JUMP     0x0001


/* Repeat types */
enum repeatTypes {
    repeatNone,
    repeatDaily,
    repeatWeekly,
    repeatMonthlyByDay,
    repeatMonthlyByDate,
    repeatYearly
};
typedef enum repeatTypes RepeatType;

/* Repeat info type (same as DateBook) */
typedef struct {
    RepeatType repeatType;            /* daily, weekly, monthlyByDay, etc. */
    DateType repeatEndDate;           /* minus one if forever */
    unsigned char repeatFrequency;    /* i.e. every 2 days */
    unsigned char repeatOn;           /* monthlyByDay and repeatWeekly only */
    unsigned char repeatStartOfWeek;  /* repeatWeekly only */
} RepeatInfoType;
typedef RepeatInfoType *RepeatInfoPtr;

typedef struct {
    UInt32 alarmSecs;
    UInt32 repeatSecs;
    unsigned short sketchLength; /* includes the 80-byte thumbnail +1 */
    unsigned short extraLength;  /* For future implementations */
    unsigned short flags;
    short priority;  /* zero-based */
    short repeatMode; /* This is authoritative over repeatInfo */
    RepeatInfoType repeatInfo;
} DiddleBugRecordType; /* 26 bytes */
typedef DiddleBugRecordType *DiddleBugRecordPtr;
#define sketchDataOffset (sizeof(DiddleBugRecordType))
#define sketchThumbnailSize 80
/* 25 bytes for the zero data, 50 bytes for thumbnail, 2 bytes for text nulls */
#define minRecordSize (sketchDataOffset+sketchThumbnailSize+25+2+1)
/* BaseRecordSize does not include sketchLength or txt strings or extraLength */
#define baseRecordSize (sketchDataOffset+2)

/* Defines for the record data flags */
#define RECORD_FLAG_ALARM_SET       0x0001
#define RECORD_FLAG_COUNTDOWN       0x0002
#define RECORD_FLAG_LOCKED          0x0004
#define RECORD_FLAG_DIRTY           0x0008 /* just applies to sketch data */
#define RECORD_FLAG_BUFDIRTY        0x0010
