ROOT_GENERATE_DICTIONARY(TRecoLKrCandidateMC.hh LinkDef.h TRecoLKrCandidateMCDICT.cc "${ROOT_INCLUDE_DIR};${CMAKE_CURRENT_SOURCE_DIR};${NA62MC_INC_DIRS}")

add_library(TRecoLKrCandidateMC SHARED TRecoLKrCandidateMC.hh TRecoLKrCandidateMC.cc TRecoLKrCandidateMCDICT.cc)