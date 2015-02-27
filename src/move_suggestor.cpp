
#include "move_suggestor.h"
#include "position_stats.h"
#include "candidate_cache.h"

MoveSuggestor::MoveSuggestor(PositionStats &ps, CandidateCache &cc)
	: _posStats(ps), _candCache(cc)
{
}

Loc MoveSuggestor::getNextMove(Depth depth)
{
	Loc move = _candCache.getNextMove(depth);
	if (move.isValid()) {
		return move;
	}

    Loc *moveBuffer = _candCache.getBuffer(depth);
	Breadth maxMoves = 9;
	if (depth > 3) maxMoves = 4;
	Breadth moveCount = filterCandidates(moveBuffer, depth, maxMoves, P1); // FIXME - our real colour
	_candCache.setDepthMoves(depth, moveCount);
	return _candCache.getNextMove(depth);

	// No moves prepared at this depth. Must generate them.
	//const PriorityLevel &ones = _posStats.getLengthPriorityLevel(P1, 1);

	//return Loc::INVALID;
}

Breadth MoveSuggestor::filterCandidates(Loc *moveBuffer, Depth depth, Breadth maxMoves, Colour ourColour)
{
	Breadth found = 0;
	bool tried[MAX_LOCS];

	Colour theirColour = otherPlayer(ourColour);
	
	// bool onePoss = getPriorityLevels(ourColour);
	const PriorityLevel &pl = _posStats.getLengthPriorityLevel(ourColour, 4);
	Breadth foundFromPL = pl.getCands(moveBuffer, maxMoves-found);
	
#if 0
	for slot in candidate_slots:
		#slot_arr = slot.iteritems()
		slot_arr = slot.get_iter()
		sorted_slot = [(count, loc) for (loc, count) in slot_arr]
		sorted_slot.sort(reverse=True)
		for count, loc in sorted_slot:
#endif
	return found;
}

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

