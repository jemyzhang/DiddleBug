/*

(c) 2001, 2002, 2003, 2004 Peter Putzer
Licensed under the GNU General Public License (GPL).

Portions copyright (c) 2000 Palm, Inc. or its subsidiaries.  All rights reserved.

*/

#include <PalmOS.h>

#include "repeat.h"
#include "globals.h"
#include "diddlebugRsc.h"
#include "diddlebug.h"
#include "diddlebugInlines.h"
#include "debug.h"

/* static functions */
static void RepeatSetDateTrigger (DateType endDate) SUTIL2;
static void RepeatGetUIValues (FormType* frm, RepeatInfoType* repeatP) SUTIL2;
static void RepeatSetUIValues (FormType* frm, RepeatInfoType* repeatP) SUTIL2;
static void RepeatSelectEndDate (EventType* event) SUTIL2;
static void RepeatChangeRepeatOn (EventType* event) SUTIL2;
static void RepeatChangeType (EventType* event) SUTIL2;
static void RepeatApply (void) SUTIL2;
static Boolean RepeatDescRectHandler(FormGadgetTypeInCallback *gadgetP, UInt16 cmd, void *paramP) SUTIL2;

#define RepeatTypeGroup                          1
#define RepeatByGroup                            2
#define maxFrequenceFieldLen                     3
#define defaultRepeatDecsLen                    32
#define maxAdvanceFieldLen                       3
/* Repeat dialog "end on" popup list items. */
#define repeatNoEndDateItem                      0
#define repeatChooseDateItem                     1
/* Default setting for the Repeating Events Dialog. */
#define defaultRepeatFrequency                   1
#define defaultRepeatEndDate                    -1
/* End Date popup list chooses */
#define noEndDateItem                            0
#define selectEndDateItem                        1
/* Token strings in repeating event description template. */
#define frequenceToken                           "^f"
#define dayOrdinalToken                          "^x"
#define weekOrdinalToken                         "^w"
#define monthNameToken                           "^m"
#define dayNameToken                             "^d"

/***********************************************************************
 *
 * FUNCTION:    SubstituteStr
 *
 * DESCRIPTION: This routine substitutes the occurrence a token, within
 *              a string, with another string.
 *
 * PARAMETERS:  str    - string containing token string
 *              token  - the string to be replaced
 *              sub    - the string to substitute for the token
 *              subLen - length of the substitute string.
 *
 * RETURNED:    pointer to the string
 *
 ***********************************************************************/
static Char* SubstituteStr(Char* str, const Char* token, const Char* sub, UInt16 subLen) {
  const UInt16 tokenLen = StrLen(token);
  const UInt16 charsToMove = subLen - tokenLen;
  const UInt16 strLen = StrLen(str);
  MemHandle strH = MemPtrRecoverHandle(str);
  const UInt16 blockSize = MemHandleSize(strH);
  Char* ptr = StrStr(str, token);

  ASSERT(str);
  ASSERT(token);
  ASSERT(sub);

  /* Find the start of the token string, if it doesn't exist, exit. */
  if (ptr == NULL)
    return str;

  /* Resize the string if necessary. */
  if (strLen + charsToMove + 1 >= blockSize) {
    MemHandleUnlock(strH);
    MemHandleResize(strH, strLen + charsToMove + 1);
    str = MemHandleLock(strH);
    ASSERT(str);
    ptr = StrStr(str, token);
    ASSERT(ptr);
  }

  /* Make room for the substitute string. */
  if (charsToMove)
    MemMove(ptr + subLen, ptr + tokenLen, StrLen (ptr + tokenLen)+1);
  
  /* Replace the token with the substitute string. */
  MemMove(ptr, sub, subLen);
  
  return str;
}

/***********************************************************************
 *
 * FUNCTION:    RepeatSetDateTrigger
 *
 * DESCRIPTION: This routine sets the label of the trigger that displays
 *              the end date of a repeating appointment.
 *
 * PARAMETERS:  endDate	- date or -1 if no end date
 *
 * RETURNED:    nothing
 *
 * NOTES:
 *      This routine assumes that the memory allocated for the label of
 *      the due date trigger is large enough to hold the lagest posible
 *      label. This label's memory is reserved by initializing the label
 *      in the resource file.
 *
 ***********************************************************************/
static void RepeatSetDateTrigger (DateType endDate) {
  FormType* frm = FrmGetFormPtr(RepeatForm);
  ListType* lst = GetObjectPointer(frm, RepeatEndOnList);
  ControlType* ctl = GetObjectPointer(frm, RepeatEndOnTrigger);
  Char* label = (Char*)CtlGetLabel(ctl); /* OK to cast; we call CtlSetLabel */

  ASSERT(lst);
  ASSERT(ctl);
  ASSERT(label);

  if (DateToInt(endDate) == apptNoEndDate) {
    StrCopy(label, LstGetSelectionText(lst, repeatNoEndDateItem));
    LstSetSelection(lst, noEndDateItem);
  } else {
    /* Format the end date into a string. */
    DateToDOWDMFormat(endDate.month, endDate.day, endDate.year + firstYear,
		      PrefGetPreference(prefDateFormat), label);
    
    LstSetSelection(lst, repeatChooseDateItem);
  }
  
  CtlSetLabel(ctl, label);
}

