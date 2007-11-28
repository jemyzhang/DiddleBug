/******************************************************************************
 * Copyright (c) 2004 PalmOne, Inc. or its subsidiaries.
 * All rights reserved.
 *****************************************************************************/

/**
 * @file 	HsCommon.h
 * @version 1.0
 * @date 	
 *
 * Main Handspring Palm OS API Header file
 * 
 *	Most functions or API's are specific to either the 68K environment or the
 *	ARM environment but both of them sometimes share the same structures, types,
 *	constants etc.. The common header files are for those shared components that
 *	are used by both the 68K and ARM parts of a library, extention etc. In
 *	general, a developer would never have to include this file seperataely. It
 *	is already included in both the 68K/Hs.h and the ARM/Hs.h. 
 *
 * @author Kiran Prasad
 * <hr>
 */
 

#ifndef __HS_COMMON_H__

#define __HS_COMMON_H__

#include <PalmTypes.h>
#include <PalmCompatibility.h>
#include <PalmOS.h>

#include <Common/System/HsAppLaunchCmd.h>
#include <Common/System/HsCreators.h>
#include <Common/System/HsChars.h>
#include <Common/System/HsErrorClasses.h>
#include <Common/System/HsExgMgrCommon.h>
#include <Common/System/HsErrors.h>


#include <Common/Libraries/FavoritesDBLibrary/FavoritesDBLibTypes.h>

#include <Common/Libraries/Imaging/ImageLibCommon.h>
#include <Common/Libraries/Imaging/ImageLibTraps.h>

#include <Common/Libraries/Camera/CameraLibCommon.h>

#include <Common/Libraries/HTTP/HS_HTTPLibApp.h>
#include <Common/Libraries/HTTP/HS_HTTPLibConst.h>
#include <Common/Libraries/MMS/MmsHelperCommon.h>

#endif /* __HS_COMMON_H__ */
