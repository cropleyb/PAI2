
#include "length_lookup.h"
// typedef uint64_t U64

// # TODO: This will need to be increased for pente-keryo
//#define COUNT_LENGTH=5

#if 0
typedef unsigned char Length;

class LengthTableItem
{
public:
	LengthTableItem() : _colour(EMPTY), _length(0) {}

	Colour _colour;
	Length _length;
	bool _empty[WIN_LENGTH];
};

const int MaxSpanMask = 4 * 4 * 4 * 4 * 4;

// The lengthLookup (and its contained class) is the only
// public scope variable, and only to LengthLookup.cpp (hopefully :) )
extern LengthTableItem lengthLookup[MaxSpanMask];
#endif

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