/***********************************************************************
 *
 * FUNCTION:    RepeatGetUIValues
 *
 * DESCRIPTION: This routine gets the current repeat settings of the
 *              ui gadgets in the repeat dialog box
 *
 *
 * PARAMETERS:  frm     - pointer to the repeat dialog
 *              repeatP - RepeatInfoType structure (fill-in by this routine)
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
void RepeatGetUIValues (FormType* frm, RepeatInfoType* repeatP) {
  UInt16 freq;
  UInt16 i;
  UInt16 id;
  UInt16 index;
  Char* str = NULL;

  /* Get the repeat type. */
  index = FrmGetControlGroupSelection(frm, RepeatTypeGroup);
  id = FrmGetObjectId(frm, index);
  if (id == RepeatYearly) {
    repeatP->repeatType = repeatYearly;
  } else if (id <= RepeatWeekly) {
    repeatP->repeatType = (RepeatType) (id - RepeatNone);
  } else {
    index = FrmGetControlGroupSelection(frm, RepeatByGroup);
    id = FrmGetObjectId(frm, index);
    if (id == RepeatByDayPushButton)
      repeatP->repeatType = repeatMonthlyByDay;
    else
      repeatP->repeatType = repeatMonthlyByDate;
  }
  
  /* Get the repeat end date. */
  repeatP->repeatEndDate = d.repeat_end_date;

  /* Get the repeat frequency. */
  str = FldGetTextPtr(GetObjectPointer(frm, RepeatFrequenceField));
  if (str) 
    freq = StrAToI(str);
  else 
    freq = 0;

  if (freq)
    repeatP->repeatFrequency = freq;
  else
    repeatP->repeatFrequency = 1;

  /*
  ** Get the start day of week.  If the original repeat type, that is the
  ** repeat type when the dialog was first displayed, is weekly then
  ** use the start date in the repeat info (the unedit data), otherwise
  ** use the current start of week.
  */
  if (repeatP->repeatType == repeatWeekly) {
    if (d.tmp_repeat.repeatType == repeatWeekly)
      repeatP->repeatStartOfWeek = d.tmp_repeat.repeatStartOfWeek;
    else
      repeatP->repeatStartOfWeek = PrefGetPreference(prefWeekStartDay);
  } else {
    /* For all other repeat types, the repeatStartOfWeek field is meaningless. */
    repeatP->repeatStartOfWeek = 0;
  }

  /* If the repeat type is weekly, get the day of the week the event repeats on. */
  if (repeatP->repeatType == repeatWeekly) {
    repeatP->repeatOn = 0;
    index = FrmGetObjectIndex(frm, RepeatDayOfWeek1PushButton);
    for (i = 0; i < daysInWeek ; i++) {
      if (FrmGetControlValue(frm, 
			     index + ((i - d.repeat_start_of_week + daysInWeek) % daysInWeek)))
	repeatP->repeatOn |= (1 << i);
    }
  } else if (repeatP->repeatType == repeatMonthlyByDay) {
    /*
    ** If the repeat type is monthly by day, get the day of the month (ex:
    ** first Friday) of the start date of the event.
    */
    if (d.tmp_repeat.repeatType == repeatMonthlyByDay)
      repeatP->repeatOn = d.tmp_repeat.repeatOn;
    else
      repeatP->repeatOn = DayOfMonth (d.frm_date.month,
				      d.frm_date.day,
				      d.frm_date.year);
  } else {
    /* For all other repeat types, the repeatOn field is meaningless. */
    repeatP->repeatOn = 0;
  }
}

/***********************************************************************
 *
 * FUNCTION:    RepeatSetUIValues
 *
 * DESCRIPTION: This routine sets the current repeat settings of the
 *              ui gadgets in the repeat dialog box
 *
 * PARAMETERS:  frm     - pointer to the Repeat Dialog
 *              repeatP - pointer to a RepeatInfoType structure.
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
void RepeatSetUIValues(FormType* frm, RepeatInfoType* repeatP) {
  UInt16 i;
  UInt16 id;
  UInt16 oldFreq;
  MemHandle freqH;
  Char* freqP = NULL;
  Boolean on = false;
  FieldType* fld = NULL;
  const Int8 startOfWeek = PrefGetPreference(prefWeekStartDay);

  /* Set the selection of the "repeat type" push button group. */
  id = repeatP->repeatType + RepeatNone;
  if (repeatP->repeatType > repeatMonthlyByDay)
    id--;
  FrmSetControlGroupSelection (frm, RepeatTypeGroup, id);


  /* Set the frequency field */
  if (repeatP->repeatType != repeatNone) {
    fld = GetObjectPointer(frm, RepeatFrequenceField);
    freqH = FldGetTextHandle(fld);
    if (freqH) {
      freqP = MemHandleLock(freqH);
      oldFreq = StrAToI(freqP);
    } else {
      freqH = MemHandleNew(maxFrequenceFieldLen);
      ASSERT(freqH);
      freqP = MemHandleLock(freqH);
      oldFreq = 0;
    }

    if (oldFreq != repeatP->repeatFrequency) {
      StrIToA(freqP, repeatP->repeatFrequency);
      FldSetTextHandle(fld, freqH);
      if (FrmVisible(frm)) {
	FldEraseField(fld);
	FldDrawField(fld);
      }
    }
    MemHandleUnlock (freqH);
  }

  /* Set the selection of the "repeat on" push button groups. */
  if (repeatP->repeatType == repeatWeekly) {
    /*
    ** If the appointment has a different start-day-of-week than
    ** the dialog-box's current start-day-of-week, rearrange the
    ** labels on the days-of-week push buttons.
    ** Note that this will only handle button-label shifts of one day.
    */
    if (startOfWeek != d.repeat_start_of_week) {
      const Char* sundayLabel = CtlGetLabel(GetObjectPointer(frm, RepeatDayOfWeek1PushButton));
      for (id = RepeatDayOfWeek1PushButton; id < RepeatDayOfWeek7PushButton; id++)
	CtlSetLabel(GetObjectPointer(frm, id), CtlGetLabel(GetObjectPointer(frm, id + 1)));

      CtlSetLabel(GetObjectPointer(frm, RepeatDayOfWeek7PushButton), sundayLabel);
      d.repeat_start_of_week = startOfWeek;
    }

    /* Turn on the push buttons for the days the appointment repeats on. */
    for (i = 0; i < daysInWeek; i++) {
      on = ((repeatP->repeatOn & (1 << i) ) != 0);
      id = RepeatDayOfWeek1PushButton +
	((i - startOfWeek + daysInWeek) % daysInWeek);
      CtlSetValue(GetObjectPointer(frm, id), on);
    }
  }

  /* Set the selection of the "repeat by" push button groups. */
  if (repeatP->repeatType == repeatMonthlyByDate)
    FrmSetControlGroupSelection(frm, RepeatByGroup, RepeatByDatePushButton);
  else
    FrmSetControlGroupSelection(frm, RepeatByGroup, RepeatByDayPushButton);
  
  /* Set the "end on" trigger label and popup list selection. */
  if (repeatP->repeatType != repeatNone)
    RepeatSetDateTrigger(repeatP->repeatEndDate);
}

