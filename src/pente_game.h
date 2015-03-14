#ifndef _pente_game_h
#define _pente_game_h

#include "move_history.h"
#include "move_suggester.h"
#include "board_reps.h"
#include "position_stats.h"
#include "utility_calc.h"

class BoardReps;
class SpanEntry;

class PenteGame
{
public:
	PenteGame();

	void makeMove(Loc l, Colour p);
    void undoLastMove();

    bool isOnlyOneMove() { return _moveSuggester.isOnlyOneMove(_currDepth); }
    Loc makeNextMove();
    bool isCutoff() const;
	UtilityValue getUtility();

	Depth getCurrDepth() { return _currDepth; }

	void reportCapture(const SpanEntry &span, bool right, Colour p);
private:
	void setAndRecordCaptures(Loc l, Colour p);

	BoardReps _boardReps;
	PositionStats _posStats;
	MoveHistory _moveHist;
    MoveSuggester _moveSuggester;
	// UtilityCalculator _utilCalc;
	CaptureDirs _captureDirs;
	Depth _currDepth;
};

#endif
