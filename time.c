/*

(c) 2001, 2002, 2004 Peter Putzer <pputzer@users.sourceforge.net>
(c) 1999, 2000 Mitch Blevins <mblevin@debian.org>.

Licensed under the GNU General Public License (GPL).

*/

#include <TextMgr.h>
#include <PalmOSGlue.h>

#include "diddlebug.h"
#include "diddlebugExtern.h"
#include "diddlebugInlines.h"
#include "diddlebugFeatures.h"
#include "diddlebugRsc.h"
#include "globals.h"
#include "debug.h"
#include "repeat.h"
#include "alarm.h"
#include "time.h"

static void DoRepeatDialog(void) SUTIL1;
static void SaveDateData(void) SUTIL2;
static void LoadDateData(void) SUTIL2;
static void DoDateSelectPopup(DateTimeType* frm_date, Char* date_str);
static void ReleaseLockedResource(MemHandle* h) SUTIL1;
static void DoSignalDialog(void) SUTIL2;
static void SetTimeForAlarmDetails(FormType* frm, Char** time_minutes, Char* time_minutesP[5],
				   DateTimeType* time, Boolean redraw);
static void MoveDateForward(DateTimeType* frm_date, Int32 x);
static Boolean SignalEvent(EventPtr e) SUTIL2;
static void UpdateCustomSignal(Boolean) SUTIL2;
static Boolean SnoozeEvent(EventPtr e);
static Boolean HandleListSelectEvent(Char* date_str, DateTimeType* frm_dateP,
				     Char** time_minutes, Char* time_minutesP[5], EventType* e);
static Boolean HandlePopSelectEvent(Char* date_str, DateTimeType* frm_dateP, EventType* e);
static void HandleNowButton(DateTimeType* frm_dateP, Char** time_minutes, Char* time_minutesP[5]);
static void HandleHoursLists(UInt16 id1, UInt16 id2, UInt16 id3,
			     DateTimeType* frm_dateP, UInt16 listID);

/*
** SetAlarmCheckbox
*/
extern inline void SetAlarmCheckbox(void) {
  CtlSetValue(GetObjectPointer(FrmGetActiveForm(), TimeSetCheck), true);
}

/*
** Handle clicks on the hours lists (either am/pm or 24h versions).
*/
static void HandleHoursLists(UInt16 id1, UInt16 id2, UInt16 id3,
			     DateTimeType* frm_dateP, UInt16 listID) {
  FormType* frm = FrmGetActiveForm();
  ListType* list = NULL;
  UInt16 i = id1;

  SetAlarmCheckbox();

  /* Unselect the other "hours" lists */
  for (; i <= id3; ++i) {
    list = GetObjectPointer(frm, i);

    if (listID == i) {
      frm_dateP->hour = LstGetSelection(list);

      if (i == id1) {
	continue;
      } else if (i == id2) {
	frm_dateP->hour += 6;
	continue;
      } else if (id3) {
	frm_dateP->hour += 18;
	continue;
      }
    }

    LstSetSelection(list, -1);
  }
}

