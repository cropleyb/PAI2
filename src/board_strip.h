#ifndef _board_strip_h
#define _board_strip_h

#include "defines.h"
#include "span_lookup_table.h"

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
		const LinePattern *found = &lengthLookup[mask];
		
		if (found->_patternType == NoPattern) continue;

        // Report it
		LinePattern toReport = *found;

		for (Breadth &candInd: toReport._candInds)
		{
			candInd += ind;
		}
		reporter.report(span, toReport, inc);

		if (found->_patternType != Threat) continue;

		// If it is a threat, it is probably a Line2 as well.
		// Look it up in the overflow table.
		found = &threatLookup[mask];
		
		if (found->_patternType == NoPattern) continue;

        // Report it too
		LinePattern toReport2 = *found;

		for (Breadth &candInd: toReport2._candInds)
		{
			candInd += ind;
		}
		reporter.report(span, toReport2, inc);
	}
}

#endif
