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
			e._maxIndex = std::min(x+4, (boardSize-1));
			e._baseLoc = Loc(0,y);
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
			BoardWidth maxInd = strip;

			if (strip >= boardSize)
			{
				minInd = strip-boardSize+1;
				maxInd = boardSize-1;
			}

			e._strip = strip;
			e._locIndex = x;
			e._minIndex = std::max((BoardWidth)(x-4), minInd);
			e._maxIndex = std::min((BoardWidth)(x+4), maxInd);
			e._baseLoc = Loc(0,strip);
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
			e._maxIndex = std::min(y+4, (boardSize-1));
			e._baseLoc = Loc(x,0);
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
			BoardWidth maxInd = strip;

			if (strip >= boardSize)
			{
				minInd = strip-boardSize+1;
				maxInd = boardSize-1;
			}

			e._strip = strip;
			e._locIndex = x;
			e._minIndex = std::max((BoardWidth)(x-4), minInd);
			e._maxIndex = std::min((BoardWidth)(x+4), maxInd);
			e._baseLoc = Loc(0,boardSize - strip - 1);
			e._offsetPerIndex = Loc(1,1)._value;
		}
	}
}

static BoardWidth builtFor = 0;

// #include <iostream>
bool buildSpanTable(BoardWidth boardSize)
{
	// std::cout << "Checking to buildSpanTable: " << (int)boardSize << std::endl;
    if (builtFor == boardSize) return false;
	// std::cout << "Building SpanTable: " << (int)boardSize << std::endl;

	buildESpanTable(boardSize);
	buildSESpanTable(boardSize);
	buildNSpanTable(boardSize);
	buildNESpanTable(boardSize);

	builtFor = boardSize;

	return true;
}

