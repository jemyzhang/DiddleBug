/*

Copyright (c) 2002, 2003, 2004 Peter Putzer <pputzer@users.sourceforge.net>.
Licensed under the GNU General Public License (GPL).

Portions copyright (c) 2000 Palm, Inc. or its subsidiaries.  All rights reserved.

*/

/* Standard PalmOS headers */
#include <PalmOS.h>
#include <AlarmMgr.h>
#include <FeatureMgr.h>
#include <PalmOSGlue.h>

/* Project specific headers */
#include "diddlebug.h"
#include "diddlebugExtern.h"
#include "diddlebugInlines.h"
#include "diddlebugFeatures.h"
#include "globals.h"
#include "debug.h"
#include "diddlebugRsc.h"
#include "repeat.h"
#include "dynamic_buttons.h"
#include "time.h"
#include "alarm.h"

/* Handspring/palmOne Treo 600 */
#include <HsChars.h>
#include <HsExt.h>

/***********************************************************************
 *
 *	Internal Constants
 *
 ***********************************************************************/
#define apptEndOfTime	0xffffffff

// Error conditions
#define diddlebugErrDuplicateAlarm 1   // Used only with AddPendingAlarm
#define diddlebugErrAlarmListFull  2   // Used only with AddPendingAlarm

// Version
#define UnsavedPrefsVersion      1

/***********************************************************************
 *
 *	Local prototypes
 *
 ***********************************************************************/

static Err AddPendingAlarm(PendingAlarmQueueType* inAlarmInternalsP, UInt16 recordNum,
			   UInt32 alarmTime, UInt16 inBaseIndex);
static UInt16 DoAlarmDialog(pref*, PendingAlarmType*, Int16*, Boolean);
static void ReserveAlarmInternals(PendingAlarmQueueType* outAlarmInternalsP);
static void ReleaseAlarmInternals(PendingAlarmQueueType* inAlarmInternalsP, Boolean inModified);
static UInt16 GetPendingAlarmCount(PendingAlarmQueueType* inAlarmInternalsP);
static void SetPendingAlarmCount(PendingAlarmQueueType* inAlarmInternalsP, UInt16 inCount);
static void InsertPendingAlarm(PendingAlarmQueueType* inAlarmInternalsP, UInt16 inAlarmIndex,
			       PendingAlarmType* inPendingInfo);
static void LoadRecordSound(DmOpenRef dbR, AlarmSoundInfoType* record_sound,
			    DiddleBugRecordType* rec, UInt16 record_index);
static void DoSpecialEffects(pref* prefs, UInt16 recordFlags, Boolean treo600);
static Boolean ApptNextRepeat(DiddleBugRecordPtr apptRec, DateTimePtr dateP);
static Boolean ClockGadgetEvent(FormGadgetTypeInCallback* gadgetP, UInt16 cmd, void* paramP);

/***********************************************************************
 *
 * FUNCTION:    GetPendingAlarm
 *
 * DESCRIPTION: Retrieves an alarm from the pending alarm queue
 *		THIS MAY BE CALLED AT INTERRUPT LEVEL (inInterrupt == true)!
 *		IF SO, DONT USE GLOBALS!!
 *
 * PARAMETERS:  inAlarmIndex -- The position of the alarm to be removed
 *		inInterrupt  -- If true, use FeatureMgr to retrieve
 *				application globals.
 *
 * RETURNED:    outPendingInfo	The pending alarm
 *
 ***********************************************************************/
extern inline void GetPendingAlarm(PendingAlarmQueueType* inAlarmInternalsP,
				   UInt16 inAlarmIndex, PendingAlarmType* outPendingInfoP)
{
  *outPendingInfoP = inAlarmInternalsP->pendingAlarm [inAlarmIndex];
}


/***********************************************************************
 *
 * FUNCTION:    SetPendingAlarm
 *
 * DESCRIPTION: Sets an alarm in the pending alarm queue. Assumes that
 *		inAlarmIndex is within range.
 *		THIS MAY BE CALLED AT INTERRUPT LEVEL (inInterrupt == true)!
 *		IF SO, DONT USE GLOBALS!!
 *
 * PARAMETERS:  inAlarmIndex  -- the position of the alarm to be removed
 *		inPendingIndo -- the pending alarm
 *		inInterrupt   -- If true, use FeatureMgr to retrieve
 *				 application globals.
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
extern inline void SetPendingAlarm(PendingAlarmQueueType* inAlarmInternalsP,
				   UInt16 inAlarmIndex, PendingAlarmType* inPendingInfoP)
{
  inAlarmInternalsP->pendingAlarm [inAlarmIndex] = *inPendingInfoP;
}

/***********************************************************************
 *
 * FUNCTION:    RemovePendingAlarm
 *
 * DESCRIPTION: Removes an alarm from the pending alarm queue
 *		THIS MAY BE CALLED AT INTERRUPT LEVEL (inInterrupt == true)!
 *		IF SO, DONT USE GLOBALS!!
 *
 * PARAMETERS:  inAlarmIndex -- The position of the alarm to be removed
 *		inInterrupt  -- If true, use FeatureMgr to retrieve
 *				application globals.
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
extern inline void RemovePendingAlarm(PendingAlarmQueueType* inAlarmInternalsP,
				      UInt16 inAlarmIndex)
{
  MemMove (&inAlarmInternalsP->pendingAlarm [inAlarmIndex],
	   &inAlarmInternalsP->pendingAlarm [inAlarmIndex + 1],
	   (inAlarmInternalsP->pendingCount - inAlarmIndex - 1) * sizeof (PendingAlarmType) );
  --(inAlarmInternalsP->pendingCount);
}


/***********************************************************************
 *
 * FUNCTION:    ClearPendingAlarms
 *
 * DESCRIPTION:
 *
 * PARAMETERS:  nothing
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
extern inline void ClearPendingAlarms(PendingAlarmQueueType* inAlarmInternalsP) {
  inAlarmInternalsP->pendingCount = 0;
}


/***********************************************************************
 *
 * FUNCTION:    GetDismissedAlarmCount
 *
 * DESCRIPTION: Returns size of dismissed alarms queue.
 *		THIS MAY BE CALLED AT INTERRUPT LEVEL (inInterrupt == true)!
 *		IF SO, DONT USE GLOBALS!!
 *
 * PARAMETERS:  inInterrupt -- If true, use FeatureMgr to retrieve
 *			       application globals.
 *
 * RETURNED:    Number of items in dismissed alarms queue
 *
 ***********************************************************************/
extern inline UInt16 GetDismissedAlarmCount(PendingAlarmQueueType* inAlarmInternalsP) {
  return inAlarmInternalsP->dismissedCount;
}


/***********************************************************************
 *
 * FUNCTION:    GetDismissedAlarmList
 *
 * DESCRIPTION: Returns list of dismissed alarms.
 *		THIS MAY BE CALLED AT INTERRUPT LEVEL (inInterrupt == true)!
 *		IF SO, DONT USE GLOBALS!!
 *
 * PARAMETERS:  inInterrupt -- If true, use FeatureMgr to retrieve
 *			       application globals.
 *
 * RETURNED:    Number of items in dismissed alarms queue
 *
 ***********************************************************************/
extern inline UInt32* GetDismissedAlarmList(PendingAlarmQueueType* inAlarmInternalsP) {
  return inAlarmInternalsP->dismissedAlarm;
}


/***********************************************************************
 *
 * FUNCTION:    AppendDismissedAlarm
 *
 * DESCRIPTION: Adds an alarm onto the dismissed alarm queue.
 *		THIS MAY BE CALLED AT INTERRUPT LEVEL (inInterrupt == true)!
 *		IF SO, DONT USE GLOBALS!!
 *
 * PARAMETERS:  inAlarmIndex -- The position at which to insert the alarm
 *		inInterrupt  -- If true, use FeatureMgr to retrieve
 *				application globals.
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
extern inline void AppendDismissedAlarm(PendingAlarmQueueType* inAlarmInternalsP,
					UInt32 inUniqueID)
{
  inAlarmInternalsP->dismissedAlarm [inAlarmInternalsP->dismissedCount] = inUniqueID;
  ++(inAlarmInternalsP->dismissedCount);
}

/***********************************************************************
 *
 * FUNCTION:    GetPendingAlarmCount
 *
 * DESCRIPTION: Returns size of pending alarms queue.
 *		THIS MAY BE CALLED AT INTERRUPT LEVEL (inInterrupt == true)!
 *		IF SO, DONT USE GLOBALS!!
 *
 * PARAMETERS:  inInterrupt -- If true, use FeatureMgr to retrieve
 *			       application globals.
 *
 * RETURNED:    Number of items in pending alarms queue
 *
 ***********************************************************************/
extern inline UInt16 GetPendingAlarmCount(PendingAlarmQueueType* inAlarmInternalsP) {
  return inAlarmInternalsP->pendingCount;
}

/***********************************************************************
 *
 * FUNCTION:    ClearDismissedAlarms
 *
 * DESCRIPTION:
 *
 * PARAMETERS:  nothing
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
extern inline void ClearDismissedAlarms (PendingAlarmQueueType* inAlarmInternalsP) {
  inAlarmInternalsP->dismissedCount = 0;
}

/***********************************************************************
 *
 * FUNCTION:    AlarmGetTrigger
 *
 * DESCRIPTION: This routine gets the time of the next scheduled
 *              alarm from the Alarm Manager.
 *
 * PARAMETERS:  nothing
 *
 * RETURNED:    the time of the next scheduled alarm, or zero if
 *              no alarm is scheduled.
 *
 ***********************************************************************/
