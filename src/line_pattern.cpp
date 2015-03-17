#include "line_pattern.h"
#include <sstream>

using std::string;
using std::endl;
using std::ostringstream;

// For debugging only
string toStr(const LinePattern &pattern)
{
	ostringstream ss;
	ss << "Colour: " << (int)pattern._colour
	   << " Match type: " << pattern._patternType;
	if (pattern._numInds > 0)
	{
		ss << " Cands: ";
		for (int i=0; i<pattern._numInds; i++)
		{
			ss << pattern._candInds[i] << ",";
		}
	}
	ss << endl;
	return ss.str();
}

std::ostream & operator<<(std::ostream &os, const LinePattern& lp)
{
    return os << toStr(lp);
}

