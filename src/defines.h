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
typedef unsigned short MoveNumber;
typedef char Depth;
typedef unsigned char Breadth;
typedef signed char CapCount;
typedef signed char BoardWidth;
typedef unsigned char CaptureDirs;
typedef unsigned char PattCount;
typedef double UtilityValue;
typedef char RulesType;
typedef char StructureId;

const Colour EMPTY = 0;
const Colour P1 = 1;
const Colour P2 = 2;
const Colour EDGE = 3;

enum DirectionType
{
	E_DIR=0,
	SE_DIR=1,
	N_DIR=2,
	NE_DIR=3,
	MAX_DIR=4,
	W_DIR=4,
	NW_DIR=5,
	S_DIR=6,
	SW_DIR=7
};

#define WIN_LENGTH 5
#define MAX_CANDS 20
#define MAX_DEPTH 20
#define MAX_WIDTH 19

#define otherPlayer(p) (p==P1?P2:P1)

#endif
