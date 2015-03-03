
#include "move_suggester.h"
#include "position_stats.h"
#include "candidate_cache.h"
#include "defines.h"
#include "bdebug.h"

MoveSuggester::MoveSuggester(PositionStats &ps, CandidateCache &cc)
	: _posStats(ps), _candCache(cc)
{
}

Loc MoveSuggester::getNextMove(Depth depth)
{
	if (_candCache.needsFilling(depth))
	{
		Loc *moveBuffer = _candCache.getBuffer(depth);

		Breadth maxMoves = 9;
		if (depth > 3) maxMoves = 4;

		Breadth moveCount = filterCandidates(moveBuffer, depth, maxMoves, P1); // FIXME - our real colour
		BD(cout << "Setting depth moves for depth " << (int)depth << " to " << (int)moveCount << endl;)
		_candCache.setDepthMoves(depth, moveCount);
	}

	return _candCache.getNextMove(depth);
}

bool MoveSuggester::getPriorityLevels(Colour ourColour)
{
	bool onePoss;

	Colour theirColour = otherPlayer(ourColour);

	const PriorityLevel &ourFours
		= _posStats.getLengthPriorityLevel(ourColour, 4);
	if (ourFours.getNumCands() > 0)
	{
		// This will win
		_toSearchLevels[0] = &ourFours;
		_numSearchLevels = 1;
		onePoss = true;
		return onePoss;
	}

	CapCount ourCaptureCount = _posStats.getCaptured(ourColour);
	const PriorityLevel &ourTakes
		= _posStats.getTakesPriorityLevel(ourColour);

	if (ourCaptureCount >= 8 and ourTakes.getNumCands() > 0) {
		// This will win too
		// _toSearch.push_back(ourTakes);
		_toSearchLevels[0] = &ourTakes;
		_numSearchLevels = 1;
		onePoss = true;
		return onePoss;
	}

	CapCount theirCaptureCount = _posStats.getCaptured(theirColour);
	const PriorityLevel &theirTakes
		= _posStats.getTakesPriorityLevel(theirColour);

	if (theirCaptureCount >= 8 and theirTakes.getNumCands() > 0) {
		// Block their takes, or capture one of the ends of an
		// attacker, or lose
		//_toSearch.push_back(ourTakes);
		//_toSearch.push_back(theirTakes);
		_toSearchLevels[0] = &ourTakes;
		_toSearchLevels[1] = &theirTakes;
		_numSearchLevels = 2;
		onePoss = false;
		return onePoss;
	}

	const PriorityLevel &theirFours
		= _posStats.getLengthPriorityLevel(theirColour, 4);

	if (theirFours.getNumCands() > 0) {
		if (theirFours.getNumCands() > 1) {
			if (ourTakes.getNumCands() > 0) {
				// We will lose unless we capture
				// _toSearch.push_back(ourTakes);
				_toSearchLevels[0] = &ourTakes;
				_numSearchLevels = 1;
				onePoss = false;
				return onePoss;
			} else {
				// Might as well block one of them, can't stop 'em all
				// _toSearch.push_back(theirFours);
				_toSearchLevels[0] = &theirFours;
				_numSearchLevels = 1;
				onePoss = true;
				return onePoss;
			}
		}

		// We will lose unless we block or capture 
		//_toSearch.push_back(theirFours);
		//_toSearch.push_back(ourTakes);
		_toSearchLevels[0] = &theirFours;
		_toSearchLevels[1] = &ourTakes;
		_numSearchLevels = 2;
		onePoss = false;
		return onePoss;
	}
	// Defaulting to many levels
	fillPriorityLevels(ourColour, theirColour);
	_numSearchLevels = 10;
	onePoss = false;
	return onePoss;
}

#define GETTAKES(C) &_posStats.getTakesPriorityLevel(C)
#define GETTHREATS(C) &_posStats.getThreatsPriorityLevel(C)
#define GETLEVEL(C,L) &_posStats.getLengthPriorityLevel(C,L)

// TODO This could be cached.
void MoveSuggester::fillPriorityLevels(Colour ourColour, Colour theirColour)
{
	_toSearchLevels[0] = GETTAKES(ourColour);
	_toSearchLevels[1] = GETTAKES(theirColour);
	_toSearchLevels[2] = GETLEVEL(ourColour, 3);
	_toSearchLevels[3] = GETLEVEL(theirColour, 3);
	_toSearchLevels[4] = GETTHREATS(ourColour);
	_toSearchLevels[5] = GETTHREATS(theirColour);
	_toSearchLevels[6] = GETLEVEL(ourColour, 2);
	_toSearchLevels[7] = GETLEVEL(theirColour, 2);
	_toSearchLevels[8] = GETLEVEL(ourColour, 1);
	_toSearchLevels[9] = GETLEVEL(theirColour, 1);
}

