#include "board_reps.h"
#include "span_lookup_table.h"
#include "line_lookup_table.h"
#include "board_strip.h"
#include "position_stats.h"

BoardReps::BoardReps(BoardWidth boardSize, PositionStats &posStats) : _posStats(posStats)
{
	U64 emptyLine = EDGE >> (2 * boardSize);

	for (int dir=E_DIR; dir<MAX_DIR; dir++)
	{
		for (BoardWidth strip=0; strip<2*boardSize; strip++)
		{
			U64 &val = _boardStrips[dir][strip];
			val = emptyLine;
		}
	}
}

void BoardReps::setOcc(Loc l, Colour c)
{
	for (int dir=E_DIR; dir<MAX_DIR; dir++)
	{
		// Find the en/decoding info for that loc in the given direction
		const SpanEntry &span = spanLookupTable[dir][l._value];

		// val represents an entire board line that intersects the given loc
		U64 &val = _boardStrips[dir][span._strip];

		matchRange(val, span, _posStats, -1);

		// Set that location (bits thereof)
		U64 shift = (U64)1 << (2 * span._locIndex);
		val &= ~(shift + (shift << 1));
		val |= (c * shift);

		matchRange(val, span, _posStats, 1);
	}
}

Colour BoardReps::getOcc(Loc l)
{
	const SpanEntry *spanEntry = &spanLookupTable[E_DIR][l._value];

	int shift = 2 * spanEntry->_locIndex;
	U64 mask = (U64)3 << shift;
	U64 val = (_boardStrips[E_DIR][spanEntry->_strip] & mask);
	return val >> shift;
}

