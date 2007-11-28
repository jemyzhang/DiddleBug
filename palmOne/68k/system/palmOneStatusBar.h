/*******************************************************************************
 * Copyright (c) 2004-2005 palmOne, Inc. or its subsidiaries.
 * All rights reserved.
 ******************************************************************************/
/** 
 * @defgroup	StatusBar Status Bar Library
 * @brief		This library provides support to manipulate the state of the status
 *              bar on devices that utilize the Dynamic Input Area (DIA).
 *
 * Devices such as Tungsten T3 and Tungsten T5 allow users to collapse/expand the input
 * area to support tall forms. This library allows the status/control bar to be hidden
 * programmatically so that forms can be displayed full screen.
 *
 * Most of the APIs in the library should already exist in Palm OS SDK, and applications
 * that are developed for DIA-enabled OS should first try to use the PalmSource's status
 * bar API.
 *
 * @{
 * @}
 */
/**
 * @ingroup StatusBar
 */
 
/**
 * @file 	palmOneStatusBar.h
 * @version 1.0
 * @brief	Public common header file for palmOne defined Status Bar library.
 * 
 * This file contains the function prototypes and constants for the palmOne Status Bar Library
 * <hr>
 */


#ifndef __STATUSBARMGRLIB_H__
#define __STATUSBARMGRLIB_H__

#include <PalmTypes.h>
#include <LibTraps.h>


/** 
 * @name Library Constants
 *
 */
/*@{*/
#define palmOneStatusBarLibName			"StatusBarMgrLib"	/**< StatusBar library name	*/
#define palmOneStatusBarLibCreator		'sBar'				/**< StatusBar library creator ID */
#define palmOneStatusBarLibType			sysFileTLibrary		/**< Default library type */
#define palmOneStatusBarFtrNumVersion	(0)					/**< Version of the currently supported features */
/*@}*/

/** 
 * @name Error Codes
 *
 */
/*@{*/
#define palmOneStatusBarErrorClass			(appErrorClass  | 0x0900)
#define palmOneStatusBarErrNoStatusBar		(palmOneStatusBarErrorClass | 1)
#define palmOneStatusBarErrInvalidSelector	(palmOneStatusBarErrorClass | 2)
#define palmOneStatusBarErrInputWindowOpen 	(palmOneStatusBarErrorClass | 3)
#define palmOneStatusBarErrBadParam			(palmOneStatusBarErrorClass | 101)
#define palmOneStatusBarErrInvalidState		(palmOneStatusBarErrorClass | 102)
/*@}*/

/**
 * Status Bar Attributes
 **/
typedef enum PalmOneStatAttrTypeTag
{
    palmOneStatusBarAttrExists     = 0,         /**< device supports the status bar */
    palmOneStatusBarAttrBarVisible,             /**< status bar is visible */
    palmOneStatusBarAttrDimension               /**< bounds of status bar window */
} PalmOneStatAttrType;


/** 
 * @name Function Traps
 *
 */
/*@{*/
#define kPalmOneStatusBarMgrLibTrapOpen		sysLibTrapOpen
#define kPalmOneStatusBarMgrLibTrapClose	sysLibTrapClose
#define kPalmOneStatusBarMgrLibGetAttribute	(sysLibTrapCustom)
#define kPalmOneStatusBarMgrLibHide			(sysLibTrapCustom + 1)
#define kPalmOneStatusBarMgrLibShow			(sysLibTrapCustom + 2)
/*@}*/

/**
 * Prototypes
 **/
#ifdef __cplusplus
extern "C" {
#endif
   
/**
 * @brief Standard open library routine
 *
 * @param refnum:	IN: Library reference number
 * @return Error code.
 **/
Err PalmOneStatusBarLibOpen(UInt16 refnum)
				SYS_TRAP(kPalmOneStatusBarMgrLibTrapOpen);

/**
 * @brief Standard close library routine
 *
 * @param refnum:	IN: Library reference number
 * @return Error code.
 **/				
Err PalmOneStatusBarLibClose(UInt16 refnum)
				SYS_TRAP(kPalmOneStatusBarMgrLibTrapClose);

/**
 * @brief Retrieve the current state of the status bar
 *
 * @param refnum:	IN: Library reference number
 * @param selector:	IN: Attribute selector (dimension or status)
 * @param *dataP:	IN: Pointer to the returned attribute value.
 * @return Error code.
 **/
Err PalmOneStatusBarGetAttribute(UInt16 refnum, PalmOneStatAttrType selector, UInt32 *dataP)
				SYS_TRAP(kPalmOneStatusBarMgrLibGetAttribute);

/**
 * @brief Hide the status bar from the bottom of the screen
 *
 * @param refnum:	IN:  Library reference number
 * @return Error code.
 **/				
Err PalmOneStatusBarHide(UInt16 refnum)
				SYS_TRAP(kPalmOneStatusBarMgrLibHide);

/**
 * @brief Unhide/show the status bar at the bottom of the screen
 *
 * @param refnum:	IN:  Library reference number
 * @return Error code.
 **/				
Err PalmOneStatusBarShow(UInt16 refnum)
				SYS_TRAP(kPalmOneStatusBarMgrLibShow);

#ifdef __cplusplus
}
#endif

#endif  //__STATUSBARMGRLIB_H__
