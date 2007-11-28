/*

Copyright (c) 2001, 2002, 2004 Peter Putzer <pputzer@users.sourceforge.net>.
Licensed under the GNU General Public License (GPL).

Portions copyright (c) 2000 Palm, Inc. or its subsidiaries.  All rights reserved.

*/

#include "thumbnails.h"
#include "listview.h"
#include "diddlebug.h"
#include "diddlebugExtern.h"
#include "diddlebugInlines.h"
#include "diddlebugRsc.h"
#include "globals.h"
#include "dynamic_buttons.h"
#include "debug.h"

/* Palm 5-way navigator */
#include "PalmChars.h"

/* Treo 600 5-way navigation */
#include <HsExt.h>
#include <HsNav.h> 

/* Number of records per page */
#define recordsPerPage           30

/* Number of records in a row */
#define recordsPerRow             6

/* Number of rows */
#define numberOfRows              recordsPerPage / recordsPerRow

/* Width of thumbnail (in pixels) */
#define THUMBNAIL_PIXELS          20

/* local functions */
static void ThumbnailViewLoadGadgets(FormType* frm) SUTIL2;
static void ThumbnailViewLoadRecords (FormType* frm) SUTIL2;
static UInt16 ThumbnailViewSelectCategory (void) SUTIL2;
static void ThumbnailViewPageScroll (WinDirectionType direction) SUTIL2;
static void ThumbnailViewScroll (Int16 linesToScroll) SUTIL2;
static void ThumbnailViewInit (FormPtr frm) SUTIL2;
static void ThumbnailDoCmdSecurity(void) SUTIL2;
static Boolean ThumbnailGadgetEvent(FormGadgetTypeInCallback* gadgetP, UInt16 cmd, void* paramP) SUTIL2;
static void ThumbnailViewUpdateScrollers(FormPtr frm) SUTIL2;
static Boolean ThumbnailViewUpdateDisplay(UInt16 updateCode) SUTIL2;
static DynamicButtonType* MapCoordinatesToButton(UInt16 x, UInt16 y) SUTIL2;
static void MapIndexToCoordinates(UInt16* dbI, UInt16* x, UInt16* y) SUTIL2;
static void ToggleButton (UInt16 x, UInt16 y, Boolean select) SUTIL2;
static Boolean ThumbnailViewMoveCursorLeft(void) SUTIL2;
static Boolean ThumbnailViewMoveCursorRight(void) SUTIL2;
static Boolean ThumbnailViewMoveCursorUp(void) SUTIL2;
static Boolean ThumbnailViewMoveCursorDown(void) SUTIL2;
static void UpdateButtonAndIndex(void) SUTIL2;
static Boolean ThumbnailViewHandleFiveWayNavSelect(void) SUTIL3;
static Boolean ThumbnailViewHandleFiveWayNavLeft(void) SUTIL3;
static Boolean ThumbnailViewHandleFiveWayNavRight(void) SUTIL3;

/***********************************************************************
 *
 * FUNCTION:    ThumbnailViewUpdateDisplay
 *
 * DESCRIPTION: This routine update the display of the thumbnail view
 *
 * PARAMETERS:  updateCode - a code that indicated what changes have been
 *                           made to the view.
 *
 * RETURNED:    Whether the event was handled, or not.
 *
 ***********************************************************************/
static Boolean ThumbnailViewUpdateDisplay(UInt16 updateCode) {
  FormType* frm = FrmGetActiveForm();
  Boolean handled = false;

  switch (updateCode) {
  case fUpdateDisplayOptsChanged:   
    SetTopVisibleRecord(0);
    /* Fall through */
  case fUpdateFontChanged:
    ThumbnailViewLoadRecords(frm);
    handled = true;
    break;

  case fUpdateDeletedCurrent:
    p.dbI = noRecordSelected;
    /* fall through */
  case fUpdateDeletedAny:   
    SetTopVisibleRecord(0);
    ThumbnailViewLoadRecords(frm);   
    handled = true;
    break;
  }

  return handled;
}

