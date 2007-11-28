/*

Copyright (c) 2001, 2002, 2004 Peter Putzer <pputzer@users.sourceforge.net>.
Licensed under the GNU General Public License (GPL).

*/

#include "details.h"
#include "diddlebug.h"
#include "diddlebugRsc.h"
#include "diddlebugInlines.h"
#include "diddlebugExtern.h"
#include "globals.h"
#include "debug.h"

/*
** RecordDetailsEvent - event handler for record details form
*/
Boolean RecordDetailsEvent(EventPtr e) {
  Boolean handled = false;
  FormPtr frm = FrmGetFormPtr(RecordDetailsForm);
  FieldType* nameFld = GetObjectPointer(frm, RecordDetailsNameField); 
  UInt16 attr = 0;
  const UInt16 prv = FrmGetObjectIndex(frm, RecordDetailsPrivateCheck);

  switch (e->eType) {
  case frmOpenEvent: 
    {  
      Char* tmp_ptr = MemHandleLock(d.record_name);
      const UInt32 tmp_len = StrLen(tmp_ptr) + 1;

      /* Initialize */
      ASSERTNOT(d.tmp_name);
      d.tmp_name = MemHandleNew(tmp_len);
      ASSERT(d.tmp_name);

      /* Load data */
      MemMove(MemHandleLock(d.tmp_name), tmp_ptr, tmp_len);
      MemHandleUnlock(d.tmp_name);
      MemHandleUnlock(d.record_name);
      
      FldSetTextHandle(nameFld, d.tmp_name);
      
      /* Look up category. */
      DmRecordInfo(d.dbR, p.dbI, &attr, NULL, NULL);
      d.tmp_category = attr & dmRecAttrCategoryMask;
      
      CategoryGetName(d.dbR, d.tmp_category, d.tmpCategoryName);
      CategorySetTriggerLabel(GetObjectPointer(frm, RecordDetailsCategoryPop), d.tmpCategoryName);
      
      /* Set priority */
      if (d.record.priority >= 0 && d.record.priority < 5)
	FrmSetControlGroupSelection(frm, 1, RecordDetailsPriority1Button + d.record.priority);
      else
	FrmSetControlGroupSelection(frm, 1, RecordDetailsPriority3Button);
      
      /* Set private flag */
      FrmSetControlValue(frm, prv, attr & dmRecAttrSecret);
      
      /* Set focus */
      FrmSetFocus(frm, FrmGetObjectIndex(frm, RecordDetailsNameField));

      /* Draw form */
      FrmDrawForm(frm);
    }
    handled = true;
    break;

  case ctlSelectEvent:
    switch (e->data.ctlEnter.controlID) {
    case RecordDetailsNoteButton:
      FldReleaseFocus(nameFld);
      DoNoteDialog(0, 0);
      FldGrabFocus(nameFld);
      handled = true;
      break;

    case RecordDetailsCategoryPop:
      {
      	Char* name = (Char*) CtlGetLabel (GetObjectPointer(frm, RecordDetailsCategoryPop));
	CategorySelect(d.dbR, frm, RecordDetailsCategoryPop, RecordDetailsCategoryList, false,
		       &d.tmp_category, name, 1, categoryDefaultEditCategoryString);
      }
      handled = true;
      break;

    case RecordDetailsOKButton:
      {
	Char* tmp_ptr = MemHandleLock(d.tmp_name);
	const UInt32 tmp_len = StrLen(tmp_ptr) + 1;
	MemHandle old_name = d.record_name;
	Char* old_ptr = NULL;
	const UInt16 oldPriority = d.record.priority;
	Boolean private = false;

	FldCompactText(nameFld);
   
	ErrTry {
	  /* Save name */
	  old_ptr = MemHandleLock(old_name);
	  d.record_name = MemHandleNew(tmp_len);
	  ASSERT(d.record_name);
	  MemMove(MemHandleLock(d.record_name), tmp_ptr, tmp_len);
	  if (StrCompare(tmp_ptr, old_ptr))
	    FlushToDisk(false);
	  MemHandleUnlock(old_name);
	  MemHandleFree(old_name);
	  old_name = NULL;
	  MemHandleUnlock(d.record_name);
	  MemHandleUnlock(d.tmp_name);
	
	  /* Save priority */
	  d.record.priority = FrmGetControlGroupSelection(frm, 1) - FrmGetObjectIndex(frm, RecordDetailsPriority1Button);
	  if (d.record.priority != oldPriority)
	    FlushToDisk(false);
	
	  /* Save category */
	  if ((attr & dmRecAttrCategoryMask) != d.tmp_category) {
	    attr = (attr & ~dmRecAttrCategoryMask) | d.tmp_category | dmRecAttrDirty;

	    DmSetRecordInfo(d.dbR, p.dbI, &attr, NULL);

	    if (p.flags & PFLAGS_KEEP_CATEGORY && DmNumRecordsInCategory(d.dbR, p.category)) {
	      SwitchCategory(p.category, FrmGetActiveForm());
	    }	else if (p.category != dmAllCategories) {
	      ChangeCategory(d.tmp_category);
	      // FIXME
	      /* Update x/y button */
	      if (FrmGetActiveFormID() == DiddleTForm)
		UpdatePageButton();
	    }
	  }

	  /* Save private status */
	  private = FrmGetControlValue(frm, prv);
	  if (private != (attr & dmRecAttrSecret)) {
	    if (private) {
	      attr |= dmRecAttrSecret | dmRecAttrDirty;
	      DmSetRecordInfo(d.dbR, p.dbI, &attr, NULL);
	
	      FlushToDisk(d.detailsWithSketch);
	      FrmAlert(SetPrivateAlert);
	  
	      if (d.detailsWithSketch) {
		if (d.privateRecordStatus != hidePrivateRecords) {
		  LoadFromDisk();
		} else {
		  SetTopVisibleRecord(0);
		  d.records_in_cat = DmNumRecordsInCategory(d.dbR, p.category);
		  p.dbI = noRecordSelected;
		  FrmGotoForm(ep.listFormID);
		}
	      }
	    } else {
	      attr &= ~dmRecAttrSecret | dmRecAttrDirty;
	      DmSetRecordInfo(d.dbR, p.dbI, &attr, NULL);
	
	      FlushToDisk(false);
	      if (d.detailsWithSketch)
		LoadFromDisk();
	    }
	  }
	  
	  if (!d.detailsWithSketch)
	    FrmUpdateForm(FrmGetActiveFormID(), fUpdateReloadRecords);

	  /* Update note button */
	  if (FrmGetActiveFormID() == DiddleTForm)
	    UpdateNoteButton();
	} 
	ErrCatch(inErr) {
	  if (old_name) {
	    MemHandleUnlock(old_name);
	    MemHandleFree(old_name);
	  }
	  MemHandleUnlock(d.record_name);
	  d.tmp_name = NULL;
	  ErrThrow(inErr);
	} ErrEndCatch;
      }

      /* Fall through for the rest */

    case RecordDetailsCancelButton:
      /* Clean up */
      d.tmp_name = NULL;
      FrmReturnToForm(0);
      d.formID = FrmGetActiveFormID();
      d.detailsWithSketch = false;
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
