#ifndef _move_history_h
#define _move_history_h

#include "defines.h"
#include "loc.h"

#define MAX_MOVES 450

class MoveHistory
{
public:
	MoveHistory() : _moveNumber(0)
	{
		for (int i=0; i<MAX_MOVES; i++)
		{
			_moveHist[i] = Loc::INVALID;
			_capHist[i] = (CaptureDirs)0;
		}
	}

	// Returns the next move number
    MoveNumber saveMove(Loc move, Colour /* p */, CaptureDirs cd)
	{
		//assert(_moveNumber % 2 == (int)p - 1);
		_moveHist[_moveNumber] = move;
		_capHist[_moveNumber] = cd;
		_moveNumber++;
		return _moveNumber;
	}

	void undoLastMove()
	{
		_moveNumber--;
	}

	Loc getMoved(MoveNumber mn) const
	{
		return _moveHist[mn];
	}

	CaptureDirs getCapDirs(MoveNumber mn) const
	{
		return _capHist[mn];
	}

	MoveNumber getLastMoveNumber() const
	{
		return _moveNumber - 1;
	}

	Loc getLastMove() const
	{
		return _moveHist[getLastMoveNumber()];
	}

private:
	MoveNumber _moveNumber;
	Loc _moveHist[MAX_MOVES];
	CaptureDirs _capHist[MAX_MOVES];
};

#endif
