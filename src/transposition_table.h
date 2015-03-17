#ifndef _transposition_table_h
#define _transposition_table_h

#include "defines.h"

#include <map>

#include <vector>

class PenteGame;

class TranspositionTable
{
public:
	TranspositionTable();

	void savePos(const PenteGame &game, UtilityValue storeVal);

	bool lookup(const PenteGame &game, UtilityValue &foundVal) const;

private:
	std::map<std::vector<U64>, UtilityValue> _transpositionTable;
};

#endif