/***********************************************************************
 *
 * FUNCTION:    ThumbnailViewUpdateScrollers
 *
 * DESCRIPTION: This routine draws or erases the thumbnail view scroll arrow
 *              buttons.
 *
 * PARAMETERS:  frm          -  pointer to the to do thumbnail form
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
void ThumbnailViewUpdateScrollers(FormPtr frm) {
  UInt16 pos, maxValue;

  if (d.records_in_cat > recordsPerPage) {
    pos = d.top_row_pos_in_cat / recordsPerRow;
    maxValue = (d.records_in_cat - recordsPerPage) / recordsPerRow;
    if ((d.records_in_cat - recordsPerPage) % recordsPerRow)
      maxValue++;
  } else {
    pos = 0;
    maxValue = 0;
  }

  SclSetScrollBar(GetObjectPointer(frm, ScrollBar), pos, 0, maxValue,
		  recordsPerPage / recordsPerRow);
}

/***********************************************************************
 *
 * FUNCTION:    ThumbnailViewLoadGadgets
 *
 * DESCRIPTION: This routine loads sketches into the thumbnail view form
 *              thumbnail gadgets.
 *
 * PARAMETERS:  recordNum index of the first record to display.
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
static void ThumbnailViewLoadGadgets(FormType* frm) {
  UInt16 row;
  MemHandle recordH;
  DynamicButtonType* btn;
  MemPtr ptr;
  UInt16 attr;
  UInt16 recordNum = d.top_visible_record;
  const UInt16 max = Min(recordsPerPage, d.records_in_cat - d.top_row_pos_in_cat);

  for (row = 0; row < max; row++) {
    /* Get the next record in the current category. */
    recordH = DmQueryNextInCategory (d.dbR, &recordNum, p.category);

    if(row == 0) {
      /* store the position of the first row so we can use */
      /* d.top_row_pos_in_cat + row when drawing           */
      d.top_row_pos_in_cat = recordH ? DmPositionInCategory(d.dbR, recordNum, p.category) : 0;
    }

    btn = (DynamicButtonType*) FrmGetGadgetData(frm, FrmGetObjectIndex(frm, Thumb1 + row));

    /* Store record number */
    btn->value = recordNum;

    /* Clear old internal values */
    btn->selected = false;

    /* Read record attributes */
    DmRecordInfo(d.dbR, recordNum, &attr, NULL, NULL);

    if (attr & dmRecAttrSecret && d.privateRecordStatus == maskPrivateRecords) {
      DrawMaskedRecord(btn->content.bmpW, maskPattern);
    } else {
/*       WinHandle oldH = NULL; */
/*       Err err = 0; */
/*       BitmapType* bmp = BmpCreate(btn->contentRect.extent.x,  */
/* 				  btn->contentRect.extent.y, 1, NULL, &err); */
/*       if (err) abort(); */

      /* Uncompress thumbnail */
      ptr = MemHandleLock(recordH);
      //      MemMove(BmpGetBits(bmp), ptr + sketchDataOffset, sketchThumbnailSize);
      MemMove(BmpGetBits(WinGetBitmap(btn->content.bmpW)), ptr + sketchDataOffset, sketchThumbnailSize);
      MemHandleUnlock(recordH);

      /* Write thumbnail to content bitmap */
/*       WinPushDrawState(); */
/*       WinSetCoordinateSystem(kCoordinatesNative); */
/*       oldH = WinSetDrawWindow(btn->content.bmpW); */
/*       WinPaintBitmap(bmp, 0, 0); */
/*       WinSetDrawWindow(oldH); */
/*       WinPopDrawState(); */

      /* Clean up */
/*       BmpDelete(bmp); */
    }

    recordNum++;

    /* Show the right gadgets... */
    ShowObject(frm, Thumb1 + row);
  }

  /* ...store the index of the last visible thumbnail... */
  d.lastVisibleThumbnail = row - 1;

  /* ... and hide the rest */
  for (; row < recordsPerPage; row++)
    HideObject(frm, Thumb1 + row);

  /* Update the scroll arrows. */
  ThumbnailViewUpdateScrollers (frm);
}


