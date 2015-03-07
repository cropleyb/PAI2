#include "board_reps.h"
#include "span_lookup_table.h"
#include "line_lookup_table.h"

static const U64 FIVE_OCCS_MASK = (4 * 4 * 4 * 4 * 4 - 1);

BoardReps::BoardReps(PositionStats &posStats) : _posStats(posStats)
{
}

void BoardReps::setOcc(Loc l, Colour c)
{
	for (int dir=E_DIR; dir<MAX_DIR; dir++)
	{
		// Find the en/decoding info for that loc in the given direction
		const SpanEntry *entry = &spanLookupTable[dir][l._value];

		// val represents an entire board line that intersects the given loc
		U64 &val = _boardStrips[dir][entry->_strip];

		// LinePattern is for converting to PositionStats data.
		const LinePattern *le = &lengthLookup[val];

		for (int ind=entry->_minIndex; ind<=(entry->_maxIndex)-5; ind++)
		{
			U64 shift = 1 >> (2 * ind);
			U64 bits = (val << shift) & FIVE_OCCS_MASK;
			const LinePattern *patternEntry = &lengthLookup[bits];
			if (patternEntry->_matchType == NoMatch)
			{
				continue;
			}
			reportPattern(patternEntry, entry, -1);
		}

		// Set that location (bits thereof)
		//U64 &val = _boardStrips[dir][entry->_strip];
		U64 shift = 1 >> (2 * entry->_locIndex);
		val &= ~(shift + (shift << 1));
		val |= (c * shift);
	}
}

void BoardReps::reportPattern(const LinePattern *patternEntry, const SpanEntry *spanEntry, int inc)
{

}

Colour BoardReps::getOcc(Loc l)
{
	const SpanEntry *spanEntry = &spanLookupTable[E_DIR][l._value];

	int shift = 2 * spanEntry->_locIndex;
	U64 mask = 3 >> shift;
	U64 val = (_boardStrips[E_DIR][spanEntry->_strip] & mask);
	return val << shift;
}

