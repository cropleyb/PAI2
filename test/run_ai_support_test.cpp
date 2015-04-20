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
| P1: 2/6    0.0149 | 13: 4/4    5.2345 | 1:  3/5    0.9634 | s: 3/2    0.4626
| P2: 3/3    0.0161 | 19: 5/3    4.2455 | 2:  4/4    12.875 | t: 4/4    0.6127
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
	g.setSilent(true);

	// depth, size, rules, contender first
	GameResult res = g.play(2, 19, 's', true);

	//EXPECT_EQ(true, res.winnerWasContender());

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
	g.setSilent(true);

	// depth, size, rules, contender first
	GameResult res = g.play(3, 13, 't', false);

	//EXPECT_EQ(true, res.winnerWasContender());

	EXPECT_EQ("3", res._depth);
	EXPECT_EQ("13", res._size);
	EXPECT_EQ("t", res._rules);
	EXPECT_EQ("P2", res._contenderP);
	EXPECT_GT(1.0, res._times[false]);
	EXPECT_GT(1.0, res._times[true]);
	EXPECT_LT(0.0001, res._times[false]);
	EXPECT_LT(0.0010, res._times[true]);
}

#if 1
TEST_F(RunAIFixture, RunMatch) {
	PenteGame p1;
	PenteGame p2;
	Match match(p1, p2);
	match.setSilent(true);
	match.setDepthRange(1, 2); // inc.
	match.setSizes(13, 19);
	match.setRulesTypes("st");
	//match.setRulesTypes("5");
	match.play();
	AllStats &allStats = match.getAllStats();
	EXPECT_EQ("Player", allStats.getCategory(0)._catName);
	EXPECT_EQ("Size", allStats.getCategory(1)._catName);
	EXPECT_EQ("Depth", allStats.getCategory(2)._catName);
	EXPECT_EQ("Rules", allStats.getCategory(3)._catName);
	EXPECT_EQ("Overall", allStats.getCategory(4)._catName);
}
#endif

class AccumFixture : public testing::Test {
public:
	AccumFixture() {
	}
};

TEST_F(AccumFixture, AddOneGame) {
	AllStats as;
	GameResult gr;
	gr._contenderP = "P1";
	gr._depth = "1";
	gr._size = "19";
	gr._rules = "S";
	gr._winnerWasContender = false;
	as.addGameResult(gr);

	CategoryType &ct = as.getCategory(1);
	EXPECT_EQ("Size", ct._catName);
	GameCounts &gc = ct.getCounts("19");
	EXPECT_EQ("0/1", gc.getWinStr());
}

TEST_F(AccumFixture, AddADifferentGame) {
	AllStats as;
	GameResult gr;
	gr._contenderP = "P2";
	gr._depth = "2";
	gr._size = "13";
	gr._rules = "T";
	gr._winnerWasContender = true;
	as.addGameResult(gr);

	CategoryType &ct2 = as.getCategory(0);
	EXPECT_EQ("Player", ct2._catName);
	GameCounts &gc2 = ct2.getCounts("P2");
	EXPECT_EQ("1/0", gc2.getWinStr());
}

TEST_F(AccumFixture, AddTwoGamesP2ContenderWon) {
	AllStats as;
	GameResult gr;
	gr._contenderP = "P2";
	gr._depth = "2";
	gr._size = "13";
	gr._rules = "T";
	gr._winnerWasContender = true;
	gr._times[0] = 0.1258;
	gr._times[1] = 0.5032;
	as.addGameResult(gr);
	as.addGameResult(gr);

	CategoryType &ct2 = as.getCategory(0);
	EXPECT_EQ("Player", ct2._catName);
	GameCounts &gc2 = ct2.getCounts("P2");
	EXPECT_EQ("2/0", gc2.getWinStr());
	EXPECT_EQ("0.2500", gc2.getTimeStr()); // Average ratio
}

TEST_F(AccumFixture, AddTwoGamesP2ContenderLost) {
	AllStats as;
	GameResult gr;
	gr._contenderP = "P2";
	gr._depth = "2";
	gr._size = "13";
	gr._rules = "T";
	gr._winnerWasContender = false;
	gr._times[0] = 0.1258;
	gr._times[1] = 0.5032;
	as.addGameResult(gr);
	as.addGameResult(gr);

	string expectedHeader = "Player     t rat.";
	CategoryType &ct2 = as.getCategory(0);
	EXPECT_EQ("Player", ct2._catName);
	EXPECT_EQ(expectedHeader, ct2.getHeader());
	GameCounts &gc2 = ct2.getCounts("P2");
	EXPECT_EQ("0/2", gc2.getWinStr());
	EXPECT_EQ("0.2500", gc2.getTimeStr()); // Average ratio
	string expectedData1 = "0/2    0.2500";
	EXPECT_EQ(expectedData1, gc2.getStr());
	EXPECT_EQ(2, ct2.getNumRows());
	EXPECT_EQ(expectedHeader, ct2.getRow(0));
	EXPECT_EQ(string("P2: ") + expectedData1, ct2.getRow(1));
	EXPECT_EQ(string("                 "), ct2.getRow(2));
}

TEST_F(AccumFixture, AddTwoGamesP1ContenderWon) {
	AllStats as;
	GameResult gr;
	gr._contenderP = "P1";
	gr._depth = "2";
	gr._size = "13";
	gr._rules = "T";
	gr._winnerWasContender = true;
	gr._times[0] = 0.1258;
	gr._times[1] = 0.5032;
	as.addGameResult(gr);
	as.addGameResult(gr);

	CategoryType &ct2 = as.getCategory(0);
	EXPECT_EQ("Player", ct2._catName);
	GameCounts &gc = ct2.getCounts("P1");
	EXPECT_EQ("2/0", gc.getWinStr());
	EXPECT_EQ("0.2500", gc.getTimeStr()); // Average ratio
}

#if 1
TEST_F(AccumFixture, TestReport) {
	AllStats as;
	GameResult gr;
	gr._contenderP = "P2";
	gr._depth = "2";
	gr._size = "13";
	gr._rules = "T";
	gr._times[0] = 0.1258;
	gr._times[1] = 0.5032;
	gr._winnerWasContender = true;
	as.addGameResult(gr);

	string expectedReportStr =
//" C vs. D: Contender (19x19 t3) B: 0.04s, W: 1.31s, B/W: 0.03" // TODO
"_______________________________________________________________________________\n"
"\n"
"| Player     t rat. | Size       t rat. | Depth      t rat. | Rules      t rat. \n"
"| P2: 1/0    0.2500 | 13: 1/0    0.2500 | 2:  1/0    0.2500 | T:  1/0    0.2500 \n"
"|                   |                   |                   |                   \n"
"|                   |                   |                   |                   \n"
"| Overall    t rat. |                   |                   |                   \n"
"| C/D:1/0    0.2500 |                   |                   |                   \n"
"_______________________________________________________________________________\n\n";
	string wholeReport = as.report();
	EXPECT_EQ(expectedReportStr, wholeReport);
}
#endif

#if 0
x/y: contender wins / defender wins
t rat: contender time / defender time
Player P1: contender went first; P2: defender went first
#endif
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
