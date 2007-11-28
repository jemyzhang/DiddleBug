/******************************************************************************
 * Copyright (c) 2004 palmOne, Inc. or its subsidiaries.
 * All rights reserved.
 *****************************************************************************/
/** @ingroup Telephony
 *
 */


/**
 *
 * @file    HsPhoneNetworkTypes.h
 *
 * @brief  Header File for Phone Library API ---- NETWORK CATEGORY
 *
 * NOTES:
 *  All implementations of the Handspring Phone Library support a common API.
 *  This API is broken up into various categories for easier management.  This file
 *  defines the SMS category.  These API calls are used to interact with the wireless network.
 */




#ifndef _HS_PHONE_NETWORK_TYPES_H__
#define _HS_PHONE_NETWORK_TYPES_H__
#include <PalmOS.h>
#include <PalmTypes.h>
#ifndef __CORE_COMPATIBILITY_H__
#include <PalmCompatibility.h>
    /** workaround for differing header files in sdk-3.5 and sdk-internal */
    #ifndef __CORE_COMPATIBILITY_H__
    #define __CORE_COMPATIBILITY_H__
    #endif
#endif


#define kMaxPhoneNumberLen  16                  /**< Max length of phone number  */

#define minPasswordLen                  4       /**< Phone password/PIN minimum length  */
#define maxPasswordLen                  8   /**< Phone password/PIN maximum length  */
#define maxPasswordLenCDMA              4   /**<        */

#define phnEnhancedRoamIndMaxLength  40             /**<        */

/**
 *  GSM CLIP Subscription Status
 **/
typedef enum {
    gsmCLIPNotProvisioned,      /**<        */
    gsmCLIPProvisioned,     /**<        */
    gsmCLIPUnknown          /**<        */
} GSMCLIPStatus;


/**
 *
 **/
typedef enum {
    clirNotProvisioned,         /**< sent: restricted presentation of the calling line */
    clirProvisioned,            /**< not sent: don't restrict presentation of the calling line */
    clirUnknown,                /**< status not available */
    clirTemporaryRestricted,        /**< not sent, override allowed */
    clirTemporaryAllowed            /**< sent: override allowed */
} PhnCLIRStatus;

#define isValidPhnCLIRStatus(s) ((s >= clirNotProvisioned) && (s <= clirTemporaryAllowed))  /**<        */


/**
 *  GSM CLIR Display Status
 **/
typedef enum {
  gsmCLIRSubscription,      /**< presentation indicator is used according to the subscription of the CLIR service */
  gsmCLIRInvocation,        /**<        */
  gsmCLIRSuppression        /**<        */
} GSMCLIRDisplayStatus;

#define isValidGSMDialClirStatus(d) ((d >= gsmDialCLIRNotProvisioned) && (d <= gsmDialCLIRTemporaryAllowed))    /**<        */

/**
 * Phone operator status
 **/
typedef enum {
    phnOpUnknown,       /**<        */
    phnOpAvailable,     /**<        */
    phnOpCurrent,       /**<        */
    phnOpForbidden      /**<        */
} PhnOperatorStatus;

#define isValidPhnOperatorStatus(s) ((s >= phnOpUnknown) && (s <= phnOpForbidden))  /**<        */



/**
 *
 **/

typedef UInt8 PhnMsgBoxType;        /**<        */
typedef enum
  {
    kBoxVoice,          /**<        */
    kBoxTelefax,        /**<        */
    kBoxEMail,          /**<        */
    kBoxOther,          /**<        */
    kBoxData,           /**<        */
    PhnMsgBoxTypeMAX        /**< THIS MUST BE LAST ENTRY.... NOT A VALID VALUE */
    // other values reserved for future expansion
  }
PhnMsgBoxEnum;

#define isValidPhnMsgBoxType(n) (n < PhnMsgBoxTypeMAX)      /**<        */

