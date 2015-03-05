
#include <stdint.h>

#include "line_lookup_table.h"

LineTableItem lengthLookup[MaxSpanMask];

bool initialised=false;


/*
Detect and report indices that build on or interfere with a 
possibly fragmented line, as well as counting these possibilities.
Here, we build a lookup table, which is a mapping from a bit pattern
to the information we need - the colour, current length and empty indices
of the row of 5 positions that we are currently looking at.
*/

Breadth candidateLookup[5] {3,1,0,2,4};

// Fwd decl.
void buildAndStoreLineValues(int levelsDone, Mask occVal, LineTableItem lti);

void extendAndStoreLineLookups(Colour occ, int levelsDone,  Mask occVal, LineTableItem lti)
{
    /*
    occ is the colour of the stone (or EMPTY) that we are extending by
    depth is the length yet to be added
    occVal is the total value so far, representing the stretch to the left
    that we have already processed.
    lti contains the info that will ultimately be stored in the table.
	They will all have been of the same 'lti._colour' as we only care about and
   	store them if they are.
    */
    if (occ != EMPTY)
	{
        // add occ to lookup value
        occVal += lti._colour << (levelsDone * 2);

        // add one to length
        lti._matchType = (MatchType)((int)lti._matchType + 1);
	} else {
		lti._candInds.push_back(levelsDone);
	}

    if (levelsDone >= 4)
	{
        if (lti._matchType > 0)
		{
            // Add Pattern
            // assert length <= 5

			// TODO in LengthLookup (i.e. caller)
            // candidates = [(candidateLookup[i], i) for i in emptyList]
            // candidates.sort()
            // candidates = [i for o,i in candidates]
            lengthLookup[occVal] = lti;
		}
	} else {
        // Recursively add to the stretch
        buildAndStoreLineValues(levelsDone+1, occVal, lti);
	}
}


void buildAndStoreLineValues(int levelsDone, Mask occVal, LineTableItem lti)
{
    // Add one stone or empty place

    // Shift what we've seen so far to the right
    //occVal *= 4;

	extendAndStoreLineLookups(EMPTY, levelsDone, occVal, lti);
	extendAndStoreLineLookups(lti._colour, levelsDone, occVal, lti);
}

// BWWx
U64 P1_CAPTURE_LEFT_PATTERN = P1 + (4 * P2) + (16 * P2); // + 64 * 0
// WBBx
U64 P2_CAPTURE_LEFT_PATTERN = P2 + (4 * P1) + (16 * P1); // + 64 * 0
// xWWB
U64 P1_CAPTURE_RIGHT_PATTERN = (P2 + (4 * P2) + (16 * P1)) * 4;
// xBBW
U64 P2_CAPTURE_RIGHT_PATTERN = (P1 + (4 * P1) + (16 * P2)) * 4;

void buildAndStoreEndedTakes(Colour c, bool side)
{
	Mask occVal;
	if (c == P1)
		if (side)
			occVal = P1_CAPTURE_LEFT_PATTERN;
		else
			occVal = P1_CAPTURE_RIGHT_PATTERN;
	else
		if (side)
			occVal = P2_CAPTURE_LEFT_PATTERN;
		else
			occVal = P2_CAPTURE_RIGHT_PATTERN;

	Breadth candInd = 0;
	if (side) candInd = 3;

	LineTableItem lti;
	lti._colour = c;
	lti._matchType = Take;

	for (int lastColumn=EMPTY; lastColumn<=EDGE; lastColumn++)
	{
		lti._candInds.clear();
		lti._candInds.push_back(candInd);

		Mask storeOccVal = occVal + (lastColumn << 8);
		lengthLookup[storeOccVal] = lti;
	}
}

void buildAndStoreAllTakes()
{
	for (Colour c=P1; c<=P2; c++)
	{
		buildAndStoreEndedTakes(c, false);
		buildAndStoreEndedTakes(c, true);
	}
}

