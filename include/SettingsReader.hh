/*
 * SettingsReader.hh
 *
 *  Created on: Mar 19, 2015
 *      Author: ncl
 */

#ifndef SETTINGSREADER_HH_
#define SETTINGSREADER_HH_

#include "ConfigParser.hh"

namespace NA62Analysis {
namespace Configuration {

class SettingsReader : public ConfigParser{
public:
	SettingsReader();
	virtual ~SettingsReader();

	void ParseFile(TString fileName);


	struct global {
		static bool fUseColors;
		static bool fProcessOutputNewLine;
	};
};

} /* namespace Configuration */
} /* namespace NA62Analysis */

#endif /* SETTINGSREADER_HH_ */
