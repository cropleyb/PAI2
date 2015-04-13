#include "alpha_beta.h"
#include "bdebug.h"

#define INF 1e40
#define NEGINF -1e40

Loc AlphaBeta::getBestMove()
{
    _bridge.clearTT();

    if (_bridge.isOnlyOneMove())
    {
        // Optimisation: one move, don't search
#if 1
        Loc theMove = _bridge.getNextMove();
#else
        Loc theMove = _bridge.makeNextMove();
		_bridge.undoLastMove();
#endif
		assert(theMove.isValid());
        return theMove;
    }

    UtilityValue alpha = NEGINF;
    UtilityValue beta = INF;

    UtilityValue val = maxValue(alpha, beta, 0);
	assert(_bestTopLevelMove.isValid());
    return _bestTopLevelMove;
}

UtilityValue AlphaBeta::maxValue(UtilityValue alpha, UtilityValue beta, Depth depth)
{
    if (_bridge.isCutoff())
    {
        UtilityValue util = _bridge.getUtility();
        return util;
    }

    UtilityValue bestVal(200*NEGINF);
    UtilityValue currVal;
	Loc bestMove;

    while (true)
    {
        // TODO: optimise out maintainence of move suggester info 
		// within PositionStats for 2nd deepest level moves.
		// This probably goes in the bridge though.

        // Make and locally store the next suggested move
        Loc currMove = _bridge.makeNextMove();

        // No more moves at this level
        if (!currMove.isValid())
        {
            break;
        }

        // Recursively calculate the worst forcible utility value
        // for the resultant position
        currVal = minValue(alpha, beta, depth+1);

        // Undo the currMove made above
        _bridge.undoLastMove();

        UtilityValue val = currVal;

        if (val > bestVal) {
            bestVal = val;
			if (depth == 0) {
				bestMove = currMove;
			}
        }
        if (val >= beta) {
            break;
        }
        if (val > INF/100.0) {
            // Game won, can't get a better value
            break;
        }
        if (alpha > val) {
            alpha = val;
		}
    }
#if 0
    // TODO: Save to transposition table
	_bridge.storeInTransTable(uv);
#endif
	if (depth == 0) {
		_bestTopLevelMove = bestMove;
	}
#ifdef DEBUG_SEARCH
	cout << " [Best(max):" << bestVal << "] ";
#endif
    return bestVal;
}

UtilityValue AlphaBeta::minValue(UtilityValue alpha, UtilityValue beta, Depth depth)
{
    if (_bridge.isCutoff()) {
        UtilityValue util = _bridge.getUtility();
        return util;
    }
    UtilityValue bestVal(200*INF);
    UtilityValue currVal;

    while (true) {
        // TODO: optimise out maintainence of move suggester for 2nd
        // deepest level moves. This probably goes in the bridge though

        // Make and locally store the next suggested move
        Loc currMove = _bridge.makeNextMove();

        // No more moves at this level
        if (!currMove.isValid()) {
            break;
        }

        // Recursively calculate the worst forcible utility value
        // for the resultant position
        currVal = maxValue(alpha, beta, depth+1);

        // Undo the currMove made above
        _bridge.undoLastMove();

        UtilityValue val = currVal;
        if (val < bestVal) {
            bestVal = val;
        }
        if (val <= alpha) {
            break;
        }
        if (val < NEGINF/100.0) {
            // Game lost, can't get a better value
            break;
        }
        if (beta < val) {
            beta = val;
		}
    }
#ifdef DEBUG_SEARCH
	cout << " [Best(min):" << bestVal << "] ";
#endif
    return bestVal;
}

