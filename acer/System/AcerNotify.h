/******************************************************************************
*                                                                             *
*            (C) Copyright 2000-2001, Acer Incorporated                       *
*                                                                             *
*-----------------------------------------------------------------------------*
*                                                                             *
*    <IDENTIFICATION>                                                         *
*       file name    : AcerNotify.h                                           *
*                                                                             *
*    <PROFILE>                                                                *
*       Type/DataStructure of Notifications for Acer System                   *
*                                                                             *
*    <HISTORY>                                                                *
*       Started on   : Aug 01 2001                                            *
*                                                                             *
******************************************************************************/
// this file is best viewed by setting TAB-stop as 4

#ifndef __ACERNOTIFY_H__
#define __ACERNOTIFY_H__

/******************************************************************************
*     Includes                                                                *
******************************************************************************/
#include <NotifyMgr.h>
#include <AcerSystemResources.h>

/******************************************************************************
*     Type/DataStructure of Notifications                                     *
******************************************************************************/


#define acerSysNotifyAudioPlayWriteData		'AuWd'
#define acerSysNotifyAudioPlayEnd			'AuEd'
#define acerSysNotifyAudioRecReadData		'AuRd'
#define acerSysNotifyAudioDSPError			'AuDe'
#define acerSysNotifyAudioVFSError			'AuVe'
#define acerSysNotifyAudioInternalError		'AuIe'
#define acerSysNotifyAudioLowBattery		'AuLb'
#define acerSysNotifyAudioCheckLCDStatus	'AuCl'

#define acerSysNotifyIRQ3Event				'msQ3'
#define acerSysNotifyKeyLockEvent			'kLck'
#define acerSysNotifyKeyUnLockEvent			'kUlk'
#define acerSysNotifyExternalIRQ6High		'eI6H'
#define acerSysNotifyExternalIRQ6Low		'eI6L'

#endif	// __ACERNOTIFY_H__

