#if 1
#include "gmock/gmock.h"
#include "game_loader.h"

#include <string>
using std::string;

#include "pente_game.h"

class GameLoaderFixture : public testing::Test {
public:
	GameLoaderFixture() {
	}

	PenteGame _game;
};

TEST_F(GameLoaderFixture, test_find_one_move) {
    string gameStr = \
"depth:4\n"
"boardsize:19\n"
"rules:tournament\n"
"1. (6, 6)\n" // This is not a tournament move...
"2. (7, 7)\n"
"3. (8, 6)\n"
"4. (9, 6)\n"
"5. (6, 4)\n"
"6. (9, 7)\n"
"7. (9, 8)\n"
"8. (9, 5)\n"
"9. (6, 5)\n"
"10. (6, 7)\n"
"11. (8, 7)\n"
"12. (9, 4)\n"
"13. (9, 3)\n"
"14. (7, 6)\n"
"15. (6, 3)\n"
"16. (6, 2)\n"
"17. (5, 7)\n";
	const char *move = getMoveFromStrWithGame(gameStr.c_str(), &_game);
	EXPECT_EQ(4, _game.getNormalDepth());
	EXPECT_EQ(19, _game.getBoardSize());
	EXPECT_EQ(string("6,7\n"), move); // This test is not really relevant
}

TEST_F(GameLoaderFixture, test_small_and_quick) {
    string gameStr = \
"depth:1\n"
"boardsize:9\n"
"rules:standard\n"
"1. (4, 4)\n"
"2. (5, 5)\n"
"3. (4, 6)\n";
	const char *move = getMoveFromStrWithGame(gameStr.c_str(), &_game);
	EXPECT_EQ(1, _game.getNormalDepth());
	EXPECT_EQ(9, _game.getBoardSize());
}

TEST_F(GameLoaderFixture, test_make_first_move) {
    string gameStr = \
"depth:1\n"
"boardsize:9\n"
"rules:standard\n";
	const char *move = getMoveFromStrWithGame(gameStr.c_str(), &_game);
	EXPECT_EQ(1, _game.getNormalDepth());
	EXPECT_EQ(9, _game.getBoardSize());
	EXPECT_EQ(string("4,4\n"), move);
}
#endif
