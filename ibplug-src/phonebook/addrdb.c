/* addrdb.c */

#include "addrdb.h"

static void
PrvAddrDBFindKey(PrvAddrPackedDBRecord *r, char **key, UInt16 *whichKey, Int16 sortByCompany)
{
	AddrDBRecordFlags fieldFlags;

	fieldFlags.allBits = r->flags.allBits;

	if (sortByCompany) {
		if (*whichKey == 1 && fieldFlags.bits.company) {
			*whichKey = 2;
			goto returnCompanyKey;
		}

		if (*whichKey <= 2 && fieldFlags.bits.name) {
			*whichKey = 3;
			goto returnNameKey;
		}

		if (*whichKey <= 3 && fieldFlags.bits.firstName) {
			*whichKey = 4;
			goto returnFirstNameKey;
		}

	} else {
		if (*whichKey == 1 && fieldFlags.bits.name) {
			*whichKey = 2;
			goto returnNameKey;
		}

		if (*whichKey <= 2 && fieldFlags.bits.firstName) {
			*whichKey = 3;
			goto returnFirstNameKey;
		}

		// For now don't consider company name when sorting by person name
		// unless there isn't a name or firstName
		if (*whichKey <= 3 && fieldFlags.bits.company &&
			!(fieldFlags.bits.name || fieldFlags.bits.firstName))
		{
			*whichKey = 4;
			goto returnCompanyKey;
		}
	}

	// All possible fields have been tried so return NULL so that
	// the uniq ID is compared.
	*whichKey = 5;
	*key = NULL;
	return;


returnCompanyKey:
	*key = (char *) &r->companyFieldOffset + r->companyFieldOffset;
	return;


returnNameKey:
	*key = &r->firstField;
	return;


returnFirstNameKey:
	*key = &r->firstField;
	if (r->flags.bits.name)
		*key += StrLen(*key) + 1;
	return;
}


static Int16
CategoryCompare (UInt8 attr1, UInt8 attr2, MemHandle appInfoH)
{
    Int16 result;
    UInt8 category1;
    UInt8 category2;
    AddrAppInfoPtr appInfoP;

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
PrvAddrDBComparePackedRecords(
	PrvAddrPackedDBRecord *r1,
	PrvAddrPackedDBRecord *r2,
	Int16 sortByCompany,
	SortRecordInfoPtr info1,
	SortRecordInfoPtr info2,
	MemHandle appInfoH)
{
	UInt16 whichKey1, whichKey2;
	char *key1, *key2;
	Int16 result;

	whichKey1 = 1;
	whichKey2 = 1;

	do {
		PrvAddrDBFindKey(r1, &key1, &whichKey1, sortByCompany);
		PrvAddrDBFindKey(r2, &key2, &whichKey2, sortByCompany);

		if (key1 == NULL) {
			if (key2 == NULL) {
				result = 0;
				return result;
			} else
				result = -1;

		} else if (key2 == NULL)
			result = 1;

		else if (!(result = StrCompare(key1, key2)))
			result = CategoryCompare(info1->attributes,
                                            info2->attributes, appInfoH);
	} while (!result);

	return result;
}


static AddrAppInfoPtr
AddrDBAppInfoGetPtr(DmOpenRef dbP)
{
	UInt16     cardNo;
	LocalID    dbID;
	LocalID    appInfoID;

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
PrvAddrDBFindSortPosition(DmOpenRef dbP,
	PrvAddrPackedDBRecord *newRecord,
	SortRecordInfoPtr newRecordInfo)
{
	Int16 sortByCompany;
	AddrAppInfoPtr appInfoPtr;

	appInfoPtr = (AddrAppInfoPtr)AddrDBAppInfoGetPtr(dbP);
	sortByCompany = appInfoPtr->misc.sortByCompany;
	MemPtrUnlock(appInfoPtr);

	return DmFindSortPosition(dbP, (void *)newRecord, newRecordInfo,
				(DmComparF *)PrvAddrDBComparePackedRecords,
				sortByCompany);
}


static void
PrvAddrDBPack(AddrDBRecordPtr s, void * recordP)
{
	Int32                offset;
	AddrDBRecordFlags    flags;
	Int16                index;
	PrvAddrPackedDBRecord*  d=0;
	Int16                len;
	void *               srcP;
	UInt8                companyFieldOffset;

	flags.allBits = 0;

	DmWrite(recordP, (Int32)&d->options, &s->options, sizeof(s->options));
	offset = (Int32)&d->firstField;

	for (index = firstAddressField; index < addressFieldsCount; index++) {
		if (s->fields[index] != NULL)
		{
			srcP = s->fields[index];
			len = StrLen(srcP) + 1;
			DmWrite(recordP, offset, srcP, len);
			offset += len;
                        flags.allBits |= ((UInt32)1 << index);
		}
	}

	// Set the flags indicating which fields are used
	DmWrite(recordP, (Int32)&d->flags.allBits, &flags.allBits, sizeof(flags.allBits));

	// Set the companyFieldOffset or clear it
	if (s->fields[company] == NULL)
		companyFieldOffset = 0;
	else {
		index = 1;
		if (s->fields[name] != NULL)
			index += StrLen(s->fields[name]) + 1;
		if (s->fields[firstName] != NULL)
			index += StrLen(s->fields[firstName]) + 1;
		companyFieldOffset = (UInt8) index;
	}
	DmWrite(recordP, (Int32)(&d->companyFieldOffset), &companyFieldOffset, sizeof(companyFieldOffset));
}


static Int16
PrvAddrDBUnpackedSize(AddrDBRecordPtr r)
{
	Int16 size;
	Int16 index;

	size = sizeof (PrvAddrPackedDBRecord) - sizeof (char);   // correct
	for (index = firstAddressField; index < addressFieldsCount; index++)
	{
		if (r->fields[index] != NULL)
			size += StrLen(r->fields[index]) + 1;
	}
	return size;
}


Err
AddrDBNewRecord(DmOpenRef dbP, AddrDBRecordPtr r, UInt16 category, UInt16 *index)
{
	MemHandle		recordH;
	PrvAddrPackedDBRecord*	recordP;
	SortRecordInfoType	sortInfo;
	UInt16			attr;

	if (!(recordH = DmNewHandle(dbP, (Int32)PrvAddrDBUnpackedSize(r))))
		return 1;

	recordP = MemHandleLock(recordH);
	PrvAddrDBPack(r, recordP);

	sortInfo.attributes = category;
	sortInfo.uniqueID[0] = 0;
	sortInfo.uniqueID[1] = 0;
	sortInfo.uniqueID[2] = 0;

	*index = PrvAddrDBFindSortPosition(dbP, recordP, &sortInfo);
	MemPtrUnlock(recordP);

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
