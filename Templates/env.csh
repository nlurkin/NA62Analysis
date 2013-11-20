setenv ANALYSISFW_PATH $$ANALYSISFW$$
setenv ANALYSISFW_USERDIR $$USERDIR$$
setenv NA62MCSOURCE $$NA62MCSOURCE$$

setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${ANALYSISFW_PATH}/lib
setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:${ANALYSISFW_USERDIR}/lib
setenv PATH ${PATH}:${ANALYSISFW_PATH}

if (!($?FWSHELL)) then
        source $ANALYSISFW_PATH/scripts/env.csh
endif
