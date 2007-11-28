/*******************************************************************************
 * Copyright (c) 2004-2005 palmOne, Inc. or its subsidiaries.
 * All rights reserved.
 ******************************************************************************/
/**
 * @defgroup	Versamail Versamail Library
 * @brief		This library provide supports for application that wish to
 *				provide users with the Versamail Font Picker dialog and support
 *				the font from with the app.
 *
 * This library also allows application to programmatically manage style and size
 * for the font types that Versamail support. Other VersaMail supports include
 * launching VersaMail to add a new email with attachment and also the ability
 * to create a plugin to send/view attachments.
 *
 * @{
 * @}
 */
/**
 * @ingroup Versamail
 */

/**
 * @file 	VMFontLibCommon.h
 * @version 1.0
 * @brief	Public common header file for the Versamail Font Library.
 *
 * This file contains the constants and structures for the palmOne VersaMail
 * Font Picker / Font Manager Service.
 */

#ifndef __PALMONEVMFONTLIBCOMMON_H__
#define __PALMONEVMFONTLIBCOMMON_H__

#include <PalmTypes.h>
#include <Font.h>
#include <DataMgr.h>

/********************************************************************
 * Traps
 ********************************************************************/
/**
 * @name Function Traps
 */
/*@{*/
#define  flTrapAPIVersion				(sysLibTrapCustom + 0)
#define  flTrapAPIGetFont				(sysLibTrapCustom + 1)
#define  flTrapAPIGetFontByID			(sysLibTrapCustom + 2)
#define  flTrapAPIFontExists			(sysLibTrapCustom + 3)
#define  flTrapAPIFontToString			(sysLibTrapCustom + 4)
#define  flTrapAPIFontCountStyleAndSize (sysLibTrapCustom + 5)
#define  flTrapAPISetFontUIRules		(sysLibTrapCustom + 6)
#define  flTrapAPIInitFontUI			(sysLibTrapCustom + 7)
#define  flTrapAPIDoFontDialog			(sysLibTrapCustom + 8)
#define	 flTrapAPIClearCache			(sysLibTrapCustom + 9)
/*@}*/

/********************************************************************
 * Library type and creator
 ********************************************************************/

#define fontPickerLibName			"PalmSGFontLib.Lib" /**< Font library name. */
#define fontPickerType				'appl'				/**< Font library Type. */
#define fontPickerCreatorID			'MA10'				/**< Font library Creator ID. */


/********************************************************************
 * Typedefs and constants used in Font Picker lib
 ********************************************************************/

/** A parameter passed was Invalid */
#define VMFontErrParam			(appErrorClass | 1)

/** Error meaning that the library is not open */
#define VMFontErrNotOpen		(appErrorClass | 2)

/** Error returned from VMFontClose() if the library is still open */
#define VMFontErrStillOpen	(appErrorClass | 3)

/**
 * Font Face
 *
 */
enum _VMFontFace {
	gillSans  = 0,			/**< Gill Sans MT Font  */
	sabon,					/**< Sabon MT Font  */
	//segoe,				/**< Segoe MT Font  */
	//thorndale, 			/**< ThornDale MT Font  */
	palm					/**< Palm  */
};

typedef UInt8 VMFontFace; /**< Used with _VMFontFace */

/**
 * Font Size
 *
 */
enum _VMFontSize {
	size6  = 0,			/**< Size = 6  */
	size9  = 1,			/**< Size = 9  */
	size12 = 2			/**< Size = 12  */
};

typedef UInt8 VMFontSize; /**< Used with _VMFontSize */

/**
 * Font Style
 *
 */
enum _VMFontStyle {
	normal = 0,			/**< Normal Fonts */
	bold   = 1,			/**< Bold Fonts */
	italic = 2			/**< Italic Fonts */
};

typedef UInt8 VMFontStyle; /**< Used with _VMFontStyle */

/**
 * Font Size Rule
 *
 */
enum _VMFontSizeRule {
	allSize,			/**< Both small and large fonts  */
	smallFonts,			/**< Only Small fonts  */
	largeFonts			/**< Only large fonts  */
};

typedef UInt8 VMFontSizeRule; /**< Used with _VMFontSizeRule */

/**
 * Font Style Rule
 *
 */
enum _VMFontStyleRule {
	allStyle,			/**< All font Styles supported */
	plainOnly			/**< Only Normal Fonts */
};

typedef UInt8 VMFontStyleRule; /**< Used with _VMFontStyleRule */

/**
 * @brief Font UI rules structure used for the font picker setting
 *
 */
typedef struct {
	Boolean fontFaceVisible; 	/**< Shows/Hides Font face  */
	Boolean fontStyleVisible;	/**< Shows/Hides Font Style */
	Boolean fontSizeVisible;	/**< Shows/Hides Font Size */
	VMFontSizeRule sizeRule;	/**< Shows the size selected: allSize, smallFonts, largeFonts */
	VMFontStyleRule styleRule;	/**< Shows the style selected: allStyle, plainOnly */
} VMFontUIRules;
typedef VMFontUIRules *VMFontUIRulesPtr;

/**
 * @brief Font characteristics structure used to describe a font
 *
 */
typedef struct {
	VMFontFace  face; 	/**< Font face. */
	VMFontSize  size; 	/**< Size of Font. */
	VMFontStyle style;	/**< Font Style */
} VMFontType;
typedef VMFontType *VMFontPtr;






#endif
