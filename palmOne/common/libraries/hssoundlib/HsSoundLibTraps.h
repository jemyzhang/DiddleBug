/*******************************************************************************
 * Copyright (c) 2004-2005 palmOne, Inc. or its subsidiaries.
 * All rights reserved.
 ******************************************************************************/
/**
 * @ingroup Sound
 */

/**
 * file 	HsSoundLibTraps.h
 * brief	Public 68k common header file for Sound Library on Treo devices.
 *
 * This file contains the function trap numbers for the Sound Library APIs.
 */

#ifndef __HSSOUNDLIB_TRAPS__H__
#define __HSSOUNDLIB_TRAPS__H__

/***********************************************************************
 * Palm OS common definitions
 ***********************************************************************/
#include <PalmTypes.h>

#if 0
#pragma mark -------- Constants --------
#endif

/**
 * @name Function Traps
 *
 */
/*@{*/
#define kHsSoundLibTrapOpen			  (sysLibTrapOpen)  /**<No definition. */
#define kHsSoundLibTrapClose		  (sysLibTrapClose) /**<No definition. */

#define kHsSoundLibTrapGetVersion     (sysLibTrapCustom + 0) /**<No definition. */
#define kHsSoundLibTrapPortGetInfo	  (sysLibTrapCustom + 1) /**<No definition. */
#define kHsSoundLibTrapPortSetParam	  (sysLibTrapCustom + 2) /**<No definition. */
#define kHsSoundLibTrapSwitchGetPort  (sysLibTrapCustom + 3) /**<No definition. */
#define kHsSoundLibTrapSwitchSetPort  (sysLibTrapCustom + 4) /**<No definition. */
#define kHsSoundLibTrapTonePlay		  (sysLibTrapCustom + 5) /**<No definition. */
#define kHsSoundLibTrapToneStop		  (sysLibTrapCustom + 6) /**<No definition. */
#define kHsSoundLibTrapFormatPlay	  (sysLibTrapCustom + 7) /**<No definition. */
#define kHsSoundLibTrapFormatRecord	  (sysLibTrapCustom + 8) /**<No definition. */
#define kHsSoundLibTrapGetUserVolume  (sysLibTrapCustom + 9) /**<No definition. */
#define kHsSoundLibTrapSetUserVolume  (sysLibTrapCustom + 10) /**<No definition. */
#define kPmSoundLibTrapClkGetTicksPerSecond (sysLibTrapCustom + 11) /**<No definition. */
#define kPmSoundLibTrapClkGetTickCount (sysLibTrapCustom + 12) /**<No definition. */
#define kPmSoundLibTrapGetPosition     (sysLibTrapCustom + 13) /**<No definition. */
#define kPmSoundLibTrapReserved        (sysLibTrapCustom + 14) /**<No definition. */
/*@}*/

#endif // INCLUDE ONCE -- __HSSOUNDLIB_TRAPS__H__
