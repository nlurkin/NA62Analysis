/*
 * TermManip.hh
 *
 *  Created on: Mar 19, 2015
 *      Author: ncl
 */

#ifndef TERMMANIP_HH_
#define TERMMANIP_HH_

#include <ostream>

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

class TermManip {
public:
	TermManip(Code c): code(c) {};
	virtual ~TermManip() {};

	friend std::ostream&
	operator<<(std::ostream& os, const TermManip& mod) {
		if(manip::enableManip) return os << "\033[" << mod.code << "m";
		else return os;
	}
private:
	Code code;
};

extern TermManip reset;
extern TermManip bold;
extern TermManip underline;
extern TermManip black;
extern TermManip red;
extern TermManip green;
extern TermManip brown;
extern TermManip blue;
extern TermManip magenta;
extern TermManip cyan;
extern TermManip white;

} /* namespace manip */



#endif /* TERMMANIP_HH_ */
