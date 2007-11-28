//**************************************************************
// Copyright (c) 2004 palmOne, Inc. or its subsidiaries.
// All rights reserved.
//**************************************************************

// <chg 2005-03-09 doc> Took out Doxygen comments and any /* comments
//	because this header is included by resource files and Palm-RC
//	chokes on them.

//**************************************************************
//
// ToDo:
//
// History:
//	  2001-12-18  dia	Creaded by Doug Anderson
//	  2002-01-03  dia	We now have a reserved range to keep
//						real bitmaps and strings in the resource
//						chain, so we don't need the flags to tell
//						us whether the user wanted ours; moved
//						resource IDs to HsResourceIDs.h.
//
//***************************************************************/


#ifndef __PM_SYS_GADGET_BBUT_RSC__H__
#define __PM_SYS_GADGET_BBUT_RSC__H__ 1

// Use this version
// ----------------
#define kPmSysGadgetBbutMappingVersion0				0


// Use these flags
// ---------------
#define kPmSysGadgetBbutFlagNone					0x00  // None
#define kPmSysGadgetBbutFlagNoBorder				0x01  // Don't draw the border...
#define kPmSysGadgetBbutFlagNoBackground			0x02  // Don't draw the background of the button
#define kPmSysGadgetBbutFlagNoBorderOrBackground	0x03  // Combination for Palm-rc...


// Use these fonts
// ---------------
#define kPmSysGadgetBbutFontStd					0
#define kPmSysGadgetBbutFontBold				1
#define kPmSysGadgetBbutFontLarge				2
#define kPmSysGadgetBbutFontSymbol				3
#define kPmSysGadgetBbutFontSymbol11			4
#define kPmSysGadgetBbutFontSymbol7				5
#define kPmSysGadgetBbutFontLed					6
#define kPmSysGadgetBbutFontLargeBold			7

#endif // ifndef __PM_SYS_GADGET_BBUT_RSC__H__
