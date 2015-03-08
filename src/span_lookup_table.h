#ifndef _span_lookup_table_h
#define _span_lookup_table_h

#include "defines.h"
#include "loc.h"

class SpanEntry
{
public:
#if 0
	SpanEntry() :
			_strip(0),
			_locIndex(0),
			_minIndex(0),
			_maxIndex(0),
			_baseLoc(Loc(0)),
			_offsetPerIndex(Loc(0)) {}

	SpanEntry(const SpanEntry& orig) :
		       _strip(orig._strip),
			   _locIndex(orig._locIndex),
			   _minIndex(orig._minIndex),
			   _maxIndex(orig._maxIndex),
			   _baseLoc(orig._baseLoc),
			   _offsetPerIndex(orig._offsetPerIndex) {}
#endif

	bool operator ==(const SpanEntry& other) const {
		// TEMP? Hack!
		return true;
#if 0
		return _strip == other._strip &&
			   _locIndex == other._locIndex &&
			   _minIndex == other._minIndex &&
			   _maxIndex == other._maxIndex &&
			   _baseLoc == other._baseLoc &&
			   _offsetPerIndex == other._offsetPerIndex;
#endif
	}

	BoardWidth _strip;    // Ind for strip number of a known direction
	BoardWidth _locIndex; // Index of this loc
    BoardWidth _minIndex; // Min index for updating this loc
	BoardWidth _maxIndex; // Max index for updating this loc
	Loc _baseLoc;         // Base location for this strip
	Loc _offsetPerIndex;  // Offset per index val for this strip

	Loc convertIndToLoc(BoardWidth ind) const {
		CompressedLoc ret = _baseLoc._value + ind * _offsetPerIndex._value;
		return Loc(ret);
	}

};

extern SpanEntry spanLookupTable[MAX_DIR][MAX_LOCS];

bool buildSpanTable(BoardWidth boardSize);

#endif
