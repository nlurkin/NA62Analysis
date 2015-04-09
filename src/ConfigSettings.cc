/*
 * SettingsReader.cc
 *
 *  Created on: Mar 19, 2015
 *      Author: ncl
 */

#include <ConfigSettings.hh>

#include <iostream>

namespace NA62Analysis {
namespace Configuration {

ConfigSettings::ConfigSettings() {
	/// \MemberDescr
	/// Default constructor
	/// \EndMemberDescr
}

ConfigSettings::~ConfigSettings() {
	/// \MemberDescr
	/// Default destructor
	/// \EndMemberDescr
}

void ConfigSettings::ParseFile(TString fileName) {
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
		ns.SetValue("skippedname", global::fSkippedName);
	}
}

bool ConfigSettings::global::fUseColors = true;
bool ConfigSettings::global::fProcessOutputNewLine = true;
std::string ConfigSettings::global::fSkippedName = "NA62Analysis";
} /* namespace Configuration */
} /* namespace NA62Analysis */
