/* code07d0.c - Preferences panel
   Copyright © 2001, 2002 Peter Putzer <pputzer@edu.uni-klu.ac.at>
   Copyright © 1999 Mitch Blevins <mblevin@debian.org>
   GNU LGPL License
   Based on LinkHistoryHack by Ben Darnel <bgdarnel@unity.ncsu.edu>
   which was...
   Based on SampleHack by Roger Chaplin <foursquaredev@att.net>
*/

#include <PalmOS.h>
#include "dbscHack.h"
#include "dbscHackRsc.h"

static void PrefsInit(FormPtr frm);
static void PrefsOK();
static MemPtr GetObjectPtr(UInt16 id);
static void SetList(UInt16 triggerid, UInt16 listid, UInt16 key);

// This is the code that will be called whenever the user taps myhack's "+"
// button in the HackMaster list. HackMaster is not smart about the content of
// this file; it will simply jump to the first executable statement in this
// file. If you need helper functions, prototype them above, and define them
// AFTER the trap patch.
Boolean PrefsHandler(EventPtr event)
{
   FormPtr         frm;
   Boolean handled=false;

   switch (event->eType) {
   case frmOpenEvent:
      frm = FrmGetActiveForm();
      PrefsInit(frm);
      FrmDrawForm(frm);
      handled=1;
      break;

   case ctlSelectEvent:
      switch (event->data.ctlEnter.controlID) {
      case PrefsOKButton:
	 PrefsOK();
	 /* FALL THRU */

      case PrefsCancelButton:
	 FrmGotoForm(9000);  // return to HackMaster control panel
	 handled=true;
	 break;
      }
      break;

   default:
      break;
   }

   return handled;
}

static void PrefsInit(FormPtr frm)
{
   MyPrefsType prefs;
   UInt16 prefssize=sizeof(prefs);

   MemSet(&prefs, sizeof(MyPrefsType), 0);
   if ((PrefGetAppPreferences('DBSC', 0, &prefs, &prefssize, true)
	==noPreferenceFound)||(prefssize<sizeof(prefs))) {
      prefs.strokestart=calcChr;
      prefs.strokeend=findChr;
      prefs.clipstart=findChr;
      prefs.clipend=calcChr;
      prefs.flags |= DBSC_FLAGS_JUMP;
   }

   SetList(StrokeFromTrigger, StrokeFromList, prefs.strokestart);
   SetList(StrokeToTrigger, StrokeToList, prefs.strokeend);
   SetList(ClipFromTrigger, ClipFromList, prefs.clipstart);
   SetList(ClipToTrigger, ClipToList, prefs.clipend);
   CtlSetValue(GetObjectPtr(JumpCheckBox), prefs.flags&DBSC_FLAGS_JUMP);
}

static void SetList(UInt16 triggerid, UInt16 listid, UInt16 key)
{
   UInt16 selection;
   Char* text;

   switch (key) {
   case launchChr:
      selection=0;
      text="Applications";
      break;

   case menuChr:
      selection=1;
      text="Menu";
      break;

   case calcChr:
      selection=2;
      text="Calculator";
      break;

   case findChr:
      selection=3;
      text="Find";
      break;

   default:
      selection=4;
      text="Disabled";
      break;
   };
   LstSetSelection(GetObjectPtr(listid), selection);
   CtlSetLabel(GetObjectPtr(triggerid), text);
}

static void PrefsOK()
{
   UInt16 key[5];
   MyPrefsType prefs;
   UInt16 prefssize=sizeof(prefs);

   key[0]=launchChr;
   key[1]=menuChr;
   key[2]=calcChr;
   key[3]=findChr;
   key[4]=0;

   prefs.strokestart=key[LstGetSelection(GetObjectPtr(StrokeFromList))];
   prefs.strokeend=key[LstGetSelection(GetObjectPtr(StrokeToList))];
   prefs.clipstart=key[LstGetSelection(GetObjectPtr(ClipFromList))];
   prefs.clipend=key[LstGetSelection(GetObjectPtr(ClipToList))];
   if (CtlGetValue(GetObjectPtr(JumpCheckBox)))
       prefs.flags |= DBSC_FLAGS_JUMP;
   else
       prefs.flags &= ~DBSC_FLAGS_JUMP;

   PrefSetAppPreferences('DBSC', 0, 0, &prefs, prefssize, true);
}

static MemPtr GetObjectPtr(UInt16 id)
{
   FormPtr frm;
   frm=FrmGetActiveForm();
   return FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, id));
}
