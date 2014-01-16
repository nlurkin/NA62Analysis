ROOTCONFIG   := $(ROOTSYS)/bin/root-config
ROOTINCDIR   := $(shell $(ROOTCONFIG) --incdir)
ROOTLIBS     := $(shell $(ROOTCONFIG) --libs) -lMinuit -lGeom -lTMVA -lEG

#ifndef G4INCLUDE 
#$(error G4INCLUDE not defined)
#endif
ifndef NA62MCSOURCE
$(error NA62MCSOURCE not defined)
endif 

#FW
FWPath 		:= $$FWPath$$
UserPath 	:= $$USERCODEPATH$$

#Directories
MAIN_INCLUDE	= $(FWPath)/include
AN_INCLUDE		= $(FWPath)/Analyzers/include
EXAMPLE_INCLUDE	= $(FWPath)/Examples/include
FWOBJDIR		= $(FWPath)/obj
FWLIBDIR		= $(FWPath)/lib
USR_INCLUDE		= $(UserPath)/Analyzers/include
USR_SRC			= $(UserPath)/Analyzers/src
USROBJDIR		= $(UserPath)/obj
USRLIBDIR		= $(UserPath)/lib

#FW definitions
MAIN		= Analyzer DetectorAcceptance functions BaseAnalysis MCSimple EventFraction ConfigParser StringTable StringBalancedTable ParticleTree 
MAINHH		= $(addprefix $(MAIN_INCLUDE)/,$(addsuffix .hh,$(MAIN)))
MAINLIBS	= -lAnalysisFW

#Analyzers definitions
ANALYZERS	= $$ANALYZERSHH$$
EXAMPLES	= $$EXAMPLEHH$$
ANALYZERSHH = $(addprefix $(AN_INCLUDE)/,$(addsuffix .hh,$(ANALYZERS)))
ANALYZERSHH += $(addprefix $(EXAMPLE_INCLUDE)/,$(addsuffix .hh,$(EXAMPLES)))
ANALYZERSO 	= $(addprefix $(FWOBJDIR)/,$(addsuffix .o,$(ANALYZERS)))
ANALYZERSO 	+= $(addprefix $(FWOBJDIR)/,$(addsuffix .o,$(EXAMPLES)))
ANALYZERSLIBS= $(addprefix -l,$(ANALYZERS))
ANALYZERSLIBS+= $(addprefix -l,$(EXAMPLES))

USRANS		= $$USRHH$$
USRANSCC 	= $(addprefix $(USR_INCLUDE)/,$(addsuffix .cc,$(USRANS)))
USRANSHH 	+= $(addprefix $(USR_INCLUDE)/,$(addsuffix .hh,$(USRANS)))
USRANSO 	+= $(addprefix $(USROBJDIR)/,$(addsuffix .o,$(USRANS)))
USRANSLIBS 	+= $(addprefix $(USRLIBDIR)/lib,$(addsuffix .so,$(USRANS)))
ANALYZERSLIBS+= $(addprefix -l,$(USRANS))

#include directories
FWHEADERS	= -I$(MAIN_INCLUDE) -I$(AN_INCLUDE) -I$(EXAMPLE_INCLUDE) -I$(USR_INCLUDE)# -I$(G4INCLUDE)
EXTRAHEADERS = $$EXTRAINCLUDEDIRS$$

#NA62
SUBDIRS := RICH LKr Spectrometer GigaTracker LAV IRC CHANTI Cedar CHOD MUV1 MUV2 SAC MUV3 MUV0 HAC

#include directories
NA62HEADERS	+= -I$(NA62MCSOURCE)/Persistency/include 
NA62HEADERS	+= $(addprefix -I$(NA62MCSOURCE)/,$(addsuffix /Persistency/include,$(SUBDIRS)))

# LIBs
NA62LIBDIR	+= -L$(NA62MCSOURCE)/Persistency/lib 
NA62LIBDIR	+= $(addprefix -L$(NA62MCSOURCE)/,$(addsuffix /Persistency/lib,$(SUBDIRS)))
NA62LIBS	+= -lNA62Persistency$(STATIC) $(addprefix -l,$(addsuffix Persistency$(STATIC),$(SUBDIRS)))
EXTRALIBDIR	+= $$EXTRALIBSDIRS$$ #-L$(G4LIB)/$(G4SYSTEM) 
EXTRALIBS	+= $$EXTRALIBS$$ #-lG4particles

#COMPILER
CFLAGS		= -O -Wall -fPIC -g
SOFLAGS		= -shared
CC			= g++

EXEC		= $$EXEC$$

all:	$(EXEC)

test: test_main.cc $(MAINHH) $(USRANSLIBS)
	@echo "Compiling test executable $@ ..."
	@$(CC) -o $@ $< $(CFLAGS) -L$(FWLIBDIR) -L$(USRLIBDIR) $(NA62LIBDIR) $(ANALYZERSLIBS) $(MAINLIBS) $(ROOTLIBS)  $(NA62LIBS) -I$(ROOTINCDIR) $(FWHEADERS) $(NA62HEADERS)

#Compilation de l'executable
$(EXEC): $(USROBJDIR)/main.o $(USRANSLIBS)
	@echo "Compiling executable $(EXEC) ..."
	@$(CC) -o $@ $< $(CFLAGS) -L$(FWLIBDIR) -L$(USRLIBDIR) $(NA62LIBDIR) $(EXTRALIBDIR) $(ANALYZERSLIBS) $(MAINLIBS) $(ROOTLIBS) $(NA62LIBS) $(EXTRALIBS) 

#Compilation des user analyzer
$(USROBJDIR)/%.o: $(USR_SRC)/%.cc $(USR_INCLUDE)/%.hh $(MAINHH)
	@echo "Compiling user analyzer object $@ ..."
	@$(CC) -o $@ -c $< $(CFLAGS) $(FWHEADERS) $(NA62HEADERS) -I$(ROOTINCDIR) $(EXTRAHEADERS)

#Compilation du main
$(USROBJDIR)/main.o: main.cc $(MAINHH) $(USRANSHH)
	@echo "Compiling object $@ ..."
	@$(CC) -o $@ -c $< $(CFLAGS) -I$(ROOTINCDIR) $(FWHEADERS) $(NA62HEADERS) $(EXTRAHEADERS)

#creation des libraries
$(USRLIBDIR)/lib%.so: $(USROBJDIR)/%.o
	@echo "Creating library $@ ..."
	@$(CC) -Wl,-soname,$(patsubst $(USRLIBDIR)/%,%,$@) -o $@ $< $(SOFLAGS) 
	
#Clean
.PHONY: clean cleanall

cleanall:	clean

clean: 
		rm -rf $(USROBJDIR)/*.o
		rm -rf $(USRLIBDIR)/*.so
		rm -rf $(EXEC)
		rm -rf outFile.root