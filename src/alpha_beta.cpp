#include "alpha_beta.h"
#include "bdebug.h"

#define INF BIG_NUM
#define NEGINF -BIG_NUM

// There is also DEBUG_SEARCH, which is across a few files.
//#define ABDEBUG
//#include <iostream>
//using namespace std;

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

    UtilityValue alpha = 1000 * NEGINF;
    UtilityValue beta = 1000 * INF;

    UtilityValue val = maxValue(alpha, beta, 0);
	assert(_bestTopLevelMove.isValid());
    return _bestTopLevelMove;
}

UtilityValue AlphaBeta::maxValue(UtilityValue alpha, UtilityValue beta, Depth depth)
{
    UtilityValue bestVal(200*NEGINF);

	bool needSearch = _bridge.needSearch();
    if (not needSearch or _bridge.needUtility())
    {
		ABD(cout << "maxValue needs utility" << endl;)
        bestVal = _bridge.getUtility();
    }

    UtilityValue currVal;
	Loc bestMove = Loc::INVALID;

	if (needSearch) {
		ABD(cout << "starting maxValue loop" << endl;)

		while (true)
		{
			// Make and locally store the next suggested move
			Loc currMove = _bridge.makeNextMove(); // determines if vct by depth > max depth

			// No more moves at this level
			if (!currMove.isValid()) {
				ABD(cout << "next move invalid in maxValue" << endl;)
				break;
			}

			// Recursively calculate the worst forcible utility value
			// for the resultant position
			currVal = minValue(alpha, beta, depth+1);

			// Undo the currMove made above
			_bridge.undoLastMove();

			UtilityValue val = currVal;

			if (depth == 0) {
				ABD(cout << "================ maxValue: top level option " << currMove << ": " << currVal << endl;)
			}

			if (val > bestVal or (depth == 0 and not bestMove.isValid())) {
				bestVal = val;
				if (depth == 0) {
					bestMove = currMove;
				}
			}
			if (val >= beta) {
				ABD(cout << "val: " << val << " >= beta: " << beta << endl;)
				break;
			}
			if (val > INF/100.0 and depth > 0) {
				// Game won, probably can't get a better value; search order should
				// handle it.
				// depth 0 needs to search all options to find the fastest win.
				ABD(cout << "game won" << endl;)
				break;
			}
			if (alpha > val) {
				alpha = val;
			}
		}
#if 1
		_bridge.storeInTransTable(bestVal);
#endif
	}
	if (depth == 0) {
        assert(bestMove.isValid());
		_bestTopLevelMove = bestMove;
	}
#ifdef DEBUG_SEARCH
	//ABD(cout << " [Best(max):" << bestVal << "] ";)
#endif
    return bestVal;
}

UtilityValue AlphaBeta::minValue(UtilityValue alpha, UtilityValue beta, Depth depth)
{
    UtilityValue bestVal(200*INF);

	bool needSearch = _bridge.needSearch();
    if (not needSearch or _bridge.needUtility())
    {
		ABD(cout << "minValue needs utility" << endl;)
        bestVal = _bridge.getUtility();
    }

    UtilityValue currVal;

	if (needSearch) {
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
				// Game won, probably can't get a better value; search order should
				// handle it.
				ABD(cout << "game lost at depth: " << (int)depth << endl;)
				break;
			}
			if (beta < val) {
				beta = val;
			}
		}
#if 1
		_bridge.storeInTransTable(bestVal);
#endif
	}

#ifdef DEBUG_SEARCH
	//cout << " [Best(min):" << bestVal << "] ";
#endif
    return bestVal;
}