UInt32 AlarmGetTrigger(UInt32* refP) {
  struct StackFrame {
    UInt16 cardNo;
    LocalID dbID;
    DmSearchStateType searchInfo;
  } *s;
  UInt32 alarmTime = 0;

  /* Reserve & initialize virtual stack frame */
  s = MemPtrNew(sizeof(struct StackFrame));
  ASSERT(s);

  DmGetNextDatabaseByTypeCreator(true, &s->searchInfo,
				 sysFileTApplication, AppType, true, &s->cardNo, &s->dbID);

  alarmTime = AlmGetAlarm(s->cardNo, s->dbID, refP);

  /* Clean up virtual stack frame */
  MemPtrFree(s);

  return alarmTime;
}


/***********************************************************************
 *
 * FUNCTION:    AlarmSetTrigger
 *
 * DESCRIPTION: This routine set the time of the next scheduled
 *              alarm.
 *
 * PARAMETERS:  the time of the next scheduled alarm, or zero if
 *              no alarm is scheduled.
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
void AlarmSetTrigger(UInt32 alarmTime, UInt32 ref) {
  struct StackFrame {
    UInt16 cardNo;
    LocalID dbID;
    DmSearchStateType searchInfo;
  } *s;

  /* Reserve & initialize virtual stack frame */
  s = MemPtrNew(sizeof(struct StackFrame));
  ASSERT(s);

  DmGetNextDatabaseByTypeCreator(true, &s->searchInfo,
				 sysFileTApplication, AppType, true, &s->cardNo, &s->dbID);

  AlmSetAlarm(s->cardNo, s->dbID, ref, alarmTime, true);

  /* Clean up virtual stack frame */
  MemPtrFree(s);
}

/***********************************************************************
 *
 * FUNCTION:    AlarmInit
 *
 * DESCRIPTION: Initialize DateAlarm internals for use at interrupt time.
 *
 * PARAMETERS:  nothing
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
void AlarmInit(void) {
  PendingAlarmQueueType* alarmInternals;

  /* Reserve queue */
  alarmInternals = MemPtrNew(sizeof(PendingAlarmQueueType));
  ASSERT(alarmInternals);
  ReserveAlarmInternals(alarmInternals);

  /* Clean up */
  ReleaseAlarmInternals(alarmInternals, true);
  MemPtrFree(alarmInternals);
}

/*
** Update the clock string and redraw the clock label.
**
** frm - the current form
** idx - the object index of the clock label
** show - show the object?
*/
static void UpdateClock(FormType* frm, UInt16 idx, Boolean show) {
  DateTimeType time;
  DynamicButtonType* btn = (DynamicButtonType*) FrmGetGadgetData(frm, idx);

  TimSecondsToDateTime(TimGetSeconds(), &time);
  TimeToAscii(time.hour, time.minute, PrefGetPreference(prefTimeFormat), btn->content.text);

  if (show)
    FrmShowObject(frm, idx);
}

/***********************************************************************
 *
 * FUNCTION:    ShowAlarmHandleEvent
 *
 * DESCRIPTION: This routine is the event handler for the alarm dialog
 *              of the Diddlebug application.
 *
 * PARAMETERS:  event  - a pointer to an EventType structure
 *
 * RETURNED:    true if the event has handle and should not be passed
 *              to a higher level handler.
 *
 *
 ***********************************************************************/
static Boolean ShowAlarmHandleEvent(EventType* event) {
  FormPtr frm = FrmGetActiveForm();
  const UInt16 idx = FrmGetObjectIndex(frm, AlarmClockGadget);

  // We don't want to allow an other application to be launched
  // while an alarm is displayed, so we intercept appStop events
  // (except when running under simulation). All other events are
  // handled by FrmDoDialog.
  if (event->eType == appStopEvent)
    {
      return true;
    }

  else if (event->eType == keyDownEvent)
    {
      if (!TxtCharIsHardKey(event->data.keyDown.modifiers, event->data.keyDown.chr)
	  && EvtKeydownIsVirtual(event))
	{
	  if (event->data.keyDown.chr == vchrPageUp) {
	    // disable 'up' scroll arrow from ticking while our alarm dialog is up.
	    return true;
	  } else if (event->data.keyDown.chr == keyboardAlphaChr) {
	    pref *prefs = NULL, *pp;
	    UInt16 prefsSize;
	    UInt32 dummy;

	    if (FtrGet(AppType, DB_FEATURE_GLOBALS_AVAILABLE, &dummy) != ftrErrNoSuchFeature)
	      pp = &p;
	    else {
	      /* Reserve space for preferences */
	      prefs = MemPtrNew(sizeof(pref));
	      ASSERT(prefs);
	      GetPreferences(prefs, &prefsSize, NULL, false);
	      pp = prefs;
	    }

	    if (FrmGlueGetObjectUsable(frm, idx)) {
	      FrmHideObject(frm, idx);
	      pp->flags &= ~PFLAGS_CLOCK_ENABLED;

	      /* Update sketch */
	      DynBtnDraw((DynamicButtonType*) FrmGetGadgetData(frm, FrmGetObjectIndex(frm, AlarmBigOkButton)));
	    } else {
	      FrmShowObject(frm, idx);
	      pp->flags |= PFLAGS_CLOCK_ENABLED;
	    }

	    PrefSetAppPreferences(AppType, dbsNormalAppPrefs, 1, pp, sizeof(pref), true);

	    /* Clean up */
	    if (prefs)
	      MemPtrFree(prefs);

	    return true;
	  }
	}
    }

  else if (event->eType == frmUpdateEvent) {
    FrmDrawForm (frm);
  }

  {
    const UInt32 ticks = TimGetTicks ();
    const UInt32 seconds = TimGetSeconds();
    const UInt32 minutes = seconds / minutesInSeconds;
    const UInt32 targetSeconds = (minutes + 1) * minutesInSeconds;
    const UInt32 delayInSeconds = targetSeconds - seconds;
    const UInt32 targetTicks = ticks + delayInSeconds * sysTicksPerSecond;

    EvtSetNullEventTick (targetTicks);

    /* update clock */
    if (FrmGlueGetObjectUsable(frm, idx))
      UpdateClock(frm, idx, true);
  }

  return false;
}

/*
** Helper function to allow taps and key presses to interrupt the alarm played
** by PlayAlarmSound.
*/
static Boolean AlarmSoundBlockingFunction (void *channelP, UInt32 userData, Int32 time) {
  EventType e;
  UInt32 interrupted = 0;
  Boolean res = true;
  
  EvtGetEvent(&e, 0);

  switch (e.eType) {
  case keyDownEvent:
    if (e.data.keyDown.chr == hsChrWakeup)
      break;
    /* else fall through */

  case penDownEvent:
  case keyUpEvent:
    interrupted = 1;
    res = false;
    break;

  default:
    /* do nothing */
  }

  if (FtrSet(AppType, DB_FEATURE_SOUND_INTERRUPTED, interrupted))
    abort(); /* fatal error */
  
  return res;
}

/*
** Helper function for playing SMFs (used by PlayAlarmSound).
*/
static void PlaySmf(MemHandle midiH) {
  struct StackFrame {
    SndMidiRecHdrType* midiHdrP;   /* pointer to MIDI record header */
    UInt8* midiStreamP;		   /* pointer to MIDI stream beginning with the 'MThd' */
                                   /* SMF header chunk */
    UInt16 recIndex;               /* record index of the MIDI record to play */
    SndSmfOptionsType smfOpt;	   /* SMF play options */
    SndSmfCallbacksType smfCallbacks; /* SMF callback functions */
  } *s;

  /* Reserve & initialize virtual stack frame */
  s = MemPtrNew(sizeof(struct StackFrame));
  ASSERT(s);
  MemSet(s, sizeof(struct StackFrame), 0);

  /* Lock the MIDI record/resource */ 
  s->midiHdrP = MemHandleLock(midiH);

  /* Get a pointer to the SMF stream */
  s->midiStreamP = (UInt8*) s->midiHdrP + s->midiHdrP->bDataOffset;

  /* Play the sound (ignore the error code) */
  /* The sound can be interrupted by a key/digitizer event */
  s->smfOpt.dwStartMilliSec = 0;
  s->smfOpt.dwEndMilliSec = sndSmfPlayAllMilliSec;
  s->smfOpt.amplitude = PrefGetPreference(prefAlarmSoundVolume);
  s->smfOpt.interruptible = false;
  s->smfOpt.reserved = 0;
  s->smfCallbacks.completion.funcP = NULL;
  s->smfCallbacks.blocking.funcP = AlarmSoundBlockingFunction;
  SndPlaySmf(NULL, sndSmfCmdPlay, s->midiStreamP, &s->smfOpt, NULL, 
	     &s->smfCallbacks, false);

  /* Unlock the record/resource */
  MemPtrUnlock(s->midiHdrP);

  /* Clean up virtual stack frame */
  MemPtrFree(s);
}

