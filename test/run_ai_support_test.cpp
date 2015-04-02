#define private public

#include "gmock/gmock.h"
#include "bdebug.h"
#include "pente_game.h"
#include "alpha_beta.h"

#include "run_ai_support.h"

#include <string>
using std::string;

#if 0
C vs. D: Contender (19x19 t3) B: 0.04s, W: 1.31s, B/W: 0.03
| Player     t rat. | Size       t rat. | Depth      t rat. | Rules     t rat.
| P1: 2/6    0.0149 | 13: 4/4    5.2345 | 1: 3/5     0.9634 | s: 3/2    0.4626
| P2: 3/3    0.0161 | 19: 5/3    4.2455 | 2: 4/4     12.875 | t: 4/4    0.6127
| Overall    t rat. |
| C/D 16/16  0.7658 |

x/y: contender wins / defender wins
t rat: contender time / defender time
Player P1: contender went first; P2: defender went first
#endif

class RunAIFixture : public testing::Test {
public:
	RunAIFixture() {
	}
};

TEST_F(RunAIFixture, run_one_game) {
	PenteGame defender;
	PenteGame contender;
	RunAIGame g(defender, contender);

	// depth, size, rules, contender first
	GameResult res = g.play(2, 19, 's', true);

	EXPECT_EQ(true, res.winnerWasContender());

	EXPECT_EQ("2", res._depth);
	EXPECT_EQ("19", res._size);
	EXPECT_EQ("s", res._rules);
	EXPECT_EQ("P1", res._contenderP);
	EXPECT_GT(1.0, res._times[false]);
	EXPECT_GT(1.0, res._times[true]);
	EXPECT_LT(0.0001, res._times[false]);
	EXPECT_LT(0.0010, res._times[true]);
}

TEST_F(RunAIFixture, run_another_game) {
	PenteGame defender;
	PenteGame contender;
	RunAIGame g(defender, contender);

	// depth, size, rules, contender first
	GameResult res = g.play(3, 13, 't', false);

	EXPECT_EQ(false, res.winnerWasContender());

	EXPECT_EQ("3", res._depth);
	EXPECT_EQ("13", res._size);
	EXPECT_EQ("t", res._rules);
	EXPECT_EQ("P2", res._contenderP);
	EXPECT_GT(1.0, res._times[false]);
	EXPECT_GT(1.0, res._times[true]);
	EXPECT_LT(0.0001, res._times[false]);
	EXPECT_LT(0.0010, res._times[true]);
}

TEST_F(RunAIFixture, Match) {
	PenteGame p1;
	PenteGame p2;
	Match match(p1, p2);
	match.setDepthRange(1, 2);
	match.setRulesTypes("st");
	match.setSizes(13, 19);
	match.play();
	const AllStats &allStats = match.getAllStats();
	EXPECT_EQ("Depth", allStats.getCategory(0)._catName);
	EXPECT_EQ("Size", allStats.getCategory(1)._catName);
	EXPECT_EQ("Player", allStats.getCategory(2)._catName);
	EXPECT_EQ("Rules", allStats.getCategory(3)._catName);
	EXPECT_EQ("Overall", allStats.getCategory(4)._catName);
}

class AccumFixture : public testing::Test {
public:
	AccumFixture() {
	}
};

TEST_F(AccumFixture, AddGames) {
	AllStats as;
	GameResult gr;
	// double _times[3];
	//gr._winner = P1;
	gr._depth = "1";
	gr._size = "19";
	gr._rules = "S";
	as.addGameResult(gr);
}

#if 0
Game: 2 PenteGame instances
Match: times and runs games, passes them to the AllStats as GameResults
GameResult: Depth, Size, Player, Rules, Winner, C time, D time, C/D time()
AllStats: Contains all the CategoryTypes
CategoryType: {Depth, Size, Player, Rules, Overall} - Name, CategoryValues
CategoryValue: Name, GameCounts
GameCounts - wins, total time, total played
Reporter: Formats the AllStats instance.
#endif
