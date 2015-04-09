/*
 * TermManip.hh
 *
 *  Created on: Mar 19, 2015
 *      Author: ncl
 */

#ifndef TERMMANIP_HH_
#define TERMMANIP_HH_

#include <ostream>

namespace NA62Analysis {
namespace manip {
	extern bool enableManip;

	enum Code {
		NONE        = 0,
		BOLD        = 1,
		UNDER       = 4,
		FG_BLACK    = 30,
		FG_RED      = 31,
		FG_GREEN    = 32,
		FG_BROWN    = 33,
		FG_BLUE     = 34,
		FG_MAGENTA  = 35,
		FG_CYAN     = 36,
		FG_WHITE    = 37,
		BG_RED      = 41,
		BG_GREEN    = 42,
		BG_BROWN    = 43,
		BG_BLUE     = 44,
		BG_MAGENTA  = 45,
		BG_CYAN     = 46,
		BG_WHITE    = 47
	};

/// \class TermManip
/// \Brief
/// Class for output stream manipulation
/// \EndBrief
///
/// \Detailed
/// Manipulators for the output stream. Enable to simply change text
///	color or font.
/// \EndDetailed
class TermManip {
public:
	TermManip(Code c): code(c) {
		/// \MemberDescr
		/// \param c : Manipulator ANSI code
		///
		/// Constructor
		/// \EndMemberDescr
	};
	virtual ~TermManip() {};

	friend std::ostream&
	operator<<(std::ostream& os, const TermManip& mod) {
		/// \MemberDescr
		/// \param os : Output stream to manipulate
		/// \param mod : manipulator to apply
		///
		/// Insert a manipulator in the stream
		/// \EndMemberDescr
		if(manip::enableManip) return os << "\033[" << mod.code << "m";
		else return os;
	}
private:
	TermManip() : code(NONE) {};
	Code code; ///< Manipulator ANSI code
};

extern const TermManip reset;
extern const TermManip bold;
extern const TermManip underline;
extern const TermManip black;
extern const TermManip red;
extern const TermManip green;
extern const TermManip brown;
extern const TermManip blue;
extern const TermManip magenta;
extern const TermManip cyan;
extern const TermManip white;

} /* namespace manip */
} /* namespace NA62Analysis */



#endif /* TERMMANIP_HH_ */
