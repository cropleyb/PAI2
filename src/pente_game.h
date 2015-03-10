#ifndef _pente_game_h
#define _pente_game_h

//#include "move_history.h"
#include "move_suggester.h"
#include "board_reps.h"
#include "position_stats.h"
//#include "utility_calculator.h"

class BoardReps;

class PenteGame
{
public:
	PenteGame();

	void makeMove(Loc);
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
