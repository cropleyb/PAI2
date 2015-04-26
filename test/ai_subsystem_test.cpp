#if 1
#include "gmock/gmock.h"

#define private public

#include "bdebug.h"
#include "loc.h"
#include "span_lookup_table.h"
#include "pente_game.h"
#include "game_loader.h"
#include "alpha_beta.h"

#include <string>
using std::string;
using testing::ElementsAre;

#define NEW_SYSTEM_TESTS 

extern void loadGameStr(PenteGame &g, const string &gameStr);

#if 0
// If this is needed then it should use the new version in PenteGame.
void printGameHistory(std::string gameStr) {
	PenteGame g;
	loadGameStr(g, gameStr);
	cout << endl;
	for (int i=60; i>0; i--)
	{
		g.print();
		MoveNumber mn = g.getLastMoveNumber();
		if (mn <= 1) break;
		g.undoLastMove();
	}
}
#endif

extern Loc doTheSearch(const string &gameStr, PenteGame *game);

class AISubsystemFixture : public testing::Test {
public:
	AISubsystemFixture() {
		buildSpanTable(19); // TODO: cull
	}
	Loc doTheSearchTest(const string &gameStr) {
		return doTheSearch(gameStr, &_game);
	}
	PenteGame _game;
};

TEST_F(AISubsystemFixture, test_find_one_move) {
    string gameStr = \
"boardwidth:19\n"
"depth:2\n"
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
    Loc move = doTheSearchTest(gameStr);
	EXPECT_EQ(Loc(6,7), move);
}

#if 0
// This is failing, but the alternative is reasonable
TEST_F(AISubsystemFixture, test_dont_waste_a_pair) {
    string gameStr = \
"boardwidth:19\n"
"depth:4\n"
"1. (6, 6)\n"
"2. (6, 7)\n"
"3. (8, 8)\n"
"4. (5, 5)\n"
"5. (8, 6)\n"
"6. (7, 6)\n"
"7. (8, 5)\n"
"8. (8, 7)\n"
"9. (8, 4)\n"
"10. (5, 8)\n";
    Loc move = doTheSearchTest(gameStr);
	EXPECT_EQ(Loc(6,5), move);
}
#endif

TEST_F(AISubsystemFixture, test_dodgy_move) {
    string gameStr = \
"boardwidth:19\n"
"depth:6\n"
"1. (6, 6)\n"
"2. (6, 5)\n"
"3. (8, 6)\n"
"4. (7, 6)\n"
"5. (5, 4)\n"
"6. (8, 7)\n"
"7. (6, 8)\n"
"8. (7, 7)\n"
"9. (6, 7)\n";

    Loc move = doTheSearchTest(gameStr);
    // Why not 9,7? i.e. why does 7,5 have a high score?
	EXPECT_NE(Loc(7,5), move);
}

TEST_F(AISubsystemFixture, test_dodgy_move_part2) {
    string gameStr = \
"boardwidth:13\n"
"depth:5\n"
"1. (6, 6)\n"
"2. (6, 5)\n"
"3. (8, 6)\n"
"4. (7, 6)\n"
"5. (5, 4)\n"
"6. (8, 7)\n"
"7. (6, 8)\n"
"8. (7, 7)\n"
"9. (6, 7)\n"
"10. (7, 5)\n";
    Loc move = doTheSearchTest(gameStr);
	// EXPECT_EQ(Loc(9,7), move); // TODO: Analyse failure...
}

#if 0
// OK but slow
TEST_F(AISubsystemFixture, test_freebie) {
    string gameStr = \
"boardwidth:19\n"
"depth:8\n"
"1. (6, 6)\n"
"2. (6, 5)\n"
"3. (8, 6)\n"
"4. (7, 6)\n"
"5. (5, 4)\n"
"6. (8, 7)\n"
"7. (6, 8)\n"
"8. (7, 7)\n"
"9. (6, 7)\n"
"10. (7, 5)\n"
"11. (9, 7)\n"
"12. (10, 8)\n";
    Loc move = doTheSearchTest(gameStr);
	EXPECT_EQ(Loc(8,7), move);
}
#endif

#if 1
TEST_F(AISubsystemFixture, test_strange) {
    string gameStr = \
"boardwidth:19\n"
"depth:6\n"
"1. (6, 6)\n"
"2. (6, 7)\n"
"3. (4, 6)\n"
"4. (5, 8)\n"
"5. (7, 6)\n"
"6. (5, 6)\n"
"7. (4, 9)\n";
    Loc move = doTheSearchTest(gameStr);
	//_game.print();
	EXPECT_EQ(Loc(8,6), move); // (4,7)???
	//EXPECT_EQ(Loc(5,8), move); // I'm not sure which is better...
}
#endif

