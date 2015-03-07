#ifndef _LOC_H
#define _LOC_H

typedef unsigned char Coord;
typedef short CompressedLoc;

#define MAX_LOCS (19 * 32)

class Loc
{
public:
	Loc(Coord x, Coord y) : _value(y * 32 + x) {}
	Loc(CompressedLoc v) : _value(v) {}
	Loc() : _value(Loc::INVALID._value) {}
	bool operator ==(const Loc &other) const { return _value == other._value; }
	bool operator <(const Loc &other) const { return _value < other._value; }
	Coord operator [](int dim) const
        { if (dim) return (_value/32); else return (_value&31); }

	bool isValid() const { return _value < (CompressedLoc)(19 * 32); }
	// TODO: operator <<
	//string toString() const { return string("(") + (_value/32) + "," + (_value%32) + ")"; }

    static Loc INVALID;

//private:
	CompressedLoc _value = 0;
};

#endif
