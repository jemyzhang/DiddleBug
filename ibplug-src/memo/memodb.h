/* memodb.h */

typedef struct {
    UInt16	renamedCategories;
    char	categoryLabels[dmRecNumCategories][dmCategoryLength];
    UInt8	categoryUniqIDs[dmRecNumCategories];
    UInt8	lastUniqID;
    UInt8	reserved1;
    UInt16	reserved2;
    UInt16	reserved3;
    UInt8	sortOrder;
    UInt8	reserved;
} MemoAppInfoType;

typedef MemoAppInfoType *MemoAppInfoPtr;

#define soUnsorted	0
#define soAlphabetic	1

typedef struct {
    char note;
} MemoDBRecordType;

typedef MemoDBRecordType * MemoDBRecordPtr;

typedef struct {
    Char* note;
} MemoItemType;

typedef MemoItemType *MemoItemPtr;

extern Err MemoNewRecord(DmOpenRef dbP, MemoItemPtr item, UInt16 category, UInt16 *index);

/* EOF */