/***********************************************************************
 *
 * FUNCTION:    RepeatDrawDescription
 *
 * DESCRIPTION: This routine draws the text description of the current
 *              repeat type and frequency.
 *
 *              The description is created from a template string.  The
 *              repeat type and frequency determines which template is
 *              used.  The template may contain one or more of the
 *              following token:
 *                   ^d - day name (ex: Monday)
 *							^f - frequency
 *                   ^x - day of the month ordinal number (ex: 1st - 31th)
 *                   ^m - month name (ex: July)
 *                   ^w - week ordinal number (1st, 2nd, 3rd, 4th, or last)
 *
 * PARAMETERS:  frm - pointer to the repeat dialog box
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
void RepeatDrawDescription(FormType* frm) {
  UInt8 repeatOn;
  UInt16 i;
  UInt16 len;
  UInt16 freq;
  UInt16 dayOfWeek;
  UInt16 templateId = repeatNoneString;
  UInt16 repeatOnCount = 0;
  Char* descP = NULL;
  Char* resP = NULL;
  Char* saveResP = NULL;
  MemHandle descH = NULL;
  MemHandle resH = NULL;
  RepeatInfoType repeat;
  FieldType* fld = GetObjectPointer(frm, RepeatDescField);

  FldEraseField (fld);

  /* Get the current setting of the repeat ui gadgets. */
  RepeatGetUIValues(frm, &repeat);
  
  /*
  ** Determine which template string to use.  The template string is
  ** used to format the description string. Note that we could add
  ** a soft constant which tells us whether we need to use different
  ** strings for freq == 1 case (depends on language), thus saving space.
  */
  freq = repeat.repeatFrequency;
  switch (repeat.repeatType) {
  case repeatNone:
    templateId = repeatNoneString;
    break;
    
  case repeatHourly:
    if (freq == 1)
      /* "Every hour" */
      templateId = everyHourRepeatString;
    else
      /* "Every [other | 2nd | 3rd...] hour" */
      templateId = hourlyRepeatString;
    break;
    
  case repeatDaily:
    if (freq == 1)
      /* "Every day" */
      templateId = everyDayRepeatString;
    else
      /* "Every [other | 2nd | 3rd...] day" */
      templateId = dailyRepeatString;
    break;
    
  case repeatWeekly:
    if (freq == 1)
      /* "Every week on [days of week]" */
      templateId = everyWeekRepeat1DayString;
    else
      templateId = weeklyRepeat1DayString;
    
    /*
    ** Generate offset to appropriate string id,
    ** based on # of days that we need to append.
    */
    for (i = 0; i < daysInWeek; i++) {
      if (repeat.repeatOn & (1 << i) ) 
	repeatOnCount++;
    }
    templateId += repeatOnCount - 1;
    break;

  case repeatMonthlyByDate:
    if (freq == 1)
      /* "The ^w ^d of every month" */
      templateId = everyMonthByDateRepeatString;
    else
      templateId = monthlyByDateRepeatString;
    break;
    
  case repeatMonthlyByDay:
    if (freq == 1)
      templateId = everyMonthByDayRepeatString;
    else
      templateId = monthlyByDayRepeatString;
    break;
    
  case repeatYearly:
    if (freq == 1)
      templateId = everyYearRepeatString;
    else
      templateId = yearlyRepeatString;
    break;
    
  default:
    ErrNonFatalDisplay("Unknown repeat type");
    break;
  }

  /*
  ** Allocate a block to hold the description and copy the template
  ** string into it.
  */
  resH = DmGetResource(strRsc, templateId);
  resP = MemHandleLock(resH);
  descH = MemHandleNew(MemPtrSize(resP));
  ASSERT(descH);
  descP = MemHandleLock(descH);
  StrCopy(descP, resP);
  MemHandleUnlock(resH);
  
  /* Substitute the month name string for the month name token. */
  resH = DmGetResource(strRsc, repeatMonthNamesString);
  resP = MemHandleLock(resH);
  for (i = 1; i < d.frm_date.month; i++)
    resP = StrChr(resP, spaceChr) + 1;
  len = StrChr(resP, spaceChr) - resP;
  descP = SubstituteStr(descP, monthNameToken, resP, len);
  MemHandleUnlock(resH);

  /* Substitute the day name string for the day name token. */
  if ((repeatOnCount == 1) || (repeat.repeatType == repeatMonthlyByDay))
    templateId = repeatFullDOWNamesString;
  else
    templateId = repeatShortDOWNamesString;
  
  resH = DmGetResource(strRsc, templateId);
  resP = MemHandleLock(resH);
  if (repeat.repeatType == repeatWeekly) {
    dayOfWeek = repeat.repeatStartOfWeek;
    repeatOn = repeat.repeatOn;
    saveResP = resP;
    while (StrStr (descP, dayNameToken)) {
      for (i = 0; i < daysInWeek; i++) {
	if (repeatOn & (1 << dayOfWeek)) {
	  repeatOn &= ~(1 << dayOfWeek);
	  break;
	}
	dayOfWeek = (dayOfWeek + 1 + daysInWeek) % daysInWeek;
      }
      resP = saveResP;
      for (i = 0; i < dayOfWeek; i++)
	resP = StrChr(resP, spaceChr) + 1;
      
      len = StrChr(resP, spaceChr) - resP;
      descP = SubstituteStr(descP, dayNameToken, resP, len);
    }
  } else {
    dayOfWeek = DayOfWeek (d.frm_date.month, d.frm_date.day,
			   d.frm_date.year/* + firstYear*/);
    for (i = 0; i < dayOfWeek; i++)
      resP = StrChr(resP, spaceChr) + 1;
    len = StrChr(resP, spaceChr) - resP;
    descP = SubstituteStr(descP, dayNameToken, resP, len);
  }
  MemHandleUnlock (resH);

  /*
  ** Substitute the repeat frequency string for the frequency token. Note that
  ** we do something special for 2nd (other), since the gender of 'other' changes
  ** for some languages, depending on whether the next word is day, month, week,
  ** or year.
  */
  if (freq == 2) {
    Char otherFreqName[16];
    const UInt16 index = repeat.repeatType - repeatNone;
    SysStringByIndex(freqOrdinal2ndStrlID, index, otherFreqName, sizeof(otherFreqName));
    descP = SubstituteStr(descP, frequenceToken, otherFreqName, StrLen(otherFreqName));
  } else {
    resH = DmGetResource(strRsc, freqOrdinalsString);
    resP = MemHandleLock(resH);
    for (i = 1; i < freq; i++)
      resP = StrChr(resP, spaceChr) + 1;
    len = StrChr(resP, spaceChr) - resP;
    descP = SubstituteStr(descP, frequenceToken, resP, len);
    MemHandleUnlock(resH);
  }

  /*
  ** Substitute the repeat week string (1st, 2nd, 3rd, 4th, or last)
  ** for the week ordinal token.
  */
  if (repeat.repeatType == repeatMonthlyByDay) {
    resH = DmGetResource(strRsc, weekOrdinalsString);
    resP = MemHandleLock(resH);
    for (i = 0; i < repeat.repeatOn / daysInWeek; i++)
      resP = StrChr (resP, spaceChr) + 1;
    len = StrChr(resP, spaceChr) - resP;
    descP = SubstituteStr(descP, weekOrdinalToken, resP, len);
    MemHandleUnlock(resH);
  } else {
    /* make sure the week ordinal token really doesn't appear */
    ErrNonFatalDisplayIf(StrStr(descP, weekOrdinalToken) != NULL, "week ordinal not substituted");
  }

  /*
  ** Substitute the repeat date string (1st, 2nd, ..., 31th) for the
  ** day ordinal token.
  */
  resH = DmGetResource(strRsc, dayOrdinalsString);
  resP = MemHandleLock(resH);
  for (i = 1; i < d.frm_date.day; i++)
    resP = StrChr(resP, spaceChr) + 1;
  len = StrChr(resP, spaceChr) - resP;
  descP = SubstituteStr(descP, dayOrdinalToken, resP, len);
  MemHandleUnlock(resH);

  /* Draw the description. */
  MemHandleUnlock(descH);
  FldFreeMemory(fld);
  FldSetTextHandle(fld, descH);
  FldDrawField(fld);
}

