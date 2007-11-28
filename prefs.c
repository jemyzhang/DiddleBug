/*

Copyright (c) 2001, 2002, 2004 Peter Putzer <pputzer@users.sourceforge.net>.
Copyright (c) 1999, 2000 Mitch Blevins <mblevin@debian.org>.

Licensed under the GNU General Public License (GPL).

*/

#include "diddlebug.h"
#include "diddlebugExtern.h"
#include "diddlebugInlines.h"
#include "diddlebugRsc.h"
#include "globals.h"
#include "debug.h"
#include "dynamic_buttons.h"
#include "prefs.h"

/* Treo 600 5-way navigation */
#include <HsExt.h>
#include <HsNav.h> 

static void InitHardwarePrefForm(FormType* frm) SUTIL1;
static void LoadPrefData(FormPtr frm) SUTIL1;
static void SavePrefData(FormPtr frm) SUTIL1;
static void DoHardwareActionList(Int8 i) SUTIL1;
static void LabelHardwareSelTrig(Int8 i) SUTIL1;
static void SetupOtherAlarmPrefs(FormType* frm) SUTIL1;
static void InitExtPrefForm(FormType* frm) SUTIL1;
static void LoadExtPrefData(FormPtr frm) SUTIL1;
static void SaveExtPrefData(FormPtr frm) SUTIL3;
static Boolean ColorGadgetEvent(FormGadgetTypeInCallback* gadgetP, UInt16 cmd, void* paramP) SUTIL3;
static void PickColor(IndexedColorType* color, DynamicButtonType* btn) SUTIL3;
static void HardwareActionDrawFunc(Int16 item, RectangleType* b, Char** text) SUTIL3;
static void PaintColorField(IndexedColorType* color, DynamicButtonType* btn) SUTIL3;
static void EnlargeBitmapAndMoveTrigger(FormType* frm, UInt16 bmpID, UInt16 triggerID, Int16 amount) SUTIL3;

/*
** Setup the alarm preferences not identical with the signal dialog
*/
static void SetupOtherAlarmPrefs(FormType* frm) {
  UInt8 val;
  ListType* lst;

  switch (p.alarm_auto_snooze_times) {
  case 0:
    val = asNever;
    break;
  case 1:
    val = asOnce;
    break;
  case 2:
    val = asTwice;
    break;
  case 3:
    val = asThrice;
    break;
  case 10:
    val = as10Times;
    break;
  case 100:
    val = as100Times;
    break;
  default:
    val = asThrice;
  }

  lst = GetObjectPointer(frm, RemindList);
  LstSetSelection(lst, val);
  CtlSetLabel(GetObjectPointer(frm, RemindPop), LstGetSelectionText(lst, val));

  switch (p.alarm_repeat_times) {
  case 1:
    val = alOnce;
    break;
  case 2:
    val = alTwice;
    break;
  case 3:
    val = alThrice;
    break;
  case 5:
    val = al5Times;
    break;
  case 10:
    val = al10Times;
    break;
  default:
    val = alOnce;
  }

  lst = GetObjectPointer(frm, AlarmTimesList);
  LstSetSelection(lst, val);
  CtlSetLabel(GetObjectPointer(frm, AlarmTimesPop), LstGetSelectionText(lst, val));

  switch (p.alarm_auto_snooze_delay) {
  case 1:
    val = ad1Minute;
    break;
  case 5:
    val = ad5Minutes;
    break;
  case 10:
    val = ad10Minutes;
    break;
  case 30:
    val = ad30Minutes;
    break;
  default:
    val = ad5Minutes;
  }

  lst = GetObjectPointer(frm, RemindDelayList);
  LstSetSelection(lst, val);
  CtlSetLabel(GetObjectPointer(frm, RemindDelayPop), LstGetSelectionText(lst, val));
}

/*
** SaveHWPrefData
*/
extern inline void SaveHWPrefData(void) {
  Int8 i = 0;

  /* Set the hardware action */
  for (; i < 4; ++i)
    p.hardware_action[i] = d.pref_hardware_action[i];

  /* Save prefs now */
  PrefSetAppPreferences(AppType, dbsNormalAppPrefs, 1, &p, sizeof(pref), true);
}

