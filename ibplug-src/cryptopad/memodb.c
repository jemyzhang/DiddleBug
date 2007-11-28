/* memodb.c */

#include <PalmOS.h>
#include "memodb.h"

static Int16
CategoryCompare(UInt8 attr1, UInt8 attr2, MemHandle appInfoH)
{
    Int16 result;
    UInt8 category1;
    UInt8 category2;
    MemoAppInfoPtr appInfoP;

    category1 = attr1 & dmRecAttrCategoryMask;
    category2 = attr2 & dmRecAttrCategoryMask;

    if ((result = category1 - category2)) {
        if (category1 == dmUnfiledCategory)
            return (1);

        if (category2 == dmUnfiledCategory)
            return (-1);

        appInfoP = MemHandleLock(appInfoH);
        result = StrCompare(appInfoP->categoryLabels[category1],
                            appInfoP->categoryLabels[category2]);
        MemPtrUnlock(appInfoP);
    }

    return result;
}


static Int16
MemoCompareRecords(
	MemoDBRecordPtr r1,
	MemoDBRecordPtr r2,
	Int16 sortOrder,
	SortRecordInfoPtr info1,
	SortRecordInfoPtr info2,
	MemHandle appInfoH)
{
    Int16 result;

    if (!sortOrder)  // unsorted/manual sort
	return 0;

    if (!(result = StrCompare(&r1->note, &r2->note)))
	result = CategoryCompare(info1->attributes, info2->attributes, appInfoH);

    return (result);
}


static MemoAppInfoPtr
MemoGetAppInfo(DmOpenRef dbP)
{
    UInt16 cardNo;
    LocalID dbID;
    LocalID appInfoID;

    if (DmOpenDatabaseInfo(dbP, &dbID, NULL, NULL, &cardNo, NULL))
	return NULL;

    if (DmDatabaseInfo(cardNo, dbID, NULL, NULL, NULL, NULL, NULL,
			NULL, NULL, &appInfoID, NULL, NULL, NULL))
	return NULL;

    if (!appInfoID)
	return NULL;

    return MemLocalIDToLockedPtr(appInfoID, cardNo);
}


static UInt16
MemoFindSortPosition(DmOpenRef dbP,
	MemoDBRecordPtr newRecord,
	SortRecordInfoPtr newRecordInfo)
{
    Int16 sortOrder;
    MemoAppInfoPtr appInfoP;

    appInfoP = (MemoAppInfoPtr)MemoGetAppInfo(dbP);
    sortOrder = appInfoP->sortOrder;
    MemPtrUnlock(appInfoP);

    return DmFindSortPosition(dbP, (void *)newRecord, newRecordInfo,
			    (DmComparF *)MemoCompareRecords,
			    sortOrder);
}


Err
MemoNewRecord(DmOpenRef dbP, MemoItemPtr item, UInt16 category, UInt16 *index)
{
    MemHandle		recordH;
    MemoDBRecordPtr	recordP;
    SortRecordInfoType	sortInfo;
    UInt16		attr;

    if (!(recordH = DmNewHandle(dbP, (Int32)StrLen(item->note)+1)))
        return 1;

    recordP = MemHandleLock(recordH);
    DmStrCopy(recordP, 0, item->note);

    sortInfo.attributes = category;
    sortInfo.uniqueID[0] = 0;
    sortInfo.uniqueID[1] = 0;
    sortInfo.uniqueID[2] = 0;

    *index = MemoFindSortPosition(dbP, recordP, &sortInfo);
    MemPtrUnlock (recordP);

    if (DmAttachRecord(dbP, index, recordH, 0)) {
	MemHandleFree(recordH);
	return 1;
    }

    DmRecordInfo (dbP, *index, &attr, NULL, NULL);
    attr &= ~dmRecAttrCategoryMask;
    attr |= category;
    DmSetRecordInfo (dbP, *index, &attr, NULL);

    return 0;
}

/* EOF */
