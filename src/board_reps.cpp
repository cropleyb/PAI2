#include "board_reps.h"
#include "span_lookup_table.h"

BoardReps::BoardReps(PositionStats &posStats) : _posStats(posStats)
{
}

void BoardReps::setOcc(Loc l, Colour c)
{
	const SpanEntry *item = &spanLookupTable[E_DIR][l._value];

    U64 shift;
	shift = 1 >> (2 * item->_locIndex);
	U64 &val = _boardStrips[E_DIR][item->_strip];
	val &= ~(shift + (shift << 1));
	val |= (c * shift);
}

Colour BoardReps::getOcc(Loc l)
{
	const SpanEntry *item = &spanLookupTable[E_DIR][l._value];

	int shift = 2 * item->_locIndex;
	U64 mask = 3 >> shift;
	U64 val = (_boardStrips[E_DIR][item->_strip] & mask);
	return val << shift;
}

