/* booger.h */
#ifndef __BOOGER_H_
#define __BOOGER_H_

#include <PalmOS.h>

/* Launch command to run booger plugin as a subroutine */
#define boogerPlugLaunchCmdBlowNose 0x9101
/* Launch command to run booger plugin as an application */
#define boogerPlugLaunchCmdSneeze   0x9102
/* Launch command to launch the launcher */
#define boogerPlugLaunchCmdLaunchLauncher 0x9201

/* Error codes */
#define boogerErrorVersionMismatch 7000

// Booger resources
#define boogerID_button         6501
#define boogerID_lilbutton      6502
#define boogerID_plugname       6503
#define boogerID_gotobehavior   6504
#define boogerID_completebehavior 6505

// Version identifiers
#define IBVERSION_ORIG          0x0001
#define IBVERSION_PICTURE       0x0002
#define IBVERSION_FLAGS         0x0004

/* Flags */
#define IBFLAG_HIRES            0x0001

/* Structure for the BoogerType returned to DiddleBug */
typedef struct {
    UInt16 cardNo;            /* cardNo */
    LocalID dbID;             /* dbID */
    UInt16 cmd;               /* Launch flags */
    MemPtr cmdPBP;            /* usually a GoTo params */
} BoogerType;
typedef BoogerType *BoogerPtr;

#define apptNoEndDate           0xffff  // end date of appts that repeat forever

/* Repeat types */
enum repeatTypes {
    repeatNone,
    repeatHourly,
    repeatDaily,
    repeatWeekly,
    repeatMonthlyByDay,
    repeatMonthlyByDate,
    repeatYearly
};
typedef enum repeatTypes RepeatType;

/* Repeat info type (DateBook version) */
typedef struct {
  RepeatType repeatType;   // daily, weekly
  UInt8 reserved1;
  DateType repeatEndDate;  // minus one if forever
  UInt8 repeatFrequency;   // i.e. every 2 days if repeatType daily  
  UInt8 repeatOn;          // monthlyByDay and repeatWeekly
  UInt8 repeatStartOfWeek; // repeatWeekly only
  UInt8 reserved2;
} DtbkRepeatInfoType;
typedef DtbkRepeatInfoType * DtbkRepeatInfoPtr;

/* Structure for cmdPBP of boogerPlugLaunchCmdBlowNose */
/* DiddleBug is responsible for freeing the pointers upon return */
typedef struct {
  Char* note;              /* Note attached to the drawing (if any) */
  UInt16 flags;            /* Boolean flags (like "hires") */
  UInt16 sketchRecordNum;  /* record number of the sketch */
  UInt16 version;          /* version of this launch param */
  UInt16 data_size;        /* Size of sketch data */
  MemPtr data;             /* Sketch data */
  Char* text;              /* Text as typed in the pick field */
  Char* title;             /* Title of the drawing (if any) */
  Char* category;          /* Category to use (if any) */
  UInt32 alarm_secs;       /* Alarm seconds (zero if no alarm) */
  DtbkRepeatInfoPtr repeat;/* Repeat info (in DateBook format) */
  Int16 priority;          /* Reminder priority (default = 1) */
  BoogerType booger;       /* Filled by the BPlug to be read by DiddleBug */
  Int16 is_complete;       /* complete flag */
} KleenexType;
typedef KleenexType *KleenexPtr;

#endif /* __BOOGER_H_ */
