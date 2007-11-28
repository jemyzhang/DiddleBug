/*******************************************************************************
 * Copyright (c) 2004-2005 palmOne, Inc. or its subsidiaries.
 * All rights reserved.
 ******************************************************************************/
/**
 * @ingroup Helper
 */

/**
 * @file 	MmsHelperCommon.h
 * @version 1.0
 * @brief	Public 68k common header files to be used with Default Helper Library.
 *
 * This file contains the type and structure parameters for the MMS Helper Service Class.
 * Developers should register to this service using Default Helper library if it wants
 * to handle MMS.
 *
 * <hr>
 */
 
#ifndef HELPER_MMS_H
#define HELPER_MMS_H

/** New helper class registered with PalmSource for sending by MMS */
#define    kHelperServiceClassIDMMS    'mms_'

/** Structure to hold the MMS object. This structure is part of the linked list
  * which is pointed by the object member of HelperServiceMMSDetailsType */
typedef struct _HelperServiceMMSObjectType 
{
	Int16	pageNum;			/**< IN: page of message this content should appear on. */
	Char* 	mimeType;			/**< IN: IANA Mime type for this object (compulsory!)
								     The object may be passed in a buffer or via a file.
								     Either set bufferP and bufferLen or set tempFileName
								  */
								
	MemPtr	bufferP;			/**< IN: The object is stored in a buffer if this is non-NULL */
	UInt32	bufferLen;			/**< IN: Length of the buffer */
	
	Char* 	tempFileName;		/**< IN: The temporary file its stored in if bufferP is NULL */
	Boolean isVFSFileName; 		/**< IN: Is the file local or under VFS e.g Memory card? */
	Boolean freeTempFile;		/**< IN: If set, the file should be deleted by the helper application */
	
	Char* 	name;				/**< IN: the name of the object as sent to us (maybe the filename) */
	
	struct 	_HelperServiceMMSObjectType* nextP; /**< IN: Linked list pointer of objects to send. */

} HelperServiceMMSObjectType;

/**
 * @brief Helpers of this Service Class send an MMS message.
 *
 * "To" address(es) are passed in the 'pData' element of the main structure
 * (HelperNotifyExecuteType)
 *
 * The 'pDetails' struct member may optionally point to
 * HelperServiceMMSDetailsType for this service class.
 */
typedef struct _HelperServiceMMSDetailsType
{
	UInt16	version;			  		/**< This is currently version 1 */

	Char*	cc;					  		/**< IN: carbon copy address string or NULL -- will
								  		     be duplicated by helper if necessary;
								  		     multiple addresses are separated by
								  		     semicolon (ex. "john@host.com; jane@host.com") */
								  		
	Char*	subject;			  		/**< IN: subject string or NULL -- will be duplicated
								  		     by helper if necessary (ex. "helper API") */
								  		
	Char*	message;			  		/**< IN: initial message body string or NULL -- will be
								  		     duplicated by helper if necessary (ex.
								 		     "Lets discuss the helper API tomorrow.")
								 		     Message will be put on first page. */
								 		
	HelperServiceMMSObjectType* object; /**< IN: Head of Linked list of objects to send or NULL */

	Char* 	bcc;						/**< IN: blind carbon copy address string or NULL -- will
								  		     be duplicated by helper if necessary;
								  		     multiple addresses are separated by
								  		     semicolon (ex. "john@host.com; jane@host.com") */
								  		
	Boolean justSend; 			  		/**< IN: Set this to true to send without invoking any UI */

} HelperServiceMMSDetailsType;


#endif //HELPER_MMS_H
