#include "transposition_table.h"
#include "pente_game.h"

TranspositionTable::TranspositionTable()
{
}

void TranspositionTable::savePos(const PenteGame &game, UtilityValue storeVal)
{
	const U64 *e_orig = game._boardReps._boardStrips[E_DIR];
	std::vector<U64> e_rep(e_orig, e_orig + 2*MAX_WIDTH);
	_transpositionTable[e_rep] = storeVal;
}

bool TranspositionTable::lookup(const PenteGame &game, UtilityValue &foundVal) const
{
	const U64 *e_orig = game._boardReps._boardStrips[E_DIR];
	std::vector<U64> e_rep(e_orig, e_orig + 2*MAX_WIDTH);

    auto search = _transpositionTable.find(e_rep);
 
    if (search != _transpositionTable.end()) {
        foundVal = search->second;
		return true;
    }
	return false;
}

