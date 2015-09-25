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

/// \class ConfigSettings
/// \Brief
/// Read settings file for the framework
/// \EndBrief
///
/// \Detailed
/// The settings file must be in the format
/// \code
/// [Global]
/// UseColors = true|false
/// ProcessOutputNewLine = true|false
/// SkippedName = NA62Analysis
/// EventsDownscaling = integer
/// \endcode
/// The currently available settings for NA62Analysis are<br>
/// UseColors: If true the terminal output can use colors.<br>
/// ProcessOutputNewLine : If true the "Processing event xxx/xxx => xx.xx% ... " lines end with
/// an end line. Else a carriage return is used and the line is erased by the next terminal output.<br>
/// SkippedName: Name for the output file containing the list of skipped input files. The file is named
/// SkippedName.skipped<br>
/// EventsDownscaling: Process only 1 events out of EventsDownscaling. This option is activated
/// with the -d/--downscaling option
/// \EndDetailed

class ConfigSettings : public ConfigParser{
public:
	ConfigSettings();
	virtual ~ConfigSettings();

	void ParseFile(TString fileName);

	/// \struct global
	/// \Brief
	/// Structure for the global settings.
	/// \EndBrief
	///
	/// \Detailed
	/// Structure containing the settings from the "Global" section in the settings file.
	/// \EndDetailed
	struct global {
		static bool fUseColors; ///< Can use colors in the output
		static bool fProcessOutputNewLine; ///< Use \\n or \\r when printing the  "Processing ..." line
		static std::string fSkippedName; ///< Name of the .skipped file
		static std::string fSvcClass; ///< Name of the svcClass
		static int fEventsDownscaling; ///< Events downscaling. Process only 1 out of fEventsDownscaling
	};
};

} /* namespace Configuration */
} /* namespace NA62Analysis */

#endif /* SETTINGSREADER_HH_ */
