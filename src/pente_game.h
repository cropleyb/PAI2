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
class PenteGame;

// The current search object
extern PenteGame *_penteGame_;

class PenteGame : public IABBridge
{
public:
	PenteGame();
	virtual ~PenteGame();

	void setPAI1(); // For debugging PAI2 with PAI1 settings...
	void setPartPAI2();

	void setColour(Colour ourColour) { _ourColour = ourColour; }
	void resetSearch() { _moveSuggester.reset(); _currDepth=0; _penteGame_=this; clearTT(); }
	void restartGame() { 
		while (getLastMoveNumber()) undoLastMove();
		resetSearch();
	}

	void makeMove(Loc l, Colour p);
    virtual void undoLastMove();

    virtual bool isOnlyOneMove() { return _moveHist.getLastMoveNumber() == 0 or _moveSuggester.isOnlyOneMove(_currDepth, _ourColour); }
    virtual Loc makeNextMove();
	virtual Loc getNextMove(); // Get it without performing it
    virtual bool needUtility();
    virtual bool needSearch();
	virtual UtilityValue getUtility();
	virtual void storeInTransTable(UtilityValue uv);

	// Just for testing...
	virtual bool isInTT() const { UtilityValue unused; return _transpositionTable.lookup(*this, unused); }

	bool isLegalMove(Loc l) const; // TODO: inline?

	void reportCapture(const SpanEntry &span, bool right, Colour p);
	CapCount getCaptured(Colour p) const { return _posStats.getCaptured(p); }

	// Game and AI parameters from user code
	void setNormalDepth(Depth d) { _normalDepth = d; }
	void setVCTDepth(Depth d) { _vctDepth = d; }
	Depth getCurrDepth() const { return _currDepth; }
	Depth getNormalDepth() const { return _normalDepth; }
	Depth getVCTDepth() const { return _vctDepth; }
	void setBoardSize(BoardWidth bs);
	BoardWidth getBoardSize() const { return _boardReps.getBoardSize(); }
	void setRules(RulesType rules);

	// Easy accessors
	MoveNumber getLastMoveNumber() const { return _moveHist.getLastMoveNumber(); }

	Loc getMove(MoveNumber mn) const { return _moveHist.getMoved(mn); }
	Colour getWonBy() const { return _posStats.getWonBy(); }

	Colour getOcc(Loc l) { return _boardReps.getOcc(l); }

	// Debugging...
	void print() const;
	void printHistory() const;

	friend class TranspositionTable;
private:
	void setAndRecordCaptures(Loc l, Colour p);
	virtual void clearTT() { _transpositionTable.clear(); _isInTT = false; }

	BoardReps _boardReps;
	PositionStats _posStats;
	UtilityCalc<PositionStats> _utilCalc;
	MoveHistory _moveHist;
    MoveSuggester _moveSuggester;
	CaptureDirs _captureDirs;
	Depth _currDepth;
	Depth _normalDepth;
	Depth _vctDepth;
	Colour _ourColour;
	TranspositionTable _transpositionTable;

	bool _allowCaptures;
	bool _forceFirstMoveInCentre;
	bool _restrictSecondP1Move;

	// These are really temporary variables
	// but I don't want to put them in the AB code.
	mutable bool _isInTT;
   	mutable UtilityValue _ttVal;
	bool _weAreForced;
};

#endif
