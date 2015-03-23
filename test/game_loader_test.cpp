#include "gmock/gmock.h"
#include "game_loader.h"

#include <string>
using std::string;

class GameLoaderFixture : public testing::Test {
public:
	GameLoaderFixture() {
	}
};

TEST_F(GameLoaderFixture, test_find_one_move) {
    string gameStr = \
"1. (6, 6)\n"
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
	const char *move = getMoveFromStr(gameStr.c_str());
	EXPECT_EQ(string("6,7\n"), move);
}

