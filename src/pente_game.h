#ifndef _pente_game_h
#define _pente_game_h

#include "move_history.h"
#include "move_suggester.h"
#include "board_reps.h"
#include "position_stats.h"
#include "utility_calc.h"
#include "iab_bridge.h"
#include "transposition_table.h"

class BoardReps;
class SpanEntry;

class PenteGame : public IABBridge
{
public:
	PenteGame();
	virtual ~PenteGame() {}

	void setColour(Colour ourColour) { _ourColour = ourColour; }

	void makeMove(Loc l, Colour p);
    virtual void undoLastMove();

    virtual bool isOnlyOneMove() { return _moveSuggester.isOnlyOneMove(_currDepth, _ourColour); }
    virtual Loc makeNextMove();
	virtual Loc getNextMove(); // Get it without performing it
    virtual bool isCutoff() const;
	virtual UtilityValue getUtility();
	// Just for testing...
	virtual bool isInTT() const { UtilityValue unused; return _transpositionTable.lookup(*this, unused); }
	virtual void clearTT() { _transpositionTable.clear(); }

	void setMaxDepth(Depth d) { _maxDepth = d; }
	Depth getCurrDepth() { return _currDepth; }

	void reportCapture(const SpanEntry &span, bool right, Colour p);

	friend class TranspositionTable;
private:
	void setAndRecordCaptures(Loc l, Colour p);

	BoardReps _boardReps;
	PositionStats _posStats;
	UtilityCalc<PositionStats> _utilCalc;
	MoveHistory _moveHist;
    MoveSuggester _moveSuggester;
	CaptureDirs _captureDirs;
	Depth _currDepth;
	Depth _maxDepth;
	Colour _ourColour;
	TranspositionTable _transpositionTable;
};

#endif
