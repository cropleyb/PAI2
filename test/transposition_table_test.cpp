#include "gmock/gmock.h"

#define private public

#include "bdebug.h"
#include "loc.h"
#include "transposition_table.h"
#include "pente_game.h"
#include "game_loader.h"

#include <string>
using std::string;
using testing::ElementsAre;

class TranspositionTableFixture : public testing::Test {
public:
	TranspositionTableFixture() {
		//tt.clear(); // TODO
		//buildSpanTable(19);
	}

	void loadAndStore(const string &gameStr, UtilityValue storeValue)
	{
		PenteGame g;
		GameLoader gl = GameLoader();
		gl.loadStr(g, gameStr);
		tt.savePos(g, storeValue);
	}

   	bool loadAndLookup(const string &gameStr, UtilityValue &val)
	{
		PenteGame g;
		GameLoader gl = GameLoader();
		gl.loadStr(g, gameStr);
		return tt.lookup(g, val);
	}

#if 0
    Loc doTheSearch(const string &gameStr, Depth maxDepth)
	{
		PenteGame g;
		g.setMaxDepth(maxDepth);
		GameLoader gl = GameLoader();
		gl.loadStr(g, gameStr);

		AlphaBeta ab(g);
		Loc bestMove = ab.getBestMove();
		return bestMove;
	}
#endif
	TranspositionTable tt;
};

TEST_F(TranspositionTableFixture, storeAndFind) {
    string gameStr1 = \
"1. (6, 6)\n"
"2. (7, 7)\n"
"3. (8, 6)\n"
"4. (9, 6)\n";
    string gameStr2 = \
"1. (6, 6)\n"
"2. (9, 6)\n"
"3. (8, 6)\n"
"4. (7, 7)\n";
    UtilityValue storeVal = 103.5;
	loadAndStore(gameStr1, storeVal);
	UtilityValue foundVal;
   	bool found = loadAndLookup(gameStr2, foundVal);
	EXPECT_EQ(true, found);
	EXPECT_EQ(storeVal, foundVal);
}

TEST_F(TranspositionTableFixture, storeAndDontFind) {
    string gameStr1 = \
"1. (6, 6)\n"
"2. (7, 7)\n"
"3. (8, 6)\n"
"4. (9, 6)\n";
    string gameStr2 = \
"1. (6, 6)\n"
"2. (9, 6)\n"
"3. (8, 6)\n"
"4. (17, 15)\n";
    UtilityValue storeVal = 103.5;
	loadAndStore(gameStr1, storeVal);
	UtilityValue foundVal;
   	bool found = loadAndLookup(gameStr2, foundVal);
	EXPECT_EQ(false, found);
}
