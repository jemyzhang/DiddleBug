/******************************************************************************
*                                                                             *
*            (C) Copyright 2000-2001, Acer Incorporated                       *
*                                                                             *
*-----------------------------------------------------------------------------*
*                                                                             *
*    <IDENTIFICATION>                                                         *
*       file name    : AcerAudioLib.h                                         *
*                                                                             *
*    <PROFILE>                                                                *
*       Audio Library function prototype                                      *
*                                                                             *
*    <HISTORY>                                                                *
*       Started on   : Aug 01 2001                                            *
*                                                                             *
******************************************************************************/
// this file is best viewed by setting TAB-stop as 4

#ifndef __ACERAUDIOLIB_H__
#define __ACERAUDIOLIB_H__

#include <AcerErrorBase.h>
#include <AcerSystemResources.h>


// BUILDING_APPLICATION
#if BUILDING_AUDIO_LIB
	// direct link to library code
	#define AUDIO_LIB_TRAP(trapNum) 
#else
	// else someone else is including this public header file; use traps
	#define AUDIO_LIB_TRAP(trapNum)	SYS_TRAP(trapNum)
#endif



/********************************************************************
 * Type and creator of Audio Library database
 ********************************************************************/
#define	audioLibCreatorID			acerSysFileCAudioLib
#define	audioLibTypeID				acerSysFileTAudioLib


/********************************************************************
 * Internal library name which can be passed to SysLibFind()
 ********************************************************************/
#define	audioLibName				acerSysLibNameAudio

/************************************************************
 * Audio Library result codes
 * (appErrorClass is reserved for 3rd party apps/libraries.
 * It is defined in SystemMgr.h)
 *************************************************************/

#define audErrNotOpen				(acerAudioErrorClass | 0x01)	// library is not open
#define audErrStillOpen				(acerAudioErrorClass | 0x02)	// returned from AudioLibClose() if
																	// the library is still open by other applications
#define audErrMemory				(acerAudioErrorClass | 0x03)	// memory error occurred when creating library gP
#define audErrBadParam				(acerAudioErrorClass | 0x04)	// invalid parameter
#define audErrUnimplemented			(acerAudioErrorClass | 0x05)	// DSP connection fail
#define audErrInvalidData			(acerAudioErrorClass | 0x06)	// Bad audio data (not mp3 format audio data)
#define audErrUnsupportedFormat		(acerAudioErrorClass | 0x07)	// Unsupported play/record mp3 format
#define audErrUnsupportedQuality	(acerAudioErrorClass | 0x08)	// Unsupported record sampling rate 16Khz

#define audErrDSPNotPowerOn			(acerAudioErrorClass | 0x09)	// DSP doesn't be powered on
#define audErrCanNotInitialDSP		(acerAudioErrorClass | 0x0A)	// DSP doesn't be initialized successfully

#define audErrCanNotPlayAgain		(acerAudioErrorClass | 0x0B)	// Can not continue playing
#define audErrCanNotOpenSourceData	(acerAudioErrorClass | 0x0C)	// Can not open source data
#define audErrCanNotOpenSourcePdb	(acerAudioErrorClass | 0x0D)	// Can not open pdb database
#define audErrCanNotOpenSourceFile	(acerAudioErrorClass | 0x0E)	// Can not open file in memory stick
#define audErrCanNotOpenSourceList	(acerAudioErrorClass | 0x0F)	// Can not open play list

#define audErrCanNotRecordAgain		(acerAudioErrorClass | 0x10)	// insufficient memory after recording for a while
#define audErrCanNotOpenTargetData	(acerAudioErrorClass | 0x11)	// Can not open target data
#define audErrCanNotOpenTargetPdb	(acerAudioErrorClass | 0x12)	// Can not open pdb database
#define audErrCanNotOpenTargetFile	(acerAudioErrorClass | 0x13)	// Can not open file in memory stick
#define audErrInsufficientMemory	(acerAudioErrorClass | 0x14)	// insufficient memory and library can't start to record

