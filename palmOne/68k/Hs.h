/**
 * \file 68K/Hs.h
 *	Main Handspring Palm OS API Header file
 * 
 * This is the main header file that all third party developers. A developer
 * would include if they wanted to use any of the handspring components. This
 * includes all the extentions, libraries and any app compoennts. This file is
 * for api calls that will work only in  code. This file automatically includes
 * the Common/HsCommon.h file so there is no need for a developer to include that file
 * seperately. This is the ONLY file that should be needed for any of the
 * handspring specific components. This file includes all the other header files
 * for the 68K space relative to this directory. It has to stay at the top of
 * the Handspring/68K tree. 
 *
 * \license
 *
 *    Copyright (c) 2002 Handspring Inc., All Rights Reserved
 *
 * \author Kiran Prasad
 *
 * $Id: //device/handheld/dev/sdk/2.2.2/incs/68k/Hs.h#1 $
 *
 *****************************************************************************/

#ifndef __HS_68K_H__

#define __HS_68K_H__


#include <Common/HsCommon.h>

#include <68K/System/FarCall.h>
#include <68K/System/HsHelper.h>
#include <68K/System/HsExgMgr.h>

#include <68K/Libraries/FavoritesDBLibrary/FavoritesDBLib.h>

#include <68K/Libraries/Imaging/ImageLib.h>
#include <68K/Libraries/Camera/CameraLib.h>

#include <68K/Libraries/HTTP/HS_HTTPLib68K.h>

#include <68K/Libraries/VMFont/palmOneVMFontLib.h>

#endif /* __HS_68K_H__ */
