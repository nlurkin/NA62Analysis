export ANALYSISFW_PATH=$$ANALYSISFW$$
export ANALYSISFW_USERDIR=$$USERDIR$$
export NA62MCSOURCE=$$NA62MCSOURCE$$

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ANALYSISFW_PATH/lib
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ANALYSISFW_USERDIR/lib
export PATH+=:$ANALYSISFW_PATH

if [ -z $FWSHELL ]; then
        source $ANALYSISFW_PATH/scripts/env.sh
fi
