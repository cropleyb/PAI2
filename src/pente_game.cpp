#include "pente_game.h"
#include "line_lookup_table.h"
#include "span_lookup_table.h"
#include "board_strip.h"

#include <iostream>

//#define PGDEBUG
#ifdef PGDEBUG
#define PGD(X) X
#include <iostream>
using namespace std;
#else
#define PGD(X)
#endif


PenteGame *_penteGame_ = 0;

PenteGame::PenteGame()
	: _moveSuggester(_posStats),
	  _boardReps(19,_posStats),
	  _utilCalc(_posStats),
	  _currDepth(0),
	  _normalDepth(1),
	  _vctDepth(0),
	  _ourColour(EMPTY)
{
	buildLineLookupTable();
	buildSpanTable(19);
	setRules('s');
	_isInTT = false;
}

PenteGame::~PenteGame() {
	if (_penteGame_ == this)
	{
		_penteGame_ = 0;
	}
}

bool PenteGame::isLegalMove(Loc l) const
{
	if ((l[0] >= _boardReps.getBoardSize()) ||
	    (l[1] >= _boardReps.getBoardSize()))
	{
		PGD(cout << "Illegal: off board: " << l << endl;)
		return false;
	}
	if (_moveHist.getLastMoveNumber() == 0 && _forceFirstMoveInCentre)
	{
		PGD(cout << "Illegal: first move not in centre: " << l << endl;)
		return _boardReps.isCentreOfBoard(l);
	}
	if (_moveHist.getLastMoveNumber() == 2 && _restrictSecondP1Move)
	{
		if (_boardReps.isTournamentExcluded(l)) {
			PGD(cout << "Illegal: Tournament move: " << l << endl;)
			return false;
		}
	}
	if (_boardReps.getOcc(l) != EMPTY)
	{
		PGD(cout << "Illegal: Already occupied: " << l << endl;)
		return false;
	}
	return true;
}

void PenteGame::makeMove(Loc l, Colour p)
{
#ifdef DEBUG_SEARCH
	std::cout << std::endl;
	for (int d=0; d<_currDepth; d++)
		std::cout << ". ";
	std::cout << (int)(_moveHist.getLastMoveNumber() - 1)<< ".";
	std::cout << l;
#endif
	if (!isLegalMove(l)) {
		std::cerr << "Illegal move requested\n";
		// TODO - Dump stack trace, board state etc. for pente.org:
		// bailOut();
	}
	_captureDirs = 0;

	const PriorityLevel &pl = _posStats.getPriorityLevel(p, Take);
	if (pl.getCount(l) > 0 &&  _allowCaptures) {
		setAndRecordCaptures(l, p);
	}
#ifdef DEBUG
	bool ok=
#endif
	_boardReps.setOcc(l, p);
#ifdef DEBUG
	assert(ok);
#endif
	_posStats.updateStrategicStats(p, l, 1);
	_moveHist.saveMove(l, p, _captureDirs);
}

void PenteGame::setAndRecordCaptures(Loc l, Colour p)
{
	for (int dir=0; dir<MAX_DIR; dir++)
	{
		const SpanEntry &span = spanLookupTable[dir][l._value];
		U64 occs = _boardReps._boardStrips[dir][span._strip];
		matchCaptures(occs, span, *this, p);
	}
}

void PenteGame::setBoardSize(BoardWidth bs)
{
	buildSpanTable(bs);
	_boardReps.setBoardSize(bs);
	_posStats.reset();
	_moveSuggester.reset();
	_moveHist.reset();
}

void PenteGame::reportCapture(const SpanEntry &span, bool left, Colour p)
{
#ifdef DEBUG_SEARCH
	std::cout << 'c';
#endif
	_posStats.reportCaptured(p, 2, 1);

	// Get the indices of the captured pieces
	BoardWidth moveInd = span._locIndex;
	int inc = 1;
	if (left)
	{
		inc = -1;
	}

	Loc capLoc1 = span.convertIndToLoc(moveInd + inc);
	Loc capLoc2 = span.convertIndToLoc(moveInd + inc + inc);
	
	_boardReps.setOcc(capLoc1, EMPTY);
	_boardReps.setOcc(capLoc2, EMPTY);
	_posStats.updateStrategicStats(otherPlayer(p), capLoc1, -1);
	_posStats.updateStrategicStats(otherPlayer(p), capLoc2, -1);

	int fullCircleDir = span._direction;
	if (left)
	{
		fullCircleDir += 4;
	}
	_captureDirs |= 1 << fullCircleDir;
}

