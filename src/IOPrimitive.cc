/*
 * IOPrimitive.cc
 *
 *  Created on: Nov 10, 2015
 *      Author: nlurkin
 */

#include "IOPrimitive.hh"

#include "PrimitiveReader.hh"

namespace NA62Analysis {
namespace Core {

IOPrimitive::IOPrimitive() {
}

IOPrimitive::~IOPrimitive() {
}

void IOPrimitive::AddReader(TString detName, bool sorted) {
	/// \MemberDescr
	/// \param detName: Detector name for which the PrimitiveReader should be created
	/// \param sorted: Flag to turn on/off the sorting of primitives
	///
	/// Creates a new PrimitiveReader for the requested detector.
	/// \EndMemberDescr
	fReaderList.insert(std::pair<TString,PrimitiveReader*>(detName,new PrimitiveReader(detName, sorted)));
}

void IOPrimitive::SetFile(TString fileName) {
	/// \MemberDescr
	/// \param fileName: Path to a primitives ROOT file.
	///
	/// Set the given file to all primitive readers.
	/// \EndMemberDescr
	for(auto it : fReaderList)
		it.second->SetFile(fileName);
}

PrimitiveReader* IOPrimitive::GetReader(TString detName) {
	/// \MemberDescr
	/// \param detName: Detector name for which the PrimitiveReader is requested
	/// \return If found, the PrimitiveReader for the specified detector
	/// \EndMemberDescr
	NA62Map<TString, PrimitiveReader*>::type::iterator it;

	if((it=fReaderList.find(detName))!=fReaderList.end()) return it->second;

	return nullptr;
}

} /* namespace Core */
} /* namespace NA62Analysis */