/***********************************************************************
 *
 * FUNCTION:    PlayAlarmSound
 *
 * DESCRIPTION:	Play a MIDI sound given a unique record ID of the MIDI record in the System
 *		MIDI database.  If the sound is not found, then the default alarm sound will
 *		be played.
 *
 * PARAMETERS:  uniqueRecID	-- unique record ID of the MIDI record.
 *              dbID            -- database ID of the MIDI database.
 *              cardNo          -- card number of the MIDI database.
 *              interruptible   -- can users interrupt the sound?
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
void PlayAlarmSound(UInt32 uniqueRecID, LocalID dbID, UInt16 cardNo)
{
  struct StackFrame {
    Err err;
    MemHandle midiH;	           /* handle of MIDI record */
    DmOpenRef dbP;		   /* reference to open database */
    UInt16 recIndex;               /* record index of the MIDI record to play */
    DmSearchStateType searchState; /* search state for finding the System MIDI database */
    Boolean bError;                /* set to true if we couldn't find the MIDI record */
  } *s;

  /* Reserve & initialize virtual stack frame */
  s = MemPtrNew(sizeof(struct StackFrame));
  ASSERT(s);
  s->dbP = NULL;
  s->bError = false;

  /* Check if the dbID and cardNo are valid */
  if (dbID == 0 && cardNo == 0)
    s->bError = true; /* no sound set */

  /* Open the MIDI database in read-only mode */
  if (!s->bError) {
    s->dbP = DmOpenDatabase(cardNo, dbID, dmModeReadOnly);
    if (!s->dbP)
      s->bError = true; /* couldn't open */
  }

  /* Find the MIDI track record */
  if (!s->bError) {
    s->err = DmFindRecordByID(s->dbP, uniqueRecID, &s->recIndex);
    if (s->err)
      s->bError = true; /* record not found */
  }

  /* Lock the record and play the sound */
  if (!s->bError) {
    /* Find the record handle and play it */
    s->midiH = DmQueryRecord(s->dbP, s->recIndex);
    PlaySmf(s->midiH);
  }

  /* Close the MIDI database */
  if (s->dbP)
    DmCloseDatabase(s->dbP);

  /* If there was an error, play the alarm sound resource stored with the app. */
  /* This is equivalent to SndPlaySystemSound(sndAlarm), but the sound is */
  /* interruptible. */
  if (s->bError) {
    /* Find the resource handle and play it */
    s->midiH = DmGetResource(midiRsc, AlarmMIDI);
    PlaySmf(s->midiH);

    /* Unlock the resource */
    DmReleaseResource(s->midiH);
  }

  /* Clean up virtual stack frame */
  MemPtrFree(s);
}

/*
** DoSpecialEffects - if running on a device with implemented Attention manager,
**                    execute the set special effects for this record.
*/
void DoSpecialEffects(pref* prefs, UInt16 recordFlags, Boolean treo600) {
  UInt32 romVersion;
  AttnFlagsType flags = kAttnFlagsNothing;

  /* See if we're running on at least OS 4.0 */
  FtrGet(sysFtrCreator, sysFtrNumROMVersion, &romVersion);

  if (romVersion >= sysVersion40 && !treo600) {
    /* Add Vibrate */
    if (ShouldTriggerVibrate(recordFlags, prefs))
      flags = kAttnFlagsVibrateBit;
    
    /* Add LED */
    if (ShouldTriggerLED(recordFlags, prefs)) {
      if (flags != kAttnFlagsNothing)
	flags = flags | kAttnFlagsLEDBit;
      else
	flags = kAttnFlagsLEDBit;
    }
    
    /* Special effects work for most devices */
    AttnDoSpecialEffects(flags);

  } else if (treo600) { 
    /* The Treo 600 needs special handling */
    UInt16 indicatorClass = kIndicatorAlertAlert;

    if (ShouldTriggerVibrate(recordFlags, prefs))
      HsIndicatorState(3, kIndicatorTypeVibrator, &indicatorClass);
    if (ShouldTriggerLED(recordFlags, prefs))
      HsIndicatorState(3, kIndicatorTypeLed, &indicatorClass);
  }
}

/***********************************************************************
 *
 * FUNCTION:    ApptNextRepeat
 *
 * DESCRIPTION: This routine computes the next occurrence of a
 *              repeating appointment.
 *
 * PARAMETERS:  apptRec - a pointer to an appointment record
 *              dateP   - passed:   date to start from
 *                        returned: date of next occurrence
 *
 * RETURNED:    true if there is an occurrence of the appointment
 *              between the date passed and the appointment's end date
 *
 ***********************************************************************/
static Boolean ApptNextRepeat(DiddleBugRecordPtr apptRec, DateTimePtr dateP)
{
  DateTimeType date = *dateP;

  // Compute the next time the appointment repeats.
  if (!NextRepeat (apptRec, &date))
    return false;

  *dateP = date;
  return  true ;
}

/***********************************************************************
 *
 * FUNCTION:    ApptGetAlarmTime
 *
 * DESCRIPTION: This routine determines the date and time of the next alarm
 *              for the appointment passed.
 *
 * PARAMETERS:  apptRec     - pointer to an appointment record
 *              currentTime - current date and time in seconds
 *
 * RETURNED:    date and time of the alarm, in seconds, or zero if there
 *              is no alarm
 *
 ***********************************************************************/
UInt32 ApptGetAlarmTime(DiddleBugRecordType* record, UInt32 currentTime) {
  UInt32 alarmTime;
  DateTimeType repeatDate;
  DateTimeType when;

  if (!record->alarmSecs && !record->snoozeSecs)
    return 0;

  // Non-repeating appointment?
  if (record->repeatInfo.repeatType == repeatNone) {
    if (record->alarmSecs >= currentTime)
      return record->alarmSecs;
    else if (record->snoozeSecs >= currentTime)
      return record->snoozeSecs;
  } else {
    TimSecondsToDateTime(record->alarmSecs, &when);

    // Repeating appointment.
    TimSecondsToDateTime (currentTime, &repeatDate);

    while (ApptNextRepeat(record, &repeatDate)) {
      alarmTime = TimDateTimeToSeconds(&repeatDate);

      if (!record->snoozeSecs) {
	if (alarmTime >= currentTime)
	  return alarmTime;
      } else {
 	if (record->snoozeSecs >= currentTime && record->snoozeSecs <= alarmTime)
	  return record->snoozeSecs;
	else if (alarmTime >= currentTime)
	  return alarmTime;
      }

      TimAdjust (&repeatDate, daysInSeconds);
    }
  }

  return 0;
}

/***********************************************************************
 *
 * FUNCTION:    ApptAlarmMunger
 *
 * DESCRIPTION: Finds all appointments with alarms within a given range
 *					 of time.
 *
 * PARAMETERS:  dbR        - reference to open database
 *		alarmStart - first valid alarm time
 *		alarmStop  - last valid alarm time
 *		countP     - in: # of alarms to fit into alarmListH
 *		             out: # of alarms found
 *			     nil is ok
 *		alarmListP - list of alarms found, nil is ok
 *		audibleP   - true if alarm sound should play, nil is ok
 *              recordNumP - out: record number of first alarm
 *
 * RETURNED:    time of the first alarm within range
 *
 * COMMENTS:	To find all alarms at a specific time, set alarmStart
 *		and alarmStop to the same time.
 *
 *		To find the just the time of the next alarm, set
 *		alarmStart to now+1, alarmStop to max, and the three
 *		output parameters to nil.
 *
 ***********************************************************************/
UInt32 ApptAlarmMunger(DmOpenRef inDbR, UInt32 inAlarmStart, UInt32 inAlarmStop,
		       PendingAlarmQueueType* inAlarmInternalsP, UInt16* ioCountP,
		       PendingAlarmType* outAlarmListP, Boolean* outAudibleP,
		       UInt16* recordNumP)
{
  struct StackFrame {
    UInt16 alarmListSize;
    UInt16 numRecords;
    UInt16 numAlarms;
    MemHandle recordH;
    DiddleBugRecordType rec;
    UInt32 alarmTime;
    UInt32 uniqueID;
    Boolean skip;
    UInt16 index;
    UInt16 dismissedCount;
    UInt32* dismissedListP;
  } *s;
  UInt32 earliestAlarm = 0;
  UInt16 recordNum;
  UInt16 i;

  /* Reserve & initialize virtual stack frame */
  s = MemPtrNew(sizeof(struct StackFrame));
  ASSERT(s);
  s->numAlarms = 0;
  s->dismissedCount = 0;
  s->dismissedListP = NULL;

  if (outAudibleP)
    *outAudibleP = false;

  if (ioCountP)
    s->alarmListSize = *ioCountP;
  else
    s->alarmListSize = 0;

  if (inAlarmInternalsP) {
    s->dismissedCount = GetDismissedAlarmCount(inAlarmInternalsP);
    s->dismissedListP = GetDismissedAlarmList(inAlarmInternalsP);
  }

  s->numRecords = DmNumRecords(inDbR);
  for (recordNum = 0; recordNum < s->numRecords; recordNum++) {
    s->recordH = DmQueryRecord(inDbR, recordNum);
    if (!s->recordH)
      break;

    DmRecordInfo(inDbR, recordNum, NULL, &s->uniqueID, NULL);

    MemMove(&s->rec, MemHandleLock(s->recordH), sizeof(DiddleBugRecordType));

    if (s->rec.flags & RECORD_FLAG_ALARM_SET && (s->rec.alarmSecs || s->rec.snoozeSecs)) {
      /* Get the first alarm on or after inAlarmStart */
      s->alarmTime = ApptGetAlarmTime(&s->rec, inAlarmStart);

      /* If in range, add the alarm to the output */
      if (s->alarmTime && s->alarmTime >= inAlarmStart && s->alarmTime <= inAlarmStop) {
	s->skip = false;
	s->index = s->numAlarms;

	if (inAlarmInternalsP) {
	  /* Skip over any alarms that have already been dismissed */
	  if (s->dismissedCount && (s->alarmTime == inAlarmStart)) {
	    for (i = 0; i < s->dismissedCount; i++) {
	      if (s->dismissedListP[i] == s->uniqueID) {
		s->skip = true;
		break;
	      }
	    }
	  }
	}

	if (!s->skip) {
	  /* If collecting alarms, add it to the list */
	  if (outAlarmListP) {
	    /* If the alarm list is too large, push the oldest alarm off of the */
	    /* queue to make room for the new one                               */
	    if (s->index >= s->alarmListSize)	{
	      MemMove(&outAlarmListP[0], &outAlarmListP[1],
		      (s->alarmListSize - 1) * sizeof(*outAlarmListP));
	      s->index = s->alarmListSize - 1;
	    }

	    outAlarmListP[s->index].recordNum = recordNum;
	    outAlarmListP[s->index].alarmTime = s->alarmTime;
	  }

	  // If the event is timed, inform the caller to play the alarm sound
	  if (outAudibleP && s->rec.alarmSecs)
	    *outAudibleP = true;

	  // Remember the earliest in-range alarm for our return value
	  if (s->alarmTime < earliestAlarm || earliestAlarm == 0) {
	    earliestAlarm = s->alarmTime;

	    if (recordNumP)
	      *recordNumP = recordNum;
	  }

	  s->numAlarms++;
	} /* don't skip this alarm */
      }	/* add alarm to output */
    } /* an alarm exists */

    MemHandleUnlock(s->recordH);
  }

  if (ioCountP)
    *ioCountP = s->numAlarms;

  /* Clean up virtual stack frame */
  MemPtrFree(s);

  return earliestAlarm;
}

