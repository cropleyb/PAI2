#ifndef _board_strip_h
#define _board_strip_h

#include "defines.h"
#include "span_entry.h"

#define FIVE_OCCS_MASK (4 * 4 * 4 * 4 * 4 - 1)

template <class REPORTER>
void matchRange(U64 occs, const SpanEntry &span, REPORTER &reporter, int inc)
{
	BoardWidth minInd = span._minIndex;
	BoardWidth maxInd = span._maxIndex;

	for (BoardWidth ind=minInd; ind<=maxInd-4; ind++)
	{
        // Extract just the 5 * 2 bits that we're currently interested in.
        int shift = ind << 1; // x 2 for 2 bits each occ - EMPTY:0, P1:1 or P2:2
        U64 mask = (occs >> shift) & FIVE_OCCS_MASK;

        // Now see if it's in our lookup table
		const LinePattern &found = lengthLookup[mask];
		
		if (found._patternType == NoPattern) continue;

        // Report it
		LinePattern toReport(found, ind);

		reporter.report(span, toReport, inc);

		if (found._patternType != Threat) continue;

		// If it is a threat, it is probably a Line2 as well.
		// Look it up in the overflow table.
		const LinePattern &found2 = threatLookup[mask];
		
		if (found2._patternType == NoPattern) continue;

        // Report it too
		LinePattern toReport2(found2, ind);

		reporter.report(span, toReport2, inc);
	}
}

#define FOUR_OCCS_MASK (4 * 4 * 4 * 4 - 1)

// BWWx
const U64 P1_CAPTURE_LEFT_PATTERN = P1 + (4 * P2) + (16 * P2); // + (64 * 0)
// WBBx
const U64 P2_CAPTURE_LEFT_PATTERN = P2 + (4 * P1) + (16 * P1); // + (64 * 0)
// xWWB
const U64 P1_CAPTURE_RIGHT_PATTERN = (P2 + (4 * P2) + (16 * P1)) * 4;
// xBBW
const U64 P2_CAPTURE_RIGHT_PATTERN = (P1 + (4 * P1) + (16 * P2)) * 4;


template <class REPORTER>
void matchCaptures(U64 occs, const SpanEntry &span, REPORTER &mr, Colour p)
{
	BoardWidth minInd = span._minIndex;
	BoardWidth maxInd = span._maxIndex;
	BoardWidth moveIndex = span._locIndex;

	// Test for capture on the right side of a line.
	if (moveIndex >= minInd + 3 && moveIndex <= maxInd)
	{
		U64 pattern = P1_CAPTURE_LEFT_PATTERN;
		if (p == P2) 
		{
			pattern = P2_CAPTURE_LEFT_PATTERN;
		}

		BoardWidth ind = moveIndex - 3;
        int shift = ind << 1; // x 2 for 2 bits each occ - EMPTY:0, P1:1 or P2:2
        U64 mask = (occs >> shift) & FOUR_OCCS_MASK;
		if (mask == pattern)
		{
			mr.reportCapture(span, true, p);
		}
	}

	// Test for capture on the left side of a line.
	if (moveIndex >= minInd && moveIndex <= maxInd - 3)
	{
		U64 pattern = P1_CAPTURE_RIGHT_PATTERN;
		if (p == P2)
		{
			pattern = P2_CAPTURE_RIGHT_PATTERN;
		}

		BoardWidth ind = moveIndex;
        int shift = ind << 1; // x 2 for 2 bits each occ - EMPTY:0, P1:1 or P2:2
        U64 mask = (occs >> shift) & FOUR_OCCS_MASK;
		if (mask == pattern)
		{
			mr.reportCapture(span, false, p);
		}
	}
}

#endif
