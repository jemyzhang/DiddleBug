#define DiddleBugErrorMessages 0 /* ErrorManager looks up tSTL 0 resource */

// These five should stay in order
#define BUTTONBAR_BM_START 1
#define UpDownBitmap       1
#define ClearBitmap        2
#define NewBitmap          3
#define DeleteBitmap       4
#define LockBitmap         5
#define BUTTONBAR_BM_END   5

#define BugBitmap            7
#define XferDoneBitmap      12
#define XferTodoBitmap      13
#define XferDatebookBitmap  14
#define XferAddressBitmap   15
#define ActionBitmap        16
#define XferNoAppBitmap     17
#define XferDoneGotoBitmap  18
#define CmdPaintBitmap      30
// #define CmdSmearBitmap      31 /* not used anymore, but don't remove! */
#define CmdEraseBitmap      32
#define CmdRoughBitmap      40
#define CmdSmoothBitmap     41
#define CmdSmootherBitmap   42
#define CategoryBitmap      43
#define AllCategoriesBitmap 44

// icons for the Insert Shape dialog
#define ShapeRectBitmap			50
#define ShapeRoundRectBitmap 	        51
#define ShapeCircleBitmap		52
#define ShapeOvalBitmap			53

#define ShapeRectFilledBitmap		60
#define ShapeRectFilledXorBitmap	61
#define ShapeRectOutlineXorBitmap	62

#define ShapeRoundRectFilledBitmap	65
#define ShapeRoundRectFilledXorBitmap 	66
#define ShapeRoundRectOutlineXorBitmap 	67

#define ShapeCircleFilledBitmap		70
#define ShapeCircleFilledXorBitmap	71
#define ShapeCircleOutlineXorBitmap	72

#define ShapeOvalFilledBitmap		75
#define ShapeOvalFilledXorBitmap	76
#define ShapeOvalOutlineXorBitmap	77

// icons for OS 3.5 command bar
#define PaintCmdBitmap	101
#define SmearCmdBitmap	102
#define EraseCmdBitmap	103

// icons for alarm dialog
#define DayBitmap       150
#define MorningBitmap   151
#define NightBitmap     152

// icons for "list" views
#define TextListBitmap            160
#define ThumbnailListBitmap       161
#define ThumbnailDetailListBitmap 162

// icons for thumbnail detail view
#define TrashBitmap     170

// icons for Treo 600 buttons
#define Treo600PhoneBitmap     180
#define Treo600CalendarBitmap  181
#define Treo600MessagingBitmap 182
#define Treo600DisplayBitmap   183

// default alarm MIDI signal
#define AlarmMIDI 999

#define DiddleForm      1000
#define DiddleListForm  1500
#define ThumbnailListButton 1501
#define ThumbnailDetailListButton 1502
#define TextListButton  1503
#define SketchTable     1510
#define NewButton       1520
#define ScrollBar       1530
#define DiddleThumbnailForm 1600
#define Thumb1          1601
#define Thumb2          1602
#define Thumb3          1603
#define Thumb4          1604
#define Thumb5          1605
#define Thumb6          1606
#define Thumb7          1607
#define Thumb8          1608
#define Thumb9          1609
#define Thumb10         1610
#define Thumb11         1611
#define Thumb12         1612
#define Thumb13         1613
#define Thumb14         1614
#define Thumb15         1615
#define Thumb16         1616
#define Thumb17         1617
#define Thumb18         1618
#define Thumb19         1619
#define Thumb20         1620
#define Thumb21         1621
#define Thumb22         1622
#define Thumb23         1623
#define Thumb24         1624
#define Thumb25         1625
#define Thumb26         1626
#define Thumb27         1627
#define Thumb28         1628
#define Thumb29         1629
#define Thumb30         1630
#define DiddleThumbnailDetailForm 1700
#define Thumb1Name      1701
#define Thumb2Name      1702
#define Thumb3Name      1703
#define Thumb4Name      1704
#define Thumb1Alarm     1710
#define Thumb2Alarm     1711
#define Thumb3Alarm     1712
#define Thumb4Alarm     1713
#define Line1           1720
#define Line2           1721
#define Line3           1722
#define Line4           1723
#define Thumb1AlarmButton 1730
#define Thumb2AlarmButton 1731
#define Thumb3AlarmButton 1732
#define Thumb4AlarmButton 1733
#define Thumb1NoteButton 1740
#define Thumb2NoteButton 1741
#define Thumb3NoteButton 1742
#define Thumb4NoteButton 1743
#define Thumb1TrashButton 1750
#define Thumb2TrashButton 1751
#define Thumb3TrashButton 1752
#define Thumb4TrashButton 1753
#define Thumb1NameMasked 1760
#define Thumb2NameMasked 1761
#define Thumb3NameMasked 1762
#define Thumb4NameMasked 1763

