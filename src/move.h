#include "loc.h"

typedef unsigned char Directions;

class Move
{
public:
	Move(Loc loc, Directions caps) : _loc(loc), _caps(caps) {}

	Loc getLoc() const { return _loc; };
	Directions getCaps() const { return _caps; }

private:
	Loc _loc;
	Directions _caps;
};