/*
** HandleListSelectEvent - handles lstSelectEvent for TimeEvent and Snooze Event
*/
static Boolean HandleListSelectEvent(Char* date_str, DateTimeType* frm_dateP,
				     Char** time_minutes, Char* time_minutesP[5],
				     EventType* e) {
  FormType* frm = FrmGetActiveForm();
  ListType* list = NULL;
  UInt16 i = 0;
  Boolean handled = true;

  switch (e->data.lstSelect.listID) {
  case TimeMinutesList:
    {
      Char tmp[maxStrIToALen];
      Char* str = LstGetSelectionText(e->data.lstSelect.pList, e->data.lstSelect.selection);
      const Int32 base = StrAToI(str);

      
      for (; i < 5; ++i) {
	StrIToA(tmp, base + i);
	StrNCopy((Char*) time_minutes + i * 3, tmp, 3);  /* equiv. to Char[5][3] time_minutes[i] */
	time_minutesP[i] = (Char*) time_minutes + i * 3; /* equiv. to Char[5][3] time_minutes[i] */
      }

      list = GetObjectPointer(frm, TimeSmallMinutesList);
      LstSetListChoices(list, time_minutesP, 5);
      LstSetSelection(list, 0);
      LstDrawList(list);
    }
    /* fall through */
  case TimeSmallMinutesList:
    SetAlarmCheckbox();
    list = GetObjectPointer(frm, TimeSmallMinutesList);
    frm_dateP->minute = StrAToI(LstGetSelectionText(list, LstGetSelection(list)));;
    break;

  case TimeMorningHoursList:
  case TimeDayHoursList:
  case TimeNightHoursList:
    HandleHoursLists(TimeMorningHoursList, TimeDayHoursList, TimeNightHoursList,
		     frm_dateP, e->data.lstSelect.listID);
    break;

  case TimeMorningHours24List:
  case TimeDayHours24List:
  case TimeNightHours24List:
    HandleHoursLists(TimeMorningHours24List, TimeDayHours24List, TimeNightHours24List,
		     frm_dateP, e->data.lstSelect.listID);
    break;

  default:
    handled = false;
  }

  /* Change the day to tomorrow if the alarm time < current time && day == today */
  if (TimGetSeconds() > TimDateTimeToSeconds(frm_dateP)) {
    const UInt16 idx = FrmGetObjectIndex(frm, TimeDatePop);

    /* Adjust date */
    MoveDateForward(frm_dateP, 1);

    /* Update selector */
    DateToAscii(frm_dateP->month, frm_dateP->day, frm_dateP->year,
		PrefGetPreference(prefLongDateFormat), date_str);
    CtlSetLabel(FrmGetObjectPtr(frm, idx), date_str);
    CtlDrawControl(FrmGetObjectPtr(frm, idx));

    SndPlaySystemSound(sndClick);
  }

  return handled;
}

/*
** Handle popSelectEvent (actually: TimeDatePop)
*/
static Boolean HandlePopSelectEvent(Char* date_str, DateTimeType* frm_dateP, EventType* e) {
  DateTimeType dt;
  FormType* frm = FrmGetActiveForm();
  const UInt16 idx = FrmGetObjectIndex(frm, TimeDatePop);
  Boolean handled = false;

  switch (e->data.popSelect.controlID) {
  case TimeDatePop:
    switch (e->data.popSelect.selection) {
    case ppToday:
      MoveDateForward(frm_dateP, 0);
      break;
    case ppTomorrow:
      MoveDateForward(frm_dateP, 1);
      break;
    case ppDayAfterTomorrow:
      MoveDateForward(frm_dateP, 2);
      break;
    case ppThisFriday:
      TimSecondsToDateTime(TimGetSeconds(), &dt);
      if (dt.weekDay <= 5)
	MoveDateForward(frm_dateP, 5 - dt.weekDay);
      else if (dt.weekDay > 5)
	MoveDateForward(frm_dateP, 6 - dt.weekDay);
      break;
    case ppNextMonday:
      TimSecondsToDateTime(TimGetSeconds(), &dt);
      if (dt.weekDay < 1)
	MoveDateForward(frm_dateP, 1);
      else if (dt.weekDay >= 1)
	MoveDateForward(frm_dateP, 8 - dt.weekDay);
      break;
    case ppNextWeek:
      MoveDateForward(frm_dateP, 7);
      break;
    case ppNextMonth:
      TimSecondsToDateTime(TimGetSeconds(), &dt);
      frm_dateP->month = dt.month == 12 ? 1 : dt.month + 1;
      frm_dateP->day = 1;
      frm_dateP->year = frm_dateP->month == 1 ? dt.year + 1 : dt.year;
      break;
    case ppPickDate:
      DoDateSelectPopup(frm_dateP, date_str);
      break;
    default:
      /* ignore all else */
    }

    DateToAscii(frm_dateP->month, frm_dateP->day, frm_dateP->year,
		PrefGetPreference(prefLongDateFormat), date_str);
    CtlSetLabel(FrmGetObjectPtr(frm, idx), date_str);
    CtlDrawControl(FrmGetObjectPtr(frm, idx));

    SndPlaySystemSound(sndClick);
    SetAlarmCheckbox();

    handled = true;
  }

  return handled;
}

