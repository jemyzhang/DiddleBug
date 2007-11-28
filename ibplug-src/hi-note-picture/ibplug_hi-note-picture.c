/* Main code for IBPlug Hi-Note (picture) */

#include <PalmOS.h>
#include "booger.h"

#include "ibplug_hi-note-pictureRsc.h"
#include "ibplug_hi-note-picture.h"

/*** Plugin specific code ***/

/* Get preferences, open (or create) app database */
static UInt16
StartApplication(void)
{
    return (errNone);
}

/* Save preferences, close forms, close app database */
static void
StopApplication(void)
{
}

/* Get the Hi-Note record size */
static UInt32
HiNoteDoStuff(MemPtr data,MemPtr oP,Boolean justCounting)
{
    UInt32 srcCnt, dupCnt, wrCnt, finalCnt, padCnt;
    UInt8 *srcPtr;
    UInt8 *wrPtr;
    UInt8 *outP;
    UInt8 lastByte;
    UInt8 wrBuf[514];
    UInt16 width, height;

    srcCnt = 3200;
    finalCnt = 4; /* width and height */
    wrCnt = 0;
    srcPtr = data;
    wrPtr = &wrBuf[2];
    outP = oP;
    if (!justCounting) {
        width = 160; height = 250;
        MemMove(outP, &width, 2);
        outP += 2;
        MemMove(outP, &height, 2);
        outP += 2;
    }
    while (srcCnt) {
        lastByte = *srcPtr++;
        *wrPtr++ = lastByte;
        srcCnt--;
        wrCnt++;
        /* Check write trigger conditions */
        if (((srcCnt > 1) && (lastByte == srcPtr[0]) && (lastByte == srcPtr[1]))
                    || (wrCnt == 512) || (!srcCnt)) {
            /* Write required */
            dupCnt = 0;
            while (srcCnt) {
                if (*srcPtr != lastByte) break; /* Next byte not a duplicate */
                srcPtr++;
                srcCnt--;
                if (++dupCnt == 63) break; /* Maximum duplicate count */
            }
            wrCnt--;
            wrBuf[0] = (dupCnt << 2) | (wrCnt >> 8);
            wrBuf[1] = wrCnt;
            wrCnt += 3;
            if (justCounting) {
                finalCnt += wrCnt;
            }
            else {
                MemMove(outP, wrBuf, wrCnt);
                outP += wrCnt;
            }
            wrPtr = &wrBuf[2];
            wrCnt = 0;
        }
    }
    /* Add the extra blank space to fill to 250 pixels height */
    padCnt = 1800; /* (250-160)*160/8 */
    while (padCnt) {
        wrCnt = 0;
        if (padCnt >= 63) {
            dupCnt = 63;
            padCnt -= 63;
        }
        else {
            dupCnt = padCnt;
            padCnt = 0;
        }
        wrBuf[0] = (dupCnt << 2) | (wrCnt >> 8);
        wrBuf[1] = wrCnt;
        wrBuf[2] = 0x00;
        if (justCounting) finalCnt += 3;
        else {
            MemMove(outP, wrBuf, 3);
            outP += 3;
        }
    }

    return finalCnt;
}

/* Create new item in app database */
static UInt16
DoTheBoogie(KleenexPtr kleenexP, DmOpenRef dbR, UInt16 *index,
            UInt32 *textLength, UInt32 *uniqueID)
{
    NoteRec note;
    MemHandle hnd;
    MemPtr recordP, sketchP;
    UInt32 recordSize;

    note.flags = 0x60; /* displayable and pict */
    note.level = 0x00; /* root level */

    *textLength = StrLen(kleenexP->text);
    recordSize = HiNoteDoStuff(kleenexP->data, NULL, true); /* just cnt */

    if (!(sketchP = MemPtrNew(recordSize)))
        return (1);
    HiNoteDoStuff(kleenexP->data, sketchP, false); /* fill */
    recordSize += *textLength; /* the pick text */
    recordSize += 3; /* flags and level and null for text */

    *index = dmMaxRecordIndex;
    if (!(hnd = DmNewRecord(dbR, index, recordSize)))
        return (1);
    recordP = MemHandleLock(hnd);
    DmWrite(recordP, 0, &note, 2);
    DmWrite(recordP, 2, kleenexP->text, *textLength + 1);
    DmWrite(recordP, *textLength + 3, sketchP, recordSize - 3 - *textLength);
    MemPtrFree(sketchP);
    MemHandleUnlock(hnd);
    DmReleaseRecord(dbR, *index, true);

    /* Get the unique ID */
    *uniqueID = 0;
    return (DmRecordInfo(dbR, *index, NULL, uniqueID, NULL));
}

/*** End of plugin specific code ***/


/* Finds and opens database,
 * calls plugin specific code to create new item,
 * then fills in GoTo parameters.
 */