#define DiddleTForm     2000
#define NoteButton      2050
#define FilledNoteButton 2051
#define ModeButton      2100
#define SmoothButton    2200
#define PenButton       2300
#define InkButton       2400
#define CategoryPop     2450
#define CategoryList    2451
#define CategoryButton  2452
#define PageButton      2490
#define PageList        2491

// These six should stay in order
#define BUTTONBAR_START  2501
#define MainDateSelTrig  2501
#define MainSwitchButton 2502
#define MainClearButton  2503
#define MainNewButton    2504
#define MainDeleteButton 2505
#define MainLockButton   2506
#define BUTTONBAR_END    2506

#define MainSmoothList  2508
#define MainPenList     2509
#define MainInkList     2510

// New transfer stuff
#define XferDoneButton    2600
#define XferDetailsButton 2601
#define XferField         2602
#define XferList          2603
#define XferDoneList      2604

#define PopShortcutList  2700
#define PopCountdownList 2800
#define DiddleMenu       3000
#define DiddleMenu35     3001
#define DiddleListMenu   3010
#define XferMenu         3100
#define TextMenu        10000 /* use the PalmOS standard Edit menu */
#define TimeForm         5000
#define TimeOkButton     5001
#define TimeCancelButton 5002
#define TimeMinutesList  5003
#define TimeSetCheck     5005
#define TimeSignalTrig   5014
#define TimeRepeatTrig   5015
#define TimeNowButton    5016
#define TimeMorningHoursList 5017
#define TimeMorningHours24List 5018
#define TimeDayHoursList 5019
#define TimeDayHours24List 5020
#define TimeNightHoursList 5021
#define TimeNightHours24List 5022
#define TimeSmallMinutesList 5023
#define TimeCustomSignalCheck 5024
#define TimeDatePop    5025
#define TimeDateList   5026
#define TimeAlarmAtLabel 5027
#define TimeRepeatLabel 5028
#define TimeCustomSignalLabel 5029
#define TimeSnoozeUntilLabel 5030

#define TimeFormHelpString    5050
#define TimeSnoozeTitleString 5051
#define TimeSnoozeHelpString  5052

#define SignalForm           5100
#define SignalOKButton       5101
#define SignalCancelButton   5102
#define SignalFormHelpString 5110
#define SignalPop            5120
#define SignalList           5121
#define VibrateCheck         5122
#define LEDCheck             5123
#define VibrateNotSupported  5124
#define LEDNotSupported      5125

#define RepeatForm                             5200
#define RepeatNone                             5201
#define RepeatHourly                           5202
#define RepeatDaily                            5203
#define RepeatWeekly                           5204
#define RepeatMonthly                          5205
#define RepeatYearly                           5206
#define RepeatEveryLabel                       5207
#define RepeatFrequenceField                   5208
#define RepeatHoursLabel                       5209
#define RepeatDaysLabel                        5210
#define RepeatWeeksLabel                       5211
#define RepeatMonthsLabel                      5212
#define RepeatYearsLabel                       5213
#define RepeatEndOnLabel                       5214
#define RepeatEndOnTrigger                     5215
#define RepeatEndOnList                        5216
#define RepeatRepeatOnLabel                    5217
#define RepeatDayOfWeek1PushButton             5218
#define RepeatDayOfWeek2PushButton             5219
#define RepeatDayOfWeek3PushButton             5220
#define RepeatDayOfWeek4PushButton             5221
#define RepeatDayOfWeek5PushButton             5222
#define RepeatDayOfWeek6PushButton             5223
#define RepeatDayOfWeek7PushButton             5224
#define RepeatByLabel                          5225
#define RepeatByDayPushButton                  5226
#define RepeatByDatePushButton                 5227
#define RepeatDescRectGadget                   5240
#define RepeatDescField                        5228
#define RepeatNoRepeatLabel                    5229
#define RepeatOkButton                         5230
#define RepeatCancelButton                     5231
#define RepeatHelpString                       5232

