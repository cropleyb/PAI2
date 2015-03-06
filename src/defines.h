#ifndef _defines_h
#define _defines_h

#include <cstdint>
#include <vector>
using std::vector;

typedef uint64_t U64;

typedef unsigned char Length;
typedef signed char Step;
typedef unsigned short Mask;
typedef unsigned char Colour;
typedef char Depth;
typedef unsigned char Breadth;
typedef char CapCount;
typedef signed char BoardWidth;

const Colour EMPTY = 0;
const Colour P1 = 1;
const Colour P2 = 2;
const Colour EDGE = 3;

enum DirectionType
{
	E_DIR=0,
	SE_DIR=1,
	S_DIR=2,
	SW_DIR=3,
	MAX_DIR
};

#define WIN_LENGTH 5
#define MAX_CANDS 20
#define MAX_DEPTH 20
#define MAX_WIDTH 19
#define MAX_LOCS (MAX_WIDTH*MAX_WIDTH)

#define otherPlayer(p) (p==P1?P2:P1)

#endif
