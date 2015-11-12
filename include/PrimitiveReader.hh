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

class TTree;

namespace NA62Analysis {
namespace Core {

/// \class PrimitiveReader
/// \Brief
/// Class reading a primitives ROOT file and matching primitives to Event time.
/// \EndBrief
///
/// \Detailed
/// Class reading a primitives ROOT file and matching primitives to Event time. Provides
/// a method to request the primitive closest to the given (timestamp,finetime) and a method
/// to request all the primitives within a window around the given (timestamp, finetime).
/// \EndDetailed
class PrimitiveReader : public Verbose {
public:
	PrimitiveReader(TString detName, bool sorted=false);
	virtual ~PrimitiveReader();

	bool SetFile(TString fileName);
	TPrimitive* FindMatchingPrimitive(int timeStamp, short fineTime);
	std::vector<TPrimitive> FindAllPrimitiveInMatchingWindow(int timeStamp, short fineTime);

	void SetL0MatchingWindowWidth(float ns);
	void SetL0MatchingWindowWidth(int timeStamp, short fineTime);

private:
	PrimitiveReader() :
		fUseSorted(false), fCurrentPrimitiveID(0), fL0MatchingWindow(0), fTree(nullptr), fCurrentPrimitive(nullptr){
	};
	TPrimitive* FindClosestToTimeStamp(int timeStamp, short fineTime, TPrimitive* p1, TPrimitive *p2);
	TPrimitive* CheckPrimitiveDeltaAndMoveTree(int timeStamp, short fineTime, TPrimitive* p);
	Long64_t GetNextPrimitiveID();
	Long64_t GetPreviousPrimitiveID();

	bool fUseSorted; ///< Sort primitives or not?
	Long64_t fCurrentPrimitiveID; ///< ID of current primitive (tree ID if not sorted, index ID if sorted)
	float fL0MatchingWindow; ///< L0 Matching window in ns
	TString fDetName; ///< Name of the detector the primitive are read from
	TTree *fTree; ///< Pointer to the Primitives TTree
	TPrimitive *fCurrentPrimitive; ///< Pointer to the current primitive in the TTree
};

} /* namespace Core */
} /* namespace NA62Analysis */

std::ostream& operator<<(std::ostream &s, TPrimitive p);
std::ostream& operator<<(std::ostream &s, TPrimitive *p);

#endif /* INCLUDE_PRIMITIVEREADER_HH_ */
