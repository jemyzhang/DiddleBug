/*

Copyright (c) 2001, 2002, 2004 Peter Putzer <pputzer@users.sourceforge.net>.
Licensed under the GNU General Public License (GPL).

Portions copyright (c) 2000 Palm, Inc. or its subsidiaries.  All rights reserved.

*/

#include "listview.h"
#include "diddlebug.h"
#include "diddlebugExtern.h"
#include "diddlebugInlines.h"
#include "diddlebugRsc.h"
#include "globals.h"
#include "debug.h"

/* Palm 5-way navigator */
#include "PalmChars.h" 

/* Treo 600 5-way navigation */
#include <HsExt.h>
#include <HsNav.h> 

/* local functions */
static Boolean ListViewUpdateDisplay (UInt16 updateCode) SUTIL1;
static UInt16 ListViewNumberOfRows(TableType* table) SUTIL1;
static void ListViewDrawRecord (void*, Int16, Int16, RectanglePtr) SUTIL1;
static void ListViewLoadTable (FormPtr frm) SUTIL1;
static void ListViewLoadRecords (FormPtr frm) SUTIL2;
static UInt16 ListViewSelectCategory (void) SUTIL1;
static void ListViewPageScroll (WinDirectionType direction) SUTIL1;
static void ListViewScroll (Int16 linesToScroll) SUTIL1;
static void ListViewInit (FormPtr frm) SUTIL1;
static void ListViewUpdateScrollers(FormPtr frm) SUTIL1;
static void ReplaceTwoColors(const RectangleType*, UInt16,
			     UIColorTableEntries, UIColorTableEntries,
			     UIColorTableEntries, UIColorTableEntries) SUTIL1;
static void ListViewSelectTableItem (Boolean, TableType*, 
				     Int16, Int16, RectangleType*) SUTIL1;
static void DrawRecordTitle(Char*, UInt16, UInt16, Int16, Int16, Int16) SUTIL1;
static void ListDoCmdSecurity(void) SUTIL1;
static void MoveCursorUp(UInt16 old_dbI) SUTIL2;
static void MoveCursorDown(UInt16 old_dbI) SUTIL2;
static Boolean ListViewHandleFiveWayNavSelect(void) SUTIL2;

/***********************************************************************
 *
 * FUNCTION:    SeekRecord
 *
 * DESCRIPTION: Given the index of a 'to do' record, this routine scans
 *              forwards or backwards for displayable 'to do' records.
 *
 * PARAMETERS:  indexP  - pointer to the index of a record to start from;
 *                        the index of the record sought is returned in
 *                        this parameter.
 *
 *              offset  - number of records to skip:
 *                     	    0 - means seek from the current record to the
 *                              next display record, if the current record is
 *                              a displayable record, its index is retuned.
 *                          1 - means skipping one displayable record
 *
 *              direction - Must be either dmSeekForward or dmSeekBackward.
 *
 *
 * RETURNED:    false is return if a displayable record was not found.
 *
 ***********************************************************************/
Boolean SeekRecord(UInt16* indexP, UInt16 offset, Int16 direction) {
  DmSeekRecordInCategory(d.dbR, indexP, offset, direction, p.category);
  if (DmGetLastErr()) return false;

  return true;
}

/***********************************************************************
 *
 * FUNCTION:    ListViewNumberOfRows
 *
 * DESCRIPTION: This routine return the maximun number of visible rows,
 *              with the current list view font setting.
 *
 * PARAMETERS:  table - List View table
 *
 * RETURNED:    maximum number of displayable rows
 *
 ***********************************************************************/
static UInt16 ListViewNumberOfRows(TableType* table)
{
  UInt16 rows;
  UInt16 rowsInTable = TblGetNumberOfRows(table);
  UInt16 tableHeight;
  FontID currFont;
  RectangleType	r;

  TblGetBounds(table, &r);
  tableHeight = r.extent.y;

  currFont = FntSetFont(stdFont);
  rows = tableHeight / FntLineHeight();
  FntSetFont(currFont);

  if (rows <= rowsInTable)
    return rows;
  else
    return rowsInTable;
}

/***********************************************************************
 *
 * FUNCTION:    DrawRecordTitle
 *
 * DESCRIPTION: This routine draws the title of the specified title.
 *
 * PARAMETERS:	 title  - pointer to record title
 *               x      - draw position
 *               y      - draw position
 *               width  - maximum width to draw.
 *
 * RETURNED:	 nothing
 *
 ***********************************************************************/
