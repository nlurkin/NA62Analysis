echo "Sourcing $PWD"

if (!($?NA62MCSOURCE)) then
        echo Set NA62MCSOURCE first
else
        setenv FWSHELL csh
        
        source ${NA62MCSOURCE}/scripts/env.csh
        
        if ( "$LD_LIBRARY_PATH" =~ "*Persistency/lib*" ) then
	        setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${NA62MCSOURCE}/RICH/Persistency/lib
	        setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${NA62MCSOURCE}/LKr/Persistency/lib
	        setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${NA62MCSOURCE}/Spectrometer/Persistency/lib
	        setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${NA62MCSOURCE}/GigaTracker/Persistency/lib
	        setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${NA62MCSOURCE}/LAV/Persistency/lib
	        setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${NA62MCSOURCE}/IRC/Persistency/lib
	        setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${NA62MCSOURCE}/CHANTI/Persistency/lib
	        setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${NA62MCSOURCE}/Cedar/Persistency/lib
	        setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${NA62MCSOURCE}/CHOD/Persistency/lib
	        setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${NA62MCSOURCE}/NewCHOD/Persistency/lib
	        setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${NA62MCSOURCE}/MUV1/Persistency/lib
	        setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${NA62MCSOURCE}/MUV2/Persistency/lib
	        setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${NA62MCSOURCE}/SAC/Persistency/lib
	        setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${NA62MCSOURCE}/MUV3/Persistency/lib
	        setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${NA62MCSOURCE}/MUV0/Persistency/lib
	        setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${NA62MCSOURCE}/HAC/Persistency/lib
	        
	        setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${NA62MCSOURCE}/Persistency/lib
        endif
endif

set gccPath=`which gcc`
set temp=`dirname "$gccPath"`
setenv ANALYSISFW_STDLIBSPATH `dirname "$temp"`
setenv CC `which  gcc`
setenv CXX `which g++`
setenv PATH /afs/cern.ch/sw/lcg/contrib/CMake/3.2.3/Linux-x86_64/bin/:${PATH}

