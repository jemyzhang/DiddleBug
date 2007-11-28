
#define		MAX_STORES			15
#define		MAX_STORE_NAME_LENGTH		25

#define		CHECK_HAVE_IN_ALL		0x0001
#define		CHECK_HAVE_IN_NEEDS		0x0002
#define		SHOW_COUPON_COLUMN		0x0004
#define		SHOW_QUANTITY_COLUMN		0x0008
#define		NO_ASSOCIATE_ALL_STORES		0x0010
#define		SHOW_AISLE_COLUMN		0x0020
#define		SHOW_PRICE_COLUMN		0x0040

#define		STORE_1				0x0001
#define		STORE_2				0x0002
#define		STORE_3				0x0004
#define		STORE_4				0x0008
#define		STORE_5				0x0010
#define		STORE_6				0x0020
#define		STORE_7				0x0040
#define		STORE_8				0x0080
#define		STORE_9				0x0100
#define		STORE_10			0x0200
#define		STORE_11			0x0400
#define		STORE_12			0x0800
#define		STORE_13			0x1000
#define		STORE_14			0x2000
#define		STORE_15			0x4000
#define		STORE_ALL			0xffff


typedef struct			
{
	Int16	isNeeded;	// MUST remain first in structure
	Int16	store;		// bitmap flags for which store(s) it belongs to
	Int16	coupon;
	Int16	oneTime;
	Int16	quantity;
	Int16	aisle;
	Int16 	price;
	// Char	itemName;	// holds start of data for each field
} RecordType;			

typedef RecordType* RecordTypePtr;  

typedef struct 			// App Preferences Structure
{
	Int16				flags;
	Char    			storeNames[10+1][MAX_STORE_NAME_LENGTH+1];
	UInt16				topVisibleRecord;
	UInt16				currentRecord;
	Boolean				isShowingAll;
	Int16				currentStoreNum;
} AppPreferencesType; // old not used in 1.3 and up

typedef struct 
{
	Int16				flags;
	Char    			storeNames[MAX_STORES+1][MAX_STORE_NAME_LENGTH+1];
	UInt16				topVisibleRecord;
	UInt16				currentRecord;
	Boolean				isShowingAll;
	Int16				currentStoreNum;
	Int16				isShowingCoupons;
	Int16				reserved2;
	Int16				reserved3;
	Int16				reserved4;
	Int16				autoSort;
	Int16				all1Timers;
	Int16				autoQuantity;
	Int16				reserved8;
	Int16				reserved9;
	Int16				reserved10;
	Int16				reserved11;
	Int16				reserved12;
} JShopAppInfoType;

typedef JShopAppInfoType * JShopAppInfoPtr;

#define APP_TYPE	'JSho'    		// type for application. 
#define DB_TYPE		'JsDb'   		// type for application database.  
#define DB_NAME		"JShopperDB"		// database name


