#ifndef __diddlebug_h
#define __diddlebug_h

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

#include "menu_shortcuts.h"
#include "booger.h"

#include <PalmOS.h>
#include <System/SysEvtMgr.h>
#include <System/ExgMgr.h>


/*
** Define the minimum OS version we support (3.5), as
** well as other OS versions so we do the right thing everywhere
** Note that you should use < or >= in these comparisons, in nearly
** every case, because 3.0.1 should be treated the same way as 3.0.
** If you say "if vers <= 3.0" you'd often do the wrong thing.
*/
#define sysVersion20    sysMakeROMVersion(2,0,0,sysROMStageRelease,0)
#define sysVersion35	sysMakeROMVersion(3,5,0,sysROMStageRelease,0)
#define sysVersion40	sysMakeROMVersion(4,0,0,sysROMStageDevelopment,0)
#define sysVersion50    sysMakeROMVersion(5,0,0,sysROMStageDevelopment,0)

/* Application type identifier */
#ifndef AppType
#define AppType	'DIDB'
#endif

/*
** Preferences version - Incremented everytime pref format changes
** Version 1.0-1.4 = 1                                            
** Version 1.5     = 2                                            
** Version 1.6     = 3                                            
** Version 1.7-2.0 = 4                                            
** Version 2.1-2.5 = 5                                            
** Version 2.6-2.50beta4 = 6                                      
** Version 2.50beta5-2.50.b.21 = 7                                
** Version 2.50.b.22-2.90.b.3 = 8
** Version 2.90.b.4-up = 9
*/
#ifndef PrefVersion
#define PrefVersion 9
#endif

/*
** Extended preferences version - Incremented everytime pref format changes
** Version 2.51.d.1-2.52 = 1
** Version 2.53.d.1-up   = 2
*/
#ifndef ExtendedPrefVersion
#define ExtendedPrefVersion 2
#endif

/* Database type */
#ifndef DBType
#define DBType	'Data'
#endif

/* Database name */
#ifndef DBName
#define DBName "DiddleBugDB"
#endif

#ifndef DBNameHR
#define DBNameHR "DiddleBugHRDB"
#endif

/* Number of pixels allowed in a pen */
#ifndef NPENPIX
#define NPENPIX	64
#endif

/* Number of pen events which may be weighted together */
#ifndef NPENWTS
#define NPENWTS 32 /* must be power of two */
#endif

/* Hi-res multiplier */
#define HiResCoef 2

/* Numbers of pixels per line */
#define MAX_PIXELS 160
#define MAX_PIXELS_HIRES 320

/* Drawing modes */
typedef enum {
  dbModeDoodle,
  dbModeText,
  dbModeLine,
  dbModeArrow,
  dbModeShape,
  dbModeFloodFill,
  dbModeCut,
  dbModeCopy,
  dbModePaste
} DrawingModes;

/* Paint-mode selection */
typedef enum {
  dbmSelectionPaint,
  dbmSelectionSmear, /* not used anymore */
  dbmSelectionErase
} ModeSelection;

/* Filter selection */
typedef enum {
  dbFilterRough,
  dbFilterSmooth,
  dbFilterSmoother
} FilterSelection;

/* Pen selection */
typedef enum {
  dbPenFine,
  dbPenMedium,
  dbPenBold,
  dbPenBroad,
  dbPenFineItalic,
  dbPenMediumItalic,
  dbPenBroadItalic
} PenSelection;

/* Ink selection */
typedef enum {
  dbInkWhite,
  dbInk1_8,
  dbInk2_8,
  dbInk3_8,
  dbInk4_8,
  dbInk5_8,
  dbInk6_8,
  dbInk7_8,
  dbInkBlack,
  dbInkRandom
} InkSelection;

/* Text insertion modes */
typedef enum {
  dbTextBlack,
  dbTextBlackOnWhite,
  dbTextWhiteOnBlack,
  dbTextXOR
} TextInsertionMode;

