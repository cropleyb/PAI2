#ifndef _structure_h
#define _structure_h

#include "line_pattern.h"
#include "loc.h"
#include "defines.h"

class Structure
{
	// Structure
	// - pattern
	// - list of relevant locs for that structure type (pattern)
	// - e.g. for Line4, all the locs
	// - for Blocked, the loc that is blocked
	// - for take and threat, the locs that are vulnerable

public:
	Structure() : _pattern(NoPattern), _numRelevant(0), _id(-1) {}
	void addRelevant(Loc l) { _relevant[_numRelevant++] = l; }

	PatternType _pattern;
	StId _id;    
	Loc _relevant[4];
	unsigned char _numRelevant;
};

// FIXME - This a guess
#define MAX_STRUCTURES 200

class StructureMap
{
	// StructureMap
	// - for each Loc, a list of StIds
	// - collection of structures
	// - free list (array of structure ids, +head ind)
public:
	StructureMap() { reset(); }

	void reset()
	{
		_numUsed = 0;
		for (StId i=0; i<MAX_STRUCTURES; i++) {
			_free[i] = i;
		}
	}

	Structure &add(PatternType pattern)
	{
		StId id = _free[_numUsed++];
		Structure&ret = _structures[id];
		ret._pattern = pattern;
		ret._id = id;
		return ret;
	}

	void addRelevant(Structure &s, Loc l) { s.addRelevant(l); }
	// Do we need the trigger points as well?

    void remove(StId id) { _free[_numUsed--] = id; _structures[id]._pattern = NoPattern; }

private:
	Structure _structures[MAX_STRUCTURES];
	StId _free[MAX_STRUCTURES];
	StId _numUsed;
};

#endif
