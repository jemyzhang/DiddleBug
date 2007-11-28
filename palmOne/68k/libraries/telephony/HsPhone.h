/******************************************************************************
 * Copyright (c) 2004 palmOne, Inc. or its subsidiaries.
 * All rights reserved.
 *****************************************************************************/
/**
 * @defgroup Telephony Telephony Library
 *
 * @{
 * @}
 */
/**
 * @ingroup Telephony
 */
 
 
/**
 * @file 	HsPhone.h
 *
 * @brief  Header file for the phone library (CDMA or GSM)
 *
 * Notes:
 * 	This is the generic header file that should be used by any code that
 * 	uses the CDMA or GSM phone libraries. If you add a function to this file
 * 	please makes sure you update both the CDMA and GSM libraries with this
 * 	function and update the ARM wrapper library if appropriate.							
 */



#ifndef HS_PHONE_H
#define HS_PHONE_H
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
/**< including the other type files */
#include <HsPhoneSMSTypes.h>
#include <HsPhoneMiscTypes.h>
#include <HsPhoneNetworkTypes.h>
#include <HsPhoneGSMTypes.h>
#include <HsPhoneCDMATypes.h>
#include <HsPhoneIOTATypes.h>
#include <HsPhoneSecurityTypes.h>
#include <HsPhoneCallControlTypes.h>
#include <HsPhoneAudioTypes.h>
#include <HsPhoneLocationTypes.h>

/**
 * Defines
 */

#include <HsPhoneLibrary.h>         /**< CATEGORY:  LIBRARY */
#include <HsPhoneMisc.h>            /**< CATEGORY:  MISC */
#include <HsPhoneSMS.h>             /**< CATEGORY:  SMS */
#include <HsPhoneNetwork.h>         /**< CATEGORY:  NETWORK */
#include <HsPhoneGSM.h>             /**< CATEGORY:  GSM */
#include <HsPhoneSecurity.h>        /**< CATEGORY:  SECURITY */
#include <HsPhoneCDMA.h>            /**< CATEGORY:  CDMA */
#include <HsPhoneIOTA.h>            /**< CATEGORY:  IOTA */
#include <HsPhonePower.h>           /**< CATEGORY:  POWER */
#include <HsPhoneEvent.h>           /**< CATEGORY:  NOTIFICATIONS & EVENTS */


#endif
