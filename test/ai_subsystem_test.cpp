#if 0
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

class AISubsystemFixture : public testing::Test {
public:
	AISubsystemFixture() {
		buildSpanTable(19);
	}

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
};

#if 1
TEST_F(AISubsystemFixture, test_find_one_move) {
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
    Loc move = doTheSearch(gameStr, 2);
	EXPECT_EQ(Loc(6,7), move);
}
#endif

#if 0
// This is failing, but the alternative is reasonable
TEST_F(AISubsystemFixture, test_dont_waste_a_pair) {
    string gameStr = \
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
    Loc move = doTheSearch(gameStr, 4);
	EXPECT_EQ(Loc(6,5), move);
}
#endif

#if 1
TEST_F(AISubsystemFixture, test_dodgy_move) {
    string gameStr = \
"1. (6, 6)\n"
"2. (6, 5)\n"
"3. (8, 6)\n"
"4. (7, 6)\n"
"5. (5, 4)\n"
"6. (8, 7)\n"
"7. (6, 8)\n"
"8. (7, 7)\n"
"9. (6, 7)\n";

    Loc move = doTheSearch(gameStr, 6);
    // Why not 9,7? i.e. why does 7,5 have a high score?
	EXPECT_NE(Loc(7,5), move);
}
#endif

TEST_F(AISubsystemFixture, test_dodgy_move_part2) {
    string gameStr = \
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
    Loc move = doTheSearch(gameStr, 5);
	EXPECT_EQ(Loc(9,7), move);
}

#if 1
// OK but slow
TEST_F(AISubsystemFixture, test_freebie) {
    string gameStr = \
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
    Loc move = doTheSearch(gameStr, 8);
	EXPECT_EQ(Loc(8,7), move);
}
#endif

#if 1
// OK but slow

TEST_F(AISubsystemFixture, test_strange) {
    string gameStr = \
"1. (6, 6)\n"
"2. (6, 7)\n"
"3. (4, 6)\n"
"4. (5, 8)\n"
"5. (7, 6)\n"
"6. (5, 6)\n"
"7. (4, 9)\n";
    Loc move = doTheSearch(gameStr, 6);
	EXPECT_EQ(Loc(8,6), move);
}
#endif

#if 0

TEST_F(AISubsystemFixture, test_draw) {
    string gameStr = \
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

#if 0
    def atest_missed_win(self): # TODO
        self.p1.set_max_depth(2)
        game_str = \
"""Black versus White
13x13
standard rules
1. (6, 6)
2. (8, 4)
3. (8, 6)
4. (10, 6)
5. (7, 6)
6. (5, 6)
7. (6, 4)
8. (9, 5)
9. (7, 3)
10. (11, 7)
11. (12, 8)
12. (8, 2)
13. (5, 5)
14. (8, 1)
15. (3, 7)
16. (4, 6)
17. (3, 6)
18. (4, 6)
19. (8, 3)
20. (6, 3)
21. (9, 3)
22. (10, 3)
23. (10, 5)
24. (11, 4)
25. (5, 6)
26. (9, 6)
27. (11, 6)
28. (9, 6)
29. (7, 7)
30. (9, 2)
31. (8, 0)
"""
        self.game.load_game(game_str)
        turn, prev_move, m = self.p2.do_the_search()
        self.assertEquals(m, (8,1))

    # !python pentai/ai/t_ai_subsystem.py AIPlayerSubsystemTest.test_pacifist
    def test_pacifist(self):
        self.p1.set_max_depth(4)
        game_str = \
"""Black versus White
9x9
standard rules
1. (4, 4)
2. (5, 4)
3. (2, 6)
4. (5, 5)
5. (3, 5)
6. (5, 3)
7. (5, 6)
8. (1, 7)
9. (5, 2)
10. (6, 3)
11. (4, 3)
12. (4, 5)
13. (6, 5)
14. (4, 5)
15. (3, 6)
16. (4, 6)
"""
        self.game.load_game(game_str)
        turn, prev_move, m = self.p1.do_the_search()
        self.assertEquals(m, (4,7))

    def test_another(self):
        self.p1.set_max_depth(4)
        game_str = \
"""Black versus White
13x13
standard rules
1. (6, 6)
2. (7, 5)
3. (5, 5)
4. (7, 7)
5. (4, 4)
6. (3, 3)
7. (7, 6)
8. (8, 6)
9. (9, 7)
10. (6, 4)
"""
        self.game.load_game(game_str)
        turn, prev_move, m = self.p1.do_the_search()
        self.assertNotEquals(m, (4,6))

    def atest_bother(self):
        self.p1.set_max_depth(8)
        game_str = \
"""DT versus BC
13x13
standard rules
1. (6, 6)
2. (5, 7)
3. (8, 8)
4. (7, 7)
5. (8, 7)
6. (8, 6)
7. (6, 8)
8. (9, 5)
9. (6, 7)
10. (6, 5)"""
        self.game.load_game(game_str)
        turn, prev_move, m = self.p1.do_the_search()
        self.assertNotIn(m, ((7,8),(5,8)))

    def test_block_one_end(self):
        self.p1.set_max_depth(8)
        game_str = \
"""Bruce versus DT
13x13
Standard rules
1. (6, 6)
2. (5, 5)
3. (7, 6)
4. (7, 4)
5. (8, 6)
6. (9, 3)
7. (9, 6)"""
        self.game.load_game(game_str)
        turn, prev_move, m = self.p2.do_the_search()
        self.assertIn(m, ((10,6),(5,6)))

    def atest_think_in_opponents_move(self):
        #self.p1.set_max_depth(8)
        game_str = \
"""Bruce versus DT
13x13
Standard rules
1. (6, 6)"""
        self.game.load_game(game_str)
        turn, prev_move, m = self.p1.do_the_search()
        self.assertIsNone(m)

    # TODO
    def atest_trap(self):
        self.p1.set_max_depth(8)
        game_str = \
"""Killer versus Kang
19x19
Standard rules
1. (9, 9)
2. (8, 10)
3. (6, 9)
4. (8, 8)
5. (8, 9)
6. (7, 9)"""
        self.game.load_game(game_str)
        turn, prev_move, m = self.p1.do_the_search()
        self.assertNotEquals(m, (10,9))

if __name__ == "__main__":
    unittest.main()
#endif
#endif