#define audErrLowBatteryContinuePlay	(acerAudioErrorClass | 0x15)// user wishes to continue playing current song after receiving low battery notification
#define audErrLowBatteryStopPlay		(acerAudioErrorClass | 0x16)// user wishes to stop playing current song after receiving low battery notification
#define audErrLowBatteryContinueRecord	(acerAudioErrorClass | 0x17)// user wishes to continue recording after receiving low battery notification
#define audErrLowBatteryStopRecord		(acerAudioErrorClass | 0x18)// user wishes to stop recording after receiving low battery notification

#define audErrWrongRefNum				(acerAudioErrorClass | 0x19)// library reference number is wrong
#define audErrVFSOperation				(acerAudioErrorClass | 0x1A)// expansion card operation error
#define audErrInternal					(acerAudioErrorClass | 0x1B)// internal memory operation error
#define audErrGlobalParameterInvalid	(acerAudioErrorClass | 0x1C)// global variables are wrong
#define audErrCanNotChangeSong			(acerAudioErrorClass | 0x1D)// current song is first/final song
#define audErrCanNotSwitchPlayPause		(acerAudioErrorClass | 0x1E)// because Dsp ping/pong buffer isn't fed, and Dsp is in loop, we can't issue any command

#define audErrDSPAbsent					(acerAudioErrorClass | 0x1F)// Dsp is absent in s20, s50

// for v2.1, can not set repeat number
// 03/14/03 whitney chang Add >>

#define audErrCanNotSetRepeatNumber		(acerAudioErrorClass | 0x20)// repeat mode == repeat no, sleep mode == disable sleep

// 03/14/03 whitney chang End <<

//-----------------------------------------------------------------------------
// Audio library function trap ID's. Each library call gets a trap number:
//   AudioLibTrapXXXX which serves as an index into the library's dispatch table.
//   The constant sysLibTrapCustom is the first available trap number after
//   the system predefined library traps Open,Close,Sleep & Wake.
//
// WARNING!!! The order of these traps MUST match the order of the dispatch
//  table in AudioLibDispatch.c!!!
//-----------------------------------------------------------------------------

typedef enum {

//	audioLibTrapGetLibAPIVersion = sysLibTrapCustom,
	audioLibTrapSetEQ = sysLibTrapCustom,
	audioLibTrapGetFeatures,
	audioLibTrapGetVolume,
	audioLibTrapSetVolume,
	audioLibTrapGetPlayBufferLength,
	audioLibTrapPlayFormat,
	audioLibTrapPlayData,
	audioLibTrapPlay,
	audioLibTrapGetRecBufferLength,
	audioLibTrapRecordFormat,
	audioLibTrapRecordData,
	audioLibTrapRecord,
	audioLibTrapStop,
	audioLibTrapSetPause,
	audioLibTrapPlaySources,
	audioLibTrapPlayGetPlayInfo,
	audioLibTrapPlayGetParam,
	audioLibTrapPlaySetParam,
	audioLibTrapPlayQueryOffset,
	audioLibTrapPlaySetOffset,
	audioLibTrapPlayNext,
	audioLibTrapPlayPrevious,
	audioLibTrapPlayA2B,
	audioLibTrapPlayA2BStop,
	audioLibTrapPlayGetNextSongInfoInList,
	audioLibTrapPlayGetInfoInPlayingList,
	audioLibTrapRecordTarget,
	audioLibTrapRecordQueryRecordTargetInfo,
	audioLibTrapRecordQueryOprValue,
	audioLibTrapRecordSetOprValue,
	audioLibTrapRecordQuerySize,
	audioLibTrapDSPPowerSavingMode,
	audioLibTrapQueryInfo,
	audioLibTrapSetPossessor,
	audioLibTrapLowBatteryProc,
	audioLibTrapPrepareSleepProc,

// for v2.1, confirm Acer Audio Library minor version
// 03/14/03 whitney chang Add >>
	audioLibTrapVersionConfirm,
// 03/14/03 whitney chang End <<
	
	audioLibTrapLast			//for calculating the number of traps

} AudioLibTrapNumberEnum;


/*************************************************************
 * Audio library functions' parameter definitions            *
 *************************************************************/
#define AUDIO_MAX_PATH_LENGTH 256

/* old */
typedef enum {

	audFtrPlayMP3 = 0x0002,
	audFtrRecordMP3 = 0x0200

} AudioFeature;


/************************************************************************
 *                           play definitions                           *
 ************************************************************************/