/***********************************************************************
 *
 * FUNCTION:    RepeatSelectEndDate
 *
 * DESCRIPTION: This routine selects the end date of a repeating event.
 *
 * PARAMETERS:  event - pointer to a popup select event
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
static void RepeatSelectEndDate(EventType* event) {
  Int16 month, day, year;
  Char* titleP = NULL;
  MemHandle titleH = NULL;

  /* "No due date" items selected? */
  if (event->data.popSelect.selection == repeatNoEndDateItem)
    DateToInt(d.repeat_end_date) = apptNoEndDate;

  /* "Select date" item selected? */
  else if (event->data.popSelect.selection == repeatChooseDateItem) {
    if (DateToInt (d.repeat_end_date) == apptNoEndDate) {
      year = d.frm_date.year;
      month = d.frm_date.month;
      day = d.frm_date.day;
    } else {
      year = d.repeat_end_date.year + firstYear;
      month = d.repeat_end_date.month;
      day = d.repeat_end_date.day;
    }
    
    titleH = DmGetResource(strRsc, endDateTitleString);
    titleP = MemHandleLock(titleH);
    
    if (SelectDay (selectDayByDay, &month, &day, &year, titleP)) {
      d.repeat_end_date.day = day;
      d.repeat_end_date.month = month;
      d.repeat_end_date.year = year - firstYear;
      
      /* Make sure the end date is not before the start date. */
      if (DateToInt(d.repeat_end_date) < DateToInt (d.frm_date)) {
	SndPlaySystemSound (sndError);
	DateToInt (d.repeat_end_date) = apptNoEndDate;
      }
    }
    
    MemHandleUnlock(titleH);
  }

  RepeatSetDateTrigger(d.repeat_end_date);
}

