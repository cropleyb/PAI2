#include "alpha_beta.h"
#include "bdebug.h"

Loc AlphaBeta::getBestMove()
{
	BD(cout << "1 getBestMove" << endl);
    if (_bridge.isOneMove())
    {
		BD(cout << "2 getBestMove" << endl);
        // Optimisation: one move, don't search
        Loc theMove = _bridge.getOnlyMove();
        return theMove;
    }

    UtilityValue alpha = NEGINF;
    UtilityValue beta = INF;

	BD(cout << "3 getBestMove");
    std::pair<UtilityValue, Loc> val = maxValue(alpha, beta, 0);
	BD(cout << "4 getBestMove: " << val.first << endl);
    return val.second;
}

std::pair<UtilityValue, Loc> AlphaBeta::maxValue(UtilityValue alpha, UtilityValue beta, Depth depth)
{
	BD(cout << "1 in maxValue - depth: " << (int)depth << endl);
    if (_bridge.isCutoff())
    {
        std::pair<UtilityValue, Loc> utilMove = _bridge.getUtilityAndMove();
		BD(cout << "2 in maxValue - getUtilityAndMove gave: " << utilMove.first << endl);
        return utilMove;
    }

	BD(cout << "3 in maxValue" << endl);
    std::pair<UtilityValue,Loc> best(200*NEGINF, Loc(-1,-1));
    std::pair<UtilityValue,Loc> curr;

    while (true)
    {
        // TODO: optimise out maintainence of move suggester info 
		// within PositionStats for 2nd deepest level moves.
		// This probably goes in the bridge though.

		BD(cout << "4 in maxValue" << endl);
        // Make and locally store the next suggested move
        Loc move = _bridge.makeNextMove();

        // No more moves at this level
        if (!move.isValid())
        {
			BD(cout << "5 in maxValue" << endl);
            break;
        }

		BD(cout << "6 in maxValue" << endl);
        // Recursively calculate the worst forcible utility value
        // for the resultant position
        curr = minValue(alpha, beta, depth+1);

		BD(cout << "7 in maxValue" << endl);
        // Undo the move made above
        _bridge.undoLastMove();
		BD(cout << "undo to depth " << (int)depth << " in maxValue" << endl);

        UtilityValue val = curr.first;
		BD(cout << "7.5 in maxValue - val: " << val << endl);

        if (val > best.first)
        {
			BD(cout << "8 in maxValue - update best to: " << val << endl);
            best.first = val;
            best.second = curr.second;
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
    return best;
}

std::pair<UtilityValue, Loc> AlphaBeta::minValue(UtilityValue alpha, UtilityValue beta, Depth depth)
{
	BD(cout << "1 in minValue - depth: " << (int)depth << endl);
    if (_bridge.isCutoff())
    {
		BD(cout << "2 in minValue" << endl);
        std::pair<UtilityValue, Loc> utilMove = _bridge.getUtilityAndMove();
        return utilMove;
    }
	BD(cout << "3 in minValue" << endl);
    std::pair<UtilityValue,Loc> best(200*INF, Loc(-1,-1));
    std::pair<UtilityValue,Loc> curr;

    while (true)
    {
        // TODO: optimise out maintainence of move suggester for 2nd
        // deepest level moves. This probably goes in the bridge though

		BD(cout << "4 in minValue" << endl);
        // Make and locally store the next suggested move
        Loc move = _bridge.makeNextMove();

        // No more moves at this level
        if (!move.isValid())
        {
			BD(cout << "5 in minValue" << endl);
            break;
        }

		BD(cout << "6 in minValue" << endl);
        // Recursively calculate the worst forcible utility value
        // for the resultant position
        curr = maxValue(alpha, beta, depth+1);

		BD(cout << "7 in minValue" << endl);
        // Undo the move made above
        _bridge.undoLastMove();

		BD(cout << "undo to depth " << (int)depth << " in minValue" << endl);

        UtilityValue val = curr.first;
		BD(cout << "7.5 in minValue - val: " << val << endl);
        if (val < best.first)
        {
			BD(cout << "8 in minValue - update best to: " << val << endl);
            best.first = val;
            best.second = curr.second;
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
    return best;
}

