#ifndef _cap_table_h
#define _cap_table_h

#include "loc.h"
#include "defines.h"

class CapTable {
	/* The cap table keeps track of all the takeable stones - along with the direction
	 * from the trigger point to the takeable stone.
	 */
public:
	CapTable() : _takes {0} {}

	void addCap(Loc trigger, DirectionType dir, CompressedLoc offset, int inc) {
		for (int capInd=1; capInd<=2; capInd++) {
			CompressedLoc capLocVal = trigger._value + (offset*capInd);
			_takes[capLocVal] += inc * (1 << ((2 * dir) + capInd - 1));
		}
	}

private:
	CompressedLoc _takes[MAX_LOCS];
};

#endif

