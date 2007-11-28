/******************************************************************************
 * Copyright (c) 2004 palmOne, Inc. or its subsidiaries.
 * All rights reserved.
 *****************************************************************************/
/** 
 *
 * @ingroup PmKeyLib
 *
 */
 
/**
 * @file 	HsKeyTypes.h
 *
 * @brief   palmOne's key constant definitions shared by DAL and Palm OS
 * (see Palm OS' CmnKeyTypes.h)
 *
 */

#ifndef __HSKEYTYPES_H__
#define __HSKEYTYPES_H__


/********************************************************************
 * Definition of the Bit numbers corresponding to Palm's key bits
 * (see CmnKeyTypes.h)
 ********************************************************************/
/** Power key */
#define	keyBitNumPower					 0	
/** Page-up */
#define	keyBitNumPageUp					 1	
/** Page-down */
#define	keyBitNumPageDown				 2	
/** App #1 */
#define	keyBitNumHard1					 3	
/** App #2 */
#define	keyBitNumHard2					 4	
/** App #3 */
#define	keyBitNumHard3					 5	
/** App #4 */
#define	keyBitNumHard4					 6	
/** Button on cradle */
#define	keyBitNumCradle					 7	
/** Antenna "key" <chg 3-31-98 RM> */
#define	keyBitNumAntenna				 8	
/** Contrast key */
#define	keyBitNumContrast				 9	


/** jog wheel up */
#define keyBitNumJogUp					12	
/** jog wheel down */
#define keyBitNumJogDown				13	
/** press/center on jog wheel */
#define keyBitNumJogPress				14	
/** jog wheel back button */
#define keyBitNumJogBack				15	
/** 5-way rocker up */
#define keyBitNumRockerUp				16	
/** 5-way rocker down */
#define keyBitNumRockerDown				17	
/** 5-way rocker left */
#define keyBitNumRockerLeft				18	
/** 5-way rocker right */
#define keyBitNumRockerRight			19	
/** 5-way rocker center/press */
#define keyBitNumRockerCenter			20	


/** Definition of the 1st extension bit field returned in the bit
 *  field array passed into KeyCurrentStateEx.  The following bit
 *  names are based on the American keyboard.
 */

/** Q key */
#define	keyBitExt1Q				0x00000001	
#define keyBitNumExt1Q				 	 0
/** W key */
#define	keyBitExt1W				0x00000002	
#define keyBitNumExt1W					 1
/** E key */
#define	keyBitExt1E				0x00000004	
#define keyBitNumExt1E					 2
/** R key */
#define	keyBitExt1R				0x00000008	
#define keyBitNumExt1R					 3
/** T key */
#define	keyBitExt1T				0x00000010	
#define keyBitNumExt1T					 4
/** Y key */
#define	keyBitExt1Y				0x00000020	
#define keyBitNumExt1Y					 5
/** U key */
#define	keyBitExt1U				0x00000040	
#define keyBitNumExt1U					 6
/** I key */
#define	keyBitExt1I				0x00000080	
#define keyBitNumExt1I					 7
/** O key */
#define	keyBitExt1O				0x00000100	
#define keyBitNumExt1O					 8
/** P key */
#define	keyBitExt1P				0x00000200	
#define keyBitNumExt1P					 9
/** A key */
#define	keyBitExt1A				0x00000400	
#define keyBitNumExt1A					10
/** S key */
#define	keyBitExt1S				0x00000800	
#define keyBitNumExt1S					11
/** D key */
#define	keyBitExt1D				0x00001000	
#define keyBitNumExt1D					12
/** F key */
#define	keyBitExt1F				0x00002000	
#define keyBitNumExt1F					13
/** G key */
#define	keyBitExt1G				0x00004000	
#define keyBitNumExt1G					14
/** H key */
#define	keyBitExt1H				0x00008000	
#define keyBitNumExt1H					15
/** J key */
#define	keyBitExt1J				0x00010000	
#define keyBitNumExt1J					16
/** K key */
#define	keyBitExt1K				0x00020000	
#define keyBitNumExt1K					17
/** L key */
#define	keyBitExt1L				0x00040000	
#define keyBitNumExt1L					18
/** Backspace key */
#define keyBitExt1Bksp			0x00080000	
#define keyBitNumExt1Bksp				19
/** Option key */
#define	keyBitExt1Opt			0x00100000	
#define keyBitNumExt1Opt				20
/** Z key */
#define	keyBitExt1Z				0x00200000	
#define keyBitNumExt1Z					21
/** X key */
#define	keyBitExt1X				0x00400000	
#define keyBitNumExt1X					22
/** C key */
#define	keyBitExt1C				0x00800000	
#define keyBitNumExt1C					23
/** V key */
#define	keyBitExt1V				0x01000000	
#define keyBitNumExt1V					24
/** B key */
#define	keyBitExt1B				0x02000000	
#define keyBitNumExt1B					25
/** N key */
#define	keyBitExt1N				0x04000000	
#define keyBitNumExt1N					26
/** M key */
#define	keyBitExt1M				0x08000000	
#define keyBitNumExt1M					27
/** Period key */
#define	keyBitExt1Period		0x10000000	
#define keyBitNumExt1Period				28
/** Enter key */
#define keyBitExt1Enter			0x20000000	
#define keyBitNumExt1Enter				29
/** Shift key */
#define	keyBitExt1Shift			0x40000000	
#define keyBitNumExt1Shift				30
/** Space key */
#define keyBitExt1Space			0x80000000	
#define keyBitNumExt1Space				31
/** All keys */
#define	keyBitsAllExt1			0xFFFFFFFF	


