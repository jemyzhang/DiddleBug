/* Main code for IBPlug Spec */

#include <PalmOS.h>
#include "booger.h"

#include "ibplug_specRsc.h"
#include "ibplug_spec.h"

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
    ImageViewerRecordType record;
    MemHandle hnd;
    MemPtr recordP;

    /* Fill the record */
    MemSet(&record, sizeof(ImageViewerRecordType), 0);
    StrNCopy(record.name, kleenexP->text, 31);
    record.versionType = 0x00ff;
    record.anchorX = 0xffff;
    record.anchorY = 0xffff;
    record.width = 160;
    record.height = 160;

    /* Write the record */
    *index = dmMaxRecordIndex;
    if (!(hnd = DmNewRecord(dbR, index, sizeof(ImageViewerRecordType)+3200)))
        return (1);
    recordP = MemHandleLock(hnd);
    DmWrite(recordP, 0, &record, sizeof(ImageViewerRecordType));
    DmWrite(recordP, sizeof(ImageViewerRecordType), kleenexP->data, 3200);
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
    UInt16 cardNo;
    LocalID dbID;
    UInt16 err, index;
    Char name[dmDBNameLength];

    /* Check for the correct version */
    if (!((kleenexP->version) & IBVERSION_PICTURE))
        return (boogerErrorVersionMismatch);

    /* Create the database */
    MemSet(name, 32, 0);
    StrNCopy(name, kleenexP->text, 31);
    cardNo = 0;
    if (DmCreateDatabase(cardNo, name, DB_CREATOR, DB_TYPE, false) ||
        !(dbID = DmFindDatabase(cardNo, name)) ||
        !(dbR = DmOpenDatabase(cardNo, dbID, dmModeReadWrite)))
    {
        return 1;
    }

    /* Call plugin specific routine to create item in database */
    err = DoTheBoogie(kleenexP, dbR, &index);

    /* Close the database */
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