/*
** PrefExtEvent - handle events on the Preferences form
*/
Boolean PrefExtEvent(EventPtr e) {
  Boolean handled = false;
  FormPtr frm = NULL;

  switch (e->eType) {

  case frmOpenEvent:
    frm = FrmGetActiveForm();
    InitExtPrefForm(frm);
    LoadExtPrefData(frm);

    FrmDrawForm(frm);
    handled = true;
    break;

  case frmGotoEvent:
    p.dbI = e->data.frmGoto.recordNum;
    SwitchCategoryForGoto();
    FrmGotoForm(p.flags & PFLAGS_WITH_TITLEBAR ? DiddleTForm : DiddleForm);
    e->eType = dbOpenRecordFieldEvent;
    EvtAddEventToQueue(e);
    handled = true;
    break;

  case ctlSelectEvent:
    switch (e->data.ctlEnter.controlID) {
    case PrefOKButton: SaveExtPrefData(FrmGetActiveForm());
    case PrefCancelButton:
      FrmGotoForm(p.formID);
      handled = true;
      break;
    }
    break;
  
  case frmObjectFocusTakeEvent:
    {
      RectangleType r;
      UInt16 idx = 0;
      
      frm = FrmGetFormPtr(e->data.frmObjectFocusTake.formID);
      idx = FrmGetObjectIndex(frm, e->data.frmObjectFocusTake.objectID);

      switch (e->data.frmObjectFocusTake.objectID) {
      case PrefExtForegroundColorGadget:
      case PrefExtBackgroundColorGadget:
	FrmGetObjectBounds(frm, idx, &r);
	FrmSetFocus(frm, idx);
	HsNavDrawFocusRing(frm, e->data.frmObjectFocusTake.objectID, hsNavFocusRingNoExtraInfo, 
			   &r, hsNavFocusRingStyleObjectTypeDefault, true);
	handled = true;
	break;
      }
    } 
    break;

  case keyDownEvent:
    {
      if (e->data.keyDown.chr == vchrRockerCenter) {
	UInt16 id = 0;
	DynamicButtonType* btn = NULL;
	
	frm = FrmGetActiveForm();
	HsNavGetFocusRingInfo(frm, &id, NULL, NULL, NULL);

	if (!id) abort();

	switch (id) {
	case PrefExtForegroundColorGadget:	
	case PrefExtBackgroundColorGadget:
	  btn = FrmGetGadgetData(frm, FrmGetObjectIndex(frm, id));
	  if (!btn) abort();

	  if (id == PrefExtForegroundColorGadget)
	    PickColor(&d.tmpInkColor, btn);
	  else 
	    PickColor(&d.tmpPaperColor, btn);

	  DynBtnDraw(btn);	
	  HsNavDrawFocusRing(frm, id, hsNavFocusRingNoExtraInfo, 
			     &btn->frameRect, 
			     hsNavFocusRingStyleObjectTypeDefault, true);
	  break;
	}
      }
    }
    break;

  default:
    /* do nothing */
  }

  return handled;
}


/*
** PrefEvent - handle events on the Preferences form
*/
Boolean PrefEvent(EventPtr e) {
  Boolean handled = false;

  switch (e->eType) {

  case frmOpenEvent:
    {
      FormType* frm = FrmGetActiveForm();
      LoadPrefData(frm);
      
      FrmDrawForm(frm);
      handled = true;
    }
    break;

  case frmGotoEvent:
    p.dbI = e->data.frmGoto.recordNum;
    SwitchCategoryForGoto();
    FrmGotoForm(p.flags & PFLAGS_WITH_TITLEBAR ? DiddleTForm : DiddleForm);
    e->eType = dbOpenRecordFieldEvent;
    EvtAddEventToQueue(e);
    handled = true;
    break;

  case ctlSelectEvent:
    switch (e->data.ctlEnter.controlID) {
    case PrefOKButton: SavePrefData(FrmGetActiveForm());
    case PrefCancelButton:
      FrmGotoForm(p.formID);
      handled = true;
      break;
    }
    break;

  default:
    /* do nothing */
  }

  return handled;
}