/*
** Handles events for gadgets acting as a dynamic OK button.
*/
static Boolean BigOKGadgetEvent(FormGadgetTypeInCallback* gadgetP, UInt16 cmd, void* paramP) {
  Boolean handled = false;
  DynamicButtonType* btn = (DynamicButtonType*)gadgetP->data;

  switch (cmd) {
  case formGadgetDeleteCmd:
    DynBtnDelete(btn);
    handled = true;
    break;

  case formGadgetDrawCmd:
    DynBtnDraw(btn);
    gadgetP->attr.visible = true;
    handled = true;
    break;

  case formGadgetHandleEventCmd:
    if (btn->enabled) {
      EventType* e = (EventType*) paramP;

      if (e->eType == frmGadgetEnterEvent) {
	FormType* frm = FrmGetActiveForm();
	const UInt16 idx = FrmGetObjectIndex(frm, AlarmClockGadget);

	if (DynBtnTrackPen(btn)) {
	  CtlHitControl(GetObjectPointer(frm, AlarmOkButton));
	  DynBtnDraw(btn);
	}

	/* Redraw clock gadget */
	if (FrmGlueGetObjectUsable(frm, idx))
	  FrmShowObject(frm, idx);
      }
    }

    handled = true;
    break;
  }

  return handled;
}

/*
** Handles events for gadgets acting as the clock.
*/
static Boolean ClockGadgetEvent(FormGadgetTypeInCallback* gadgetP, UInt16 cmd, void* paramP) {
  Boolean handled = false;
  DynamicButtonType* btn = (DynamicButtonType*)gadgetP->data;

  switch (cmd) {
  case formGadgetDeleteCmd:
    DynBtnDelete(btn);
    handled = true;
    break;

  case formGadgetDrawCmd:
    DynBtnDraw(btn);
    gadgetP->attr.visible = true;
    handled = true;
    break;
  }

  return handled;
}

/***********************************************************************
 *
 * FUNCTION:    DoAlarmDialog
 *
 * DESCRIPTION: Handles the dialog for a single alarm
 *
 * PARAMETERS:  inAppointmentP		Appointment containing the alarm
 *
 * RETURNED:    Button pressed by user
 *
 ***********************************************************************/
UInt16 DoAlarmDialog(pref* prefs, PendingAlarmType* inPendingInfoP,
		     Int16* snoozeSelection, Boolean first)
{
  struct StackFrame {
    BitmapType* bmp;
    BitmapTypeV3* bmpV3;
    BitmapType* myBmp;
    MemPtr ptr;
    MemHandle t;
    DiddleBugRecordType record;
    DmOpenRef dbR;
    Err err;
    UInt32 highDataOffset;
    Boolean note;
    DynamicButtonType* btn;
    WinHandle winH;
    FormType* curForm;
    AlarmSoundInfoType record_sound;
    UInt32 interrupted;
    UInt16 attr;
    
    /* Hardware support... */
    Boolean hires;                  /* Device supports 320 x 320 pixels */
    Boolean treo600;
    UInt32 keyguard;                /* Is the keyguard enabled? */

    /* ...and different OS versions */
    Boolean os4;                    /* We are running on at least PalmOS 4 */
    Boolean os5;                    /* We are running on a new ARM-based device */
  } *s;
  FormType* frm = FrmInitForm(AlarmForm);
  const UInt16 clockIndex = FrmGetObjectIndex(frm, AlarmClockGadget);
  UInt8 i;
  UInt16 buttonHit;

  /* Reserve & initialize virtual stack frame */
  s = MemPtrNew(sizeof(struct StackFrame));
  ASSERT(s);
  MemSet(s, sizeof(struct StackFrame), 0);
  s->curForm = FrmGetActiveForm();

  if (s->curForm)
    FrmSetActiveForm(frm);

  /* Set the event handler for the alarm dialog. */
  FrmSetEventHandler(frm, ShowAlarmHandleEvent);

  /* Load sketch */
  s->dbR = DmOpenDatabaseByTypeCreator(DBType, AppType, dmModeReadOnly|dmModeShowSecret);

  /* Read in record */
  s->t = DmQueryRecord(s->dbR, inPendingInfoP->recordNum);
  s->ptr = MemHandleLock(s->t);
  MemMove(&s->record, s->ptr, sizeof(DiddleBugRecordType));
    
  /* Check for various OS versions */
  s->os4 = CheckROMVersion(sysVersion40);
  s->os5 = CheckROMVersion(sysVersion50);
   
  /* Check for OS5 double density screen */
  if (s->os5)
    s->hires = CheckForDoubleDensity();

  /* Check for Treo 600 smartphone */
  s->treo600 = CheckForTreo600();

  /* Load image */
  if (s->hires) {
      s->bmp = BmpCreate(MAX_PIXELS_HIRES, MAX_PIXELS_HIRES, 1, NULL, &s->err);
      if (s->err) abort();
      s->bmpV3 = BmpCreateBitmapV3(s->bmp, kDensityDouble, BmpGetBits(s->bmp), NULL);
      s->myBmp = (BitmapType*) s->bmpV3;
  } else {
      s->bmp = BmpCreate(MAX_PIXELS, MAX_PIXELS, 1, NULL, &s->err);
      if (s->err) abort();
      s->myBmp = s->bmp;
  }

  /* Is this a private record? */
  DmRecordInfo(s->dbR, inPendingInfoP->recordNum, &s->attr, NULL, NULL);
  if (s->attr & dmRecAttrSecret && 
      PrefGetPreference(prefShowPrivateRecords) != showPrivateRecords) {
    const CustomPatternType maskPattern = { 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55 };
    s->winH = WinCreateBitmapWindow(s->myBmp, &s->err);
    if (s->err) abort();
    DrawMaskedRecord(s->winH, &maskPattern);
    WinDeleteWindow(s->winH, false);
    s->winH = NULL;
  } else {
    UncompressSketch(s->ptr + sketchDataOffset + sketchThumbnailSize + 1,
		     BmpGetBits(s->bmp), 
		     s->record.sketchLength - sketchThumbnailSize - 1, s->hires);
  }
  
  /* Initialize the clock */
  s->btn = DynBtnInitGadget(AlarmClockGadget, rectangleFrame, false, true, false, 
			    s->hires, dynBtnText, 
			    timeStringLength, &s->err, frm, ClockGadgetEvent);
  if (s->err) abort();
  s->btn->font = boldFont;
  s->btn->center = true;
  s->btn->enabled = false;
  UpdateClock(frm, clockIndex, false);
  if (prefs->flags & PFLAGS_CLOCK_ENABLED)
    FrmShowObject(frm, clockIndex);

  /* Initialize big OK button */
  s->btn = DynBtnInitGadget(AlarmBigOkButton, noFrame, false, true, false, 
			    s->hires, dynBtnGraphical, 
			    0, &s->err, frm, BigOKGadgetEvent);
  if (s->err) abort();
  s->winH = WinSetDrawWindow(s->btn->content.bmpW);
  WinPushDrawState();
  WinSetBackColor(0); /* otherwise the pixels will not count as "off" */
  WinSetForeColor(1); /* otherwise pixels are screwed-up on PalmOS 5 */

  if (s->hires) {
      WinSetCoordinateSystem(kCoordinatesNative);
      WinDrawBitmap((BitmapType*) s->bmpV3, 0, 0);
  } else {
    WinDrawBitmap(s->bmp, 0, 0);
  }
     
  WinPopDrawState();
  WinSetDrawWindow(s->winH);
  s->btn->enabled = prefs->flags & PFLAGS_BIG_OK ? true : false;

  /* Discard image */
  if (s->bmpV3) BmpDelete((BitmapType*) s->bmpV3);
  BmpDelete(s->bmp);

  /* Read the additional alarm info */
  s->highDataOffset = sketchDataOffset + s->record.sketchLength;
  s->highDataOffset += StrLen((Char*)(s->ptr + s->highDataOffset)) + 1; /* +1 for null char */
  s->note = StrLen((Char*)(s->ptr + s->highDataOffset)) ? true : false;
  LoadRecordSound(s->dbR, &s->record_sound, NULL, inPendingInfoP->recordNum);

  /* Clean up */
  MemHandleUnlock(s->t);
  DmCloseDatabase(s->dbR);

  /* Show or hide buttons */
  if (s->note)
    FrmShowObject(frm, FrmGetObjectIndex(frm, AlarmNoteButton));

  /* Disable Treo 600 keyguard */
  if (s->treo600)
    HsAttrSet(hsAttrKeyboardLocked, 0, &s->keyguard); 

  FrmDrawForm (frm);

  if (first) {
    /* Play alarm sounds */
    for (i = 0; i < prefs->alarm_repeat_times; ++i) {
      DoSpecialEffects(prefs, s->record.flags, s->treo600);
      PlayAlarmSound(s->record_sound.uniqueRecID, s->record_sound.dbID, 
		     s->record_sound.cardNo);
      FtrGet(AppType, DB_FEATURE_SOUND_INTERRUPTED, &s->interrupted);
      FtrUnregister(AppType, DB_FEATURE_SOUND_INTERRUPTED);
      if (s->interrupted != 0) break; /* Don't play the rest of the alarms */
    }
  }

  /* Display the alarm dialog. */
  buttonHit = FrmDoDialog(frm);

  *snoozeSelection = LstGetSelection(GetObjectPointer(frm, AlarmSnoozeList));

  FrmDeleteForm (frm);
  FrmSetActiveForm(s->curForm);

  /* Clean up virtual stack frame */
  MemPtrFree(s);

  return buttonHit;
}