Breadth MoveSuggester::filterCandidates(Loc *moveBuffer, Depth depth, Breadth maxMoves, Colour ourColour)
{
	Breadth found = 0;
	bool tried[MAX_LOCS];
	
	Colour searchColour = ourColour;
	if (depth % 2) {
		searchColour = otherPlayer(ourColour);
	}

	bool onePoss = getPriorityLevels(searchColour);
	if (onePoss) {
		maxMoves = 1;
	}

	for (int slotInd=0; slotInd<_numSearchLevels; slotInd++)
	{
		const PriorityLevel *pl = _toSearchLevels[slotInd];
		Breadth foundFromPL = pl->getCands(moveBuffer, maxMoves-found);
		BD(cout << "Found " << (int)foundFromPL << endl;)

		found += foundFromPL;
		if (found >= maxMoves)
		{
			BD(cout << "Found enough in MS" << endl;)
			return found;
		}
		moveBuffer += foundFromPL;
	}
	return found;
}

#if 0
	// TODO: sorting per level
	for slot in candidate_slots:
		#slot_arr = slot.iteritems()
		slot_arr = slot.get_iter()
		sorted_slot = [(count, loc) for (loc, count) in slot_arr]
		sorted_slot.sort(reverse=True)
		for count, loc in sorted_slot:
	return found;
#endif

#if 0
    def get_iter_inner(self, our_colour, state=None, depth=0, min_priority=0, tried={}): # min_priority is ignored
        #print "get_iter_inner"
        their_colour = opposite_colour(our_colour)
        candidate_slots, one_poss = self.get_priority_levels(our_colour)
        
        tried = set()
        for slot in candidate_slots:
            #slot_arr = slot.iteritems()
            slot_arr = slot.get_iter()
            sorted_slot = [(count, loc) for (loc, count) in slot_arr]
            sorted_slot.sort(reverse=True)
            for count, loc in sorted_slot:
                if count > 0:
                    if not loc.val in tried:
                        loc_tup = loc.get_tuple()
                        if self.vision < 100:
                            if random.random() * 100 > self.vision:
                                # Can't see that sorry ;)
                                print "Blind to that"
                                continue
                        # Check for 2nd P1 move
                        try:
                            if state:
                                # TODO: Rename to check_legality
                                state.is_illegal(loc_tup)
                        except IllegalMoveException:
                            print "Skipping illegal suggestion - probably tournament move restriction"
                            continue

                        tried.add(loc.val)
                        yield loc_tup

                        if one_poss:
                            #print "one_poss"
                            return
                        if len(tried) >= self.max_moves_func(depth):
                            return

        // if len(tried) == 0:
            #print "No candidates: %s " % candidate_slots
            #raise NoMovesException("In PF2!")
#endif

#if 0
from pentai.base.defines import *
from pentai.base.pente_exceptions import *
from pentai.base.loc import *
from pentai.ai.priority_level import *

#cimport cython

""" This one uses the PriorityLevel classes for hopefully rapid
    state updating """

def max_moves_sample_func(depth):
    return 9