/***********************************************************************
 *
 * FUNCTION:    ThumbnailViewLoadRecords
 *
 * DESCRIPTION: This routine loads sketches into the thumbnails view form.
 *
 * PARAMETERS:  nothing
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
static void ThumbnailViewLoadRecords(FormType* frm) {
  UInt16 recordNum;

  if (p.dbI != noRecordSelected) {
    /* Is the current record before the first visible record? */
    if (d.top_visible_record > p.dbI)
      SetTopVisibleRecord(p.dbI - DmPositionInCategory(d.dbR, p.dbI, p.category) % recordsPerRow);

    /* Is the current record after the last visible record? */
    else {
      recordNum = d.top_visible_record;
      DmSeekRecordInCategory(d.dbR, &recordNum, DmPositionInCategory(d.dbR, p.dbI, p.category) % recordsPerRow, dmSeekForward, p.category);
      if (recordNum < p.dbI)
	SetTopVisibleRecord(p.dbI - DmPositionInCategory(d.dbR, p.dbI, p.category) % recordsPerRow);
    }
  }

  /* Make sure we show a full display of records. */
  if (d.records_in_cat) {
    recordNum = dmMaxRecordIndex;
    DmSeekRecordInCategory(d.dbR, &recordNum, recordsPerPage - 1 - (recordsPerRow - d.records_in_cat % recordsPerRow) % recordsPerRow, dmSeekBackward, p.category);
    SetTopVisibleRecord(Min(d.top_visible_record, recordNum));
    /* Ensure that the top visible record actually is part of this category */
    recordNum = d.top_visible_record;
    DmSeekRecordInCategory(d.dbR, &recordNum, 0, dmSeekForward, p.category);
    d.top_visible_record = recordNum;
  } else
    SetTopVisibleRecord(0);

  ThumbnailViewLoadGadgets(frm);
}


/***********************************************************************
 *
 * FUNCTION:    ThumbnailViewSelectCategory
 *
 * DESCRIPTION: This routine handles selection, creation and deletion of
 *              categories form the Details Dialog.
 *
 * PARAMETERS:  nothing
 *
 * RETURNED:    The index of the new category.
 *
 *              The following global variables are modified:
 *			p.category
 *			d.show_all_categories
 *			d.categoryName
 *
 ***********************************************************************/
static UInt16 ThumbnailViewSelectCategory(void) {
  FormPtr frm;
  UInt16 category;
  Boolean categoryEdited;

  /* Process the category popup list. */
  category = p.category;

  frm = FrmGetActiveForm();
  categoryEdited = CategorySelect(d.dbR, frm, CategoryPop,
				  CategoryList, true, &category, d.categoryName, 1,
				  categoryDefaultEditCategoryString);

  if (categoryEdited || (category != p.category)) {
    ChangeCategory(category);

    /* Disable 5-way navigation */
    d.fiveWayNavigation = false;
    p.dbI = noRecordSelected;

    /* Display the new category. */
    ThumbnailViewLoadRecords(frm);
  }

  return (category);
}

/***********************************************************************
 *
 * FUNCTION:    ThumbnailViewNextCategory
 *
 * DESCRIPTION: This routine display the next category,  if the last
 *              catagory isn't being displayed
 *
 * PARAMETERS:  nothing
 *
 * RETURNED:    nothing
 *
 *              The following global variables are modified:
 *			p.category
 *			d.categoryName
 *
 ***********************************************************************/
void ThumbnailViewNextCategory(void) {
  UInt16 category = CategoryGetNext(d.dbR, p.category);

  if (category != p.category) {
    FormPtr frm = FrmGetActiveForm();
    ControlPtr ctl = GetObjectPointer(frm, CategoryPop);

    ChangeCategory(category);

    /* Set the label of the category trigger. */
    CategoryGetName(d.dbR, p.category, d.categoryName);
    CategorySetTriggerLabel(ctl, d.categoryName);

    /* Display the new category. */
    SetTopVisibleRecord(0);
    ThumbnailViewLoadGadgets(frm);
  }
}