/* Shape type codes */
typedef enum {
  dbShapeRectangle,
  dbShapeRoundedRectangle,
  dbShapeCircle,
  dbShapeOval
} ShapeTypes;

/* Shape fill codes */
typedef enum {
  dbsFillOutline,
  dbsFillFilled,
  dbsFillXOR,
  dbsFillOutlineXOR
} ShapeFillTypes;

/* Shortcut codes */
typedef enum {
  scLine,
  scRectangle,
  scRoundedRectangle,
  scOval,
  scCircle,
  scFill,
  scText
} ShortcutMenu;

/* Listview codes */
#define noRecordSelected   0xffff
#define noRecordSelectedID -1

/* Old Alarm Repeat modes */
#define REPEAT_NONE         0
#define REPEAT_HOURLY       1
#define REPEAT_DAILY        2
#define REPEAT_WEEKDAYS     3
#define REPEAT_WEEKLY       4
#define REPEAT_BIWEEKLY     5

/* Old Repeat bitfields */
#define REPEATBITS_WEEKDAYS 0x3e
#define REPEATBITS_SUN      0x01
#define REPEATBITS_MON      0x02
#define REPEATBITS_TUE      0x04
#define REPEATBITS_WED      0x08
#define REPEATBITS_THU      0x10
#define REPEATBITS_FRI      0x20
#define REPEATBITS_SAT      0x40

/* Misc. */
#define ALARM_STRING_LENGTH 32

/* Beaming */
#define BEAM_BUF_SIZE 512

/*
** Extract the pen pixel offsets from the penpex array bytes.
** Each nibble is treated as a 2's complement integer.
*/
#define dxFromPenpix(p)	(((int)(p>>4))-8)
#define dyFromPenpix(p)	(((int)(p&0xF))-8)

/*
** AlarmData structure - deprecated (18 bytes)
**                       Used only in versions 2.5 and earlier
*/
typedef struct {
  Boolean is_alarm_set;
  Boolean is_countdown;
  UInt32 alarm_secs;
  Int16 repeat_mode;
  UInt32 repeat_secs;
  Boolean is_locked;
  UInt32 reserved;
} DeprecatedAlarmType;
typedef DeprecatedAlarmType *DeprecatedAlarmPtr;

/*
** Structure for globals needed by xfer mode
*/
typedef struct {
  MemHandle pluglistH;
  Int16 num_plugs;
  Int16 plug_menu_width;
  Int16 flags;
  Boolean complete;
  Int16 status[4];	    /* Status of transfer details */
  Int16 choice_map[4];	    /* Mapping of selection number to "real" transfer index */
} GlobalXferType;
typedef GlobalXferType *GlobalXferPtr;

/* Repeat info type (similar to DateBook) */
typedef struct {
  RepeatType repeatType;    /* daily, weekly, monthlyByDay, etc. */
  DateType repeatEndDate;   /* minus one if forever */
  UInt8 repeatFrequency;    /* i.e. every 2 days */
  UInt8 repeatOn;           /* monthlyByDay and repeatWeekly only */
  UInt8 repeatStartOfWeek;  /* repeatWeekly only */
} RepeatInfoType;
typedef RepeatInfoType *RepeatInfoPtr;

/*
** Structure for extra alarm information
** specifying the location of the MIDI file to play
*/
typedef struct {
  UInt32 uniqueRecID;
  Int16 listIndex;
  LocalID dbID;
  UInt16 cardNo;
} AlarmSoundInfoType;
typedef AlarmSoundInfoType* AlarmSoundInfoPtr;