#define RecordDetailsForm         5300
#define RecordDetailsNameField    5310
#define RecordDetailsCategoryPop  5311
#define RecordDetailsCategoryList 5312
#define RecordDetailsPrivateCheck 5313
#define RecordDetailsOKButton     5320
#define RecordDetailsCancelButton 5321
#define RecordDetailsNoteButton   5322
#define RecordDetailsPriority1Button 5323
#define RecordDetailsPriority2Button 5324
#define RecordDetailsPriority3Button 5325
#define RecordDetailsPriority4Button 5326
#define RecordDetailsPriority5Button 5327
#define RecordDetailsHelpString   5330

/* the maximum length of a record name */
#define TITLE_MAX_LEN             32

// Default Categories
#define DefaultCategories 5400

// Strings
#define SunString       5601
#define MonString       5602
#define TueString       5603
#define WedString       5604
#define ThuString       5605
#define FriString       5606
#define SatString       5607
#define CountdownString 5610
#define SingleDayString 5611
#define MultiDayString  5612
#define TodayString     5613
#define TomorrowString  5614
#define NoCountdownString 5615
#define NoAlarmString   5616
#define OffString       5617
#define AbsoluteString  5618
#define CustomString    5619
#define MinString       5620
#define HourString      5621
#define DayString       5622
#define WeekString      5623
#define SelectDateString 5624
#define MinsString      5625
#define HoursString     5626
#define DaysString      5627
#define WeeksString     5628
#define NoUndoString    5653
#define BeamTypeString	5654
#define BeamContentString 5655
#define LaunchString    5656

// These should stay in consecutive order
#define NoActionString	   5657
#define NewReminderString  5658
#define NextSketchString   5659
#define NextCategoryString 5660
#define ListViewString     5661
#define DetailViewString   5662

// for MIDI alarm sounds
#define MIDINotSelectedString 5670

// for Note dialog
#define UntitledNoteTitleString 5671

// for Global Find
#define FindHeaderString  5672

// for snoozing
#define SnoozeString      5673

// for flood fill
#define FloodFillMessageString 5674

// These four should stay in consecutive order
#define XferMenuOptionsStrings 5700
#define XferGotoString         5700
#define XferCompletedString    5701
#define XferBackLinkString     5702
#define XferDeleteSketchString 5703

#define XferNoTextString    5705
#define XferSavingString    5706

// Help strings (58xx)
#define DiddleBugHelpString		5800
#define PrefHelpString 			5801
#define ShapeHelpString			5802
#define ConfirmDeleteHelpString	        5803
#define ConfirmLockedDeleteHelpString   5804
#define ConfirmClearHelpString	        5805
#define ConfirmFillHelpString	        5806
#define ConfirmUndoHelpString	        5807
#define LockAskHelpString		5808
#define TextHelpString			5809
#define PrefHWHelpString		5810
#define PrefAlarmHelpString             5811
#define DiddleListHelpString            5812
#define DiddleThumbnailHelpString       5813
#define DiddleThumbnailDetailHelpString 5814
#define DeleteCorruptedSketchHelpString 5815

// Strings
// These six should stay in consecutive order
#define repeatTypesNoneString	       200
#define repeatTypesHourlyString        201
#define repeatTypesDailyString         202
#define repeatTypesWeeklyString        203
#define repeatTypesMonthlyByDayString  204
#define repeatTypesMonthlyByDateString 205
#define repeatTypesYearlyString        206

#define hourlyRepeatString                              127     // repeat description template
#define dailyRepeatString				128	// description template
#define weeklyRepeat1DayString				129	// repeat description template
#define weeklyRepeat2DayString				130	// repeat description template
#define weeklyRepeat3DayString				131	// repeat description template
#define weeklyRepeat4DayString				132	// repeat description template
#define weeklyRepeat5DayString				133	// repeat description template
#define weeklyRepeat6DayString				134	// repeat description template
#define weeklyRepeat7DayString				135	// repeat description template
#define monthlyByDayRepeatString			136	// repeat description template
#define monthlyByDateRepeatString		        137	// repeat description template
#define yearlyRepeatString				138	// repeat description template
#define freqOrdinalsString				139	// 1st, 2nd, 3rd, ..., 99st
#define weekOrdinalsString				140	// 1st, 2nd, 3rd, 4th, and last
#define dayOrdinalsString				141	// 1st - 31st
#define repeatNoneString				142	// "No Repeat"
#define everyHourRepeatString                           143
#define everyDayRepeatString				144	// "Every day"
#define everyWeekRepeat1DayString			145	// "Every week on <DOW name>"
#define everyWeekRepeat2DayString			146	// "Every week on <DOW name> and <DOW name>"
#define everyWeekRepeat3DayString			147	//
#define everyWeekRepeat4DayString			148	//
#define everyWeekRepeat5DayString			149	//
#define everyWeekRepeat6DayString			150	//
#define everyWeekRepeat7DayString			151	//
#define everyMonthByDayRepeatString		        152	// "The ^w ^d of every month"
#define everyMonthByDateRepeatString		        153	// "The ^x of every month"
#define everyYearRepeatString				154	// "^m ^x every year"
#define repeatMonthNamesString				155	// "January February..."
#define repeatFullDOWNamesString			156	// "Sunday Monday..."
#define repeatShortDOWNamesString			157	// "Sun Mon..."
#define endDateTitleString                              158     // "Ending On"

