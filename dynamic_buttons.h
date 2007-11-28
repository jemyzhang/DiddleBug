#ifndef DYNAMIC_BUTTONS_H
#define DYNAMIC_BUTTONS_H

/*

(c) 2001,2002,2003 Peter Putzer
Licensed under the GNU General Public License (GPL).

*/

#include <PalmOS.h>
#include "sections.h"

typedef enum {
  dynBtnGraphical, /* a button containing a bitmap */
  dynBtnText	   /* a button containing a string */
} DynBtnType;

/*
** Structure for "dynamic buttons" (really extended gadgets)
*/
typedef struct {
  UInt16 id;	             /* Gadget Resource ID */
  DynBtnType type;	     /* graphical or text button? */
  FrameType frame;           /* the frame style */
  Boolean gray;		     /* draw a gray frame? */
  RectangleType frameRect;   /* the rectangle for the frame */
  RectangleType contentRect; /* the rectangle for the content (bitmap or text) */
  Boolean selected;	     /* selected? */
  Boolean enabled;           /* interaction enabled? */
  UInt16 frmID;              /* the ID of the gadget's form */
  FontID font;               /* font ID for text buttons */
  Boolean center;            /* center button text? */
  BitmapType* bmp;           /* Auxilliary bitmap for OS5 hi-res */
  Boolean hires;             /* Should the content bitmap be hi-res? */
 
  union {
    WinHandle bmpW;	     /* the bitmap for the gadget (wrapped in a window) */
    Char* text;		     /* the text for the gadget */
  } content;

  UInt16 value;              /* some user-defined data */

} DynamicButtonType;

/*
** Creates a new dynamic button and returns a locked pointer to it.
*/
extern DynamicButtonType* DynBtnCreate(UInt16 id, FrameType f, Boolean gray, Boolean full,
				       Boolean screenDepth, Boolean sonyClie,
				       UInt16 formID, DynBtnType type, 
				       Int16 text_maxlength,
				       const RectangleType* rect, Err* error);

/*
** Initializes a gadget with the dynamic button data and event handler given.
**
** Clients should discard the returned DynamicButton if err is non-NULL.
*/
extern DynamicButtonType* DynBtnInitGadget(UInt16 id, FrameType f, Boolean gray,
					   Boolean full, Boolean screenDepth, 
					   Boolean hires, DynBtnType type,
					   Int16 text_maxlength, Err* err,
					   FormType* frm, FormGadgetHandlerType handler);
/*
** Deletes the given dynamic button and frees the memory.
*/
extern void DynBtnDelete(DynamicButtonType* btn);

/*
** Draws the given dynamic button onto the current drawing window.
*/
extern void DynBtnDraw(DynamicButtonType* btn);

/*
** Tracks pen movement in the dynamic button
*/
extern Boolean DynBtnTrackPen(DynamicButtonType* btn);

#endif