/*
** Record structure is: DiddleBugRecordType +
**                      Byte sketch[sketchLength] +
**                      Null terminated caption +
**                      Null terminated note +
**                      Byte extra[extraLength]
*/
typedef struct {
  UInt32 alarmSecs;
  UInt32 snoozeSecs;
  UInt16 sketchLength;  /* includes the 50-byte thumbnail +1 */
  UInt16 extraLength;   /* For future implementations */
  UInt16 flags;
  Int16 priority;  	/* zero-based */
  Int16 repeatMode; 	/* This has been made obsolete by v.2.50 */
  RepeatInfoType repeatInfo;
} DiddleBugRecordType; 	/* 26 bytes */
typedef DiddleBugRecordType *DiddleBugRecordPtr;
#define sketchDataOffset (sizeof(DiddleBugRecordType))
#define sketchThumbnailSize 80
/* 25 bytes for the zero data, 80 bytes for thumbnail, 2 bytes for text nulls */
/* BaseRecordSize does not include sketchLength or txt strings or extraLength */
#define baseRecordSize (sketchDataOffset+2)

/* Defines for the record data flags */
#define RECORD_FLAG_ALARM_SET       0x0001
#define RECORD_FLAG_COUNTDOWN       0x0002
#define RECORD_FLAG_LOCKED          0x0004
#define RECORD_FLAG_DIRTY           0x0008 /* just applies to sketch data */
#define RECORD_FLAG_BUFDIRTY        0x0010
#define RECORD_FLAG_LED             0x0020
#define RECORD_FLAG_VIBRATE         0x0040
#define RECORD_FLAG_CUSTOM_SIGNAL   0x0080
#define RECORD_FLAG_TO_REPEAT       0x0100
#define recordSetAlarm()        d.record.flags |= RECORD_FLAG_ALARM_SET
#define recordUnsetAlarm()      d.record.flags &= ~RECORD_FLAG_ALARM_SET
#define recordSetCountdown()    d.record.flags |= RECORD_FLAG_COUNTDOWN
#define recordUnsetCountdown()  d.record.flags &= ~RECORD_FLAG_COUNTDOWN
#define recordLock()            d.record.flags |= RECORD_FLAG_LOCKED
#define recordUnlock()          d.record.flags &= ~RECORD_FLAG_LOCKED
#define recordSetDirty()        d.record.flags |= RECORD_FLAG_DIRTY
#define recordUnsetDirty()      d.record.flags &= ~RECORD_FLAG_DIRTY
#define recordSetBufDirty()     d.record.flags |= RECORD_FLAG_BUFDIRTY
#define recordUnsetBufDirty()   d.record.flags &= ~RECORD_FLAG_BUFDIRTY
#define recordSetLED()          d.record.flags |= RECORD_FLAG_LED
#define recordUnsetLED()        d.record.flags &= ~RECORD_FLAG_LED
#define recordSetVibrate()      d.record.flags |= RECORD_FLAG_VIBRATE
#define recordUnsetVibrate()    d.record.flags &= ~RECORD_FLAG_VIBRATE
#define recordSetCustomSignal() d.record.flags |= RECORD_FLAG_CUSTOM_SIGNAL
#define recordUnsetCustomSignal() d.record.flags &= ~RECORD_FLAG_CUSTOM_SIGNAL
#define recordIsAlarmSet        ((d.record.flags)&RECORD_FLAG_ALARM_SET)
#define recordIsCountdown       ((d.record.flags)&RECORD_FLAG_COUNTDOWN)
#define recordIsLocked          ((d.record.flags)&RECORD_FLAG_LOCKED)
#define recordIsDirty           ((d.record.flags)&RECORD_FLAG_DIRTY)
#define recordIsBufDirty        ((d.record.flags)&RECORD_FLAG_BUFDIRTY)
#define recordIsLEDSet          ((d.record.flags)&RECORD_FLAG_LED)
#define recordIsVibrateSet      ((d.record.flags)&RECORD_FLAG_VIBRATE)
#define recordIsCustomSignalSet ((d.record.flags)&RECORD_FLAG_CUSTOM_SIGNAL)

/*
** Adjust offset for HandEra 330 if necessary
**
** Floating point operations do not work for code not in segment 0
** (actually, that's not true anymore - but we will avoid
** floating ops altogether for performance reasons)
*/
#define handeraD(A)             ((A) * 3 / 2)
#define handera(A)		(d.handera ? ((A) * 3 / 2) : (A))