/*
** Updates the "custom signal" checkbox in the alarm details form
** and sets the correct label for the "signal" trigger.
*/
static void UpdateCustomSignal(Boolean drawLabel) {
  RectangleType r;
  FormType* frm = FrmGetActiveForm();
  const UInt16 idx = FrmGetObjectIndex(frm, TimeSignalTrig);
  ControlType* ctl = FrmGetObjectPtr(frm, idx);

  CtlSetValue(GetObjectPointer(frm, TimeCustomSignalCheck), d.tmp_custom_signal);

  /* Set correct label for signal trigger */
  InitMIDIList(NULL);
  LabelMIDITrigger(d.tmp_signal.listIndex, d.alarmText, sndMidiNameLength);

  if (d.tmp_led)
    StrNCat(d.alarmText, "/L", sndMidiNameLength + 2);

  if (d.tmp_vibrate)
    StrNCat(d.alarmText, "/V", sndMidiNameLength + 4);

  FrmGetObjectBounds(frm, idx, &r);
  TxtGlueTruncateString(d.alarmText, r.extent.x - 2);
  CtlSetLabel(ctl, d.alarmText);
  if (drawLabel)
    CtlEraseControl(ctl);
  FrmSetObjectBounds(frm, idx, &r);

  if (drawLabel)
    CtlDrawControl(ctl);
}

/*
** SignalEvent - event handler for signal form
*/
static Boolean SignalEvent(EventPtr e) {
  Boolean handled = false;
  FormPtr frm = FrmGetActiveForm();

  switch (e->eType) {
  case popSelectEvent:
    switch (e->data.popSelect.controlID) {
    case SignalPop:
      if (d.midi_count > 0 && e->data.popSelect.selection >= 0) {
	DmOpenRef midiDB = NULL;
	UInt8* midiData = NULL;
	UInt16 idx = 0;
	SndSmfOptionsType options = { 0, sndSmfPlayAllMilliSec, sndMaxAmp, true, 0, 0 };
	SndMidiListItemType* midi = (SndMidiListItemType*) (MemHandleLock(d.midi_list))
	  + e->data.popSelect.selection;

	LabelMIDITrigger(e->data.popSelect.selection, d.midi_label, sndMidiNameLength);
	TruncatePopupTriggerText(frm, SignalPop, d.midi_label);
	CtlSetLabel(e->data.popSelect.controlP, d.midi_label);

	/* Play the sound */
	midiDB = DmOpenDatabase(midi->cardNo, midi->dbID, dmModeReadOnly);
	if (!midiDB) abort();
	if (DmFindRecordByID(midiDB, midi->uniqueRecID, &idx)) abort();
	midiData = MemHandleLock(DmQueryRecord(midiDB, idx));
	options.amplitude = PrefGetPreference(prefAlarmSoundVolume);
	SndPlaySmf(NULL, sndSmfCmdPlay, midiData, &options, NULL, NULL, true);

	/* Clean up */
	MemHandleUnlock(d.midi_list);
	MemPtrUnlock(midiData);
	DmCloseDatabase(midiDB);
      }
      handled = true;
      break;

    default:
      break;
    }

  default:
    break;
  }

  return handled;
}

/*
** MoveDateForward - moves the alarm date to TODAY + x
*/
static void MoveDateForward(DateTimeType* frm_date, Int32 x) {
  DateType date;

  DateSecondsToDate(TimGetSeconds(), &date);
  DateAdjust(&date, x);

  frm_date->month = date.month;
  frm_date->day = date.day;
  frm_date->year = date.year + firstYear;
}

/*
** DoRepeatDialog - pop up the repeat details dialog from the time details form
*/
static void DoRepeatDialog(void) {
  FormType* frm = FrmInitForm(RepeatForm);

  ASSERT(frm);

  /* Init temporary data */
  d.repeat_start_of_week = sunday;

  /* Init form */
  RepeatInit(frm);
  FrmDrawForm(frm);
  if (d.tmp_repeat.repeatType != repeatNone)
    FrmSetFocus(frm, FrmGetObjectIndex(frm, RepeatFrequenceField));
  RepeatDrawDescription(frm);

  /* Popup form */
  FrmSetEventHandler(frm, RepeatHandleEvent);
  if (FrmDoDialog(frm) == RepeatOkButton) {
    RectangleType r;
    FormType* timeF = FrmGetFormPtr(TimeForm);
    const UInt16 idx = FrmGetObjectIndex(timeF, TimeRepeatTrig);
    ControlType* ctl = FrmGetObjectPtr(timeF, idx);

    /* RepeatApply() called in event handler */

    /* The label of the repeat trigger is locked - unlock it. */
    ReleaseLockedResource(&d.repeatTriggerHandle);

    /* Set new label */
    FrmGetObjectBounds(timeF, idx, &r);
    d.repeatTriggerHandle = DmGetResource(strRsc, repeatTypesNoneString + d.tmp_repeat.repeatType);
    CtlSetLabel(ctl, MemHandleLock(d.repeatTriggerHandle));
    FrmSetObjectBounds(timeF, idx, &r);
    CtlDrawControl(ctl);
  }

  /* Clean up */
  FrmDeleteForm(frm);
}

