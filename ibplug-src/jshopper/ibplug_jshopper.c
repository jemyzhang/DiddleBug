/* Main code for IBPlug JShopper */

#include <PalmOS.h>
#include "booger.h"

#include "ibplug_jshopperRsc.h"
#include "ibplug_jshopper.h"
#include "jshop.h"

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

/* Create new item in app database */
static UInt16
DoTheBoogie(KleenexPtr kleenexP, DmOpenRef dbR, UInt16 *index)
{
    RecordType record;
    MemHandle hnd;
    MemPtr recordP;
    UInt16 strLen;

    MemSet(&record, sizeof(RecordType), 0);
    record.isNeeded = 0xffff;
    record.store = STORE_ALL;
    record.oneTime = 0xffff;
    record.quantity = 1;
    strLen = StrLen(kleenexP->text);
    *index = dmMaxRecordIndex;
    if (!(hnd = DmNewRecord(dbR, index, sizeof(RecordType)+strLen+1)))
        return (1);
    recordP = MemHandleLock(hnd);
    DmSet(recordP, 0, sizeof(RecordType)+strLen+1, 0);
    DmWrite(recordP, 0, &record, sizeof(RecordType));
    DmWrite(recordP, sizeof(RecordType), kleenexP->text, strLen);
    MemHandleUnlock(hnd);
    DmReleaseRecord(dbR, *index, true);

    return (errNone);
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

    /* Check for the correct version */
    if (!((kleenexP->version) & IBVERSION_ORIG))
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
    err = DoTheBoogie(kleenexP, dbR, &index);

    /* Close the database */
    if (closedb)
        DmCloseDatabase(dbR);

    /* Did it work? */
    if (err)
        return (1);

    /* Load the GoTo parameters */
    kleenexP->booger.cmdPBP = NULL;

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
    kleenexP->booger.cmd = sysAppLaunchCmdNormalLaunch;

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