/***********************************************************************
 *
 * FUNCTION:    ApptGetAlarmsList
 *
 * DESCRIPTION: This routine returns a list of appointment that have
 *              alarm scheduled at the specified time.  The list contains
 *              the handles of the appointment record.
 *
 * PARAMETERS:  dbP       - pointer to the database
 *              alarmTime - time of the alarm expressed as seconds from
 *                          1/1/1904.
 *              countP    - returnd: number of entries in list
 *              quiteP    - returnd: true if alarm sound should not play.
 *
 * RETURNED:    the handle of a block containing the record handles of
 *              appointments that have alarms scheduled at the passed time,
 *              or zero if there are not alarms scheduled.
 *
 ***********************************************************************/
MemHandle ApptGetAlarmsList (DmOpenRef inDbR, UInt32 inAlarmStart, UInt32 inAlarmStop,
			     UInt16* outCountP, Boolean* outAudibleP)
{
  UInt16 numBytes;
  MemHandle alarmListH;
  PendingAlarmType* alarmListP;
  PendingAlarmQueueType	alarmInternals;

  // Allocate a block to hold the a list of record handles.
  numBytes = sizeof (PendingAlarmType) * apptMaxDisplayableAlarms;
  alarmListH = MemHandleNew (numBytes);
  ASSERT(alarmListH);

  alarmListP = MemHandleLock (alarmListH);
  MemSet (alarmListP, numBytes, 0);

  // Gather any previously dismissed alarms so that they can be filtered out of the
  // returned list
  ReserveAlarmInternals (&alarmInternals);

  *outCountP = apptMaxDisplayableAlarms;
  ApptAlarmMunger (inDbR, inAlarmStart, inAlarmStop,
		   &alarmInternals, outCountP, alarmListP, outAudibleP, NULL);

  ReleaseAlarmInternals (&alarmInternals, true);

  MemHandleUnlock (alarmListH);

  // Free the list if no alarms were found.
  if (*outCountP == 0) {
    MemHandleFree (alarmListH);
    alarmListH = 0;
  } else {
    MemHandleResize (alarmListH, *outCountP * sizeof (PendingAlarmType));
  }

  return (alarmListH);
}

/***********************************************************************
 *
 * FUNCTION:    ApptGetTimeOfNextAlarm
 *
 * DESCRIPTION: This routine determines the time of the next scheduled
 *              alarm.
 *
 * PARAMETERS:  dbP           - pointer to the database
 *              timeInSeconds - time to search forward from
 *
 * RETURNED:    time of the next alarm, in seconds from 1/1/1904, or
 *              zreo if there are no alarms scheduled.
 *
 ***********************************************************************/
extern inline UInt32 ApptGetTimeOfNextAlarm(DmOpenRef inDbR, UInt32 inAlarmStart, UInt16* recordNumP) {
  return ApptAlarmMunger(inDbR, inAlarmStart, apptEndOfTime, NULL, NULL, NULL, NULL, recordNumP);
}

/*
** GetSnoozeSecsFromList - return number of seconds to snooze
*/
static UInt32 GetSnoozeSecsFromList(UInt16 list_selection) {
  UInt32 secs;

  if (list_selection == 1) {
    secs = DoAbsoluteSnoozeDialog();
  } else {
    --list_selection;

    switch (list_selection) {
    case 1:  /* 1 min */
    case 2:  /* 2 min */
    case 3:  /* 3 min */
    case 4:  /* 4 min */
    case 5:  /* 5 min */
      secs = minutesInSeconds * list_selection;
      break;
    case 6:  /* 10 min */
    case 7:  /* 15 min */
    case 8:  /* 20 min */
    case 9:  /* 25 min */
    case 10: /* 30 min */
      secs = minutesInSeconds * 5L * (list_selection-4);
      break;
    case 11: /* 45 min */
      secs = minutesInSeconds * 45L;
      break;
    case 12: /* 1 hr   */
    case 13: /* 2 hr   */
    case 14: /* 3 hr   */
    case 15: /* 4 hr   */
    case 16: /* 5 hr   */
      secs = hoursInSeconds * (list_selection-11);
      break;
    case 17: /* 8 hr   */
    case 18: /* 10 hr  */
    case 19: /* 12 hr  */
      secs = hoursInSeconds * 2L * (list_selection-13);
      break;
    case 20: /* 1 day  */
    case 21: /* 2 days */
    case 22: /* 3 days */
    case 23: /* 4 days */
    case 24: /* 5 days */
    case 25: /* 6 days */
    case 26: /* 1 week */
      secs = daysInSeconds * (list_selection-19);
      break;
    case 27: /* 2 weeks */
      secs = 14L * daysInSeconds;
      break;
    default:
      secs = 0;
      break;
    }
  }

  return secs;
}