static UInt16
PickBooger(KleenexPtr kleenexP)
{
    DmOpenRef dbR;
    DmSearchStateType searchstate;
    UInt32 creatorID, dbtype;
    UInt16 cardNo;
    LocalID dbID;
    Boolean closedb = false;
    UInt16 err, index;
    UInt32 textLength, uniqueID;

    /* Check for the correct version */
    if (!((kleenexP->version) & IBVERSION_PICTURE))
        return (boogerErrorVersionMismatch);

    /* Open the database */
    if (DmGetNextDatabaseByTypeCreator(true, &searchstate, DB_TYPE,
                                       DB_CREATOR, true, &cardNo, &dbID))
        return (1);

    if ((dbR = DmOpenDatabase(cardNo, dbID, dmModeReadWrite))) {
        closedb = true;

    } else if (DmGetLastErr() == dmErrAlreadyOpenForWrites) {
        dbR = NULL;
        while ((dbR = DmNextOpenDatabase(dbR))) {
            DmOpenDatabaseInfo(dbR, &dbID, NULL, NULL, &cardNo, NULL);
            DmDatabaseInfo(cardNo, dbID, NULL, NULL, NULL, NULL, NULL, NULL,
                           NULL, NULL, NULL, &dbtype, &creatorID);
            if ((dbtype == DB_TYPE) && (creatorID == DB_CREATOR))
                break;
        }
        if (!dbR)
            return (1);
    }

    /* Call plugin specific routine to create item in database */
    err = DoTheBoogie(kleenexP, dbR, &index, &textLength, &uniqueID);

    /* Close the database */
    if (closedb)
        DmCloseDatabase(dbR);

    /* Did it work? */
    if (err)
        return (1);

    /* Load the GoTo parameters */
    if (!(kleenexP->booger.cmdPBP = MemPtrNew(sizeof(GoToParamsType))))
        return (1);
    MemSet(kleenexP->booger.cmdPBP, sizeof(GoToParamsType), 0);
    ((GoToParamsType *)(kleenexP->booger.cmdPBP))->dbCardNo = cardNo;
    ((GoToParamsType *)(kleenexP->booger.cmdPBP))->dbID = dbID;
    ((GoToParamsType *)(kleenexP->booger.cmdPBP))->recordNum = index;
    ((GoToParamsType *)(kleenexP->booger.cmdPBP))->matchPos = textLength;
    ((GoToParamsType *)(kleenexP->booger.cmdPBP))->matchCustom = uniqueID;
    MemPtrSetOwner(kleenexP->booger.cmdPBP, 0);

    return (errNone);
}


/* Fills in 'booger' structure for return to calling app */
static UInt16
BlowNose(KleenexPtr kleenexP)
{
    DmSearchStateType searchstate;

    if (DmGetNextDatabaseByTypeCreator(true, &searchstate, sysFileTApplication,
                                       GOTO_ID, true,
                                       &kleenexP->booger.cardNo,
                                       &kleenexP->booger.dbID))
    {
        // App associated with plugin not found
        if (kleenexP->booger.cmdPBP) {
            MemPtrFree(kleenexP->booger.cmdPBP);
            kleenexP->booger.cmdPBP = NULL;
        }
        return (1);
    }
    kleenexP->booger.cmd = sysAppLaunchCmdGoTo;

    return (errNone);
}


/* Jumps to newly created item using app specified in 'booger' structure */
static UInt16
Sneeze(KleenexPtr kleenexP)
{
    UInt16 err;
    LocalID dbID;
    UInt16 cardNo;

    // Free up parameters in 'kleenex' structure
    if (kleenexP->note)      MemPtrFree(kleenexP->note);
    if (kleenexP->data)      MemPtrFree(kleenexP->data);
    if (kleenexP->text)      MemPtrFree(kleenexP->text);
    if (kleenexP->title)     MemPtrFree(kleenexP->title);
    if (kleenexP->category)  MemPtrFree(kleenexP->category);
    if (kleenexP->repeat)    MemPtrFree(kleenexP->repeat);

    // Jump to default app if not specified
    if (!kleenexP->booger.dbID) {
        if ((err = BlowNose(kleenexP)))
            return (err);
    }

    if ((kleenexP->booger.cmd == sysAppLaunchCmdNormalLaunch) ||
        (kleenexP->booger.cmd == boogerPlugLaunchCmdLaunchLauncher))
    {
        // Don't use the GoTo params
        if (kleenexP->booger.cmdPBP) {
            MemPtrFree(kleenexP->booger.cmdPBP);
            kleenexP->booger.cmdPBP = NULL;
        }

    } else if (kleenexP->booger.cmd == sysAppLaunchCmdOpenDB) {
        // Change GoTo params to OpenDB params
        cardNo = ((GoToParamsType *)(kleenexP->booger.cmdPBP))->dbCardNo;
        dbID = ((GoToParamsType *)(kleenexP->booger.cmdPBP))->dbID;
        ((SysAppLaunchCmdOpenDBType *)(kleenexP->booger.cmdPBP))->cardNo = cardNo;
        ((SysAppLaunchCmdOpenDBType *)(kleenexP->booger.cmdPBP))->dbID = dbID;
    }

    // Achoo!
    if (SysUIAppSwitch(kleenexP->booger.cardNo, kleenexP->booger.dbID,
                       kleenexP->booger.cmd, kleenexP->booger.cmdPBP))
    {
        if (kleenexP->booger.cmdPBP)
            MemPtrFree(kleenexP->booger.cmdPBP);
        return (1);
    }

    return (errNone);
}


/* Start here! */
UInt32
PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags)
{
    UInt16 err;

    // Called as a subroutine
    if (cmd == boogerPlugLaunchCmdBlowNose) {
        if ((err = StartApplication()))
            return (err);

        if (!(err = PickBooger((KleenexPtr)cmdPBP)))
            err = BlowNose((KleenexPtr)cmdPBP);

        StopApplication();
        return (err);
    }

    // Running as an application
    if (cmd == boogerPlugLaunchCmdSneeze) {
        if ((err = StartApplication()))
            return (err);

        if (!(err = PickBooger((KleenexPtr)cmdPBP)))
            err = Sneeze((KleenexPtr)cmdPBP);

        StopApplication();
        return (err);
    }

    return (sysErrParamErr);
}

// EOF