/***********************************************************************
 *
 * FUNCTION:    ThumbnailViewPageScroll
 *
 * DESCRIPTION: This routine scrolls the list of of memo titles
 *              in the direction specified.
 *
 * PARAMETERS:  direction - winUp or down
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
static void ThumbnailViewPageScroll (WinDirectionType direction) {
  UInt16 newTopVisibleRecord = d.top_visible_record;
  
  p.dbI = noRecordSelected;

  /* Scroll the table down a page (less one row). */
  if (direction == winDown) {
    if (d.top_row_pos_in_cat + recordsPerPage * 2 - recordsPerRow < d.records_in_cat) {
      /* Try going forward one page */
      if (!SeekRecord(&newTopVisibleRecord, recordsPerPage - recordsPerRow, dmSeekForward)) {
	abort(); /* Should never happen */
      }     
    } else {
      /* Try going backwards one page from the last record */
      newTopVisibleRecord = dmMaxRecordIndex;
      if (!SeekRecord(&newTopVisibleRecord, recordsPerPage - 1 - (recordsPerRow - d.records_in_cat % recordsPerRow) % recordsPerRow, dmSeekBackward)) {
	/* Not enough records to fill one page.  Start with the first record */
	newTopVisibleRecord = 0;
	SeekRecord(&newTopVisibleRecord, 0, dmSeekForward);
      } 
    }
  } else {
    /* Scroll up a page (less one row). */
    if (!SeekRecord(&newTopVisibleRecord, recordsPerPage - recordsPerRow, dmSeekBackward)) {
      /* Not enough records to fill one page.  Start with the first record */
      newTopVisibleRecord = 0;
      SeekRecord(&newTopVisibleRecord, 0, dmSeekForward);
    }
  }

  /* Avoid redraw if no change */
  if (d.top_visible_record != newTopVisibleRecord) {
    SetTopVisibleRecord(newTopVisibleRecord);
    ThumbnailViewLoadRecords(FrmGetActiveForm());
  }
}



/***********************************************************************
 *
 * FUNCTION:    ThumbnailViewScroll
 *
 * DESCRIPTION: This routine scrolls the list of of memo titles
 *              in the direction specified.
 *
 * PARAMETERS:  linesToScroll - the number of lines to scroll
 *                              positve: down, negative: up
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
static void ThumbnailViewScroll(Int16 linesToScroll) {
  UInt16 newTopVisibleRecord = d.top_visible_record;
  FormType* frm = FrmGetActiveForm();

  p.dbI = noRecordSelected;

  /* Scroll down. */
  if (linesToScroll > 0)
    SeekRecord (&newTopVisibleRecord, linesToScroll * recordsPerRow, dmSeekForward);

  /* Scroll up. */
  else if (linesToScroll < 0)
    SeekRecord (&newTopVisibleRecord, -linesToScroll * recordsPerRow, dmSeekBackward);

  ErrFatalDisplayIf (d.top_visible_record == newTopVisibleRecord, "Invalid scroll value");

  SetTopVisibleRecord(newTopVisibleRecord);

  ThumbnailViewLoadGadgets(frm);
}

/*
** Handles events for my custom thumbnail gadgets.
*/
static Boolean ThumbnailGadgetEvent(FormGadgetTypeInCallback* gadgetP, UInt16 cmd, void* paramP) {
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

  case formGadgetEraseCmd:
    WinEraseRectangleFrame(btn->frame, &btn->contentRect);
    break;

  case formGadgetHandleEventCmd:
    {
      EventType* e = (EventType*) paramP;

      if (e->eType == frmGadgetEnterEvent) {
	if (DynBtnTrackPen(btn)) {
	  UInt16 attr;

	  /* A thumbnail was selected, display it. */
	  p.dbI = btn->value;

	  /* Get the category and secret attribute of the current record. */
	  DmRecordInfo(d.dbR, p.dbI, &attr, NULL, NULL);

	  /* If this is a "private" record, then determine what is to be shown. */
	  if (attr & dmRecAttrSecret) {
	    switch (d.privateRecordStatus) {
	    case showPrivateRecords:
	    case maskPrivateRecords:
	      FrmGotoForm(p.flags & PFLAGS_WITH_TITLEBAR ? DiddleTForm : DiddleForm);
	      break;

	      /* This case should never be executed!!!!!!! */
	    case hidePrivateRecords:
	    default:
	      break;
	    }
	  } else {
	    FrmGotoForm(p.flags & PFLAGS_WITH_TITLEBAR ? DiddleTForm : DiddleForm);
	  }

	  DynBtnDraw(btn);
	}
      }

      handled = true;
    } break;
  }

  return handled;
}