/*
** PrefAlarmEvent - handle events on the Alarm Preferences form
*/
Boolean PrefAlarmEvent(EventPtr e) {
  Boolean handled = false;
  FormPtr frm = FrmGetActiveForm();

  switch (e->eType) {

  case popSelectEvent:
    switch (e->data.popSelect.controlID) {
    case SignalPop:
    case RemindMeSignalPop:
      if (d.midi_count > 0 && e->data.popSelect.selection >= 0) {
	DmOpenRef midiDB;
	UInt8* midiData;
	UInt16 idx;
	Char* label;
	SndSmfOptionsType options = { 0, sndSmfPlayAllMilliSec, sndMaxAmp, true, 0, 0 };
	SndMidiListItemType* midi = (SndMidiListItemType*) (MemHandleLock(d.midi_list))
	  + e->data.popSelect.selection;

	if (e->data.popSelect.controlID == SignalPop)
	  label = d.midi_label;
	else
	  label = d.midi_label2;

	LabelMIDITrigger(e->data.popSelect.selection, label, sndMidiNameLength);
	TruncatePopupTriggerText(frm, e->data.popSelect.controlID, label);
	CtlSetLabel(e->data.popSelect.controlP, label);

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
    }
    break;

  default:
    /* do nothing */
  }

  return handled;
}


/*
** Initialize necessary data objects and open the Alarm Preferences form
*/
void DoAlarmPreferences(void) {
  FormType* frm = FrmInitForm(PrefAlarmForm);

  InitMIDIList(NULL);

  if (d.formID == DiddleForm || d.formID == DiddleTForm)
    FlushToDisk(true);

  FrmSetEventHandler(frm, PrefAlarmEvent);
  SetupSignalDialog(frm, CHECK_FLAG(PFLAGS_DEFAULT_LED), CHECK_FLAG(PFLAGS_DEFAULT_VIBRATE),
		    p.std_alarm.listIndex);
  SetupSignalList(frm, RemindMeSignalList, RemindMeSignalPop, p.remind_alarm.listIndex, d.midi_label2);
  SetupOtherAlarmPrefs(frm);

  if (FrmDoDialog(frm) == PrefOKButton) {
    ControlType* ctl;
    Int16 i;

    if (d.midi_count > 0) {
      SndMidiListItemType* midi = MemHandleLock(d.midi_list);

      /* Standard signal */
      i = LstGetSelection(GetObjectPointer(frm, SignalList));
      p.std_alarm.uniqueRecID = midi[i].uniqueRecID;
      p.std_alarm.dbID = midi[i].dbID;
      p.std_alarm.cardNo = midi[i].cardNo;
      p.std_alarm.listIndex = i;

      /* Remind Me signal */
      i = LstGetSelection(GetObjectPointer(frm, RemindMeSignalList));
      p.remind_alarm.uniqueRecID = midi[i].uniqueRecID;
      p.remind_alarm.dbID = midi[i].dbID;
      p.remind_alarm.cardNo = midi[i].cardNo;
      p.remind_alarm.listIndex = i;

      /* Clean up */
      MemHandleUnlock(d.midi_list);
    }

    /* Set blinking LED flag if necessary */
    ctl = GetObjectPointer(frm, LEDCheck);
    if (CtlEnabled(ctl) && CtlGetValue(ctl))
      p.flags |= PFLAGS_DEFAULT_LED;
    else
      p.flags &= ~PFLAGS_DEFAULT_LED;

    /* Set vibrate flag if necessary */
    ctl = GetObjectPointer(frm, VibrateCheck);
    if (CtlEnabled(ctl) && CtlGetValue(ctl))
      p.flags |= PFLAGS_DEFAULT_VIBRATE;
    else
      p.flags &= ~PFLAGS_DEFAULT_VIBRATE;

    /* Set remind times */
    switch (LstGetSelection(GetObjectPointer(frm, RemindList))) {
    case asNever:
      p.alarm_auto_snooze_times = 0;
      break;
    case asOnce:
      p.alarm_auto_snooze_times = 1;
      break;
    case asTwice:
      p.alarm_auto_snooze_times = 2;
      break;
    case asThrice:
      p.alarm_auto_snooze_times = 3;
      break;
    case as10Times:
      p.alarm_auto_snooze_times = 10;
      break;
    case as100Times:
      p.alarm_auto_snooze_times = 100;
      break;
    }

    /* Set alarm repeat times */
    switch (LstGetSelection(GetObjectPointer(frm, AlarmTimesList))) {
    case alOnce:
      p.alarm_repeat_times = 1;
      break;
    case alTwice:
      p.alarm_repeat_times = 2;
      break;
    case alThrice:
      p.alarm_repeat_times = 3;
      break;
    case al5Times:
      p.alarm_repeat_times = 5;
      break;
    case al10Times:
      p.alarm_repeat_times = 10;
      break;
    }

    /* Set remind delay */
    switch (LstGetSelection(GetObjectPointer(frm, RemindDelayList))) {
    case ad1Minute:
      p.alarm_auto_snooze_delay = 1;
      break;
    case ad5Minutes:
      p.alarm_auto_snooze_delay = 5;
      break;
    case ad10Minutes:
      p.alarm_auto_snooze_delay = 10;
      break;
    case ad30Minutes:
      p.alarm_auto_snooze_delay = 30;
      break;
    }

    /* Save prefs now */
    PrefSetAppPreferences(AppType, dbsNormalAppPrefs, 1, &p, sizeof(pref), true);
  }

  /* Clean up */
  FrmDeleteForm(frm);
}


/*
** LoadPrefData - preload the Pref data onto the form
*/
static void LoadPrefData(FormType* frm) {
  ControlPtr ctl = NULL;
  FieldType* field = GetObjectPointer(frm, PrefSwipeField);
  Char tmp[5];
  Int8 i = 0;

  /* Set the disable-titlebar checkbox */
  ctl = GetObjectPointer(frm, PrefSwitchTitleCheck);
  CtlSetValue(ctl, CHECK_FLAG(PFLAGS_SWITCH_TITLE_ACTION));
  /* Set the confirm delete checkbox */
  ctl = GetObjectPointer(frm, PrefConfirmDeleteCheck);
  CtlSetValue(ctl, CHECK_FLAG(PFLAGS_CONFIRM_DELETE));
  /* Set the confirm clear checkbox */
  ctl = GetObjectPointer(frm, PrefConfirmClearCheck);
  CtlSetValue(ctl, CHECK_FLAG(PFLAGS_CONFIRM_CLEAR));
  /* Set the BigOK checkbox */
  ctl = GetObjectPointer(frm, PrefBigOkCheck);
  CtlSetValue(ctl, CHECK_FLAG(PFLAGS_BIG_OK));
  /* Set the KeepDrawTool checkbox */
  ctl = GetObjectPointer(frm, PrefKeepDrawToolCheck);
  CtlSetValue(ctl, CHECK_FLAG(PFLAGS_KEEP_DRAW_TOOL));
  /* Set the ArtistMode checkbox */
  ctl = GetObjectPointer(frm, PrefArtistModeCheck);
  CtlSetValue(ctl, CHECK_FLAG(PFLAGS_INK_ARTIST));
  /* Set the ChangeDepth checkbox */
  ctl = GetObjectPointer(frm, PrefChangeDepthCheck);
  CtlSetValue(ctl, CHECK_FLAG(PFLAGS_CHANGE_DEPTH));
  /* Set the NewRecordOnStart checkbox */
  ctl = GetObjectPointer(frm, PrefNewRecordOnStartCheck);
  CtlSetValue(ctl, CHECK_FLAG(PFLAGS_OPEN_NEW_RECORD));

  /* Set the MinSwipeField text */
  StrIToA(tmp, p.min_swipe);
  i = StrLen(tmp);
  FldInsert(field, tmp, Min(i, 2));
}

/*
** Update color field inside the button.
*/
static void PaintColorField(IndexedColorType* color, DynamicButtonType* btn) {
  RectangleType r;
  WinHandle oldH = WinSetDrawWindow(btn->content.bmpW);

  /* Save draw state */
  WinPushDrawState();

  /* Set up color and pattern */
  WinSetForeColor(*color);
  
  /* Paint the color field */
  WinGetDrawWindowBounds(&r);
  WinDrawRectangle(&r, 2);
  DynBtnDraw(btn);

  /* Clean up */
  WinPopDrawState();
  WinSetDrawWindow(oldH);
}

/*
** Handle the actual color picking for PrefExtForm
*/
static void PickColor(IndexedColorType* color, DynamicButtonType* btn) {
  IndexedColorType oldColor = *color;
  Boolean colorPicked = UIPickColor(color, NULL, UIPickColorStartPalette, NULL, NULL);

  if (!colorPicked)
    *color = oldColor;
  else
    PaintColorField(color, btn);
}

/*
** Handles events for color picker gadgets in PrefExtForm
*/
static Boolean ColorGadgetEvent(FormGadgetTypeInCallback* gadgetP, UInt16 cmd, void* paramP) {
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
    {
      EventType* e = (EventType*) paramP;

      if (e->eType == frmGadgetEnterEvent) {
	if (DynBtnTrackPen(btn)) {
	  /* Handle the selection */
	  if (btn->id == PrefExtForegroundColorGadget)
	    PickColor(&d.tmpInkColor, btn);
	  else if (btn->id == PrefExtBackgroundColorGadget)
	    PickColor(&d.tmpPaperColor, btn);

	  DynBtnDraw(btn);
	}
      }

      handled = true;
    } break;
  }

  return handled;
}

