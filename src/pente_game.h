#ifndef _pente_game_h
#define _pente_game_h

#include "move_history.h"
#include "move_suggester.h"
#include "board_reps.h"
#include "position_stats.h"
//#include "utility_calculator.h"

class BoardReps;
class SpanEntry;

class PenteGame
{
public:
	PenteGame();

	void makeMove(Loc l, Colour p);

    bool isOneMove() const;
    Loc getOnlyMove();

    // Why do we need it to return the move?
	// std::pair<UtilityValue, Loc>() getUtilityAndMove();

    void makeNextMove();
	Loc getLastMove() { return _lastMove; }
    void undoLastMove();
    void isCutoff(Depth depth);

	void reportCapture(const SpanEntry &span, bool right, Colour p);
private:
	void setAndRecordCaptures(Loc l, Colour p);

	BoardReps _boardReps;
	PositionStats _posStats;
	MoveHistory _moveHist;
    MoveSuggester _moveSuggester;
	// UtilityCalculator _utilCalc;
	CaptureDirs _captureDirs;
	Loc _lastMove;
};

#endif
