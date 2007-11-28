#
# The basic makefile. The localization specific ones just instantiate "LOC"
#

VERSION=1.0.b.2
DATE=`date -R`
APPID = DBSC

INCLUDES = -I../sony -I../sony/Libraries -I../sony/System

STDCFLAGS = -palmos5.0 -Os $(DEFINES) $(INCLUDES) -Wall

ifeq ($(DEBUG), 0)
	CFLAGS = $(STDCFLAGS)
else
	CFLAGS = -g -mdebug-labels -DDIDDLEDEBUG -DERROR_CHECK_LEVEL=2 $(STDCFLAGS) 
endif

CSFLAGS = -g -O2
GLUEFLAGS = -lPalmOSGlue

MAKE = make
OBJS = diddlesnap.o 
CC = m68k-palmos-gcc

PILRC = pilrc
PILRCOPT = -ro
TXT2BITM = txt2bitm
BUILDPRC = build-prc
MULTIGEN = m68k-palmos-multigen
EXE = DiddleSnap
PRC = $(EXE).prc


all: $(PRC)

.S.o:
	$(CC) $(TARGETFLAGS) -c $<

.c.s:
	$(CC) $(CSFLAGS) $<

$(PRC): $(EXE) bin.ro
	$(BUILDPRC) diddlesnap.def -o $@ $(EXE) bin.ro

bin.ro: diddlesnap.rcp
	$(PILRC) $(PILRCOPT) diddlesnap.rcp bin

$(EXE): $(OBJS) 
	$(CC) $(CFLAGS) -Wl,-Map,linkmap.txt  $(OBJS) -o $@ $(GLUEFLAGS)

install:
	pilot-xfer -i $(PRC)

clean:
	rm -rf *.[oa] $(EXE) bin.ro $(PRC) *.stamp *.bin *.grc

clobber: clean
	rm $(PRC)