/*
** Initializes the color picker gadgets in the PrefExtForm.
*/
static void InitExtPrefForm(FormType* frm) {
  DynamicButtonType* btn = NULL;
  Err err = 0;

  /* Set up a draw window so that the debug simulator does not complain */
  if (WinGetDrawWindow() == NULL) 
    WinSetDrawWindow(FrmGetWindowHandle(frm));

  btn = DynBtnInitGadget(PrefExtForegroundColorGadget, rectangleFrame, true, false,
			 true, d.hires, 
			 dynBtnGraphical, 0, &err, frm, ColorGadgetEvent);

  btn = DynBtnInitGadget(PrefExtBackgroundColorGadget, rectangleFrame, true, false,
			 true, d.hires, 
			 dynBtnGraphical, 0, &err, frm, ColorGadgetEvent);
}

/*
** LoadExtPrefData - preload the Extended Preferences data onto the form
*/
static void LoadExtPrefData(FormType* frm) {
  ControlType* ctl = NULL;
  ListType* list = NULL;
  DynamicButtonType* btn = NULL;

  /* Set the "datetime stamp as default title" checkbox */
  ctl = GetObjectPointer(frm, PrefExtDateTimeStampCheck);
  CtlSetValue(ctl, CHECK_FLAG(PFLAGS_AUTO_DATETIME_STAMP));

  /* Set the "keep category on modification" checkbox */
  ctl = GetObjectPointer(frm, PrefExtKeepCategoryCheck);
  CtlSetValue(ctl, CHECK_FLAG(PFLAGS_KEEP_CATEGORY));

  /* Set the "never overwrite title" checkbox */
  ctl = GetObjectPointer(frm, PrefExtNeverOverwriteTitleCheck);
  CtlSetValue(ctl, CHECK_FLAG(PFLAGS_NEVER_OVERWRITE_TITLE));

  /* Set the "default priority" */
  ctl = GetObjectPointer(frm, PrefExtDefaultPriorityPop);
  list = GetObjectPointer(frm, PrefExtDefaultPriorityList);
  LstSetSelection(list, ep.defaultPriority);
  CtlSetLabel(ctl, LstGetSelectionText(list, LstGetSelection(list)));

  /* Set the Create after last... checkboxes */
  ctl = GetObjectPointer(frm, PrefExtCreateLastHardButtonCheck);
  CtlSetValue(ctl, CHECK_FLAG(PFLAGS_CREATE_LAST_HARDBUTTON));
  ctl = GetObjectPointer(frm, PrefExtCreateLastIconCheck);
  CtlSetValue(ctl, CHECK_FLAG(PFLAGS_CREATE_LAST_ICON));

  /* Set the color fields */
  btn = FrmGetGadgetData(frm, FrmGetObjectIndex(frm, PrefExtForegroundColorGadget));
  d.tmpInkColor = ep.inkColor;
  PaintColorField(&d.tmpInkColor, btn);
  btn = FrmGetGadgetData(frm, FrmGetObjectIndex(frm, PrefExtBackgroundColorGadget));
  d.tmpPaperColor = ep.paperColor;
  PaintColorField(&d.tmpPaperColor, btn);
}

