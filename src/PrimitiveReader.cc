/*
 * PrimitiveReader.cc
 *
 *  Created on: Nov 9, 2015
 *      Author: nlurkin
 */

#include "PrimitiveReader.hh"

#include <TChain.h>

#include "TVEvent.hh" //For ClockPeriod

namespace NA62Analysis {
namespace Core {

static double FineTimePeriod = ClockPeriod / 256.;

PrimitiveReader::PrimitiveReader(TString detName) :
		fCurrentPrimitiveID(-1), fL0MatchingWindow(0), fTree(
				new TChain(detName)), fCurrentPrimitive(new TPrimitive) {
	fTree->SetBranchAddress("Primitive", &fCurrentPrimitive);
}

PrimitiveReader::~PrimitiveReader() {
}

bool PrimitiveReader::AddFile(TString fileName) {
	fTree->AddFile(fileName);

	return true;
}

TPrimitive* PrimitiveReader::findMatchingPrimitive(int timeStamp,
		short fineTime) {

	TPrimitive previousPrimitive;
	while (fCurrentPrimitive->GetTimeStamp() < timeStamp
			&& fCurrentPrimitiveID < fTree->GetEntries()) {
		previousPrimitive = *fCurrentPrimitive;
		fTree->GetEntry(++fCurrentPrimitiveID);
	}

	//Reached end of file: if the current is close enough, it's the matching one
	if (fCurrentPrimitiveID == fTree->GetEntries())
		return checkPrimitiveDeltaAndMoveTree(timeStamp, fineTime,
				fCurrentPrimitive);

	//Found first primitive with timestamp at or after event timestamp
	//If after, check which one is closest between the previous and the current: that's the matching one
	TPrimitive* tempPrimitive;
	if (fCurrentPrimitive->GetTimeStamp() > timeStamp) {
		tempPrimitive = findClosestToTimeStamp(timeStamp, fineTime,
				fCurrentPrimitive, &previousPrimitive);
		return checkPrimitiveDeltaAndMoveTree(timeStamp, fineTime,
				tempPrimitive);
	}

	//If at, we can now run over the finetime to find the closest
	while (fCurrentPrimitive->GetTimeStamp() == timeStamp
			&& fCurrentPrimitive->GetFineTime() < fineTime
			&& fCurrentPrimitiveID < fTree->GetEntries()) {
		previousPrimitive = *fCurrentPrimitive;
		fTree->GetEntry(++fCurrentPrimitiveID);
	}

	//Reached end of file, if the current is close enough, it's the matching one
	if (fCurrentPrimitiveID == fTree->GetEntries())
		return checkPrimitiveDeltaAndMoveTree(timeStamp, fineTime,
				fCurrentPrimitive);

	//Found the first primitive with finetime at or above the event finetime.
	//If at, we found it.
	if (fCurrentPrimitive->GetFineTime() == fineTime)
		return checkPrimitiveDeltaAndMoveTree(timeStamp, fineTime,
				fCurrentPrimitive);

	//If after, find the closest to the event: that's the matching one, if close
	//enough
	tempPrimitive = findClosestToTimeStamp(timeStamp, fineTime,
			fCurrentPrimitive, &previousPrimitive);
	return checkPrimitiveDeltaAndMoveTree(timeStamp, fineTime, tempPrimitive);
}

std::vector<TPrimitive> PrimitiveReader::findAllPrimitiveInMatchingWindow(
		int timeStamp, short fineTime) {
	std::vector<TPrimitive> listPrim;

	int deltaTS = fL0MatchingWindow / ClockPeriod;
	short deltaFT = (((double) fL0MatchingWindow / ClockPeriod) - deltaTS)
			/ 256.;

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

	while (fCurrentPrimitive->GetTimeStamp() < minTimeStamp
			&& fCurrentPrimitiveID < fTree->GetEntries()) {
		fTree->GetEntry(++fCurrentPrimitiveID);
	}

	//If reached end of file: check if the last primitive is in the window
	//Cannot go further so return the vector anyway
	TPrimitive* tempPrimitive;
	if (fCurrentPrimitiveID == fTree->GetEntries()) {
		tempPrimitive = checkPrimitiveDeltaAndMoveTree(timeStamp, fineTime,
				fCurrentPrimitive);
		if (tempPrimitive)
			listPrim.push_back(*tempPrimitive);
		return listPrim;
	}

	//Found first primitive with timestamp at or after event minTimeStamp
	//If at, go through the fineTime to find the first one in the window
	while (fCurrentPrimitive->GetTimeStamp() == minTimeStamp
			&& fCurrentPrimitive->GetFineTime() < minFineTime
			&& fCurrentPrimitiveID < fTree->GetEntries()) {
		fTree->GetEntry(++fCurrentPrimitiveID);
	}

	//If reached end of file: check if the last primitive is in the window
	//Cannot go further so return the vector anyway
	if (fCurrentPrimitiveID == fTree->GetEntries()) {
		tempPrimitive = checkPrimitiveDeltaAndMoveTree(timeStamp, fineTime,
				fCurrentPrimitive);
		if (tempPrimitive)
			listPrim.push_back(*tempPrimitive);
		return listPrim;
	}

	//At this point we have the first primitive after the beginning of
	//the window.)

	//We can now go through the next primitives until we are outside of
	//the window.
	while (fCurrentPrimitive->GetTimeStamp() < maxTimeStamp
			|| (fCurrentPrimitive->GetTimeStamp() == maxTimeStamp
					&& fCurrentPrimitive->GetFineTime() <= maxFineTime)) {
		listPrim.push_back(*fCurrentPrimitive);
		fTree->GetEntry(++fCurrentPrimitiveID);
	}

	return listPrim;
}

TPrimitive* PrimitiveReader::findClosestToTimeStamp(int timeStamp,
		short fineTime, TPrimitive* p1, TPrimitive* p2) {

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

TPrimitive* PrimitiveReader::checkPrimitiveDeltaAndMoveTree(int timeStamp,
		short fineTime, TPrimitive* p) {
	//No primitive, return nullptr
	if (p == nullptr)
		return nullptr;

	int deltaTS1(abs(p->GetTimeStamp() - timeStamp) * ClockPeriod);
	double deltaFT1(abs(p->GetFineTime() - fineTime) * FineTimePeriod);

	//Primitive too far away from trigger, not good
	if ((deltaTS1 + deltaFT1) > fL0MatchingWindow)
		return nullptr;

	//Close enough. Move the tree current entry on this primitive
	//If the checked primitive is not the Current, it must be the previous,
	//move the tree back by one.
	//Else checked primitive is the Current one and the tree current entry
	//is already good
	if (p != fCurrentPrimitive)
		fTree->GetEntry(--fCurrentPrimitiveID);
	return fCurrentPrimitive;
}

} /* namespace Core */
} /* namespace NA62Analysis */