/* new */
typedef enum {

	PlayEQNormal = 0x01,
	PlayEQRock = 0x02,
	PlayEQLatin = 0x03,
	PlayEQDance = 0x04,
	PlayEQJazz = 0x05,
	PlayEQDefault = PlayEQNormal

} AudioPlayEQModeEnum, AudioEQMode/* old and define anew */;


/* old and define anew */
typedef enum {

	audVolumeMin = 0x00,
	audVolumeDefault = 0xB9,// transferred value (ranges in 0x00~0xFF) of Dsp default volume (0x28)
	audVolumeMax = 0xFF		// transferred value (ranges in 0x00~0xFF) of Dsp maximum volume (0x37)

} AudioVolume;

/* old */
typedef enum {

	audFormatMP3 = 0x02

} AudioFormat;

/* new */
typedef enum {

	AudioPlayFormatMP3 = audFormatMP3

} AudioPlayFormatEnum;

/* new */
typedef enum {

	PlayRepeatNo,
	PlayRepeatOne,
	PlayRepeatAll

} AudioPlayRepeatModeEnum;

/* new */
typedef enum {

	PlaySleepCountTime,		// calculate the elapse time to determine when to sleep
	PlaySleepCountNumber,	// one audio data, file or pdb as one count
	PlaySleepDisable		// to forbid the sleep function

} AudioPlaySleepModeEnum;

/* new */
typedef enum {

	PlaySetOprVolumeValue,
	PlaySetOprEQMode,
	PlaySetOprShuffleMode,
	PlaySetOprRepeatMode,
	PlaySetOprSleepMode,
	PlaySetOprSleepValue,
	PlaySetOprLEDControl,  //** whether library control LED status while Dsp is playing

// for v2.1, set demand number of repeat for noe song and all songs in play list.
// 03/14/03 whitney chang Add >>
	PlaySetOprRepeatNumber
// 03/14/03 whitney chang End

} AudioPlaySetParamEnum;

/* new */
typedef enum {

	AudioPlayFromData,
	AudioPlayFromPdb,
	AudioPlayFromFile,
	AudioPlayFromList

} AudioPlayObjectEnum;

/* new */
typedef struct {
	UInt8	*dataP;
	UInt32	size;
	Boolean	makeCopy; 
	// If set makeCopy to true, the  audio library will allocate an non-movable memory to 
	// store this size of data for background playing.
	// If set makeCopy to false, the AP should stop playing before existing, 
	// otherwise it will  cause memory fetal error.
} AudioPlayDataType;

/* new */
typedef struct {
	Char	name[AUDIO_MAX_PATH_LENGTH];
	UInt16	cardNo;
} AudioPlayPdbType;

/* new */
typedef struct {
	Char	pathFile[AUDIO_MAX_PATH_LENGTH];
} AudioPlayFileType;


/* new, play list position, e.g. in internal memory, in memory stick card */
typedef enum {

	AudioPlayListInInternal,
	AudioPlayListInMSCard

} AudioPlayListPosEnum;


/* new */
typedef struct {
	Char	pathList[AUDIO_MAX_PATH_LENGTH];
	Int16	dbIndex; // which record to retrieve as first
	Char	pathFile[AUDIO_MAX_PATH_LENGTH];
	Boolean	restricted;
	// if restricted is true, library will play restrictedly according dbIndex and pathFile of pathList,
	// otherwise, library will play first file of pathList or continue current play
	Boolean changeList;
	// if changeList is true, library will fix current play file list and random list accordint to
	// new pathList; otherwise, library will only deal with restricted, dbIndex and pathFile parameters.
	AudioPlayListPosEnum pathListPos;
} AudioPlayListType;


/* new */
 typedef struct
 {
 	Char	FileName[AUDIO_MAX_PATH_LENGTH];
 	Boolean	Checked;
 	UInt32	reserved1;
 	UInt32	reserved2;
 } PlayListDBRecordType;

/* new */
typedef struct _PlayFileListType
 {
 	Int16	nId; // the continuous id in playFileList, i.e. 0, 1, 2, ...
 	Int16	nIndex; // nIndexth song of all songs (checked and not checked) in memory stick, it's not continuous
 	Char 	FileFullPath[AUDIO_MAX_PATH_LENGTH];
 	struct _PlayFileListType *Prev;
 	struct _PlayFileListType *Next;
 } PlayFileListType;

