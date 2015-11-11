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
#include "Verbose.hh"

class TChain;

namespace NA62Analysis {
namespace Core {

class PrimitiveReader : public Verbose {
public:
	PrimitiveReader(TString detName);
	virtual ~PrimitiveReader();

	bool AddFile(TString fileName);
	TPrimitive* FindMatchingPrimitive(int timeStamp, short fineTime);
	std::vector<TPrimitive> FindAllPrimitiveInMatchingWindow(int timeStamp, short fineTime);

	void SetL0MatchingWindowWidth(float ns);
	void SetL0MatchingWindowWidth(int timeStamp, short fineTime);

private:
	PrimitiveReader() :
		fCurrentPrimitiveID(0), fL0MatchingWindow(0), fTree(nullptr), fCurrentPrimitive(nullptr){
	};
	TPrimitive* FindClosestToTimeStamp(int timeStamp, short fineTime, TPrimitive* p1, TPrimitive *p2);
	TPrimitive* CheckPrimitiveDeltaAndMoveTree(int timeStamp, short fineTime, TPrimitive* p);

	int fCurrentPrimitiveID;
	float fL0MatchingWindow; // in ns
	TChain *fTree;
	TPrimitive *fCurrentPrimitive;
};

} /* namespace Core */
} /* namespace NA62Analysis */

std::ostream& operator<<(std::ostream &s, TPrimitive p);
std::ostream& operator<<(std::ostream &s, TPrimitive *p);

#endif /* INCLUDE_PRIMITIVEREADER_HH_ */