static void DrawRecordTitle(Char* title, UInt16 num, UInt16 idx, Int16 x, Int16 y, Int16 width)
{
  Boolean deleteTitle = false;
  UInt16 titleLen = StrLen (title);

  ASSERT(num);

  if (titleLen == 0) {
    Char num_str1[maxStrIToALen], num_str2[maxStrIToALen];
    MemHandle template = DmGetResource(strRsc, UntitledNoteTitleString);

    title = TxtParamString(MemHandleLock(template),
			   StrIToA(num_str1, idx), StrIToA(num_str2, num), NULL, NULL);
    titleLen = StrLen(title);
    MemHandleUnlock(template);
    DmReleaseResource(template);

    deleteTitle = true;
  }

  if (FntWidthToOffset (title, titleLen, width, NULL, NULL) == titleLen)
    WinDrawChars (title, titleLen, x, y);
  else {
    Int16 titleWidth;

    titleLen = FntWidthToOffset (title, titleLen, width - FntCharWidth (chrEllipsis), NULL, &titleWidth);
    WinDrawChars (title, titleLen, x, y);
    WinDrawChar (chrEllipsis, x + titleWidth, y);
  }

  if (deleteTitle)
    MemPtrFree(title);
}

/***********************************************************************
 *
 * FUNCTION:    ListViewDrawRecord
 *
 * DESCRIPTION: This routine draws the title memo record in the list
 *              view.  This routine is called by the table routine,
 *              TblDrawTable, each time a line of the table needs to
 *              be drawn.
 *
 * PARAMETERS:  table  - pointer to the memo list table (TablePtr)
 *              row    - row of the table to draw
 *              column - column of the table to draw
 *              bounds - bound to the draw region
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
static void ListViewDrawRecord (void * table, Int16 row, Int16 column, RectanglePtr bounds) {
  UInt16 len;
  UInt16 category;
  UInt16 recordNum;
  MemHandle recH;
  MemPtr ptr;
  Int16 x, y;
  DiddleBugRecordType rec;
  UInt16 pos;
  // this string should handle up to "99999." but does not have to be 0 terminated when the . is added
  Char posStr[6];
  UInt16 attr;
  RectangleType maskRectangle;

  // Get the record number that corresponds to the table item to draw.
  // The record number is stored in the "intValue" field of the item.
  recordNum = TblGetRowID (table, row);

  DmRecordInfo (d.dbR, recordNum, &attr, NULL, NULL);
  // If the record is private and we are to hide private records, then get out of here.
  // This should be taken care of by the calling function, but we will go ahead and
  // take care of it here also.
  if ((attr & dmRecAttrSecret) && d.privateRecordStatus == hidePrivateRecords)
    return;

  x = bounds->topLeft.x + 1;
  y = bounds->topLeft.y;

  FntSetFont (stdFont);

  // Format the memo's postion, within its category, an draw it.
  category = p.category;

  //pos = DmPositionInCategory (d.dbR, recordNum, category);
  pos = d.top_row_pos_in_cat + row;
  StrIToA (posStr, pos+1);
  len = StrLen(posStr);
  // Warning if more than 99999 record (5 chars)
  ErrNonFatalDisplayIf(len > sizeof(posStr) - 1, "Too many records");
  posStr[len++] = '.';
  // This string does not need 0 termination for up to the end of the function. So the size is ok

  if (len < 3) x += FntCharWidth ('1');
  WinDrawChars (posStr, len, x, y);
  x += FntCharsWidth (posStr, len) + 4;

  // If we are here then we either we either mask the memo out or display the
  // record title.
  if (((attr & dmRecAttrSecret) && d.privateRecordStatus == maskPrivateRecords)) {
    MemMove (&maskRectangle, bounds, sizeof (RectangleType));
    maskRectangle.topLeft.x = x;
    maskRectangle.extent.x = bounds->extent.x - x;

    //If next row is masked, thicken rect so as to keep boundary at 1 pixel.
    /*    // THIS CODE REMOVED because people didn't like combining the masks together.
	  if (TblGetLastUsableRow(table) > row)
	  {
	  DmRecordInfo (d.dbR, TblGetRowID (table, row+1), &attr, NULL, NULL);
	  if (attr & dmRecAttrSecret)
	  maskRectangle.extent.y++;
	  }
    */
    ListViewDisplayMask (&maskRectangle);
  } else {
    recH = DmQueryRecord(d.dbR, recordNum);
    ptr = MemHandleLock(recH);
    MemMove(&rec, ptr, sizeof(DiddleBugRecordType));
    DrawRecordTitle((Char*)(ptr + sketchDataOffset + rec.sketchLength), d.records_in_cat, pos + 1,
		    x, y, bounds->extent.x - x);
    MemHandleUnlock(recH);
  }
}

