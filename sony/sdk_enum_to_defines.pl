#!/usr/bin/env perl
#Time-stamp: <2002-03-22 11:08:36 rattles>
#Author: Ronny Iversen <ronny@falch.net>
# - Will convert CLIE sdk files to use #defines instead of typedefs,
# - since current prc tools does not handle traps based on enums very
# - well.
#
#Modified by Erik Gillespie <rattles@yakko.cs.wmich.edu>
# - Will now fix the SonySndLib.h header file
# - Replaced the "(sysLibTrapCustom+0)" with just "sysLibTrapCustom"
#
#Usage: start the program using perl sony_conv.pl in the same directory as
#SonyCLIE.h the file is found. This will produce
#Libraries/SonyMsaLib_new.h and Libraries/SonyHRLib_new.h.  Then store
#the old Libraries/SonyMsaLib.h and Libraries/SonyHRLib.h a safe place
#before renaming the _new files to replace the old ones.


while(@ARGV) {
    $option = shift(@ARGV);
    if($option eq "-file") {
	$file = shift(@ARGV);
    } elsif($option eq "-outputfile") {
	$outputfile = shift(@ARGV);
    } elsif($option eq "-enum_start_string") {
	$enum_start_string = shift(@ARGV);
    } elsif($option eq "-enum_stop_string") {
	$enum_stop_string = shift(@ARGV);
    }
}

#default calls if no parameters are given.
if($file && $outputfile && $enum_start_string && $enum_stop_string) {
    \&sdk_enums_to_defines($file, $outputfile, $enum_start_string, $enum_stop_string);
} elsif($file || $outputfile || $enum_start_string || $enum_stop_string) {
    die "Usage: <-file \"filename\"> <-outputfile \"outputfilename\"> <-enum_start_string \"the first line of the enum\"> <-enum_stop_string \"the last line of the enum\">";
} else { #default
    #cleans up the sony clie sdk
    \&sdk_enums_to_defines("Libraries/SonyMsaLib.h", "Libraries/SonyMsaLib_new.h", "typedef enum {", "} MsaLibTrapNumberEnum;", 0);
    \&sdk_enums_to_defines("Libraries/SonyHRLib.h", "Libraries/SonyHRLib_new.h", "typedef enum tagHRTrapNumEnum", "} HRTrapNumEnum;", 0);
    \&sdk_enums_to_defines("Libraries/SonyRmcLib.h", "Libraries/SonyRmcLib_new.h", "typedef enum {", "} RmcLibTrapNumberEnum;", 0);
    \&sdk_enums_to_defines("Libraries/SonySilkLib.h", "Libraries/SonySilkLib_new.h", "typedef enum {", "} SilkLibTrapNumberEnum;", 0);
    \&sdk_enums_to_defines("Libraries/SonyJpegUtilLib.h", "Libraries/SonyJpegUtilLib_new.h", "typedef enum {", "} JpegUtilLibTrapNumberEnum;", 0);
    \&sdk_enums_to_defines("Libraries/SonySndLib.h", "Libraries/SonySndLib_new.h", "typedef enum {", "", 0);
    
    #cleans up the kypcera sdk
    \&sdk_enums_to_defines("pdQCore.h", "pdQCore_new.h", "typedef enum {", "} PDQCoreLibTrapEnum;", 10);
    \&sdk_enums_to_defines("pdQRegistry.h", "pdQRegistry_new.h", "typedef enum {", "} PDQRegTrapNumberEnum;", 0);
}

#the function that does it all
sub sdk_enums_to_defines {    
    my($file) = shift;
    my($outputfile) = shift;
    my($enum_start_string) = shift;
    my($enum_stop_string) = shift;
    my($instance) = shift;

    open(INPUT_FILE, "$file") or return;
    @work_arr =  <INPUT_FILE>;
    close(INPUT_FILE);
    
    open(OUTPUT_FILE, ">$outputfile");
    $found = 0;
    $parsing = 0;
    $count = 0;

    print "\n>>> Converting $file to $outputfile.\n";
    print ">>> $enum_start_string\n";
    print ">>> $enum_stop_string\n";

    $mush = 0;
    foreach $line(@work_arr) {
	# begin the parsing
	if(!$found && $line =~ /$enum_start_string/) {	
	    if($mush++ == $instance) {
		print ">>> enum_start_string $enum_start_string found, starting to parse..\n";
		$found = 1;
		$parsing = 1;
	    }
	}

	if(!$parsing) {
	    print OUTPUT_FILE $line;
	} else {
	    if($line =~ /^\t\w/) {
		$new_line = $line;
		chop($new_line);
		chop($new_line);
		if($new_line !~ /$\,/) {
		    $new_line .= ",";
		}
		if ($new_line =~ /=/) {
		    $new_line =~ s/=/ /;
		}
		else {
		    $new_line =~ s/[\, ]/ \(sysLibTrapCustom\+$count\) \/\//;
		    $count++;
		}
		$new_line =~ s/\t/\#define\t/;
		print OUTPUT_FILE "$new_line\n";
	    } else {
		print OUTPUT_FILE "\t//$line";
	    }
	}
	
	#stop the parsing
	if($found && $line =~ /$enum_stop_string/) {
	    print ">>> enum_stop_string $enum_stop_string found, parsing stopped.\n";
	    $parsing = 0;
	}
    }
}
