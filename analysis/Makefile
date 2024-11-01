# Simple makefile

EXE=bin/$(study)
MAINDIR=.

SRCDIR=studies/$(study)
OBJDIR=studies/$(study)

SOURCES=$(wildcard $(SRCDIR)/*.cc) $(wildcard tools/*.cc)
OBJECTS=$(SOURCES:%.cc=%.o)
HEADERS=$(SOURCES:.cc=.h)

CORRECTIONLIBDIR=${CMSSW_BASE}/../../../external/py3-correctionlib/2.0.0-0c4f44c8dd5561d8c0660135feeb81f4/lib/python3.9/site-packages/correctionlib

CC          = g++
CXX         = g++
CXXFLAGS    = -g -O2 -Wall -fPIC -Wshadow -Woverloaded-virtual
LD          = g++
SOFLAGS     = -g -shared
CXXFLAGS    = -g -O2 -Wall -fPIC -Wshadow -Woverloaded-virtual
LDFLAGS     = -g -O2 -L$(MAINDIR)/rapido/src -L$(MAINDIR)/NanoTools/NanoCORE -L${CMSSW_BASE}/lib/${SCRAM_ARCH} 
LDFLAGS    += -L$(CORRECTIONLIBDIR)/lib
LDFLAGS    += $(shell root-config --glibs)
ROOTLIBS    = $(shell root-config --libs)
ROOTCFLAGS  = $(shell root-config --cflags)
CXXFLAGS   += $(ROOTCFLAGS)
CFLAGS      = $(ROOTCFLAGS) -Wall -Wno-unused-function -g -O2 -fPIC -fno-var-tracking
CFLAGS     += -I$(MAINDIR)/rapido/src -I$(MAINDIR)/NanoTools/NanoCORE -I${CMSSW_BASE}/src -I$(MAINDIR)/include   # base includes
CFLAGS     += -I${CMSSW_BASE}/../../../external/boost/1.67.0/include                                             # needed for JER tools
CFLAGS     += -I$(CORRECTIONLIBDIR)/include							                                             # correctionlib
EXTRAFLAGS  = -fPIC -ITMultiDrawTreePlayer -Wunused-variable -lTMVA -lEG -lGenVector -lXMLIO -lMLP -lTreePlayer -lImt
EXTRAFLAGS += -lRAPIDO -lNANO_CORE -lCondFormatsJetMETObjects -lJetMETCorrectionsModules -lcorrectionlib

all: $(EXE)

$(EXE): $(OBJECTS)
	$(LD) $(CXXFLAGS) $(LDFLAGS) $(OBJECTS) $(ROOTLIBS) $(EXTRAFLAGS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	$(CC) $(CFLAGS) $< -c -o $@

clean:
	rm -f $(OBJECTS) $(EXE)

.PHONY: all