/***********************************************************************
 *
 * FUNCTION:    ThumbnailViewInit
 *
 * DESCRIPTION: This routine initializes the "Thumbnail View".
 *
 * PARAMETERS:  event  - a pointer to an EventType structure
 *
 * RETURNED:    nothing.
 *
 ***********************************************************************/
static void ThumbnailViewInit (FormPtr frm) {
  ControlPtr ctl;
  UInt16 i;
  Err err;
  DynamicButtonType* btn;

  for (i = Thumb1; i <= Thumb30; ++i) {
    btn = DynBtnInitGadget(i, rectangleFrame, true, true, false, d.sonyClie, 
			   d.sonyHRRefNum, false, dynBtnGraphical, 0, &err, frm,
			   ThumbnailGadgetEvent);
    if (err != errNone) abort();
  }

  ThumbnailViewLoadRecords(frm);

  /* Set the label of the category trigger. */
  ctl = GetObjectPointer(frm, CategoryPop);
  CategoryGetName(d.dbR, p.category, d.categoryName);
  CategorySetTriggerLabel(ctl, d.categoryName);

  /* Update scroll bar variables */
  ThumbnailViewUpdateScrollers(frm);

  /* Initialize grid coordinates */
  MapIndexToCoordinates(&p.dbI, &d.thumbnailX, &d.thumbnailY);
}

/*
** Handle Security command
*/
static void ThumbnailDoCmdSecurity(void) {
  FormType* frm = FrmGetActiveForm();
  Boolean wasHiding = d.privateRecordStatus == hidePrivateRecords;
  UInt16 mode;

  d.privateRecordStatus = SecSelectViewStatus();

  if (wasHiding ^ (d.privateRecordStatus == hidePrivateRecords)) {
    /* We have to close the database first */
    if (DmCloseDatabase(d.dbR) != errNone) abort();

    mode = dmModeReadWrite;
    if (d.privateRecordStatus != hidePrivateRecords)
      mode |= dmModeShowSecret;

    /* Re-open the database */
    if ((d.dbR = DmOpenDatabaseByTypeCreator(DBType, AppType, mode)) == NULL)
      abort();

    /* Update cached values */
    d.records_in_cat = DmNumRecordsInCategory(d.dbR, p.category);
    if (d.privateRecordStatus == hidePrivateRecords)
      SetTopVisibleRecord(0);

    ThumbnailViewLoadRecords (frm);
  }
}

/*
** Map grid coordinates to a certain dynamic button.
*/
static DynamicButtonType* MapCoordinatesToButton(UInt16 x, UInt16 y) {
  FormType* frm = FrmGetActiveForm();

  return FrmGetGadgetData(frm, FrmGetObjectIndex(frm, Thumb1 + y * recordsPerRow + x));
}

/*
** Map record index to grid coordinates.
*/
static void MapIndexToCoordinates(UInt16* dbI, UInt16* x, UInt16* y) {
  UInt16 pos;

  if (*dbI == noRecordSelected)
    *dbI = d.top_visible_record;

  pos = DmPositionInCategory(d.dbR, *dbI, p.category);
  if (DmGetLastErr() != errNone) abort();

  *y = (pos - d.top_row_pos_in_cat) / recordsPerRow;
  *x = (pos - d.top_row_pos_in_cat) % recordsPerRow;
}

/*
** Map grid coordinates to record index.
*/
extern inline UInt16 MapCoordinatesToIndex(UInt16 x, UInt16 y) {
  return MapCoordinatesToButton(x, y)->value;
}

/*
** Move the cursor left, if possible.
**
** The return value indicates whether we need to scroll up.
*/
static Boolean ThumbnailViewMoveCursorLeft(void) {
  const UInt16 pos = DmPositionInCategory(d.dbR, p.dbI, p.category);
  if (DmGetLastErr() != errNone) abort();

  /* If this is the first record in the category, don't do anything */
  if (pos == 0)
    return false;

  if (d.thumbnailX > 0) {
    --d.thumbnailX;
    return false;
  } else {
    d.thumbnailX = recordsPerRow - 1;
    if (d.thumbnailY > 0) {
      --d.thumbnailY;
      return false;
    } else {
      return true;
    }
  }
}