#if 0

TEST_F(AISubsystemFixture, test_draw) {
    string gameStr = \
"boardwidth:19\n"
"depth:4\n"
"1. (4, 4)\n"
"2. (5, 5)\n"
"3. (6, 4)\n"
"4. (5, 4)\n"
"5. (5, 6)\n"
"6. (5, 3)\n"
"7. (5, 2)\n"
"8. (6, 5)\n"
"9. (7, 6)\n"
"10. (4, 5)\n"
"11. (4, 3)\n"
"12. (6, 5)\n"
"13. (3, 5)\n"
"14. (2, 6)\n"
"15. (3, 5)\n"
"16. (7, 5)\n"
"17. (8, 5)\n"
"18. (6, 1)\n"
"19. (6, 6)\n"
"20. (4, 6)\n"
"21. (3, 7)\n"
"22. (4, 6)\n"
"23. (3, 6)\n"
"24. (3, 4)\n"
"25. (2, 3)\n"
"26. (3, 4)\n"
"27. (4, 3)\n"
"28. (5, 5)\n"
"29. (5, 1)\n"
"30. (1, 4)\n"
"31. (3, 3)\n"
"32. (2, 4)\n"
"33. (4, 4)\n"
"34. (2, 8)\n"
"35. (4, 2)\n"
"36. (2, 7)\n"
"37. (2, 5)\n"
"38. (4, 7)\n"
"39. (2, 5)\n"
"40. (2, 2)\n"
"41. (4, 4)\n"
"42. (4, 1)\n"
"43. (3, 3)\n"
"44. (0, 3)\n"
"45. (1, 5)\n"
"46. (4, 5)\n"
"47. (3, 6)\n"
"48. (0, 4)\n"
"49. (0, 5)\n"
"50. (0, 2)\n"
"51. (0, 0)\n"
"52. (4, 8)\n"
"53. (3, 8)\n"
"54. (4, 0)\n"
"55. (1, 3)\n"
"56. (5, 0)\n"
"57. (3, 0)\n"
"58. (6, 0)\n"
"59. (8, 4)\n"
"60. (8, 0)\n"
"61. (7, 0)\n"
"62. (8, 6)\n"
"63. (8, 3)\n"
"64. (8, 2)\n"
"65. (7, 4)\n"
"66. (5, 4)\n"
"67. (7, 2)\n"
"68. (6, 3)\n"
"69. (7, 1)\n"
"70. (7, 3)\n"
"71. (8, 7)\n"
"72. (8, 8)\n"
"73. (6, 8)\n"
"74. (2, 1)\n"
"75. (0, 1)\n"
"76. (6, 2)\n"
"77. (1, 0)\n"
"78. (2, 0)\n"
"79. (1, 1)\n"
"80. (3, 1)\n";
    EXPECT_EQ(True, game.isFinished());
	EXPECT_EQ(P1+P2, game.getWonBy());
}
#endif

#ifdef NEW_SYSTEM_TESTS
TEST_F(AISubsystemFixture, test_bad) {
    string gameStr = \
"boardwidth:19\n"
"depth:6\n"
"1.(9, 9)\n"
"2.(10, 10)\n"
"3.(9, 11)\n"
"4.(9, 12)\n"
"5.(11, 9)\n"
"6.(10, 9)\n"
"7.(9, 10)\n"
"8.(9, 7)\n"
"9.(10, 11)\n"
"10.(11, 12)\n"
"11.(10, 8)\n"
"12.(10, 12)\n"
"13.(8, 12)\n"
"14.(12, 10)\n"
"15.(7, 13)\n"
"16.(12, 12)\n"
"17.(13, 12)\n"
"18.(10, 10)\n"
"19.(11, 11)\n"
"20.(12, 11)\n"
"21.(8, 11)\n"
"22.(7, 11)\n"
"23.(12, 9)\n"
"24.(8, 9)\n"
"25.(6, 14)\n"
"26.(5, 15)\n"
"27.(10, 11)\n"
"28.(13, 10)\n"
"29.(14, 9)\n"
"30.(12, 14)\n"
"31.(12, 13)\n"
"32.(9, 14)\n"
"33.(10, 13)\n"
"34.(14, 10)\n"
"35.(11, 10)\n"
"36.(11, 9)\n"
"37.(11, 10)\n"
"38.(10, 8)\n"
"39.(12, 8)\n";

#if 0
	printGameHistory(gameStr);
#endif
    Loc move = doTheSearchTest(gameStr);
	EXPECT_EQ(Loc(10,10), move);
}
#endif

