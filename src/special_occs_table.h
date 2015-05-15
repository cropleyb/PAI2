#ifndef _special_occs_table_h
#define _special_occs_table_h

#include "loc.h"

union SpecialOccCounts {
    unsigned short all;

    struct {
        unsigned char fours: 5;
        unsigned char blocked4s: 5;
        unsigned char takes: 3;
    };
};

class SpecialOccsTable {
public:
	SpecialOccsTable() : _counts{0} {}

private:
	SpecialOccCounts _counts[MAX_LOCS];
};

#endif
