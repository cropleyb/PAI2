#ifndef _line_pattern_h
#define _line_pattern_h

#include "defines.h"

/*
 * A line pattern is used to match significant patterns in stretches of 5
 * Locs on the board. The LineLookupTable is used to find these rapidly.
 */

enum LinePatternType
{
	NoPattern=0,
	Line1=1,
	Line2=2,
	Threat=3,
	Line3=4,
	Take=5,
	Line4=6,
    Line5=7,
	Blocked4=8,
	MAX_LINE_PATTERN_TYPE=9
};

enum ExtraPriorityLevels {
	TakeTake=11,
	Blocked4Take=12,
	FourTake=13,
	MAX_PATTERN_TYPE=14
};

class LinePattern
{
public:
	LinePattern() : _colour(EMPTY), _patternType(NoPattern), _numCands(0) {}

	LinePattern(const LinePattern &orig) :
		_colour(orig._colour),
		_patternType(orig._patternType),
		_numCands(orig._numCands)
	{
		for (int i=0; i<5; i++) {
			_inds[i] = orig._inds[i];
		}
	}

	// Special copy constructor for adding a candidate offset simultaneously
	LinePattern(const LinePattern &orig, Breadth candOffset) :
		_colour(orig._colour),
		_patternType(orig._patternType),
		_numCands(orig._numCands)
	{
		for (int i=0; i<5; i++)
		{
			_inds[i] = orig._inds[i] + candOffset;
		}
	}
	
	bool operator ==(const LinePattern &other) const
	{
		if ((_colour != other._colour) ||
		    (_patternType != other._patternType)) return false;
		for (int i=0; i<_numCands; i++)
			if (_inds[i] != other._inds[i])
				return false;
		return true;
	}

	Breadth victim(bool second) const {
		// only applicable to takes...
		return _inds[(int)second + 1];
	}

	Breadth taker() const {
		// only applicable to takes...
		return _inds[3];
	}

	Breadth blocker() const {
		// only applicable to blocked 5s...
		return _inds[0];
	}

	Breadth occupied(int index) const {
		// only applicable to fours so far...
		return _inds[index + 1];
	}

	// For debugging only
	friend std::ostream & operator<<(std::ostream &os, const LinePattern& lti);

	Colour _colour;
	LinePatternType _patternType;
	Breadth _inds[5];
    unsigned char _numCands;
};

#endif