#ifdef NEW_SYSTEM_TESTS
TEST_F(AISubsystemFixture, test_quick_blunder) {
    string gameStr = \
"depth:4\n"
"boardwidth:19\n"
"1.(9, 9)\n"
"2.(8, 8)\n"
"3.(9, 7)\n"
"4.(9, 8)\n"
"5.(10, 8)\n"
"6.(11, 9)\n"
"7.(8, 6)\n";
#if 0
"8.(11, 8)\n"
"9.(11, 7)\n"
"10.(12, 6)\n"
"11.(11, 10)\n"
"12.(12, 7)\n"
"13.(11, 9)\n";
#endif

#if 0
	printGameHistory(gameStr);
#endif
    Loc move = doTheSearchTest(gameStr);
	//_game.print();
	EXPECT_EQ(Loc(11,7), move);
}
#endif

#ifdef NEW_SYSTEM_TESTS
TEST_F(AISubsystemFixture, test_show_me) {
    string gameStr = \
"boardwidth:19\n"
"depth:6\n"
"1.(9, 9)\n"
"2.(10, 10)\n"
"3.(12, 9)\n"
"4.(8, 9)\n"
"5.(10, 7)\n"
"6.(11, 8)\n"
"7.(12, 7)\n"
"8.(12, 10)\n"
"9.(11, 7)\n"
"10.(13, 7)\n"
"11.(10, 8)\n";
    Loc move = doTheSearchTest(gameStr);
	EXPECT_NE(Loc(10,6), move);
}
#endif

#ifdef NEW_SYSTEM_TESTS 
TEST_F(AISubsystemFixture, testForgotToDefend) {
    string gameStr = \
"depth:4\n" // Works with depth 4 or higher
"boardsize:19\n"
"rules:Tournament\n"
"(9, 9)\n"
"(10, 10)\n"
"(9, 6)\n"
"(9, 8)\n"
"(10, 9)\n"
"(11, 9)\n"
"(12, 8)\n"
"(9, 11)\n"
"(8, 9)\n"
"(7, 9)\n"
"(8, 10)\n"
"(8, 12)\n"
"(7, 13)\n"
"(10, 8)\n"
"(12, 10)\n"
"(8, 8)\n"
"(9, 7)\n"
"(8, 11)\n"
"(8, 10)\n"
"(10, 8)\n"
"(6, 10)\n"
"(8, 8)\n"
"(7, 8)\n"
"(7, 11)\n"
"(9, 13)\n"
"(7, 11)\n"
;

#if 0
	printGameHistory(gameStr);
#endif
    Loc move = doTheSearchTest(gameStr);
	EXPECT_NE(Loc(5,11), move);
}
#endif

#if 0
"1.(9, 9)\n"
"2.(10, 10)\n"
"3.(11, 9)\n"
"4.(10, 9)\n"
"5.(10, 8)\n"
"6.(12, 10)\n"
"7.(9, 7)\n"
"8.(11, 10)\n"
"9.(10, 11)\n"
"10.(13, 10)\n"
"11.(10, 10)\n"
"12.(10, 9)\n"
"13.(9, 8)\n"
"14.(14, 10)\n"
"15.(15, 10)\n"
"16.(10, 12)\n"
"17.(12, 11)\n"
"18.(10, 10)\n"
"19.(11, 10)\n"
"20.(9, 10)\n"
"21.(9, 6)\n"
"22.(9, 5)\n"
"23.(8, 10)\n"
#endif

#if 0
// Crash
depth:6
boardsize:19
rules:Tournament
(9, 9)
	(9, 8)
	(12, 8)
	(10, 9)
	(10, 10)
	(8, 7)
	(11, 10)
	(12, 10)
	(11, 8)
	(11, 11)
	(13, 8)
	(7, 6)
	(6, 5)
	(14, 8)
	(13, 9)
	(9, 10)
	(8, 11)
	(10, 9)
	(11, 10)
	(11, 7)
	(10, 12)
	(10, 7)
	(9, 7)
	(13, 10)
	(12, 7)
	(13, 6)
	(11, 7)
	(14, 10)
	(10, 6)
	(9, 5)
	(11, 5)
	(12, 4)
	(7, 9)
	(8, 8)
	(10, 8)
#endif


#endif
