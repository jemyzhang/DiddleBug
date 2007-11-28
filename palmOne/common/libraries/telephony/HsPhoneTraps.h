/******************************************************************************
 * Copyright (c) 2004 palmOne, Inc. or its subsidiaries.
 * All rights reserved.
 *****************************************************************************/
/** @ingroup Telephony
 *
 */


/**
 *
 * @file    HsPhoneTraps.h
 *
 * @brief  Traps of all functions exported by the Phone Library
 *
 * NOTES:
 */



#ifndef HS_PHONE_TRAPS_H
#define HS_PHONE_TRAPS_H

#if (CPU_TYPE == CPU_68K)
#define PHN_LIB_TRAP(trapNum) SYS_TRAP(trapNum)
#endif


#define     PhnLibTrapGetLibAPIVersion                  (0+sysLibTrapCustom)       /**< 0xA805           */
#define     PhnLibTrapUninstall                         (1+sysLibTrapCustom)       /**< 0xA806           */
#define     PhnLibTrapRegister                          (2+sysLibTrapCustom)       /**< 0xA807           */
#define     PhnLibTrapFirstAppForService                (3+sysLibTrapCustom)       /**< 0xA808  GSM Only */
#define     PhnLibTrapNewAddress                        (4+sysLibTrapCustom)       /**< 0xA809           */
#define     PhnLibTrapGetField                          (5+sysLibTrapCustom)       /**< 0xA80A           */
#define     PhnLibTrapGetID                             (6+sysLibTrapCustom)       /**< 0xA80B           */
#define     PhnLibTrapSetField                          (7+sysLibTrapCustom)       /**< 0xA80C           */
#define     PhnLibTrapSetNumber                         (8+sysLibTrapCustom)       /**< 0xA80D           */
#define     PhnLibTrapSetID                             (9+sysLibTrapCustom)       /**< 0xA80E           */
#define     PhnLibTrapAddressToText                     (10+sysLibTrapCustom)      /**< 0xA80F           */
#define     PhnLibTrapHasName                           (11+sysLibTrapCustom)      /**< 0xA810           */
#define     PhnLibTrapEqual                             (12+sysLibTrapCustom)      /**< 0xA811           */
#define     PhnLibTrapDial                              (13+sysLibTrapCustom)      /**< 0xA812           */
#define     PhnLibTrapSendDTMF                          (14+sysLibTrapCustom)      /**< 0xA813           */
#define     PhnLibTrapDisconnect                        (15+sysLibTrapCustom)      /**< 0xA814           */
#define     PhnLibTrapActivate                          (16+sysLibTrapCustom)      /**< 0xA815           */
#define     PhnLibTrapConference                        (17+sysLibTrapCustom)      /**< 0xA816           */
#define     PhnLibTrapHold                              (18+sysLibTrapCustom)      /**< 0xA817           */
#define     PhnLibTrapGetConnectionInfo                 (19+sysLibTrapCustom)      /**< 0xA818           */
#define     PhnLibTrapSetBarring                        (20+sysLibTrapCustom)      /**< 0xA819  GSM Only */
#define     PhnLibTrapGetBarring                        (21+sysLibTrapCustom)      /**< 0xA81A  GSM Only */
#define     PhnLibTrapSetForwarding                     (22+sysLibTrapCustom)      /**< 0xA81B           */
#define     PhnLibTrapGetForwarding                     (23+sysLibTrapCustom)      /**< 0xA81C           */
#define     PhnLibTrapSetCallWaiting                    (24+sysLibTrapCustom)      /**< 0xA81D           */
#define     PhnLibTrapGetCallWaiting                    (25+sysLibTrapCustom)      /**< 0xA81E           */
#define     PhnLibTrapSetPhoneLock                      (26+sysLibTrapCustom)      /**< 0xA81F           */
#define     PhnLibTrapGetPhoneLock                      (27+sysLibTrapCustom)      /**< 0xA820           */
#define     PhnLibTrapSetCLIP                           (28+sysLibTrapCustom)      /**< 0xA821  GSM Only */
#define     PhnLibTrapGetCLIP                           (29+sysLibTrapCustom)      /**< 0xA822  GSM Only */
#define     PhnLibTrapSetOperatorLock                   (30+sysLibTrapCustom)      /**< 0xA823           */
#define     PhnLibTrapGetOperatorLock                   (31+sysLibTrapCustom)      /**< 0xA824  GSM Only */
#define     PhnLibTrapChangePassword                    (32+sysLibTrapCustom)      /**< 0xA825           */
#define     PhnLibTrapPasswordDialog                    (33+sysLibTrapCustom)      /**< 0xA826  GSM Only */
#define     PhnLibTrapCurrentOperator                   (34+sysLibTrapCustom)      /**< 0xA827           */
#define     PhnLibTrapCurrentProvider                   (35+sysLibTrapCustom)      /**< 0xA828  GSM Only */
#define     PhnLibTrapGetOperatorList                   (36+sysLibTrapCustom)      /**< 0xA829           */
#define     PhnLibTrapSetOperator                       (37+sysLibTrapCustom)      /**< 0xA82A           */
#define     PhnLibTrapGetOwnNumbers                     (38+sysLibTrapCustom)      /**< 0xA82B           */
#define     PhnLibTrapSetOwnNumbers                     (39+sysLibTrapCustom)      /**< 0xA82C  GSM Only */
#define     PhnLibTrapGetRingingInfo                    (40+sysLibTrapCustom)      /**< 0xA82D           */
#define     PhnLibTrapSetRingingInfo                    (41+sysLibTrapCustom)      /**< 0xA82E           */
#define     PhnLibTrapGetToneIDs                        (42+sysLibTrapCustom)      /**< 0xA82F           */
#define     PhnLibTrapGetToneName                       (43+sysLibTrapCustom)      /**< 0xA830           */
#define     PhnLibTrapPlayTone                          (44+sysLibTrapCustom)      /**< 0xA831           */
#define     PhnLibTrapStopTone                          (45+sysLibTrapCustom)      /**< 0xA832           */
#define     PhnLibTrapFindEntry                         (46+sysLibTrapCustom)      /**< 0xA833  GSM Only */
#define     PhnLibTrapSelectAddress                     (47+sysLibTrapCustom)      /**< 0xA834  GSM Only */
#define     PhnLibTrapGetMicrophone                     (48+sysLibTrapCustom)      /**< 0xA835  GSM Only */
#define     PhnLibTrapSetMicrophone                     (49+sysLibTrapCustom)      /**< 0xA836  GSM Only */
#define     PhnLibTrapGetVolume                         (50+sysLibTrapCustom)      /**< 0xA837           */
#define     PhnLibTrapSetVolume                         (51+sysLibTrapCustom)      /**< 0xA838           */
#define     PhnLibTrapRegistered                        (52+sysLibTrapCustom)      /**< 0xA839           */
#define     PhnLibTrapRoaming                           (53+sysLibTrapCustom)      /**< 0xA83A           */
#define     PhnLibTrapSignalQuality                     (54+sysLibTrapCustom)      /**< 0xA83B           */
#define     PhnLibTrapErrorRate                         (55+sysLibTrapCustom)      /**< 0xA83C  GSM Only */
#define     PhnLibTrapBattery                           (56+sysLibTrapCustom)      /**< 0xA83D           */
#define     PhnLibTrapCardInfo                          (57+sysLibTrapCustom)      /**< 0xA83E           */
#define     PhnLibTrapSIMInfo                           (58+sysLibTrapCustom)      /**< 0xA83F           */
#define     PhnLibTrapGetSIMStatus                      (59+sysLibTrapCustom)      /**< 0xA840  GSM Only */
#define     PhnLibTrapParamText                         (60+sysLibTrapCustom)      /**< 0xA841  GSM Only */
#define     PhnLibTrapGetErrorText                      (61+sysLibTrapCustom)      /**< 0xA842           */
#define     PhnLibTrapGetDBRef                          (62+sysLibTrapCustom)      /**< 0xA843           */
#define     PhnLibTrapReleaseDBRef                      (63+sysLibTrapCustom)      /**< 0xA844           */
#define     PhnLibTrapNewMessage                        (64+sysLibTrapCustom)      /**< 0xA845           */
#define     PhnLibTrapDeleteMessage                     (65+sysLibTrapCustom)      /**< 0xA846           */
#define     PhnLibTrapSendMessage                       (66+sysLibTrapCustom)      /**< 0xA847           */
#define     PhnLibTrapSetText                           (67+sysLibTrapCustom)      /**< 0xA848           */
#define     PhnLibTrapSetDate                           (68+sysLibTrapCustom)      /**< 0xA849           */
#define     PhnLibTrapSetOptions                        (69+sysLibTrapCustom)      /**< 0xA84A           */
#define     PhnLibTrapSetAddresses                      (70+sysLibTrapCustom)      /**< 0xA84B           */
#define     PhnLibTrapSetStatus                         (71+sysLibTrapCustom)      /**< 0xA84C           */
#define     PhnLibTrapSetFlags                          (72+sysLibTrapCustom)      /**< 0xA84D           */
#define     PhnLibTrapSetOwner                          (73+sysLibTrapCustom)      /**< 0xA84E           */
#define     PhnLibTrapGetText                           (74+sysLibTrapCustom)      /**< 0xA84F           */
#define     PhnLibTrapGetDate                           (75+sysLibTrapCustom)      /**< 0xA850           */
#define     PhnLibTrapGetOptions                        (76+sysLibTrapCustom)      /**< 0xA851           */
#define     PhnLibTrapGetAddresses                      (77+sysLibTrapCustom)      /**< 0xA852           */
#define     PhnLibTrapGetStatus                         (78+sysLibTrapCustom)      /**< 0xA853           */
#define     PhnLibTrapGetFlags                          (79+sysLibTrapCustom)      /**< 0xA854           */
#define     PhnLibTrapGetOwner                          (80+sysLibTrapCustom)      /**< 0xA855           */
#define     PhnLibTrapGetType                           (81+sysLibTrapCustom)      /**< 0xA856           */
#define     PhnLibTrapIsLegalCharacter                  (82+sysLibTrapCustom)      /**< 0xA857           */
#define     PhnLibTrapMapCharacter                      (83+sysLibTrapCustom)      /**< 0xA858           */
#define     PhnLibTrapSetServiceCentreAddress           (84+sysLibTrapCustom)      /**< 0xA859           */
#define     PhnLibTrapGetServiceCentreAddress           (85+sysLibTrapCustom)      /**< 0xA85A           */
#define     PhnLibTrapLength                            (86+sysLibTrapCustom)      /**< 0xA85B           */
#define     PhnLibTrapGetSubstitution                   (87+sysLibTrapCustom)      /**< 0xA85C           */
#define     PhnLibTrapNewAddressList                    (88+sysLibTrapCustom)      /**< 0xA85D           */
#define     PhnLibTrapDisposeAddressList                (89+sysLibTrapCustom)      /**< 0xA85E           */
#define     PhnLibTrapAddAddress                        (90+sysLibTrapCustom)      /**< 0xA85F           */
#define     PhnLibTrapGetNth                            (91+sysLibTrapCustom)      /**< 0xA860           */
#define     PhnLibTrapSetNth                            (92+sysLibTrapCustom)      /**< 0xA861           */
#define     PhnLibTrapCount                             (93+sysLibTrapCustom)      /**< 0xA862           */
#define     PhnLibTrapSetModulePower                    (94+sysLibTrapCustom)      /**< 0xA863           */
#define     PhnLibTrapModulePowered                     (95+sysLibTrapCustom)      /**< 0xA864           */
#define     PhnLibTrapModuleButtonDown                  (96+sysLibTrapCustom)      /**< 0xA865           */
#define     PhnLibTrapBoxInformation                    (97+sysLibTrapCustom)      /**< 0xA866           */
#define     PhnLibTrapGetBoxNumber                      (98+sysLibTrapCustom)      /**< 0xA867           */
#define     PhnLibTrapGetDataApplication                (99+sysLibTrapCustom)      /**< 0xA868    GSM Only */
#define     PhnLibTrapSetDataApplication                (100+sysLibTrapCustom)     /**< 0xA869    GSM Only */
#define     PhnLibTrapDebug                             (101+sysLibTrapCustom)     /**< 0xA86A    GSM Only */
#define     PhnLibTrapGetPhoneBook                      (102+sysLibTrapCustom)     /**< 0xA86B    GSM Only */
#define     PhnLibTrapSetPhoneBook                      (103+sysLibTrapCustom)     /**< 0xA86C    GSM Only */
#define     PhnLibTrapSetPhoneBookEntry                 (104+sysLibTrapCustom)     /**< 0xA86D    GSM Only */
#define     PhnLibTrapGetPhoneBookIndex                 (105+sysLibTrapCustom)     /**< 0xA86E    GSM Only */
#define     PhnLibTrapBatteryCharge                     (106+sysLibTrapCustom)     /**< 0xA86F    GSM Only */
#define     PhnLibTrapGetEchoCancellation               (107+sysLibTrapCustom)     /**< 0xA870    GSM Only */
#define     PhnLibTrapSetEchoCancellation               (108+sysLibTrapCustom)     /**< 0xA871    GSM Only */
#define     PhnLibTrapGetEquipmentMode                  (109+sysLibTrapCustom)     /**< 0xA872           */
#define     PhnLibTrapSetEquipmentMode                  (110+sysLibTrapCustom)     /**< 0xA873           */
#define     PhnLibTrapGetSMSRingInfo                    (111+sysLibTrapCustom)     /**< 0xA874    GSM Only */
#define     PhnLibTrapSetSMSRingInfo                    (112+sysLibTrapCustom)     /**< 0xA875    GSM Only */
#define     PhnLibTrapPlayDTMF                          (113+sysLibTrapCustom)     /**< 0xA876           */
#define     PhnLibTrapStartVibrate                      (114+sysLibTrapCustom)     /**< 0xA877           */
#define     PhnLibTrapStopVibrate                       (115+sysLibTrapCustom)     /**< 0xA878           */
#define     PhnLibTrapSendUSSD                          (116+sysLibTrapCustom)     /**< 0xA879    GSM Only */
#define     PhnLibTrapSATGetMainMenu                    (117+sysLibTrapCustom)     /**< 0xA87A    GSM Only */
#define     PhnLibTrapSATSendRequest                    (118+sysLibTrapCustom)     /**< 0xA87B    GSM Only */
#define     PhnLibTrapSATEndSession                     (119+sysLibTrapCustom)     /**< 0xA87C    GSM Only */
#define     PhnLibTrapHomeOperatorID                    (120+sysLibTrapCustom)     /**< 0xA87D    GSM Only */
#define     PhnLibTrapCurrentOperatorID                 (121+sysLibTrapCustom)     /**< 0xA87E    GSM Only */
#define     PhnLibTrapUsableSignalStrengthThreshold     (122+sysLibTrapCustom)     /**< 0xA87F    GSM Only */
#define     PhnLibTrapConnectionAvailable               (123+sysLibTrapCustom)     /**< 0xA880    GSM Only */
#define     PhnLibTrapGetPhoneCallStatus                (124+sysLibTrapCustom)     /**< 0xA881           */
#define     PhnLibTrapSendSilentDTMF                    (125+sysLibTrapCustom)     /**< 0xA882           */
#define     PhnLibTrapSetCLIR                           (126+sysLibTrapCustom)     /**< 0xA883    GSM Only */
#define     PhnLibTrapGetCLIR                           (127+sysLibTrapCustom)     /**< 0xA884    GSM Only */
#define     PhnLibTrapMute                              (128+sysLibTrapCustom)     /**< 0xA885           */
#define     PhnLibTrapIsCallMuted                       (129+sysLibTrapCustom)     /**< 0xA886           */
#define     PhnLibTrapGetSMSGateway                     (130+sysLibTrapCustom)     /**< 0xA887    GSM Only */
#define     PhnLibTrapDeactivateBarring                 (131+sysLibTrapCustom)     /**< 0xA888    GSM Only */
#define     PhnLibTrapGPRSAttached                      (132+sysLibTrapCustom)     /**< 0xA889    GSM Only */
#define     PhnLibTrapGPRSPDPContextDefine              (133+sysLibTrapCustom)     /**< 0xA88A    GSM Only */
#define     PhnLibTrapGPRSPDPContextListConstruct       (134+sysLibTrapCustom)     /**< 0xA88B    GSM Only */
#define     PhnLibTrapGPRSPDPContextListDestruct        (135+sysLibTrapCustom)     /**< 0xA88C    GSM Only */
#define     PhnLibTrapGetRemainingPasswordTries         (136+sysLibTrapCustom)     /**< 0xA88D    GSM Only */
#define     PhnLibTrapGPRSQualityOfServiceGet           (137+sysLibTrapCustom)     /**< 0xA88E    GSM Only */
#define     PhnLibTrapGPRSQualityOfServiceSet           (138+sysLibTrapCustom)     /**< 0xA88F    GSM Only */
#define     PhnLibTrapSetBoxNumber                      (139+sysLibTrapCustom)     /**< 0xA890    GSM Only */
#define     PhnLibTrapGetBinary                         (140+sysLibTrapCustom)     /**< 0xA891    GSM Only */
#define     PhnLibTrapNetworkAvailable                  (141+sysLibTrapCustom)     /**< 0xA892    GSM Only */
#define     PhnLibTrapSetFDNEntry                       (142+sysLibTrapCustom)     /**< 0xA893    GSM Only */
#define     PhnLibTrapGetFDNList                        (143+sysLibTrapCustom)     /**< 0xA894    GSM Only */
#define     PhnLibTrapDecodeMMI                         (144+sysLibTrapCustom)     /**< 0xA895    GSM Only */
#define     PhnLibTrapExecuteMMI                        (145+sysLibTrapCustom)     /**< 0xA896    GSM Only */
#define     PhnLibTrapSetActiveLine                     (146+sysLibTrapCustom)     /**< 0xA897    GSM Only */
#define     PhnLibTrapSetBand                           (147+sysLibTrapCustom)     /**< 0xA898    GSM Only */
#define     PhnLibTrapGetBand                           (148+sysLibTrapCustom)     /**< 0xA899    GSM Only */
#define     PhnLibTrapGetCustomServiceProfile           (149+sysLibTrapCustom)     /**< 0xA89A    GSM Only */
#define     PhnLibTrapIsEmergencyNumber                 (150+sysLibTrapCustom)     /**< 0xA89B    GSM Only */
#define     PhnLibTrapAttrGet                           (151+sysLibTrapCustom)     /**< 0xA89C    GSM Only */
#define     PhnLibTrapAttrSet                           (152+sysLibTrapCustom)     /**< 0xA89D    GSM Only */
#define     PhnLibTrapGetSDNList                        (153+sysLibTrapCustom)     /**< 0xA89E    GSM Only */
#define     PhnLibTrapSendLongDTMF                      (154+sysLibTrapCustom)     /**< 0xA89F    GSM Only */
#define     PhnLibTrapGetICCID                          (155+sysLibTrapCustom)     /**< 0xA8A0    GSM Only */
#define     PhnLibTrapGetSpn                            (156+sysLibTrapCustom)     /**< 0xA8A1    GSM Only */
#define     PhnLibTrapGPRSAttachRequest                 (157+sysLibTrapCustom)     /**< 0xA8A2    GSM Only */

