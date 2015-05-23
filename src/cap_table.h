#ifndef _cap_table_h
#define _cap_table_h

#include "loc.h"
#include "defines.h"

#include <iostream>
using namespace std;

class CapTable {
	/* The cap table keeps track of all the takeable stones - along with the direction
	 * from the trigger point to the takeable stone.
	 */
public:
	CapTable() : _takes {0} {}

	CompressedLoc addOneCap(Loc trigger, DirectionType dir, CompressedLoc offset, Breadth dist, int inc) {
//		cout << "trigger._value: " << trigger._value << " offset: " << offset << " dist: " << (int)dist << endl;
		CompressedLoc capLocVal = trigger._value + (offset*-dist);
		//cout << "capLocVal._value: " << capLocVal << endl;
		_takes[capLocVal] += inc * (1 << ((2 * dir) + dist - 1));
		return capLocVal;
	}

	CompressedLoc getTakes(CompressedLoc cLoc) const {
		return _takes[cLoc];
	}

	// TODO: remove
	bool hasTakeInDirection(CompressedLoc cLoc, DirectionType dir) const {
		CompressedLoc takesForLoc = _takes[cLoc];
		int bitMask = 3 * (1 << (2 * dir));
		return (takesForLoc & bitMask) > 0;
	}

	Loc getTriggerInDirection(CompressedLoc cOcc, DirectionType dir) const {
		CompressedLoc takesForLoc = _takes[cOcc];
        int dist = (takesForLoc >> (2 * dir)) & 3;
		if (dist == 0) return Loc::INVALID;
		CompressedLoc offset = dirOffsets[dir];
		// cout << "GTID: offset: " << (int)offset << " dir: " << (int)dir << " dist: " << dist << endl;
		return Loc(cOcc + dist*offset);
	}

private:
	CompressedLoc _takes[MAX_LOCS]; // TODO: should be another type.
};

#endif