/*
** LoadHWPrefData - preload the hardware Pref data onto the form
*/
extern inline void LoadHWPrefData(void) {
  Int8 i = 0;

  /* Update the "cache" for the hardware action */
  for (; i < 4; ++i)
    d.pref_hardware_action[i] = p.hardware_action[i];
}


/*
** SaveExtPrefData - save the extended preferences data from the form
*/
static void SaveExtPrefData(FormType* frm) {
  ControlType* ctl = NULL;
  ListType* list = NULL;
  Int16 val = -1;

  /* Set the values from the form */
  ctl = GetObjectPointer(frm, PrefExtDateTimeStampCheck);
  if (CtlGetValue(ctl)) p.flags |= PFLAGS_AUTO_DATETIME_STAMP;
  else p.flags &= ~PFLAGS_AUTO_DATETIME_STAMP;
  ctl = GetObjectPointer(frm, PrefExtKeepCategoryCheck);
  if (CtlGetValue(ctl)) p.flags |= PFLAGS_KEEP_CATEGORY;
  else p.flags &= ~PFLAGS_KEEP_CATEGORY;
  ctl = GetObjectPointer(frm, PrefExtNeverOverwriteTitleCheck);
  if (CtlGetValue(ctl)) p.flags |= PFLAGS_NEVER_OVERWRITE_TITLE;
  else p.flags &= ~PFLAGS_NEVER_OVERWRITE_TITLE;

  /* Get default priority */
  list = GetObjectPointer(frm, PrefExtDefaultPriorityList);
  val = LstGetSelection(list);
  if (val != noListSelection)
    ep.defaultPriority = val;

  /* Get values for "Create after last..." checkboxes */
  ctl = GetObjectPointer(frm, PrefExtCreateLastHardButtonCheck);
  if (CtlGetValue(ctl)) p.flags |= PFLAGS_CREATE_LAST_HARDBUTTON;
  else p.flags &= ~PFLAGS_CREATE_LAST_HARDBUTTON;
  ctl = GetObjectPointer(frm, PrefExtCreateLastIconCheck);
  if (CtlGetValue(ctl)) p.flags |= PFLAGS_CREATE_LAST_ICON;
  else p.flags &= ~PFLAGS_CREATE_LAST_ICON;

  /* Get the values of the color fields */
  ep.inkColor = d.tmpInkColor;
  ep.paperColor = d.tmpPaperColor;

  /* Save prefs now */
  PrefSetAppPreferences(AppType, dbsNormalAppPrefs, 1, &p, sizeof(pref), true);
  PrefSetAppPreferences(AppType, dbsExtendedAppPrefs, 1, &ep, sizeof(extended_pref), true);
}

