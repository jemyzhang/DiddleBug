## Makefile for IBPlug Diddle

TARGET = ibplug_diddle
APPNAME = "IBPlug Diddle"
APPID = "IBPx"

VERSION = "1.3"
SED = sed

OBJS = $(TARGET).o
LIBS =

CC = m68k-palmos-gcc
REAL_CC = gcc

CFLAGS = -Wall -g -O2 -palmos5r4

PILRC = pilrc

OBJRES = m68k-palmos-obj-res
NM = m68k-palmos-nm
BUILDPRC = build-prc -t BooG -v 2
PILOTXFER = pilot-xfer

all: $(TARGET).prc

install: all
	cp $(TARGET).prc ../../plugins

install-$(LANG): $(TARGET)-$(LANG).prc
	cp $(TARGET)-$(LANG).prc ../../plugins-$(LANG)

.S.o:
	$(CC) $(TARGETFLAGS) -c $<

.c.s:
	$(CC) $(CSFLAGS) $<

$(TARGET).prc: $(REPLACE_PRCTYPE) $(REPLACE_PRCVERSION) code0000.$(TARGET).grc code0001.$(TARGET).grc data0000.$(TARGET).grc pref0000.$(TARGET).grc rloc0000.$(TARGET).grc bin.res
	$(BUILDPRC) $(TARGET).prc $(APPNAME) $(APPID) code0001.$(TARGET).grc code0000.$(TARGET).grc data0000.$(TARGET).grc *.bin pref0000.$(TARGET).grc rloc0000.$(TARGET).grc

$(TARGET)-$(LANG).prc: $(REPLACE_PRCTYPE) $(REPLACE_PRCVERSION) code0000.$(TARGET).grc code0001.$(TARGET).grc data0000.$(TARGET).grc pref0000.$(TARGET).grc rloc0000.$(TARGET).grc bin-$(LANG).res
	$(BUILDPRC) $(TARGET)-$(LANG).prc $(APPNAME) $(APPID) code0001.$(TARGET).grc code0000.$(TARGET).grc data0000.$(TARGET).grc *.bin pref0000.$(TARGET).grc rloc0000.$(TARGET).grc

code0000.$(TARGET).grc: $(TARGET)
	$(OBJRES) $(TARGET)

code0001.$(TARGET).grc: code0000.$(TARGET).grc

data0000.$(TARGET).grc: code0000.$(TARGET).grc

pref0000.$(TARGET).grc: code0000.$(TARGET).grc

rloc0000.$(TARGET).grc: code0000.$(TARGET).grc

bin.res: $(TARGET).rcp
	$(SED) -e 's/^VERSION .*/VERSION 1 $(VERSION)/' $(TARGET).rcp > $(TARGET).rcp.bak && cp $(TARGET).rcp.bak $(TARGET).rcp
	$(PILRC) $(TARGET).rcp .
	mv Tbmp1965.bin BooG1965.bin
	mv Tbmp1966.bin BooG1966.bin
	mv tSTR1967.bin BooG1967.bin
	mv tSTR1968.bin BooG1968.bin
	mv tSTR1969.bin BooG1969.bin
	touch bin.res

bin-$(LANG).res: $(TARGET)-$(LANG).rcp
	-rm -f *.res
	$(SED) -e 's/^VERSION .*/VERSION 1 $(VERSION)/' $(TARGET)-$(LANG).rcp > $(TARGET)-$(LANG).rcp.bak && cp $(TARGET)-$(LANG).rcp.bak $(TARGET)-$(LANG).rcp
	$(PILRC) $(TARGET)-$(LANG).rcp .
	mv Tbmp1965.bin BooG1965.bin
	mv Tbmp1966.bin BooG1966.bin
	mv tSTR1967.bin BooG1967.bin
	mv tSTR1968.bin BooG1968.bin
	mv tSTR1969.bin BooG1969.bin
	touch bin-$(LANG).res

$(TARGET)-$(LANG).rcp:
	$(SED) -f ../plugin-$(LANG).sed $(TARGET).rcp > $(TARGET)-$(LANG).rcp

replace_prctype: replace_prctype.c
	$(REAL_CC) -o $@ replace_prctype.c

replace_prcversion: replace_prcversion.c
	$(REAL_CC) -o $@ replace_prcversion.c

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LIBS)

send: $(TARGET).prc
	$(PILOTXFER) -i $(TARGET).prc

depend:
	makedepend -Y -I. *.c


clean:
	-rm -f *.[oa] *~ $(TARGET) $(TARGET)-*.rcp *.bin *.res *.grc

veryclean: clean
	-rm -f *.prc *.bak replace_prctype replace_prcversion

