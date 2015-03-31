#include "pente_game.h"
#include "line_lookup_table.h"
#include "span_lookup_table.h"
#include "board_strip.h"

#include <iostream>

PenteGame *_penteGame_ = 0;

PenteGame::PenteGame()
	: _moveSuggester(_posStats),
	  _boardReps(19,_posStats),
	  _utilCalc(_posStats),
	  _currDepth(0),
	  _maxDepth(4),
	  _ourColour(EMPTY)
{
	buildLineLookupTable();
	buildSpanTable(19);
	setRules("");
	_penteGame_ = this;
}

PenteGame::~PenteGame() {
	_penteGame_ = 0;
}

bool PenteGame::isLegalMove(Loc l) const
{
	if (l[0] >= _boardReps.getBoardSize()) return false;
	if (l[1] >= _boardReps.getBoardSize()) return false;
	if (_moveHist.getLastMoveNumber() == 0 && _forceFirstMoveInCentre)
	{
		return _boardReps.isCentreOfBoard(l);
	}
	if (_moveHist.getLastMoveNumber() == 2 && _restrictSecondP1Move)
	{
		return !_boardReps.isTournamentExcluded(l);
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
	if (pl.getCount(l) > 0) {
		setAndRecordCaptures(l, p);
	}
#ifdef DEBUG
	bool ok=
#endif
	_boardReps.setOcc(l, p);
#ifdef DEBUG
	assert(ok);
#endif
	_posStats.updateCheckerboardStats(p, l, 1);
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
	_boardReps.setBoardSize(bs);
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
	_posStats.updateCheckerboardStats(otherPlayer(p), capLoc1, -1);
	_posStats.updateCheckerboardStats(otherPlayer(p), capLoc2, -1);

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

	_posStats.updateCheckerboardStats(movedPlayer, l, -1);

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
				_posStats.updateCheckerboardStats(capturedPlayer, capLoc1, 1);
				_posStats.updateCheckerboardStats(capturedPlayer, capLoc2, 1);
			}
		}
	}
}

Loc
PenteGame::makeNextMove()
{
    Loc move = _moveSuggester.getNextMove(_currDepth, _ourColour);
	if (move.isValid())
		makeMove(move, (1 + (++_currDepth + _ourColour) % 2));
	return move;
}

Loc
PenteGame::getNextMove()
{
    Loc move = _moveSuggester.getNextMove(_currDepth, _ourColour);
	return move;
}

bool PenteGame::isCutoff() const
{
	if (_posStats.getWonBy()) return true;
	return _currDepth >= _maxDepth;
}

UtilityValue PenteGame::getUtility()
{
#if 0
	if (_currDepth > 2 && _currDepth < _maxDepth - 1)
	{
		UtilityValue ttVal;
		bool isInTT = _transpositionTable.lookup(*this, ttVal);
		if (isInTT) return ttVal;
	}
#endif

	MoveNumber lastMn = _moveHist.getLastMoveNumber() - 1;
	// lastMn 0, sc P1, depth 0 -> tc P1
	// lastMn 1, sc P2, depth 0 -> tc P2
	Colour searchColour = _ourColour;
	Colour turnColour = 1 + (1 + _currDepth + _ourColour) % 2; // TODO: incremental with otherPlayer?
	UtilityValue uv = _utilCalc.calcUtility(turnColour, searchColour, lastMn+1);
#ifdef DEBUG_SEARCH
	std::cout << ' ' << uv;
#endif
#if 0
	if (_currDepth > 2 && _currDepth < _maxDepth - 1)
	{
		_transpositionTable.savePos(*this, uv);
	}
#endif

	return uv;
}

using namespace std;

void PenteGame::print() const
{
	MoveNumber mn = getLastMoveNumber() - 1;
	CapCount c1 = getCaptured(P1);
	CapCount c2 = getCaptured(P2);

	cout << "Move " << mn+1 << ": " << getMove(mn) << 
		"; Caps: " << (int)c1 << "-" << (int)c2 << ":" << endl;
	_boardReps.print();
}

void PenteGame::setRules(const std::string& rulesStr)
{
	// Default to standard rules
	_allowCaptures = true;
	_forceFirstMoveInCentre = false;
	_restrictSecondP1Move = false;

	if (rulesStr.size() == 0) return;

	char rulesChar = ::tolower(rulesStr[0]);

	if (rulesChar == 't') {
		_forceFirstMoveInCentre = true;
		_restrictSecondP1Move = true;
	} else if (rulesChar == '5') {
		_allowCaptures = false;
	}
}