/*
** SavePrefData
*/
static void SavePrefData(FormType* frm) {
  ControlType* ctl = NULL;
  FieldType* field = GetObjectPointer(frm, PrefSwipeField);

  /* Set the values from the form */
  ctl = GetObjectPointer(frm, PrefSwitchTitleCheck);
  if (CtlGetValue(ctl)) p.flags |= PFLAGS_SWITCH_TITLE_ACTION;
  else p.flags &= ~PFLAGS_SWITCH_TITLE_ACTION;
  ctl = GetObjectPointer(frm, PrefConfirmDeleteCheck);
  if (CtlGetValue(ctl)) p.flags |= PFLAGS_CONFIRM_DELETE;
  else p.flags &= ~PFLAGS_CONFIRM_DELETE;
  ctl = GetObjectPointer(frm, PrefConfirmClearCheck);
  if (CtlGetValue(ctl)) p.flags |= PFLAGS_CONFIRM_CLEAR;
  else p.flags &= ~PFLAGS_CONFIRM_CLEAR;
  ctl = GetObjectPointer(frm, PrefBigOkCheck);
  if (CtlGetValue(ctl)) p.flags |= PFLAGS_BIG_OK;
  else p.flags &= ~PFLAGS_BIG_OK;
  ctl = GetObjectPointer(frm, PrefKeepDrawToolCheck);
  if (CtlGetValue(ctl)) p.flags |= PFLAGS_KEEP_DRAW_TOOL;
  else p.flags &= ~PFLAGS_KEEP_DRAW_TOOL;
  ctl = GetObjectPointer(frm, PrefArtistModeCheck);
  if (CtlGetValue(ctl)) p.flags |= PFLAGS_INK_ARTIST;
  else p.flags &= ~PFLAGS_INK_ARTIST;
  ctl = GetObjectPointer(frm, PrefChangeDepthCheck);
  if (CtlGetValue(ctl)) p.flags |= PFLAGS_CHANGE_DEPTH;
  else p.flags &= ~PFLAGS_CHANGE_DEPTH;
  ctl = GetObjectPointer(frm, PrefNewRecordOnStartCheck);
  if (CtlGetValue(ctl)) p.flags |= PFLAGS_OPEN_NEW_RECORD;
  else p.flags &= ~PFLAGS_OPEN_NEW_RECORD;

  /* Get the min swipe distance from the field */
  p.min_swipe = StrAToI(FldGetTextPtr(field));

  /* Save prefs now */
  PrefSetAppPreferences(AppType, dbsNormalAppPrefs, 1, &p, sizeof(pref), true);
}

/*
** PrefHWEvent - handle events on the hardware preferences form
*/
Boolean PrefHWEvent(EventPtr e) {
  Boolean handled = false;

  switch (e->eType) {

  case frmOpenEvent: 
    {
      FormType* frm = FrmGetActiveForm();
      Int8 i = 0;

      LoadHWPrefData();
      InitHardwarePrefForm(frm);
      for (; i < 4; ++i)
	LabelHardwareSelTrig(i);

      FrmDrawForm(frm);
      handled = true;
    }
    break;

  case frmCloseEvent:
    if (d.happlist) {
      MemHandleUnlock(d.happlist);
      MemHandleFree(d.happlist);
      d.happlist = NULL;
      d.papplist = NULL;
      d.appcount = 0;
    }
    break;

  case frmGotoEvent:
    p.dbI = e->data.frmGoto.recordNum;
    SwitchCategoryForGoto();
    FrmGotoForm(p.flags & PFLAGS_WITH_TITLEBAR ? DiddleTForm : DiddleForm);
    e->eType = dbOpenRecordFieldEvent;
    EvtAddEventToQueue(e);
    handled = true;
    break;

  case ctlSelectEvent:
    switch (e->data.ctlEnter.controlID) {
    case PrefOKButton: SaveHWPrefData();
    case PrefCancelButton:
      FrmGotoForm(p.formID);
      handled = true;
      break;
    case PrefHardware1SelTrig:
    case PrefHardware2SelTrig:
    case PrefHardware3SelTrig:
    case PrefHardware4SelTrig:
      DoHardwareActionList(e->data.ctlEnter.controlID - PrefHardware1SelTrig);
      handled = true;
      break;
    }
    break;
  default:
    /* do nothing */
  }

  return handled;
}

/*
** Enlarge button bitmap and move trigger to the right.
*/
static void EnlargeBitmapAndMoveTrigger(FormType* frm, UInt16 bmpID, 
					UInt16 triggerID, Int16 amount) {
  RectangleType r;
  Coord x = 0;
  Coord y = 0;
  const UInt16 triggerIdx = FrmGetObjectIndex(frm, triggerID);
  const UInt16 bmpIdx = FrmGetObjectIndex(frm, bmpID);

  FrmGetObjectPosition(frm, triggerIdx, &x, &y);
  FrmSetObjectPosition(frm, triggerIdx, x + amount, y);

  FrmGetObjectBounds(frm, bmpIdx, &r);
  r.extent.x += amount;
  FrmSetObjectBounds(frm, bmpIdx, &r);
}