/***********************************************************************
 *
 * FUNCTION:    DisplayAlarm
 *
 * DESCRIPTION: This routine displays all pending alarms.
 *
 * PARAMETERS:  nothing
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
void DisplayAlarm(Boolean appIsActive) {
  struct StackFrame {
    RefType ref;
    UInt32 currentAlarmTime;
    UInt32 nextAlarmTime;
    UInt32 previousAlarmTime;
    DmOpenRef dbP;
    PendingAlarmType pendingInfo;
    UInt16 buttonHit;
    UInt32 uniqueID;
    DiddleBugRecordType record;
    MemHandle recordH;
    UInt16 dismissedCount;
    UInt32* dismissedAlarms;
    PendingAlarmQueueType alarmInternals;
    UInt32 snoozeDuration;
    Int16 snoozeSelection;
    Boolean pleaseDelete;
    Boolean goto_diddlebug;
    Boolean goto_note;
    Boolean savePrefs;
    Boolean private_record;
    pref prefs, *pp;
    UInt16 prefsSize, attr;
    EventType updateEvent;
    DmSearchStateType searchstate;
    UInt16 cardno;
    LocalID dbID;
    UInt16 count;
    UInt16 record_to_goto;
    Boolean first;
    UInt32 now;
    privateRecordViewEnum oldPrivateRecordStatus;
  } *s;
  UInt16 i;

  /* Reserve & initialize virtual stack */
  s = MemPtrNew(sizeof(struct StackFrame));
  ASSERT(s);
  MemSet(s, sizeof(struct StackFrame), 0);
  s->buttonHit = AlarmOkButton;
  s->record_to_goto = -1;
  s->first = true;

  if (!appIsActive) {
    s->dbP = DmOpenDatabaseByTypeCreator (DBType, AppType, dmModeReadWrite|dmModeShowSecret);
  } else {
    s->dbP = d.dbR;
    FtrSet(AppType, DB_FEATURE_GLOBALS_AVAILABLE, 1);
  }

  if (!s->dbP) return;

  /* We need the preferences */
  if (appIsActive) {
    s->pp = &p;
    s->savePrefs = false;
  } else {
    s->pp = &s->prefs;
    s->savePrefs = !GetPreferences(s->pp, &s->prefsSize, NULL, false);
  }

  /* Get resources */
  ReserveAlarmInternals (&s->alarmInternals);

  // Get the list of pending alarms, if we don't have a list of pending
  // alarms exit.
  if (GetPendingAlarmCount(&s->alarmInternals) == 0)
    goto Exit;

  // Get a pointer to the dismissed alarm list. If there's no list, we must be in an
  // extremely low memory condition, so exit.
  s->dismissedCount = GetDismissedAlarmCount(&s->alarmInternals);
  s->dismissedAlarms = GetDismissedAlarmList(&s->alarmInternals);
  if (s->dismissedAlarms == NULL) {
    ErrNonFatalDisplay ("Couldn't get dismissed alarms");
    goto Exit;
  }

  // Display alarms until there are no more pending or the user hits snooze
  while (true) {
    if (GetPendingAlarmCount(&s->alarmInternals) == 0) {
      ClearPendingAlarms (&s->alarmInternals);
      break;
    }

    // Get the record number and alarm time, of the next alarm, from
    // the pending alarms list.
    GetPendingAlarm (&s->alarmInternals, 0, &s->pendingInfo);

    // Get the sketch record
    s->recordH = DmQueryRecord(s->dbP, s->pendingInfo.recordNum);
    MemMove(&s->record, MemHandleLock(s->recordH), sizeof(DiddleBugRecordType));
    MemHandleUnlock(s->recordH);
    DmRecordInfo(s->dbP, s->pendingInfo.recordNum, NULL, &s->uniqueID, NULL);
    s->currentAlarmTime = ApptGetAlarmTime(&s->record, s->pendingInfo.alarmTime);

    // Show the alarm dialog
    //
    // Because we are working with a local copy of the alarm internals, we must
    // be careful to release them before presenting the alarm dialog. If we fail
    // to do so, any subsequent alarms that are received by AlarmTriggered(),
    // while the dialog is still up, will not appear in our copy of the internals.
    ReleaseAlarmInternals (&s->alarmInternals, true);

    s->buttonHit = DoAlarmDialog(s->pp, &s->pendingInfo, &s->snoozeSelection, s->first);
    s->first = false;

    ReserveAlarmInternals(&s->alarmInternals);

    /* Snooze? */
    s->snoozeDuration = GetSnoozeSecsFromList(s->snoozeSelection);
    s->now = TimGetSeconds();
    s->record.snoozeSecs = 0;

    // Suspend alarm notification before fiddling with the pending alarms list
    // to avoid collisions with AlarmTriggered(), which also fiddles with the list
    s->nextAlarmTime = AlarmGetTrigger(&s->ref.united);
    AlarmSetTrigger(0, 0);

    if (s->snoozeDuration && s->buttonHit == AlarmOkButton) {
      if (s->record.repeatInfo.repeatType == repeatNone ||
	  (s->now + s->snoozeDuration <= ApptGetAlarmTime(&s->record, s->now + 1))) {
	s->record.snoozeSecs = s->now + s->snoozeDuration;
      } else {
	/* If we should have been snoozing,            */
	/* but didn't because the time was too long... */
	FrmAlert(BadSnoozeForm);
	s->record.snoozeSecs = 0;
      }
    } else {
      s->record.snoozeSecs = 0;
    }

    /* Save new snooze value */
    DmWrite(MemHandleLock(s->recordH), 0, &s->record, sizeof(DiddleBugRecordType));
    MemHandleUnlock(s->recordH);

    /* Update record in app if running */
    if (appIsActive && p.dbI == s->pendingInfo.recordNum)
      d.record.snoozeSecs = s->record.snoozeSecs;

    /* Remove the alarm from the pending list */
    RemovePendingAlarm(&s->alarmInternals, 0);

    if (s->currentAlarmTime != s->previousAlarmTime)
      ClearDismissedAlarms(&s->alarmInternals);

    AppendDismissedAlarm(&s->alarmInternals, s->uniqueID);

    s->previousAlarmTime = s->currentAlarmTime;

    /* Reschedule alarm if we are snoozing */
    if (s->record.snoozeSecs) {
      s->nextAlarmTime = ApptGetTimeOfNextAlarm(s->dbP, TimGetSeconds(), &s->ref.split.recordNum);
      s->ref.split.ref = 0;
    } else if (s->nextAlarmTime && s->ref.split.ref > 0) {
      /* Cancel any scheduled repeating alarm sounds and schedule the next alarm */
      s->nextAlarmTime = ApptGetTimeOfNextAlarm(s->dbP, s->nextAlarmTime + 1, &s->ref.split.recordNum);
      s->ref.split.ref = 0;
    }

    /* Restore alarm notification */
    AlarmSetTrigger(s->nextAlarmTime, s->ref.united);

    /* Jump to DiddleBug or delete sketch as necessary */
    switch (s->buttonHit) {
    case AlarmDiddleBugButton:
      s->goto_diddlebug = true;
      s->record_to_goto = s->pendingInfo.recordNum;
      s->goto_note = false;
      s->pleaseDelete = false;
      break;

    case AlarmNoteButton:
      s->goto_note = true;
      s->record_to_goto = s->pendingInfo.recordNum;
      s->goto_diddlebug = false;
      s->pleaseDelete = false;
      break;

    case AlarmDeleteButton:
      if ((s->record.flags & RECORD_FLAG_LOCKED)  && !FrmAlert(ConfirmLockedDelete))
	s->pleaseDelete = true;
      else if ((s->pp->flags & PFLAGS_CONFIRM_DELETE) && !FrmAlert(ConfirmDeleteForm))
	s->pleaseDelete = true;
      break;

    default:
      s->pleaseDelete = false;
    }

    if (s->pleaseDelete) {
      DmRemoveRecord(s->dbP, s->pendingInfo.recordNum);
      if (appIsActive) {
	if (s->pendingInfo.recordNum == p.dbI)
	  s->updateEvent.data.frmUpdate.updateCode = fUpdateDeletedCurrent;
	else
	  s->updateEvent.data.frmUpdate.updateCode = fUpdateDeletedAny;

	if (s->pendingInfo.recordNum <= p.dbI && p.dbI > 0)
	  p.dbI --;

	/* Update cached number of records */
	d.records_in_cat = DmNumRecordsInCategory(d.dbR, p.category);

	s->updateEvent.eType = frmUpdateEvent;
	EvtAddEventToQueue(&s->updateEvent);
      }
      else if (s->savePrefs && (s->pendingInfo.recordNum <= s->prefs.dbI) && (s->prefs.dbI > 0))
	s->prefs.dbI--;

      /* Decrease pending alarm indexes if necessary */
      s->count = GetPendingAlarmCount(&s->alarmInternals);
      for (i = 0; i < s->count; ++i) {
	if (s->alarmInternals.pendingAlarm[i].recordNum > s->pendingInfo.recordNum && 
	    s->alarmInternals.pendingAlarm[i].recordNum > 0) {
	  s->alarmInternals.pendingAlarm[i].recordNum--;
	}
      }

      RescheduleAlarms(s->dbP);
    }
  }

  /* Clear the list of dismissed siblings */
  ClearDismissedAlarms(&s->alarmInternals);

  /* Save the preferences */
  if (s->savePrefs)
    PrefSetAppPreferences(AppType, dbsNormalAppPrefs, 1, &s->prefs, sizeof(pref), true);

 Exit:
  /* Clean up */
  ReleaseAlarmInternals(&s->alarmInternals, true);
  FtrUnregister(AppType, DB_FEATURE_GLOBALS_AVAILABLE);

  /* Goto DiddleBug? */
  if (s->goto_note || s->goto_diddlebug) {
    /* Is this a private record? */
    DmRecordInfo(s->dbP, s->record_to_goto, &s->attr, NULL, NULL);
    s->private_record = s->attr & dmRecAttrSecret;

    /* we can close the database now */
    if (!appIsActive)
      DmCloseDatabase(s->dbP);
    
    s->oldPrivateRecordStatus = PrefGetPreference(prefShowPrivateRecords);
    if (!s->private_record || SecVerifyPW(showPrivateRecords)) {
      /* We only want to unmask this one record, so restore the preference. */
      if (s->oldPrivateRecordStatus != showPrivateRecords)	
	PrefSetPreference(prefShowPrivateRecords, s->oldPrivateRecordStatus);
      
      if (!appIsActive
	  && !DmGetNextDatabaseByTypeCreator(true, &s->searchstate, sysFileTApplication, AppType,
					     true, &s->cardno, &s->dbID))
	{
	  GoToParamsPtr par = MemPtrNew(sizeof(GoToParamsType));
	  ASSERT(par);
	  MemPtrSetOwner(par, 0);

	  par->searchStrLen = 0;
	  par->dbCardNo = s->cardno;
	  par->dbID = s->dbID;
	  par->recordNum = s->record_to_goto;
	  par->matchPos = 0;
	  par->matchFieldNum = s->goto_diddlebug ? ffName : ffNote;
	  par->matchCustom = 0;

	  SysUIAppSwitch(s->cardno, s->dbID, sysAppLaunchCmdGoTo, par);
	} else {
	  SendGotoEvent(s->record_to_goto, s->goto_diddlebug ? ffName : ffNote, 0, 0);
	}
    }
  } else {
    /* Just close the database */
    if (!appIsActive)
      DmCloseDatabase(s->dbP);
  }

  /* Clean up virtual stack frame */
  MemPtrFree(s);
}

