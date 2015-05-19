
#include <stdint.h>

#include "line_lookup_table.h"

LinePattern lengthLookup[MaxSpanMask];
LinePattern threatLookup[MaxSpanMask];

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
void buildAndStoreLineValues(int levelsDone, Mask occVal, LinePattern lti);

#include <assert.h>

void extendAndStoreLineLookups(Colour occ, int levelsDone,  Mask occVal, LinePattern lti)
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
        int ptRaw = lti._patternType + 1;
		if (ptRaw == Threat || ptRaw == Take) ptRaw++;
        lti._patternType = (LinePatternType)((int)ptRaw);
	} else {
		assert(lti._numCands < 5);
		lti._inds[lti._numCands++] = levelsDone;
	}

    if (levelsDone >= 4)
	{
        if (lti._patternType > 0)
		{
            // Add Pattern
            // assert length <= 5

			// TODO in LengthLookup (i.e. caller)?
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


void buildAndStoreLineValues(int levelsDone, Mask occVal, LinePattern lti)
{
    // Add one stone or empty place

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
	Breadth takerInd = 3;
	Breadth victim1Ind = 1;
	Breadth victim2Ind = 2;
	if (side) {
		candInd = 3;
		takerInd = 0;
	}

	LinePattern lti;
	lti._colour = c;
	lti._patternType = Take;

	for (int lastColumn=EMPTY; lastColumn<=EDGE; lastColumn++)
	{
		lti._numCands = 1;
		lti._inds[0] = candInd;
		lti._inds[1] = victim1Ind;
		lti._inds[2] = victim2Ind;
		lti._inds[3] = takerInd;

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

	LinePattern lti;
	lti._colour = c;
	lti._patternType = Threat;
	lti._inds[lti._numCands++] = 0;
	lti._inds[lti._numCands++] = 3;

	for (int lastColumn=EMPTY; lastColumn<=EDGE; lastColumn++)
	{
		Mask storeOccVal = occVal + (lastColumn << 8);
		if (lengthLookup[storeOccVal]._patternType != NoPattern)
		{
			threatLookup[storeOccVal] = lengthLookup[storeOccVal];
		}
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

// WBBBB
U64 P1_BLOCKED_LEFT_PATTERN = P2 + (4 * P1) + (16 * P1) + (64 * P1) + (256 * P1);
// BWWWW
U64 P2_BLOCKED_LEFT_PATTERN = P1 + (4 * P2) + (16 * P2) + (64 * P2) + (256 * P2);
// BBBBW
U64 P1_BLOCKED_RIGHT_PATTERN = P1 + (4 * P1) + (16 * P1) + (64 * P1) + (256 * P2);
// WWWWB
U64 P2_BLOCKED_RIGHT_PATTERN = P2 + (4 * P2) + (16 * P2) + (64 * P2) + (256 * P1);

void buildAndStoreBlocked(Colour c, bool side)
{
	Mask occVal;
	if (c == P1)
		if (side)
			occVal = P1_BLOCKED_LEFT_PATTERN;
		else
			occVal = P1_BLOCKED_RIGHT_PATTERN;
	else
		if (side)
			occVal = P2_BLOCKED_LEFT_PATTERN;
		else
			occVal = P2_BLOCKED_RIGHT_PATTERN;

	Breadth candInd = 0;
	if (side) candInd = 3;

	LinePattern lti;
	lti._colour = c;
	lti._patternType = Blocked;
	lengthLookup[occVal] = lti;
}

void buildAndStoreAllBlocked()
{
	buildAndStoreBlocked(P1, false);
	buildAndStoreBlocked(P1, true);
	buildAndStoreBlocked(P2, false);
	buildAndStoreBlocked(P2, true);
}

/* Build the entire lookup table */
void buildLineLookupTable()
{
    // We only care about stretches of 5 with one colour and empties in it.
	if (initialised) return;
	initialised = true;
	LinePattern lti;
	lti._colour = P1;
    buildAndStoreLineValues(0, 0, lti);
	lti._colour = P2;
    buildAndStoreLineValues(0, 0, lti);

    buildAndStoreAllTakes();
    buildAndStoreAllThreats();
    buildAndStoreAllBlocked();
}