typedef UInt8 PhnMsgBoxPriorityType;
typedef enum
{
  kSmsPriorityNormal,
  kSmsPriorityInteractive,
  kSmsPriorityUrgent,
  kSmsPriorityEmergency,
  kSmsPriorityLast
}
PhnMsgBoxPriorityEnum;

/**
 *
 **/
typedef struct
  {
    Boolean         indicatorOn;    /**<        */
    PhnMsgBoxType   type;       /**<        */
    Int16           messageCount;   /**<        */
    Int16           lineNumber;     /**<        */
    PhnMsgBoxPriorityType           priority; /**< voicemail priority - example: normal, urgent etc. */
  }
PhnMsgBoxDataType;




/**
 * Operator info
 **/
#define MAX_LONGOPERATORNAME_LENGTH 24      /**<        */
#define MAX_SHORTOPERATORNAME_LENGTH 16     /**<        */
typedef struct
  {
    PhnOperatorStatus   status;         /**<        */
  PhnOperatorID   id;                       /**< Not really used since CDMA just allows one service provider */
  char longname[MAX_LONGOPERATORNAME_LENGTH+1];         /**< name + terminating NULL char */
  char shortname[MAX_SHORTOPERATORNAME_LENGTH+1];       /**< name + terminating NULL char */
  Boolean         isRoaming;                /**< in sync with the current roaming status */
  }
PhnOperatorType;

/**
 * Operator list
 **/
typedef struct
  {
    short           count;      /**<        */
    PhnOperatorType opData[1];      /**<        */
  }
PhnOperatorListType,* PhnOperatorListPtr;

/**
 *
 **/
typedef enum {
    gsmRegModeAutomatic,        /**<        */
    gsmRegModeManual,       /**<        */
    gsmRegModeDeregister,       /**<        */
    gsmRegModeFormat,       /**<        */
    gsmRegModeManualAutomatic   /**<        */
} GSMRegistrationMode;

#define isValidGSMRegistrationMode(r) (r <= gsmRegModeManualAutomatic)  /**<        */

/**
 *  Possible roaming status'
 *  GSM devices only use first two
 **/
typedef UInt8 PhnRoamStatus;        /**< set a type for this enumeration */
typedef enum
  {
    PhnRoamStatusOff,           /**<        */
    PhnRoamStatusOn,            /**<        */
    PhnRoamStatusBlink,         /**<        */
    PhnRoamStatusEnhancedIndicator      /**< If this is the roaming status, call PhnLibGetEnhancedRoamIndicator  */
  }
PhnRoamStatusEnum;

/**
 *  Phone call forwarding reason
 **/
typedef UInt8 PhnForwardType;       /**< set a type for this enumeration */
typedef enum {
    phnForwardUnconditional,    /**<        */
    phnForwardOnBusy,       /**<        */
    phnForwardOnNoReplay,       /**<        */
    phnForwardOnNotReachable,   /**<        */
    phnForwardTelefax,      /**<        */
    phnForwardData,         /**<        */
    phnForwardCancelAll,        /**<        */
    phnForwardConditionalAll,   /**<        */
    phnForwardLast          /**<        */
} PhnForwardEnum;

#define isValidPhnForwardType(f) (f < phnForwardLast)       /**<        */

/**
 * Phone call barring options
 **/
typedef enum {
    phnBarOutNone,              /**<        */
    phnBarOutAll,               /**<        */
    phnBarOutInternational,         /**<        */
    phnBarOutInternationalExceptHome,   /**<        */

    phnBarInNone,               /**<        */
    phnBarInAll,                /**<        */
    phnBarInWhenRoaming,            /**<        */
    phnBarAll               /**<        */

} PhnBarFacilityTypeEnum;
typedef UInt8   PhnBarFacilityType;
typedef UInt8*  PhnBarFacilityPtr;
#define isValidPhnBarFacilityType(b) (b <= phnBarAll)   /**<        */