/*
** Drop List info
*/
typedef struct {
  Int16 dropListWidth[4];
  UInt16 type;
  UInt16 iconRsc;
} DropListInfoType;
typedef DropListInfoType *DropListInfoPtr;
#define DROPLIST_TYPE_BITMAP 0
#define DROPLIST_TYPE_PEN    1
#define DROPLIST_TYPE_INK    2

/*
** Global data structure.
*/
typedef struct {
  DmOpenRef dbR;		  /* database MemHandle for scratch buffer */
  WinHandle winM;		  /* main window */
  WinHandle winbufM; 		  /* main buffer */
  WinHandle realCanvas;	          /* Offscreen window with depth 1 */
  UInt16 formID;     		  /* Current formID (including index) */
  struct wts {
    UInt16 x, y;    	          /* incoming pen coordinates */
  } wts[NPENWTS];
  UInt16 nwts;           	  /* pen event coordinates */
  UInt16 wx, wy;         	  /* sum of pwt pen coordinates */
  UInt16 ox, oy;          	  /* last synthesized pen coordinates */
  DrawingModes drawmode;          /* dbModeDoodle, dbModeText, dbModeShape */
  DrawingModes last_insert;       /* Memory of d.drawmode for Last Insert */
  Int16 lock_count;               /* Number of unsuccesful writes to locked */
  Char InsertionText[32];         /* Text buffer for the "Insert Text" mode */
  WinHandle TextWin;              /* Offscreen window holding insertion text */
  FontID txt_oldfont;             /* Old text font to restore after insert */
  PointType anchor;               /* Anchor used for line, arrow, and shapes */
  DiddleBugRecordType record;     /* Buffer for the record data */
  Char CurrentTimeStr[20];        /* String with the current weekday and time */
  DateTimeType frm_date;          /* Current date selected in the Date form */
  Char date_str[15];              /* Label for the Date selector trigger */
  Int32 nextPeriod;               /* tick count until the next check */
  UInt16 cardNo;                  /* cardNo of the application */
  LocalID dbID;                   /* dbID of the application prc file */
  TimeFormatType shorttime;       /* Time format for titlebar */
  Int16 shortcut_width;           /* Width of the shortcut popup list */
  DropListInfoType dropList;      /* Info for the drop lists */
  Int16 alarm_select_width;       /* Width of popup alarm selection list */
  MemHandle happlist;             /* List of system applications */
  SysDBListItemType *papplist;    /* List of system applications (locked) */
  UInt16 appcount;                /* Number of system applications */
  Boolean is_xfer_mode;           /* transfer mode */
  Boolean alarm_deleted_current;  /* alarm deleted current sketch */
  Boolean alarm_deleted_any;      /* alarm deleted any sketch */
  AlarmSoundInfoType record_sound;/* MIDI information for current record */
  MemHandle record_note;          /* String containing the (opt.) note for the current record */
  MemHandle record_name;          /* String containing the (opt.) label for the current record */
  Boolean record_private;         /* Is the record a private one? */
  MemHandle midi_list;            /* List of MIDI files */
  UInt16 midi_count;              /* Count of MIDI files */
  Char midi_label[32];            /* Label for MIDI popup trigger */
  Char midi_label2[32];           /* Label for 2nd MIDI popup trigger */
  Boolean undo_mark;              /* checkpoint marker for undo */
  Boolean linker_available;       /* Is Rick Brams Linker active? */
  GlobalXferType xfer;            /* Holds global transfer data */
  const UInt8* penpixList[7];     /* List of pens */
  const CustomPatternType* inkList[9]; /* List of inks */
  Boolean erase_menu;		  /* Erase the command bar in the next "key action"? */
  PointType pbPoint;		  /* Original coordinates of PageButton enter-event */
  Char bpText[8];		  /* String buffer for the PageButton label */
  Char hwText[4][64];		  /* String buffer for the hardware button selector trigger */
  Char time_minutes[5][3];        /* List of minute strings for the time form */
  Char* time_minutesP[5];         /* Pointers to strings for the **** LstSetListChoices function */
  UInt32 pref_hardware_action[4]; /* Buffer for pref.hardware_action */
  Int16 tmp_pen;		  /* Buffer for pref.penSelection */
  Int16 tmp_ink;		  /* Buffer for pref.inkSelection */
  struct screenInfo {
    UInt32 width, height, depth;
    Boolean color;
  } oldScreen;		          /* The state of the screen before running DiddleBug */
  RepeatType repeat_event_type;   /* Selected repeat type */
  DateType repeat_end_date;       /* End date for repeating alarms */
  UInt16 repeat_start_of_week;    /* Which day does the week start on? */
  RepeatInfoType tmp_repeat;      /* Buffer for the repeat details dialog */
  Char alarmText[sndMidiNameLength + 5]; /* String buffer for alarm signal trigger */
  AlarmSoundInfoType tmp_signal;  /* Buffer for the custom signal dialog */
  Boolean tmp_led;                /* Buffer for the custom signal dialog */
  Boolean tmp_vibrate;            /* Buffer for the custom signal dialog */
  Boolean tmp_custom_signal;      /* Buffer for the custom signal dialog */
  FontID note_font;               /* Font for the note view */
  Boolean in_phone_lookup;        /* Flag for note view */
  UInt16 tmp_category;            /* Temporary category for details dialog */
  Char tmpCategoryName[dmCategoryLength]; /* Buffer for temporary category name */
  Boolean detailsWithSketch;      /* withSketch "parameter" for details dialog */
  MemHandle tmp_name;             /* Temporary name for details dialog */
  Char categoryName[dmCategoryLength]; /* Buffer for category name */
  UInt16 top_row_pos_in_cat;      /* Position in category of topmost record/row */
  privateRecordViewEnum	privateRecordStatus; /* Show/hide private records */
  Boolean unmaskedCurrentRecord;  /* Has the user unmasked the current record? */
  UInt16 records_in_cat;          /* How many records are in the category? */
  UInt16 top_visible_record;      /* Topmost record visible in list view */
  UInt16* pos_to_index;           /* Map category position to index for DoPageList() */
  MemHandle repeatTriggerHandle;  /* Handle of label string for TimeRepeatTrig */
  PointType title_enter_pos;      /* Position of last frmTitleEnterEvent */
  Boolean drawing;                /* Are we currently drawing? */

  /* Hardware support... */
  Boolean handera;		  /* Device is a HandEra 330 */
  Boolean sonyClie;               /* Device is a Sony Clie */
  UInt16 sonyHRRefNum;            /* References number of Sony hi-res library */
  Boolean sonyLoadedHRLib;        /* Did we load the hi-res library ourselves? */
  Boolean hires;                  /* Device supports 320 x 320 pixels */
  Boolean acer;                   /* Acer s50 and s60 need special handling... */
  Boolean treo600;                /* ...the Treo 600 smartphone as well... */
  Boolean treo650;                /* ...the Treo 650 smartphone as well... */
  Boolean treo680;                /* ...the Treo 680 smartphone as well... */
  Boolean palmOne;                /* ...even the Tungsten and Zire device do */

  /* ...and different OS versions */
  Boolean os4;                    /* We are running on at least PalmOS 4 */
  Boolean os5;                    /* We are running on a new ARM-based device */

  /* Hi-res support */
  RectangleType r_hires;
  BitmapType* winbufBmp;
  BitmapType* realCanvasBmp;

  /* Color preferences */
  IndexedColorType tmpInkColor;   /* Foreground color */
  IndexedColorType tmpPaperColor; /* Background color */
  
  /* Check if it is safe to draw */
  Boolean doNotUpdateFormTitle;   /* Can we safely update the DiddleTForm title? */

  /* 5-way (and Jog Dial) navigation mode for listviews */
  Boolean fiveWayNavigation;      /* Should we move through the sketches one-by-one? */
  UInt16 thumbnailX;              /* x coordinate of selection in thumbnail view */
  UInt16 thumbnailY;              /* y coordinate of selection in thumbnail view */
  UInt16 lastVisibleThumbnail;    /* What's the last visible thumbnail? */

  /* Clipboard */
  WinHandle clipboard;
  BitmapType* clipboardBmp;

  /* Out-of-memory handling */
  Boolean exiting;
} data;
typedef data* DataPtr;

