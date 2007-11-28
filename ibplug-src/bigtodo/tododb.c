#include <PalmOS.h>

#include "tododb.h"

MemHandle
ToDoGetAppInfo(DmOpenRef dbP)
{
    UInt16 cardNo;
    LocalID dbID;
    LocalID appInfoID;

    DmOpenDatabaseInfo(dbP, &dbID, NULL, NULL, &cardNo, NULL);

    DmDatabaseInfo(cardNo, dbID, NULL, NULL, NULL, NULL, NULL,
                   NULL, NULL, &appInfoID, NULL, NULL, NULL);

    return ((MemHandle)MemLocalIDToGlobal(appInfoID, cardNo));
}


static Int16
DateTypeCmp(DateType d1, DateType d2)
{
    Int16 result;

    if ((result = d1.year - d2.year)) {
        if ((*(Int16 *) &d1) == -1)
            return 1;
        if ((*(Int16 *) &d2) == -1)
            return -1;
        return result;
    }

    if ((result = d1.month - d2.month))
        return result;

    return (d1.day - d2.day);
}


static Int16
CategoryCompare(UInt8 attr1, UInt8 attr2, MemHandle appInfoH)
{
    Int16 result;
    UInt8 category1;
    UInt8 category2;
    ToDoAppInfoPtr appInfoP;


    category1 = attr1 & dmRecAttrCategoryMask;
    category2 = attr2 & dmRecAttrCategoryMask;

    if ((result = category1 - category2)) {
        if (category1 == dmUnfiledCategory)
            return (1);
        else if (category2 == dmUnfiledCategory)
            return (-1);

        appInfoP = MemHandleLock(appInfoH);
        result = StrCompare(appInfoP->categoryLabels[category1],
                            appInfoP->categoryLabels[category2]);

        MemPtrUnlock(appInfoP);
        return result;
    }

    return result;
}


static Int16
ToDoCompareRecords(ToDoDBRecordPtr r1,
                   ToDoDBRecordPtr r2, Int16 sortOrder,
                   SortRecordInfoPtr info1, SortRecordInfoPtr info2,
                   MemHandle appInfoH)
{
    Int16 result = 0;

    /* Sort by priority, due date, and category */
    if (sortOrder == soPriorityDueDate) {
        result = (r1->priority & priorityOnly) - (r2->priority & priorityOnly);
        if (result == 0) {
            result = DateTypeCmp(r1->dueDate, r2->dueDate);
            if (result == 0) {
                result = CategoryCompare(info1->attributes,
                                            info2->attributes, appInfoH);
            }
        }
    }

    /* Sort by due date, priority, and category */
    else if (sortOrder == soDueDatePriority) {
        result = DateTypeCmp(r1->dueDate, r2->dueDate);
        if (result == 0) {
            result = (r1->priority & priorityOnly) -
                        (r2->priority & priorityOnly);
            if (result == 0) {
                result = CategoryCompare(info1->attributes,
                                            info2->attributes, appInfoH);
            }
        }
    }

    /* Sort by category, priority, due date */
    else if (sortOrder == soCategoryPriority) {
        result = CategoryCompare(info1->attributes, info2->attributes,
                                    appInfoH);
        if (result == 0) {
            result = (r1->priority & priorityOnly) -
                        (r2->priority & priorityOnly);
            if (result == 0) {
                result = DateTypeCmp(r1->dueDate, r2->dueDate);
            }
        }
    }

    /* Sort by category, due date, priority */
    else if (sortOrder == soCategoryDueDate) {
        result = CategoryCompare(info1->attributes, info2->attributes,
                                    appInfoH);
        if (result == 0) {
            result = DateTypeCmp(r1->dueDate, r2->dueDate);
            if (result == 0) {
                result = (r1->priority & priorityOnly) -
                            (r2->priority & priorityOnly);
            }
        }
    }

    return result;
}


UInt8
ToDoGetSortOrder(DmOpenRef dbP)
{
    UInt8 sortOrder = 0;
    ToDoAppInfoPtr appInfoP;

    appInfoP = MemHandleLock(ToDoGetAppInfo(dbP));
    sortOrder = appInfoP->sortOrder;
    MemPtrUnlock(appInfoP);

    return (sortOrder);
}


static UInt16
ToDoFindSortPosition(DmOpenRef dbP, ToDoDBRecord *newRecord,
                     SortRecordInfoPtr newRecordInfo)
{
    Int16 sortOrder;

    sortOrder = ToDoGetSortOrder(dbP);
    return (DmFindSortPosition(dbP, newRecord, newRecordInfo,
                               (DmComparF *)ToDoCompareRecords, sortOrder));
}


Err
ToDoNewRecord(DmOpenRef dbP, ToDoItemPtr item, UInt16 category, UInt16 *index)
{
    Err err;
    UInt16 size;
    Char zero=0;
    UInt16 attr;
    UInt16 newIndex;
    UInt32 offset;
    ToDoDBRecordPtr nilP=0;
    ToDoDBRecordPtr recordP;
    MemHandle recordH;
    SortRecordInfoType sortInfo;

    /* Compute the size of the new to do record */
    size = sizeDBRecord;
    if (item->description)
        size += StrLen(item->description);
    if (item->note)
        size += StrLen(item->note);

    /*  Allocate a chunk in the database for the new record */
    recordH = (MemHandle)DmNewHandle(dbP, size);
    if (recordH == NULL)
        return dmErrMemError;

    /* Pack the the data into the new record */
    recordP = MemHandleLock(recordH);
    DmWrite(recordP, (UInt32)&nilP->dueDate, &item->dueDate,
                    sizeof(nilP->dueDate));
    DmWrite(recordP, (UInt32)&nilP->priority, &item->priority,
                    sizeof(nilP->priority));

    offset = (UInt32)&nilP->description;
    if (item->description) {
        DmStrCopy(recordP, offset, item->description);
        offset += StrLen(item->description) + 1;
    } else {
        DmWrite(recordP, offset, &zero, 1);
        offset++;
    }

    if (item->note)
        DmStrCopy(recordP, offset, item->note);
    else
        DmWrite(recordP, offset, &zero, 1);

    sortInfo.attributes = category;
    sortInfo.uniqueID[0] = 0;
    sortInfo.uniqueID[1] = 0;
    sortInfo.uniqueID[2] = 0;

    /* Determine the sort position of the new record */
    newIndex = ToDoFindSortPosition(dbP, recordP, &sortInfo);

    MemPtrUnlock(recordP);

    /* Insert the record */
    if ((err = DmAttachRecord(dbP, &newIndex, (MemHandle)recordH, 0))) {
        MemHandleFree(recordH);

    } else {
        *index = newIndex;

        /* Set the category */
        DmRecordInfo(dbP, newIndex, &attr, NULL, NULL);
        attr &= ~dmRecAttrCategoryMask;
        attr |= category;
        DmSetRecordInfo(dbP, newIndex, &attr, NULL);
    }

    return err;
}

// EOF