/**
 * CDMA Roam Preferences
 **/
typedef struct
  {
    Boolean         roamCallGuardEnable;    /**<        */
    Boolean         roamRingerEnable;       /**<        */
    UInt8           roamRingerID;       /**<        */
  }
PhnRoamPrefInfoType,* PhnRoamPrefInfoPtr;

/**
 *
 **/
typedef enum  {
    registrationNone,       /**<        */
    registrationHome,       /**<        */
    registrationSearch,         /**<        */
    registrationDenied,     /**<        */
    registrationUnknown,        /**<        */
    registrationRoaming     /**<        */
} PhnRegistrationStatus;

#define isValidPhnRegistrationStatus(r) (r <= registrationRoaming)      /**<        */
//#define isValidPhnRegistrationStatus(r) ((r >= registrationNone) && (r <= registrationRoaming))

/**
 *  Phone call forwarding mode
 **/
typedef UInt8 PhnForwardModeType;   /**<        */
typedef enum {
    phnForwardModeDisable,      /**<        */
    phnForwardModeEnable,       /**<        */
    phnForwardModeInterrogate,  /**<        */
    phnForwardModeRegistration, /**<        */
    phnForwardModeErasure       /**<        */
} PhnForwardModeEnum;

#define isValidPhnForwardModeType(f) (f <= phnForwardModeErasure)   /**<        */
/**
 * MMI Action Code
 **/
typedef enum
{
  mmiActionNoAction     = 0,        /**<        */
  mmiActionActivate     = 0x01,     /**<        */
  mmiActionDeactivate   = 0x02,     /**<        */
  mmiActionInterrogate  = 0x04,     /**<        */
  mmiActionRegister     = 0x08,     /**<        */
  mmiActionErasure      = 0x10,     /**<        */
  mmiActionUnlock       = 0x20,     /**<        */
  mmiActionHandspring   = 0x40      /**<        */
}
PhnMMIActionCodeEnum;
typedef UInt8 PhnMMIActionCode;


/**
 * MMI Service Code
 **/
typedef enum
{
    mmiServiceInvalid = 0,      /**<        */
    mmiServiceChangePIN,        /**<        */
    mmiServiceChangePIN2,       /**<        */
    mmiServiceUnlockPIN,        /**<        */
    mmiServiceUnlockPIN2,       /**<        */
    mmiServiceIMEI,         /**<        */
    mmiServiceCallerId,     /**<        */
    mmiServiceNumberSent,       /**<        */
    mmiServiceCOLP,         /**<        */
    mmiServiceCOLR,         /**<        */
    mmiServiceCallWaiting,      /**<        */
    mmiServiceForwardUnconditional, /**<        */
    mmiServiceForwardBusy,      /**<        */
    mmiServiceForwardNoReply,   /**<        */
    mmiServiceForwardNotReachable,  /**<        */
    mmiServiceForwardAll,       /**<        */
    mmiServiceForwardConditionalAll,/**<        */
    mmiServiceBAOC,         /**< bar all outging calls */
    mmiServiceBAOIC,        /**< bar all outging international calls */
    mmiServiceBAOICX,       /**< bar all outging international calls expect home */
    mmiServiceBAIC,         /**< bar all incoming calls */
    mmiServiceBAICR,        /**< bar all incoming calls while roaming */
    mmiServiceAllBarringServ,   /**< bar all services */
    mmiServiceAllOutBarringServ,    /**< bar all outgoing services */
    mmiServiceAllInBarringServ,     /**< bar all incoming services */
    mmiServiceChangeBarPwd,     /**< Change barring password */
    mmiServiceUSSD,         /**<        */
    mmiServicePhoneNumber,      /**<        */
    mmiServiceCallControl,      /**<        */
    mmiServiceSelectorOperatorLock, /**<        */
    mmiServiceSelectorProviderLock, /**<        */
    mmiServiceNNN,          /**<        */
    mmiServiceUnimplemented     /**< defined by GSM standard but not implemented in our phone */
}
PhnMMIServiceCodeEnum;
typedef UInt8 PhnMMIServiceCode;