/*
** Maximum number of actions for hardware buttons.
*/
#define HW_MAX_ACTIONS 20

/*
** Currently defined actions for hardware buttons.
*/
typedef enum {
  hwNoAction,
  hwNewRecord,
  hwNextRecord,
  hwNextCategory,
  hwListView,
  hwDetails,
  /* insert new actions here */
  hwLastAction
} HardwareActionType; /* Should be limited to HW_MAX_ACTIONS */

/*
** Preferences data structure
*/
typedef struct {
  Int16 version;                   /* version number */
  Int16 dbI;                       /* page record for scratch buffer */
  UInt16 formID;                   /* titled or untitled or list */
  UInt32 flags;                    /* Boolean flags */
  RectangleType r;                 /* current drawing window */
  Int16 modeSelection;             /* Selection for the mode */
  Int16 filtSelection;             /* Selection for the filter */
  Int16 penSelection;              /* Selection for the pen */
  Int16 erasePenSelection;         /* Selection for the erase pen */
  Int16 inkSelection;              /* Selection for the ink */
  UInt16 filt;                     /* pen event weighting */
  UInt8 penpix[NPENPIX];           /* pen coordinate offsets */
  CustomPatternType inkpat;        /* current ink pattern */
  FontID txt_font;                 /* Font for insertion of text */
  Int16 txt_mode;                  /* Text insertion mode */
  Int16 shape_type;                /* shape mode: circle, rectangle, etc. */
  Int16 shape_fill;                /* Whether shapes are filled or not */
  UInt16 category;                 /* Current category */
  UInt32 xfer_current_plug;        /* The currently selected xfer plugin */
  UInt32 hardware_action[4];	   /* Action to take on hardware buttons */
  UInt8 min_swipe;                 /* Amount of pixels needed to activate "swiping" buttons */
  Int16 shape_ink;                 /* Ink selection for Insert Shape form */
  Int16 shape_pen;                 /* Pen selection for Insert Shape form */
  AlarmSoundInfoType std_alarm;    /* The default alarm sound */
  AlarmSoundInfoType remind_alarm; /* The alarm sound for "Remind Me" */
  UInt8 alarm_repeat_times;        /* Repeat normal alarm n times */
  UInt8 alarm_auto_snooze_delay;   /* Try again in n minutes */
  UInt8 alarm_auto_snooze_times;   /* Try again n times */
} pref;