void PenteGame::undoLastMove()
{
	MoveNumber mn = _moveHist.getLastMoveNumber() - 1;
	_currDepth--;
	
	Loc l = _moveHist.getMoved(mn);
	CaptureDirs cd = _moveHist.getCapDirs(mn);
	_moveHist.undoLastMove();
#ifdef DEBUG_SEARCH
	std::cout << " U->" << _moveHist.getLastMove() << ' ';
#endif

	_boardReps.setOcc(l, EMPTY);
	Colour movedPlayer = 1 + (mn) % 2;

	_posStats.updateStrategicStats(movedPlayer, l, -1);

	if (cd)
	{
		Colour capturedPlayer = 1 + (mn + 1) % 2;

		// Must undo some captures
		for (int dir=0; dir<8; dir++)
		{
			if (cd & (1 << dir))
			{
				_posStats.reportCaptured(movedPlayer, 2, -1);

				int realDir=dir;
				bool left = false;
				if (dir >= 4)
				{
					left = true;
					realDir -= 4;
				}
				const SpanEntry &span = spanLookupTable[realDir][l._value];

				BoardWidth moveInd = span._locIndex;
				int inc = 1;
				if (left)
				{
					inc = -1;
				}

				Loc capLoc1 = span.convertIndToLoc(moveInd + inc);
				Loc capLoc2 = span.convertIndToLoc(moveInd + inc + inc);
				
				_boardReps.setOcc(capLoc1, capturedPlayer);
				_boardReps.setOcc(capLoc2, capturedPlayer);
				_posStats.updateStrategicStats(capturedPlayer, capLoc1, 1);
				_posStats.updateStrategicStats(capturedPlayer, capLoc2, 1);
			}
		}
	}
}

Loc
PenteGame::makeNextMove()
{
    Loc move = getNextMove();

	if (move.isValid()) {
		if (!isLegalMove(move)) {
			std::cerr << "Illegal move returned by makeNextMove " << move << std::endl;
			// TODO - Dump stack trace, board state etc. for pente.org:
			// bailOut();
			assert(0);
		}
		makeMove(move, (1 + (++_currDepth + _ourColour) % 2));
	}
	return move;
}

Loc
PenteGame::getNextMove()
{
	Loc move;
	MoveNumber mn = _moveHist.getLastMoveNumber();
    if (mn == 0) {
		move = _boardReps.getCentreLoc();
	} else {
	 	move = _moveSuggester.getNextMove(_currDepth, _ourColour);
		//assert(move.isValid());
	}
	return move;
}

bool PenteGame::needUtility()
{
	_weAreForced = false;

	if (not _currDepth) return false;

	if (_posStats.getWonBy()) {
		PGD(cout << "Game Already Over, needUtility" << endl;)
		return true;
	}

	// Look up the TT for this position
	if (_currDepth > 2)
	{
		_isInTT = _transpositionTable.lookup(*this, _ttVal);
		// evaluating the utility function also uses the looked up _ttVal
		// to save looking it up twice
		if (_isInTT) return true;
	} else {
		_isInTT = false;
	}
	
	// When do we need to evaluate the util func? max depth and max VCT depth?
	// Or VCT posns. where we are not forced? NOT(#Their4s==0 or #TheirTakes>0 and #theirCaps >= 8)
	// Or positions with no move suggestions.
	if (_currDepth < _normalDepth-1) return false;

	if (not _vctDepth) return _currDepth >= _normalDepth;

	_weAreForced = _posStats.isForced(_ourColour);
	return not _weAreForced;
}

UtilityValue PenteGame::getUtility()
{
	if (_isInTT) {
		return _ttVal;
	}

	MoveNumber lastMn = _moveHist.getLastMoveNumber() - 1;
	// lastMn 0, sc P1, depth 0 -> tc P1
	// lastMn 1, sc P2, depth 0 -> tc P2
	Colour searchColour = _ourColour;
	Colour turnColour = 1 + (1 + _currDepth + _ourColour) % 2; // TODO: incremental with otherPlayer?
	UtilityValue uv = _utilCalc.calcUtility(turnColour, searchColour, lastMn+1);
#ifdef DEBUG_SEARCH
	std::cout << ' ' << uv;
#endif
#if 1
	if (not _isInTT) {
		storeInTransTable(uv);
	}
	_isInTT = false;
#endif

	return uv;
}

bool PenteGame::needSearch()
{
//When do we need to do more search?
	if (_posStats.getWonBy()) {
		PGD(cout << "Game Already Over, don't needSearch" << endl;)
		return false;
	}

	if (_currDepth < _normalDepth) return true;

	if (_currDepth >= _vctDepth + _normalDepth) return false;

	bool theyAreForced = _posStats.isForced(otherPlayer(_ourColour));
	return _weAreForced or theyAreForced;
}

void PenteGame::storeInTransTable(UtilityValue uv)
{
	if (_currDepth > 2 && _currDepth < _normalDepth - 1)
	//if (_currDepth > 2) // TODO: Store VCT too
	{
		_transpositionTable.savePos(*this, uv);
	}
}

void PenteGame::setRules(RulesType rules)
{
	// Default to standard rules
	_allowCaptures = true;
	bool canWinByCaptures = true;
	_forceFirstMoveInCentre = false;
	_restrictSecondP1Move = false;

	if (rules == 't' or rules == 'T') { // TODO: to upper/lower
		_forceFirstMoveInCentre = true;
		_restrictSecondP1Move = true;
	} else if (rules == '5') {
		canWinByCaptures = false;
	}
	_posStats.setCanWinByCaptures(canWinByCaptures);
}

using namespace std;

void PenteGame::print() const
{
	MoveNumber mn = _moveHist.getLastMoveNumber() - 1;
	CapCount c1 = getCaptured(P1);
	CapCount c2 = getCaptured(P2);

	cout << "Move " << mn+1 << ": " << getMove(mn) << 
		"; Caps: " << (int)c1 << "-" << (int)c2 << ":" << endl;
	_boardReps.print();
}

void PenteGame::printHistory() const
{
	for (int i=0; i<_moveHist.getLastMoveNumber(); i++) {
		Loc move = _moveHist.getMoved(i);
		cout << move << endl;
	}
}