/***********************************************************************
 *
 * FUNCTION:    RepeatChangeRepeatOn
 *
 * DESCRIPTION: This routine is called when one of the weekly "repeat on"
 *              push button is pushed.  This routine checks
 *              if all the buttons has been turned off,  if so the day
 *              of the week of the appointment's start date is turn on.
 *
 * PARAMETERS:  event - pointer to and event
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
static void RepeatChangeRepeatOn(EventType* event) {
  UInt16 id = 0;
  UInt16 dayOfWeek = 0;
  FormType* frm = FrmGetFormPtr(RepeatForm);
  Boolean on = false;
  UInt16 i = 0;
  const UInt16 idx = FrmGetObjectIndex(frm, RepeatDayOfWeek1PushButton);

  /* Check if any of the buttons are on. */
  for (; i < daysInWeek; i++) {
    if (FrmGetControlValue(frm, idx + i) != 0) {
      on = true;
      break;
    }
  }

  /* If all the buttons are off, turn on the start date's button. */
  if (!on) {
    dayOfWeek = DayOfWeek (d.frm_date.month,
			   d.frm_date.day,
			   d.frm_date.year /*+ firstYear*/); /* frm_date is DateTimeType */
    dayOfWeek = (dayOfWeek - d.repeat_start_of_week + daysInWeek) % daysInWeek;
    
    id = RepeatDayOfWeek1PushButton + dayOfWeek;
    CtlSetValue(GetObjectPointer(frm, id), true);
  }

  /* Update the display of the repeat description. */
  RepeatDrawDescription(frm);
}

/***********************************************************************
 *
 * FUNCTION:    RepeatChangeType
 *
 * DESCRIPTION: This routine changes the ui gadgets in the repeat dialog
 *              such that they match the newly selected repeat type.  The
 *              routine is called when one of the repeat type push buttons
 *              are pushed.
 *
 * PARAMETERS:  event - pointer to and event
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
static void RepeatChangeType(EventType* event) {
  UInt16 id;
  FormType* frm = FrmGetFormPtr(RepeatForm);
  const RepeatType oldType = d.repeat_event_type;
  RepeatType newType;
  RepeatInfoType repeat;

  /* If the type if monthly default to monthly-by-date. */
  newType = (RepeatType) (event->data.ctlSelect.controlID - RepeatNone);
  if (newType > repeatWeekly) 
    newType++;

  if (oldType == newType)
    return;

  /* Initialize the UI gadgets. */
  if (newType == d.tmp_repeat.repeatType) {
    RepeatSetUIValues(frm, &d.tmp_repeat);

    /*
    ** If reselecting current repeat type, reset d.repeat_end_date global
    ** to current date so if user attemps to "choose" a new day, the
    ** default matches date displayed as opposed to last date picked
    ** last in choose form.
    */
    d.repeat_end_date = d.tmp_repeat.repeatEndDate;
  } else {
    repeat.repeatType = newType;

    /*
    ** When switching to a repeat type different from the current
    ** setting, always start user with default end date and frequency
    ** settings.
    */
    DateToInt(repeat.repeatEndDate) = defaultRepeatEndDate;
    DateToInt(d.repeat_end_date) = defaultRepeatEndDate;
    repeat.repeatFrequency = defaultRepeatFrequency;
    
    repeat.repeatStartOfWeek = PrefGetPreference(prefWeekStartDay);

    if (newType == repeatWeekly) {
      repeat.repeatOn = (1 << DayOfWeek(d.frm_date.month,
					d.frm_date.day,
					d.frm_date.year/*+firstYear*/));
    } else if (newType == repeatMonthlyByDay) {
      repeat.repeatOn = DayOfMonth(d.frm_date.month,
				   d.frm_date.day,
				   d.frm_date.year/* + firstYear*/);
    } else {
      repeat.repeatOn = 0;
    }

    RepeatSetUIValues (frm, &repeat);
  }
  
  /*
  ** Hide the UI gadgets that are unique to the repeat type we are
  ** no longer editing.
  */
  switch (oldType) {
  case repeatNone:
    HideObject(frm, RepeatNoRepeatLabel);
    break;

  case repeatHourly:
    HideObject(frm, RepeatHoursLabel);
    break;

  case repeatDaily:
    HideObject(frm, RepeatDaysLabel);
    break;

  case repeatWeekly:
    HideObject(frm, RepeatWeeksLabel);
    for (id = RepeatRepeatOnLabel; id <= RepeatDayOfWeek7PushButton; id++)
      HideObject (frm, id);
    break;

  case repeatMonthlyByDay:
  case repeatMonthlyByDate:
    HideObject(frm, RepeatMonthsLabel);
    for (id = RepeatByLabel; id <= RepeatByDatePushButton; id++)
      HideObject (frm, id);
    break;

  case repeatYearly:
    HideObject(frm, RepeatYearsLabel);
    break;
  }

  /* Handle switching to or from "no" repeat. */
  if (oldType == repeatNone) {
    ShowObject(frm, RepeatEveryLabel);
    ShowObject(frm, RepeatFrequenceField);
    ShowObject(frm, RepeatEndOnLabel);
    ShowObject(frm, RepeatEndOnTrigger);
  } else if (newType == repeatNone) {
    HideObject(frm, RepeatEveryLabel);
    HideObject(frm, RepeatFrequenceField);
    HideObject(frm, RepeatEndOnLabel);
    HideObject(frm, RepeatEndOnTrigger);
  }

  /* Show the UI object that are appropriate for the new repeat type. */
  switch (newType) {
  case repeatNone:
    ShowObject(frm, RepeatNoRepeatLabel);
    break;

  case repeatHourly:
    ShowObject(frm, RepeatHoursLabel);
    break;

  case repeatDaily:
    ShowObject(frm, RepeatDaysLabel);
    break;

  case repeatWeekly:
    ShowObject(frm, RepeatWeeksLabel);
    ShowObject(frm, RepeatRepeatOnLabel);
    for (id = RepeatRepeatOnLabel; id <= RepeatDayOfWeek7PushButton; id++)
      ShowObject(frm, id);
    break;

  case repeatMonthlyByDay:
  case repeatMonthlyByDate:
    ShowObject(frm, RepeatMonthsLabel);
    ShowObject(frm, RepeatByLabel);
    ShowObject(frm, RepeatByDayPushButton);
    ShowObject(frm, RepeatByDatePushButton);
    break;

  case repeatYearly:
    ShowObject(frm, RepeatYearsLabel);
    break;
  }

  d.repeat_event_type = newType;

  /* Update the display of the repeat descrition. */
  RepeatDrawDescription(frm);
}