/**
 * Call Control Codes
 **/
typedef enum {
  mmiControlReleaseAllHeld = 0,     /**<        */
  mmiControlReleaseAllActive,       /**<        */
  mmiControlReleaseCallx,       /**<        */
  mmiControlSwap,           /**<        */
  mmiControlActivateCallx,      /**<        */
  mmiControlConference,         /**<        */
  mmiControlTransfer            /**<        */
}PhnCallControlTypeEnum;

typedef UInt8 PhnCallControlType;


typedef enum
{
  PhnRoamIconOn,            /**<        */
  PhnRoamIconOff,           /**<        */
  PhnRoamIconFlash          /**<        */
}
PhnRoamIconState;
/************************************************************
 *  Structs
 *************************************************************/

#define kMaxMMISequenceLen 64       /**<        */

/**
 * Telephone Service Types
 **/
typedef enum {
  phnTeleserviceUnknown = 0,        /**<        */
  phnTeleserviceVoice = 1,      /**<        */
  phnTeleserviceData = 2,       /**<        */
  phnTeleserviceFax = 4,        /**<        */
  phnTeleserviceDefault = 7,        /**<  Voice + Data + Fax */
  phnTeleserviceSMS = 8,        /**<        */
  phnTeleserviceDataCircSync = 16,  /**<        */
  phnTeleserviceDataCircAsync = 32, /**<        */
  phnTeleserviceDedicatedPack = 64, /**<        */
  phnTeleserviceDedicatedPAD = 128  /**<        */
} PhnTeleserviceType;


/**
 * Telephone Service MMI code Types
 **/
typedef enum {
  phnTeleserviceUnknownMmiCode        = 0,  /**<        */
  phnTeleserviceVoiceMmiCode          = 11, /**<        */
  phnTeleserviceAllMmiCode            = 12, /**<        */
  phnTeleserviceFaxMmiCode            = 13, /**<        */
  phnTeleserviceSMSMmiCode            = 16, /**<        */
  phnTeleserviceAllExceptSMSCode      = 19, /**<        */
  phnTeleserviceDataCircSyncMmiCode   = 24, /**<        */
  phnTeleserviceDataCircAsyncMmiCode  = 25, /**<        */
  phnTeleserviceDedicatedPackMmiCode  = 26, /**<        */
  phnTeleserviceDedicatedPADMmiCode   = 27  /**<        */
} PhnTeleserviceMmiCodeType;

/**
 * @brief MMI Decode struct
 **/
typedef struct
{
  UInt16  size;             /**<        */
  PhnMMIActionCode  actionCode;     /**<        */
  PhnMMIServiceCode  serviceCode;   /**<        */
  Boolean sendHit;          /**<        */
  char text[kMaxMMISequenceLen+1];  /**<        */
  union
  {
    struct
    {
      char  number[kMaxPhoneNumberLen+1];   /**<        */
      PhnTeleserviceType type;          /**<        */
      UInt16  time;             /**<        */
    } mmiForwarding;
    struct
    {
      PhnTeleserviceType type;          /**<        */
      char pwd[maxPasswordLen+1];       /**<        */
    } mmiBarring;
    struct
    {
      PhnTeleserviceType type;          /**<        */
    } mmiCallWaiting;               /**<        */
    struct
    {
      char oldPwd[maxPasswordLen+1];        /**<        */
      char newPwd[maxPasswordLen+1];        /**<        */
      char repeatNewPwd[maxPasswordLen+1];  /**<        */
    } mmiPassword;
    struct
    {
      PhnCallControlType action;        /**<        */
      PhnConnectionID id;           /**<        */
    } mmiCallControl;
    struct
    {
      char  name[kMaxPhoneNumberLen+1]; /**<        */
      char  number[kMaxPhoneNumberLen+1];   /**<        */
      GSMDialCLIRMode mode;         /**<        */
    } mmiPhoneNumber;

  } data;

}
PhnMMIEntryType, *PhnMMIEntryPtr;

