/*
 * TermManip.hh
 *
 *  Created on: Mar 19, 2015
 *      Author: ncl
 */

#include "TermManip.hh"

namespace NA62Analysis {
namespace manip {

const TermManip reset   (Code::NONE);
const TermManip bold    (Code::BOLD);
const TermManip underline(Code::UNDER);
const TermManip black   (Code::FG_BLACK);
const TermManip red     (Code::FG_RED);
const TermManip green   (Code::FG_GREEN);
const TermManip brown   (Code::FG_BROWN);
const TermManip blue    (Code::FG_BLUE);
const TermManip magenta (Code::FG_MAGENTA);
const TermManip cyan    (Code::FG_CYAN);
const TermManip white   (Code::FG_WHITE);

bool enableManip = false;
} /* namespace manip */
} /* namespace NA62Analysis */
