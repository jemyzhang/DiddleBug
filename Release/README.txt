******************************************************************************
*                              DiddleBug 2.90.b.3                            *
******************************************************************************

IMPORTANT WARNING: Please have a look at "Upgrading sketches" in the Mini-FAQ

Included files:
==============
DiddleBug-english.prc............English version
DiddleBug-german.prc.............German version
DiddleBug-french.prc.............French version
DiddleBug-tchinese.prc...........Traditional Chinese version
DiddleBug-schinese.prc...........Simplified Chinese version
dbscHack.prc.....................DiddleSnap (Screenshot Hack for PalmOS<=4.x)
DiddleSnap.prc...................DiddleSnap (for PalmOS5)
README.txt.......................This README 
doc/index.htm....................Overview of DiddleBug
doc/manual.htm...................Manual (unfinished)
doc/plugins.htm..................A short guide to IntelliBooger(tm) plugins
doc/faq.htm......................Answers to Frequently Asked Questions
doc/snap.htm.....................Description of DiddleSnap
plugins/*.prc....................IntelliBooger(tm) plugins
plugins-german/*.prc.............German IntelliBooger(tm) plugins
plugins-french/*.prc.............French IntelliBooger(tm) plugins
support/SysZLib-*.prc............Library needed for optional PNG functionality
				 (if you have a PalmOS 5 device, install the
				 -arm version to get optimized code for your
				 platform)


Mini-FAQ:
========
* HandEra 330: use in "scale-to-fit" mode only, screenshot hack not supported
* Garbage titles: created by old beta, you can delete them in Details dialog
* General troubleshooting: do a soft-reset with the "Up" button pressed,
  followed by another soft-reset, and try again - often, this helps. 
* Upgrading on hi-res device converts all the old (lo-res) sketches. This may
  take some time, depending on the speed of your device. To successfully 
  upgrade your sketches, delete DiddleBug if you're currently using v.2.90.b.1
  or v.2.90.b.2. Then, re-install your old DiddleBugDB.pdb file. Now you are
  set up to install DiddleBug v.2.90.b.3. When you start it for the first time,
  it automatically converts all the sketches. This only works with a lo-res
  DiddleBugDB database, not with one modified by one of the previous betas of
  v.2.90.


What's new in 2.90.b.4?
======================
* Full support for Tungsten 5-way navigation (even in list view)
* Full support for Treo 600 (including 5-way navigation)
* Fixed shape drawing on lo-res devices
* Fixed thumbnail generation on hi-res devices
* New "Sort By Name" command
* Custom ink and paper colors
* Support for Acer s50/s60 devices
* Better handling for private records (only the current record is unmasked)
* Fixed broken LED/vibrate alarm on non-Sony devices
* Miscellaneous bugfixes and UI-cleanups
 
What's new in 2.90.b.3?
======================
* Should work on all devices (hi- and lo-res) now
* Screenshot application (DiddleSnap) for OS5 devices added
* DiddleSnap Hack works on hi-res Sony Clie devices
* Lo-res sketches are "upgraded" now
* Use "DiddleBugHRDB" as the name of the sketch database on hi-res devices to
  be compatible with WindleBugHR
* Several bugs on Sony Clie devices fixed
* LED and vibrate alarm support for some Sony Clie devices. 
* Plugins should be able to cope with hi-res now (in theory at least)

What's new in 2.90.b.2?
======================
* support for Sony Clie devices running OS 4.1

What's new in 2.90.b.1?
======================
* fixed crash when zlib is not installed (English version only)
* hi-res support for PalmOS 5

What's new in 2.60?
==================
* editing sketch details returns to list view correctly
* added PNG plugin
* added "Send" functionality for SnapperMail, BlueTooth, ...
* allow corrupted sketches to be deleted (instead of crashing DB)
* supports Tungsten T five-way navigator
* partial support for Sony jog-dial

What's new in 2.55?
==================
* fixed further alarm crash caused by one missing line of code
* doesn't crash anymore when there are no sketches in the DB and
  "create new sketch at start-up" is checked
* fixed possible crash when switching between views with hard button
* "switch view" button also works in the two thumbnail views

What's new in 2.54?
==================
* alarms display correctly on OS5
* a new alarm crash (caused by a typo in the code) fixed

What's new in 2.53?
==================
* new fullscreen mode without toolbar
* added "priorities" for notes (also transfered to To Do)
* fixed mysterious crash when alarms add up without being acknowledged
* "Lock" doesn't affect all records any more in detailed thumbnail list
* separate setting for hardbuttons and menu/icon for "Create after last"
* compatibility fixes for PalmOS 5

What's new in 2.52?
==================
* new optional "thumbnail" views
* new flood fill tool 
* added "sort by alarm time" to list view
* new WordSmith and FireViewer plugins
* better handling of optional clock in alarm form
* DiddleBug uses almost no stack space anymore for the alarms, so various
  mysterious alarm crashes should be gone
* misc. bugfixes

What's new in 2.51?
==================
* new "Absolute..." snooze feature
* optional clock in Alarm dialog
* bugfix for Button Preferences: all applications can be started now
* new "Switch between Sketch/List view" action for Button Preferences
* bugfix for Set Alarm Details dialog/24h mode
* new slide shortcut on titlebar
* new "Create after last" setting for new sketches
* seperate filter setting for "Erase" mode
* new (optional) automatic date-/time-stamp for new sketches
* misc. small bugfixes

What's new in 2.50?
==================
* doesn't try to upgrade the record format on a new install
* fixed a small memory leak

What's new in 2.50.rc.3?
=======================
* fixed Confirm Undo help string
* works in hi-res on the Sony Clie (the sketch itself is still 160x160, though)
* actually plays "Reminder" sound when reminding
* French translation updates

What's new in 2.50.rc.2?
=======================
* fixed crash when setting a record private in non-German versions
* update records again when upgrading

What's new in 2.50.rc.1?
=======================
* better compatibility with old VolumeDA application (and similar tools)
* bugfix in notes handling
* automatically use compatibility mode on Sony Clie (so there should not
  be a need to disable HiRes Assist anymore)
* disable titlebar shortcut popup by default (as it interferes with the
  menu activation on OS >= 3.5)
* in Absolute Alarm... dialog: change alarm date to tomorrow if the selected
  time is before the current time (previous versions of DiddleBug already 
  did this)
* bugfix in New button handling in list view
* small bugfix in Absolute Alarm... dialog
* small bugfix in Repeat dialog
* small bugfix in Delete Sketch handling

What's new in 2.50.b.22?
=======================
* "Shape Ink" is sticky when using "Insert Last"
* fixes for Chrome (frmUpdateEvent is handled correctly now)
* checks for valid index before trying to open the last record
* redesigned alarm signal handling - storing alternate MIDI DBs was impractical
* added "Remind Me Signal"
* added new plugin for Handspring Treo "PhoneBook"

What's new in 2.50.b.21?
=======================
* snooze was broken in b.20: fixed
* new icon for "Paint" mode (the old one was ambiguous for some)
* removed "Smear" mode as no-one really used it
  => "Paint" and "Erase" can be toggled in one click now
* "Note" button always shown now (either filled or unfilled)
* Command Bar updated to better conform with Palm UI guidelines
* clock change detected and handled correctly now
* internal clean-up

What's new in 2.50.b.20?
=======================
* implemented "Remind Me: Never"
* don't crash on first start (after database creation)
* user can select a category upon beam receive
* French translation updates
* French translation for plugins
* works better with Khroma and similar tools now
* various fixes for small bugs detected with POSE + debug ROM

What's new in 2.50.b.19?
=======================
* updated French translation
* MIDI databases with non-standard creator ID work again
* fixed countdown-bug: 10h and 12h were not working
* fixed small bugs in cleanup code
* fixed infinite loop ("crash") with the combination of snooze & repeated alarms

What's new in 2.50.b.18?
=======================
* again sound alarm only after sketch is shown (missing "!" in if-statement)
* don't crash if LocalID of MIDI database has changed between
  setting a signal and the actual alarm (i.e. after a hard reset)
* fixed fill/clear not working correctly with Khroma
* fixed the IntelliBooger (tm) plugins

What's new in 2.50.b.17?
=======================
* fixed Snooze handling
* fixed two memory leaks
* fixed crash when another applications alarm is being displayed
* sound alarm even when another application displays its own alarm form
* play default signal if "custom signal" is checked, but no MIDI file selected

What's new in 2.50.b.16?
=======================
* various small bugfixes
* better conformance to Palm UI guidelines
* finally really fixed snooze behavior

What's new in 2.50.b.15?
=======================
* works in Flash-ROM again
* fixed bug in Undo when Keep Drawing Tools is set
* trigger special alarms before sound (and only do that after showing the alarm)
* fixed snoozing behavior
* added ToDo (DB4) plugin

What's new in 2.50beta14?
========================
* new alarm handling system, fixing all of the bugs with the old one
* new IntelliBooger (tm) plugins: Doc, pedit, pedit32, peditPro 
  (courtesy J.D.Hedden)
* categories are handled in all the "Memo"-like plugins
  (dito)
* French translation

What's new in 2.50beta13?
========================
* repeat alarms several times if not acknowledged
* improved help texts
* support for "private" records
* correctly upgrade from old versions
* cache application list for hardware button preferences
* play sample when selection alarm sounds

What's new in 2.50beta12?
========================
* "Global Find" support
* new "Switch Category" option for hardware buttons
* fixes crash when opening dialogs from List View
* fixes bug in Alarm Details dialog
* includes IntelliBooger plugins

What's new in 2.50beta11?
========================
* List view
* misc. bugfixes

What's new in 2.50beta10?
========================
* hourly repeats supported again
* browsing past the first/last sketch works reliably now
* category preserved when duplicating records
* after changing a record's category, switch display to new one
* "Note" button in title bar
* "Page" button displays a list of all records now

What's new in 2.50beta9?
=======================
* updated to work with Khroma
* Record Details dialog (you can set a name & a note)
* categories
* nicer behavior if no MIDI sounds are found
* fixes Preferences crash with Countdown enabled
* Graffiti shortcut for popping up the alarm list (for pScript compatibility)
* various fixes for the HandEra 330
* Phone Lookup feature for Transfer mode

What's new in 2.50beta8?
=======================
* crash in Chinese versions fixed
* alarm behavior fixed (all the right MIDI files are played,
  and you also see the alarm immediately)
* no confirm dialog when deleting empty images
* don't decrease index when deleting an image unless
  it would be > number of remaining images  

What's new in 2.50beta7?
=======================
* the crash when opening the "Set Alarm Details" dialog is really fixed 
* Chinese versions

What's new in 2.50beta6?
=======================
* MIDI sound is actually played now
* doesn't crash anymore when opening the "Set Alarm Details" dialog

What's new in 2.50beta5?
=======================
* uses MIDI files for alarms now
* supports LED and Vibrate alarms
* custom alarms for individual records
* overhauled repeat dialog/repeat handling
  => existing repeats will not be honored at the moment,
     though they will be converted in the final release
* small internal fixes (should make DiddleBug more robust
  with regards to API changes)

What's new in 2.50beta4?
=======================
* new Keep Drawing Tool preference option
* new Artist Mode preference option (randomizes ink before each stroke)
* bugfixes for 16bit mode
* works in ScaledToFit mode on HandEra 330!
* works in flash-ROM again (definitely this time!)
* cosmetic fixes for Transfer Done button
* some assorted bugfixes, as usual

What's new in 2.50beta3?
=======================
* works in all screenmodes (color, grayscale and b/w)
* cosmetic updates for grayscale/color devices (buttons use the system colors now)
* simplified menus (experimental)
* should work in flash-ROM again (not tested)
* menu in transfer-mode can be opened with tap on titlebar
* ScreenShot Hack works again (up to 8 bit color mode, 16 bit not yet supported)
* several updates to German translation

What's new in 2.49b?
===================
* German version
* help added to Preferences and Insert Shape dialogs
* navigation forward and backward via Graffitti-space/Graffitti-backspace
  and forward/backward swipes on the "page button" (the button labeled "x/z")
* lock/unlock via menus improved
* various small bugfixes

What's new in 2.49a?
===================
This version of DiddleBug should work on a lot more devices, notably color (IIIc, ...) and HandEra 330 (in "Center" mode => it works in "scale to fit" mode as long as you don't attempt to change any sketch, or create a new one)

Oh, and it only works on OS 3.5 and above. Use DiddleBug 2.15 if you've got a Palm still running one of the earlier OSes.


Homepage
========

The DiddleBug homepage can be found at http://diddlebug.sourceforge.net