/***********************************************************************
 *
 * FUNCTION:    ListViewDisplayMask
 *
 * DESCRIPTION: Draws the masked display for the record.
 *
 * PARAMETERS:  bounds (Input):  The bounds of the table item to display.
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
void ListViewDisplayMask (RectanglePtr bounds)
{
  RectangleType tempRect;
  CustomPatternType origPattern;
  MemHandle bitmapH;
  BitmapType * bitmapP;

  MemMove (&tempRect, bounds, sizeof (RectangleType));
  // Make sure it fits nicely into the display.
  tempRect.topLeft.y++;
  tempRect.extent.y --;
  tempRect.extent.x -= SecLockWidth + 1;

  WinGetPattern(&origPattern);
  WinSetPattern (maskPattern);
  WinFillRectangle (&tempRect, 0);
  WinSetPattern((const CustomPatternType*) &origPattern);

  //draw lock icon
  bitmapH = DmGetResource (bitmapRsc, SecLockBitmap);
  if (bitmapH) {
    bitmapP = MemHandleLock (bitmapH);
    WinDrawBitmap (bitmapP, tempRect.topLeft.x + tempRect.extent.x + 1,
		   tempRect.topLeft.y + ((tempRect.extent.y - SecLockHeight) / 2));
    MemPtrUnlock (bitmapP);
  }
}

/***********************************************************************
 *
 * FUNCTION:    ListViewUpdateScrollers
 *
 * DESCRIPTION: This routine draws or erases the list view scroll arrow
 *              buttons.
 *
 * PARAMETERS:  frm          -  pointer to the to do list form
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
void ListViewUpdateScrollers(FormPtr frm) {
  UInt16 pos, maxValue;
  Int16 rows = ListViewNumberOfRows (GetObjectPointer(frm, SketchTable));

  if (d.records_in_cat > rows) {
    pos = d.top_row_pos_in_cat;
    maxValue = d.records_in_cat - rows;
  } else {
    pos = 0;
    maxValue = 0;
  }

  SclSetScrollBar (GetObjectPointer(frm, ScrollBar), pos, 0, maxValue, rows);
}


/***********************************************************************
 *
 * FUNCTION:    ListViewLoadTable
 *
 * DESCRIPTION: This routine loads memo database records into
 *              the list view form.
 *
 * PARAMETERS:  recordNum index of the first record to display.
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
static void ListViewLoadTable (FormPtr frm)
{
  UInt16 row;
  UInt16 recordNum;
  UInt16 lineHeight;
  UInt16 dataHeight;
  UInt16 tableHeight;
  UInt16 numRows;
  UInt32 uniqueID;
  FontID currFont;
  TablePtr table;
  MemHandle recordH;
  RectangleType	r;

  table = GetObjectPointer(frm, SketchTable);

  TblGetBounds (table, &r);
  tableHeight = r.extent.y;

  currFont = FntSetFont (stdFont);
  lineHeight = FntLineHeight ();
  FntSetFont (currFont);

  dataHeight = 0;

  recordNum = d.top_visible_record;

  // For each row in the table, store the record number in the table item
  // that will dispaly the record.
  numRows = TblGetNumberOfRows (table);
  for (row = 0; row < numRows; row++) {
    // Get the next record in the current category.
    recordH = DmQueryNextInCategory (d.dbR, &recordNum, p.category);
    if(row == 0) {
      // store the position of the first row so we can use d.top_row_pos_in_cat+row
      // when drawing
      d.top_row_pos_in_cat = recordH ? DmPositionInCategory(d.dbR, recordNum, p.category) : 0;
    }

    // If the record was found, store the record number in the table item,
    // otherwise set the table row unusable.
    if (recordH && (tableHeight >= dataHeight + lineHeight)) {
      TblSetRowID (table, row, recordNum);
      TblSetItemStyle (table, row, 0, customTableItem);
      TblSetItemFont (table, row, 0, stdFont);

      TblSetRowHeight (table, row, lineHeight);

      DmRecordInfo (d.dbR, recordNum, NULL, &uniqueID, NULL);
      if ((TblGetRowData (table, row) != uniqueID) ||
	  ( ! TblRowUsable (table, row)))
	{
	  TblSetRowUsable (table, row, true);

	  // Store the unique id of the record in the row.
	  TblSetRowData (table, row, uniqueID);

	  // Mark the row invalid so that it will draw when we call the
	  // draw routine.
	  TblMarkRowInvalid (table, row);
	}

      if (row+1 < numRows) recordNum++;

      dataHeight += lineHeight;
    } else {
      // Set the row height - when scrolling winDown, the heights of the last rows of
      // the table are used to determine how far to scroll.  As rows are deleted
      // from the top of the table, formerly unused rows scroll into view, and the
      // height is used before the next call to ListViewLoadTable (which would set
      // the height correctly).
      TblSetRowHeight (table, row, lineHeight);

      TblSetRowUsable (table, row, false);
    }
  }

  // Update the scroll arrows.
  ListViewUpdateScrollers (frm);
}


/***********************************************************************
 *
 * FUNCTION:    ListViewLoadRecords
 *
 * DESCRIPTION: This routine loads memo database records into
 *              the list view form.
 *
 * PARAMETERS:  nothing
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
static void ListViewLoadRecords(FormPtr frm) {
  TablePtr table = FrmGetObjectPtr(frm, FrmGetObjectIndex (frm, SketchTable));
  UInt16 rowsInTable = ListViewNumberOfRows (table);
  UInt16 recordNum;

  // Is the current record before the first visible record?
  if (p.dbI != noRecordSelected) {
    if (d.top_visible_record > p.dbI)
      SetTopVisibleRecord(p.dbI);

    // Is the current record after the last visible record?
    else {
      recordNum = d.top_visible_record;
      DmSeekRecordInCategory (d.dbR, &recordNum, rowsInTable - 1,
			      dmSeekForward, p.category);
      if (recordNum < p.dbI)
	SetTopVisibleRecord(p.dbI);
    }
  }

  // Make sure we show a full display of records.
  if (d.records_in_cat) {
    recordNum = dmMaxRecordIndex;
    DmSeekRecordInCategory(d.dbR, &recordNum, (rowsInTable-1), dmSeekBackward, p.category);
    SetTopVisibleRecord(Min(d.top_visible_record, recordNum));
    /* Ensure that the top visible record actually is part of this category */
    recordNum = d.top_visible_record;
    DmSeekRecordInCategory(d.dbR, &recordNum, 0, dmSeekForward, p.category);
    d.top_visible_record = recordNum;
  } else {
    SetTopVisibleRecord(0);
  }  

  ListViewLoadTable (frm);

  // Set the callback routine that will draw the records.
  TblSetCustomDrawProcedure (table, 0, ListViewDrawRecord);

  TblSetColumnUsable (table, 0, true);
}


