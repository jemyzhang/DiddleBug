/******************************************************************************
 * Copyright (c) 2004 palmOne, Inc. or its subsidiaries.
 * All rights reserved.
 *****************************************************************************/
/** @ingroup Telephony
 *
 */
 
 
/**
 * @file 	HsPhoneSecurity.h
 *
 * @brief  Header File for Phone Library API ---- SECURITY CATEGORY
 *
 * Notes:
 * 	All implementations of the Handspring Phone Library support a common API. 
 * 	This API is broken up into various categories for easier management.  This file
 * 	defines the public portion of the Security category.  These API calls are used 
 * 	to protect access to device and phone features.
 *			
 */


#ifndef HS_PHONESECURITY_H
#define HS_PHONESECURITY_H
#include <PalmOS.h>
#include <PalmTypes.h>
#ifndef __CORE_COMPATIBILITY_H__
#include <PalmCompatibility.h>
	/** workaround for differing header files in sdk-3.5 and sdk-internal */
	#ifndef __CORE_COMPATIBILITY_H__
	#define __CORE_COMPATIBILITY_H__
	#endif 
#endif 
#include <HsPhoneTraps.h>     /**< trap table definition for phone library calls */
#include <HsPhoneErrors.h>    /**< error codes returned by phone library functions */
#include <HsPhoneTypes.h>

/**
 *  @brief 
 * 
 *  @param refNum: 	IN:
 *  @param lock:	IN:
 *  @param autoLockCheckBoxState:	IN:
 *  @retval Err Error code.
 **/
extern Err PhnLibGetPhoneLock (UInt16 refNum, Boolean * lock, Boolean * autoLockCheckBoxState)
  PHN_LIB_TRAP(PhnLibTrapGetPhoneLock);

extern Err PhnLibSetPhoneLock (UInt16 refNum, Boolean enable, PhnLockMode lockMode, CharPtr password)
      PHN_LIB_TRAP (PhnLibTrapSetPhoneLock);  

extern Err PhnLibSetOperatorLock(UInt16 refNum, UInt16 facilityType, Boolean enable, CharPtr password)
				PHN_LIB_TRAP(PhnLibTrapSetOperatorLock);


extern Err PhnLibGetRemainingPasswordTries(UInt16 refNum, Int16* pinTries, Int16* pin2Tries, Int16* pukTries, Int16* puk2Tries)
				PHN_LIB_TRAP(PhnLibTrapGetRemainingPasswordTries);
                                
extern Err PhnLibCRSM(UInt16 refNum, UInt16 command, UInt32 fileId, 
      UInt16 p1, UInt16 p2, UInt16 p3, UInt8* data, UInt16 dataSize, 
    UInt16* sw1, UInt16* sw2, char ** response, UInt16* responseSize) 
  PHN_LIB_TRAP(PhnLibTrapCRSM);
                                

#endif
