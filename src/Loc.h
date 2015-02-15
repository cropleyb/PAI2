#ifndef _LOC_H
#define _LOC_H

typedef unsigned char Coord;

class Loc
{
public:
	Loc(Coord x, Coord y) : _value(y * 32 + x) {}
	bool operator ==(const Loc &other) const { return _value == other._value; }

private:
	unsigned short _value = 0;
};

#endif
