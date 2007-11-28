/* dbscHack.h

   Copyright © 2002 Peter Putzer
   Copyright © 1999 Mitch Blevins
   Licensed under the GNU GPL - http://www.gnu.org/copyleft/gpl.html
   Based on LinkHistoryHack by Ben Darnel <bgdarnel@unity.ncsu.edu>
   which was...
   Based on SampleHack by Roger Chaplin <foursquaredev@att.net>
*/


// The hack's creator ID -- make sure you reserve one at the Palm Developer
// Zone.
#define MYCRID          'DBSC'

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