#define kMaxImeiLen 24          /**<        */

/**
 * @brief MMI result struct
 **/
typedef struct
{
  UInt16 size;              /**<        */
  PhnMMIServiceCode  serviceCode;   /**<        */
  union
  {
    struct
    {
      Boolean enabled;              /**<        */
      char  number[kMaxPhoneNumberLen+1];   /**<        */
      PhnTeleserviceType type;          /**<        */
      UInt16  time;             /**<        */
    } mmiForwarding;
    struct
    {
      Boolean enabled;              /**<        */
      PhnTeleserviceType type;          /**<        */
    } mmiBarring;
    struct
    {
      Boolean enabled;              /**<        */
      PhnTeleserviceType type;          /**<        */
    } mmiCallWaiting;
    struct
    {
      Boolean enabled;              /**<        */
    } mmiCallerID;
    struct
    {
      GSMCLIRDisplayStatus displayStatus;   /**<        */
      PhnCLIRStatus subscriptionStatus;     /**<        */
    } mmiNumberSent;
    struct
    {
      Boolean enabled;              /**<        */
    } mmiColp;
        struct
    {
      Boolean enabled;              /**<        */
    } mmiColr;
    struct
    {
      char imei[kMaxImeiLen+1];         /**<        */
    } mmiIMEI;

  } data;
}
PhnMMIResultType, *PhnMMIResultPtr;

typedef enum
{
  bandGSM_EGSM = 0,
  band1800,
  band900_1800,
  band1900,
  bandAuto,
  band850,
  band1900_850
} _PhnBandSelection;

typedef UInt8 PhnBandSelection;
#define isValidPhnBandSelection(f) (f <= band1900_850)


/**
 * This defines the roaming mode of the radio.
 */
typedef enum
{
  phnRoamPrefHomeOnly,   /**< Home network only (no roaming)      */
  phnRoamPrefAny,        /**< Any network       (roaming allowed) */
  phnRoamPrefRoamOnly,   /**< Roam network only (roaming only)    */
  phnRoamPrefLast        /**< end of list enum                    */
}
_PhnRoamPrefSelectionTypeEnum;
typedef UInt8 PhnRoamPrefSelectionType;
#define isValidRoamMode(x)  (x < phnRoamPrefLast)


/**
 * This is now deprecated.
 * It is only implemented to maintain backwards compatibility
 * Currently, the only user is NetMaster
 **/
typedef enum
{
    phnOneXStatusPRevNoSvc,     /**< Service is not available. */
    phnOneXStatusPRev1,         /**< J-STD-008 service is in use. */
    phnOneXStatusPRev3,         /**< IS-95A service is in use. */
    phnOneXStatusPRev4,         /**< IS-95B service is in use. */
    phnOneXStatusPRev6,         /**< IS-2000 release 0 service is in use. */
    phnOneXStatusPRev7      /**< IS-2000 release A service is in use. */
}
_PhnOneXProtRevType;
typedef UInt8 PhnOneXProtRevType;   /**<        */
#define isValidPhnOneXProtRevType(p) (p <= phnOneXStatusPRev7)      /**<        */

/**
 * Describes the current packet data session state.
 * Currently there are 3 states. Active, dormant, or none.
 **/
typedef enum
{
  phnPacketDataSessionNone,         /**< No packet data session exists. */
  phnPacketDataSessionDormant,      /**< Packet data session dormant    */
  phnPacketDataSessionActive        /**< Packet data session active     */
}
_PhnPacketDataSessionType;
typedef UInt8 PhnPacketDataSessionType;     /**<        */
#define isValidPhnPacketDataSessionType(p) (p <= phnPacketDataSessionActive)        /**<        */

