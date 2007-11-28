/* Header file for Windows Bitmap */

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

// EOF
