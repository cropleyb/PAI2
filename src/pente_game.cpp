#include "pente_game.h"
#include "line_lookup_table.h"
#include "span_lookup_table.h"
#include "board_strip.h"

PenteGame::PenteGame()
	: _moveSuggester(_posStats), _boardReps(19,_posStats)
{
	buildLineLookupTable();
	buildSpanTable(19);
}

void PenteGame::makeMove(Loc l, Colour p)
{
	const PriorityLevel &pl = _posStats.getPriorityLevel(p, Take);
	if (pl.getCount(l) > 0) {
		setAndRecordCaptures(l, p);
	}

	_boardReps.setOcc(l, p);
}

void PenteGame::setAndRecordCaptures(Loc l, Colour p)
{
	// TODO
	for (int dir=0; dir<MAX_DIR; dir++)
	{
		SpanEntry span = spanLookupTable[dir][l._value];
		U64 occs = _boardReps._boardStrips[dir][span._strip];
		matchCaptures(occs, span, *this, p);
	}
}

void PenteGame::reportCapture(const SpanEntry &span, bool right, Colour p)
{
	_posStats.reportCaptured(p, 2, 1);
}

#if 0
class PenteGame
{
public:
	PenteGame();

    void undo();

    bool isOneMove() const;
    Loc getOnlyMove();

    // Why do we need it to return the move?
	// std::pair<UtilityValue, Loc>() getUtilityAndMove();

    void makeNextMove();
    void undoLastMove();
    void isCutoff(Depth depth);

private:
	BoardReps _boardReps;
	PositionStats _posStats;
	// MoveHistory _moveHist;
    MoveSuggester _moveSuggester;
	// UtilityCalculator _utilCalc;
};

#endif
