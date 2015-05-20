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

	CompressedLoc addOneCap(Loc trigger, DirectionType dir, CompressedLoc offset, Breadth dist, int inc) {
		CompressedLoc capLocVal = trigger._value + (offset*dist);
		_takes[capLocVal] += inc * (1 << ((2 * dir) + dist - 1));
		return capLocVal;
	}

	bool hasTake(CompressedLoc cLoc) const {
		return _takes[cLoc];
	}

private:
	CompressedLoc _takes[MAX_LOCS];
};

#endif

