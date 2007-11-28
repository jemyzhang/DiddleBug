# Makefile for Link History Hack
# Please read the readme.txt file that came in the Sample Hack package

# Add .bin to the list of suffixes that make recognizes
.SUFFIXES:  .bin

HACK        = dbscHack
HACKNAME    = "DiddleBug ScreenCap"
HACKID      = DBSC  # make sure this matches what's in the source files
HACKSRC     = code03e8.c code07d0.c
HACKRES     = $(HACK).rcp $(HACK)Rsc.h

CFLAGS   = -palmos4.0 -Os -Wall -I../sony -I../sony/Libraries -I../sony/System
GLUEFLAGS = -lPalmOSGlue

# You shouldn't have to change anything below this line

CC      = m68k-palmos-gcc
OBJRES  = m68k-palmos-obj-res

all:    $(HACK).prc

HACKBIN = $(HACKSRC:.c=.bin)

install: $(HACK).prc
	cp $(HACK).prc ../diddleSnapHack.prc

# This rule builds a standalone code resource (.bin) from a C source file
.c.bin:
	$(CC) $(CFLAGS) -nostartfiles -o $(*F) $< $(GLUEFLAGS)
	$(OBJRES) $(*F)
	cp code0001.$(*F).grc $@
	rm $(*F)

$(HACK).prc: hackresbin $(HACKBIN)
	build-prc -H -c $(HACKID) -n $(HACKNAME) -o $@ *.bin

hackresbin: $(HACKRES)
	pilrc $(HACK).rcp
	touch $@

$(HACKBIN): $(HACK).h $(HACK)Rsc.h

clean:
	rm -f *.grc *.bin hackresbin

veryclean: clean
	-rm -f $(HACK).prc

send:
	pilot-xfer -i $(HACK).prc