/*
** Move the cursor right, if possible.
**
** The return value indicates whether we need to scroll down.
*/
static Boolean ThumbnailViewMoveCursorRight(void) {
  const UInt16 pos = DmPositionInCategory(d.dbR, p.dbI, p.category);
  if (DmGetLastErr() != errNone) abort();

  /* This is the last record, don't do anything */ 
  if (pos + 1 == d.records_in_cat)
    return false;

  if (d.thumbnailX < recordsPerRow - 1) {
    ++d.thumbnailX;
    return false;
  } else {
    d.thumbnailX = 0;

    if (d.thumbnailY < numberOfRows - 1) {
      ++d.thumbnailY;
      return false;
    } else {
      return true;
    }
  }
}

/*
** Move the cursor up, if possible.
**
** The return value indicates whether we need to scroll up.
*/
static Boolean ThumbnailViewMoveCursorUp(void) {
  if (d.thumbnailY == 0) {
    if (d.top_row_pos_in_cat == 0)
      return false;
    else 
      return true;
  }

  --d.thumbnailY;
  return false;
}

/*
** Move the cursor down, if possible.
**
** The return value indicates whether we need to scroll down.
*/
static Boolean ThumbnailViewMoveCursorDown(void) {
  const UInt16 pos = DmPositionInCategory(d.dbR, p.dbI, p.category);
  if (DmGetLastErr() != errNone) abort();

  /* We're already at the very end. */
  if (pos + 1 == d.records_in_cat)
    return false;
 
  /* Just go to the last visible thumbnail if we can't move downward a full row. */
  if (pos + recordsPerRow >= d.records_in_cat) {
    UInt16 x1, y1; /* dummy coordinates */
    UInt16 dummyIndex = dmMaxRecordIndex;
    Err err = DmSeekRecordInCategory(d.dbR, &dummyIndex, 0, dmSeekBackward, p.category);
    if (err != errNone) abort();

    MapIndexToCoordinates(&dummyIndex, &x1, &y1);

    /* Only move the cursor if we can move to a new row, otherwise do nothing. */
    if (y1 != d.thumbnailY) {
      d.thumbnailY = y1;
      d.thumbnailX = x1;
    }
    
    /* Handle the special case that we need to scroll AND would go past the end. */
    if (d.thumbnailY > numberOfRows - 1) {
      d.thumbnailY = numberOfRows - 1;
      return true;
    }

    return false;
  }

  /* Simply go down a row. */
  if (d.thumbnailY < numberOfRows - 1) {
    ++d.thumbnailY;
    return false;
  }
   
  /* We need to scroll down. */
  return true;
}

/*
** Select or deselect a button given by grid coordinates.
*/
static void ToggleButton(UInt16 x, UInt16 y, Boolean select) {
  DynamicButtonType* btn = MapCoordinatesToButton(x, y);
  btn->selected = select;
  DynBtnDraw(btn);
}

/*
** Highlight the cursor position and update the record index.
*/
static void UpdateButtonAndIndex(void) {
  ToggleButton(d.thumbnailX, d.thumbnailY, true);
  p.dbI = MapCoordinatesToIndex(d.thumbnailX, d.thumbnailY);
}

/* 
** Handle 5-way navigation select.
*/
static Boolean ThumbnailViewHandleFiveWayNavSelect(void) {
  if (d.records_in_cat) {
    if (!d.fiveWayNavigation) {
      d.fiveWayNavigation = true;
      
      if (p.dbI == noRecordSelected)
	p.dbI = d.top_visible_record;
      
      MapIndexToCoordinates(&p.dbI, &d.thumbnailX, &d.thumbnailY);
      ToggleButton(d.thumbnailX, d.thumbnailY, true);	    
    } else {
      d.fiveWayNavigation = false;
      p.dbI = MapCoordinatesToIndex(d.thumbnailX, d.thumbnailY);
      FrmGotoForm(p.flags & PFLAGS_WITH_TITLEBAR ? DiddleTForm : DiddleForm);
    }
  }
  
  return true;
}

