ROOTCONFIG   := $(ROOTSYS)/bin/root-config
ROOTINCDIR   := $(shell $(ROOTCONFIG) --incdir)
ROOTLIBS     := $(shell $(ROOTCONFIG) --libs) -lMinuit -lGeom -lTMVA -lMLP -lEG

#ifndef G4INCLUDE 
#$(error G4INCLUDE not defined)
#endif
ifndef NA62MCSOURCE
$(error NA62MCSOURCE not defined)
endif 


#FW 
#Directories
MAIN_INCLUDE= include
MAIN_SRC	= src
AN_INCLUDE	= Analyzers/include
AN_SRC		= Analyzers/src
EXAMPLE_INCLUDE	= Examples/include
EXAMPLE_SRC	= Examples/src
FWOBJDIR	= obj
FWLIBDIR	= lib

#include directories
FWHEADERS	= -I$(MAIN_INCLUDE) -I$(AN_INCLUDE) -I$(EXAMPLE_INCLUDE)# -I$(G4INCLUDE)

#FW definitions
MAIN		= Analyzer BaseAnalysis DetectorAcceptance MCSimple 	\
				functions NeuralNetwork EventFraction ConfigParser	\
				StringTable StringBalancedTable ParticleTree 		\
				ParticleInterface HistoHandler UserMethods #ROOTDICT
MAINO		= $(addprefix $(FWOBJDIR)/,$(addsuffix .o,$(MAIN)))
MAINLIBS	= $(FWLIBDIR)/libAnalysisFW.so