/***********************************************************************
 *
 * FUNCTION:    RepeatApply
 *
 * DESCRIPTION: This routine applies the changes made in the Repeat Dialog.
 *              The changes or copied to a block intiialize by the
 *              Details Dialog,  they a not written to the database until
 *              the Details Dialog is applied.
 *
 * PARAMETERS:  nothing
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
static void RepeatApply(void) {
  RepeatInfoType repeat;

  RepeatGetUIValues(FrmGetFormPtr(RepeatForm), &repeat);
  d.tmp_repeat = repeat;
}

/***********************************************************************
 *
 * FUNCTION:    RepeatDescRectHandler
 *
 * DESCRIPTION: This routine is the event handler for rectangle gadget
 *		surrounding the repeat description in the "Repeat
 *              Dialog Box".
 *
 *		Instead of drawing a static rectangle the size of the
 *		gadget bounds, I have sized the gadget to be the full area
 *		of the repeat dialog the description field will still fit
 *		should the field have to be slightly resized for any reason.
 *		The bounding rect drawn is calculated from the field's
 *		bounding rect.
 *
 * PARAMETERS:  gadgetP	- pointer to the gadget
 *		cmd	- the event type to be handled
 *		paramp	- any additional data that is passed to the gadget
 *
 * RETURNED:    true if the event was handled and should not be passed
 *              to a higher level handler.
 *
 ***********************************************************************/
static Boolean RepeatDescRectHandler(FormGadgetTypeInCallback *gadgetP, UInt16 cmd, void *paramP) {
  FieldType* fld = NULL;
  RectangleType	r;
  Boolean handled = false;

  switch(cmd) {
  case formGadgetEraseCmd:
  case formGadgetDrawCmd:

    /* Get the repeat description field and calculate a bounding box */
    fld = GetObjectPointer(FrmGetFormPtr(RepeatForm), RepeatDescField);
    FldGetBounds (fld, &r);
    RctInsetRectangle (&r, -4);
    
    if (cmd == formGadgetDrawCmd)
      WinDrawRectangleFrame(simpleFrame, &r);
    else
      WinEraseRectangle(&r, 0);

    handled = true;
    break;

  case formGadgetHandleEventCmd:
    /* do nothing */
    break;

  default:
    break;
  }
  
  return handled;
}


/***********************************************************************
 *
 * FUNCTION:    RepeatInit
 *
 * DESCRIPTION: This routine initializes tthe "Repeat Dialog Box".  All
 *              the object in the dialog are initialize, even if they
 *              are not used given the current repeat type.
 *
 * PARAMETERS:  frm - pointer to the repeat dialog box
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
void RepeatInit(FormType* frm) {
  UInt16 id = RepeatDayOfWeek1PushButton;

  /*
  ** Set usable the ui object that are appropriate for the given
  ** repeat type.
  */
  switch (d.tmp_repeat.repeatType) {
  case repeatNone:
    ShowObject(frm, RepeatNoRepeatLabel);
    break;

  case repeatHourly:
    ShowObject(frm, RepeatEveryLabel);
    ShowObject(frm, RepeatFrequenceField);
    ShowObject(frm, RepeatHoursLabel);
    ShowObject(frm, RepeatEndOnLabel);
    ShowObject(frm, RepeatEndOnTrigger);
    break;

  case repeatDaily:
    ShowObject(frm, RepeatEveryLabel);
    ShowObject(frm, RepeatFrequenceField);
    ShowObject(frm, RepeatDaysLabel);
    ShowObject(frm, RepeatEndOnLabel);
    ShowObject(frm, RepeatEndOnTrigger);
    break;

  case repeatWeekly:
    ShowObject(frm, RepeatEveryLabel);
    ShowObject(frm, RepeatFrequenceField);
    ShowObject(frm, RepeatWeeksLabel);
    ShowObject(frm, RepeatRepeatOnLabel);
    ShowObject(frm, RepeatEndOnLabel);
    ShowObject(frm, RepeatEndOnTrigger);
    for (; id <= RepeatDayOfWeek7PushButton; id++)
      ShowObject(frm, id);
    break;

  case repeatMonthlyByDay:
  case repeatMonthlyByDate:
    ShowObject(frm, RepeatEveryLabel);
    ShowObject(frm, RepeatFrequenceField);
    ShowObject(frm, RepeatMonthsLabel);
    ShowObject(frm, RepeatByLabel);
    ShowObject(frm, RepeatByDayPushButton);
    ShowObject(frm, RepeatByDatePushButton);
    ShowObject(frm, RepeatEndOnLabel);
    ShowObject(frm, RepeatEndOnTrigger);
    break;

  case repeatYearly:
    ShowObject(frm, RepeatEveryLabel);
    ShowObject(frm, RepeatFrequenceField);
    ShowObject(frm, RepeatYearsLabel);
    ShowObject(frm, RepeatEndOnLabel);
    ShowObject(frm, RepeatEndOnTrigger);
    break;
  }

  FrmSetGadgetHandler(frm, FrmGetObjectIndex(frm, RepeatDescRectGadget),
		      RepeatDescRectHandler);

  d.repeat_end_date = d.tmp_repeat.repeatEndDate;
  d.repeat_event_type = d.tmp_repeat.repeatType;
  
  RepeatSetUIValues(frm, &d.tmp_repeat);
}

/***********************************************************************
 *
 * FUNCTION:    RepeatHandleEvent
 *
 * DESCRIPTION: This routine is the event handler for the "Repeat
 *              Dialog Box".
 *
 * PARAMETERS:  event  - a pointer to an EventType structure
 *
 * RETURNED:    true if the event was handled and should not be passed
 *              to a higher level handler.
 *
 ***********************************************************************/
