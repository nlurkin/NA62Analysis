/*
 * PrimitiveReader.cc
 *
 *  Created on: Nov 9, 2015
 *      Author: nlurkin
 */

#include "PrimitiveReader.hh"

#include <iostream>

#include <TTree.h>
#include <TTreeIndex.h>
#include <TFile.h>

#include "TVEvent.hh" //For ClockPeriod
#include "TPrimitive.hh"

namespace NA62Analysis {
namespace Core {

static double FineTimePeriod = ClockPeriod / 256.;

PrimitiveReader::PrimitiveReader(TString detName, bool sorted) :
		Verbose((detName + "Prim").Data()), fUseSorted(sorted), fCurrentPrimitiveID(
				-1), fL0MatchingWindow(0), fDetName(detName), fTree(nullptr), fCurrentPrimitive(
				new TPrimitive) {
	/// \MemberDescr
	/// \param detName: Name of the detector that should be read
	/// \param sorted: Do the primitives need sorting or not?
	///
	/// Constructor.
	/// \EndMemberDescr
}

PrimitiveReader::~PrimitiveReader() {
	/// \MemberDescr
	/// Destructor
	/// \EndMemberDescr
}

bool PrimitiveReader::SetFile(TString fileName) {
	/// \MemberDescr
	/// \param fileName: Path to a primitives ROOT file
	/// \return True if the file was successfully read
	///
	/// Set a primitives ROOT file and read the detector TTree
	/// \EndMemberDescr

	TFile *fd = TFile::Open(fileName, "R");

	if(!fd->IsOpen()){
		std::cout << normal() << "Unable to open primitives file " << fileName << std::endl;
		return false;
	}

	fTree = static_cast<TTree*>(fd->Get(fDetName));

	if(!fTree){
		std::cout << normal() << "Unable to find TTree " << fDetName << " in file " << fileName << std::endl;
		return false;
	}

	fTree->SetBranchAddress("fPrimitive", &fCurrentPrimitive);

	return true;
}

TPrimitive* PrimitiveReader::FindMatchingPrimitive(int timeStamp,
		short fineTime) {
	/// \MemberDescr
	/// \param timeStamp: Event timestamp
	/// \param fineTime: Event fine time
	/// \return Pointer to the primitive corresponding to the event if found, else nullptr
	///
	/// Read the primitives and return the primitive closest to the event time.
	/// If the primitive found is too far away from the event (outside of the
	/// L0MatchingWindow around the event), the primitive is discarded.
	/// \EndMemberDescr

	std::cout << trace() << "FindMatchingPrimitive(" << timeStamp << ","
			<< fineTime << ")" << std::endl;

	std::cout << trace() << "Skipping primitive until TS>=" << timeStamp
			<< std::endl;
	TPrimitive previousPrimitive;
	while (fCurrentPrimitive->GetTimeStamp() < timeStamp
			&& fCurrentPrimitiveID < fTree->GetEntries()) {
		previousPrimitive = *fCurrentPrimitive;
		fTree->GetEntry(GetNextPrimitiveID());
	}

	//Reached end of file: if the current is close enough, it's the matching one
	if (fCurrentPrimitiveID == fTree->GetEntries()) {
		std::cout << debug() << "Reached EOF without finding matching primitive"
				<< std::endl;
		std::cout << trace() << "Last primitive reached: " << fCurrentPrimitive
				<< std::endl;
		return CheckPrimitiveDeltaAndMoveTree(timeStamp, fineTime,
				fCurrentPrimitive);
	}

	//Found first primitive with timestamp at or after event timestamp
	//If after, check which one is closest between the previous and the current: that's the matching one
	TPrimitive* tempPrimitive;
	if (fCurrentPrimitive->GetTimeStamp() > timeStamp) {
		std::cout << debug()
				<< "Found no primitive at TS. Checking surrounding primitives."
				<< std::endl;
		std::cout << trace() << "Comparing " << previousPrimitive << " && "
				<< fCurrentPrimitive << std::endl;
		tempPrimitive = FindClosestToTimeStamp(timeStamp, fineTime,
				fCurrentPrimitive, &previousPrimitive);
		std::cout << trace() << "Closest: " << tempPrimitive << std::endl;
		return CheckPrimitiveDeltaAndMoveTree(timeStamp, fineTime,
				tempPrimitive);
	}

	std::cout << debug() << "Skipping primitives until FT>=" << fineTime
			<< std::endl;
	//If at, we can now run over the finetime to find the closest
	while (fCurrentPrimitive->GetTimeStamp() == timeStamp
			&& fCurrentPrimitive->GetFineTime() < fineTime
			&& fCurrentPrimitiveID < fTree->GetEntries()) {
		previousPrimitive = *fCurrentPrimitive;
		fTree->GetEntry(GetNextPrimitiveID());
	}

	//Reached end of file, if the current is close enough, it's the matching one
	if (fCurrentPrimitiveID == fTree->GetEntries()) {
		std::cout << debug() << "Reached EOF without finding matching primitive"
				<< std::endl;
		std::cout << trace() << "Last primitive reached: " << fCurrentPrimitive
				<< std::endl;
		return CheckPrimitiveDeltaAndMoveTree(timeStamp, fineTime,
				fCurrentPrimitive);
	}

	//Found the first primitive with finetime at or above the event finetime.
	//If at, we found it.
	if (fCurrentPrimitive->GetFineTime() == fineTime) {
		std::cout << debug() << "Found the exact primitive" << std::endl;
		return CheckPrimitiveDeltaAndMoveTree(timeStamp, fineTime,
				fCurrentPrimitive);
	}

	//If after, find the closest to the event: that's the matching one, if close
	//enough
	std::cout << debug()
			<< "Found no primitive at TS. Checking surrounding primitives."
			<< std::endl;
	std::cout << trace() << "Comparing " << previousPrimitive << " && "
			<< fCurrentPrimitive << std::endl;
	tempPrimitive = FindClosestToTimeStamp(timeStamp, fineTime,
			fCurrentPrimitive, &previousPrimitive);
	std::cout << trace() << "Closest: " << tempPrimitive << std::endl;
	return CheckPrimitiveDeltaAndMoveTree(timeStamp, fineTime, tempPrimitive);
}

std::vector<TPrimitive> PrimitiveReader::FindAllPrimitiveInMatchingWindow(
		int timeStamp, short fineTime) {
	/// \MemberDescr
	/// \param timeStamp: Event timestamp
	/// \param fineTime: Event fine time
	/// \return Vector of all the Primitives found close to the Event time
	///
	/// Read the primitives and return the list of primitives within a time
	/// window of L0MatchingWindow around the event time.
	/// \EndMemberDescr

	std::vector<TPrimitive> listPrim;

	std::cout << trace() << "FindAllPrimitiveInMatchingWindow(" << timeStamp
			<< "," << fineTime << ")" << std::endl;

	int deltaTS = fL0MatchingWindow / ClockPeriod;
	short deltaFT = (((double) fL0MatchingWindow / ClockPeriod) - deltaTS)
			* 256.;

	int minTimeStamp = timeStamp - deltaTS;
	int minFineTime = fineTime - deltaFT;

	if (minFineTime < 0) {
		minFineTime = minFineTime + 256;
		minTimeStamp--;
	}

	int maxTimeStamp = timeStamp + deltaTS;
	int maxFineTime = fineTime + deltaFT;

	if (maxFineTime > 256) {
		maxFineTime = maxFineTime - 256;
		maxTimeStamp++;
	}

	std::cout << trace() << "L0 Matching Window goes from (" << minTimeStamp
			<< "," << minFineTime << ") to (" << maxTimeStamp << ","
			<< maxFineTime << ")" << std::endl;

	std::cout << trace() << "Skipping primitive until TS>=" << minTimeStamp
			<< std::endl;
	while (fCurrentPrimitive->GetTimeStamp() < minTimeStamp
			&& fCurrentPrimitiveID < fTree->GetEntries()) {
		fTree->GetEntry(GetNextPrimitiveID());
	}

	//If reached end of file: check if the last primitive is in the window
	//Cannot go further so return the vector anyway
	TPrimitive* tempPrimitive;
	if (fCurrentPrimitiveID == fTree->GetEntries()) {
		std::cout << debug() << "Reached EOF without finding matching primitive"
				<< std::endl;
		std::cout << trace() << "Last primitive reached: " << fCurrentPrimitive
				<< std::endl;
		tempPrimitive = CheckPrimitiveDeltaAndMoveTree(timeStamp, fineTime,
				fCurrentPrimitive);
		if (tempPrimitive)
			listPrim.push_back(*tempPrimitive);
		return listPrim;
	}

	//Found first primitive with timestamp at or after event minTimeStamp
	//If at, go through the fineTime to find the first one in the window
	std::cout << trace() << "Skipping primitive until FT>=" << minFineTime
			<< std::endl;
	while (fCurrentPrimitive->GetTimeStamp() == minTimeStamp
			&& fCurrentPrimitive->GetFineTime() < minFineTime
			&& fCurrentPrimitiveID < fTree->GetEntries()) {
		fTree->GetEntry(GetNextPrimitiveID());
	}

	//If reached end of file: check if the last primitive is in the window
	//Cannot go further so return the vector anyway
	if (fCurrentPrimitiveID == fTree->GetEntries()) {
		std::cout << debug() << "Reached EOF without finding matching primitive"
				<< std::endl;
		std::cout << trace() << "Last primitive reached: " << fCurrentPrimitive
				<< std::endl;
		tempPrimitive = CheckPrimitiveDeltaAndMoveTree(timeStamp, fineTime,
				fCurrentPrimitive);
		if (tempPrimitive)
			listPrim.push_back(*tempPrimitive);
		return listPrim;
	}

	//At this point we have the first primitive after the beginning of
	//the window.)

	//We can now go through the next primitives until we are outside of
	//the window.
	std::cout << debug() << "Adding primitives until (TS, FT)>=("
			<< maxTimeStamp << "," << maxFineTime << ")" << std::endl;

	while (fCurrentPrimitive->GetTimeStamp() < maxTimeStamp
			|| (fCurrentPrimitive->GetTimeStamp() == maxTimeStamp
					&& fCurrentPrimitive->GetFineTime() <= maxFineTime)) {
		std::cout << trace() << "Adding primitive " << fCurrentPrimitive
				<< std::endl;
		listPrim.push_back(*fCurrentPrimitive);
		fTree->GetEntry(GetNextPrimitiveID());
	}

	return listPrim;
}

TPrimitive* PrimitiveReader::FindClosestToTimeStamp(int timeStamp,
		short fineTime, TPrimitive* p1, TPrimitive* p2) {
	/// \MemberDescr
	/// \param timeStamp: Event timestamp
	/// \param fineTime: Event fine time
	/// \param p1: Pointer to first primitive to compare
	/// \param p2: Pointer to second primitive to compare
	/// \return Of p1 and p2, pointer to the primitive closest to the given time
	///
	/// Computes the &Delta;T between p1 and the given time and between p2 and
	/// the given time and returns the one with smallest &Delta;T.
	/// Starts by converting timestamp in ns, then the FineTime in ns and sum them.
	/// \EndMemberDescr

	//fineTime = [0...256] 25ns/256
	int deltaTS1(abs(p1->GetTimeStamp() - timeStamp) * ClockPeriod);
	int deltaTS2(abs(p2->GetTimeStamp() - timeStamp) * ClockPeriod);

	double deltaFT1(abs(p1->GetFineTime() - fineTime) * FineTimePeriod);
	double deltaFT2(abs(p2->GetFineTime() - fineTime) * FineTimePeriod);

	if ((deltaTS1 + deltaFT1) > (deltaTS2 + deltaFT2))
		return p2;
	else
		return p1;

}

TPrimitive* PrimitiveReader::CheckPrimitiveDeltaAndMoveTree(int timeStamp,
		short fineTime, TPrimitive* p) {
	/// \MemberDescr
	/// \param timeStamp: Event timestamp
	/// \param fineTime: Event fine time
	/// \param p: Pointer to primitive to check
	/// \return Pointer to the current primitive in the TTree if close enough
	/// to the event time (within L0MatchingWindow), or nullptr if too far away.
	///
	/// Check if the primitive is within the L0MatchingWindow around the given time.
	/// If yes, move the TTree read pointer (current primitive) to the given primitive.
	/// Else don't move the TTree and return nullptr.
	/// \EndMemberDescr
	//No primitive, return nullptr
	if (p == nullptr)
		return nullptr;

	double deltaTS1(abs(p->GetTimeStamp() - timeStamp) * ClockPeriod);
	double deltaFT1(abs(p->GetFineTime() - fineTime) * FineTimePeriod);

	//Primitive too far away from trigger, not good
	std::cout << debug() << "Checking primitive-event time delta" << std::endl;
	std::cout << trace() << "Delta: " << deltaTS1 << "+" << deltaFT1 << "="
			<< (double) ((double) deltaTS1 + (double) deltaFT1)
			<< " L0MatchingWindow:" << fL0MatchingWindow << std::endl;
	if ((deltaTS1 + deltaFT1) > fL0MatchingWindow) {
		std::cout << debug()
				<< "Primitive outside of L0MatchingWindow, skipping..."
				<< std::endl;
		return nullptr;
	}

	//Close enough. Move the tree current entry on this primitive
	//If the checked primitive is not the Current, it must be the previous,
	//move the tree back by one.
	//Else checked primitive is the Current one and the tree current entry
	//is already good
	if (p != fCurrentPrimitive)
		fTree->GetEntry(GetPreviousPrimitiveID());
	std::cout << trace() << "Returning primitive " << fCurrentPrimitive
			<< std::endl;
	return fCurrentPrimitive;
}

void PrimitiveReader::SetL0MatchingWindowWidth(float ns) {
	/// \MemberDescr
	/// \param ns: L0MatchingWindow to set in ns
	///
	/// Set the L0Matching window. The window starts from ns ns before the event time
	/// and ends ns ns after the event time.
	/// \EndMemberDescr
	fL0MatchingWindow = ns;
}

void PrimitiveReader::SetL0MatchingWindowWidth(int timeStamp, short fineTime) {
	/// \MemberDescr
	/// \param timeStamp: L0MatchingWindow to set in timeStamp units
	/// \param fineTime: L0MatchingWindow to set in FineTime units
	///
	/// Set the L0Matching window. The window starts from (timeStamp+fineTime/256.) timestamps
	/// units before the event time and ends (timeStamp+fineTime/256.) timestamps units
	/// after the event time.
	/// \EndMemberDescr
	fL0MatchingWindow = timeStamp * ClockPeriod + fineTime * FineTimePeriod;
}

Long64_t PrimitiveReader::GetNextPrimitiveID() {
	/// \MemberDescr
	/// \return Next entry in the tree.
	///
	/// Selects the next entry in the tree. If not sorted, simply the next entry in the tree.
	/// If sorted, tree entry corresponding to the next index.
	/// \EndMemberDescr

	++fCurrentPrimitiveID;
	if (fUseSorted) {
		TTreeIndex *index = static_cast<TTreeIndex*>(fTree->GetTreeIndex());
		if(!index){
			fTree->BuildIndex("fPrimitive.fTimeStamp", "fPrimitive.fFineTime");
			index = static_cast<TTreeIndex*>(fTree->GetTreeIndex());
		}
		return index->GetIndex()[fCurrentPrimitiveID];
	} else
		return fCurrentPrimitiveID;
}

Long64_t PrimitiveReader::GetPreviousPrimitiveID() {
	/// \MemberDescr
	/// \return Previous entry in the tree.
	///
	/// Selects the previous entry in the tree. If not sorted, simply the previous entry in the tree.
	/// If sorted, tree entry corresponding to the previous index.
	/// \EndMemberDescr

	--fCurrentPrimitiveID;
	if (fUseSorted) {
		TTreeIndex *index = static_cast<TTreeIndex*>(fTree->GetTreeIndex());
		if(!index){
			fTree->BuildIndex("fPrimitive.fTimeStamp", "fPrimitive.fFineTime");
			index = static_cast<TTreeIndex*>(fTree->GetTreeIndex());
		}
		return index->GetIndex()[fCurrentPrimitiveID];
	} else
		return fCurrentPrimitiveID;
}

} /* namespace Core */
} /* namespace NA62Analysis */

std::ostream& operator <<(std::ostream& s, TPrimitive p) {
	s << "(" << p.GetTimeStamp() << "," << p.GetFineTime() << ")";
	return s;
}

std::ostream& operator <<(std::ostream& s, TPrimitive *p) {
	s << "(" << p->GetTimeStamp() << "," << p->GetFineTime() << ")";
	return s;
}