/*
** Handle presses on the "Now" button
*/
static void HandleNowButton(DateTimeType* frm_dateP, Char** time_minutes, Char* time_minutesP[5]) {
  DateTimeType time;

  TimSecondsToDateTime(TimGetSeconds(), &time);
  SetTimeForAlarmDetails(FrmGetActiveForm(), time_minutes, time_minutesP, &time, true);

  /* actually save time */
  frm_dateP->hour = time.hour;
  frm_dateP->minute = time.minute;
  frm_dateP->second = time.second;
}

/*
** TimeEvent - handle events on the time details form
*/
Boolean TimeEvent(EventPtr e) {
  Boolean handled = false;

  switch (e->eType) {

  case frmOpenEvent:
    LoadDateData();
    FrmDrawForm(FrmGetActiveForm());
    handled = true;
    break;

  case appStopEvent:
  case frmCloseEvent:
    /* The label of the repeat trigger is locked - unlock it. */
    ReleaseLockedResource(&d.repeatTriggerHandle);
    break;

/*   case frmUpdateEvent: */
/*     break; */

  case frmGotoEvent:
    /* The label of the repeat trigger is locked - unlock it. */
    ReleaseLockedResource(&d.repeatTriggerHandle);

    p.dbI = e->data.frmGoto.recordNum;
    SwitchCategoryForGoto();
    FrmGotoForm(p.flags & PFLAGS_WITH_TITLEBAR ? DiddleTForm : DiddleForm);
    e->eType = dbOpenRecordFieldEvent;
    EvtAddEventToQueue(e);
    handled = true;
    break;

  case ctlSelectEvent:
    switch (e->data.ctlEnter.controlID) {
    case TimeOkButton:
      SaveDateData();
      if (p.formID == DiddleThumbnailDetailForm)
	p.dbI = noRecordSelected;
      FrmGotoForm(p.formID);
      handled = true;
      break;
    case TimeCancelButton:
      FrmGotoForm(p.formID);
      handled = true;
      break;
    case TimeSignalTrig:
      DoSignalDialog();
      handled = true;
      break;
    case TimeRepeatTrig:
      DoRepeatDialog();
      handled = true;
      break;
    case TimeNowButton:
      HandleNowButton(&d.frm_date, (Char**)d.time_minutes, d.time_minutesP);
      handled = true;
      break;
    }
    break;

  case popSelectEvent:
    handled = HandlePopSelectEvent(d.date_str, &d.frm_date, e);
    break;

  case lstSelectEvent:
    handled = HandleListSelectEvent(d.date_str, &d.frm_date, (Char**)d.time_minutes,
				    d.time_minutesP, e);
    break;

  default:
    /* do nothing */
  }

  return handled;
}