// String lists
#define freqOrdinal2ndStrlID				100	// 2nd (other)

#define AlarmForm       6000
#define AlarmOkButton   6001
#define AlarmDeleteButton 6002
#define AlarmDiddleBugButton 6003
#define AlarmSnoozePop  6004
#define AlarmSnoozeList 6005
#define AlarmBigOkButton 6006
#define AlarmNoteButton 6007
#define AlarmClockGadget 6008
#define CustomTimeForm  7000
#define CustomHourList  7001
#define CustomMinuteList 7002
#define CustomSecondList 7003
#define CustomOkButton  7004
#define CustomCancelButton 7005
#define CustomTimeHelpString 7006
#define IndexForm           8000
#define ConfirmDeleteForm   8300
#define ConfirmClearForm    8301
#define BadSnoozeForm   8302
#define ConfirmLockedDelete 8303
#define ConfirmFillForm    8304
#define ConfirmUndoForm    8305
#define SetPrivateAlert 8306
#define NoOpenError     8400
#define LockAskForm     8500
#define AlarmSetError   8600
#define RomIncompatibleAlert 8700
#define NoPluginSelected 8750
#define NoPluginInstalled 8751
#define PluginError     8752
#define GenericError    8753
#define AlarmTimeTooEarly 8754
#define SnoozeTimeTooEarlyAlert 8756
#define DeleteCorruptedSketchAlert 8757
#define AboutForm       8800
#define AboutBitmapGadget 8801
#define AboutFormHelpString 8802
#define TextForm        9100
#define TextField       9101
#define TextModePop     9102
#define TextModeList    9103
#define TextStdFontPush 9104
#define TextBoldFontPush    9105
#define TextLargeFontPush   9106
#define TextLargeBoldFontPush 9107
#define TextInsertButton    9120
#define TextCancelButton    9121
#define ShapeForm       	9200
// these 4 should stay consecutive
#define ShapeRectPush		9201
#define ShapeRoundRectPush 	9202
#define ShapeCirclePush		9203
#define ShapeOvalPush		9204
// these 4 also
#define ShapeOutlinePush	9205
#define ShapeFilledPush		9206
#define ShapeFilledXorPush	9207
#define ShapeOutlineXorPush	9208

#define ShapeInsertButton       9220
#define ShapeCancelButton       9221

#define PrefForm                     9300
#define PrefSwitchTitleCheck         9301
#define PrefConfirmClearCheck        9302
#define PrefConfirmDeleteCheck       9303
#define PrefDisableTitlebarDroplists 9304
#define PrefBigOkCheck               9305
#define PrefKeepDrawToolCheck	     9306
#define PrefArtistModeCheck	     9307
#define PrefSwipeField		     9308
#define PrefChangeDepthCheck         9309
#define PrefNewRecordOnStartCheck    9310
#define PrefOKButton          	     9318
#define PrefCancelButton	     9319
#define PrefHWForm		     9320
#define PrefHardwareSelTrig          9321
#define PrefHardwareActionList       9322
#define PrefHardwareNonePush         9323
#define PrefHardware1Bitmap	     9330
#define PrefHardware2Bitmap	     9331
#define PrefHardware3Bitmap	     9332
#define PrefHardware4Bitmap	     9333
#define PrefHardware1SelTrig	     9335
#define PrefHardware2SelTrig	     9336
#define PrefHardware3SelTrig	     9337
#define PrefHardware4SelTrig	     9338
#define PrefAlarmForm                9350
#define RemindPop                    9351
#define RemindList                   9352
#define RemindDelayPop               9353
#define RemindDelayList              9354
#define AlarmTimesPop                9355
#define AlarmTimesList               9356
#define RemindMeSignalPop            9357
#define RemindMeSignalList           9358
#define DeleteNoteAlert              9400
#define PrefExtForm                  9500
#define PrefExtDateTimeStampCheck    9502
#define PrefExtKeepCategoryCheck     9503
#define PrefExtDefaultPriorityPop    9504
#define PrefExtDefaultPriorityList   9505
#define PrefExtCreateLastHardButtonCheck 9506
#define PrefExtCreateLastIconCheck   9507
#define PrefExtForegroundColorGadget 9508
#define PrefExtBackgroundColorGadget 9509
#define PrefExtNeverOverwriteTitleCheck 9510
#define PrefExtHelpString            9550