/**
 * This is now deprecated.
 * It is only implemented to maintain backwards compatibility
 * Currently, the only user is NetMaster
 **/
typedef enum 
{
    phnMIPSessionNone,      /**<        */
    phnMIPSession           /**< A Mobile IP session exists. */
}
_PhnMIPSessionType;
typedef UInt8 PhnMIPSessionType;        /**<        */
#define isValidPhnMIPSessionType(m) (m <= phnMIPSession)        /**<        */


/**
 * This is now deprecated.
 * It is only implemented to maintain backwards compatibility
 * Currently, the only user is NetMaster and the Phone App
 **/
typedef struct
{
  PhnOneXProtRevType        oneXStatusPRev;     /**<        */
  PhnPacketDataSessionType  pdSessionStatus;    /**<        */
  PhnMIPSessionType         mipSessionType; /**<        */
}
PhnOneXStatus;


/**
 * This defines the different data service types.
 */
typedef enum
{
  phnDataServiceNone,
  phnDataServiceCSD,
  phnDataService1xRTT,
  phnDataService1xEVDO,
  phnDataServiceGPRS,
  phnDataServiceEdge,
  phnDataServiceLast
}
_PhnDataServiceType;
typedef UInt16 PhnDataServiceType;
#define isValidDataService(x) (x < phnDataServiceLast)


/**
 * List of errors on why a data call was disconnected.
 */
typedef enum
{
  phnPacketDataEndStatusUnknown,       /**< Unknown end status      */
  phnPacketDataEndStatusNormal,        /**< Normal                  */
  phnPacketDataEndStatusReorder,       /**< reoder                  */
  phnPacketDataEndStatusRetry,         /**< retry                   */
  phnPacketDataEndStatusTimeout,       /**< call                    */
  phnPacketDataEndStatusIntercept,     /**< Intercept               */
  phnPacketDataEndStatusReject,        /**< Service Option rejected */ 
  phnPacketDataEndStatusNoReason,      /**< No reason given         */
  phnPacketDataEndStatusAuthFailed,    /**< authentication failed   */
  phnPacketDataEndStatusDropped,       /**< dropped call            */
  phnPacketDataEndStatusLast           /**< place holder for last   */
}
_PhnPacketDataEndStatusEnum;
typedef UInt16 PhnPacketDataEndStatusType;
#define isValidPacketDataEndStatus(x) (x < phnPacketDataEndStatusLast)

#define PhnDataSessionStateTypeMajorVersion 0x0001L  // high word
#define PhnDataSessionStateTypeMinorVersion 0x0000L  // low  word
#define PhnDataSessionStateTypeVersion      (((UInt32)PhnDataSessionStateTypeMajorVersion << 16) | PhnDataSessionStateTypeMinorVersion)
typedef struct
{
  UInt32                      version;          /**< (1.0) should be set to the structure version that you are requesting                   */
  PhnDataServiceType          availableService; /**< (1.0) data connection type available                                                   */
  PhnDataServiceType          sessionType;      /**< (1.0) data connection type (valid only if status is Active or Dormant)                 */
  PhnPacketDataSessionType    sessionStatus;    /**< (1.0) data connection status                                                           */
  PhnPacketDataEndStatusType  disconnectStatus; /**< (1.0) data connection end status (only useful after a data call has been terminated)   */
}
PhnDataSessionStateType;


/**
 *
 **/
typedef struct
{
  UInt32 timeInSecs;        /**< Current time in minutes */
  Boolean daylightSavings;  /**< Is this daylight savings  */
  Int16 timeZone;          /**< Number of minutes east of GMT  */
  UInt8  daylightSavingsHour; /**< DST adjusted hours : GSM Only  */

} PhnNetworkTime;


#endif // _HS_PHONE_NETWORK_TYPES_H__
