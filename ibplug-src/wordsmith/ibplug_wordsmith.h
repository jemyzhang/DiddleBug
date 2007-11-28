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
#define DB_TYPE    'BDOC'
#define DB_CREATOR 'WrdS'

// Application that will be launched to display new item
#define GOTO_ID    'WrdS'


typedef struct {        /* 16 bytes total */
  Int16   version;      /* 1 = plain text, 2 = compressed text */
  Int16   reserved1;
  Int32   doc_size;     /* uncompressed size in bytes */
  Int16   num_recs;     /* not counting itself */
  Int16   rec_size;     /* in bytes: usually 4096 (4K) */
  Int32   reserved2;
} DocHeaderType;

// EOF