/*
** Load the alarm sound data from the specified record
**
** rec may be NULL.
*/
void LoadRecordSound(DmOpenRef dbR, AlarmSoundInfoType* record_sound,
		     DiddleBugRecordType* rec, UInt16 record_index) {
  struct StackFrame {
    MemHandle t;
    MemPtr ptr;
    DiddleBugRecordType record;
    UInt16 highDataOffset;
    pref tempPref;
    UInt16 prefsSize;
  } *s;

  /* Reserve virtual stack frame */
  s = MemPtrNew(sizeof(struct StackFrame));
  ASSERT(s);

  /* Load preferences */
  GetPreferences(&s->tempPref, &s->prefsSize, NULL, false);

  /* Open the record and get the alarm info for it */
  s->t = DmQueryRecord(dbR, record_index);
  if (!s->t) abort();
  s->ptr = MemHandleLock(s->t);
  MemMove(&s->record, s->ptr, sizeof(DiddleBugRecordType));

  /* Read the additional alarm info */
  s->highDataOffset = sketchDataOffset + s->record.sketchLength;
  s->highDataOffset += StrLen((Char*)(s->ptr + s->highDataOffset));
  s->highDataOffset++; /* The null character */
  s->highDataOffset += StrLen((Char*)(s->ptr + s->highDataOffset));
  s->highDataOffset++; /* The null character */

  /* Read in the AlarmSoundInfoType structure, if one exists */
  if (s->record.extraLength == sizeof(AlarmSoundInfoType))
    MemMove(record_sound, s->ptr + s->highDataOffset, s->record.extraLength);
  
  /* Otherwise, read the default sound from the preferences */
  if (!(s->record.flags & RECORD_FLAG_CUSTOM_SIGNAL) || record_sound->listIndex == -1)
    *record_sound = s->tempPref.std_alarm;

  /* Store record if caller wants it */
  if (rec)
    *rec = s->record;

  /* Clean up */
  MemHandleUnlock(s->t);

  /* Clean up virtual stack frame */
  MemPtrFree(s);
}

/***********************************************************************
 *
 * FUNCTION:	AlarmTriggered
 *
 * DESCRIPTION:	This routine is called when the alarm manager informs the
 *		diddlebug application that an alarm has triggered.
 *		THIS IS CALLED AT INTERRUPT LEVEL! DONT USE GLOBALS!!
 *
 * PARAMETERS:	time of the alarm.
 *
 * RETURNED:	time of the next alarm
 *
 ***********************************************************************/
void AlarmTriggered(SysAlarmTriggeredParamType* cmdPBP, Boolean appIsActive) {
  struct StackFrame {
    pref prefs;
    MemHandle alarmListH;
    PendingAlarmType* alarmListP;
    DmOpenRef dbP;
    UInt32 alarmRangeBegin;
    UInt32 alarmRangeEnd;
    UInt16 alarmCount;
    Boolean audible;
    UInt32 alarmTime;
    Boolean playSound;
    Boolean newAlarm;
    RefType ref;
    RefType cmdRef;
    UInt32 repeatSoundTime;
    UInt16 baseIndex;
    Err err;
    PendingAlarmQueueType alarmInternals;
    Int16 prefsSize;
    AlarmSoundInfoType record_sound;
    DiddleBugRecordType record;
    UInt32 interrupted;
    Boolean treo600;
  } *s;
  UInt16 i;

  /* Reserve & initialize virtual stack frame */
  s = MemPtrNew(sizeof(struct StackFrame));
  ASSERT(s);
  s->playSound = false;
  s->baseIndex = 0;
  s->err = errNone;

  /* Open database */
  s->dbP = DmOpenDatabaseByTypeCreator(DBType, AppType, dmModeReadOnly|dmModeShowSecret);
  if (!s->dbP) return;

  /* Get the app preferences */
  GetPreferences(&s->prefs, &s->prefsSize, NULL, false);

  /* Get resources */
  ReserveAlarmInternals(&s->alarmInternals);

  // Establish the times for which alarms need to be retrieved.
  s->alarmRangeEnd = cmdPBP->alarmSeconds;
  s->alarmRangeBegin = s->alarmRangeEnd;

  /* Change cmdPBP->ref to a usable format */
  s->cmdRef.united = cmdPBP->ref;

  s->newAlarm = (s->cmdRef.split.ref == 0);

  // DisplayAlarm(), as currently implemented, displays all pending alarms (including
  // any alarms triggered while in DisplayAlarm, itself) prior to returning. If the
  // pending count is not zero, we can assume that DisplayAlarm is already up (or is
  // about to receive an earlier sysAppLaunchCmdDisplayAlarm event), so we can suppress
  // the unnecessary launch event.
  if (GetPendingAlarmCount(&s->alarmInternals) != 0) {
    cmdPBP->purgeAlarm = true;

    // Make sure not to interfere with the currently displayed alarm
    s->baseIndex = 1;
  }

  // If this is the first notification of an alarm, add it to the list of pending alarms.
  if (s->newAlarm) {
    // Get a list of all the appointments that have an alarm scheduled
    // at the passed time.

    // Because we are working with a local copy of the alarm internals, we must
    // be careful to release them before calling ApptGetAlarmsList(), which also
    // to do so, any subsequent alarms that are received by AlarmTriggered(),
    // while the dialog is still up, will not appear in our copy of the internals.
    ReleaseAlarmInternals(&s->alarmInternals, true);
    s->alarmListH =  ApptGetAlarmsList(s->dbP, s->alarmRangeBegin, s->alarmRangeEnd, &s->alarmCount, &s->audible);
    ReserveAlarmInternals(&s->alarmInternals);

    if (s->alarmCount != 0) {
      s->alarmListP = (PendingAlarmType*) MemHandleLock(s->alarmListH);

      // Add the alarms that just trigggered to the pending alarms list.
      for (i = 0; i < s->alarmCount; ++i) {
	s->err = AddPendingAlarm(&s->alarmInternals, s->alarmListP[i].recordNum,
				 s->alarmListP[i].alarmTime, s->baseIndex);
	if (s->audible && (s->err != diddlebugErrAlarmListFull)) {
	  // Play the alarm sound unless the pending alarms list has become full. This
	  // should save a little bit of battery life when the device is left unattended.
	  s->playSound = true;
	  LoadRecordSound(s->dbP, &s->record_sound, &s->record, s->alarmListP[i].recordNum);
	}
      }

      // Free alarm data
      MemHandleFree(s->alarmListH);
    } else {
      // If our alarms disappeared, don't bother calling the display code
      cmdPBP->purgeAlarm = true;
    }
  } else {
    // If this it not a new alarm, then the alarm notification dialog is currently on
    // display, so we set the purgeAlarm flag to block the sysAppLaunchCmdDisplayAlarm
    // launch command.
    cmdPBP->purgeAlarm = true;
    s->audible = true;
    s->playSound = true;

    /* read the alarm sound/flags */
    LoadRecordSound(s->dbP, &s->record_sound, &s->record, s->cmdRef.split.recordNum);

    /* Load repeat sound */
    s->record_sound = s->prefs.remind_alarm;
  }

  /* Check for Treo 600 smartphone */
  s->treo600 = CheckForTreo600();
  
  /* Play the alarm sound & trigger special effects. */
  if (s->playSound && cmdPBP->purgeAlarm) {
    for (i = 0; i < s->prefs.alarm_repeat_times; ++i) {
      DoSpecialEffects(&s->prefs, s->record.flags, s->treo600);
      PlayAlarmSound(s->record_sound.uniqueRecID, s->record_sound.dbID, 
		     s->record_sound.cardNo);
      FtrGet(AppType, DB_FEATURE_SOUND_INTERRUPTED, &s->interrupted);
      FtrUnregister(AppType, DB_FEATURE_SOUND_INTERRUPTED);
      if (s->interrupted != 0) break; /* Don't play the rest of the alarms */
    }
  }

  // Get the time of the next alarm on any event.
  s->alarmTime = ApptGetTimeOfNextAlarm(s->dbP, s->alarmRangeEnd +1, &s->ref.split.recordNum);
  s->ref.split.ref = 0;

  // If the time of the next event's alarm is after the time of the next
  // repeat  sound, schedule an alarm for the repeat sound.
  if (s->audible && s->cmdRef.split.ref < s->prefs.alarm_auto_snooze_times) {
    s->repeatSoundTime = cmdPBP->alarmSeconds + s->prefs.alarm_auto_snooze_delay * minutesInSeconds;

    if ((s->alarmTime == 0 || s->repeatSoundTime < s->alarmTime) && s->repeatSoundTime >= TimGetSeconds()) {
      s->alarmTime = s->repeatSoundTime;
      s->ref.split.recordNum = s->cmdRef.split.recordNum;
      s->ref.split.ref = s->cmdRef.split.ref + 1;
    }
  }

  /* Set next alarm */
  AlarmSetTrigger(s->alarmTime, s->ref.united);

  /* Clean up */
  ReleaseAlarmInternals(&s->alarmInternals, true);
  DmCloseDatabase(s->dbP);

  /* Free virtual stack frame */
  MemPtrFree(s);
}


