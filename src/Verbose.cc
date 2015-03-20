/*
 * Verbose.cc
 *
 *  Created on: Mar 20, 2015
 *      Author: ncl
 */

#include "Verbose.hh"

#include <iostream>
#include <iomanip>

namespace NA62Analysis {

Verbose::Verbose() :
	fVerbosityTest(Verbosity::kNo),
	fModuleName("[ NA62Analysis ]"),
	fCurrentStream(&std::cout)
{
	/// \MemberDescr
	/// Constructor
	/// \EndMemberDescr
}

Verbose::Verbose(std::string name) :
	fVerbosityTest(Verbosity::kNo),
	fModuleName("[ " + name + " ]"),
	fCurrentStream(&std::cout)
{
	/// \MemberDescr
	/// \param name : Module display name
	///
	/// Constructor with name
	/// \EndMemberDescr
}

Verbose::~Verbose() {
	/// \MemberDescr
	/// Destructor
	/// \EndMemberDescr
}

void Verbose::SetVerbosity(Verbosity::VerbosityLevel v){
	/// \MemberDescr
	/// \param v : Verbosity value
	///
	/// Change verbosity
	/// \EndMemberDescr

	std::cout << user() << "Setting verbosity to " << v << std::endl;
	fVerbosityLevel = v;
}

const Verbose& operator<<(const Verbose &level, std::ostream& (*f)(std::ostream&)) {
	if(level.CanPrint()) level.GetStream() << f;
	return level;
};

const Verbose& operator <<(std::ostream& s, const Verbose &level) {
	level.SetStream(s);
	if(level.CanPrint()){
		s << std::left << std::setw(6) << Verbose::GetVerbosityLevelName(level.GetTestLevel())
		  << " - " << std::setw(15) << level.GetModuleName() << " ";
	}
	return level;
}

bool Verbose::TestLevel(Verbosity::VerbosityLevel level) const {
	if(level<=fVerbosityLevel) return true;
	else return false;
}

bool Verbose::CanPrint() const {
	if(fVerbosityTest<=fVerbosityLevel) return true;
	else return false;
}

std::string Verbose::GetVerbosityLevelName(Verbosity::VerbosityLevel v) {
	if(v==Verbosity::kUser) return "USER";
	else if(v==Verbosity::kNormal) return "NORMAL";
	else if(v==Verbosity::kDebug) return "DEBUG";
	else if(v==Verbosity::kExtended) return "EXTEND";
	return ""; //Do not happen
}

Verbosity::VerbosityLevel Verbose::fVerbosityLevel = Verbosity::kNo;
} /* namespace NA62Analysis */
