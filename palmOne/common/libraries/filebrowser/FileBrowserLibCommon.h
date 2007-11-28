/*******************************************************************************
 * Copyright (c) 2004-2005 palmOne, Inc. or its subsidiaries.
 * All rights reserved.
 ******************************************************************************/
/**
 * @defgroup	FileBrowser File Browser Library
 * @brief		This library is used to provide support for Files application
 *              integration.
 *
 * Applications which want to utilize the File Browser UI or register itself with
 * the Files application on Tungsten devices should use this library to do so.
 * Registration with the Files app has to be done in addition to registration with
 * the Exchange Library.
 *
 * Registering with the Files application is also done using Exchange Library but
 * with palmOne defined registration IDs (exgRegEditCreatorID, etc). This will enable
 * the application to be launched automatically when a certain file type or extension
 * is selected on the Files app. Developers can also use the APIs available in this
 * library to present File Browser UI in their own application to the user (open and
 * save dialogs).
 *
 * @{
 * @}
 */
/**
 * @ingroup FileBrowser
 */

/**
 * @file 	FileBrowserLibCommon.h
 * @version 1.0
 * @brief	Public 68K common header file for File Browser library API.
 *
 * This file contains the File Browser Library common definitions and error codes.
 * <hr>
 */


#ifndef __FILE_BROWSER_LIB_COMMON_H__
#define __FILE_BROWSER_LIB_COMMON_H__

#include <ExgLib.h>

/**
 * Use SysLibFind to find this library. There is no need to load it.
 * This must match the database name since this is an Exchange
 * Library In Application (ELIA).
 */
#define kFileBrowserLibName        "FileBrowser-PFil"

#define kFileBrowserLibFlagOneVolume        0x00000001UL  /**< Flags for Open and Save As dialogs*/

/** @name Flags for Open dialog */
/*@{*/
#define kFileBrowserLibFlagNoFiles          0x00000002UL  /**< no files; only folders */
#define kFileBrowserLibFlagNoFolders        0x00000004UL  /**< no folders; only files */
/*@}*/

/** @name Flags for Save As dialog */
/*@{*/
#define kFileBrowserLibFlagPromptOverwrite  0x00000008UL  /**< warn before replacing */
#define kFileBrowserLibFlagRequireExtension 0x00000010UL  /**< only given extensions */
#define kFileBrowserLibFlagNoNewFolder      0x00000020UL  /**< no New Folder button */
/*@}*/

/** @name Flags for Open & Save As dialog */
/*@{*/
#define kFileBrowserLibFlagNoDirChange		0x00000040UL  /**< stay within tree */
#define kFileBrowserLibFlagNoSubDir			0x00000080UL  /**< stay within folder */
/*@}*/

// Buffer sizes
#define kFileBrowserLibMaxPathLength        2047  // room for most paths
#define kFileBrowserLibPathBufferSize       (kFileBrowserLibMaxPathLength + 1)

// Exchange Manager registry enums
//<><> Remove when PalmSource adds these to their SDK
#define exgRegEditCreatorID                 0xff7b  // creator ID registry
#define exgRegEditExtensionID               0xff7d  // filename extension registry
#define exgRegEditTypeID                    0xff7e  // MIME type registry

// ExgControl opcodes
#define kFileBrowserLibCtlGetVersion        exgLibCtlSpecificOp  // API version

// File Browser library API versions
#define kFileBrowserLibAPIVersion0          0  // original version on Tungsten T5
#define kFileBrowserLibAPIVersion1          1  // version with multiselect

/** @name Error Codes */
/*@{*/
#define kFileBrowserLibErrorClass       (oemErrorClass + 0x0400)
#define kFileBrowserLibErrStillOpen     (kFileBrowserLibErrorClass | 0x01)
#define kFileBrowserLibErrPathTooLong   (kFileBrowserLibErrorClass | 0x02)
#define kFileBrowserLibErrMemError      (kFileBrowserLibErrorClass | 0x03)
#define kFileBrowserLibErrCantFind      (kFileBrowserLibErrorClass | 0x04)
#define kFileBrowserLibErrInternal      (kFileBrowserLibErrorClass | 0x05)
#define kFileBrowserLibErrCantOpen      (kFileBrowserLibErrorClass | 0x06)
#define kFileBrowserLibErrCantParse		(kFileBrowserLibErrorClass | 0x07)
/*@}*/

/** @name Function Traps */
/*@{*/
#define kFileBrowserLibTrapShowOpenDialog   		(exgLibTrapLast)
#define kFileBrowserLibTrapShowSaveAsDialog 		(exgLibTrapLast + 1)
#define kFileBrowserLibTrapParseFileURL				(exgLibTrapLast + 2)
#define kFileBrowserLibTrapShowMultiselectDialog	(exgLibTrapLast + 3)
/*@}*/

#endif // __FILE_BROWSER_LIB_COMMON_H__