Boolean RepeatHandleEvent (EventType* event) {
  Boolean handled = false;

  if (event->eType == ctlSelectEvent) {
    switch (event->data.ctlSelect.controlID) {
      
    case RepeatOkButton:
      RepeatApply();
      /* Fall through... */
    case RepeatCancelButton:
      /* ...to system */
      break;

    case RepeatNone:
    case RepeatHourly:
    case RepeatDaily:
    case RepeatWeekly:
    case RepeatMonthly:
    case RepeatYearly:
      RepeatChangeType(event);
      handled = true;
      break;
      
    case RepeatDayOfWeek1PushButton:
    case RepeatDayOfWeek2PushButton:
    case RepeatDayOfWeek3PushButton:
    case RepeatDayOfWeek4PushButton:
    case RepeatDayOfWeek5PushButton:
    case RepeatDayOfWeek6PushButton:
    case RepeatDayOfWeek7PushButton:
      RepeatChangeRepeatOn(event);
      handled = true;
      break;

    case RepeatByDayPushButton:
    case RepeatByDatePushButton:
      RepeatDrawDescription(FrmGetFormPtr(RepeatForm));
      handled = true;
      break;
    }
  } else if (event->eType == keyDownEvent &&
	     !TxtCharIsHardKey(event->data.keyDown.modifiers, event->data.keyDown.chr) && 
	     !EvtKeydownIsVirtual(event)) {
    const WChar chr = event->data.keyDown.chr;
    if (TxtCharIsDigit(chr) || TxtCharIsCntrl(chr)) {
      Boolean noSelection = true;
      UInt16 startPos = 0;
      UInt16 endPos = 0;
      FormType* frm = FrmGetFormPtr(RepeatForm);
      FieldType* fld = GetObjectPointer(frm, RepeatFrequenceField);

      FldGetSelection(fld, &startPos, &endPos);
      noSelection = startPos == endPos;
      FldHandleEvent(fld, event);

      /*
      ** There are 3 ways that the repeat every value can be
      ** updated
      **	1) by the soft keyboard (on silkscreen)
      **	2) via graffiti entry in the form with a selection range
      ** 	3) via graffiti entry in the form with no selection range
      **	   (i.e. just an insertion cursor)
      ** Methods 1 & 2 result in a fldChangedEvent being posted so the
      ** update will be handled in response to that event.  ONLY when
      ** there is no selection range replacement, do we do the update here
      ** to avoid a double redraw of the description field.
      */
      if (noSelection)
	RepeatDrawDescription(frm);
    }

    handled = true;

  } else if (event->eType == fldChangedEvent) {
    /*
    ** When the user changes the "repeat every" value via the
    ** soft keyboard or in the form via a selection replacement
    ** a fldChangedEvent is posted. Update the description in
    ** response.
    */
    RepeatDrawDescription(FrmGetFormPtr(RepeatForm));

    handled = true;

  } else if (event->eType == popSelectEvent) {
    if (event->data.popSelect.listID == RepeatEndOnList) {
      RepeatSelectEndDate (event);
      handled = true;
    }
  }

  return handled;
}

/*
** Compare two dates
*/
extern inline Int16 DateCompare(DateType d1, DateType d2) {
  return (DateToInt(d1) > DateToInt(d2)) ? 1 : ((DateToInt(d1) < DateToInt(d2)) ? -1 : 0);
}

/***********************************************************************
 *
 * FUNCTION:    NextRepeat
 *
 * DESCRIPTION: This routine computes the date of the next
 *              occurrence of a repeating appointment.
 *
 * PARAMETERS:  rec     - a pointer to a DiddleBug record
 *              date    - passed:   date to start from
 *                        returned: date of next occurrence
 *
 * RETURNED:    true if the appointment occurs again
 *
 ***********************************************************************/
