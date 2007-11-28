/******************************************************************************
 * Copyright (c) 2004-2005 Palm, Inc. or its subsidiaries.
 * All rights reserved.
 *****************************************************************************/
/**
 * @ingroup Locs
 */

/**
 *
 * @file	LocsLib.h
 * @brief	Public 68K include file for location/timezone/DST support on Palm devices.
 *
 */


#ifndef _LOCSLIB_H_
#define _LOCSLIB_H_

// PalmSource includes:
#include <DataMgr.h>
#include <LibTraps.h>
#include <LocaleMgr.h>
#include <PalmTypes.h>
#include <SystemResources.h>
#include <SysUtils.h>
#include <Common/Libraries/locs/LocsLibCommon.h>


/******************************************************************************
 *
 *  PROTOTYPES
 *
 *****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif


// Required entry points:

/// Standard library open routine.
///
/// @param iLibRefNum:	IN: Library reference number.
/// @return Error code.
extern Err               LocsLibOpen                   (UInt16 iLibRefNum)
    SYS_TRAP (sysLibTrapOpen);

/// Standard library close routine.
///
/// @param iLibRefNum:	IN: Library reference number.
/// @return Error code.
extern Err               LocsLibClose                  (UInt16 iLibRefNum)
    SYS_TRAP (sysLibTrapClose);

/// Standard library sleep routine.
///
/// @param iLibRefNum:	IN: Library reference number.
/// @return Error code.
extern Err               LocsLibSleep                  (UInt16 iLibRefNum)
    SYS_TRAP (sysLibTrapSleep);

/// Standard library wake routine.
///
/// @param iLibRefNum:	IN: Library reference number.
/// @return Error code.
extern Err               LocsLibWake	               (UInt16 iLibRefNum)
    SYS_TRAP (sysLibTrapWake);

// LocsLib general public custom functions (library extends to callers):

/// Get the version of the library on the device.
///
/// @param iLibRefNum:	IN: Library reference number.
/// @param iSDKVersion:	IN: Currently used library version in the SDK
/// @param oVersionP:		OUT: Version of the library on the device.
/// @return Error code.
extern Err               LocsLibGetVersion             (UInt16 iLibRefNum, UInt32 iSDKVersion, UInt32* oVersionP)
    SYS_TRAP (locsLibTrapGetVersion);

/// Runs the event loop for handling the Locations dialogues.
/// Note: the preSelect, noEditOnSelect, and sendChangeNotify
/// values of the caller data block don't apply for this API.
///
/// @param iLibRefNum:	IN: Library reference number.
/// @param ioSelLocsP:	I/O: Pointer to a selected locations object 
///                          that contains data on selected locations.
///                          e.g.: The Clock 2.0 app displays
///                          several selected locations and the
///                          associated times.  If any of those
///                          locations are removed or renamed, the
///                          Clock 2.0 app needs to know so it can
///                          update the display.
///                          This object is updated if any of the
///                          selected locations are renamed or removed.
///                          Use the dirty bits to determine quickly
///                          if any data were changed and if so, which.
///                          Pass NULL to ignore.
/// @return True if changes are made.
extern Boolean           LocsLibLaunchEditListDialogue (UInt16 iLibRefNum, LocsLibSelLocsType* ioSelLocsP)
    SYS_TRAP (locsLibTrapLaunchEditListDlg);
    
/// Run the location selection dialogue, and then the 
/// Edit Location dialogue if the user tapped [OK] in
/// the location selection dialogue and if the caller
/// didn't specify to skip that dialogue.
///
/// @param iLibRefNum:	IN: Library reference number.
/// @param ioSelLocsP:	I/O: Pointer to a selected locations object 
///                          that contains data on selected locations.
///                          e.g.: The Clock 2.0 app displays
///                          several selected locations and the
///                          associated times.  If any of those
///                          locations are removed or renamed, the
///                          Clock 2.0 app needs to know so it can
///                          update the display.
///                          This object is updated if any of the
///                          selected locations are renamed or removed.
///                          Use the dirty bits to determine quickly
///                          if any data were changed and if so, which.
///                          Must not be NULL.
/// @return True if changes are made.
extern Boolean           LocsLibLaunchSelDialogue      (UInt16 iLibRefNum, LocsLibSelLocsType* ioSelLocsP)
    SYS_TRAP (locsLibTrapLaunchSelDialogue);

// Location public custom functions:

/// Copy from one location data into another
///
/// @param iLibRefNum:		 IN:  Library reference number
/// @param thisP:					 IN:  Pointer to the location object to copy to
/// @param iLocToCopyP		 IN:  Pointer to the location to be copied
extern void              LocationCopy                  (UInt16 iLibRefNum, LocationType* thisP, const LocationType* iLocToCopyP)
    SYS_TRAP (locsLibTrapLocCopy);

/// Destructor for location objects
///
/// @param iLibRefNum:			IN: Library reference number
/// @param thisP:						IN: Pointer to the location object to destroy
extern void              LocationFinal                 (UInt16 iLibRefNum, LocationType* thisP)
    SYS_TRAP (locsLibTrapLocFinal);

/// Calculate the size in bytes of the location object with the name field size
/// included, but without the size of the pointer to the name field.
///
/// @param iLibRefNum:		IN: Library reference number
/// @param thisP:					IN: Pointer to the location object to size
/// @return Size in bytes
extern UInt16            LocationGetSize               (UInt16 iLibRefNum, const LocationType* thisP)
    SYS_TRAP (locsLibTrapLocGetSize);

/// Constructor for location objects; initialize the given location data
/// structure.
///
/// @param iLibRefNum:			 IN:  Library reference number
/// @param thisP:						 IN:  Pointer to the location object to initialize
extern void              LocationInit                  (UInt16 iLibRefNum, LocationType* thisP)
    SYS_TRAP (locsLibTrapLocInit);

/// Determine if a given DST date is in the past relative to the current time
///
/// @param iLibRefNum:	 IN:  Library reference number
/// @param iDstDateP:		 IN:  Pointer to a DST date to use for determining if that
///				 							 			date is in the past relative to the current time.
/// @return True if given DST date is in the past.
extern Boolean           LocationIsDSTPast             (UInt16 iLibRefNum, DSTType* iDstDateP)
    SYS_TRAP (locsLibTrapLocIsDSTPast);

/// Create a generic location using the given country and UTC values.
///
/// @param iLibRefNum:				IN:  Library reference number
/// @param thisP:							IN:  Pointer to the location object to initialize
///				 												 with generic location data
/// @param iCountryCode:			IN:  Country code for generic location
/// @param iUTC:							IN:  UTC (timezone is minutes east west of prime
///				 												 meridian) for generic location; also used to
///																 create the location name.
/// @return errNone if success.
extern Err               LocationMakeGeneric           (UInt16 iLibRefNum, LocationType* thisP, CountryType iCountryCode, Int16 iUTC)
    SYS_TRAP (locsLibTrapLocMakeGeneric);

/// Mutator for changing location name value
///
/// @param iLibRefNum:	 IN:  Library reference number
/// @param thisP:				 IN:  Pointer to the location object to change
/// @param iNameP:			 IN:  Pointer to the new name
extern void              LocationSetName               (UInt16 iLibRefNum, LocationType* thisP, const Char* iNameP)
    SYS_TRAP (locsLibTrapLocSetName);

// LocationsDB public custom functions:

/// Modify a record and re-sort the db if needed.
///
/// @param iLibRefNum:	IN:  Library reference number
/// @param dbP:					IN:  Pointer to the given location database 
///				 									 to be modified.
/// @param ioRecordIndexP:	 I/O: Pointer to the record index of the location
///                               record to be modified. If the record is
///                               moved, this value is updated with the 
///                               new index.
/// @param iLocationP:	IN:  Pointer to the new location data that is
///                          used to update the record.
///
/// @return errNone if success.
extern Err               LocDBChangeRecord          (UInt16 iLibRefNum, DmOpenRef dbP, UInt16* ioRecordIndexP, const LocationType* iLocationP)
    SYS_TRAP (locsLibTrapDBChangeRecord);
    
/// Comparison function to be used with system sort routines.
///
/// Note: the system sort functions, like SysQSort, treat the
/// elements of the passed list as is; i.e.: instead of
/// passing the elements to this function, pointers to
/// the elements are passed.  Thus, it is necessary to
/// here to use double pointers and dereferencing to
/// to compare the actual records.
///
/// @param iLibRefNum:		IN:  Library reference number
/// @param iRecord01P:		IN:  Pointer to the first location record.
/// @param iRecord02P:		IN:  Pointer to the second location record.
/// @param iFlags					IN:  Sort order.
///
/// @return -n if 1st is lesser, +n if 2nd is lesser, 0 if the two are the same,
///         except for DST data, which isn't compared.
extern Int16             LocDBCompareRecordsSimple  (UInt16 iLibRefNum, LocDBRecordType** iRecord01P, LocDBRecordType** iRecord02P, Int32 iFlags)
    SYS_TRAP (locsLibTrapDBCmpRecordsSimple);
    
/// This function will format and copy locked record (LocationType) data into another 
/// locationType 
///
/// @param iLibRefNum: 			IN:  library reference number
/// @param ioLocationP:			I/O: Pointer to the location object that is
///                              filled with the converted record data.
///	@param iRecordP:				IN:  Pointer to the record whose data is
///                              converted to the location object format.
extern void              LocDBConvertRecord         (UInt16 iLibRefNum, LocationType* ioLocationP, const LocDBRecordType* iRecordP)
    SYS_TRAP (locsLibTrapDBConvertRecord);

/// Copy the data in a given location object into a given
/// location db record format. Note: the caller must allocate enough memory to the 
/// record object to hold the location data!
///
/// @param iLibRefNum:		IN:  Library reference number
/// @param ioRecordP:			I/O: Pointer to a record that is filled with
///                            the location object data.
/// @param iLocationP:		IN:  Pointer to the location object whose data
///                            is copied to the record.
extern void              LocDBCopyLocation          (UInt16 iLibRefNum, LocDBRecordType* ioRecordP, const LocationType* iLocationP)
    SYS_TRAP (locsLibTrapDBCopyLocation);
    
/// Deletes a record from the cities database.
///
/// @param iLibRefNum:		IN:  Library reference number
/// @param dbP:			 			IN:  Pointer to the database containing the record to delete.
/// @param iRecordIndex:	IN: Index of the record to delete.
///
/// @return errNone if success.
extern Err               LocDBDeleteRecord          (UInt16 iLibRefNum, DmOpenRef dbP, UInt16 iRecordIndex)
    SYS_TRAP (locsLibTrapDBDeleteRecord);
    
/// Finds a record having the custom bit set.
///
/// @param iLibRefNum:		IN:  Library reference number
/// @param iDbP:					IN:  Pointer to a locations record database.
/// @param ioRecordIndex:	I/O: Index of the record to begin searching from,
///                            and index of the found record.
/// @param oRecordH:			OUT: Pointer to the record handle; set by this
///                            function.
///
/// @return Pointer to the record found or NULL. Caller is responsible
///         for freeing record when done.
extern LocDBRecordType*  LocDBFindCustomRecord      (UInt16 iLibRefNum, DmOpenRef iDbP, UInt16* ioRecordIndex, MemHandle* oRecordH)
    SYS_TRAP (locsLibTrapDBFindCustomRecord);

/// Returns the record index for the first record matching
/// the given location data.
///
/// @param iLibRefNum:		IN:  Library reference number
/// @param dbP:						IN:  Pointer to the given cities database 
///                            to search for a match.
/// @param oRecordIndexP:	OUT: Pointer to record index var to store the
///                            record index of the matching record found
///                            in the search. Set to dmMaxRecordIndex if
///                            no match is found.
/// @param iMatchLocationP: IN: Pointer to the location data for which
///                             to find the first matching record in 
///                             the given database.
///
/// @return Error code for a failure, or errNone if success.  If no match
///         is found, the index value is set to the value dmMaxRecordIndex.
extern Err               LocDBFindFirst             (UInt16 iLibRefNum, DmOpenRef dbP, UInt16* oRecordIndexP, LocDBRecordType* iMatchValueP)
    SYS_TRAP (locsLibTrapDBFindFirst);
    
/// Sequential search of the locations database records for a
/// match to the given city name.
///
/// @param iLibRefNum:			 IN:  Library reference number
/// @param dbP:							 IN:  Pointer to the record db to search.
/// @param oRecordIndexP:		 OUT: Pointer to an index var to set with the
///                               index of the matching record, or dmMaxRecordIndex
///                               if no match.
/// @param iNameP:					 IN:  Pointer to the name string for which to find a match.
///
/// @return errNone if no problems.  No match indicated by oRecordIndexP set to dmMaxRecordIndex.
extern Err               LocDBFindFirstByName       (UInt16 iLibRefNum, DmOpenRef dbP, UInt16* oRecordIndexP, const Char* iNameP)
    SYS_TRAP (locsLibTrapDBFindFirstByName);
    
/// Sequential search of the locations database records for
/// the first location matching the given country and UTC values.
///
/// @param iLibRefNum:			 IN:  Library reference number
/// @param dbP:							 IN:  Pointer to the record db to search.
/// @param oRecordIndexP:		 OUT: Pointer to an index var to set with the
///                               index of the matching record, or dmMaxRecordIndex
///                               if no match.
/// @param iCountry:				 IN:  The country code for which to find a match.
/// @param iUTC:						 IN:  The UTC for which to find a match.
///
/// @return errNone if no problems.  No match indicated by oRecordIndexP set to dmMaxRecordIndex.
extern Err               LocDBFindFirstByUTCCountry (UInt16 iLibRefNum, DmOpenRef dbP, UInt16* oRecordIndexP, CountryType iCountry, Int16 iUTC)
    SYS_TRAP (locsLibTrapDBFindFirstByUTCCtry);
    
/// Returns the record index for the first record in the given
/// list matching the given location data.
///
/// Note: This complements the get list functions, which return
/// a list of pointers to location records in a db.
///
/// @param iLibRefNum:						 IN:  Library reference number
/// @param iLocsListItemsCount: 	 IN:  Number of record items in the list.
/// @param iLocsListP:					 	 IN:  Pointer to the list of location records.
/// @param iMatchLocationP:		 		 IN:  Pointer to the location data for which
/// 			                              to find the first matching record in 
///                                     the given list.
///
/// @return The index of the list item matching the given location, or
///         if no match found, then dmMaxRecordIndex.
extern UInt16            LocDBFindFirstInList       (UInt16 iLibRefNum, UInt16 iLocsListItemsCount, LocDBRecordType** iLocsListP, const LocationType* iMatchLocationP)
    SYS_TRAP (locsLibTrapDBFindFirstInList);

/// Returns a handle to a list of pointers to location records
/// having the custom bit set. It is the responsibility of the caller to free the
/// memory for the returned list.
///
/// @param iLibRefNum:	IN:  Library reference number
/// @param iDbP:				IN:  Pointer to a locations record database.
/// @param oListCount:	OUT: Number of records found with custom bit set.
///
/// @return Handle to the list of location record pointers or NULL.
///         Note: Caller is repsonsible for freeing memory associated
///         with the list of pointers to records.
extern MemHandle         LocDBGetCustomList         (UInt16 iLibRefNum, DmOpenRef iDbP, UInt16* oListCount)
    SYS_TRAP (locsLibTrapDBGetCustomList);
    
/// Get an array of pointers to those records from the the given database.
///
/// @param iLibRefNum:	IN:  Library reference number
/// @param iDbP:				IN:  Pointer to an open locations database.
/// @param oListCount:	OUT: Pointer to the list count to be set with
///                          the number of items in the locations list.
/// @param iCompareF:		IN:	 Function pointer for the function to use
///                          when sorting the list. Pass NULL if the
///                          list should not be sorted.
/// @param iCompareFlags:	IN:  Any flags for comparison/sort behavior.
///
/// @return Pointer to the array of location data, or NULL.
///         Note: Caller is responsible for freeing this memory.
extern LocDBRecordType** LocDBGetRecordList         (UInt16 iLibRefNum, DmOpenRef iDbP, UInt16* oListCount, CmpFuncPtr iCompareF, Int32 iCompareFlags)
    SYS_TRAP (locsLibTrapDBGetRecordList);
    
/// Returns a pointer to a locked record chunk in the given db.
/// It is the responsibility of the caller to unlock and release the record
/// when done using the record. It is must to call LocDBCovertRecordV20 to get
///	formated data in another LocationType variable.
///
/// @param iLibRefNum: 			IN:  library reference number
/// @param dbP:							IN:  Pointer to a database having the desired record.
/// @param iRecordIndex:		IN:  Index of the record in the given db.
/// @param oRecordHP:				OUT: Pointer to the record handle; set by this function.
///
/// @return Pointer to the locked record or NULL; if non-NULL, caller
///         is responsible for unlocking and releasing the record.
extern LocDBRecordType*  LocDBGetRecord             (UInt16 iLibRefNum, DmOpenRef dbP, UInt16 iRecordIndex, MemHandle* oRecordHP)
    SYS_TRAP (locsLibTrapDBGetRecord);
    
/// Open the default db and open and initialize the custom db.
///
/// @param iLibRefNum: 			IN:  library reference number
/// @param oDefDbPP:				OUT: Pointer to the default db pointer to initialize.
/// @param oCusDbPP:				OUT: Pointer to the custom db pointer to initialize.
///
/// @return On success the default and custom db pointers are initialized.
extern Err               LocDBInitDBs               (UInt16 iLibRefNum, DmOpenRef* oDefDbPP, DmOpenRef* oCusDbPP)
    SYS_TRAP (locsLibTrapDBInitDBs);
    
/// Generates a new record for the given database.
///
/// @param iLibRefNum: 			IN:  library reference number
/// @param dbP:							IN:  Pointer to the given location database 
///                              into which the location data is to be
///                              inserted as a new record.
/// @param oRecordIndexP:		OUT: Pointer to the given record index value
///                              to be initialized with the new location
///                              record's index.
/// @param iLocationP:			IN:  Pointer to the given location data from
///                              which to generate the new location record.
///
/// @return errNone if success.
extern Err               LocDBNewRecord             (UInt16 iLibRefNum, DmOpenRef dbP, UInt16* oRecordIndexP, const LocationType* iLocationP)
    SYS_TRAP (locsLibTrapDBNewRecord);
    
///	Open the custom db and return the dbID and an initialized db pointer.
///
/// @param iLibRefNum: 			IN:  library reference number
/// @param iDbInfoP:				IN:  Pointer to a database info struct having db
///                              name, crid, type, and card number.
/// @param iOpenMode:				IN:  Read and write mode to open the database for.
/// @param oDbPP:						OUT: Pointer to a db reference that is set by this
///                              function.
/// @param iDefDbP:					IN:  Pointer to the default db from which to init
///                              the custom db if necessary.  Initialization
///                              occurs if the custom db must be created, and
///                              then any records marked as custom in the given
///                              default db are copied into the custom db.
///
/// @return Zero (0) on failure the dbP is set to NULL; the database ID
///         is returned on success and the dbP is set to the address
///         of the database.
extern LocalID           LocDBOpenCustom            (UInt16 iLibRefNum, const LocDBInfoType* iDbInfoP, UInt16 iOpenMode, DmOpenRef* oDbPP, const DmOpenRef iDefDbP)
    SYS_TRAP (locsLibTrapDBOpenCustom);
    
///	Open the default db and return the dbID and an initialized db pointer.
///
/// @param iLibRefNum: 			IN:  library reference number
/// @param oDbPP:						OUT: Pointer to a db reference that is set by this
///                              function.
///
/// @return Error code (errNone if successful).  Zero (0) on failure 
///         and the dbP is set to NULL; occurs if the default db isn't
///         found and the install from ROM fails.  The database ID is
///         returned on success and the dbP is set to the address
///         of the database.
extern LocalID           LocDBOpenDefault           (UInt16 iLibRefNum, DmOpenRef* oDbPP)
    SYS_TRAP (locsLibTrapDBOpenDefault);
    
/// Determines the size in bytes of a record.
///
/// @param iLibRefNum: 			IN:  library reference number
/// @param iLocRecordP:			IN:  Pointer to the record for which to 
///                              determine the size.
///
/// @return Size of the given record in bytes.
extern UInt16            LocDBRecordSize            (UInt16 iLibRefNum, const LocDBRecordType* iLocRecordP)
    SYS_TRAP (locsLibTrapDBRecordSize);
    
/// Set the custom field of the record.
///
/// @param iLibRefNum: 			IN:  library reference number
/// @param ioLocRecordP:		I/O: Pointer to the record for which to set the
///                              custom field.
/// @param iValue:					IN:  Value to which the custom field is set.
///
/// @return errNone if success.
extern Err               LocDBSetRecordCustom       (UInt16 iLibRefNum, LocDBRecordType* ioLocRecordP, UInt8 iValue)
    SYS_TRAP (locsLibTrapDBSetRecordCustom);
    
/// Sorts the given location data database.
///
/// @param iLibRefNum: 			IN:  library reference number
/// @param iDbP:						IN:  Pointer to the location db to sort.
/// @param iSortOrder:			IN:  Sort order desired.
///
/// @return errNone if success.
extern Err               LocDBSort                  (UInt16 iLibRefNum, DmOpenRef iDbP, LocDBSortOrderType iSortOrder)
    SYS_TRAP (locsLibTrapDBSort);

// LocsLibDSTAlarm public functions:

/// Clear an existing DST alert alarm from the system.
///
/// @param iLibRefNum: 			IN:  library reference number
extern void              LocsLibDSTAlarmClear       (UInt16 iLibRefNum)
    SYS_TRAP (locsLibTrapDSTAlarmClear);
    
/// Check if the current time is within a daylight saving time range.
///
/// @param iLibRefNum: 			IN:  library reference number
/// @param iDSTStartP:			IN:  Pointer to the start DST date.
/// @param iDSTEndP:				IN:  Pointer to the end DST date.
/// @param iTimeInSecs:			IN:	 The time in seconds against which to check
///				 											 the given DST date range.
///
/// @return True if in the DST range.
extern Boolean           LocsLibDSTAlarmIsDST       (UInt16 iLibRefNum, const DSTType* iDSTStartP, const DSTType* iDSTEndP, UInt32 iTimeInSecs)
    SYS_TRAP (locsLibTrapDSTAlarmIsDST);
    
/// Set the DST alert alarm to the given DST date.
///
/// @param iLibRefNum: 			IN:  library reference number
/// @param iDstP:						IN:  Pointer to the DST date data.
///
/// @return Time in seconds for which the DST alert was set, or
///         0 if not set (e.g.: if the given DST date data indicates DST is not observed).
extern UInt32            LocsLibDSTAlarmSet         (UInt16 iLibRefNum, const DSTType* iDstStartP, const DSTType* iDstEndP)
    SYS_TRAP (locsLibTrapDSTAlarmSet);

/// Generates a new record for the given database.
///
/// @param iLibRefNum: 			IN:  library reference number
/// @param dbP:							IN:  Pointer to the given location database 
///                              into which the location data is to be
///                              inserted as a new record.
/// @param oRecordIndexP:		OUT: Pointer to the given record index value
///                              to be initialized with the new location
///                              record's index.
/// @param iLocationP:			IN:  Pointer to the given location data from
///                              which to generate the new location record.
///
/// @return errNone if success.
extern Err			      LocDBNewRecordV20         (UInt16 iLibRefNum, DmOpenRef dbP, UInt16* oRecordIndexP, LocationType* iLocationP)
    SYS_TRAP (locsLibTrapLocDBNewRecordV20);
    
/// Returns a pointer to a locked record chunk in the given db.
/// It is the responsibility of the caller to unlock and release the record
/// when done using the record. It is must to call LocDBCovertRecordV20 to get
///	formated data in another LocationType variable.
///
/// @param iLibRefNum: 			IN:  library reference number
/// @param dbP:							IN:  Pointer to a database having the desired record.
/// @param iRecordIndex:		IN:  Index of the record in the given db.
/// @param oRecordHP:				OUT: Pointer to the record handle; set by this function.
///
/// @return Pointer to the locked record or NULL; if non-NULL, caller
///         is responsible for unlocking and releasing the record.
extern LocationType*  LocDBGetRecordV20       		(UInt16 iLibRefNum, DmOpenRef dbP, UInt16 iRecordIndex, MemHandle* oRecordHP)
    SYS_TRAP (locsLibTrapLocDBGetRecordV20);
    
/// This function will format and copy locked record (LocationType) data into another 
/// locationType 
///
/// @param iLibRefNum: 			IN:  library reference number
/// @param destRecordP:			IN:  Pointer to the location object that is
///                              filled with the converted record data.
///	@param srcRecordP:			IN:  Pointer to the record whose data is
///                              converted to the location object format.
extern void              LocDBConvertRecordV20      (UInt16 iLibRefNum, LocationType* destRecordP, const LocationType* srcRecordP)
    SYS_TRAP (locsLibTrapLocDBConvertRecordV20);

/// Modify a record and re-sort the db if needed.
///
/// @param iLibRefNum:	IN:  Library reference number
/// @param dbP:					IN:  Pointer to the given location database 
///				 									 to be modified.
/// @param ioRecordIndexP:	 I/O: Pointer to the record index of the location
///                               record to be modified. If the record is
///                               moved, this value is updated with the 
///                               new index.
/// @param iLocationP:	IN:  Pointer to the new location data that is
///                          used to update the record.
///
/// @return errNone if success.
extern Err               LocDBChangeRecordV20       (UInt16 iLibRefNum, DmOpenRef dbP, UInt16* ioRecordIndexP, LocationType* iLocationP)
    SYS_TRAP (locsLibTrapLocDBChangeRecordV20);
    
/// Get an array of pointers to those records from the the given database.
///
/// @param iLibRefNum:	IN:  Library reference number
/// @param iDbP:				IN:  Pointer to an open locations database.
/// @param oListCount:	OUT: Pointer to the list count to be set with
///                          the number of items in the locations list.
/// @param iCompareF:		IN:	 Function pointer for the function to use
///                          when sorting the list. Pass NULL if the
///                          list should not be sorted.
/// @param iCompareFlags:	IN:  Any flags for comparison/sort behavior.
///
/// @return Pointer to the array of location data, or NULL.
///         Note: Caller is responsible for freeing this memory.
extern LocationType** LocDBGetRecordListV20   		(UInt16 iLibRefNum, DmOpenRef iDbP, UInt16* oListCount, CmpFuncPtr iCompareF, Int32 iCompareFlags)
    SYS_TRAP (locsLibTrapLocDBGetRecordListV20);

/// Get a valid location from the locations DB nearest to the 
/// location specified
///
/// @param iLibRefNum:	 IN:  Library reference number
/// @param iDefDb:			 IN:  Reference to default location database  
/// @param iCustDb:			 IN:  Reference to custom location database  
/// @param iPosP:				 IN:  Pointer to reference position
/// @param oLocP:				 OUT: Pointer to location, nearest city will 
///				 							 			return in this pointer
/// @return Error if database not found.
extern Err 			LocDBGetNearestLocation			(UInt16 iLibRefNum,DmOpenRef iDb1P,DmOpenRef iDb2P,PosType* iPosP,LocationType* oLocP)
    SYS_TRAP (locsLibTrapLocDBGetNearestLocation);

/// Constructor for location objects v 2.0 (support for note); initialize the 
/// given location data structure.
///
/// @param iLibRefNum:			 IN:  Library reference number
/// @param thisP:						 IN:  Pointer to the location object to initialize
extern void              LocationInitV20            (UInt16 iLibRefNum, LocationType* thisP)
    SYS_TRAP (locsLibTrapLocationInitV20);
    
/// Destructor for location objects v 2.0 (support for note)
///
/// @param iLibRefNum:			IN: Library reference number
/// @param thisP:						IN: Pointer to the location object to destroy 
extern void              LocationFinalV20           (UInt16 iLibRefNum, LocationType* thisP)
    SYS_TRAP (locsLibTrapLocationFinalV20);
    
/// Get version of database
///
/// @param iLibRefNum:	 IN:  Library reference number
/// @param cardNo:			 IN:  cardNo for database
/// @param dbNameP:			 IN:  Pointer to database name to search
/// @param versionP:		 OUT: Pointer to version, version no will return
///      	 							 		  in this variable								
///
/// @return Error if database not found.
extern Err               LocDBGetVersion            (UInt16 iLibRefNum, UInt16 cardNo, const Char *dbNameP, UInt16 *version)
    SYS_TRAP (locsLibTrapLocDBGetVersion);
    
/// Copy note to location note field
///
/// @param iLibRefNum:	 IN:  Library reference number
/// @param thisP:				 IN:  Pointer to the location object to change.
/// @param iNoteP:			 IN:  Pointer to the new note.
extern void              LocationSetNote            (UInt16 iLibRefNum, LocationType* thisP, const Char* iNameP)
    SYS_TRAP (locsLibTrapLocationSetNote);
    
/// Initialize position values of location
///
/// @param iLibRefNum:	 IN:  Library reference number
/// @param thisP:				 IN:  Pointer to the city object to initialize.
/// @param iPosP:				 IN:  Pointer to PosType to set values of location
extern void              LocationSetPosition        (UInt16 iLibRefNum, LocationType* thisP, const PosType* iPosP)
    SYS_TRAP (locsLibTrapLocationSetPosition);


extern Boolean		         LocsLibLocationNote        (UInt16 iLibRefNum, const Char* iNameP, Boolean noteStatus)
    SYS_TRAP (locsLibTrapLocsLibLocationNote);
    
/// Calculate the size in bytes of the location object with the name and note field size
/// included, but without the size of the pointer to the name and note field.
///
/// @param iLibRefNum:		IN: Library reference number
/// @param locationP:					IN: Pointer to the location object to size
/// @return Size in bytes
extern UInt16           LocationGetSizeV20          (UInt16  iLibRefNum,  LocationType* locationP)
    SYS_TRAP (locsLibTrapLocationGetSizeV20);
    
// Location public custom functions:

/// Copy from one location data into another
///
/// @param iLibRefNum:		 IN:  Library reference number
/// @param thisP:					 IN:  Pointer to the location object to copy to
/// @param iLocToCopyP		 IN:  Pointer to the location to be copied
extern void              LocationCopyV20            (UInt16 iLibRefNum, LocationType* thisP, const LocationType* iLocToCopyP)
    SYS_TRAP (locsLibTrapLocationCopyV20);
    
/// Finds a record having the custom bit set.
///
/// @param iLibRefNum:		IN:  Library reference number
/// @param iDbP:					IN:  Pointer to a locations record database.
/// @param ioRecordIndex:	I/O: Index of the record to begin searching from,
///                            and index of the found record.
/// @param oRecordH:			OUT: Pointer to the record handle; set by this
///                            function.
///
/// @return Pointer to the record found or NULL. Caller is responsible
///         for freeing record when done.
extern LocationType*  LocDBFindCustomRecordV20      	(UInt16 iLibRefNum, DmOpenRef iDbP, UInt16* ioRecordIndex, MemHandle* oRecordH)
    SYS_TRAP (locsLibTrapLocDBFindCustomRecordV20);


#ifdef __cplusplus
}
#endif



#endif


/******************************************************************************
 *
 * END LocsLib.h
 *
 *****************************************************************************/