/*
** LoadDateData - preset the Alarm Details form with current data
*/
static void LoadDateData(void) {
  FormPtr frm = FrmGetActiveForm();
  ControlPtr ctl = NULL;
  UInt32 time_secs = 0;
  RectangleType r;
  const UInt16 idx = FrmGetObjectIndex(frm, TimeRepeatTrig);

  ctl = GetObjectPointer(frm, TimeSetCheck);
  if (recordIsAlarmSet) {
    /* Set the Alarm Set checkbox */
    CtlSetValue(ctl, true);
    /* Get the alarm time */
    time_secs = ApptGetAlarmTime(&d.record, TimGetSeconds());
  }
  else {
    /* Set the Alarm Set checkbox */
    CtlSetValue(ctl, false);
    /* Get the current time */
    time_secs = TimGetSeconds();
  }
  /* Get a DateTimeType from the seconds */
  TimSecondsToDateTime(time_secs, &d.frm_date);

  if (!recordIsAlarmSet) {
    const UInt16 extra_time = d.frm_date.minute % 5;
    
    /* Truncate to the nearest 5 minutes */
    d.frm_date.second = 0;
    d.frm_date.minute = d.frm_date.minute - extra_time;
    
    /* Then increment by 5 minutes */
    if (extra_time) 
      TimAdjust(&d.frm_date, 5 * minutesInSeconds);
  }

  /* Label the explicit date pushbutton */
  DateToAscii(d.frm_date.month, d.frm_date.day, d.frm_date.year,
	      PrefGetPreference(prefLongDateFormat), d.date_str);
  CtlSetLabel(FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, TimeDatePop)), 
	      d.date_str);

  /* Initialize temporary repeat info */
  d.tmp_repeat = d.record.repeatInfo;

  /* Set correct label for repeat trigger */
  FrmGetObjectBounds(frm, idx, &r);
  d.repeatTriggerHandle = DmGetResource(strRsc, repeatTypesNoneString + d.tmp_repeat.repeatType);
  CtlSetLabel(FrmGetObjectPtr(frm, idx), MemHandleLock(d.repeatTriggerHandle));
  FrmSetObjectBounds(frm, idx, &r);

  /* Set up temporary info for signal dialog */
  d.tmp_signal = d.record_sound;
  d.tmp_led = recordIsLEDSet;
  d.tmp_vibrate = recordIsVibrateSet;
  d.tmp_custom_signal = recordIsCustomSignalSet;

  SetTimeForAlarmDetails(frm, (Char**)d.time_minutes, d.time_minutesP, &d.frm_date, false);
  UpdateCustomSignal(false);
}

/*
** SaveDateData - Save all data from the date/time selection dialog
*/
static void SaveDateData(void) {
  FormPtr frm = FrmGetActiveForm();
  ControlPtr ctl;

  /* Get whether or not the alarm is set */
  ctl = GetObjectPointer(frm, TimeSetCheck);
  if (CtlGetValue(ctl)) recordSetAlarm();
  else recordUnsetAlarm();
  /* This should already be set right, but just to make sure... */
  recordUnsetCountdown();
  /* Set the repeat mode */
  d.record.repeatInfo = d.tmp_repeat;

  d.frm_date.second = 0;
  d.record.alarmSecs = TimDateTimeToSeconds(&(d.frm_date));
  d.record.snoozeSecs = 0;
  if (d.record.alarmSecs < TimGetSeconds()) {
    d.record.alarmSecs = 0;
    recordUnsetAlarm();
    FrmAlert(AlarmTimeTooEarly);
  }

  /* Set custom signal information from temporaries */
  d.record_sound = d.tmp_signal;
  if (d.tmp_led) 
    recordSetLED();
  else
    recordUnsetLED();
  if (d.tmp_vibrate) 
    recordSetVibrate();
  else 
    recordUnsetVibrate();

  /* Should we use a custom signal at all? */
  ctl = GetObjectPointer(frm, TimeCustomSignalCheck);
  if (CtlGetValue(ctl)) 
    recordSetCustomSignal();
  else
    recordUnsetCustomSignal();

  /* Write to disk */
  FlushToDisk(false); /* just the header stuff */

  /* Reset the alarm */
  RescheduleAlarms(d.dbR);
}

/*
** DoDateSelectPopup - Pop winUp the dialog to specify the data for the alarm
*/
static void DoDateSelectPopup(DateTimeType* frm_date, Char* date_str) {
  FormPtr frm = FrmGetActiveForm();
  ControlType* ctl = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, TimeDatePop));
  Char titleStr[32];
  
  /* Pop winUp the form and get the date */
  SysCopyStringResource(titleStr, SelectDateString);
  if (SelectDay(selectDayByDay, &(frm_date->month), &(frm_date->day),
		&(frm_date->year), titleStr)) {

    /* Set the pushbutton value */
    DateToAscii(frm_date->month, frm_date->day, frm_date->year,
		PrefGetPreference(prefLongDateFormat), date_str);
    CtlSetLabel(ctl, date_str);
    CtlDrawControl(ctl);

    SndPlaySystemSound(sndClick);
    SetAlarmCheckbox();
  }
}

