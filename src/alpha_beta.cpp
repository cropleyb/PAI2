#include "alpha_beta.h"

#include <iostream>
using std::cout;
using std::endl;

Loc AlphaBeta::getBestMove()
{
	cout << "1 in getBestMove" << endl;
    if (_bridge.isOneMove())
    {
		cout << "2 in getBestMove" << endl;
        // Optimisation: one move, don't search
        Loc theMove = _bridge.getOnlyMove();
        return theMove;
    }

    UtilityValue alpha = NEGINF;
    UtilityValue beta = INF;

	cout << "3 in getBestMove" << endl;
    std::pair<UtilityValue, Loc> val = maxValue(alpha, beta, 0);
	cout << "4 in getBestMove:" << val.first << endl;
    return val.second;
}

std::pair<UtilityValue, Loc> AlphaBeta::maxValue(UtilityValue alpha, UtilityValue beta, unsigned char depth)
{
	cout << "1 in maxValue - depth: " << (int)depth << endl;
    if (_bridge.isCutoff(depth))
    {
        std::pair<UtilityValue, Loc> utilMove = _bridge.getUtilityAndMove();
		cout << "2 in maxValue - getUtilityAndMove gave: " << utilMove.first << endl;
        return utilMove;
    }

	cout << "3 in maxValue" << endl;
    std::pair<UtilityValue,Loc> best(200*NEGINF, Loc(-1,-1));
    std::pair<UtilityValue,Loc> curr;

    while (true)
    {
        // TODO: optimise out maintainence of move suggester for 2nd
        // deepest level moves. This probably goes in the bridge though

		cout << "4 in maxValue" << endl;
        // Make and locally store the next suggested move
        Loc move = _bridge.makeNextMove();

        // No more moves at this level
        if (!move.isValid())
        {
			cout << "5 in maxValue" << endl;
            break;
        }

		cout << "6 in maxValue" << endl;
        // Recursively calculate the worst forcible utility value
        // for the resultant position
        curr = minValue(alpha, beta, depth+1);

		cout << "7 in maxValue" << endl;
        // Undo the move made above
        _bridge.undoLastMove();
		cout << "undo to depth " << (int)depth << " in maxValue" << endl;

        UtilityValue val = curr.first;
		cout << "7.5 in maxValue - val: " << val << endl;

        if (val > best.first)
        {
			cout << "8 in maxValue - update best to: " << val << endl;
            best.first = val;
            best.second = curr.second;
        }
        if (val >= beta)
		{
			cout << "9 in maxValue" << endl;
            break;
        }
        if (val > INF/100.0)
		{
			cout << "10 in maxValue" << endl;
            // Game won, can't get a better value
            break;
        }
		cout << "11 in maxValue" << endl;
        if (alpha > val)
		{
			cout << "12 in maxValue" << endl;
            alpha = val;
		}
		cout << "13 in maxValue" << endl;
    }
#if 0
    // TODO: Save to transposition table
    // game.save_utility(state, depth, v)
#endif
	cout << "14 in maxValue" << endl;
    return best;
}

std::pair<UtilityValue, Loc> AlphaBeta::minValue(UtilityValue alpha, UtilityValue beta, unsigned char depth)
{
	cout << "1 in minValue - depth: " << (int)depth << endl;
    if (_bridge.isCutoff(depth))
    {
		cout << "2 in minValue" << endl;
        std::pair<UtilityValue, Loc> utilMove = _bridge.getUtilityAndMove();
        return utilMove;
    }
	cout << "3 in minValue" << endl;
    std::pair<UtilityValue,Loc> best(200*INF, Loc(-1,-1));
    std::pair<UtilityValue,Loc> curr;

    while (true)
    {
        // TODO: optimise out maintainence of move suggester for 2nd
        // deepest level moves. This probably goes in the bridge though

		cout << "4 in minValue" << endl;
        // Make and locally store the next suggested move
        Loc move = _bridge.makeNextMove();

        // No more moves at this level
        if (!move.isValid())
        {
			cout << "5 in minValue" << endl;
            break;
        }

		cout << "6 in minValue" << endl;
        // Recursively calculate the worst forcible utility value
        // for the resultant position
        curr = maxValue(alpha, beta, depth+1);

		cout << "7 in minValue" << endl;
        // Undo the move made above
        _bridge.undoLastMove();

		cout << "undo to depth " << (int)depth << " in minValue" << endl;

        UtilityValue val = curr.first;
		cout << "7.5 in minValue - val: " << val << endl;
        if (val < best.first)
        {
			cout << "8 in minValue - update best to: " << val << endl;
            best.first = val;
            best.second = curr.second;
        }
        if (val <= alpha)
		{
			cout << "9 in minValue" << endl;
            break;
        }
        if (val < NEGINF/100.0)
		{
			cout << "10 in minValue" << endl;
            // Game lost, can't get a better value
            break;
        }
		cout << "11 in minValue" << endl;
        if (beta < val)
		{
			cout << "12 in minValue" << endl;
            beta = val;
		}
		cout << "13 in minValue" << endl;
    }
#if 0
    // TODO: Save to transposition table
    // game.save_utility(state, depth, v)
#endif
	cout << "14 in minValue" << endl;
    return best;
}

