#ifndef _line_pattern_h
#define _line_pattern_h

#include "defines.h"

/*
 * A line pattern is used to match significant patterns in stretches of 5
 * Locs on the board. The LineLookupTable is used to find these rapidly.
 */

enum PatternType
{
	NoPattern=0,
	Line1=1,
	Line2=2,
	Threat=3,
	Line3=4,
	Take=5,
	Line4=6,
    Line5=7,
	Blocked=8,
	MAX_PATTERN_TYPE=9
};

class LinePattern
{
public:
	LinePattern() : _colour(EMPTY), _patternType(NoPattern), _numInds(0) {}

	LinePattern(const LinePattern &orig) :
		_colour(orig._colour),
		_patternType(orig._patternType),
		_numInds(orig._numInds)
	{
		for (int i=0; i<_numInds; i++)
			_candInds[i] = orig._candInds[i];
	}

	bool operator ==(const LinePattern &other) const
	{
		if ((_colour != other._colour) ||
		    (_patternType != other._patternType) ||
		    (_numInds != other._numInds)) return false;
		for (int i=0; i<_numInds; i++)
			if (_candInds[i] != other._candInds[i])
				return false;
		return true;
	}

	// For debugging only
	friend std::ostream & operator<<(std::ostream &os, const LinePattern& lti);

	Colour _colour;
	PatternType _patternType;
	Breadth _candInds[5];
    unsigned char _numInds;
};

#endif