/*
** Unlock and release locked resource handle.
*/
static void ReleaseLockedResource(MemHandle* h) {
  if (*h) {
    MemHandleUnlock(*h);
    DmReleaseResource(*h);
    *h = NULL;
  }
}

/*
** DoSignalDialog - pop up the signal selection dialog from the time details form
*/
static void DoSignalDialog(void) {
  Boolean custom = false;
  FormType* frm = FrmInitForm(SignalForm);

  ASSERT(frm);

  if (!d.tmp_custom_signal) {
    d.tmp_signal = p.std_alarm;
    d.tmp_led = CHECK_FLAG(PFLAGS_DEFAULT_LED);
    d.tmp_vibrate = CHECK_FLAG(PFLAGS_DEFAULT_VIBRATE);
  }

  /* Load data */
  SetupSignalDialog(frm, d.tmp_led, d.tmp_vibrate, d.tmp_signal.listIndex);

  /* Popup form */
  FrmSetEventHandler(frm, SignalEvent);
  if (FrmDoDialog(frm) == SignalOKButton) {
    ControlType* ctl = NULL;
    const Int16 i = LstGetSelection(GetObjectPointer(frm, SignalList));

    ASSERT(i != -1);

    if (d.midi_count > 0) {
      SndMidiListItemType* midi = MemHandleLock(d.midi_list);
      d.tmp_signal.uniqueRecID = midi[i].uniqueRecID;
      d.tmp_signal.dbID = midi[i].dbID;
      d.tmp_signal.cardNo = midi[i].cardNo;
      d.tmp_signal.listIndex = i;
      MemHandleUnlock(d.midi_list);

      custom = true;
    }

    /* Set blinking LED flag if necessary */
    ctl = GetObjectPointer(frm, LEDCheck);
    d.tmp_led = CtlEnabled(ctl) && CtlGetValue(ctl);
    custom = custom || CtlEnabled(ctl);

    /* Set vibrate flag if necessary */
    ctl = GetObjectPointer(frm, VibrateCheck);
    d.tmp_vibrate = CtlEnabled(ctl) && CtlGetValue(ctl);
    custom = custom || CtlEnabled(ctl);

    /* Set "custom signal" flag for record */
    d.tmp_custom_signal = custom;
  } else {
    /* Restore "-None-" value */
    d.tmp_signal = d.record_sound;
    d.tmp_led = recordIsLEDSet;
    d.tmp_vibrate = recordIsVibrateSet;
  }

  /* Clean up */
  FrmDeleteForm(frm);

  /* Update alarm detail form */
  UpdateCustomSignal(true);
}