/* new */
typedef union {
	AudioPlayDataType	playDataInfo;
	AudioPlayPdbType	playPdbInfo;
	AudioPlayFileType	playFileInfo;
	AudioPlayListType	playListInfo;
} AudioPlayObjectType;

/* new */
typedef enum {

	AudioPlayNormal,
	AudioPlayA2B

} AudioPlayStateEnum;

/* new */
typedef struct {
	UInt32	offsetA;
} AudioPlayNormalType;

/* new */
typedef struct {
	UInt32	offsetA;
	UInt32	offsetB;
} AudioPlayA2BType;

/* new */
typedef union {
	AudioPlayNormalType	playNormalInfo;
	AudioPlayA2BType	playA2BInfo;
} AudioPlayStateType;

/* new */
typedef struct {
	AudioPlayObjectEnum	playObjectEnum;
	AudioPlayObjectType	playSrcInfo;
	AudioPlayStateEnum	playStateEnum;
	AudioPlayStateType	playStateInfo;
	AudioPlayFormatEnum	playFormat;
} AudioPlayInfo, *AudioPlayInfoPtr;

/* new */
typedef struct {
	UInt8					playOprVolumeValue; //** 0x00(min)~ 0xFF(max)
	UInt16					playBitRate; //** with K unit
	AudioPlayEQModeEnum		playOprEQMode;
	Boolean					playOprShuffleMode;
	AudioPlayRepeatModeEnum	playOprRepeatMode;
	AudioPlaySleepModeEnum	playOprSleepMode;
	UInt16					playOprSleepValue;
	Boolean					playOprLEDControl; //** whether library control LED status while Dsp is playing

// for v2.1, set demand number of repeat for noe song and all songs in play list.
// 03/14/03 whitney chang Add >>
	UInt16					playOprRepeatNumber;  //** Demand number of repeat
	UInt16					playOprRepeatCounter; //** Current repeated number
// 03/14/03 whitney chang End <<

} AudioPlayParamType, *AudioPlayParamPtr;

/**************************************************************************
 *                           Record definitions                           *
 **************************************************************************/

/* new */
typedef enum {

	AudioRecordFormatMP3 = audFormatMP3

} AudioRecordFormatType;

/* new */
typedef enum {

	RecordSampleRate_16K = 0x03	// 16K Hz, Dsp default

} AudioRecordSampleRateType, AudioQuality/* old and define anew */;

/* new */
typedef enum {

	RecordBitRate_128K = 0x02,	// 128K bps, Dsp default
	RecordBitRate_64K = 0x04	// 64K bps

} AudioRecordBitRateType, AudioRecordBitRate/* old and define anew */;


typedef enum {

	AudioRecordToData,
	AudioRecordToPdb,
	AudioRecordToFile

} AudioRecordTargetEnum;


typedef enum {

	RecordSetOprSamplingRate,
	RecordSetOprBitrate,
	RecordSetOprLEDControl, //** whether library control LED status while Dsp is recording
	RecordSetOprBoost

} AudioRecordSetOprType;


typedef struct {
	UInt8		*dataP;
	// Note that the recorder apps should not exit until the end of recording, otherwise it will
	// cause the memory fetal error if assign the dataP as recording buffer. That is, this target
	// format does NOT support background functionality. 
} AudioRecordDataType;


typedef struct {
	Char		nameP[AUDIO_MAX_PATH_LENGTH];
	UInt16		cardNo;
} AudioRecordPdbType;


typedef struct {
	Char		pathFileP[AUDIO_MAX_PATH_LENGTH];
} AudioRecordFileType;


typedef union {
	AudioRecordDataType	recordDataInfo;
	AudioRecordPdbType	recordPdbInfo;
	AudioRecordFileType	recordFileInfo;
} AudioRecordTargetInfoType, *AudioRecordTargetInfoPtr;


typedef struct {
	AudioRecordTargetEnum		recordSpecies;
	AudioRecordTargetInfoType	recordTargetInfo;
	AudioRecordFormatType		recordFormat;
} AudioRecordTargetType, *AudioRecordTargetPtr;