/*
** Initializes the bitmaps in the PrefHWForm.
*/
static void InitHardwarePrefForm(FormType* frm) {
  Err err = errNone;
  DmSearchStateType state;
  UInt16 cardNo = 0;
  LocalID localID = 0;
  Coord xOffset = 0;
  DynamicButtonType* btn = NULL;

  /* Set up offset for DateBook button */
  if (d.hires)
    xOffset = 3;
  else
    xOffset = 3;

  /* Move trigger if this is a palmOne device */
  if (d.palmOne)
    EnlargeBitmapAndMoveTrigger(frm, PrefHardware1Bitmap, PrefHardware1SelTrig, 6);

  /* Draw bitmap for DateBook button */
  btn = DynBtnInitGadget(PrefHardware1Bitmap, noFrame, false, true, true, d.hires, dynBtnGraphical, 0,
			 &err, frm, BitmapGadgetEvent);
  if (err) abort();
  if (d.treo600)
    CopyAppBitmapToWindow(AppType, bitmapRsc, Treo600PhoneBitmap, btn->content.bmpW, xOffset, 0);
  else 
    CopyAppBitmapToWindow(sysFileCDefaultButton1App, iconType, 1000, btn->content.bmpW, xOffset, 0);

  /* Set up offset for Address button */
  if (d.treo600)
    xOffset = 3;
  else if (d.hires)
    xOffset = 3;
  else
    xOffset = -2;

  /* Move trigger if this is a palmOne device */
  if (d.palmOne)
    EnlargeBitmapAndMoveTrigger(frm, PrefHardware2Bitmap, PrefHardware2SelTrig, 6);

  /* Draw bitmap for Address button */
  btn = DynBtnInitGadget(PrefHardware2Bitmap, noFrame, false, true, true, d.hires, dynBtnGraphical, 0,
			 &err, frm, BitmapGadgetEvent);
  if (err) abort();
  if (d.treo600)
    CopyAppBitmapToWindow(AppType, bitmapRsc, Treo600CalendarBitmap, btn->content.bmpW, xOffset, 0);
  else 
  CopyAppBitmapToWindow(sysFileCDefaultButton2App, iconType, 1000, btn->content.bmpW, xOffset, 0);

  /* Set up offset for ToDo button */
  if (d.hires)
    xOffset = 3;
  else
    xOffset = 3;

  /* Move trigger if this is a palmOne device */
  if (d.palmOne)
    EnlargeBitmapAndMoveTrigger(frm, PrefHardware3Bitmap, PrefHardware3SelTrig, 6);

  /* Draw bitmap for ToDo button */
  btn = DynBtnInitGadget(PrefHardware3Bitmap, noFrame, false, true, true,d.hires, dynBtnGraphical, 0,
			 &err, frm, BitmapGadgetEvent);
  if (err) abort();
  if (d.treo600)
    CopyAppBitmapToWindow(AppType, bitmapRsc, Treo600MessagingBitmap, btn->content.bmpW, xOffset, 0);
  else 
  CopyAppBitmapToWindow(sysFileCDefaultButton3App, iconType, 1000, btn->content.bmpW, xOffset, 0);

  /* Set up offset for MemoPad/NotePad button */
  if (d.hires)
    xOffset = 3;
  else 
    xOffset = 3;

  /* Move trigger if this is a palmOne device */
  if (d.palmOne)
    EnlargeBitmapAndMoveTrigger(frm, PrefHardware4Bitmap, PrefHardware4SelTrig, 6);

  /* Draw bitmap for MemoPad/NotePad button */
  btn = DynBtnInitGadget(PrefHardware4Bitmap, noFrame, false, true, true,d.hires, dynBtnGraphical, 0,
			 &err, frm, BitmapGadgetEvent);
  if (err) abort();
  if (d.treo600)
    CopyAppBitmapToWindow(AppType, bitmapRsc, Treo600DisplayBitmap, btn->content.bmpW, xOffset, 0);
  else {
    /* Test for existence of NotePad application */
    if (DmGetNextDatabaseByTypeCreator(true, &state, sysFileTApplication, 'npad', false, &cardNo, &localID) == dmErrCantFind)
      CopyAppBitmapToWindow(sysFileCDefaultButton4App, iconType, 1000, btn->content.bmpW, xOffset, 0);
    else
      CopyAppBitmapToWindow('npad', iconType, 1000, btn->content.bmpW, xOffset, 0);
  }
}

/*
** HardwareActionDrawFunc
*/
static void HardwareActionDrawFunc(Int16 item, RectangleType* b, Char** text) {
  SysDBListItemType *app;
  MemHandle t;
  Char str[64];
  Char* str2;

  switch (item) {
  case hwNoAction:
  case hwNewRecord:
  case hwNextRecord:
  case hwNextCategory:
  case hwListView:
  case hwDetails:
    t = DmGetResource(strRsc, NoActionString + item);
    str2 = MemHandleLock(t);
    WinDrawTruncChars(str2, StrLen(str2), b->topLeft.x, b->topLeft.y, b->extent.x);
    MemHandleUnlock(t);
    DmReleaseResource(t);
    break;

  default:
    app = d.papplist + (item - hwLastAction);
    SysCopyStringResource(str, LaunchString);
    TxtReplaceStr(str, 63, app->name, 1);
    WinDrawTruncChars(str, StrLen(str), b->topLeft.x, b->topLeft.y, b->extent.x);
  }
}