/* Pref flags */
#define PFLAGS_SWITCH_TITLE_ACTION    0x00000001L
#define PFLAGS_CONFIRM_CLEAR          0x00000002L
#define PFLAGS_CONFIRM_DELETE         0x00000004L
#define PFLAGS_BIG_OK                 0x00000008L
#define PFLAGS_XFER_GOTO              0x00000010L
#define PFLAGS_XFER_DELETE            0x00000020L
#define PFLAGS_XFER_BACKLINK          0x00000040L
#define PFLAGS_KEEP_DRAW_TOOL	      0x00000080L
#define PFLAGS_INK_ARTIST	      0x00000100L
#define PFLAGS_CHANGE_DEPTH           0x00000200L
#define PFLAGS_DEFAULT_LED            0x00000400L
#define PFLAGS_DEFAULT_VIBRATE        0x00000800L
#define PFLAGS_OPEN_NEW_RECORD        0x00001000L
#define PFLAGS_WITH_TITLEBAR          0x00002000L
#define PFLAGS_CLOCK_ENABLED          0x00004000L
#define PFLAGS_CREATE_LAST_HARDBUTTON 0x00008000L
#define PFLAGS_AUTO_DATETIME_STAMP    0x00010000L
#define PFLAGS_KEEP_CATEGORY          0x00020000L
#define PFLAGS_WITHOUT_TOOLBAR        0x00040000L
#define PFLAGS_CREATE_LAST_ICON       0x00080000L
#define PFLAGS_NEVER_OVERWRITE_TITLE  0x00100000L

