/*
 * TermManip.hh
 *
 *  Created on: Mar 19, 2015
 *      Author: ncl
 */

#include "TermManip.hh"


namespace manip {

TermManip reset   (Code::NONE);
TermManip bold    (Code::BOLD);
TermManip underline(Code::UNDER);
TermManip black   (Code::FG_BLACK);
TermManip red     (Code::FG_RED);
TermManip green   (Code::FG_GREEN);
TermManip brown   (Code::FG_BROWN);
TermManip blue    (Code::FG_BLUE);
TermManip magenta (Code::FG_MAGENTA);
TermManip cyan    (Code::FG_CYAN);
TermManip white   (Code::FG_WHITE);

bool enableManip = false;
}