/***********************************************************************
 *
 * FUNCTION:    ListViewSelectCategory
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
static UInt16 ListViewSelectCategory (void) {
  FormPtr frm;
  TablePtr table;
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
    ListViewLoadRecords(frm);
    table = GetObjectPointer(frm, SketchTable);
    TblEraseTable(table);
    TblDrawTable(table);
  }

  return category;
}

/***********************************************************************
 *
 * FUNCTION:    ListViewNextCategory
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
void ListViewNextCategory(void)
{
  UInt16 category;
  FormPtr frm = FrmGetActiveForm();
  TablePtr table;
  ControlPtr ctl;

  category = CategoryGetNext (d.dbR, p.category);

  if (category != p.category) {
    ChangeCategory (category);

    // Set the label of the category trigger.
    ctl = GetObjectPointer(frm, CategoryPop);
    CategoryGetName (d.dbR, p.category, d.categoryName);
    CategorySetTriggerLabel (ctl, d.categoryName);

    // Display the new category.
    SetTopVisibleRecord(0);
    ListViewLoadTable (frm);
    table = GetObjectPointer(frm, SketchTable);
    TblEraseTable (table);
    TblDrawTable (table);
  }
}


/***********************************************************************
 *
 * FUNCTION:    ListViewPageScroll
 *
 * DESCRIPTION: This routine scrolls the list of of memo titles
 *              in the direction specified.
 *
 * PARAMETERS:  direction - winUp or down
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
static void ListViewPageScroll (WinDirectionType direction)
{
  TablePtr table;
  Int16 rowsInTable;
  UInt16 newTopVisibleRecord;

  table = GetObjectPointer(FrmGetActiveForm(), SketchTable);
  rowsInTable = ListViewNumberOfRows (table);

  newTopVisibleRecord = d.top_visible_record;
  p.dbI = noRecordSelected;

  // Scroll the table down a page (less one row).
  if (direction == winDown) {
    // Try going forward one page
    if (!SeekRecord (&newTopVisibleRecord, rowsInTable - 1, dmSeekForward)) {
      // Try going backwards one page from the last record
      newTopVisibleRecord = dmMaxRecordIndex;
      if (!SeekRecord (&newTopVisibleRecord, rowsInTable - 1, dmSeekBackward)) {
	// Not enough records to fill one page.  Start with the first record
	newTopVisibleRecord = 0;
	SeekRecord (&newTopVisibleRecord, 0, dmSeekForward);
      }
    }
  }

  // Scroll up a page (less one row).
  else {
    if (!SeekRecord (&newTopVisibleRecord, rowsInTable - 1, dmSeekBackward)) {
      // Not enough records to fill one page.  Start with the first record
      newTopVisibleRecord = 0;
      SeekRecord (&newTopVisibleRecord, 0, dmSeekForward);
    }
  }

  // Avoid redraw if no change
  if (d.top_visible_record != newTopVisibleRecord) {
    SetTopVisibleRecord(newTopVisibleRecord);
    ListViewLoadRecords (FrmGetActiveForm ());
    TblRedrawTable(table);
  }

  /* Have a valid selected record (for 5-way navigation) */
  p.dbI = d.top_visible_record;
}