// xWWx
U64 P1_THREAT_PATTERN = 0 + (4 * P2) + (16 * P2); // + 64 * 0
// WBBx
U64 P2_THREAT_PATTERN = 0 + (4 * P1) + (16 * P1); // + 64 * 0

void buildAndStoreEndedThreats(Colour c)
{
	Mask occVal;
	if (c == P1)
		occVal = P1_THREAT_PATTERN;
	else
		occVal = P2_THREAT_PATTERN;

	LineTableItem lti;
	lti._colour = c;
	lti._matchType = Threat;
	lti._candInds.push_back(0);
	lti._candInds.push_back(3);

	for (int lastColumn=EMPTY; lastColumn<=EDGE; lastColumn++)
	{
		Mask storeOccVal = occVal + (lastColumn << 8);
		lengthLookup[storeOccVal] = lti;
	}
}

void buildAndStoreAllThreats()
{
	for (Colour c=P1; c<=P2; c++)
	{
		buildAndStoreEndedThreats(c);
	}
}

/* Build the entire lookup table */
void buildAll()
{
    // We only care about stretches of 5 with one colour and empties in it.
	if (initialised) return;
	initialised = true;
	LineTableItem lti;
	lti._colour = P1;
    buildAndStoreLineValues(0, 0, lti);
	lti._colour = P2;
    buildAndStoreLineValues(0, 0, lti);

    buildAndStoreAllTakes();
    buildAndStoreAllThreats();
}

#if 0
from pentai.base.defines import *

from pentai.ai.utility_stats cimport report_length_candidate
cimport cython

from libc.stdint cimport uint64_t as U64
"""
Detect and report indices that build on or interfere with a 
possibly fragmented line, as well as counting these possibilities.
Here, we build a lookup table, which is a mapping from a bit pattern
to the information we need - the colour, current length and empty indices
of the row of 5 positions that we are currently looking at.
"""
cdef U64 FIVE_OCCS_MASK
FIVE_OCCS_MASK = (4 ** 5 - 1)

global length_lookup
length_lookup = {}

candidate_lookup = [3,1,0,2,4]

def extend_and_store_lookups(occ, depth, occ_val, length, colour, empty_list, rep_str):
    """
    occ is the colour of the stone (or EMPTY) that we are extending by
    depth is the length yet to be added
    occ_val is the total value so far, representing the stretch to the left
    that we have already processed.
    length is the number of stones seen so far in that stretch. They will all
    have been of the same 'colour' as we only care about and store them if
    they are.
    rep_str is a representation of the stones seen so far, for debugging.
    """
    if occ != EMPTY:
        # add occ to lookup value
        occ_val += colour

        # add one to length
        length += 1
        rep_str = rep_str + str(colour)
    else:
        rep_str = rep_str + " "
        empty_list.append(depth)

    if depth <= 0:
        if length > 0:
            # add_pattern
            assert length <= 5
            rep_str = rep_str + ">"
            candidates = [(candidate_lookup[i], i) for i in empty_list]
            candidates.sort()
            candidates = [i for o,i in candidates]
            length_lookup[occ_val] = colour, length, candidates, rep_str
    else:
        # Recursively add to the stretch
        build_and_store_values(depth-1, occ_val, length, colour, empty_list[:], rep_str)


def build_and_store_values(depth, occ_val, length, colour, empty_list, rep_str=None):
    """ Add one stone or empty place """
    # For debugging.
    if rep_str == None:
        rep_str = "<"

    # Shift what we've seen so far to the right
    occ_val *= 4
    for occ in (EMPTY, colour):
        extend_and_store_lookups(occ, depth, occ_val, length, colour, empty_list[:], rep_str)


def prepare_length_lookups():
    """ Build the entire lookup table """
    # We only care about stretches of 5 with one colour and empties in it.
    build_and_store_values(4, 0, 0, P1, [])
    build_and_store_values(4, 0, 0, P2, [])

# TODO: Something better than a global
prepare_length_lookups()
#endif

