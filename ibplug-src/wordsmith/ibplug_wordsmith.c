/* Main code for IBPlug WordSmith */

#include <PalmOS.h>
#include "booger.h"

#include "ibplug_wordsmithRsc.h"
#include "ibplug_wordsmith.h"

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

/* Create new wordsmith file */
static UInt16
DoTheBoogie(KleenexPtr kleenexP, UInt16 *cardNo, LocalID *dbID)
{
    DmOpenRef dbR;
    Char title[dmDBNameLength];
    DocHeaderType hdr;
    UInt16 idx;
    MemHandle hnd;
    MemPtr data;
    const UInt32 titleLen = StrLen(kleenexP->text);
    const UInt32 noteLen = kleenexP->note ? StrLen(kleenexP->note) : 0;

    /* Create the doc file database */
    *cardNo = 0;
    MemSet(title, dmDBNameLength, 0);
    StrNCopy(title, kleenexP->text, 31);
    if (DmCreateDatabase(*cardNo, title, DB_CREATOR, DB_TYPE, false) ||
        !(*dbID = DmFindDatabase(0, title)) ||
        !(dbR = DmOpenDatabase(*cardNo, *dbID, dmModeReadWrite)))
    {
        FrmCustomAlert(alertID_file_exists, title, "", "");
        return (1);
    }

    /* Set up the header */
    MemSet(&hdr, sizeof(DocHeaderType), 0);
    hdr.version = 1;
    hdr.doc_size = noteLen ? titleLen + 2 + noteLen : titleLen;
    hdr.num_recs = 1;
    hdr.rec_size = 4096;

    /* Write the header */
    idx = dmMaxRecordIndex;
    if ((hnd = DmNewRecord(dbR, &idx, sizeof(DocHeaderType)))) {
        data = MemHandleLock(hnd);
        DmWrite(data, 0, &hdr, sizeof(DocHeaderType));
        MemHandleUnlock(hnd);
        DmReleaseRecord(dbR, idx, true);
    }

    /* Write the title to the doc */
    idx = dmMaxRecordIndex;
    if ((hnd = DmNewRecord(dbR, &idx, hdr.doc_size))) {
        data = MemHandleLock(hnd);
        DmWrite(data, 0, kleenexP->text, titleLen);
	if (noteLen) {
	  DmSet(data, titleLen, 2, '\n');
	  DmWrite(data, titleLen + 2, kleenexP->note, noteLen);
	}
        MemHandleUnlock(hnd);
        DmReleaseRecord(dbR, idx, true);
    }

    /* Close the database */
    DmCloseDatabase(dbR);

    return (errNone);
}

/*** End of plugin specific code ***/


/* Calls plugin specific code to create new doc file,
 * then fills in GoTo parameters.
 */
static UInt16
PickBooger(KleenexPtr kleenexP)
{
    UInt16 cardNo;
    LocalID dbID;
    UInt16 err;

    /* Check for the correct version */
    if (!((kleenexP->version) & IBVERSION_ORIG))
        return (boogerErrorVersionMismatch);

    /* Call plugin specific routine to create doc file */
    err = DoTheBoogie(kleenexP, &cardNo, &dbID);

    /* Did it work? */
    if (err)
        return (1);

    /* Load the GoTo parameters */
    if (!(kleenexP->booger.cmdPBP = MemPtrNew(sizeof(SysAppLaunchCmdOpenDBType))))
        return (1);
    MemSet(kleenexP->booger.cmdPBP, sizeof(SysAppLaunchCmdOpenDBType), 0);
    ((SysAppLaunchCmdOpenDBType *)(kleenexP->booger.cmdPBP))->cardNo = cardNo;
    ((SysAppLaunchCmdOpenDBType *)(kleenexP->booger.cmdPBP))->dbID = dbID;
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

    kleenexP->booger.cmd = sysAppLaunchCmdOpenDB;

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
        kleenexP->booger.cmd = boogerPlugLaunchCmdLaunchLauncher;
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


UInt32
PilotMain(UInt16 cmd, MemPtr cmdPBP, UInt16 launchFlags)
{
    UInt16 err;
    EventType evt;

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

    if ((cmd == boogerPlugLaunchCmdLaunchLauncher) ||
        (cmd == sysAppLaunchCmdGoTo))
    {
        /* Tell OS to start launcher */
        MemSet(&evt, sizeof(EventType), 0);
        evt.eType = keyDownEvent;
        evt.data.keyDown.chr = launchChr;
        evt.data.keyDown.modifiers = commandKeyMask;
        EvtAddEventToQueue(&evt);

        /* Loop until told to quit */
        do {
            EvtGetEvent(&evt, evtWaitForever);
            SysHandleEvent(&evt);
        } while (evt.eType != appStopEvent);
        return (errNone);
    }

    return (sysErrParamErr);
}

// EOF
