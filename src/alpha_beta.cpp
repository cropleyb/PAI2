#include "alpha_beta.h"
#include "bdebug.h"

#define INF BIG_NUM
#define NEGINF -BIG_NUM

// There is also DEBUG_SEARCH, which is across a few files.
//#define ABDEBUG
#ifdef ABDEBUG
#define ABD(X) X
#include <iostream>
using namespace std;
#else
#define ABD(X)
#endif

Loc AlphaBeta::getBestMove()
{
    _bridge.resetSearch();

    if (_bridge.isOnlyOneMove())
    {
        // Optimisation: one move, don't search
        Loc theMove = _bridge.getNextMove();
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
    UtilityValue bestVal(200*NEGINF);

    if (_bridge.needUtility())
    {
		ABD(cout << "maxValue needs utility" << endl;)
        bestVal = _bridge.getUtility();
    }
	if (!_bridge.needSearch()) {
		ABD(cout << "maxValue doesn't need search at depth: " << (int)depth << endl;)
		return bestVal;
	}

    UtilityValue currVal;
	Loc bestMove = Loc::INVALID;

    while (true)
    {
        // Make and locally store the next suggested move
        Loc currMove = _bridge.makeNextMove(); // determines if vct by depth > max depth

        // No more moves at this level
        if (!currMove.isValid()) {
#if 0
			// TODO?: If no moves, calc util
			if (bestVal <= 200*NEGINF);
				bestVal = _bridge.getUtility();
#endif
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
			ABD(cout << "val: " << val << " >= beta: " << beta << endl;)
            break;
        }
        if (val > INF/100.0) {
			ABD(cout << "game won" << endl;)
            // Game won, can't get a better value
            break;
        }
        if (alpha > val) {
            alpha = val;
		}
    }
#if 1
	_bridge.storeInTransTable(bestVal);
#endif
	if (depth == 0) {
        assert(bestMove.isValid());
		_bestTopLevelMove = bestMove;
	}
#ifdef DEBUG_SEARCH
	ABD(cout << " [Best(max):" << bestVal << "] ";)
#endif
    return bestVal;
}

UtilityValue AlphaBeta::minValue(UtilityValue alpha, UtilityValue beta, Depth depth)
{
    UtilityValue bestVal(200*INF);

    if (_bridge.needUtility())
    {
		ABD(cout << "minValue needs utility" << endl;)
        bestVal = _bridge.getUtility();
    }
	if (!_bridge.needSearch()) {
		ABD(cout << "minValue doesn't need search" << endl;)
		return bestVal;
	}

    UtilityValue currVal;

    while (true) {
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
			ABD(cout << "val: " << val << " <= alpha: " << alpha << endl;)
            break;
        }
        if (val < NEGINF/100.0) {
            // Game lost, can't get a better value
			ABD(cout << "game lost" << endl;)
            break;
        }
        if (beta < val) {
            beta = val;
		}
    }
#if 1
	_bridge.storeInTransTable(bestVal);
#endif
#ifdef DEBUG_SEARCH
	cout << " [Best(min):" << bestVal << "] ";
#endif
    return bestVal;
}