/***********************************************************************
 *
 * FUNCTION:    RescheduleAlarms
 *
 * DESCRIPTION: This routine computes the time of the next alarm and
 *              compares it to the time of the alarm scheduled with
 *              Alarm Manager,  if they are different it reschedules
 *              the next alarm with the Alarm Manager.
 *
 * PARAMETERS:  dbP - the appointment database
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
void RescheduleAlarms(DmOpenRef dbP) {
  struct StackFrame {
    RefType ref;
    UInt32 timeInSeconds;
    UInt32 nextAlarmTime;
    UInt32 scheduledAlarmTime;
    PendingAlarmQueueType alarmInternals;
  } *s;

  /* Reserve virtual stack frame */
  s = MemPtrNew(sizeof(struct StackFrame));
  ASSERT(s);

  ReserveAlarmInternals(&s->alarmInternals);

  // The pending alarm queue used to be empty, as a rule, any time that
  // RescheduleAlarms could possibly be called. With the addition of the
  // snooze feature, the pending alarm queue may not be empty and, if this
  // is the case, could be invalidated by the changes that prompted the
  // call to RescheduleAlarms. We clear them here and will rebuild the
  // queue in AlarmTriggered().
  ClearPendingAlarms(&s->alarmInternals);

  s->scheduledAlarmTime = AlarmGetTrigger(&s->ref.united);
  s->timeInSeconds = TimGetSeconds();
  if (s->timeInSeconds < s->scheduledAlarmTime || s->scheduledAlarmTime == 0 || s->ref.split.ref > 0)
    s->scheduledAlarmTime = s->timeInSeconds;

  s->nextAlarmTime = ApptGetTimeOfNextAlarm(dbP, s->scheduledAlarmTime, &s->ref.split.recordNum);

  // If the scheduled time of the next alarm is not equal to the
  // calculate time of the next alarm,  reschedule the alarm with
  // the alarm manager.
  if (s->scheduledAlarmTime != s->nextAlarmTime) {
    s->ref.split.ref = 0;
    AlarmSetTrigger(s->nextAlarmTime, s->ref.united);
  }

  ReleaseAlarmInternals(&s->alarmInternals, true);

  /* Clean up virtual stack frame */
  MemPtrFree(s);
}


/***********************************************************************
 *
 * FUNCTION:    AlarmReset
 *
 * DESCRIPTION: This routine is called when the system time is changed
 *              by the Preference application, or when the device is reset.
 *
 * PARAMETERS:  newerOnly - If true, we will not reset the alarm if the
 *              time of the next (as calculated) is greater then the
 *              currently scheduled alarm.
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
void AlarmReset(Boolean newerOnly) {
  struct StackFrame {
    UInt32 ref;
    UInt32 currentTime;
    UInt32 alarmTime;
    DmOpenRef dbP;
    PendingAlarmQueueType alarmInternals;
  } *s;

  /* Reserve virtual stack frame */
  s = MemPtrNew(sizeof(struct StackFrame));
  ASSERT(s);

  if (newerOnly) {
    s->alarmTime = AlarmGetTrigger(&s->ref);
    s->currentTime = TimGetSeconds();
    if (s->alarmTime && s->alarmTime <= s->currentTime)
      return;
  }

  /* Clear any pending alarms. */
  AlarmSetTrigger(0, 0);

  ReserveAlarmInternals(&s->alarmInternals);
  ReleaseAlarmInternals(&s->alarmInternals, true);

  /* Open the sketch database. */
  s->dbP = DmOpenDatabaseByTypeCreator(DBType, AppType, dmModeReadOnly);
  if (!s->dbP) return;

  RescheduleAlarms(s->dbP);

  DmCloseDatabase(s->dbP);

  /* Clean up virtual stack frame */
  MemPtrFree(s);
}

/***********************************************************************
 *
 * FUNCTION:    ReserveAlarmInternals
 *
 * DESCRIPTION: Returns the internal struct which stores pending alarms,
 *		etc. for use by AlarmTrigger and DisplayAlarm, et al.
 *		THIS MAY BE CALLED AT INTERRUPT LEVEL (inInterrupt == true)!
 *		IF SO, DONT USE GLOBALS!!
 *
 * PARAMETERS:  inInterrupt -- If true, use FeatureMgr to retrieve
 *			       application globals.
 *
 * RETURNED:    Pointer to the internal struct
 *
 ***********************************************************************/
static void ReserveAlarmInternals(PendingAlarmQueueType* outAlarmInternalsP) {
  UInt16 prefsSize = sizeof (PendingAlarmQueueType);
  Int16	prefsVersion;

  prefsVersion = PrefGetAppPreferences (AppType, dbuAlarmQueue, outAlarmInternalsP,
					&prefsSize, false);

  if (prefsVersion == noPreferenceFound) {
    MemSet (outAlarmInternalsP, sizeof (PendingAlarmQueueType), 0);

    PrefSetAppPreferences (AppType, dbuAlarmQueue,
			   UnsavedPrefsVersion, outAlarmInternalsP,
			   sizeof (PendingAlarmQueueType), false);
  }
}


/***********************************************************************
 *
 * FUNCTION:    ReleaseAlarmInternals
 *
 * DESCRIPTION: Releases the internal struct which stores pending alarms,
 *		etc. for use by AlarmTrigger and DisplayAlarm, et al.
 *		THIS MAY BE CALLED AT INTERRUPT LEVEL, SO DONT USE GLOBALS!!
 *
 * PARAMETERS:  inAlarmInternalsP -- Pointer to the internal struct
 *		inModified        -- If true, use FeatureMgr to retrieve
 *				     application globals.
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
static void ReleaseAlarmInternals(PendingAlarmQueueType* inAlarmInternalsP, Boolean inModified)
{
  if (inModified) {
    PrefSetAppPreferences (AppType, dbuAlarmQueue,
			   UnsavedPrefsVersion, inAlarmInternalsP,
			   sizeof (PendingAlarmQueueType), false);
  }
}

/***********************************************************************
 *
 * FUNCTION:    SetPendingAlarmCount
 *
 * DESCRIPTION: Set size of pending alarms queue. Assumes count is
 *		within range.
 *		THIS MAY BE CALLED AT INTERRUPT LEVEL (inInterrupt == true)!
 *		IF SO, DONT USE GLOBALS!!
 *
 * PARAMETERS:  inCount     -- Number of items in pending alarms queue
 *		inInterrupt -- If true, use FeatureMgr to retrieve
 *			       application globals.
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
extern inline void SetPendingAlarmCount(PendingAlarmQueueType* inAlarmInternalsP, UInt16 inCount) {
  inAlarmInternalsP->pendingCount = inCount;
}


/***********************************************************************
 *
 * FUNCTION:    AddPendingAlarm
 *
 * DESCRIPTION: This routine add an entry to the pending alarms list.
 *
 * PARAMETERS:  record index
 *              time of the alarm.
 *
 * RETURNED:    true if the alarm  has added to the pending alarms list,
 *              false if the alarm replaced a pending alarm or has not
 *              added at all.
 *
 ***********************************************************************/
static Err
AddPendingAlarm (PendingAlarmQueueType* inAlarmInternalsP, UInt16 recordNum,
		 UInt32 alarmTime, UInt16 inBaseIndex)
{
  UInt32 count;
  PendingAlarmType baseAlarm, tempAlarm, newAlarm;
  Err err = errNone;
  UInt16 alarmIndex;

  count = GetPendingAlarmCount (inAlarmInternalsP);
  GetPendingAlarm (inAlarmInternalsP, inBaseIndex, &baseAlarm);

  // As a fail safe, make sure that the given record isn't already in the list
  for (alarmIndex = 0; alarmIndex < count; ++alarmIndex) {
    GetPendingAlarm (inAlarmInternalsP, alarmIndex, &tempAlarm);
    if (recordNum == tempAlarm.recordNum)
      return diddlebugErrDuplicateAlarm;
  }

  // If we're over the limit and the given alarm is newer than the oldest one
  // in the list, remove the oldest one
  if ((count >= apptMaxDisplayableAlarms) && (alarmTime > baseAlarm.alarmTime)) {
    RemovePendingAlarm (inAlarmInternalsP, inBaseIndex);
    count = apptMaxDisplayableAlarms - 1;
    err = diddlebugErrAlarmListFull;
  }

  // Find the index at which the given alarm should be inserted
  for (alarmIndex = inBaseIndex; alarmIndex < count; ++alarmIndex) {
    GetPendingAlarm (inAlarmInternalsP, alarmIndex, &tempAlarm);
    if (alarmTime < tempAlarm.alarmTime)
      break;
  }

  newAlarm.recordNum = recordNum;
  newAlarm.alarmTime = alarmTime;
  InsertPendingAlarm (inAlarmInternalsP, alarmIndex, &newAlarm);

  return err;
}


/***********************************************************************
 *
 * FUNCTION:    InsertPendingAlarm
 *
 * DESCRIPTION: Inserts an alarm into the pending alarm queue
 *		THIS MAY BE CALLED AT INTERRUPT LEVEL (inInterrupt == true)!
 *		IF SO, DONT USE GLOBALS!!
 *
 * PARAMETERS:  inAlarmIndex --	The position at which to insert the alarm
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
static void InsertPendingAlarm(PendingAlarmQueueType* inAlarmInternalsP, UInt16 inAlarmIndex,
			       PendingAlarmType* inPendingInfo)
{
  const UInt32 count = GetPendingAlarmCount(inAlarmInternalsP);

  if (count >= apptMaxDisplayableAlarms)
    return;

  SetPendingAlarmCount(inAlarmInternalsP, count + 1);

  if (inAlarmIndex < count) {
    MemMove(&inAlarmInternalsP->pendingAlarm[inAlarmIndex + 1],
	    &inAlarmInternalsP->pendingAlarm[inAlarmIndex],
	    (count - inAlarmIndex - 1) * sizeof (PendingAlarmType));
  }

  SetPendingAlarm(inAlarmInternalsP, inAlarmIndex, inPendingInfo);
}
