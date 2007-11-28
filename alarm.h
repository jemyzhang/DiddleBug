#ifndef DIDDLEBUG_ALARM_H
#define DIDDLEBUG_ALARM_H

#include <PalmOS.h>
#include "sections.h"

/* Constants */
#define apptMaxDisplayableAlarms 10

/* Alarm queue data structures */
typedef struct {
  UInt16 recordNum;
  UInt32 alarmTime;
} PendingAlarmType;

typedef struct {
  UInt16 pendingCount;
  PendingAlarmType pendingAlarm[apptMaxDisplayableAlarms];
  UInt16 dismissedCount;
  UInt32 dismissedAlarm[apptMaxDisplayableAlarms];
} PendingAlarmQueueType;

typedef union {
  struct {
    UInt16 recordNum;
    UInt16 ref;
  } split;

  UInt32 united;

} RefType;

extern void AlarmInit(void);
extern void DisplayAlarm(Boolean appIsActive);
extern void AlarmTriggered(SysAlarmTriggeredParamType* cmdPBP, Boolean appIsActive);
extern void RescheduleAlarms (DmOpenRef dbP);
extern void AlarmReset (Boolean newerOnly);
extern void PlayAlarmSound(UInt32 uniqueRecID, LocalID dbID, UInt16 cardNo);
extern UInt32 AlarmGetTrigger (UInt32* refP);
extern void AlarmSetTrigger (UInt32 alarmTime, UInt32 ref);
extern UInt32 ApptGetAlarmTime(DiddleBugRecordType* record, UInt32 currentTime);

#endif /* DIDDLEBUG_ALARM_H */
