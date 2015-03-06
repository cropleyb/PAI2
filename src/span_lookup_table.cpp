#include "span_lookup_table.h"

SpanEntry spanLookupTable[MAX_DIR][MAX_LOCS];

void buildESpanTable(BoardWidth boardSize) // DirectionType dir)
{
	for (BoardWidth x=0; x<boardSize; x+=1)
	{
		for (BoardWidth y=0; y<boardSize; y+=1)
		{
			Loc l(x,y);
			SpanEntry *e = &spanLookupTable[0][l._value];

			e->_minIndex = std::max(x-4, 0);
			e->_maxIndex = std::min(x+4, (boardSize-1));
			e->_locIndex = x;
			e->_baseLoc = Loc(0,y);
			e->_offsetPerIndex = Loc(1,0);
		}
	}
}

void buildSpanTable(BoardWidth boardSize)
{
	buildESpanTable(boardSize);
}