/*
** SetTimeForAlarmDetails - set the Alarm Details form with a certain time
*/
static void SetTimeForAlarmDetails(FormType* frm, Char** time_minutes, Char* time_minutesP[5],
				   DateTimeType* time, Boolean redraw) {
  ListPtr list1, list2, list3;
  UInt16 idx1, idx2, idx3;
  UInt16 other1, other2, other3;
  TimeFormatType format = PrefGetPreference(prefTimeFormat);

  /* Set the hour list */
  if (format != tfColon24h && format != tfDot24h && format != tfHours24h && format != tfComma24h) {
    idx1 = FrmGetObjectIndex(frm, TimeMorningHoursList);
    idx2 = FrmGetObjectIndex(frm, TimeDayHoursList);
    idx3 = FrmGetObjectIndex(frm, TimeNightHoursList);

    other1 = FrmGetObjectIndex(frm, TimeMorningHours24List);
    other2 = FrmGetObjectIndex(frm, TimeDayHours24List);
    other3 = FrmGetObjectIndex(frm, TimeNightHours24List);
  } else {
    idx1 = FrmGetObjectIndex(frm, TimeMorningHours24List);
    idx2 = FrmGetObjectIndex(frm, TimeDayHours24List);
    idx3 = FrmGetObjectIndex(frm, TimeNightHours24List);

    other1 = FrmGetObjectIndex(frm, TimeMorningHoursList);
    other2 = FrmGetObjectIndex(frm, TimeDayHoursList);
    other3 = FrmGetObjectIndex(frm, TimeNightHoursList);
  }

  /* Show and hide the correct lists */
  FrmHideObject(frm, other1);
  FrmHideObject(frm, other2);
  FrmHideObject(frm, other3);
  FrmShowObject(frm, idx1);
  FrmShowObject(frm, idx2);
  FrmShowObject(frm, idx3);

  /* Get pointer to hour lists for further setup */
  list1 = FrmGetObjectPtr(frm, idx1);
  list2 = FrmGetObjectPtr(frm, idx2);
  list3 = FrmGetObjectPtr(frm, idx3);

  if (time->hour < 6) {
    LstSetSelection(list1, time->hour);
    LstSetSelection(list2, -1);
    LstSetSelection(list3, -1);
  } else if (time->hour < 18) {
    LstSetSelection(list1, -1);
    LstSetSelection(list2, time->hour - 6);
    LstSetSelection(list3, -1);
  } else {
    LstSetSelection(list1, -1);
    LstSetSelection(list2, -1);
    LstSetSelection(list3, time->hour - 18);
  }

  if (redraw) {
    LstDrawList(list1);
    LstDrawList(list2);
    LstDrawList(list3);
  }

  /* Set the minutes list */
  list1 = GetObjectPointer(frm, TimeMinutesList);
  list2 = GetObjectPointer(frm, TimeSmallMinutesList);
  LstSetSelection(list1, time->minute/5);
  {
    UInt16 i = 0;
    const Int32 base = StrAToI(LstGetSelectionText(list1, LstGetSelection(list1)));
    Char tmp[maxStrIToALen];

    for (; i < 5; ++i) {
      StrIToA(tmp, base + i);
      StrNCopy((Char*) time_minutes + i * 3, tmp, 3);  /* equiv. to Char[5][3] time_minutes[i] */
      time_minutesP[i] = (Char*) time_minutes + i * 3; /* equiv. to Char[5][3] time_minutes[i] */
    }

    LstSetListChoices(list2, time_minutesP, 5);
  }
  LstSetSelection(list2, time->minute % 5);

  if (redraw) {
    LstDrawList(list1);
    LstDrawList(list2);
  }
}

/*
** SnoozeEvent - handle events on the absolute snooze form
*/
static Boolean SnoozeEvent(EventType* e) {
  Boolean handled = false;
  DateTimeType* frm_dateP = NULL;
  Char** time_minutes = NULL;
  Char** time_minutesP = NULL;
  Char* date_str = NULL;
  Err err = errNone;

  /* Load frm_date & other buffers*/
  err = FtrGet(AppType, DB_FEATURE_TIME_FRM_DATE, (UInt32*) &frm_dateP);
  if (err) abort();
  err = FtrGet(AppType, DB_FEATURE_TIME_MINUTES, (UInt32*) &time_minutes);
  if (err) abort();
  err = FtrGet(AppType, DB_FEATURE_TIME_MINUTES_P, (UInt32*) &time_minutesP);
  if (err) abort();
  err = FtrGet(AppType, DB_FEATURE_TIME_DATE_STRING, (UInt32*) &date_str);
  if (err) abort();

  switch (e->eType) {

  case appStopEvent:
  case frmCloseEvent:
    break;

  case frmGotoEvent:
    handled = true;
    break;

  case ctlSelectEvent:
    switch (e->data.ctlEnter.controlID) {
    case TimeOkButton:
/*       SaveDateData(); */
/*       FrmGotoForm(p.formID); */
/*       handled = true; */
      break;
    case TimeCancelButton:
/*       FrmGotoForm(p.formID); */
/*       handled = true; */
      break;
    case TimeSignalTrig:
    case TimeRepeatTrig:
      /* Do nothing */
      handled = true;
      break;
    case TimeNowButton:
      HandleNowButton(frm_dateP, time_minutes, time_minutesP);
      handled = true;
      break;
    }
    break;

  case popSelectEvent:
    handled = HandlePopSelectEvent(date_str, frm_dateP, e);
    break;

  case lstSelectEvent:
    handled = HandleListSelectEvent(date_str, frm_dateP, time_minutes, time_minutesP, e);
    break;

  default:
    /* do nothing */
  }

  return handled;
}