/***********************************************************************
 *
 * FUNCTION:    ListViewScroll
 *
 * DESCRIPTION: This routine scrolls the list of of memo titles
 *              in the direction specified.
 *
 * PARAMETERS:  linesToScroll (scroll up if negative)
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
static void ListViewScroll(Int16 linesToScroll) {
  Int16 i;
  UInt16 newTopVisibleRecord = d.top_visible_record;
  TableType* table = GetObjectPointer(FrmGetActiveForm(), SketchTable);
  UInt16 rows = ListViewNumberOfRows (table);
  UInt16 lastRow;
  UInt16 scrollAmount;
  RectangleType	scrollR;
  RectangleType	vacated;
  WinDirectionType direction;

  //  p.dbI = noRecordSelected;

  // Scroll down.
  if (linesToScroll > 0)
    SeekRecord (&newTopVisibleRecord, linesToScroll, dmSeekForward);

  // Scroll up.
  else if (linesToScroll < 0)
    SeekRecord (&newTopVisibleRecord, -linesToScroll, dmSeekBackward);

  ErrFatalDisplayIf (d.top_visible_record == newTopVisibleRecord, "Invalid scroll value");

  SetTopVisibleRecord(newTopVisibleRecord);

  // Move the bits that will remain visible.
  if (((linesToScroll > 0) && (linesToScroll < rows)) ||
      ((linesToScroll < 0) && (-linesToScroll < rows)))
    {
      scrollAmount = 0;

      if (linesToScroll > 0) {
	lastRow = TblGetLastUsableRow (table) - 1;
	for (i = 0; i < linesToScroll; i++) {
	  scrollAmount += TblGetRowHeight (table, lastRow);
	  TblRemoveRow (table, 0);
	}
	direction = winUp;
      } else {
	for (i = 0; i < -linesToScroll; i++) {
	  scrollAmount += TblGetRowHeight (table, 0);
	  TblInsertRow (table, 0);
	}
	direction = winDown;
      }

      TblGetBounds (table, &scrollR);
      WinScrollRectangle (&scrollR, direction, scrollAmount, &vacated);
      WinEraseRectangle (&vacated, 0);
    }

  ListViewLoadTable (FrmGetActiveForm ());
  TblRedrawTable(table);

  /* Have a valid selected record (for 5-way navigation) */
/*   p.dbI = d.top_visible_record; */
}



/***********************************************************************
 *
 * FUNCTION:    ListViewInit
 *
 * DESCRIPTION: This routine initializes the "List View" of the
 *              Record application.
 *
 * PARAMETERS:  event  - a pointer to an EventType structure
 *
 * RETURNED:    nothing.
 *
 ***********************************************************************/
static void ListViewInit (FormPtr frm)
{
  ControlPtr ctl;

  ListViewLoadRecords (frm);

  // Set the label of the category trigger.
  ctl = GetObjectPointer(frm, CategoryPop);
  CategoryGetName (d.dbR, p.category, d.categoryName);
  CategorySetTriggerLabel (ctl, d.categoryName);

  /* Update scroll bar variables */
  ListViewUpdateScrollers(frm);
}

/***********************************************************************
 *
 * FUNCTION:    ReplaceTwoColors
 *
 * DESCRIPTION: This routine does a selection or deselection effect by
 *		replacing foreground and background colors with a new pair
 *		of colors. In order to reverse the process, you must pass
 *		the colors in the opposite order, so that the current
 *		and new colors are known to this routine. This routine
 *		correctly handling the cases when two or more of these
 *		four colors are the same, but it requires that the
 *		affected area of the screen contains neither of the
 *		given NEW colors, unless these colors are the same as
 *		one of the old colors.
 *
 * PARAMETERS:	 rP  - pointer to a rectangle to 'invert'
 *		 cornerDiam	- corner diameter
 *		 oldForeground	- UI color currently used for foreground
 *		 oldBackground	- UI color currently used for background
 *		 newForeground	- UI color that you want for foreground
 *		 newBackground	- UI color that you want for background
 *
 * RETURNED:	 nothing
 *
 ***********************************************************************/
