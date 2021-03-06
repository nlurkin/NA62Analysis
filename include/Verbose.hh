/*
 * Verbose.hh
 *
 *  Created on: Mar 20, 2015
 *      Author: ncl
 */

#ifndef VERBOSE_HH_
#define VERBOSE_HH_

#include <ostream>

#include <TString.h>

#include "FWEnums.hh"

#define PRINTVAR(v) #v << "= " << v << " "

namespace NA62Analysis {

/// \class Verbose
/// \Brief
/// Base class for any verbose class
/// \EndBrief
///
/// \Detailed
/// Implement some output stream manipulator for different verbosity levels.\n
/// Prints output in the format\n
/// VERBOSELEVEL - [ModuleName]   Message
/// \EndDetailed

class Verbose {
public:
	Verbose();
	Verbose(std::string name);
	virtual ~Verbose();

	void SetVerbosity(Verbosity::VerbosityLevel v);
	void SetGlobalVerbosity(Verbosity::VerbosityLevel v);
	void SetLogToFile(TString fileName);

	Verbosity::VerbosityLevel GetVerbosityLevel() const {
		/// \MemberDescr
		/// \return Currently used verbosity level (can be local or global)
		/// \EndMemberDescr
		return fLocalVerbosityActive ? fLocalVerbosityLevel : fVerbosityLevel;
	};
	Verbosity::VerbosityLevel GetTestLevel() const {
		/// \MemberDescr
		/// \return Verbosity level currently printed
		/// \EndMemberDescr
		return fVerbosityTest;
	};
	std::string GetModuleName() const {
		/// \MemberDescr
		/// \return Module display name
		/// \EndMemberDescr
		return fModuleName;
	};

	void SetStream(std::ostream &s) const {
		/// \MemberDescr
		/// \param s : Output stream reference
		///
		/// Set the output stream to write into
		/// \EndMemberDescr
		fCurrentStream=&s;
	};
	std::ostream& GetStream() const {
		/// \MemberDescr
		/// \return Output stream currently used
		/// \EndMemberDescr
		return *fCurrentStream;
	};

	const Verbose& PrintLevel(Verbosity::VerbosityLevel v) const {
		/// \MemberDescr
		/// \param v : Verbosity level requested
		/// \return reference to itself
		///
		/// Set the currently printed verbosity level and return a reference
		/// to itself. Use like
		/// \code
		/// cout << PrintLevel(Verbosity::kDebug) << "This is a debug message: " << var << endl;
		/// \endcode
		/// \EndMemberDescr
		fVerbosityTest = v; return *this;
	};
	bool TestLevel(Verbosity::VerbosityLevel level) const;
	bool CanPrint() const;

	//Standard levels stream manipulators (shortcuts for PrintLevel)
	const Verbose& noverbose() const {
			/// \MemberDescr
			/// \return Reference to itself
			///
			/// Manipulator for no verbosity level print. Convenience proxy to
			/// \code
			/// Printlevel(Verbosity::kNo)
			/// \endcode
			/// Use like
			/// \code
			/// cout << noverbose() << "This is a message: " << var << endl;
			/// \endcode
			/// This one is always printed, whatever the current verbosity level.
			/// It just ensure that the verbose style stream prefix in printed.
			/// \EndMemberDescr
			return PrintLevel(Verbosity::kNo);
	};
	const Verbose& standard() const {
			/// \MemberDescr
			/// \return Reference to itself
			///
			/// Manipulator for standard verbosity level print. Convenience proxy to
			/// \code
			/// Printlevel(Verbosity::kStandard)
			/// \endcode
			/// Use like
			/// \code
			/// cout << standard() << "This is a standard: " << var << endl;
			/// \endcode
			/// This level does NOT automatically print the usual prefixes.
			/// \EndMemberDescr
			return PrintLevel(Verbosity::kStandard);
	};
	const Verbose& user() const {
		/// \MemberDescr
		/// \return Reference to itself
		///
		/// Manipulator for user verbosity level print. Convenience proxy to
		/// \code
		/// Printlevel(Verbosity::kUser)
		/// \endcode
		/// Use like
		/// \code
		/// cout << user() << "This is a user message: " << var << endl;
		/// \endcode
		/// \EndMemberDescr
		return PrintLevel(Verbosity::kUser);
	};
	const Verbose& normal() const {
		/// \MemberDescr
		/// \return Reference to itself
		///
		/// Manipulator for user verbosity level print. Convenience proxy to
		/// \code
		/// Printlevel(Verbosity::kNormal)
		/// \endcode
		/// Use like
		/// \code
		/// cout << normal() << "This is a normal message: " << var << endl;
		/// \endcode
		/// \EndMemberDescr
		return PrintLevel(Verbosity::kNormal);
	};
	const Verbose& extended() const {
		/// \MemberDescr
		/// \return Reference to itself
		///
		/// Manipulator for user verbosity level print. Convenience proxy to
		/// \code
		/// Printlevel(Verbosity::kExtended)
		/// \endcode
		/// Use like
		/// \code
		/// cout << extended() << "This is a extended message: " << var << endl;
		/// \endcode
		/// \EndMemberDescr
		return PrintLevel(Verbosity::kExtended);
	};
	const Verbose& debug() const {
		/// \MemberDescr
		/// \return Reference to itself
		///
		/// Manipulator for user verbosity level print. Convenience proxy to
		/// \code
		/// Printlevel(Verbosity::kDebug);
		/// \endcode
		/// Use like
		/// \code
		/// cout << debug() << "This is a debug message: " << var << endl;
		/// \endcode
		/// \EndMemberDescr
		return PrintLevel(Verbosity::kDebug);
	};
	const Verbose& trace() const {
		/// \MemberDescr
		/// \return Reference to itself
		///
		/// Manipulator for user verbosity level print. Convenience proxy to
		/// \code
		/// Printlevel(Verbosity::kTrace)
		/// \endcode
		/// Use like
		/// \code
		/// cout << trace() << "This is a trace message: " << var << endl;
		/// \endcode
		/// \EndMemberDescr
		return PrintLevel(Verbosity::kTrace);
	};

	static std::string GetVerbosityLevelName(Verbosity::VerbosityLevel v);
	static Verbosity::VerbosityLevel GetVerbosityLevelFromName(TString v);

	static bool fLogToFile; ///< Should the logs be written in file instead of specified ostream ?
	static Verbosity::VerbosityLevel fVerbosityLevel; ///< Global verbosity of the program
	static std::ofstream fLogFileStream; ///< File output stream used when fLogToFile is true

private:
	bool fLocalVerbosityActive; ///< Should local verbosity level be used ?
	Verbosity::VerbosityLevel fLocalVerbosityLevel; ///< Verbosity for this module only
	mutable Verbosity::VerbosityLevel fVerbosityTest; ///< Transient member. Store the currently requested verbosity output
	std::string fModuleName; ///< Name to display in the output
	mutable std::ostream *fCurrentStream; ///< Transient member. Store the current ostream on which we write
};

template <class T>
const Verbose& operator<<(const Verbose &level, T x) {
	/// \MemberDescr
	/// \param level : Reference to Verbose class
	/// \param x : Value to print (template)
	/// \return Reference to itself
	///
	/// Print value to output stream only of the currently requested verbosity level
	/// is at least equal to the verbosity level.
	/// \EndMemberDescr
	if(level.CanPrint()) level.GetStream() << x;
	return level;
}
const Verbose& operator<<(const Verbose &level, std::ostream& (*f)(std::ostream&));
const Verbose& operator <<(std::ostream& s, const Verbose &level);


} /* namespace NA62Analysis */

#endif /* VERBOSE_HH_ */
