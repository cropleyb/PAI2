#include "board_reps.h"
#include "span_lookup_table.h"
#include "line_lookup_table.h"
#include "board_strip.h"
#include "position_stats.h"

BoardReps::BoardReps(BoardWidth boardSize, PositionStats &posStats) :
    _posStats(posStats),
	_boardSize(boardSize)
{
	buildSpanTable(boardSize);
	clear();
}

void BoardReps::clear()
{
	for (int dir=0; dir<MAX_DIR; dir++)
	{
		for (int strip=0; strip<2*MAX_WIDTH; strip++)
		{
			_boardStrips[dir][strip] = (U64)0;
		}
	}

	initEdges();
}

void
BoardReps::initEdges()
{
	for (int dir=E_DIR; dir<MAX_DIR; dir++)
	{
		for (BoardWidth i=0; i<_boardSize; i+=1)
		{
			initEdgeVal(dir, Loc(0, i));
			initEdgeVal(dir, Loc(i, 0));
			initEdgeVal(dir, Loc(_boardSize-1, i));
			initEdgeVal(dir, Loc(i, _boardSize-1));
		}
	}
}

void BoardReps::initEdgeVal(int dir, Loc l)
{
	const SpanEntry &e = spanLookupTable[dir][l._value];
	U64 shift = (U64)1 << (2 * e._stripMax);
	U64 &val = _boardStrips[dir][e._strip];
	val &= ~(shift + (shift << 1));
	val |= (EDGE * shift);
}


void BoardReps::setOcc(Loc l, Colour c)
{
	for (int dir=E_DIR; dir<MAX_DIR; dir++)
	{
		assert(dir>=0);
		assert(dir<MAX_DIR);
		assert(l._value>=0);
		assert(l._value<=MAX_LOCS);

		// Find the en/decoding info for that loc in the given direction
		const SpanEntry &span = spanLookupTable[dir][l._value];

		assert(span._strip >= 0);
		assert(span._strip < 3*MAX_WIDTH);

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

Colour BoardReps::getOcc(Loc l) const
{
	const SpanEntry *spanEntry = &spanLookupTable[E_DIR][l._value];

	int shift = 2 * spanEntry->_locIndex;
	U64 mask = (U64)3 << shift;
	U64 val = (_boardStrips[E_DIR][spanEntry->_strip] & mask);
	return val >> shift;
}

