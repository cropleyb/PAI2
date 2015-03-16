#include "pente_game.h"
#include "line_lookup_table.h"
#include "span_lookup_table.h"
#include "board_strip.h"

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
}

#include <iostream>

void PenteGame::makeMove(Loc l, Colour p)
{
	//std::cout << std::endl;
	//for (int d=0; d<_currDepth; d++)
		//std::cout << ". ";
	//std::cout << l;
	_captureDirs = 0;

	const PriorityLevel &pl = _posStats.getPriorityLevel(p, Take);
	if (pl.getCount(l) > 0) {
		setAndRecordCaptures(l, p);
	}

	_boardReps.setOcc(l, p);
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

void PenteGame::reportCapture(const SpanEntry &span, bool left, Colour p)
{
	//std::cout << " (cap) [" << (int)p << ']';
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

	int fullCircleDir = span._direction;
	if (left)
	{
		fullCircleDir += 4;
	}
	_captureDirs |= 1 << fullCircleDir;
}

void PenteGame::undoLastMove()
{
	//std::cout << " U ";
	MoveNumber mn = _moveHist.getLastMoveNumber();
	_currDepth--;
	
	Loc l = _moveHist.getMoved(mn);
	CaptureDirs cd = _moveHist.getCapDirs(mn);
	_moveHist.undoLastMove();

	_boardReps.setOcc(l, EMPTY);

	if (cd)
	{
		Colour capturingPlayer = 1 + (mn) % 2;
		Colour capturedPlayer = 1 + (mn + 1) % 2;

		// Must undo some captures
		for (int dir=0; dir<8; dir++)
		{
			if (cd & (1 << dir))
			{
				//std::cout << " (uncap) [" << (int)capturingPlayer << ']';
				// THIS IS GIVING THE WRONG COLOUR. OR IS IT THE FORWARD CAP?
				_posStats.reportCaptured(capturingPlayer, 2, -1);

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
	return _currDepth >= _maxDepth;
}

UtilityValue PenteGame::getUtility()
{
	MoveNumber lastMn = _moveHist.getLastMoveNumber();
	// lastMn 0, sc P1, depth 0 -> tc P1
	// lastMn 1, sc P2, depth 0 -> tc P2
	//Colour searchColour = 1 + (lastMn % 2); // TODO: Cacheable
	//Colour turnColour = 1 + (lastMn + _currDepth) % 2; // TODO: incremental with otherPlayer.
	Colour searchColour = _ourColour;
	Colour turnColour = 1 + (1 + _currDepth + _ourColour) % 2; // TODO: incremental with otherPlayer.
	UtilityValue uv = _utilCalc.calcUtility(turnColour, searchColour, lastMn+1);
	// std::cout << uv;
	return uv;
}

