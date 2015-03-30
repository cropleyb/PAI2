#ifndef _board_reps_h
#define _board_reps_h

#include "loc.h"
#include "defines.h"

class PositionStats;
class SpanEntry;
class LinePattern;

class BoardReps
{
public:
	BoardReps(BoardWidth boardSize, PositionStats &posStats);

    void clear();

    bool setOcc(Loc l, Colour c);
    Colour getOcc(Loc l) const;

	void setBoardSize(BoardWidth bs);
	BoardWidth getBoardSize() const { return _boardSize; }

	friend class PenteGame;
	friend class TranspositionTable;

private:
	void initEdges();
	void initEdgeVal(int dir, Loc l);
	
	// For debugging...
	void print() const;

	PositionStats &_posStats;
	BoardWidth _boardSize;
	U64 _boardStrips[MAX_DIR][2*MAX_WIDTH];
	// std::array<std::array<U64, MAX_DIR>, 2*MAX_WIDTH> _boardStrips; // TODO
};

#endif