/*
** Handle 5-way navigation (right).
*/
static Boolean ThumbnailViewHandleFiveWayNavRight(void) {
  if (!d.fiveWayNavigation) {
    FrmGotoForm(DiddleThumbnailDetailForm);
  } else {
    /* Deselect previous item */
    ToggleButton(d.thumbnailX, d.thumbnailY, false);
    
    /* Select new item, scroll if necessary */
    if (ThumbnailViewMoveCursorRight())
      ThumbnailViewScroll(1);
    UpdateButtonAndIndex();
  }

  return true;
}

/*
** Handle 5-way navigation (right).
*/
static Boolean ThumbnailViewHandleFiveWayNavLeft(void) {
  if (!d.fiveWayNavigation) {
    FrmGotoForm(DiddleListForm);
  } else {
    /* Deselect previous item */	 
    ToggleButton(d.thumbnailX, d.thumbnailY, false);
    
    /* Select new item, scroll if necessary */
    if (ThumbnailViewMoveCursorLeft())
      ThumbnailViewScroll(-1);	      
    UpdateButtonAndIndex();
  }

  return true;
}

/***********************************************************************
 *
 * FUNCTION:    ThumbnailViewHandleEvent
 *
 * DESCRIPTION: This routine is the event handler for the "Thumbnail View"
 *              of the Record application.
 *
 * PARAMETERS:  event  - a pointer to an EventType structure
 *
 * RETURNED:    true if the event was handled and should not be passed
 *              to a higher level handler.
 *
 ***********************************************************************/
