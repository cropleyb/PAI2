#include "alpha_beta.h"
#include "bdebug.h"

Loc AlphaBeta::getBestMove()
{
	BD(cout << "1 getBestMove" << endl);
    if (_bridge.isOnlyOneMove())
    {
		BD(cout << "2 getBestMove" << endl);
        // Optimisation: one move, don't search
        Loc theMove = _bridge.getNextMove();
        return theMove;
    }

    UtilityValue alpha = NEGINF;
    UtilityValue beta = INF;

	BD(cout << "3 getBestMove");
    UtilityValue val = maxValue(alpha, beta, 0);
	BD(cout << "4 getBestMove: " << val.first << endl);
    return _bestTopLevelMove;
}

UtilityValue AlphaBeta::maxValue(UtilityValue alpha, UtilityValue beta, Depth depth)
{
	BD(cout << "1 in maxValue - depth: " << (int)depth << endl);
    if (_bridge.isCutoff())
    {
        UtilityValue util = _bridge.getUtility();
		BD(cout << "2 in maxValue - getUtility gave: " << utilMove << endl);
        return util;
    }

	BD(cout << "3 in maxValue" << endl);
    UtilityValue bestVal(200*NEGINF);
    UtilityValue currVal;
	Loc bestMove;

    while (true)
    {
        // TODO: optimise out maintainence of move suggester info 
		// within PositionStats for 2nd deepest level moves.
		// This probably goes in the bridge though.

		BD(cout << "4 in maxValue" << endl);
        // Make and locally store the next suggested move
        Loc currMove = _bridge.makeNextMove();

        // No more moves at this level
        if (!currMove.isValid())
        {
			BD(cout << "5 in maxValue" << endl);
            break;
        }

		BD(cout << "6 in maxValue" << endl);
        // Recursively calculate the worst forcible utility value
        // for the resultant position
        currVal = minValue(alpha, beta, depth+1);

		BD(cout << "7 in maxValue" << endl);
        // Undo the currMove made above
        _bridge.undoLastMove();
		BD(cout << "undo to depth " << (int)depth << " in maxValue" << endl);

        UtilityValue val = currVal;
		BD(cout << "7.5 in maxValue - val: " << val << endl);

        if (val > bestVal)
        {
			BD(cout << "8 in maxValue - update bestVal to: " << val << endl);
            bestVal = val;
			if (depth == 0)
			{
				bestMove = currMove;
			}
        }
        if (val >= beta)
		{
			BD(cout << "9 in maxValue" << endl);
            break;
        }
        if (val > INF/100.0)
		{
			BD(cout << "10 in maxValue" << endl);
            // Game won, can't get a better value
            break;
        }
		BD(cout << "11 in maxValue" << endl);
        if (alpha > val)
		{
			BD(cout << "12 in maxValue" << endl);
            alpha = val;
		}
		BD(cout << "13 in maxValue" << endl);
    }
#if 0
    // TODO: Save to transposition table
    // game.save_utility(state, depth, v)
#endif
	BD(cout << "14 in maxValue" << endl);
	if (depth == 0)
	{
		_bestTopLevelMove = bestMove;
	}
    return bestVal;
}

UtilityValue AlphaBeta::minValue(UtilityValue alpha, UtilityValue beta, Depth depth)
{
	BD(cout << "1 in minValue - depth: " << (int)depth << endl);
    if (_bridge.isCutoff())
    {
		BD(cout << "2 in minValue" << endl);
        UtilityValue util = _bridge.getUtility();
        return util;
    }
	BD(cout << "3 in minValue" << endl);
    UtilityValue bestVal(200*INF);
    UtilityValue currVal;

    while (true)
    {
        // TODO: optimise out maintainence of move suggester for 2nd
        // deepest level moves. This probably goes in the bridge though

		BD(cout << "4 in minValue" << endl);
        // Make and locally store the next suggested move
        Loc currMove = _bridge.makeNextMove();

        // No more moves at this level
        if (!currMove.isValid())
        {
			BD(cout << "5 in minValue" << endl);
            break;
        }

		BD(cout << "6 in minValue" << endl);
        // Recursively calculate the worst forcible utility value
        // for the resultant position
        currVal = maxValue(alpha, beta, depth+1);

		BD(cout << "7 in minValue" << endl);
        // Undo the currMove made above
        _bridge.undoLastMove();

		BD(cout << "undo to depth " << (int)depth << " in minValue" << endl);

        UtilityValue val = currVal;
		BD(cout << "7.5 in minValue - val: " << val << endl);
        if (val < bestVal)
        {
			BD(cout << "8 in minValue - update best to: " << val << endl);
            bestVal = val;
        }
        if (val <= alpha)
		{
			BD(cout << "9 in minValue" << endl);
            break;
        }
        if (val < NEGINF/100.0)
		{
			BD(cout << "10 in minValue" << endl);
            // Game lost, can't get a better value
            break;
        }
		BD(cout << "11 in minValue" << endl);
        if (beta < val)
		{
			BD(cout << "12 in minValue" << endl);
            beta = val;
		}
		BD(cout << "13 in minValue" << endl);
    }
	BD(cout << "14 in minValue" << endl);
    return bestVal;
}

