/*
 * SettingsReader.cc
 *
 *  Created on: Mar 19, 2015
 *      Author: ncl
 */

#include <SettingsReader.hh>

#include <iostream>

namespace NA62Analysis {
namespace Configuration {

SettingsReader::SettingsReader() {
	/// \MemberDescr
	/// Default constructor
	/// \EndMemberDescr
}

SettingsReader::~SettingsReader() {
	/// \MemberDescr
	/// Default destructor
	/// \EndMemberDescr
}

void SettingsReader::ParseFile(TString fileName) {
	/// \MemberDescr
	/// \param fileName : Path to the settings file to parse
	///
	/// Parse a settings file and fill the settings structure.
	/// \EndMemberDescr

	ConfigParser::ParseFile(fileName);

	if(NamespaceExists("global")){
		ConfigNamespace ns = GetNamespace("global");
		ns.SetValue("usecolors", global::fUseColors);
		ns.SetValue("processoutputnewline", global::fProcessOutputNewLine);
	}
}

bool SettingsReader::global::fUseColors = true;
bool SettingsReader::global::fProcessOutputNewLine = true;
} /* namespace Configuration */
} /* namespace NA62Analysis */
