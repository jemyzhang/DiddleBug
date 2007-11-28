/*******************************************************************************
 * Copyright (c) 2004-2005 palmOne, Inc. or its subsidiaries.
 * All rights reserved.
 ******************************************************************************/
/**
 *  @defgroup	HWUtil Hardware Utility Library
 *  @brief		This library provides support for applications to control the state
 *				of certain pieces of hardware on the device.
 *
 *  This library currently allows the application to programmatically turn the
 *  display on/off as well as controling the behavior of the LED light if available.
 *
 *  @{
 *  @}
 */
/**
 * @ingroup HWUtil
 */

/**
 * @file 	HardwareUtils68K.h
 * @version 1.0
 * @brief   Public common header file to the Hardware Utilities Library used by 68K applications.
 * 
 * This file contains the library constants and function prototypes for the library.
 * <hr>
 */

#ifndef __HWUTILS68K_H__
#define	__HWUTILS68K_H__

#include <PalmTypes.h>
#include <LibTraps.h>

/**
 * @name Library Constants
 *
 */
/*@{*/
#define kHWUtilsType					sysFileTLibrary		/**<		*/
#define kHWUtilsCreator					'HWut'			/**<		*/
#define kHWUtilsName					"HardwareUtils"		/**<		*/
/*@}*/

/**
 * @name Function Traps
 */
/*@{*/
#define kHWUtilsTrapOpen				sysLibTrapOpen
#define kHWUtilsTrapClose				sysLibTrapClose
#define kHWUtilsTrapBlinkLED			(sysLibTrapCustom + 0)
#define kHWUtilsTrapSetBlinkRate		(sysLibTrapCustom + 1)
#define kHWUtilsTrapEnableDisplay		(sysLibTrapCustom + 2)
#define	kHWUtilsTrapGetDisplayState		(sysLibTrapCustom + 3)
/*@}*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @brief Standard library open routine
 *
 *  @param refnum:	IN: Library reference number
 *  @return Error code.
 **/
Err HWUOpen(UInt16 refnum)
			SYS_TRAP(kHWUtilsTrapOpen);

/**
 *  @brief Standard library close routine
 *
 *  @param refnum:	IN: Library reference number
 *  @return Error code.
 **/
Err HWUClose(UInt16 refnum)
			SYS_TRAP(kHWUtilsTrapClose);

/**
 *  @brief Set the LED blink state
 * 
 *  @param refnum:	IN: Library reference number
 *  @param b:		IN: True if LED should be blinking. False otherwise.
 *  @return Error code
 **/
Err HWUBlinkLED(UInt16 refnum, Boolean b)
			SYS_TRAP(kHWUtilsTrapBlinkLED);

/**
 *  @brief Set the blink rate of the LED light
 *
 *  @param refnum:	IN: Library reference number
 *  @param rate:	IN: The rate at which the LED should blink.
 *  @return Error code.
 **/
Err HWUSetBlinkRate(UInt16 refnum, UInt16 rate)
			SYS_TRAP(kHWUtilsTrapSetBlinkRate);

/**
 *  @brief Set the state of the display
 *
 *  @param refnum:	IN: Library reference number
 *  @param on:		IN: True to enable display. False otherwise.
 *  @return Error code.
 **/
Err	HWUEnableDisplay(UInt16 refnum, Boolean on)
			SYS_TRAP(kHWUtilsTrapEnableDisplay);

/**
 *  @brief Retrieve the current state of the display
 *
 *  @param refnum:	IN: Library reference number
 *  @return True if the display is on. False otherwise.
 **/
Boolean HWUGetDisplayState(UInt16 refnum)
			SYS_TRAP(kHWUtilsTrapGetDisplayState);

#ifdef __cplusplus 
}
#endif

#endif 	//__HWUTILS68K_H__

