/*******************************************************************************
 * Copyright (c) 2004-2005 palmOne, Inc. or its subsidiaries.
 * All rights reserved.
 ******************************************************************************/
/**
 * @defgroup	Imaging Imaging Library
 * @brief		This library provides support for image drawing and manipulation on
 *				Treo 600 devices.
 *
 * Applications can use this library to draw JPEG images on the screen or create
 * thumbnails and such on the Treo 600 devices. This library also allows some
 * interaction with the camera within the 3rd party application itself. For the
 * newer devices, developers should try to use palmOne Photo Library before using
 * the Image Library.
 *
 * palmOne Photo Library allows better integration with the built-in Media application
 * and storing image on the device. Developers who would like to use this library
 * should first check the version of the library and then make the appropriate API
 * calls depending on the version.
 *
 * The database API provides applications with the ability to:
 *  - Get the list of albums.
 *  - Create, delete and rename albums.
 *  - Get the list of photos in each album.
 *  - Add photos to albums.
 *
 * The image manipulation API provides applications with the ability to:
 *  - Create and capture images and video clips.
 *  - Read and write image data.
 *  - Convert images to different formats.
 *  - Display images.
 *  - Scale, rotate and crop images.
 *
 * @{
 * @}
 */
/**
 * @ingroup Imaging
 */

/**
 * @file 	ImageLibCommon.h
 * @version 1.0
 * @brief	Public 68k common header file for the Image Library API on Treo 600 devices.
 *
 * This file contains common defines and error codes to be used with Imaging Library API.
 * <hr>
 */

#ifndef IMAGELIB_COMMON_H
#define IMAGELIB_COMMON_H

#include <PalmOS.h>
#include <PalmTypes.h>

#define ImageLibCreatorID 'imgL'            /**< Imaging library creator ID */
#define ImageLibTypeID    'libr'            /**< Standard library type */
#define ImageLibName      "ImageLibrary"    /**< Imaging library database name */

/**
 * @name	Preview Image Dimensions
 * @brief	Preview height and width in standard pixels. On a double density device,
 * 			the bitmap returned would be twice these dimensions.
 */ 
/*@{*/
#define kPalmSizeImageWidth     160
#define kPalmSizeImageHeight    120
#define kFullSizeImageWidth     640
#define kFullSizeImageHeight    480
#define kThumbnailImageWidth     40
#define kThumbnailImageHeight    30
#define kPreviewBitmapWidth     (kPalmSizeImageWidth)
#define kPreviewBitmapHeight    (kPalmSizeImageHeight)
/*@}*/

#define kInvalidRecIndex ((UInt16)0xFFFF ) /**< define for invalid record index */

#define kImageFilenameLength    (dmDBNameLength) /**< max filename length */

/**
 * @name Sort Orders
 */
/*@{*/
#define kNoSortOrder (0)
#define kSortByName  (1)
#define kSortByDate  (2)
#define kSortByNameDesc  (3)
#define kSortByDateDesc  (4)
/*@}*/

/**
 * @name Image Operation Codes
 */
/*@{*/
#define kResizeOperation (0)
#define kJPEGOperation   (1)
#define kVFSOperation    (2)
/*@}*/

/** Handle returned by ImageLibIterateImages or ImageLibIterateVFSImages. */
typedef void * ImageIteratorHandle;

/** Passed in when calling ImageLibSetProgressCallback */
typedef void (*ImageLibProgressCallbackType) (Int32 progress, Int32 max, Int8 operation);

/**
 * @name Error Codes
 */
/*@{*/
#define imageLibErrorClass				(appErrorClass | 0x1000)
#define imageLibErrBadParam             (imageLibErrorClass)
#define imageLibErrNoCamera             (imageLibErrorClass + 1)
#define imageLibErrInternalFailure      (imageLibErrorClass + 2)
#define imageLibErrWriteFailure         (imageLibErrorClass + 3)
#define imageLibErrReadFailure          (imageLibErrorClass + 4)
#define imageLibErrBadData              (imageLibErrorClass + 5)
#define imageLibErrBadImageType         (imageLibErrorClass + 6)
#define imageLibErrBadImageDepth        (imageLibErrorClass + 7)
#define imageLibErrBadDimensions        (imageLibErrorClass + 8)
#define imageLibErrInitializationFailed (imageLibErrorClass + 9)
#define imageLibErrBadColorspace        (imageLibErrorClass + 10)
#define imageLibErrBadThumbnailType     (imageLibErrorClass + 11)
#define imageLibErrThumnailExists       (imageLibErrorClass + 12)
#define imageLibErrBadScaleFactor       (imageLibErrorClass + 13)
#define imageLibErrBadBitmapType        (imageLibErrorClass + 14)
#define imageLibErrImageAlreadyDownsized (imageLibErrorClass + 15)
#define imageLibErrImageLocked           (imageLibErrorClass + 16)
/*@}*/

#endif