static void ReplaceTwoColors (const RectangleType *rP, UInt16 cornerDiam,
			      UIColorTableEntries oldForeground, UIColorTableEntries oldBackground,
			      UIColorTableEntries newForeground, UIColorTableEntries newBackground)
{
  UInt8 oldForegroundIndex = UIColorGetTableEntryIndex(oldForeground);
  UInt8 oldBackgroundIndex = UIColorGetTableEntryIndex(oldBackground);
  UInt8 newForegroundIndex = UIColorGetTableEntryIndex(newForeground);
  UInt8 newBackgroundIndex = UIColorGetTableEntryIndex(newBackground);

  WinPushDrawState();
  WinSetDrawMode(winSwap);
  WinSetPatternType (blackPattern);

  if (newBackgroundIndex == oldForegroundIndex)
    if (newForegroundIndex == oldBackgroundIndex)
      {
	// Handle the case when foreground and background colors change places,
	// such as on black and white systems, with a single swap.
	WinSetBackColor(oldBackgroundIndex);
	WinSetForeColor(oldForegroundIndex);
	WinPaintRectangle(rP, cornerDiam);
      }
    else
      {
	// Handle the case when the old foreground and the new background
	// are the same, using two swaps.
	WinSetBackColor(oldForegroundIndex);
	WinSetForeColor(oldBackgroundIndex);
	WinPaintRectangle(rP, cornerDiam);
	WinSetBackColor(oldBackgroundIndex);
	WinSetForeColor(newForegroundIndex);
	WinPaintRectangle(rP, cornerDiam);
      }
  else if (oldBackgroundIndex == newForegroundIndex)
    {
      // Handle the case when the old background and the new foreground
      // are the same, using two swaps.
      WinSetBackColor(newForegroundIndex);
      WinSetForeColor(oldForegroundIndex);
      WinPaintRectangle(rP, cornerDiam);
      WinSetBackColor(newBackgroundIndex);
      WinSetForeColor(oldForegroundIndex);
      WinPaintRectangle(rP, cornerDiam);
    }
  else
    {
      // Handle the case when no two colors are the same, as is typically the case
      // on color systems, using two swaps.
      WinSetBackColor(oldBackgroundIndex);
      WinSetForeColor(newBackgroundIndex);
      WinPaintRectangle(rP, cornerDiam);
      WinSetBackColor(oldForegroundIndex);
      WinSetForeColor(newForegroundIndex);
      WinPaintRectangle(rP, cornerDiam);
    }

  WinPopDrawState();
}

/***********************************************************************
 *
 * FUNCTION:    ListViewSelectTableItem
 *
 * DESCRIPTION: This routine either selects or unselects the specified
 *					 table item.
 *
 * PARAMETERS:	 selected - specifies whether an item should be selected or
 *			    unselected
 *		 table 	  - pointer to a table object
 *               row      - row of the item (zero based)
 *               column   - column of the item (zero based)
 *               r        - pointer to a structure that will hold the bound
 *                          of the item
 * RETURNED:	 nothing
 *
 ***********************************************************************/

static void ListViewSelectTableItem(Boolean selected, TableType* table, 
				    Int16 row, Int16 column, RectangleType* r)
{
  /* Get the item's rectangle. */
  TblGetItemBounds(table, row, column, r);

  /* Set up the drawing state the way we want it. */
  WinPushDrawState();
  WinSetBackColor(UIColorGetTableEntryIndex(UIFieldBackground));
  WinSetForeColor(UIColorGetTableEntryIndex(UIObjectForeground));
  WinSetTextColor(UIColorGetTableEntryIndex(UIObjectForeground));

  /* Erase and (re)draw the item. */
  WinEraseRectangle(r, 0);
  ListViewDrawRecord(table, row, column, r);

  /* If selected, make it look that way. */
  if (selected)
    ReplaceTwoColors(r, 0,
		     UIObjectForeground, UIFieldBackground,
		     UIObjectSelectedForeground, UIObjectSelectedFill);

  /* Restore the previous drawing state. */
  WinPopDrawState();
}

/***********************************************************************
 *
 * FUNCTION:    ListViewUpdateDisplay
 *
 * DESCRIPTION: This routine update the display of the list view
 *
 * PARAMETERS:  updateCode - a code that indicated what changes have been
 *                           made to the view.
 *
 * RETURNED:    nothing
 *
 ***********************************************************************/
static Boolean ListViewUpdateDisplay (UInt16 updateCode)
{
  FormType* frm = FrmGetActiveForm();
  TableType* table = GetObjectPointer(frm, SketchTable);
  Boolean handled = false;

  switch (updateCode) {
  case fUpdateDisplayOptsChanged:
    SetTopVisibleRecord(0);
    /* Fall through */
  case fUpdateFontChanged:  
    ListViewLoadRecords (frm);
    TblEraseTable (table);
    TblDrawTable (table);
    handled = true;
    break;

  case fUpdateDeletedCurrent:
    p.dbI = noRecordSelected;
    /* fall through */
  case fUpdateDeletedAny:
    SetTopVisibleRecord(0);
    ListViewLoadRecords (frm);
    TblEraseTable (table);
    TblDrawTable (table);
    handled = true;
    break;
  }

  /* Have a valid selected record (for 5-way navigation) */
  p.dbI = d.top_visible_record;

  return handled;
}

