#ifndef _transposition_table_h
#define _transposition_table_h

#include "defines.h"

#include <unordered_map>

#include <vector>

class PenteGame;

typedef struct{
    inline size_t operator() (const std::vector<U64>& vec) const {
		std::size_t seed = 0;
		for(auto& v : vec) {
			seed ^= std::hash<U64>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
		return seed;
    }
} VectorU64Hash;
 
typedef struct{
    inline bool operator() (const std::vector<U64>& lhs, const std::vector<U64> &rhs) const { 
		for (size_t i=0; i<lhs.size(); i++) {
			if (lhs[i] != rhs[i])
				return false;
		}
		return true;
    }
} VectorU64Eq;
 
typedef std::unordered_map<vector<U64>, UtilityValue, VectorU64Hash, VectorU64Eq> tt_hashmap_t;

class TranspositionTable
{
public:
	TranspositionTable();

	void clear();

	void savePos(const PenteGame &game, UtilityValue storeVal);

	bool lookup(const PenteGame &game, UtilityValue &foundVal) const;

private:
    tt_hashmap_t _transpositionTable;
};

#endif