Boolean ThumbnailViewHandleEvent (EventPtr event) {
  FormPtr frm = NULL;
  Boolean handled = false;

  switch (event->eType) {
  case keyDownEvent:
    /* Hardware button pressed? */
    if (TxtCharIsHardKey(event->data.keyDown.modifiers, event->data.keyDown.chr)) {
      if (!(event->data.keyDown.modifiers & poweredOnKeyMask))
	ThumbnailViewNextCategory();
      handled = true;
    } else if (EvtKeydownIsVirtual(event)) {
      switch (event->data.keyDown.chr) {
      case vchrPageUp:
      case vchrRockerUp:
	if (d.fiveWayNavigation) {
	  /* Deselect previous item */
	  ToggleButton(d.thumbnailX, d.thumbnailY, false);
	  
	  /* Select new item, scroll if necessary */
	  if (ThumbnailViewMoveCursorUp())
	    ThumbnailViewScroll(-1);
	  UpdateButtonAndIndex();
	} else {
	  ThumbnailViewPageScroll(winUp);
	}
	handled = true;
	break;

      case vchrPageDown:
      case vchrRockerDown:
	if (d.fiveWayNavigation) {
	  /* Deselect previous item */
	  ToggleButton(d.thumbnailX, d.thumbnailY, false);
	  
	  /* Select new item, scroll if necessary */
	  if (ThumbnailViewMoveCursorDown())
	    ThumbnailViewScroll(1);
	  UpdateButtonAndIndex();
	} else {
	  ThumbnailViewPageScroll(winDown);
	}
	handled = true;
	break;
	
	/* Treo 600 5-way navigation */
      case vchrRockerCenter:
	handled = ThumbnailViewHandleFiveWayNavSelect();
	break;
      case vchrRockerLeft:
	handled = ThumbnailViewHandleFiveWayNavLeft();
	break;
      case vchrRockerRight:
	handled = ThumbnailViewHandleFiveWayNavRight();
	break;

	/* Tungsten 5-way navigation */
      case vchrNavChange:
	if (NavDirectionPressed(event, Left))
	  handled = ThumbnailViewHandleFiveWayNavLeft();
	else if (NavDirectionPressed(event, Right))
	  handled = ThumbnailViewHandleFiveWayNavRight();
	else if (NavSelectPressed(event))
	  handled = ThumbnailViewHandleFiveWayNavSelect();
	break;

/*       case vchrSendData: */
/* 	ThumbnailViewDoCommand(ThumbnailRecordBeamCategoryCmd); */
/* 	handled = true; */
/* 	break; */

      default:
	/* ignore */
      }
    }
    break;

  case ctlSelectEvent:
    switch (event->data.ctlSelect.controlID) {
    case NewButton:
      /* Create new sketch after all existing ones */
      p.dbI = DmNumRecords(d.dbR);

      AllocImage();
      FrmGotoForm(p.flags & PFLAGS_WITH_TITLEBAR ? DiddleTForm : DiddleForm);
      handled = true;
      break;

    case CategoryPop:
      ThumbnailViewSelectCategory();
      handled = true;
      break;

    case TextListButton:
      FrmGotoForm(DiddleListForm);
      handled = true;
      break;

    case ThumbnailDetailListButton:
      FrmGotoForm(DiddleThumbnailDetailForm);
      handled = true;
      break;
    }
    break;

  case frmGotoEvent:
    p.dbI = event->data.frmGoto.recordNum;
    SwitchCategoryForGoto();
    FrmGotoForm(p.flags & PFLAGS_WITH_TITLEBAR ? DiddleTForm : DiddleForm);
    event->eType = dbOpenRecordFieldEvent;
    EvtAddEventToQueue(event);

    handled = true;
    break;

  case menuEvent:
    {
      Char chr = 0;

      switch(event->data.menu.itemID) {
      case menuitemID_CmdAbout:
	DoAboutDialog();
	handled = true;
	break;

      case menuitemID_CmdSecurity:
	ThumbnailDoCmdSecurity();
	handled = true;
	break;

      case menuitemID_CmdSortByAlarm:      
      case menuitemID_CmdSortByName:
	if (event->data.menu.itemID == menuitemID_CmdSortByAlarm)
	  DmInsertionSort(d.dbR, &SortByAlarmTime, 0);
	else
	  DmInsertionSort(d.dbR, &SortByName, 0);

       	SetTopVisibleRecord(0);
	ThumbnailViewLoadRecords(FrmGetActiveForm());
	handled = true;
	break;

      case menuitemID_CmdPref: chr=cmdPref; break;
      case menuitemID_CmdExtPref: chr=cmdExtPref; break;
      case menuitemID_CmdHWPref: chr=cmdHWPref; break;
      case menuitemID_CmdAlarmPref: chr=cmdAlarmPref; break;
      case menuitemID_CmdHelp: chr=cmdHelp; break;
      }

      if (!handled)
	handled = KeyDown(chr);
    }
    break;

  case frmOpenEvent:
    frm = FrmGetActiveForm();
    ThumbnailViewInit(frm);
    FrmDrawForm(frm);

    /* Disable 5-way navigation initially */
    d.fiveWayNavigation = false;

    /* Clean up */
    if (d.record_name) MemHandleFree(d.record_name);
    d.record_name = NULL;
    if (d.record_note) MemHandleFree(d.record_note);
    d.record_note = NULL;

    handled = true;
    break;

  case frmUpdateEvent:
    handled = ThumbnailViewUpdateDisplay(event->data.frmUpdate.updateCode);
    break;

  case menuCmdBarOpenEvent:
    MenuCmdBarAddButton(menuCmdBarOnLeft, BarSecureBitmap,
			menuCmdBarResultMenuItem, menuitemID_CmdSecurity, 0);

    /* Tell the field package to not add buttons automatically; */
    /* we've done it all ourselves.                             */
    event->data.menuCmdBarOpen.preventFieldButtons = true;

    /* don't set handled to true; this event must fall through to the system. */
    break;

  case sclRepeatEvent:
    ThumbnailViewScroll (event->data.sclRepeat.newValue - event->data.sclRepeat.value);
    break;

  case dbNonKeyCommandEvent:
    handled = HandleNonKeyCommandCode(event->data.menu.itemID);
    break;

/*   case frmObjectFocusTakeEvent: */
/*     { */
/*       RectangleType r; */
/*       FormType* frm = FrmGetFormPtr(event->data.frmObjectFocusTake.formID); */
/*       const UInt16 id = event->data.frmObjectFocusTake.objectID; */
/*       const UInt16 idx = FrmGetObjectIndex(frm, id); */

/*       if (id >= Thumb1 && id <= Thumb30) { */
/* 	FrmGetObjectBounds(frm, idx, &r); */
/* 	FrmSetFocus(frm, idx); */
/* 	HsNavDrawFocusRing(frm, id, hsNavFocusRingNoExtraInfo, &r, */
/* 			   hsNavFocusRingStyleObjectTypeDefault, false); */
/* 	handled = true; */
/*       } */
/*     } */
/*     break; */

  default:
    /* do nothing */
  }

  return handled;
}
