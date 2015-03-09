#include "span_lookup_table.h"


SpanEntry spanLookupTable[MAX_DIR][MAX_LOCS];

void buildESpanTable(BoardWidth boardSize)
{
	for (BoardWidth x=0; x<boardSize; x+=1)
	{
		for (BoardWidth y=0; y<boardSize; y+=1)
		{
			Loc l(x,y);
			SpanEntry &e = spanLookupTable[E_DIR][l._value];

			e._strip = y;
			e._locIndex = x;
			e._minIndex = std::max(x-4, 0);
			e._maxIndex = std::min(x+4, (int)boardSize);
			e._baseLoc = Loc(0,y)._value;
			e._offsetPerIndex = Loc(1,0)._value;
		}
	}
}

void buildSESpanTable(BoardWidth boardSize)
{
	for (BoardWidth x=0; x<boardSize; x+=1)
	{
		for (BoardWidth y=0; y<boardSize; y+=1)
		{
			Loc l(x,y);
			SpanEntry &e = spanLookupTable[SE_DIR][l._value];

			BoardWidth strip = x+y;

			BoardWidth minInd = 0;
			BoardWidth maxInd = strip + 1; // EDGE?

			if (strip >= boardSize)
			{
				minInd = strip-boardSize+1;
				maxInd = boardSize;
			}

			e._strip = strip;
			e._locIndex = x;
			e._minIndex = std::max((BoardWidth)(x-4), minInd);
			e._maxIndex = std::min((BoardWidth)(x+4), maxInd);
			e._baseLoc = Loc(0,strip)._value;
			e._offsetPerIndex = 1 - 32; // Work around prevention of neg. Loc
		}
	}
}

void buildNSpanTable(BoardWidth boardSize)
{
	for (BoardWidth x=0; x<boardSize; x+=1)
	{
		for (BoardWidth y=0; y<boardSize; y+=1)
		{
			Loc l(x,y);
			SpanEntry &e = spanLookupTable[N_DIR][l._value];

			e._strip = x;
			e._locIndex = y;
			e._minIndex = std::max(y-4, 0);
			e._maxIndex = std::min(y+4, (int)boardSize);
			e._baseLoc = Loc(x,0)._value;
			e._offsetPerIndex = Loc(0,1)._value;
		}
	}
}

void buildNESpanTable(BoardWidth boardSize)
{
	for (BoardWidth x=0; x<boardSize; x+=1)
	{
		for (BoardWidth y=0; y<boardSize; y+=1)
		{
			Loc l(x,y);
			SpanEntry &e = spanLookupTable[NE_DIR][l._value];

			BoardWidth strip = boardSize + x - y - 1;

			BoardWidth minInd = 0;
			BoardWidth maxInd = strip + 1; // include the EDGE

			if (strip >= boardSize)
			{
				minInd = strip-boardSize+1;
				maxInd = boardSize; // include the EDGE
			}

			e._strip = strip;
			e._locIndex = x;
			e._minIndex = std::max((BoardWidth)(x-4), minInd);
			e._maxIndex = std::min((BoardWidth)(x+4), maxInd);
			e._baseLoc = 32 * ((int)y-(int)x);
			e._offsetPerIndex = Loc(1,1)._value;
			assert(e._baseLoc < 700);
		}
	}
}

static BoardWidth builtFor = 0;

bool buildSpanTable(BoardWidth boardSize)
{
    if (builtFor == boardSize) return false;

	buildESpanTable(boardSize);
	buildSESpanTable(boardSize);
	buildNSpanTable(boardSize);
	buildNESpanTable(boardSize);

	builtFor = boardSize;

	return true;
}