/*
** DoAbsoluteSnoozeDialog - popup modified TimeForm to select an
**                          absolute value for "Snooze".
*/
UInt32 DoAbsoluteSnoozeDialog(void) {
  FormType* frm = FrmInitForm(TimeForm);
  MemHandle titleH = NULL;
  RectangleType r;
  UInt16 idx;
  Coord y;
  DateTimeType time;
  Int32 secs = 0;
  Char time_minutes[5][3];
  Char* time_minutesP[5];
  Char date_str[15];

  ASSERT(frm);

  /* Initialize Char buffers */
  MemSet(time_minutes, sizeof(time_minutes), 0);
  MemSet(time_minutesP, sizeof(time_minutesP), 0);
  MemSet(date_str, sizeof(date_str), 0);

  /* Hide unused objects */
  FrmHideObject(frm, FrmGetObjectIndex(frm, TimeSetCheck));
  FrmHideObject(frm, FrmGetObjectIndex(frm, TimeRepeatTrig));
  FrmHideObject(frm, FrmGetObjectIndex(frm, TimeSignalTrig));
  FrmHideObject(frm, FrmGetObjectIndex(frm, TimeCustomSignalCheck));
  FrmHideObject(frm, FrmGetObjectIndex(frm, TimeRepeatLabel));
  FrmHideObject(frm, FrmGetObjectIndex(frm, TimeAlarmAtLabel));
  FrmHideObject(frm, FrmGetObjectIndex(frm, TimeCustomSignalLabel));

  /* Show some important labels */
  idx = FrmGetObjectIndex(frm, TimeSnoozeUntilLabel);
  FrmShowObject(frm, idx);

  /* Move some objects */
  FrmGetObjectBounds(frm, idx, &r);
  y = r.topLeft.y + r.extent.y;
  idx = FrmGetObjectIndex(frm, TimeDatePop);
  FrmSetObjectPosition(frm, idx, r.topLeft.x, y);
  FrmSetObjectPosition(frm, FrmGetObjectIndex(frm, TimeDateList), r.topLeft.x, y);

  /* Set a new title... */
  titleH = DmGetResource(strRsc, TimeSnoozeTitleString);
  FrmSetTitle(frm, MemHandleLock(titleH));

  /* ...and a help text */
  FrmGlueSetHelpID(frm, TimeSnoozeHelpString);

  /* Set initial time */
  TimSecondsToDateTime(TimGetSeconds(), &time);
  SetTimeForAlarmDetails(frm, (Char**)time_minutes, time_minutesP, &time, false);

  /* Set initial date */
  DateToAscii(time.month, time.day, time.year, PrefGetPreference(prefLongDateFormat), date_str);
  CtlSetLabel(FrmGetObjectPtr(frm, idx), date_str);

  /* Draw form */
  FrmDrawForm (frm);

  /* Allocate frm_date & other data */
  if (FtrSet(AppType, DB_FEATURE_TIME_FRM_DATE, (UInt32) &time))
    abort(); /* fatal error */
  if (FtrSet(AppType, DB_FEATURE_TIME_MINUTES, (UInt32) time_minutes))
    abort(); /* fatal error */
  if (FtrSet(AppType, DB_FEATURE_TIME_MINUTES_P, (UInt32) time_minutesP))
    abort(); /* fatal error */
  if (FtrSet(AppType, DB_FEATURE_TIME_DATE_STRING, (UInt32) date_str))
    abort(); /* fatal error */

  /* Pop-up form */
  FrmSetEventHandler(frm, SnoozeEvent);
  if (FrmDoDialog(frm) == TimeOkButton) {
    /* Get seconds */
    time.second = 0;
    secs = TimDateTimeToSeconds(&time) - TimGetSeconds();

    if (secs <= 0) {
      secs = 0;
      FrmAlert(SnoozeTimeTooEarlyAlert);
    }
  }

  /* Clean up */
  MemHandleUnlock(titleH);
  DmReleaseResource(titleH);
  FtrUnregister(AppType, DB_FEATURE_TIME_FRM_DATE);
  FtrUnregister(AppType, DB_FEATURE_TIME_MINUTES);
  FtrUnregister(AppType, DB_FEATURE_TIME_MINUTES_P);
  FrmDeleteForm(frm);

  return secs;
}
