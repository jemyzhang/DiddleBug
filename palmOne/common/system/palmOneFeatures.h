/******************************************************************************
 * Copyright (c) 2005 palmOne, Inc. or its subsidiaries.
 * All rights reserved.
 *****************************************************************************/
/**
 *@ingroup System
 *
 */

/**
 * @file 	palmOneFeatures.h
 * @version 1.0
 * @date 	02/29/2002
 *
 * @brief Contains Palm-specific feature definitions.
 *
 * <hr>
 */

#ifndef __PALMONEFEATURES_H__
#define	__PALMONEFEATURES_H__

/**
 * The following feature should be defined on all devices that support hi-res
 * screens & include NotePad 2.0 or higher.  As the OS does not yet provide a
 * method for the desktop SW/conduits to query the handheld in order to
 * determine the density capabilities of the device, this feature has been
 * added.  By testing the presence/content of this feature, the device can
 * properly convert any old lo-density RLE data to be hi-density PNG data on
 * the first hotsync to the device rather than have to do a note-by-note
 * conversion at run time on the handheld the first time each old note has been
 * opened.
 */
#define densityFtrCreator       'dnsT'		/**< 		*/
#define densityFtrVersion       0		/**< 		*/
/**
 * one of the DensityType values defined in the Bitmap.h file should be stored
 * in this feature designating the density supported by the device.
 */

/**
 * @name
 *
 */
/*@{*/
#define screenFtrCreator		'scnP'		/**< Creator for screen feature */
#define dpiFtrValue				1	/**< Screen DPI feature */
/*@}*/

/**
 * @name Headset feature
 *
 */
/*@{*/
#define headsetFtrCreator		'hseT'		/**< 		*/
#define headsetFtrVersion		0		/**< 		*/
/*@}*/

/**
 * @name
 *
 */
/*@{*/
#define	sysExternalHeadsetInsertEvent		'hsiN'	/**< Broadcast when headset is inserted */
#define	sysExternalHeadsetRemoveEvent		'hsrM'	/**< Broadcast when headset is removed */
/*@}*/

/**
 * Hard Disk Spinup notification
 *
 * Notification sent by the hard disk slot driver that the disk has spun up
 *
 */
#define sysNotifyHardDiskSpinup   'HSPU'

/**
 * External dialog open/close from backgound task notification
 *
 * These notifications should be sent by a background task if it wants to 
 * open a new/close a dialog. This notification should be used in conjunction
 * with checking the SysUIBusy flag. The logic to be used is as follows:
 *     if (!SysUIBusy)
 *         display dialog
 *     else
 *         call SysNotifyBroadcast with sysNotifyExternalDialogOpenedEvent
 *         if (!SysUIBusy)
 *             display dialog
 *
 * The foreground application should also be prepared to handle this event 
 * especially if it is using the LCD overlay. It should be prepared to receive
 * more than one such notification by keeping a count of open dialog event. 
 * This count should be decremented for every 
 * sysNotifyExternalDialogClosedEvent. Only when this count becomes zero should
 * the application restart using the display (LCD overlay). 
 *
 */
#define sysNotifyExternalDialogOpenedEvent   'edgO'  /**< Broadcast before opening a dialog */
#define sysNotifyExternalDialogClosedEvent   'edgC'  /**< Broadcast after closing the dialog */

 

/**
 * @name USB features
 *
 * These features provide information on the USB capabilities of the device.
 *
 */
/*@{*/
#define pmUsbFtrCreator		'pUSB'		/**< Creator for USB feature */
#define pmUsbFtrVersion			0		/**< USB feature number */


#define USBFeatureCreator			'fUSB'
#define USBFeatureVersion			0

#define kUSBMTP						0x0001
#define kUSBTransfer				0x0002
#define kUSBHotSync					0x0004
#define kUSBDriveMode				0x0008
#define kUSBConsole					0x0010
#define kUSBDUN						0x0020
#define kUSBLogging					0x0040

#define USBSpeedCreator				'sUSB'
#define USBSpeedVersion				0

#define kUSBFSClient				0x0001
#define kUSBHSClient				0x0002
#define kUSBFSOnTheGo				0x0004
#define kUSBHSOnTheGo				0x0008
#define kUSBFSOnTheGoMultiDevice	0x0010
#define kUSBHSOnTheGoMultiDevice	0x0020

#define USBChargeCreator			'cUSB'
#define USBChargeVersion			0

#define kUSB500mACharge				0x0001

#define USBHwrCreator				'hUSB'
#define USBHwrVersion				0

#define kUSBMiniConnector			0x0001
#define kUSBMultiConnector			0x0002

#define irFtrVersionNumber			1
#define irFtrLowBatteryLevel		1

/*@}*/




#endif // __PALMONEFEATURES_H__
