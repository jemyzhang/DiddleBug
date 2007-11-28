#ifndef HI_NOTE_TEXT_H
#define HI_NOTE_TEXT_H

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
#define DB_TYPE    'HND2'
#define DB_CREATOR 'HNA1'

// Application that will be launched to display new item
#define GOTO_ID    'HNA1'


#define flagsTwistOpen   0x80
#define flagsDisplayable 0x40
#define flagsPict        0x20
#define flagsReadOnly    0x10

typedef struct {
    UInt8 flags;
    UInt8 level;
    /* Null terminated string */
    /* Pictdata */
} NoteRec;

#endif
// EOF
