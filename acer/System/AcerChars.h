/******************************************************************************
*                                                                             *
*            (C) Copyright 2000-2001, Acer Incorporated                       *
*                                                                             *
*-----------------------------------------------------------------------------*
*                                                                             *
*    <IDENTIFICATION>                                                         *
*       file name    : AcerChars.h                                            *
*                                                                             *
*    <PROFILE>                                                                *
*       Acer-specific Virtual character definition                            *
*                                                                             *
*    <HISTORY>                                                                *
*       Started on   : Aug 01 2001                                            *
*                                                                             *
******************************************************************************/
// this file is best viewed by setting TAB-stop as 4

#ifndef __ACERCHARS_H__
#define __ACERCHARS_H__

#ifdef	vchrAcerMin
#undef	vchrAcerMin
#endif
#ifdef	vchrAcerMax
#undef	vchrAcerMax
#endif

//		Acer
#define	vchrAcerMin					(0x1800)			// 256 command keys
#define	vchrAcerMax					(0x18FF)
#define	vchrAcerHardRecord			(0x1800)
#define	vchrAcerSilkCIME			(0x1801)
#define vchrAcerSilkHW				(0x1802)
#define vchrAcerSilkConvert			(0x1803)
#define vchrAcerSilkConfirm			(0x1804)
#define vchrAcerSilkDictionary		(0x1805)
#define	vchrAcerHardMultiFunc		(0x1806)
#define	vchrAcerHardBack			(0x1807)
#define	vchrAcerSilkPunctuation		(0x1808)


/***** Notification Trigger *****/
#define vchrAcerSysNotify			(0x18FF)


#endif // __ACERCHARS_H__

