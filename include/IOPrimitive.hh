/*
 * IOPrimitive.hh
 *
 *  Created on: Nov 10, 2015
 *      Author: nlurkin
 */

#ifndef INCLUDE_IOPRIMITIVE_HH_
#define INCLUDE_IOPRIMITIVE_HH_

#include <TString.h>

#include "containers.hh"

namespace NA62Analysis {
namespace Core {

class PrimitiveReader;

class IOPrimitive {
public:
	IOPrimitive();
	virtual ~IOPrimitive();

	void AddReader(TString detName, bool sorted);
	void SetFile(TString fileName);

	PrimitiveReader* GetReader(TString detName);
private:
	NA62Map<TString, PrimitiveReader*>::type fReaderList;
};

} /* namespace Core */
} /* namespace NA62Analysis */

#endif /* INCLUDE_IOPRIMITIVE_HH_ */
