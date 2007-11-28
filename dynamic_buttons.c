/*

(c) 2001, 2002, 2003, 2004 Peter Putzer
Licensed under the GNU General Public License (GPL).

*/

#include "dynamic_buttons.h"
#include "debug.h"

/* Hi-res multiplier */
#define HiResCoef 2

/*
** Returns the correct background color for the form.
*/
extern inline IndexedColorType FormBackColor(const DynamicButtonType* btn) {
  if (!WinModal(FrmGetWindowHandle(FrmGetFormPtr(btn->frmID))))
    return UIColorGetTableEntryIndex(UIFormFill);
  else
    return UIColorGetTableEntryIndex(UIDialogFill);
}

/*
** Creates a new dynamic button and returns a locked pointer to it.
*/
DynamicButtonType* DynBtnCreate(UInt16 id, FrameType f, Boolean gray, Boolean full,
				Boolean screenDepth, Boolean hires, UInt16 formID,
				DynBtnType type, Int16 text_maxlength,
				const RectangleType* rect, Err* error) {
  DynamicButtonType* btn
    = (DynamicButtonType*) MemPtrNew(sizeof(DynamicButtonType));
  ASSERT(btn);

  if (!btn) {
    *error = 1;
    return NULL;
  }

  btn->id = id;
  btn->type = type;
  btn->frame = f;
  btn->gray = gray;
  btn->selected = false;
  btn->enabled = true;
  btn->frmID = formID;
  RctCopyRectangle(rect, &btn->frameRect);
  btn->font = boldFont;
  btn->center = false;
  btn->hires = hires;

  /*
  ** Calculate the size of the bitmap (and its position)
  */
  if (!full) {
    WinGetFramesRectangle(btn->frame, rect, &btn->contentRect);
    btn->contentRect.topLeft.x = rect->topLeft.x * 2 - btn->contentRect.topLeft.x;
    btn->contentRect.topLeft.y = rect->topLeft.y * 2 - btn->contentRect.topLeft.y;
    btn->contentRect.extent.x = rect->extent.x * 2 - btn->contentRect.extent.x;;
    btn->contentRect.extent.y = rect->extent.y * 2 - btn->contentRect.extent.y;;
  } else {
    RctCopyRectangle(rect, &btn->contentRect);
  }

  if (type == dynBtnGraphical) {
    BitmapTypeV3* bmpV3 = NULL;
    Coord x, y;

      if (btn->hires) {
	/* Set things up for hi-res */
	if (!WinGetDrawWindow())
	  WinSetDrawWindow(WinGetDisplayWindow());
	WinPushDrawState();
	WinSetCoordinateSystem(kCoordinatesNative);
	x = WinScaleCoord(btn->contentRect.extent.x, true);
	y = WinScaleCoord(btn->contentRect.extent.y, true);
	WinPopDrawState();
      } else {
	x = btn->contentRect.extent.x;
	y = btn->contentRect.extent.y;
	}

    if (!screenDepth) {
      btn->bmp = BmpCreate(x, y, 1, NULL, error);
    } else {
      UInt32 depth = 0;
      WinScreenMode(winScreenModeGet, NULL, NULL, &depth, NULL);
      btn->bmp = BmpCreate(x, y, depth, NULL, error);
    }

    if (*error) {
      MemPtrFree((MemPtr)btn);
      return NULL;
    }

      if (btn->hires) {
	/* Make this a double density bitmap */
	bmpV3 = BmpCreateBitmapV3(btn->bmp, kDensityDouble, BmpGetBits(btn->bmp), NULL);
	ASSERT(bmpV3);
	btn->content.bmpW = WinCreateBitmapWindow((BitmapType*) bmpV3, error);
	ASSERT(btn->content.bmpW);
      } else {
	btn->content.bmpW = WinCreateBitmapWindow(btn->bmp, error);
	ASSERT(btn->content.bmpW);
	btn->bmp = NULL;
      }

    /* Make background "transparent" */
    if (screenDepth) {
      const WinHandle oldH = WinSetDrawWindow(btn->content.bmpW);
      WinSetBackColor(FormBackColor(btn));
      WinEraseWindow();
      WinSetDrawWindow(oldH);
    }

    if (*error) {
      if (bmpV3) BmpDelete((BitmapType*) bmpV3);
      if (btn->bmp) BmpDelete(btn->bmp);
      MemPtrFree((MemPtr)btn);
      return NULL;
    }
  } else {
    btn->content.text = (Char*) MemPtrNew(sizeof(Char) * text_maxlength + 1);
    ASSERT(btn->content.text);
    if (!btn->content.text) {
      *error = 1;
      return NULL;
    }
    MemSet(btn->content.text, text_maxlength + 1, 0);
  }

  return btn;
}