// Functions were reserved for GSM/GPRS  (these were reserved when Treo 300 shipped). Do
// not need to be GSM/GPRS specific really now that we have one header file, so treat
// them just as free traps for future functions   
#define     PhnLibTrapSetNoiseCancellation              (158+sysLibTrapCustom)    /**< 0xA8A3              */
#define     PhnLibTrapGetNoiseCancellation              (159+sysLibTrapCustom)    /**< 0xA8A4              */
#define     PhnLibTrapCRSM                              (160+sysLibTrapCustom)    /**< 0xA8A5              */
#define     PhnLibTrapGetDedicatedDataMode              (161+sysLibTrapCustom)    /**< 0xA8A6              */
#define     PhnLibTrapSetDedicatedDataMode              (162+sysLibTrapCustom)    /**< 0xA8A7              */
#define     PhnLibTrapGetMVNOName                       (163+sysLibTrapCustom)    /**< 0xA8A8              */
#define     GSMReserved7                                (164+sysLibTrapCustom)    /**< 0xA8A9              */
#define     GSMReserved8                                (165+sysLibTrapCustom)    /**< 0xA8AA              */
#define     GSMReserved9                                (166+sysLibTrapCustom)    /**< 0xA8AB              */
#define     GSMReserved10                               (167+sysLibTrapCustom)    /**< 0xA8AC              */
#define     GSMReserved11                               (168+sysLibTrapCustom)    /**< 0xA8AD              */
#define     GSMReserved12                               (169+sysLibTrapCustom)    /**< 0xA8AE              */
#define     GSMReserved13                               (170+sysLibTrapCustom)    /**< 0xA8AF              */
#define     GSMReserved14                               (171+sysLibTrapCustom)    /**< 0xA8B0              */
#define     GSMReserved15                               (172+sysLibTrapCustom)    /**< 0xA8B1              */
#define     GSMReserved16                               (173+sysLibTrapCustom)    /**< 0xA8B2              */
#define     GSMReserved17                               (174+sysLibTrapCustom)    /**< 0xA8B3              */
#define     GSMReserved18                               (175+sysLibTrapCustom)    /**< 0xA8B4              */
#define     GSMReserved19                               (176+sysLibTrapCustom)    /**< 0xA8B5              */
#define     GSMReserved20                               (177+sysLibTrapCustom)    /**< 0xA8B6              */
#define     GSMReserved21                               (178+sysLibTrapCustom)    /**< 0xA8B7              */
#define     GSMReserved22                               (179+sysLibTrapCustom)    /**< 0xA8B8              */


