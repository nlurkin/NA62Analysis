ROOTCONFIG   := $(ROOTSYS)/bin/root-config
ROOTINCDIR   := $(shell $(ROOTCONFIG) --incdir)
ROOTLIBS     := $(shell $(ROOTCONFIG) --libs) -lMinuit -lGeom -lTMVA -lMLP -lEG

VARS_OLD := $(VARIABLES)

#ifndef G4INCLUDE 
#$(error G4INCLUDE not defined)
#endif
ifndef NA62MCSOURCE
$(error NA62MCSOURCE not defined)
endif 


#FW 
#Directories
MAIN_INCLUDE:= include
MAIN_SRC	:= src
AN_DIR		:= Analyzers
AN_INCLUDE	:= $(filter-out %CMakeLists.txt/include %CMakeLists.txt~/include,$(addsuffix /include,$(wildcard Analyzers/*)))
AN_SRC		:= $(filter-out %CMakeLists.txt/src %CMakeLists.txt~/src,$(addsuffix /src,$(wildcard Analyzers/*)))
EXAMPLE_INCLUDE	:= Examples/include
EXAMPLE_SRC	:= Examples/src
PO_INCLUDE	:= PhysicsObjects/include
PO_SRC		:= PhysicsObjects/src
FWOBJDIR	:= obj
FWLIBDIR	:= lib

#include directories
FWHEADERS	:= -I$(MAIN_INCLUDE) $(addprefix -I,$(AN_INCLUDE)) -I$(EXAMPLE_INCLUDE) -I$(PO_INCLUDE) # -I$(G4INCLUDE)

#FW definitions
MAIN		= Analyzer BaseAnalysis DetectorAcceptance MCSimple 	\
				functions NeuralNetwork EventFraction ConfigParser	\
				StringTable StringBalancedTable ParticleTree 		\
				ParticleInterface HistoHandler UserMethods 			\
				CounterHandler IOHandler #ROOTDICT
MAINO		= $(addprefix $(FWOBJDIR)/,$(addsuffix .o,$(MAIN)))
MAINLIBS	= $(FWLIBDIR)/libAnalysisFW.so


#Analyzers definitions
ANALYZERS	= $(basename $(notdir $(wildcard $(AN_DIR)/*/include/*.hh)))
EXAMPLES	= $(basename $(notdir $(wildcard $(EXAMPLE_INCLUDE)/*.hh)))

ANALYZERSO	= $(addsuffix .o,$(ANALYZERS))
EXAMPLESO	= $(addsuffix .o,$(EXAMPLES))
ANALYZERSLIBS= $(addprefix $(FWLIBDIR)/lib,$(addsuffix .so,$(ANALYZERS)))
ANALYZERSLIBS+= $(addprefix $(FWLIBDIR)/lib,$(addsuffix .so,$(EXAMPLES)))

#PhysicsObjects definitions
PO			= $(basename $(notdir $(wildcard $(PO_INCLUDE)/*.hh)))
POSO		= $(addsuffix .o,$(PO))
POLIBS		= $(addprefix $(FWLIBDIR)/lib,$(addsuffix .so,$(PO)))

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
all:	$(MAINLIBS) $(ANALYZERSLIBS) $(POLIBS)

printall:
	$(foreach v,											\
		$(filter-out $(VARS_OLD) VARS_OLD .VARIABLES, $(.VARIABLES)),	\
		$(info $(v) = $($(v))))


example: $(FWLIBDIR)/libTRecoLKrCandidateMC.so

#Compilation des PO
$(FWOBJDIR)/PO%.o: $(PO_SRC)/%.cc $(PO_INCLUDE)/%.hh
	@mkdir -p $(FWOBJDIR)
	@echo "Compiling PhysicsObjects object $@ ..."
	@$(CC) -o $@ -c $< $(CFLAGS) $(FWHEADERS) $(NA62HEADERS) -I$(ROOTINCDIR)

#Clean
.PHONY: clean cleanall

cleanall:	clean

clean: 
	rm -rf $(FWOBJDIR)
	rm -rf $(FWLIBDIR)
	rm -rf *.pyc

#Dictionnaires ROOT

$(FWOBJDIR)/%DICT.cc: $(PO_INCLUDE)/%.hh $(PO_INCLUDE)/LinkDef.h
	@echo "Generating dictionary for $@ ..."
	@rootcint -f $@ -c $(CFLAGS) $(NA62HEADERS) $(FWHEADERS) $<

$(FWOBJDIR)/%DICT.o: $(FWOBJDIR)/%DICT.cc $(PO_INCLUDE)/%.hh
	@echo "Compiling dictionary for $@ ..."
	@$(CC) -o $@ -c $< $(CFLAGS) $(NA62HEADERS) -I$(ROOTINCDIR) -I.
