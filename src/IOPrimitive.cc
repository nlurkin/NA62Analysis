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
	fReaderList.insert(std::pair<TString,PrimitiveReader*>(detName,new PrimitiveReader(detName, sorted)));
}

void IOPrimitive::SetFile(TString fileName) {
	for(auto it : fReaderList)
		it.second->SetFile(fileName);
}

PrimitiveReader* IOPrimitive::GetReader(TString detName) {
	NA62Map<TString, PrimitiveReader*>::type::iterator it;

	if((it=fReaderList.find(detName))!=fReaderList.end()) return it->second;

	return nullptr;
}

} /* namespace Core */
} /* namespace NA62Analysis */
