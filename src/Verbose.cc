/*
 * Verbose.cc
 *
 *  Created on: Mar 20, 2015
 *      Author: ncl
 */

#include "Verbose.hh"

#include <iostream>
#include <iomanip>
#include <fstream>

namespace NA62Analysis {

Verbose::Verbose() :
	fLocalVerbosityActive(false),
	fLocalVerbosityLevel(Verbosity::kNo),
	fVerbosityTest(Verbosity::kNo),
	fModuleName("[NA62Analysis]"),
	fCurrentStream(&std::cout)
{
	/// \MemberDescr
	/// Constructor
	/// \EndMemberDescr
}

Verbose::Verbose(std::string name) :
	fLocalVerbosityActive(false),
	fLocalVerbosityLevel(Verbosity::kNo),
	fVerbosityTest(Verbosity::kNo),
	fModuleName("[" + name + "]"),
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
	/// Change local verbosity
	/// \EndMemberDescr

	std::cout << normal() << "Setting verbosity to " << v << std::endl;
	fLocalVerbosityActive = true;
	fLocalVerbosityLevel = v;
}

void Verbose::SetGlobalVerbosity(Verbosity::VerbosityLevel v){
	/// \MemberDescr
	/// \param v : Verbosity value
	///
	/// Change global verbosity
	/// \EndMemberDescr

	std::cout << "Setting global verbosity to " << v << std::endl;
	fVerbosityLevel = v;
}

const Verbose& operator<<(const Verbose &level, std::ostream& (*f)(std::ostream&)) {
	/// \MemberDescr
	/// \param level : Reference to Verbose class
	/// \param f : std::ostream manipulator (endl, flush, ...)
	///
	/// Apply manipulator to output stream if requested verbosity level is at
	/// least equal to the verbosity level.
	/// \EndMemberDescr

	if(level.CanPrint()) level.GetStream() << f;
	return level;
};

const Verbose& operator <<(std::ostream& s, const Verbose &level) {
	/// \MemberDescr
	/// \param s : Output stream
	/// \param level : Reference to Verbose class
	///
	/// Start verbose printing to the specified output stream. Print
	/// requested verbosity level and module name if requested verbosity
	/// level is at least equal to verbosity level.
	/// \EndMemberDescr

	if(Verbose::fLogToFile) level.SetStream(Verbose::fLogFileStream);
	else level.SetStream(s);
	if(level.CanPrint() && level.GetTestLevel()!=Verbosity::kStandard){
		level.GetStream() << std::left << std::setw(6) << Verbose::GetVerbosityLevelName(level.GetTestLevel())
		  << " - " << std::setw(15) << level.GetModuleName() << " ";
	}
	return level;
}

bool Verbose::TestLevel(Verbosity::VerbosityLevel level) const {
	/// \MemberDescr
	/// \param level : Tested verbosity level
	/// \return True if level is at least equal to the verbosity level
	/// \EndMemberDescr

	if(fLocalVerbosityActive && (level<=fLocalVerbosityLevel)) return true;
	else if(!fLocalVerbosityActive && (level<=fVerbosityLevel)) return true;
	else return false;
}

bool Verbose::CanPrint() const {
	/// \MemberDescr
	/// \return True if the currently used verbosity level is at least
	/// equal to the verbosity level.
	/// \EndMemberDescr

	return TestLevel(fVerbosityTest);
}

std::string Verbose::GetVerbosityLevelName(Verbosity::VerbosityLevel v) {
	/// \MemberDescr
	/// \param v : Verbosity level
	/// \return String corresponding to the verbosity level
	/// \EndMemberDescr

	if(v==Verbosity::kUser) return "USER";
	else if(v==Verbosity::kNormal) return "NORMAL";
	else if(v==Verbosity::kExtended) return "EXTEND";
	else if(v==Verbosity::kDebug) return "DEBUG";
	else if(v==Verbosity::kTrace) return "TRACE";
	return ""; //Do not happen
}

Verbosity::VerbosityLevel Verbose::GetVerbosityLevelFromName(TString v) {
	/// \MemberDescr
	/// \param v : String representing a verbosity level
	/// \return Verbosity level corresponding to the string v
	/// \EndMemberDescr

	if(v.CompareTo("kuser", TString::kIgnoreCase)==0) return Verbosity::kUser;
	else if(v.CompareTo("knormal", TString::kIgnoreCase)==0) return Verbosity::kNormal;
	else if(v.CompareTo("kextended", TString::kIgnoreCase)==0) return Verbosity::kExtended;
	else if(v.CompareTo("kdebug", TString::kIgnoreCase)==0) return Verbosity::kDebug;
	else if(v.CompareTo("ktrace", TString::kIgnoreCase)==0) return Verbosity::kTrace;
	return Verbosity::kNormal;
}

void Verbose::SetLogToFile(TString fileName) {
	/// \MemberDescr
	/// \param fileName : Path to the log file
	///
	/// Request redirection of all log messages to log file
	/// \EndMemberDescr

	Verbose::fLogToFile = true;
	fLogFileStream.open(fileName.Data(), std::ofstream::out);
	std::cout << "Logging to file " << fileName << std::endl;
}

Verbosity::VerbosityLevel Verbose::fVerbosityLevel = Verbosity::kNo;
bool Verbose::fLogToFile = false;
std::ofstream Verbose::fLogFileStream;

} /* namespace NA62Analysis */