#Analyzers definitions
ANALYZERS	= $(patsubst $(AN_INCLUDE)/%.hh,%,$(wildcard $(AN_INCLUDE)/*.hh))
EXAMPLES	= $(patsubst $(EXAMPLE_INCLUDE)/%.hh,%,$(wildcard $(EXAMPLE_INCLUDE)/*.hh))

ANALYZERSO	= $(addsuffix .o,$(ANALYZERS))
EXAMPLESO	= $(addsuffix .o,$(EXAMPLES))
ANALYZERSLIBS= $(addprefix $(FWLIBDIR)/lib,$(addsuffix .so,$(ANALYZERS)))
ANALYZERSLIBS+= $(addprefix $(FWLIBDIR)/lib,$(addsuffix .so,$(EXAMPLES)))

#NA62
SUBDIRS := RICH LKr Spectrometer GigaTracker LAV IRC CHANTI Cedar CHOD MUV1 MUV2 SAC MUV3 MUV0 HAC

#include directories
NA62HEADERS	+= -I$(NA62MCSOURCE)/Persistency/include 
NA62HEADERS	+= $(addprefix -I$(NA62MCSOURCE)/,$(addsuffix /Persistency/include,$(SUBDIRS)))

#COMPILER
CFLAGS		= -O -Wall -fPIC -g3
SOFLAGS		= -shared
CC			= g++
 
#all:	$(FWOBJDIR)/ROOTDICT.cc $(MAINLIBS) $(ANALYZERSLIBS)
all:	$(MAINLIBS) $(ANALYZERSLIBS)

example: $(FWLIBDIR)/libTRecoLKrCandidateMC.so

#Compilation des analyzer
$(FWOBJDIR)/%.o: $(AN_SRC)/%.cc $(AN_INCLUDE)/%.hh
	@mkdir -p $(FWOBJDIR)
	@echo "Compiling analyzer object $@ ..."
	@$(CC) -o $@ -c $< $(CFLAGS) $(FWHEADERS) $(NA62HEADERS) -I$(ROOTINCDIR)

#Compilation des examples
$(FWOBJDIR)/%.o: $(EXAMPLE_SRC)/%.cc $(EXAMPLE_INCLUDE)/%.hh
	@mkdir -p $(FWOBJDIR)
	@echo "Compiling example object $@ ..."
	@$(CC) -o $@ -c $< $(CFLAGS) $(FWHEADERS) $(NA62HEADERS) -I$(ROOTINCDIR) -IExamples/TRecoLKrCandidateMC

#Compilation du framework
$(FWOBJDIR)/%.o: $(MAIN_SRC)/%.cc $(MAIN_INCLUDE)/%.hh
	@mkdir -p $(FWOBJDIR)
	@echo "Compiling fw object $@ ..."
	@$(CC) -o $@ -c $< $(CFLAGS) $(FWHEADERS) $(NA62HEADERS) -I$(ROOTINCDIR)

#creation des libraries
$(FWLIBDIR)/libAnalysisFW.so: $(MAINO)
	@mkdir -p $(FWLIBDIR)
	@echo "Creating fw library $@ ..."
	@$(CC) -Wl,-soname,$(patsubst $(FWLIBDIR)/%,%,$@) -o $@ $(SOFLAGS) $(MAINO) $(ROOTLIBS)
	
$(FWLIBDIR)/lib%.so: $(FWOBJDIR)/%.o
	@mkdir -p $(FWLIBDIR)
	@echo "Creating library $@ ..."
	@$(CC) -Wl,-soname,$(patsubst $(FWLIBDIR)/%,%,$@) -o $@ $< $(SOFLAGS) $(ROOTLIBS)

#Dictionnaires ROOT
$(FWOBJDIR)/ROOTDICT.cc: $(MAIN_INCLUDE)/EventFraction.hh $(MAIN_INCLUDE)/LinkDef.h
	@mkdir -p $(FWOBJDIR)
	@echo "Generating dictionary ..."
	@rootcint -f $@ -c $(CFLAGS) $(FWHEADERS) EventFraction.hh LinkDef.h

$(FWOBJDIR)/ROOTDICT.o: $(FWOBJDIR)/ROOTDICT.cc $(MAIN_INCLUDE)/EventFraction.hh
	@mkdir -p $(FWOBJDIR)
	@echo "Compiling dictionary ..."
	@$(CC) -o $@ -c $< $(CFLAGS) $(FWHEADERS) -I$(ROOTINCDIR)

#Clean
.PHONY: clean cleanall

cleanall:	clean

clean: 
	rm -rf $(FWOBJDIR)
	rm -rf $(FWLIBDIR)
	rm -rf *.pyc

#Compilation of additional library for example analyzer
$(FWLIBDIR)/libTRecoLKrCandidateMC.so: $(FWOBJDIR)/TRecoLKrCandidateMCDICT.o $(FWOBJDIR)/TRecoLKrCandidateMC.o
	@echo "Creating library $@ ..."
	@$(CC) -Wl,-soname,libTRecoLKrCandidateMC.so -o $@ $(FWOBJDIR)/TRecoLKrCandidateMCDICT.o $(FWOBJDIR)/TRecoLKrCandidateMC.o $(SOFLAGS) $(ROOTLIBS)

$(FWOBJDIR)/TRecoLKrCandidateMC.o: Examples/TRecoLKrCandidateMC/TRecoLKrCandidateMC.cc Examples/TRecoLKrCandidateMC/TRecoLKrCandidateMC.hh
	@echo "Compiling analyzer object $@ ..."
	@$(CC) -o $@ -c $< $(CFLAGS) $(FWHEADERS) $(NA62HEADERS) -I$(ROOTINCDIR)


#Dictionnaires ROOT
$(FWOBJDIR)/TRecoLKrCandidateMCDICT.cc: Examples/TRecoLKrCandidateMC/TRecoLKrCandidateMC.hh Examples/TRecoLKrCandidateMC/LinkDef.h
	@echo "Generating dictionary for TRecoLKrCandidate ..."
	@rootcint -f $@ -c $(CFLAGS) $(NA62HEADERS) -IExamples/TRecoLKrCandidateMC TRecoLKrCandidateMC.hh LinkDef.h

$(FWOBJDIR)/TRecoLKrCandidateMCDICT.o: $(FWOBJDIR)/TRecoLKrCandidateMCDICT.cc Examples/TRecoLKrCandidateMC/TRecoLKrCandidateMC.hh
	@echo "Compiling dictionary for TRecoLKrCandidate..."
	@$(CC) -o $@ -c $< $(CFLAGS) $(NA62HEADERS) -I$(ROOTINCDIR) -IExamples/TRecoLKrCandidateMC
