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
#define DB_TYPE    'JsDb'
#define DB_CREATOR 'JSho'

// Application that will be launched to display new item
#define GOTO_ID    'JSho'

// EOF