typedef struct {
	AudioRecordSampleRateType	recordSampleRate;
	AudioRecordBitRateType		recordBitRate;
	Boolean						recordLEDControl; //** whether library control LED status while Dsp is recording
	Boolean 					recordBoost;
} AudioRecordOprType, *AudioRecordOprPtr;


typedef enum {

	AudioOprDSPPwrOff, //** DSP has no power
	AudioOprDecoding, //** DSP is playing
	AudioOprEncoding, //** DSP is recording
	AudioOprPause, //** pause playing or recording
	AudioOprStop //** DSP is stoped playing or recording

} AudioCodingStateType;


typedef struct {
	AudioCodingStateType	codingState;	//** Dsp coding status, decoding, encoding, ..., etc.
	Char					appDBNameP[32]; //** application database name
} AudioLibInfoType, *AudioLibInfoPtr; 


/********************************************************************
 * API Prototypes
 ********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif


//--------------------------------------------------
// Standard library open, close, sleep and wake functions
//--------------------------------------------------

// 1. Open Audio library, creates and initializes the used globals.
extern Err AudioLibOpen(UInt16 refNum)	AUDIO_LIB_TRAP(sysLibTrapOpen);

// 2. Close Audio library, frees globals.
extern Err AudioLibClose(UInt16 refNum, UInt16 *numappsP)	AUDIO_LIB_TRAP(sysLibTrapClose);

// 3. Handles system sleep notification.
extern Err AudioLibSleep(UInt16 refNum)	AUDIO_LIB_TRAP(sysLibTrapSleep);

// 4. Handles system wake notification.
extern Err AudioLibWake(UInt16 refNum)	AUDIO_LIB_TRAP(sysLibTrapWake);


//--------------------------------------------------
// Custom library API functions
//--------------------------------------------------
	
// 5. Switch between several positions of the equalizer - Normal, Rock, Latin, Dance, Jazz.
extern Err AudioLibSetEQ(UInt16 refNum, UInt8 mode)
				AUDIO_LIB_TRAP(audioLibTrapSetEQ);
				
// 6. Find out what features are supported on DSP -- PlayMP3, RecordMP3.
extern Err AudioLibGetFeatures(UInt16 refNum, UInt16 *featureP)
				AUDIO_LIB_TRAP(audioLibTrapGetFeatures);
				
// 7. Gets the current volume level of the audio output device.
extern Err AudioLibGetVolume(UInt16 refNum, UInt8 *volumeP)
				AUDIO_LIB_TRAP(audioLibTrapGetVolume);

// 8. Sets the current volume level of the audio output device.
extern Err AudioLibSetVolume(UInt16 refNum, UInt8 volume)
				AUDIO_LIB_TRAP(audioLibTrapSetVolume);

// 9. Gets the size of DSP buffer in bytes unit for playing.
extern Err AudioLibGetPlayBufferLength(UInt16 refNum, UInt16 *lengthP)
				AUDIO_LIB_TRAP(audioLibTrapGetPlayBufferLength);

// 9.1. Gets the size of DSP buffer in bytes unit for playing.
extern Err AudioLibGetDSPPlayBufferLength(UInt16 refNum, UInt16 *lengthP)
				AUDIO_LIB_TRAP(audioLibTrapGetPlayBufferLength);

// 10. Defines the format of audio data for playing.
extern Err AudioLibPlayFormat(UInt16 refNum, AudioFormat format)
				AUDIO_LIB_TRAP(audioLibTrapPlayFormat);

// 10.1. Defines the format of audio data for playing.
extern Err AudioLibSetPlayFormat(UInt16 refNum, AudioFormat format)
				AUDIO_LIB_TRAP(audioLibTrapPlayFormat);

// 11. Feeds DSP buffer with each section of audio data for playing.
extern Err AudioLibPlayData(UInt16 refNum, UInt8 *AudioData, UInt16 length, Boolean new_song)
				AUDIO_LIB_TRAP(audioLibTrapPlayData);

// 11.1. Feeds DSP buffer with each section of audio data for playing.
extern Err AudioLibFeedPlayData(UInt16 refNum, UInt8 *AudioData, UInt16 length, Boolean new_song)
				AUDIO_LIB_TRAP(audioLibTrapPlayData);

// 12. Informs DSP the total length of the audio file and then start to playback.
extern Err AudioLibPlay(UInt16 refNum, UInt32 length)
				AUDIO_LIB_TRAP(audioLibTrapPlay);

// 12.1. Informs DSP the total length of the audio file and then start to playback.
extern Err AudioLibStartPlay(UInt16 refNum, UInt32 length)
				AUDIO_LIB_TRAP(audioLibTrapPlay);

// 13. Gets the size of DSP buffer for recording in bytes unit.
extern Err AudioLibGetRecBufferLength(UInt16 refNum, UInt16 *lengthP)
				AUDIO_LIB_TRAP(audioLibTrapGetRecBufferLength);

// 13.1. Gets the size of DSP buffer for recording in bytes unit.
extern Err AudioLibGetDSPRecordBufferLength(UInt16 refNum, UInt16 *lengthP)
				AUDIO_LIB_TRAP(audioLibTrapGetRecBufferLength);

// 14. Defines audio data format, sampling rate and bit rate for recording.
extern Err AudioLibRecordFormat(UInt16 refNum, AudioFormat format, AudioQuality quality, AudioRecordBitRate bitrate)
				AUDIO_LIB_TRAP(audioLibTrapRecordFormat);

// 14.1. Defines audio data format, sampling rate and bit rate for recording.
extern Err AudioLibSetRecordFormat(UInt16 refNum, AudioFormat format, AudioQuality quality, AudioRecordBitRate bitrate)
				AUDIO_LIB_TRAP(audioLibTrapRecordFormat);

// 15. Reads back recorded audio data from DSP buffer.
extern Err AudioLibRecordData(UInt16 refNum, UInt8 *AudioData, UInt16 *lengthP)
				AUDIO_LIB_TRAP(audioLibTrapRecordData);

// 15.1. Reads back recorded audio data from DSP buffer.
extern Err AudioLibReadRecordData(UInt16 refNum, UInt8 *AudioData, UInt16 *lengthP)
				AUDIO_LIB_TRAP(audioLibTrapRecordData);

// 16. Starts to record.
extern Err AudioLibRecord(UInt16 refNum)
				AUDIO_LIB_TRAP(audioLibTrapRecord);

// 16.1. Starts to record.
extern Err AudioLibStartRecord(UInt16 refNum)
				AUDIO_LIB_TRAP(audioLibTrapRecord);
				
// 17. Stops playing or recording.
extern Err AudioLibStop(UInt16 refNum)
				AUDIO_LIB_TRAP(audioLibTrapStop);

// 18.Pauses playing or recording.
extern Err AudioLibSetPause(UInt16 refNum, Boolean pause)
				AUDIO_LIB_TRAP(audioLibTrapSetPause);

// 19.Play one audio data, a pdb, a file or a list.
extern Err AudioLibPlaySources(UInt16 refNum, AudioPlayInfoPtr playSrcP, Boolean Playing)
				AUDIO_LIB_TRAP(audioLibTrapPlaySources);

// 20.Query current playing source information.
extern Err AudioLibPlayGetPlayInfo(UInt16 refNum, AudioPlayInfoPtr playInfoP)
				AUDIO_LIB_TRAP(audioLibTrapPlayGetPlayInfo);

// 21.Query current playing operators information.
extern Err AudioLibPlayGetParam(UInt16 refNum, AudioPlayParamPtr playParamP)
				AUDIO_LIB_TRAP(audioLibTrapPlayGetParam);

// 22.Set playing operators.
extern Err AudioLibPlaySetParam(UInt16 refNum, AudioPlaySetParamEnum playSetParam, void *setValueP, Boolean setDefault)
				AUDIO_LIB_TRAP(audioLibTrapPlaySetParam);

// 23.Query the offset of currently playing data.
extern Err AudioLibPlayQueryOffset(UInt16 refNum, UInt32 *playOffsetP)
				AUDIO_LIB_TRAP(audioLibTrapPlayQueryOffset);

// 24.Set the offset of playing or will-be played data.
extern Err AudioLibPlaySetOffset(UInt16 refNum, UInt32 playOffset, Boolean Playing)
				AUDIO_LIB_TRAP(audioLibTrapPlaySetOffset);

// 25.Play next audio data.
extern Err AudioLibPlayNext(UInt16 refNum, Boolean Playing)
				AUDIO_LIB_TRAP(audioLibTrapPlayNext);

// 26.Play previous audio data.
extern Err AudioLibPlayPrevious(UInt16 refNum, Boolean Playing)
				AUDIO_LIB_TRAP(audioLibTrapPlayPrevious);

// 27.Play audio data between playOffsetA and playOffsetB of current playing data source.
extern Err AudioLibPlayA2B(UInt16 refNum, UInt32 playOffsetA, UInt32 playOffsetB)
				AUDIO_LIB_TRAP(audioLibTrapPlayA2B);

// 28.Stop playing A->B state.
extern Err AudioLibPlayA2BStop(UInt16 refNum)
				AUDIO_LIB_TRAP(audioLibTrapPlayA2BStop);

// 29.Query next oncoming played song information.
extern Err AudioLibPlayGetNextSongInfoInList(UInt16 refNum, Int16 *index, Char *pathFile)
				AUDIO_LIB_TRAP(audioLibTrapPlayGetNextSongInfoInList);

// 30.Query index and pathFile information in current playing list
extern Err AudioLibPlayGetInfoInPlayingList(UInt16 refNum, Boolean getIndexInfo, void *setValueP, void *getValueP)
				AUDIO_LIB_TRAP(audioLibTrapPlayGetInfoInPlayingList);

// 31.Record an audio to data, a file, or a pdb.
extern Err AudioLibRecordTarget(UInt16 refNum, AudioRecordTargetPtr recordTargetP)
				AUDIO_LIB_TRAP(audioLibTrapRecordTarget);

// 32.Query current recording information.
extern Err AudioLibRecordQueryRecordTargetInfo(UInt16 refNum, AudioRecordTargetPtr targetInfoP)
				AUDIO_LIB_TRAP(audioLibTrapRecordQueryRecordTargetInfo);

// 33.Query current recording operators information.
extern Err AudioLibRecordQueryOprValue(UInt16 refNum, AudioRecordOprPtr recordOprP)
				AUDIO_LIB_TRAP(audioLibTrapRecordQueryOprValue);

// 34.Set recording operators.
extern Err AudioLibRecordSetOprValue(UInt16 refNum, AudioRecordSetOprType recordSetOpr, void *setValueP, Boolean setDefault)
				AUDIO_LIB_TRAP(audioLibTrapRecordSetOprValue);

// 35.Query the size of recorded data.
extern Err AudioLibRecordQuerySize(UInt16 refNum, UInt32 *recordSizeP)
				AUDIO_LIB_TRAP(audioLibTrapRecordQuerySize);

// 36.Power off Dsp.
extern Err AudioLibDSPPowerSavingMode(UInt16 refNum)
				AUDIO_LIB_TRAP(audioLibTrapDSPPowerSavingMode);

// 37.Query audio library version, ..., etc.
extern Err AudioLibQueryInfo(UInt16 refNum, AudioLibInfoPtr infoP)
				AUDIO_LIB_TRAP(audioLibTrapQueryInfo);

// 38.Set which application possesses background play/record.
extern Err AudioLibSetPossessor(UInt16 refNum)
				AUDIO_LIB_TRAP(audioLibTrapSetPossessor);

// 39.Deal properly with background play/record while meeting low battery.
extern Err AudioLibLowBatteryProc(UInt16 refNum)
				AUDIO_LIB_TRAP(audioLibTrapLowBatteryProc);

// 40.Deal properly with Turn Off & Lock Device.
extern Err AudioLibPrepareSleepProc(UInt16 refNum)
				AUDIO_LIB_TRAP(audioLibTrapPrepareSleepProc);

// for v2.1, confirm Acer Audio Library minor version
// 03/14/03 whitney chang Add >>

// 41.Feedback version string to audio library to enable new functions in Acer Audio API v2.1 or later.
extern Err AudioLibVersionConfirm(UInt16 refNum, Char* strVersion)
				AUDIO_LIB_TRAP(audioLibTrapVersionConfirm);
// 03/14/03 whitney chang End <<

// For loading the library in Palm OS Mac emulation mode
extern Err AudioLibInstall(UInt16 refNum, SysLibTblEntryPtr entryP);

#ifdef __cplusplus 
}
#endif


#endif	// __ACERAUDIOLIB_H__
