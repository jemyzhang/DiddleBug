/******************************************************************************
*                                                                             *
*            (C) Copyright 2000-2001, Acer Incorporated                       *
*                                                                             *
*-----------------------------------------------------------------------------*
*                                                                             *
*    <IDENTIFICATION>                                                         *
*       file name    : AcerHwrOEMIDs.h                                        *
*                                                                             *
*    <PROFILE>                                                                *
*       GHwrOEMIDs (CompanyID, HALID, DeviceID) definitions                   *
*                                                                             *
*    <HISTORY>                                                                *
*       Started on   : Aug 01 2001                                            *
*                                                                             *
******************************************************************************/
// this file is best viewed by setting TAB-stop as 4

#ifndef __ACERHWROEMIDS_H__
#define __ACERHWROEMIDS_H__

/******************************************************************************
 *    Include                                                                 *
 ******************************************************************************/
#include <HwrMiscFlags.h>


/******************************************************************************
 *    Definitions                                                             *
 ******************************************************************************/

/*** GHwrOEMCompanyID ***/
// All acer-based model has this ID.
// can be obtained by FtrGet(sysFtrCreator, sysFtrNumOEMCompanyID, &value)
#ifdef hwrOEMCompanyIDAcer
	#define acerHwrOEMCompanyID_Sony		hwrOEMCompanyIDAcer
#else
	#define acerHwrOEMCompanyID_Sony		'acer'
#endif

/*** GHwrOEMHALID ***/
// defined for each HAL source code, same codes have the same ID
// can be obtained by FtrGet(sysFtrCreator, sysFtrNumOEMHALID, &value)
#define acerHwrOEMHALIDMono					'momo'
#define acerHwrOEMHALIDColor				'coco'

/*** GHwrOEMDeviceID ***/
// defined for each model. more than one models have the same HALID,
// but not DeviceID
// can be obtained by FtrGet(sysFtrCreator, sysFtrNumOEMDeviceID, &value)
#define sonyHwrOEMDeviceIDMono				'momo'
#define sonyHwrOEMDeviceIDColor				'coco'

#endif	// __ACERHWROEMIDS_H__