/*
** LabelHardwareSelTrig - Label the selection trigger for the hardware button
*/
static void LabelHardwareSelTrig(Int8 i) {
  FormPtr frm = FrmGetActiveForm();
  ControlPtr ctl;
  Char appname[32];
  UInt16 cardNo;
  LocalID dbID;
  DmSearchStateType searchstate;

  /* Get the selection trigger */
  ctl = GetObjectPointer(frm, PrefHardware1SelTrig + i);

  /* Determine the text label */
  switch (d.pref_hardware_action[i]) {

  case hwNoAction:
    SysCopyStringResource(d.hwText[i], NoActionString);
    break;

  case hwNewRecord:
    SysCopyStringResource(d.hwText[i], NewReminderString);
    break;

  case hwNextRecord:
    SysCopyStringResource(d.hwText[i], NextSketchString);
    break;

  case hwNextCategory:
    SysCopyStringResource(d.hwText[i], NextCategoryString);
    break;

  case hwListView:
    SysCopyStringResource(d.hwText[i], ListViewString);
    break;

  case hwDetails:
    SysCopyStringResource(d.hwText[i], DetailViewString);
    break;

  default:
    if (d.pref_hardware_action[i] < HW_MAX_ACTIONS) {
      d.pref_hardware_action[i] = hwNoAction;
      SysCopyStringResource(d.hwText[i], NoActionString);
      break;
    }

    if (DmGetNextDatabaseByTypeCreator(true, &searchstate, sysFileTApplication,
				       d.pref_hardware_action[i] - HW_MAX_ACTIONS,
				       true, &cardNo, &dbID)) {
      d.pref_hardware_action[i] = hwNoAction;
      SysCopyStringResource(d.hwText[i], NoActionString);
    } else {
      MemHandle name;
      DmOpenRef appDB = DmOpenDatabase(cardNo, dbID, dmModeReadOnly);
      if (!appDB) abort();

      name = DmGet1Resource(ainRsc, 1000);
      if (!name) DmDatabaseInfo(cardNo, dbID, appname, NULL, NULL, NULL, NULL,
				NULL, NULL, NULL, NULL, NULL, NULL);

      SysCopyStringResource(d.hwText[i], LaunchString);
      TxtReplaceStr(d.hwText[i], 63, name ? (Char*)MemHandleLock(name) : appname, 1);

      /* Clean up */
      if (name) {
	MemHandleUnlock(name);
	DmReleaseResource(name);
      }
      DmCloseDatabase(appDB);
    }
  }

  /* Label the selection trigger */
  CtlSetLabel(ctl, d.hwText[i]);
}

/*
** DoHardwareActionList - allow user to select hardware button action
*/
static void DoHardwareActionList(Int8 i) {
  SysDBListItemType *app;
  UInt16 idx;
  FormPtr frm;
  ListPtr list;
  Int16 list_selection = -1;
  Char str[64];
  RectangleType r;
  UInt16 tmp, list_width = 0;

  /* Get the list of applications */
  if (!d.happlist) {
    if (!SysCreateDataBaseList(sysFileTApplication, 0, &d.appcount, &d.happlist, true))
      abort();
    d.papplist = MemHandleLock(d.happlist);
  }

  /* Calculate necessary width and find current index */
  for (idx = 0; idx < d.appcount; ++idx) {
    app = d.papplist + idx;

    SysCopyStringResource(str, LaunchString);
    TxtReplaceStr(str, 63, app->name, 1);

    tmp = FntCharsWidth(str, StrLen(str));
    list_width = Max(list_width, tmp);

    /* find current list index */
    if (d.pref_hardware_action[i] >= hwLastAction
	&& app->creator == d.pref_hardware_action[i] - HW_MAX_ACTIONS)
      list_selection = idx + hwLastAction;
  }
  for (idx = hwNoAction; idx < hwLastAction; ++idx) {
    SysCopyStringResource(str, NoActionString + idx);
    tmp = FntCharsWidth(str, StrLen(str));
    list_width = Max(list_width, tmp);
  }

  /* fix list index if selected item is not an app */
  if (d.pref_hardware_action[i] < hwLastAction)
    list_selection = d.pref_hardware_action[i];

  /* Popup the list */
  frm = FrmGetActiveForm();
  list = GetObjectPointer(frm, PrefHardwareActionList);
  LstSetListChoices(list, NULL, d.appcount + hwLastAction);
  LstSetDrawFunction(list, HardwareActionDrawFunc);
  LstSetSelection(list, list_selection);
  LstMakeItemVisible(list, list_selection);
  FrmGetObjectBounds(frm, FrmGetObjectIndex(frm, PrefHardware1SelTrig + i), &r);
  r.extent.x = list_width + 15;
  r.extent.y = 10 * 10;
  FrmSetObjectBounds(frm, FrmGetObjectIndex(frm, PrefHardwareActionList), &r);
  list_selection = LstPopupList(list);

  /* Set the preferences according to the new selection */
  if (list_selection != -1) {
    if (list_selection >= hwLastAction) {
      app = d.papplist + (list_selection - hwLastAction);
      d.pref_hardware_action[i] = app->creator + HW_MAX_ACTIONS;
    }
    else d.pref_hardware_action[i] = list_selection;
  }
  LabelHardwareSelTrig(i);
}