#define     PhnLibTrapAPGetNth                          (180+sysLibTrapCustom)    /**< 0xA8B9    CDMA Only */
#define     PhnLibTrapGetMSID                           (181+sysLibTrapCustom)    /**< 0xA8BA    CDMA Only */
#define     PhnLibTrapGetPhnOprtMode                    (182+sysLibTrapCustom)    /**< 0xA8BB    CDMA Only */
#define     PhnLibTrapSetPhnOprtMode                    (183+sysLibTrapCustom)    /**< 0xA8BC    CDMA Only */
#define     PhnLibTrapSendSMSMTAck                      (184+sysLibTrapCustom)    /**< 0xA8BD    CDMA Only */
#define     PhnLibTrapQuerySMSMT                        (185+sysLibTrapCustom)    /**< 0xA8BE    CDMA Only */
#define     PhnLibTrapIsPhoneActivated                  (186+sysLibTrapCustom)    /**< 0xA8BF    CDMA Only */
#define     PhnLibTrapUpdateMessageCount                (187+sysLibTrapCustom)    /**< 0xA8C0    CDMA Only */
#define     PhnLibTrapGetSMSPreference                  (188+sysLibTrapCustom)    /**< 0xA8C1    CDMA Only */
#define     PhnLibTrapSetSMSPreference                  (189+sysLibTrapCustom)    /**< 0xA8C2    CDMA Only */
#define     PhnLibTrapGetSpMode                         (190+sysLibTrapCustom)    /**< 0xA8C3    CDMA Only */
#define     PhnLibTrapLedAlert                          (191+sysLibTrapCustom)    /**< 0xA8C4    CDMA Only */
#define     PhnLibTrapExit911Mode                       (192+sysLibTrapCustom)    /**< 0xA8C5    CDMA Only */
#define     PhnLibTrapMiscDebugInfo                     (193+sysLibTrapCustom)    /**< 0xA8C6    CDMA Only */
#define     PhnLibTrapGetHeadsetConnectedInfo           (194+sysLibTrapCustom)    /**< 0xA8C7    CDMA Only */
#define     PhnLibTrapExitPowerSaveMode                 (195+sysLibTrapCustom)    /**< 0xA8C8    CDMA Only */
#define     PhnLibTrapGetToneLength                     (196+sysLibTrapCustom)    /**< 0xA8C9    CDMA Only */
#define     PhnLibTrapSetToneLength                     (197+sysLibTrapCustom)    /**< 0xA8CA    CDMA Only */
#define     PhnLibTrapUnlockPhoneLock                   (198+sysLibTrapCustom)    /**< 0xA8CB    CDMA Only */
#define     PhnLibTrapLoadCDMATime                      (199+sysLibTrapCustom)    /**< 0xA8CC    CDMA Only */
#define     PhnLibTrapGetCallCountByService             (200+sysLibTrapCustom)    /**< 0xA8CD              */
#define     PhnLibTrapGetDefaultDataApps                (201+sysLibTrapCustom)    /**< 0xA8CE    CDMA Only */
#define     PhnLibTrapModemIssueResetInfo               (202+sysLibTrapCustom)    /**< 0xA8CF    CDMA Only */
#define     PhnLibTrapGetRoamPrefMode                   (203+sysLibTrapCustom)    /**< 0xA8D0    CDMA Only */
#define     PhnLibTrapSetRoamPrefMode                   (204+sysLibTrapCustom)    /**< 0xA8D1    CDMA Only */
#define     PhnLibTrapGetRoamPrefInfo                   (205+sysLibTrapCustom)    /**< 0xA8D2    CDMA Only */
#define     PhnLibTrapSetRoamPrefInfo                   (206+sysLibTrapCustom)    /**< 0xA8D3    CDMA Only */
#define     PhnLibTrapIsSpecialNumber                   (207+sysLibTrapCustom)    /**< 0xA8D4    CDMA Only */
#define     PhnLibTrapSetSpecialNumbers                 (208+sysLibTrapCustom)    /**< 0xA8D5    CDMA Only */
#define     PhnLibTrapGetSpecialNumbers                 (209+sysLibTrapCustom)    /**< 0xA8D6    CDMA Only */
#define     PhnLibTrapGetDTMFburstMode                  (210+sysLibTrapCustom)    /**< 0xA8D7    CDMA Only */
#define     PhnLibTrapSetDTMFburstMode                  (211+sysLibTrapCustom)    /**< 0xA8D8    CDMA Only */
#define     PhnLibTrapSetupDataConfig                   (212+sysLibTrapCustom)    /**< 0xA8D9    CDMA Only */
#define     PhnLibTrapGetTotalDataTxBytes               (213+sysLibTrapCustom)    /**< 0xA8DA    CDMA Only */
#define     PhnLibTrapGetTotalDataRxBytes               (214+sysLibTrapCustom)    /**< 0xA8DB    CDMA Only */
#define     PrvPhnLibTrapCheckPowerSync                 (215+sysLibTrapCustom)    /**< 0xA8DC    CDMA Only */
#define     PhnLibTrapSetPhoneSSMode                    (216+sysLibTrapCustom)    /**< 0xA8DD    CDMA Only */
#define     PhnLibTrapEnableDM                          (217+sysLibTrapCustom)    /**< 0xA8DE    CDMA Only */
#define     PhnLibTrapPowerSaveDisable                  (218+sysLibTrapCustom)    /**< 0xA8DF    CDMA Only */
#define     PhnLibTrapCloseNetIF                        (219+sysLibTrapCustom)    /**< 0xA8E0    CDMA Only */
#define     PhnLibTrapGetMdmPosition                    (220+sysLibTrapCustom)    /**< 0xA8E1    CDMA Only */
#define     PhnLibTrapGetPDSessionConfigParam           (221+sysLibTrapCustom)    /**< 0xA8E2    CDMA Only */
#define     PhnLibTrapSetPDSessionConfigParam           (222+sysLibTrapCustom)    /**< 0xA8E3    CDMA Only */
#define     PhnLibTrapStopPDSession                     (223+sysLibTrapCustom)    /**< 0xA8E4    CDMA Only */
#define     PhnLibTrapSndSave                           (224+sysLibTrapCustom)    /**< 0xA8E5    CDMA Only */
#define     PhnLibTrapSndDelete                         (225+sysLibTrapCustom)    /**< 0xA8E6    CDMA Only */
#define     PhnLibTrapListPDP                           (226+sysLibTrapCustom)    /**< 0xA8E7    CDMA Only */
#define     PhnLibTrapSetPDP                            (227+sysLibTrapCustom)    /**< 0xA8E8    CDMA Only */
#define     PhnLibTrapGetPDP                            (228+sysLibTrapCustom)    /**< 0xA8E9    CDMA Only */
#define     PhnLibTrapCopyPDP                           (229+sysLibTrapCustom)    /**< 0xA8EA    CDMA Only */
#define     PhnLibTrapDeletePDP                         (230+sysLibTrapCustom)    /**< 0xA8EB    CDMA Only */
#define     PhnLibTrapReplacePDPPassword                (231+sysLibTrapCustom)    /**< 0xA8EC    CDMA Only */
#define     PhnLibTrapGetTotalPDPs                      (232+sysLibTrapCustom)    /**< 0xA8ED    CDMA Only */
#define     PhnLibTrapGetOneXStatus                     (233+sysLibTrapCustom)    /**< 0xA8EE    CDMA Only */
#define     PhnLibTrapUpdateCustomizedNV                (234+sysLibTrapCustom)    /**< 0xA8EF    CDMA Only */
#define     PhnLibTrapSetMdmAutoMsgMask                 (235+sysLibTrapCustom)    /**< 0xA8F0    CDMA Only */
#define     PhnLibTrapGetPhoneSSMode                    (236+sysLibTrapCustom)    /**< 0xA8F1    CDMA Only */
#define     PhnLibTrapGetPDPIndexRange                  (237+sysLibTrapCustom)    /**< 0xA8F2    CDMA Only */
#define     PhnLibTrapGetCktSwitchProfile               (238+sysLibTrapCustom)    /**< 0xA8F3    CDMA Only */
#define     PhnLibTrapStartIOTASession                  (239+sysLibTrapCustom)    /**< 0xA8F4    CDMA Only */
#define     PhnLibTrapEndIOTASession                    (240+sysLibTrapCustom)    /**< 0xA8F5    CDMA Only */
#define     PhnLibTrapTunnelIS683ReqMsg                 (241+sysLibTrapCustom)    /**< 0xA8F6    CDMA Only */
#define     PhnLibTrapSelectNAM                         (242+sysLibTrapCustom)    /**< 0xA8F7    CDMA Only */
#define     PhnLibTrapGetNAMInUse                       (243+sysLibTrapCustom)    /**< 0xA8F8    CDMA Only */
#define     PhnLibTrapCommitIOTAParams                  (244+sysLibTrapCustom)    /**< 0xA8F9    CDMA Only */
#define     PhnLibTrapReadIOTAItem                      (245+sysLibTrapCustom)    /**< 0xA8FA    CDMA Only */
#define     PhnLibTrapWriteIOTAItem                     (246+sysLibTrapCustom)    /**< 0xA8FB    CDMA Only */
#define     PhnLibTrapGetIOTAStatus                     (247+sysLibTrapCustom)    /**< 0xA8FC    CDMA Only */
#define     PhnLibTrapSetCurrentUsageHandler            (248+sysLibTrapCustom)    /**< 0xA8FD    CDMA Only */
#define     PhnLibTrapSendDTMFBurst                     (249+sysLibTrapCustom)    /**< 0xA8FE    CDMA Only */
#define     PhnLibTrapAPNewAddress                      (250+sysLibTrapCustom)    /**< 0xA8FF    CDMA Only */
#define     PhnLibTrapAPGetField                        (251+sysLibTrapCustom)    /**< 0xA900    CDMA Only */
#define     Reserved2                                   (252+sysLibTrapCustom)    /**< 0xA901    CDMA Only */
#define     PhnLibTrapCardInfoEx                        (253+sysLibTrapCustom)    /**< 0xA902    CDMA Only */
#define     PhnLibTrapBoxInformationEx                  (254+sysLibTrapCustom)    /**< 0xA903    CDMA Only */
#define     PhnLibTrapSignalQualityEx                   (255+sysLibTrapCustom)    /**< 0xA904    CDMA Only */
#define     PhnLibTrapReplacePDPNAI                     (256+sysLibTrapCustom)    /**< 0xA905    CDMA Only */
#define     PhnLibTrapReplacePDPSvcString               (257+sysLibTrapCustom)    /**< 0xA906    CDMA Only */
#define     PhnLibTrapPDPLockUnlock                     (258+sysLibTrapCustom)    /**< 0xA907    CDMA Only */
#define     PhnLibTrapPhoneRestore                      (259+sysLibTrapCustom)    /**< 0xA908    CDMA Only */
#define     PhnLibTrapVerifyICDVer                      (260+sysLibTrapCustom)    /**< 0xA909    CDMA Only */
#define     PhnLibTrapGetToneIDsEx                      (261+sysLibTrapCustom)    /**< 0xA90A    CDMA only */
#define     PhnLibTrapPlayToneEx                        (262+sysLibTrapCustom)    /**< 0xA90B    CDMA only */
#define     PhnLibTrapEnhancedRoamIndicator             (263+sysLibTrapCustom)    /**< 0xA90C    CDMA only */
#define     PhnLibTrapNewMOMessage                      (264+sysLibTrapCustom)    /**< 0xA90D    CDMA only */
#define     PhnLibTrapGetTTYSetting                     (265+sysLibTrapCustom)    /**< 0xA90E              */
#define     PhnLibTrapSetTTYSetting                     (266+sysLibTrapCustom)    /**< 0xA90F              */
#define     PhnLibTrapGetRadioState                     (267+sysLibTrapCustom)    /**< 0xA910              */
#define     PhnLibTrapGetLineState                      (268+sysLibTrapCustom)    /**< 0xA911              */
#define     PhnLibTrapGetLengthDetails                  (269+sysLibTrapCustom)    /**< 0xA912              */
#define     PhnLibTrapGetPicturesToken                  (270+sysLibTrapCustom)    /**< 0xA913    CDMA Only */
#define     PhnLibTrapSetPicturesToken                  (271+sysLibTrapCustom)    /**< 0xA914    CDMA Only */
#define     PhnLibTrapGetPicturesServerInfo             (272+sysLibTrapCustom)    /**< 0xA915    CDMA Only */
#define     PhnLibTrapSendPendingMessages               (273+sysLibTrapCustom)    /**< 0xA916              */
#define     PhnLibTrapForceDataDormant                  (274+sysLibTrapCustom)    /**< 0xA917    CDMA Only */
#define     PhnLibTrapGetSysPrefMode                    (275+sysLibTrapCustom)    /**< 0xA918    CDMA Only */
#define     PhnLibTrapSetSysPrefMode                    (276+sysLibTrapCustom)    /**< 0xA919    CDMA Only */
#define     PhnLibTrapGetLifeTimer                      (277+sysLibTrapCustom)    /**< 0xA91A              */
#define     PhnLibTrapGetVoicePrivacyMode               (278+sysLibTrapCustom)    /**< 0xA91B    CDMA Only */
#define     PhnLibTrapSetVoicePrivacyMode               (279+sysLibTrapCustom)    /**< 0xA91C    CDMA Only */
#define     PhnLibTrapGetMMSServerInfo                  (280+sysLibTrapCustom)    /**< 0xA91D    CDMA Only */
#define     PhnLibTrapGetSMSSegmentInfo                 (281+sysLibTrapCustom)    /**< 0xA91E    CDMA Only */
#define     PhnLibTrapDebugHook                         (282+sysLibTrapCustom)    /**< 0xA91F              */
#define     PhnLibTrapContinuePhonePowerOn              (283+sysLibTrapCustom)    /**< 0xA920              */
#define     PhnLibTrapContinuePhonePowerOff             (284+sysLibTrapCustom)    /**< 0xA921              */
#define     PhnLibTrapSetSMSCallbackNumber              (285+sysLibTrapCustom)    /**< 0xA922              */
#define     PhnLibTrapGetDeviceId                       (286+sysLibTrapCustom)    /**< 0xA923              */
#define     PhnLibTrapSendFlash                         (287+sysLibTrapCustom)    /**< 0xA924    CDMA ONLY */
#define     PhnLibTrapGetCDMASystemInfo                 (288+sysLibTrapCustom)    /**< 0xA925    CDMA ONLY */
#define     PhnLibTrapGetHomepageFromRadio              (289+sysLibTrapCustom)    /**< 0xA926    CDMA ONLY */
#define     PhnLibTrapGetProxyInfo                      (290+sysLibTrapCustom)    /**< 0xA927    CDMA ONLY */
#define     PhnLibTrapIsRadioGenericCDMA                (291+sysLibTrapCustom)    /**< 0xA928    CDMA ONLY */
#define     PhnLibTrapGetGenericCDMAPreference          (292+sysLibTrapCustom)    /**< 0xA929    CDMA ONLY */          
#define     PhnLibTrapIsCarrierDefinedEmergencyNumber   (293+sysLibTrapCustom)    /**< 0xA92A    CDMA ONLY */
#define     PhnLibTrapGetDialPrefix                     (294+sysLibTrapCustom)    /**< 0xA92B    CDMA ONLY */
#define     PhnLibTrapSetDialPrefix                     (295+sysLibTrapCustom)    /**< 0xA92C    CDMA ONLY */
#define     PhnLibTrapGetDataSessionInfo                (296+sysLibTrapCustom)    /**< 0xA92D              */
#define     PhnLibTrapGetMediaTVSettings                (297+sysLibTrapCustom)    /**< 0xA92E    CDMA ONLY */
#define     PhnLibTrapOffHold                           (298+sysLibTrapCustom)    /**< 0xA92F              */
#define     PhnLibTrapGetMMSServerInfoEx                (299+sysLibTrapCustom)    /**< 0xA930    CDMA ONLY */
#define     PhnLibTrapGetDataLife                       (300+sysLibTrapCustom)    /**< 0xA931    CDMA ONLY */
#define     PhnLibTrapHACSetting                        (301+sysLibTrapCustom)    /**< 0xA932              */
#define     PhnLibTrapGetMEID                   (302+sysLibTrapCustom)    /**< 0xA933              */ 
#define     PhnLibTrapLast                              (302+sysLibTrapCustom)    /**< 0xA933              */
#endif