/*
** Initializes a gadget with the dynamic button data and event handler given.
**
** Clients should discard the returned DynamicButton if err is non-NULL.
*/
DynamicButtonType* DynBtnInitGadget(UInt16 id, FrameType f, Boolean gray, 
				    Boolean full, Boolean screenDepth, 
				    Boolean hires, DynBtnType type,
				    Int16 text_maxlength, Err* err,
				    FormType* frm, FormGadgetHandlerType handler) {
  DynamicButtonType* btn = NULL;
  RectangleType r;
  const UInt16 idx = FrmGetObjectIndex(frm, id);
  
  ASSERT(frm);

  FrmGetObjectBounds(frm, idx, &r);
  btn = DynBtnCreate(id, f, gray, full, screenDepth,
		     hires, FrmGetFormId(frm), type, text_maxlength, &r, err);
  ASSERT(btn);

  if (!*err) {
    FrmSetGadgetData(frm, idx, btn);
    FrmSetGadgetHandler(frm, idx, handler);
  }

  return btn;
}

/*
** Deletes the given dynamic button and frees the memory.
*/
void DynBtnDelete(DynamicButtonType* btn) {
  if (!btn) return;

  if (btn->type == dynBtnGraphical) {
    BitmapType* bmp = WinGetBitmap(btn->content.bmpW);
    ASSERT(bmp);

    WinDeleteWindow(btn->content.bmpW, false);
    BmpDelete(bmp);
    if (btn->bmp) BmpDelete(btn->bmp);
  } else {
    MemPtrFree((MemPtr)btn->content.text);
  }

  MemPtrFree((MemPtr) btn);
}

/*
** Returns the correct foreground color depending on the selection state.
*/
extern inline IndexedColorType ForeColor(const DynamicButtonType* btn) {
  if (btn->selected)
    return UIColorGetTableEntryIndex(UIObjectSelectedForeground);
  else
    return UIColorGetTableEntryIndex(UIObjectForeground);
}

/*
** Draws the given dynamic button onto the current drawing window.
*/
void DynBtnDraw(DynamicButtonType* btn) {
  const IndexedColorType foreColor = ForeColor(btn);

  /* Store drawing state */
  WinPushDrawState();

  /* Set background to object fill */
  if (btn->selected)
    WinSetBackColor(UIColorGetTableEntryIndex(UIObjectSelectedFill));
  else if (btn->frame != noFrame)
    WinSetBackColor(UIColorGetTableEntryIndex(UIObjectFill));

  /* Erase button (showing the select color if necessary */
  WinEraseRectangle(&btn->frameRect, 0);

  if (btn->type == dynBtnGraphical) {
    /* Set the foreground color */
    WinSetForeColor(foreColor);

    /* Draw the bitmap label */
      WinSetDrawMode(winErase);
      WinPaintBitmap(WinGetBitmap(btn->content.bmpW), btn->contentRect.topLeft.x, btn->contentRect.topLeft.y);
      WinSetDrawMode(winOverlay);
      WinPaintBitmap(WinGetBitmap(btn->content.bmpW), btn->contentRect.topLeft.x, btn->contentRect.topLeft.y);
  } else {
    Coord x, y;

    /* Prepare drawing context */
    WinSetTextColor(foreColor);
    FntSetFont(btn->font);
    WinSetUnderlineMode(noUnderline);

    /* Calculate position */
    if (btn->center) {
      const Int16 len = FntCharsWidth(btn->content.text, StrLen(btn->content.text));
      x = btn->contentRect.topLeft.x + (btn->contentRect.extent.x - len) / 2;
      y = btn->contentRect.topLeft.y + (btn->contentRect.extent.y - FntCharHeight()) / 2;
    } else {
      x = btn->contentRect.topLeft.x;
      y = btn->contentRect.topLeft.y;
    }

    /* Draw the text label */
    WinPaintChars(btn->content.text, StrLen(btn->content.text), x, y);
  }

  /* Set foreground to frame color and background to form fill */
  WinSetForeColor(UIColorGetTableEntryIndex(UIObjectFrame));

  /* Is this a dialog (i.e. modal) form? */
  WinSetBackColor(FormBackColor(btn));

  /* Draw the frame */
  if (btn->gray)
    WinDrawGrayRectangleFrame(btn->frame, &btn->frameRect);
  else
    WinDrawRectangleFrame(btn->frame, &btn->frameRect);

  WinPopDrawState();
}

/*
** Tracks pen movement in the dynamic button.
*/
Boolean DynBtnTrackPen(DynamicButtonType* btn) {
  UInt16 x, y;
  Boolean penDown, oldSel;

  ASSERT(btn);

  /* Redraw button */
  btn->selected = true;
  DynBtnDraw(btn);

  do {
    EvtGetPen(&x, &y, &penDown);

    oldSel = btn->selected;
    btn->selected = RctPtInRectangle(x, y, &btn->frameRect);
    if (oldSel != btn->selected) 
      DynBtnDraw(btn);

  } while (penDown);

  /* Redraw button */
  btn->selected = false;
  DynBtnDraw(btn);

  return RctPtInRectangle(x, y, &btn->frameRect);
}