class PriorityFilter6(object):
    def __init__(self, orig=None):
        self.reset(orig)

        self.max_moves_func = max_moves_sample_func
        if orig != None:
            self.max_moves_func = orig.max_moves_func
        self.vision = 100

    def set_vision(self, val):
        self.vision = val

    def set_our_colour(self, val):
        pass

    def reset(self, orig=None):
        if orig != None:
            self.candidates_by_priority_and_colour = \
                    orig.candidates_by_priority_and_colour
        else:
            cbpc = self.candidates_by_priority_and_colour = []
            for priority in range(6):
                l = []
                cbpc.append(l)
                for colour in range(3):
                    l.append(PriorityLevel())

        self.captured = [None, 0, 0]

    def copy(self):
        return PriorityFilter6(orig=self)

    def set_vision(self, val):
        self.vision = val

    def set_max_moves_func(self, mmf):
        self.max_moves_func = mmf

    def set_max_moves_per_depth_level(self, mmpdl, narrowing, chokes=[]):
        if narrowing != 0:
            def mmpdl_func(depth):
                # This is really hacky, but it seemed to work
                for d, w in chokes:
                    if depth >= d:
                        return w
                return mmpdl - round(narrowing * depth)
        else:
            def mmpdl_func(depth):
                # This is really hacky, but it seemed to work
                for d, w in chokes:
                    if depth >= d:
                        return w
                return mmpdl

        self.set_max_moves_func(mmpdl_func)

    # This isn't being called?!
    def get_captured(self, colour):
        return self.captured[colour]

    def priority_level(self, level, colour):
        return self.candidates_by_priority_and_colour[level][colour]

    def get_priority_levels(self, our_colour):
        their_colour = opposite_colour(our_colour)

        our_fours = self.priority_level(5, our_colour)
        if our_fours.get_num_cands() > 0:
            # This will win
            return [our_fours], True

        our_captures = self.get_captured(our_colour)
        our_takes = self.priority_level(4, our_colour)

        if our_captures >= 8 and our_takes.get_num_cands() > 0:
            # This will win too
            return [our_takes], True

        their_captures = self.get_captured(their_colour)
        their_takes = self.priority_level(4, their_colour)

        if their_captures >= 8 and their_takes.get_num_cands() > 0:
            # Block their takes, or capture one of the ends of an
            # attacker, or lose
            return [our_takes, their_takes], False

        their_fours = self.priority_level(5, their_colour)

        if their_fours.get_num_cands() > 0:
            if their_fours.get_num_cands() > 1:
                if our_takes.get_num_cands() > 0:
                    # We will lose unless we capture
                    return [our_takes], False
                else:
                    # Might as well block one of them, can't stop 'em all
                    return [their_fours], True

            # We will lose unless we block or capture 
            return [their_fours, our_takes], False

        ret = []
        #print "defaulting to many levels"
        for level in range(4, -1, -1):
            for colour in [our_colour, their_colour]:
                ret.append(self.priority_level(level, colour))
        return ret, False

    def get_iter(self, our_colour, state=None, depth=0, min_priority=0, tried={}):
        real_moves = list(self.get_iter_inner(our_colour, state, depth, min_priority, tried))
        #print "get_iter OUTER %s" % (real_moves,)

        for m in real_moves:
            yield m

    def get_iter_inner(self, our_colour, state=None, depth=0, min_priority=0, tried={}): # min_priority is ignored
        #print "get_iter_inner"
        their_colour = opposite_colour(our_colour)
        candidate_slots, one_poss = self.get_priority_levels(our_colour)
        
        tried = set()
        for slot in candidate_slots:
            #slot_arr = slot.iteritems()
            slot_arr = slot.get_iter()
            sorted_slot = [(count, loc) for (loc, count) in slot_arr]
            sorted_slot.sort(reverse=True)
            for count, loc in sorted_slot:
                if count > 0:
                    if not loc.val in tried:
                        loc_tup = loc.get_tuple()
                        if self.vision < 100:
                            if random.random() * 100 > self.vision:
                                # Can't see that sorry ;)
                                print "Blind to that"
                                continue
                        # Check for 2nd P1 move
                        try:
                            if state:
                                # TODO: Rename to check_legality
                                state.is_illegal(loc_tup)
                        except IllegalMoveException:
                            print "Skipping illegal suggestion - probably tournament move restriction"
                            continue

                        tried.add(loc.val)
                        yield loc_tup

                        if one_poss:
                            #print "one_poss"
                            return
                        if len(tried) >= self.max_moves_func(depth):
                            return

        // if len(tried) == 0:
            #print "No candidates: %s " % candidate_slots
            #raise NoMovesException("In PF2!")

    def __repr__(self):
        return "%s" % self.candidates_by_priority_and_colour[5]

    def add_or_remove_candidates(self, colour, length, pos_list, inc=1):
        if length == 5:
            # won already, ignore
            return
        if length == 4: # allow space for take priority
            length = 5
        if length < 3:  # allow space for threat priority
            length -= 1
        slot = self.candidates_by_priority_and_colour[length][colour]
        for pos in pos_list:
            assert pos[0] >= 0
            assert pos[1] >= 0
            //if pos == (9,8) and length == 5:
            #    print "INC: %s" % inc
            adjust_slot(self, slot, pos, inc)

    def add_or_remove_take(self, colour, pos, inc=1):
        assert pos[0] >= 0
        assert pos[1] >= 0
        # Valuing takes between 3s and 4s
        slot = self.candidates_by_priority_and_colour[4][colour]
        adjust_slot(self, slot, pos, inc)

    def add_or_remove_threat(self, colour, pos, inc=1):
        assert pos[0] >= 0
        assert pos[1] >= 0
        # Valuing threats between 2s and 3s
        slot = self.candidates_by_priority_and_colour[2][colour]
        adjust_slot(self, slot, pos, inc)

#@cython.profile(False)
#cdef inline adjust_slot(self, slot, pos, inc):
def adjust_slot(self, slot, pos, inc):
    slot.add_or_remove_candidate(Loc(pos), inc)
    '''
    slot[pos] = slot.setdefault(pos, 0) + inc
    if slot[pos] == 0:
        del slot[pos]
    '''
#endif