/*
** Extended preferences data structure
** (to be backwards compatible)
*/
typedef struct {
  UInt16 version;                  /* version number */
  Int16 eraseFiltSelection;        /* Selection for the erase filter */
  UInt16 listFormID;               /* ID of the most recently used list view form */
  Int16 defaultPriority;           /* Must be between 0 and 4 */
  IndexedColorType inkColor;       /* Foreground color */
  IndexedColorType paperColor;     /* Background color */

  UInt32 reserved[19];             /* Reserved for future expansion */
} extended_pref;

/*
** Constants for the various preference records
*/
typedef enum {
  dbsNormalAppPrefs,
  dbsExtendedAppPrefs
} DiddleBugSavedPrefs;

typedef enum {
  dbuAlarmQueue
} DiddleBugUnsavedPrefs;

/*
** Structure for DiddleBug Records - deprecated
*/
/*
typedef struct {
    char data[3200];
    Boolean is_alarm_set;
    Boolean is_countdown;
    UInt32 alarm_secs;
    Int16 repeat_mode;
    UInt32 repeat_secs;
    Boolean is_locked;
    UInt32 reserved;
} DiddleBugType;
typedef DiddleBugType *DiddleBugPtr;
*/

/* Shortcut menu locations */
typedef enum {
  scHigh,
  scLow
} ShortCutLocationType;

/* Postpone menu choices */
typedef enum {
  ppToday,
  ppTomorrow,
  ppDayAfterTomorrow,
  ppThisFriday,
  ppNextMonday,
  ppNextWeek,
  ppNextMonth,
  ppPickDate
} PostPoneChoicesType;

/* Possible fields for "Find" */
typedef enum {
  ffName,
  ffNote,
  ffDetails,
  ffAlarm
} FindFieldsType;

/* Auto snooze times */
typedef enum {
  asNever,
  asOnce,
  asTwice,
  asThrice,
  as10Times,
  as100Times
} AutoSnoozeTimesType;

/* Alarm sound times */
typedef enum {
  alOnce,
  alTwice,
  alThrice,
  al5Times,
  al10Times
} AlarmSoundTimesType;

/* Auto snooze delay times */
typedef enum {
  ad1Minute,
  ad5Minutes,
  ad10Minutes,
  ad30Minutes
} AutoSnoozeDelayType;

/* Custom events */
typedef enum {
  dbOpenRecordFieldEvent = firstUserEvent,
  dbNonKeyCommandEvent
} DiddleBugEvents;

/* Commands that have no keycode assigned */
typedef enum {
  nkCmdHardwareNew,
  nkCmdNextCategory  
} NonKeyCommandCodeType;

/*
** Form update codes - used to determine how things should be redrawn.
*/
typedef enum {
  fUpdateRedrawAll =          0x0000,
  fUpdateCategoryChanged =    0x0001,
  fUpdateDisplayOptsChanged = 0x0002,
  fUpdateFontChanged =        0x0004,
  fUpdateDeletedCurrent =     0x0008,
  fUpdateDeletedAny =         0x0010,
  fUpdateEraseToolbar =       0x0020,
  fUpdateReloadRecords =      0x0040
} FormUpdateCodesType;

/*
** Error codes.
*/
typedef enum {
  dbErrCorruptedSketch = appErrorClass
} DiddleBugErrorCodesType;

#endif /* __diddlebug_h */
