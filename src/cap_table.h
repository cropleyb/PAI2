#ifndef _cap_table_h
#define _cap_table_h

#include "loc.h"
#include "defines.h"

//#include <iostream>
//using namespace std;

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

	bool hasTake(CompressedLoc cLoc) const {
		return _takes[cLoc];
	}

	bool hasTakeInDirection(CompressedLoc cLoc, DirectionType dir) const {
		CompressedLoc takesForLoc = _takes[cLoc];
		int bitMask = 3 * (1 << (2 * dir));
		return (takesForLoc & bitMask) > 0;
	}

private:
	CompressedLoc _takes[MAX_LOCS]; // TODO: should by another type.
};

#endif

