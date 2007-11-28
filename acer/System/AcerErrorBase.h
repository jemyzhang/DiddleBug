/******************************************************************************
*                                                                             *
*            (C) Copyright 2000-2001, Acer Incorporated                       *
*                                                                             *
*-----------------------------------------------------------------------------*
*                                                                             *
*    <IDENTIFICATION>                                                         *
*       file name    : AcerErrorBase.h                                        *
*                                                                             *
*    <PROFILE>                                                                *
*       The defines of error base in sony Palm OS PDA system                  *
*                                                                             *
*    <HISTORY>                                                                *
*       Started on   : Aug 01 2001                                            *
*                                                                             *
******************************************************************************/
// this file is best viewed by setting TAB-stop as 4

#ifndef __ACERERRORBASE_H__
#define __ACERERRORBASE_H__

#include <ErrorBase.h>

// error code
#define acerErrorClass				(oemErrorClass)
#define acerAudioErrorClass			(acerErrorClass | 0x100)
#define acerHDErrorClass			(acerErrorClass | 0x200)

#endif // __ACERERRORBASE_H__

