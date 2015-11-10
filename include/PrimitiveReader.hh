/*
 * PrimitiveReader.hh
 *
 *  Created on: Nov 9, 2015
 *      Author: nlurkin
 */

#ifndef INCLUDE_PRIMITIVEREADER_HH_
#define INCLUDE_PRIMITIVEREADER_HH_

#include <TString.h>
#include "TPrimitive.hh"

class TChain;

namespace NA62Analysis {
namespace Core {

class PrimitiveReader {
public:
	PrimitiveReader(TString detName);
	virtual ~PrimitiveReader();

	bool AddFile(TString fileName);
	TPrimitive* findMatchingPrimitive(int timeStamp, short fineTime);
	std::vector<TPrimitive> findAllPrimitiveInMatchingWindow(int timeStamp, short fineTime);

private:
	PrimitiveReader() :
		fCurrentPrimitiveID(0), fL0MatchingWindow(0), fTree(nullptr), fCurrentPrimitive(nullptr){
	};
	TPrimitive* findClosestToTimeStamp(int timeStamp, short fineTime, TPrimitive* p1, TPrimitive *p2);
	TPrimitive* checkPrimitiveDeltaAndMoveTree(int timeStamp, short fineTime, TPrimitive* p);

	int fCurrentPrimitiveID;
	float fL0MatchingWindow; // in ns
	TChain *fTree;
	TPrimitive *fCurrentPrimitive;
};

} /* namespace Core */
} /* namespace NA62Analysis */

#endif /* INCLUDE_PRIMITIVEREADER_HH_ */
