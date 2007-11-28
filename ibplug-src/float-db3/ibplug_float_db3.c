/* Main code for IBPlug Floating Event (DB3) */

#include <PalmOS.h>
#include "booger.h"

#include "ibplug_float_db3Rsc.h"
#include "ibplug_float_db3.h"
#include "datedb.h"

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

/* Set start and end times from a given seconds */
static void
SetSaneTimes(UInt32 secs, TimeType *startP, TimeType *endP)
{
    DateTimeType date_time;

    TimSecondsToDateTime(secs, &date_time);
    if (secs % 60)
        TimAdjust(&date_time, (60 - (secs % 60)));
    startP->hours = date_time.hour;
    startP->minutes = date_time.minute;
    if (startP->hours != 23) {
      endP->hours = startP->hours+1;
      endP->minutes = startP->minutes;
    } else {
      endP->hours = startP->hours;
      endP->minutes = 59;
    }
}

/* Create new item in app database */
static UInt16
DoTheBoogie(KleenexPtr kleenexP, DmOpenRef dbR, UInt16 *index)
{
    ApptDBRecordType datebook;
    ApptDateTimeType dbwhen;
    AlarmInfoType dbalarm;
    Char* funny_field;
    UInt32 secs;
    UInt16 result;

    MemSet(&datebook, sizeof(ApptDBRecordType), 0);
    MemSet(&dbwhen, sizeof(ApptDateTimeType), 0);
    MemSet(&dbalarm, sizeof(AlarmInfoType), 0);
    datebook.when = &dbwhen;
    datebook.alarm = &dbalarm;

    // Set up the record
    datebook.repeat = kleenexP->repeat;
    datebook.description = kleenexP->text;

    // Set date and time
    if (!(secs = kleenexP->alarm_secs))
        secs = TimGetSeconds();
    DateSecondsToDate(secs, &dbwhen.date);
    if (kleenexP->alarm_secs % 86400) {
        SetSaneTimes(secs, &dbwhen.startTime, &dbwhen.endTime);
    } else {
        TimeToInt(dbwhen.startTime) = apptNoTime;
        TimeToInt(dbwhen.endTime) = apptNoTime;
    }

    // Set the alarm
    dbalarm.advanceUnit = aauMinutes;
    if (kleenexP->alarm_secs) {
        dbalarm.advance = 0;
    } else {
        dbalarm.advance = apptNoAlarm;
        datebook.alarm = NULL;
    }

    // Set completion flag
    if (kleenexP->note)
        funny_field = (Char*)MemPtrNew(MemPtrSize(kleenexP->note) + 10);
    else
        funny_field = (Char*)MemPtrNew(10);

    if (kleenexP->is_complete)
        StrCopy(funny_field, "##c@@@@@@\n");
    else
        StrCopy(funny_field, "##f@@@@@@\n");

    /* Concatenate with orignal note */
    if (kleenexP->note)
        StrCat(funny_field, kleenexP->note);
    datebook.note = funny_field;

    // Add it to the DB
    result = ApptNewRecord(dbR, &datebook, index);
    
    /* Clean up */
    MemPtrFree(funny_field);

    return result;
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
    UInt32 result;

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
    if (!(kleenexP->booger.cmdPBP = MemPtrNew(sizeof(GoToParamsType))))
        return (1);
    MemSet(kleenexP->booger.cmdPBP, sizeof(GoToParamsType), 0);
    ((GoToParamsType *)(kleenexP->booger.cmdPBP))->dbCardNo = cardNo;
    ((GoToParamsType *)(kleenexP->booger.cmdPBP))->dbID = dbID;
    ((GoToParamsType *)(kleenexP->booger.cmdPBP))->recordNum = index;
    MemPtrSetOwner(kleenexP->booger.cmdPBP, 0);

    /* Send signal to set the alarm */
    if (kleenexP->alarm_secs) {
        if (DmGetNextDatabaseByTypeCreator(true, &searchstate, sysFileTApplication,
                                           GOTO_ID, true, &cardNo, &dbID))
        {
            FrmAlert(alertID_noDateBk3);
            MemPtrFree(kleenexP->booger.cmdPBP);
            return (1);
        }
        if (SysAppLaunch(cardNo, dbID, 0, sysAppLaunchCmdSyncNotify, NULL, &result))
            FrmAlert(alertID_noAlarmSet);
    }

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