/*
** Handle Security command
*/
static void ListDoCmdSecurity(void) {
  FormType* frm = FrmGetActiveForm();
  TableType* table;
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

    ListViewLoadRecords (frm);
  }

  table = GetObjectPointer(frm, SketchTable);
  TblEraseTable (table);
  TblDrawTable (table);
}

/*
** Move the cursor up one step and scroll if necessary.
*/
static void MoveCursorUp(UInt16 old_dbI) {
  FormType* frm = FrmGetActiveForm();
  TableType* table = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, SketchTable));
  UInt16 curPos, oldPos;
  RectangleType r;

  /* Get the positions corresponding to the record indices */
  oldPos = DmPositionInCategory(d.dbR, old_dbI, p.category);
  if (DmGetLastErr() != errNone) abort();
  curPos = DmPositionInCategory(d.dbR, p.dbI, p.category);
  if (DmGetLastErr() != errNone) abort();

  /* Deselect previous item */
  ListViewSelectTableItem(false, table, oldPos - d.top_row_pos_in_cat, 0, &r);

  /* Select new item, scroll if necessary */
  if (oldPos - d.top_row_pos_in_cat == 0)
    ListViewScroll(-1);
  ListViewSelectTableItem(true, table, curPos - d.top_row_pos_in_cat, 0, &r);
}

/*
** Move the cursor down one step and scroll if necessary.
*/
static void MoveCursorDown(UInt16 old_dbI) {
  FormType* frm = FrmGetActiveForm();
  TableType* table = FrmGetObjectPtr(frm, FrmGetObjectIndex(frm, SketchTable));
  const UInt16 numRows = TblGetNumberOfRows(table);
  UInt16 curPos, oldPos;
  RectangleType r;
       
  /* Get the positions corresponding to the record indices */
  oldPos = DmPositionInCategory(d.dbR, old_dbI, p.category);
  if (DmGetLastErr() != errNone) abort();
  curPos = DmPositionInCategory(d.dbR, p.dbI, p.category);
  if (DmGetLastErr() != errNone) abort();

  /* Deselect previous item */
  ListViewSelectTableItem(false, table, oldPos - d.top_row_pos_in_cat, 0, &r);

  /* Select new item, scroll if necessary */
  if (curPos - d.top_row_pos_in_cat >= numRows)
    ListViewScroll(1);
  ListViewSelectTableItem(true, table, curPos - d.top_row_pos_in_cat, 0, &r);
}

/*
** Handle 5-way navigation select.
*/
static Boolean ListViewHandleFiveWayNavSelect(void) {
  if (!d.fiveWayNavigation) {
    RectangleType r;
    UInt16 pos;
    
    if (p.dbI == noRecordSelected && d.records_in_cat)
      p.dbI = d.top_visible_record;
    
    if (p.dbI != noRecordSelected) {
      pos = DmPositionInCategory(d.dbR, p.dbI, p.category);
      if (DmGetLastErr() != errNone) abort();
      
      d.fiveWayNavigation = true;
      ListViewSelectTableItem(true, GetObjectPointer(FrmGetActiveForm(),
						     SketchTable),
			      pos - d.top_row_pos_in_cat, 0, &r);
    }
  } else {
    d.fiveWayNavigation = false;	    
    FrmGotoForm(p.flags & PFLAGS_WITH_TITLEBAR ? DiddleTForm : DiddleForm);
  }
  
  return true;
}
/***********************************************************************
 *
 * FUNCTION:    ListViewHandleEvent
 *
 * DESCRIPTION: This routine is the event handler for the "List View"
 *              of the Record application.
 *
 * PARAMETERS:  event  - a pointer to an EventType structure
 *
 * RETURNED:    true if the event was handled and should not be passed
 *              to a higher level handler.
 *
 ***********************************************************************/
