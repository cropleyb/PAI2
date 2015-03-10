#ifndef _span_entry_h
#define _span_entry_h

#include "defines.h"
#include "loc.h"

class SpanEntry
{
public:

	bool operator ==(const SpanEntry& other) const {
		// TEMP? Hack!
		return true;
	}

	Loc convertIndToLoc(BoardWidth ind) const {
		CompressedLoc ret = _baseLoc + ind * _offsetPerIndex;
		return Loc(ret);
	}

	BoardWidth _strip;    // Ind for strip number of a known direction
	BoardWidth _locIndex; // Index of this loc
    BoardWidth _minIndex; // Min index for updating this loc
	BoardWidth _maxIndex; // Max index for updating this loc
	BoardWidth _stripMax; // Max index for the entire strip
	int _baseLoc;         // Base location for this strip
	int _offsetPerIndex;  // Offset per index val for this strip
};

#endif
