/*******************************************************************************
 * Copyright (c) 2004-2005 palmOne, Inc. or its subsidiaries.
 * All rights reserved.
 ******************************************************************************/
/**
 * @defgroup	HiRes Hi-Resolution Timer Library
 * @brief		This library provides support for high resolution timer on Palm OS
 *				devices.
 *
 * Applications which would like to use a more granular (higher resolution) timer
 * than the standard Palm OS sysTicksPerSecond() can use this library to do so.
 *
 * @{
 * @}
 */
/**
 * @ingroup HiRes
 */

/**
 * @file	palmOneHiResTimerCommon.h
 * @version 1.0
 * @brief	Public 68k common header file for Hi-Resolution Timer API.
 *
 * This file contains the common library constants and error codes.
 * <hr>
 */

#ifndef _PALMONEHIRESTIMERCOMMON_H_
#define _PALMONEHIRESTIMERCOMMON_H_

#include <PalmTypes.h>
#include <ErrorBase.h>
#include <SystemMgr.h>


/***********************************************************************
 * Type and creator of the Library
 ***********************************************************************/

#define		kHiResTimeLibType		'libr'				/**< Hi-Res timer library database type */
#define		kHiResTimeLibCreator	'pHRT'				/**< Hi-Res timer library database creator */

/***********************************************************************
 * Internal library name which can be passed to SysLibFind()
 ***********************************************************************/

#define		kHiResTimeLibName		"HRTimer-Lib"   /**< Hi-Res timer library name */

/***********************************************************************
 * Library versioning
 ***********************************************************************/

/**
 * @name Library Versions
 */
/*@{*/
#define		kHiResTimeLibVersion1	sysMakeROMVersion(1, 0, 0, sysROMStageRelease, 0) /**< Hi-Res timer library version 1 */
#define		kHiResTimeLibVersion2	sysMakeROMVersion(2, 0, 0, sysROMStageRelease, 0) /**< Hi-Res timer library version 2 */
#define		kHiResTimeLibVersion	kCamLibVersion2 /**< Hi-Res timer library version 2 */
/*@}*/

/***********************************************************************
 * HiRes Timer result codes
 ***********************************************************************/

/**
 * @name Error Codes
 */
/*@{*/
/** Hi-Res timer base error number */
#define kHiResTimeErrorClass			(oemErrorClass + 0x100)
/** Bad Parameter */
#define kHiResTimeLibErrBadParam		(kHiResTimeErrorClass | 0x01)
/** Library is not open */
#define kHiResTimeLibErrNotOpen			(kHiResTimeErrorClass | 0x02)
/** Returned from HRTimeLibClose() if the library is still open */
#define kHiResTimeLibErrStillOpen		(kHiResTimeErrorClass | 0x03)
/** Fucntion not availabe - If returned by HRTimeLibOpen, there is no HiRes Timer available. */
#define kHiResTimeLibErrNotAvailable	(kHiResTimeErrorClass | 0x04)
/*@}*/

/***********************************************************************
 * Library trap
 ***********************************************************************/

/**
 * @name Function Traps
 */
/*@{*/
#define kHRLibTrapTimerTicksPerSecond 	(sysLibTrapCustom)
#define kHRLibTrapTimerGetTime			(sysLibTrapCustom+1)
/*@}*/

#endif  // _PALMONEHIRESTIMERCOMMON_H_
