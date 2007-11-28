/******************************************************************************
*                                                                             *
*            (C) Copyright 2000-2001, Acer Incorporated                       *
*                                                                             *
*-----------------------------------------------------------------------------*
*                                                                             *
*    <IDENTIFICATION>                                                         *
*       file name    : AcerHiDensityLib.h                                     *
*                                                                             *
*    <PROFILE>                                                                *
*       Include file for High Resolution Display API                          *
*                                                                             *
*    <HISTORY>                                                                *
*       Started on   : Aug 01 2001                                            *
*                                                                             *
******************************************************************************/
// this file is best viewed by setting TAB-stop as 4

#ifndef __ACERHIDENSITYLIB_H__
#define __ACERHIDENSITYLIB_H__

/******************************************************************************
*    Includes                                                                 *
******************************************************************************/
#include <AcerSystemResources.h>
#include <AcerErrorBase.h>

// BUILDING_APPLICATION
#if BUILDING_APP_OR_LIB_HIGH_RES
	// direct link to library code
	#define HD_TRAP(trapNum)
#else
	// else someone else is including this public header file; use traps
	#define HD_TRAP(trapNum)	SYS_TRAP(trapNum)
#endif

/******************************************************************************
*    define SCREEN SIZE                                                       *
******************************************************************************/
#define hdWidth			320
#define hdHeight		320
#define stdWidth		160
#define stdHeight		160

/******************************************************************************
*    define hrErrorClass                                                      *
******************************************************************************/
#define hdErrNone		 0
#define hdErrorClass	(acerHDErrorClass)
#define hdErrParam		(hrErrorClass | 1)
#define hdErrNotOpen	(hrErrorClass | 2)
#define hdErrStillOpen	(hrErrorClass | 3)
#define hdErrNoGlobals	(hrErrorClass | 4)
#define hdErrMemory		(hrErrorClass | 5)
#define hdErrNoFeature	(hrErrorClass | 6)

/******************************************************************************
*    define High Resolution Font                                              *
******************************************************************************/
//enum hdFontID {
//};

//typedef enum hdFontID HDFontID;

/******************************************************************************
*    define HRTrapNumEnum                                                     *
******************************************************************************/
typedef enum tagHDTrapNumEnum
{
	HDTrapGetAPIVersion = sysLibTrapCustom,
	HDTrapSystem

} HDTrapNumEnum;


/******************************************************************************
*    API Prototypes                                                           *
******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
*    Standard library open, close, sleep and wake functions                   *
******************************************************************************/
extern Err	HDOpen(UInt16 refNum)		HD_TRAP(sysLibTrapOpen);

extern Err	HDClose(UInt16 refNum)		HD_TRAP(sysLibTrapClose);

extern Err	HDSleep(UInt16 refNum)		HD_TRAP(sysLibTrapSleep);

extern Err	HDWake(UInt16 refNum)		HD_TRAP(sysLibTrapWake);


/******************************************************************************
*    API Prototypes                                                           *
******************************************************************************/
extern Err	HDGetAPIVersion(UInt16 refNum, UInt16 *versionP)
	HD_TRAP(HDTrapGetAPIVersion);

/* System Use Only */
extern Err HDSystem(UInt16 refNum, UInt16 operation, UInt32 *param1, UInt32 *param2, UInt32 *param3)
	HD_TRAP(HDTrapSystem);
	
/** Number of API : 2 **/

#ifdef __cplusplus
}
#endif

#endif /* __ACERHIDENSITYLIB_H__ */