/** Definition of the 2nd extension bit field returned in the bit
 *  field array passed into KeyCurrentStateEx.  The following bit
 *  names are based on the American keyboard.
 */

/** Symbol (list type) */
#define	keyBitExt2Symbol		0x00000001	
#define keyBitNumExt2Symbol				 0
/** Cmd/menu */
#define	keyBitExt2CmdMenu		0x00000002	
#define keyBitNumExt2CmdMenu			 1
// OBSOLETE: Treo Jog up
// #define keyBitExt2JogUp		0x00000004	
// #define keyBitNumExt2JogUp			 2
// OBSOLETE: Treo Jog down
// #define keyBitExt2JogDown		0x00000008	
// #define keyBitNumExt2JogDown			 3
// OBSOLETE: Treo Jog scan
// #define keyBitExt2JogScan		0x00000010	
// #define keyBitNumExt2JogScan			 4
/** Launcher */
#define keyBitExt2Launcher		0x00000020	
#define keyBitNumExt2Launcher			 5
/** UNUSED: Available for use (was going to be RadioPower) */
#define keyBitExt2Unused6		0x00000040	
#define keyBitNumExt2Unused6			 6
/** Manufacturing Test */
#define keyBitExt2MfgTest		0x00000080	
#define keyBitNumExt2MfgTest			 7
/** Volume Up */
#define keyBitExt2VolumeUp		0x00000100	
#define keyBitNumExt2VolumeUp			 8
/** Volume Down */
#define keyBitExt2VolumeDown	0x00000200	
#define keyBitNumExt2VolumeDown			 9
/** Side button */
#define keyBitExt2Side			0x00000400	
#define keyBitNumExt2Side				10
/** Zero key */
#define keyBitExt2Zero			0x00000800	
#define keyBitNumExt2Zero			    11
/** Right Shift key */
#define keyBitExt2RShift		0x00001000	
#define keyBitNumExt2RShift				12
/** Our Phone key */
#define keyBitExt2Hard11		0x00002000	
#define keyBitNumExt2Hard11				13
/** All keys */
#define	keyBitsAllExt2			0xFFFFFFFF	


/** This keyBit is guaranteed not to map to any character */
#define keyBitNumUnused			0xFFFF


/********************************************************************
 *Special (impossible) combinations of modifiers that we use
 * to send special events through the key queue
 ********************************************************************/
#define specialHoldKeyMask    (autoRepeatKeyMask | doubleTapKeyMask)
#define specialReleaseKeyMask (autoRepeatKeyMask | poweredOnKeyMask)
#define specialAvail3KeyMask  (doubleTapKeyMask  | poweredOnKeyMask)
#define specialAvail4KeyMask  (autoRepeatKeyMask | doubleTapKeyMask | poweredOnKeyMask)

#define specialKeyMask		  (autoRepeatKeyMask | doubleTapKeyMask | poweredOnKeyMask)


/********************************************************************
 *Extended shift state values for supporting an option and option lock state
 ********************************************************************/
/** Temp shift states (see Graffiti.h) */
#define hsGrfTempShiftOpt		(grfTempShiftLower+1)

/** Local names for shift lock states (see GraffitiShift.h) */
#define hsGlfOptLock			(glfNumLock << 1)

// Treo names (for compatibility)
//#define keyGrfTempShiftOption	hsGrfTempShiftOpt
//#define keyGlfOptLock			hsGlfOptLock

/** Internal shift states (extends GsiShiftStateTag) */
#define hsGsiOptLock			0xFE
#define hsGsiShiftOpt			0xFF

#endif  // __HSKEYTYPES_H__


