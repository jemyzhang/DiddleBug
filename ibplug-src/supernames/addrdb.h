/* addrdb.h */
#ifndef ADDRDB_H
#define ADDRDB_H

#include <PalmOS.h>

#define addrNumFields	19
#define numPhoneLabels	8

#define firstAddressField	name
#define firstPhoneField		phone1
#define lastPhoneField		phone5
#define numPhoneFields		(lastPhoneField - firstPhoneField + 1)
#define numPhoneLabelsStoredFirst  numPhoneFields
#define numPhoneLabelsStoredSecond (numPhoneLabels - numPhoneLabelsStoredFirst)

#define firstRenameableLabel	custom1
#define lastRenameableLabel	custom4
#define lastLabel		(addressFieldsCount + numPhoneLabelsStoredSecond)

#define IsPhoneLookupField(p)	(addrLookupWork <= (p) && (p) <= addrLookupMobile)

#define addrLabelLength		16

typedef union {
	struct {
		UInt16 reserved	:13;
		UInt16 note	:1;
		UInt16 custom4	:1;
		UInt16 custom3	:1;
		UInt16 custom2	:1;
		UInt16 custom1	:1;
		UInt16 title	:1;
		UInt16 country	:1;
		UInt16 zipCode	:1;
		UInt16 state	:1;
		UInt16 city	:1;
		UInt16 address	:1;
		UInt16 phone5	:1;
		UInt16 phone4	:1;
		UInt16 phone3	:1;
		UInt16 phone2	:1;
		UInt16 phone1	:1;
		UInt16 company	:1;
		UInt16 firstName :1;
		UInt16 name	:1;

	} bits;
	UInt32 allBits;
} AddrDBRecordFlags;

typedef union {
	struct {
		UInt16 reserved	:10;
		UInt16 phone8	:1;
		UInt16 phone7	:1;
		UInt16 phone6	:1;
		UInt16 note	:1;
		UInt16 custom4	:1;
		UInt16 custom3	:1;
		UInt16 custom2	:1;
		UInt16 custom1	:1;
		UInt16 title	:1;
		UInt16 country	:1;
		UInt16 zipCode	:1;
		UInt16 state	:1;
		UInt16 city	:1;
		UInt16 address	:1;
		UInt16 phone5	:1;
		UInt16 phone4	:1;
		UInt16 phone3	:1;
		UInt16 phone2	:1;
		UInt16 phone1	:1;
		UInt16 company	:1;
		UInt16 firstName :1;
		UInt16 name	:1;
	} bits;
	UInt32 allBits;
} AddrDBFieldLabelsDirtyFlags;

typedef struct {
	UInt16 reserved:7;
	UInt16 sortByCompany	:1;
} AddrDBMisc;

typedef enum {
	name,
	firstName,
	company,
	phone1,
	phone2,
	phone3,
	phone4,
	phone5,
	address,
	city,
	state,
	zipCode,
	country,
	title,
	custom1,
	custom2,
	custom3,
	custom4,
	note,
	addressFieldsCount
} AddressFields;

typedef union {
	struct {
		UInt16 reserved	:8;
		UInt16 email	:1;
		UInt16 fax	:1;
		UInt16 pager	:1;
		UInt16 voice	:1;
		UInt16 mobile	:1;
		UInt16 work	:1;
		UInt16 home	:1;
		UInt16 preferred :1;
	} bits;
	UInt32 allBits;
} AddrDBPhoneFlags;

typedef enum {
	workLabel,
	homeLabel,
	faxLabel,
	otherLabel,
	emailLabel,
	mainLabel,
	pagerLabel,
	mobileLabel
} AddressPhoneLabels;

typedef union {
	struct {
		UInt16 reserved:8;
		UInt16 displayPhoneForList:4;
		UInt16 phone5:4;
		UInt16 phone4:4;
		UInt16 phone3:4;
		UInt16 phone2:4;
		UInt16 phone1:4;
	} phones;
	UInt32 phoneBits;
} AddrOptionsType;

typedef struct {
	AddrOptionsType	options;
	Char *		fields[addressFieldsCount];
} AddrDBRecordType;

typedef AddrDBRecordType *AddrDBRecordPtr;

typedef char addressLabel[addrLabelLength];

typedef struct {
	UInt16	renamedCategories;
	char	categoryLabels[dmRecNumCategories][dmCategoryLength];
	UInt8	categoryUniqIDs[dmRecNumCategories];
	UInt8	lastUniqID;

	UInt8	reserved1;
	UInt16	reserved2;
	AddrDBFieldLabelsDirtyFlags dirtyFieldLabels;
	addressLabel	fieldLabels[addrNumFields + numPhoneLabelsStoredSecond];
	CountryType	country;
	UInt8		reserved;
	AddrDBMisc	misc;
} AddrAppInfoType;

typedef AddrAppInfoType *AddrAppInfoPtr;

typedef struct {
	AddrOptionsType		options;
	AddrDBRecordFlags	flags;
	UInt8			companyFieldOffset;
	char			firstField;
} PrvAddrPackedDBRecord;

extern Err AddrDBNewRecord(DmOpenRef dbP, AddrDBRecordPtr r, UInt16 category, UInt16 *index);

#endif
/* EOF */
