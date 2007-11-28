/* Error routines */
#define abort() ErrDisplayFileLineMsg(__FILE__, __LINE__, "")

#ifdef BOOGERDEBUG
#define ASSERT(a) if(!a) ErrDisplayFileLineMsg(__FILE__, __LINE__, "Assert")
#define ASSERTNOT(a) if(a) ErrDisplayFileLineMsg(__FILE__, __LINE__, "Assert")
#else  /* BOOGERDEBUG */
#define ASSERT(a)
#define ASSERTNOT(a)
#endif /* BOOGERDEBUG */

/*** Plugin specific #defines ***/

// Database to which item will be added
#define DB_TYPE    'vIMG'
#define DB_CREATOR 'View'

// Application that will be launched to display new item
#define GOTO_ID    'Spec'


typedef struct {
    Char name[32];
    UInt16 versionType;   /* Should be 0x00ff for diddlebug records */
    UInt32 nextRec;
    UInt32 note;
    UInt16 lastPosX;
    UInt16 lastPosY;
    UInt32 reserved;
    UInt16 anchorX;  /* 0xffff */
    UInt16 anchorY;  /* 0xffff */
    UInt16 width;    /* Word-aligned */
    UInt16 height;
    /* data  in screen format */
} ImageViewerRecordType;
typedef ImageViewerRecordType *ImageViewerRecordPtr;

// EOF
