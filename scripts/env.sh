echo "Sourcing $PWD"

if [ -z $NA62MCSOURCE ]; then
        echo Set NA62MCSOURCE first
else
        export FWSHELL=sh

        source $NA62MCSOURCE/scripts/env.sh
        
        if [[ ! $LD_LIBRARY_PATH =~ Persistency/lib ]]; then
        	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$NA62MCSOURCE/RICH/Persistency/lib
        	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$NA62MCSOURCE/LKr/Persistency/lib
        	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$NA62MCSOURCE/Spectrometer/Persistency/lib
        	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$NA62MCSOURCE/GigaTracker/Persistency/lib
        	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$NA62MCSOURCE/LAV/Persistency/lib
        	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$NA62MCSOURCE/IRC/Persistency/lib
        	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$NA62MCSOURCE/CHANTI/Persistency/lib
        	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$NA62MCSOURCE/Cedar/Persistency/lib
        	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$NA62MCSOURCE/CHOD/Persistency/lib
        	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$NA62MCSOURCE/NewCHOD/Persistency/lib
        	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$NA62MCSOURCE/MUV1/Persistency/lib
        	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$NA62MCSOURCE/MUV2/Persistency/lib
        	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$NA62MCSOURCE/SAC/Persistency/lib
        	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$NA62MCSOURCE/MUV3/Persistency/lib
        	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$NA62MCSOURCE/MUV0/Persistency/lib
        	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$NA62MCSOURCE/HAC/Persistency/lib
        
        	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$NA62MCSOURCE/Persistency/lib
        fi
fi

gccPath=`which gcc`
export ANALYSISFW_STDLIBSPATH="$(dirname $(dirname "$gccPath"))"
export CC=`which gcc`
export CXX=`which g++`
export PATH=/afs/cern.ch/sw/lcg/contrib/CMake/3.2.3/Linux-x86_64/bin/:$PATH