Boolean NextRepeat(DiddleBugRecordType* rec, DateTimeType* dateP) {

  /*
  ** Get the frequency on occurrence
  **(ex: every 2nd day, every 3rd month, etc).
  */ 
  const UInt16 freq = rec->repeatInfo.repeatFrequency;
  
  Int16  i = 0;
  UInt16 day;
  UInt16 year;
  UInt16 adjust;
  UInt16 weeksDiff;
  UInt16 monthsDiff;
  UInt16 daysInMonth;
  UInt16 dayOfWeek;
  UInt16 apptWeekDay;
  UInt16 firstDayOfWeek;
  UInt16 daysTilNext;
  UInt16 monthsTilNext;
  UInt32 dateInDays;
  UInt32 startInDays;
  DateType start, date, when, next;
  DateTimeType nextReal;
  UInt32 startSecs, nextSecs;

  DateSecondsToDate(TimDateTimeToSeconds(dateP), &date);
  TimSecondsToDateTime(rec->alarmSecs, &nextReal);
  nextReal.year = dateP->year;
  nextReal.month = dateP->month;
  nextReal.day = dateP->day;
  nextReal.weekDay = dateP->weekDay;

  /* Calculate alarm date */
  DateSecondsToDate(rec->alarmSecs, &when);

  /* Is the date passed after the end date of the appointment? */
  if (DateCompare(date, rec->repeatInfo.repeatEndDate) > 0)
    return false;

  /* Is the date passed before the start date of the appointment? */
  if (DateCompare(date, when) < 0)
    date = when;
  
  /* Get the date of the first occurrecne of the appointment. */
  start = when;

  switch (rec->repeatInfo.repeatType) {
    /* Hourly repeating appointment */
  case repeatHourly:
    startSecs = TimDateTimeToSeconds(dateP);
    nextSecs = rec->alarmSecs;
    while (nextSecs < startSecs)
      nextSecs += freq * hoursInSeconds;
    
    TimSecondsToDateTime(nextSecs, &nextReal);
    DateSecondsToDate(nextSecs, &next);
    break;

    /* Daily repeating appointment. */
  case repeatDaily:
    dateInDays = DateToDays(date);
    startInDays = DateToDays(start);
    daysTilNext = (dateInDays - startInDays + freq - 1) / freq * freq;
    if (startInDays + daysTilNext > (UInt32) maxDays)
      return false;
    DateDaysToDate (startInDays + daysTilNext, &next);
    break;
    
    /*
    ** Weekly repeating appointment (ex: every Monday and Friday).
    ** Yes, weekly repeating appointment can occur more then once a
    ** week.
    */
  case repeatWeekly:
    dateInDays = DateToDays(date);
    startInDays = DateToDays(start);
    
    firstDayOfWeek = (DayOfWeek (1, 1, firstYear) -
		      rec->repeatInfo.repeatStartOfWeek + daysInWeek) % daysInWeek;

    dayOfWeek = DayOfWeek(date.month, date.day, date.year+firstYear);
    apptWeekDay = (dayOfWeek - rec->repeatInfo.repeatStartOfWeek +
		   daysInWeek) % daysInWeek;

    /*
    ** Are we in a week in which the appointment occurrs, if not
    ** move to that start of the next week in which the appointment
    ** does occur.
    */
    weeksDiff = (((dateInDays + firstDayOfWeek) / daysInWeek) -
		 ((startInDays + firstDayOfWeek) / daysInWeek)) % freq;
    if (weeksDiff) {
      adjust = ((freq - weeksDiff) * daysInWeek) - apptWeekDay;
      apptWeekDay = 0;
      dayOfWeek = (dayOfWeek + adjust) % daysInWeek;
    } else {
      adjust = 0;
    }

    /* Find the next day on which the appointment repeats. */
    for (; i < daysInWeek; i++) {
      if (rec->repeatInfo.repeatOn & (1 << dayOfWeek))
	break;
      
      adjust++;
      
      if (++dayOfWeek == daysInWeek)
	dayOfWeek = 0;

      if (++apptWeekDay == daysInWeek)
	adjust += (freq - 1) * daysInWeek;
    }

    if (dateInDays + adjust > (UInt32) maxDays)
      return false;
    
    DateDaysToDate (dateInDays + adjust, &next);
    break;

    /*
    ** Monthly-by-day repeating appointment
    ** (ex: the 3rd Friday of every month).
    */
  case repeatMonthlyByDay:
    /* Compute the number of month until the appointment repeats again. */
    monthsTilNext = ((((date.year - start.year) * monthsInYear) +
		      (date.month - start.month)) + freq - 1) / freq * freq;

    while (true) {
      year = start.year + (start.month - 1 + monthsTilNext) / monthsInYear;
      if (year >= numberOfYears)
	return false;
      
      next.year = year;
      next.month = (start.month - 1 + monthsTilNext) % monthsInYear + 1;
      
      dayOfWeek = DayOfWeek (next.month, 1, next.year+firstYear);
      if ((rec->repeatInfo.repeatOn % daysInWeek) >= dayOfWeek)
	day = rec->repeatInfo.repeatOn - dayOfWeek + 1;
      else
	day = rec->repeatInfo.repeatOn + daysInWeek - dayOfWeek + 1;
      
      /*
      ** If repeat-on day is between the last sunday and the last
      ** saturday, make sure we're not passed the end of the month.
      */
      if ( (rec->repeatInfo.repeatOn >= domLastSun) &&
	   (day > DaysInMonth (next.month, next.year+firstYear))) {
	day -= daysInWeek;
      }
      next.day = day;

      /*
      ** Its posible that "next date" calculated above is
      ** before the date passed.  If so, move forward
      ** by the length of the repeat freguency and preform
      ** the calculation again.
      */
      if (DateToInt(date) > DateToInt(next))
	monthsTilNext += freq;
      else
	break;
    }
    break;

    /*
    ** Monthly-by-date repeating appointment
    ** (ex: the 15th of every month).
    */
  case repeatMonthlyByDate:
    /* Compute the number of month until the appointment repeats again. */
    monthsDiff = (date.year - start.year) * monthsInYear + date.month - start.month;
    monthsTilNext = (monthsDiff + freq - 1) / freq * freq;

    if (date.day > start.day && !(monthsDiff % freq))
      monthsTilNext += freq;

    year = start.year +	(start.month - 1 + monthsTilNext) / monthsInYear;
    if (year >= numberOfYears)
      return false;

    next.year = year;
    next.month = (start.month - 1 + monthsTilNext) % monthsInYear + 1;
    next.day = start.day;

    /* Make sure we're not passed the last day of the month. */
    daysInMonth = DaysInMonth(next.month, next.year+firstYear);
    if (next.day > daysInMonth)
      next.day = daysInMonth;
    break;

    /* Yearly repeating appointment. */
  case repeatYearly:
    next.day = start.day;
    next.month = start.month;
    
    year = start.year + ((date.year - start.year + freq - 1) / freq * freq);

    if (date.month > start.month || 
	(date.month == start.month && date.day > start.day))
      year += freq;

    /* Specal leap day processing. */
    if (next.month == february && next.day == 29 &&
	next.day > DaysInMonth(next.month, year+firstYear)) {
      next.day = DaysInMonth (next.month, year+firstYear);
    }
    if (year >= numberOfYears)
      return false;
    
    next.year = year;
    break;

  default:
    /* ignore */
  }

  /* Is the next occurrence after the end date of the appointment? */
  if (DateCompare(next, rec->repeatInfo.repeatEndDate) > 0)
    return false;

  dateP->day = next.day;
  dateP->month = next.month;
  dateP->year = next.year + firstYear;
  dateP->hour = nextReal.hour;
  dateP->minute = nextReal.minute;
  dateP->second = nextReal.second;

  return true;
}