#if 0
from pentai.base.defines import INFINITY
from pentai.base.pente_exceptions import *
import pentai.base.logger as log

INF = INFINITY
NEGINF = -INFINITY

debug=False

def argmax(aspi, fn):
    """ aspi: action state pair iterator
    """
    '''
    try:
        curr_state = aspi.next()
    except StopIteration:
        log.warn("No moves at all!")
        raise NoMovesException()

    #import pdb
    #pdb.set_trace()
    try:
        #print "Second next call"
        next_state = aspi.next()
    except StopIteration:
        # Don't bother searching, there is only one move
        # TODO: This is assuming that there is only one move because
        # they have been correctly prioritised, and that the value
        # returned by the search is irrelevant.
        log.debug("Only one move")
        return curr_state, 0

    #import pdb
    #pdb.set_trace()
    #st()
    val = fn(curr_state)
    best = val, curr_state

    curr_state = next_state
    val = fn(curr_state)
    if val > best[0]:
        best = val, curr_state
    curr_state[1].undo_last_move()
    '''

    best = -200*INF, (None, None)

    for curr_state in aspi:
        val = fn(curr_state)
        if val > best[0]:
            best = val, curr_state
        curr_state[1].undo_last_move()

    # We only actually use the action (best[1][0])
    return best[1], best[0]

def alphabeta_search(state, game):
    """Search game to determine best action; use alpha-beta pruning.
    This version cuts off search and uses an evaluation function."""

    def max_value(state, alpha, beta, depth):
        if cutoff_test(state, depth):
            return game.utility(state, depth)
        v = NEGINF
        save_vs = []
        i = -1
        #for i, (a, s) in enumerate(game.successors(state, depth)):
        for (a, s) in game.successors(state, depth):
            i += 1
            curr_v = min_value(s, alpha, beta, depth+1)
            s.undo_last_move()
            save_vs.append(curr_v)
            v = max(v, curr_v)
            if v >= beta:
                break
            if v > INFINITY/100.0:
                # Game won, can't get better
                break
            alpha = max(alpha, v)

            if game.use_bl_cutoff():
                if i == 0:
                    best_val = v
                    worst_val = v
                else:
                    if v > best_val:
                        best_val = v
                    elif v < worst_val:
                        v = best_val
                        break

        """ Save to transposition table """
        game.save_utility(state, depth, v)

        return v

    def min_value(state, alpha, beta, depth):
        if cutoff_test(state, depth):
            return game.utility(state, depth)
        v = INF
        save_vs = []
        #for i, (a, s) in enumerate(game.successors(state, depth)):
        i = -1
        for (a, s) in game.successors(state, depth):
            i += 1
            curr_v = max_value(s, alpha, beta, depth+1)
            s.undo_last_move()
            save_vs.append(curr_v)
            v = min(v, curr_v)
            if v <= alpha:
                break
            if v < NEGINF/100.0:
                # Game lost, can't get worse
                # (for P1, but P2 is trying to minimise)
                break
            beta = min(beta, v)

            if game.use_bl_cutoff():
                if i == 0:
                    best_val = v
                    worst_val = v
                else:
                    if v < best_val:
                        best_val = v
                    elif v > worst_val:
                        v = best_val
                        break

        """ Save to transposition table """
        game.save_utility(state, depth, v)

        return v

    def cutoff_test(state, depth):
        """ Has the search been interrupted, or do we have a match in the
            transposition table? """

        return game.terminal_test(state, depth)

    def top_min_func(pair):
        a, s = pair
        return min_value(s, NEGINF, INF, 1)

    # Body of alphabeta_search starts here:
    action, value = argmax(game.successors(state, 1), top_min_func)
    return action, value

#endif