Boolean ListViewHandleEvent(EventType* event) {
  FormType* frm = NULL;
  Boolean handled = false;
  UInt16 attr = 0;

  switch (event->eType) {
  case keyDownEvent:
    /* Hardware button pressed? */
    if (TxtCharIsHardKey(event->data.keyDown.modifiers, event->data.keyDown.chr)) {
      if (!(event->data.keyDown.modifiers & poweredOnKeyMask))
	ListViewNextCategory();
      handled = true;
    } else if (EvtKeydownIsVirtual(event)) {
      switch (event->data.keyDown.chr) {
	
	/* Tungsten 5-way navigation */
      case vchrNavChange:
	if (NavDirectionPressed(event, Left)) {
	  FrmGotoForm(DiddleThumbnailDetailForm);
	  handled = true;
	} else if (NavDirectionPressed(event, Right)) {
	  FrmGotoForm(DiddleThumbnailForm);
	  handled = true;
	} else if (NavSelectPressed(event)) {
	  handled = ListViewHandleFiveWayNavSelect();
	}
	break;
	
	/* Treo 600 5-way navigation */
      case vchrRockerCenter:
	handled = ListViewHandleFiveWayNavSelect();
	break;
      case vchrRockerLeft:
	FrmGotoForm(DiddleThumbnailDetailForm);
	handled = true;
	break;
      case vchrRockerRight:
	FrmGotoForm(DiddleThumbnailForm);
	handled = true;
	break;

      case vchrPageUp:	 
      case vchrRockerUp:
	if (d.fiveWayNavigation) {
	  Int16 old_dbI = p.dbI;
	  
	  /* Try going backward one record */
	  if (!SeekRecord(&p.dbI, 1, dmSeekBackward))
	    p.dbI = old_dbI; /* Nowhere to go, that's OK */
	  else
	    MoveCursorUp(old_dbI);
	} else {
	  ListViewPageScroll(winUp);
	}
	handled = true;
	break;
	
      case vchrPageDown:
      case vchrRockerDown:
	if (d.fiveWayNavigation) {
	  Int16 old_dbI = p.dbI;

	  /* Try going forward one record */
	  if (!SeekRecord(&p.dbI, 1, dmSeekForward))
	    p.dbI = old_dbI; /* Nowhere to go, that's OK */
	  else
	    MoveCursorDown(old_dbI);
	} else {
	  ListViewPageScroll(winDown);
	}
	handled = true;
	break;

	/*       case vchrSendData: */
	/* 	ListViewDoCommand(ListRecordBeamCategoryCmd); */
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
      ListViewSelectCategory();
      handled = true;
      break;

    case ThumbnailListButton:
      FrmGotoForm(DiddleThumbnailForm);
      handled = true;
      break;

    case ThumbnailDetailListButton:
      FrmGotoForm(DiddleThumbnailDetailForm);
      handled = true;
      break;
    }
    break;

  case tblSelectEvent:
    /* An item in the list of memos was selected, display it. */
    p.dbI = TblGetRowID (event->data.tblSelect.pTable, event->data.tblSelect.row);

    /* Get the category and secret attribute of the current record. */
    DmRecordInfo (d.dbR, p.dbI, &attr, NULL, NULL);

    /* If this is a "private" record, then determine what is to be shown. */
    if (attr & dmRecAttrSecret)	{
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

    handled = true;
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
	ListDoCmdSecurity();
	handled = true;
	break;

      case menuitemID_CmdSortByAlarm:      
      case menuitemID_CmdSortByName:
	if (event->data.menu.itemID == menuitemID_CmdSortByAlarm)
	  DmInsertionSort(d.dbR, &SortByAlarmTime, 0);
	else
	  DmInsertionSort(d.dbR, &SortByName, 0);

	frm = FrmGetActiveForm();
	SetTopVisibleRecord(0);
	ListViewLoadRecords(frm);
	TblRedrawTable(GetObjectPointer(frm, SketchTable));
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
    ListViewInit(frm);
    FrmDrawForm(frm);

    /* Disable 5-way navigation initially */
    d.fiveWayNavigation = false;

    /* Clean up */
    if (d.record_name) MemHandleFree(d.record_name);
    d.record_name = NULL;
    if (d.record_note)MemHandleFree(d.record_note);
    d.record_note = NULL;

    handled = true;
    break;

  case menuCmdBarOpenEvent:
    MenuCmdBarAddButton(menuCmdBarOnLeft, BarSecureBitmap,
			menuCmdBarResultMenuItem, menuitemID_CmdSecurity, 0);

    /* tell the field package to not add buttons automatically; */
    /* we've done it all ourselves.                             */
    event->data.menuCmdBarOpen.preventFieldButtons = true;

    /* don't set handled to true; this event must fall through to the system. */
    break;

  case frmUpdateEvent:
    handled = ListViewUpdateDisplay(event->data.frmUpdate.updateCode);
    break;

  case sclRepeatEvent:
    ListViewScroll(event->data.sclRepeat.newValue - event->data.sclRepeat.value);
    break;

  case dbNonKeyCommandEvent:
    handled = HandleNonKeyCommandCode(event->data.menu.itemID);
    break;

  default:
    /* ignore */
  }

  return handled;
}
