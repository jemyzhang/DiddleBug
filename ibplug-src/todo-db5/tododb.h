#ifndef __TDTODOMGR_H__
#define __TDTODOMGR_H__

#include <DateTime.h>

#define LocalizedAppInfoStr	1000

#define todoLabelLength		12
#define todoNumFields		16

#define toDoMaxPriority		5

/* Sort orders */
#define soDueDatePriority	0
#define soPriorityDueDate	1
#define soCategoryPriority	2
#define soCategoryDueDate	3

#define toDoSortByPriorityDirty	0x0001

typedef struct {
  UInt16 renamedCategories;
  Char 	categoryLabels[dmRecNumCategories][dmCategoryLength];
  UInt8 categoryUniqIDs[dmRecNumCategories];
  UInt8	lastUniqID;
  UInt8	reserved1;
  UInt16 reserved2;
  UInt16 dirtyAppInfo;
  UInt8	sortOrder;
} ToDoAppInfoType;
typedef ToDoAppInfoType * ToDoAppInfoPtr;

typedef struct {
  DateType dueDate;
  UInt8 priority;	// high bit is complete flag
  Char description;
} ToDoDBRecord;

typedef ToDoDBRecord * ToDoDBRecordPtr;

#define sizeDBRecord (sizeof (ToDoDBRecord) + 1)

typedef struct {
  DateType dueDate;
  UInt8 priority;	// high bit is complete flag
  Char* description;
  Char* note;
} ToDoItemType;
typedef ToDoItemType * ToDoItemPtr;

/* Flags */
#define completeFlag 	0x80
#define priorityOnly 	~completeFlag
#define toDoNoDueDate	0xffff

/* Routines */    
Err ToDoNewRecord(DmOpenRef dbP, ToDoItemPtr item, UInt16 category, UInt16 *index);
MemHandle ToDoGetAppInfo(DmOpenRef dbP);
UInt8 ToDoGetSortOrder(DmOpenRef dbP);

#endif