/* 
** IDs > 9999 are reserved for internal use by PalmOS.
*/ 

// #define MENU_FIRST_ITEMID           4000
#define menuitemID_PenFine          4000
#define menuitemID_PenMedium        4001
#define menuitemID_PenBold          4002
#define menuitemID_PenBroad         4003
#define menuitemID_PenFineItalic    4004
#define menuitemID_PenMediumItalic  4005
#define menuitemID_PenBroadItalic   4006
#define menuitemID_InkWhite         4007
#define menuitemID_InkDith12        4008
#define menuitemID_InkDith25        4009
#define menuitemID_InkDith37        4010
#define menuitemID_InkDith50        4011
#define menuitemID_InkDith62        4012
#define menuitemID_InkDith75        4013
#define menuitemID_InkDith87        4014
#define menuitemID_InkBlack         4015
#define menuitemID_InkRandom        4016
#define menuitemID_ModePaint        4017
#define menuitemID_ModeSmear        4018
#define menuitemID_ModeErase        4019
#define menuitemID_CmdRough         4020
#define menuitemID_CmdSmooth        4021
#define menuitemID_CmdSmoother      4022
#define menuitemID_CmdAbout         4023
#define menuitemID_CmdPref          4024
#define menuitemID_CmdClear         4025
#define menuitemID_CmdFill          4026
#define menuitemID_CmdInvert        4027
#define menuitemID_CmdRemove        4028
#define menuitemID_CmdTitleOn       4029
#define menuitemID_CmdNew           4030
#define menuitemID_CmdPrevious      4031
#define menuitemID_CmdNext          4032
#define menuitemID_CmdText          4033
#define menuitemID_CmdLine          4034
#define menuitemID_CmdShape         4035
#define menuitemID_CmdInsertLast    4036
#define menuitemID_CmdLock          4037
#define menuitemID_CmdTransfer      4038
#define menuitemID_CmdSketchUndo    4039
#define menuitemID_CmdBeam          4040
#define menuitemID_CmdUnlock	    4041
#define menuitemID_CmdHelp	    4042
#define menuitemID_CmdDuplicate	    4043
#define menuitemID_CmdTitleOff      4044
#define menuitemID_CmdHWPref	    4045
#define menuitemID_CmdAlarmPref     4046
#define menuitemID_CmdDetails       4047
#define menuitemID_CmdListView      4048
#define menuitemID_CmdSecurity      4049
#define menuitemID_CmdExtPref       4050
#define menuitemID_CmdFloodFill     4051
#define menuitemID_CmdSortByAlarm   4052
#define menuitemID_CmdSortByPriority 4053
#define menuitemID_CmdToolbarOn     4054
#define menuitemID_CmdToolbarOff    4055
#define menuitemID_CmdSend          4056
#define menuitemID_CmdSortByName    4057
#define menuitemID_CmdSketchCopy    4058
#define menuitemID_CmdSketchPaste   4059
#define menuitemID_CmdSketchCut     4060
// #define MENU_LAST_ITEMID            4060
// #define MENU_NUM_ITEMS               61

// New for version 2.0 transfer menu
#define menuitemID_XferDone         4100
#define menuitemID_XferPhoneLookup  4110
#define menuitemID_XferUndo         10000 /* PalmOS standard Edit menu item */
#define menuitemID_XferCut          10001 /* PalmOS standard Edit menu item */
#define menuitemID_XferCopy         10002 /* PalmOS standard Edit menu item */
#define menuitemID_XferPaste        10003 /* PalmOS standard Edit menu item */
#define menuitemID_XferSelectAll    10004 /* PalmOS standard Edit menu item */
#define menuitemID_XferKeyboard     10006 /* PalmOS standard Edit menu item */
#define menuitemID_XferGHelp        10007 /* PalmOS standard Edit menu item */
