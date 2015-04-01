#define private public

#include "gmock/gmock.h"
#include "bdebug.h"
#include "pente_game.h"
#include "alpha_beta.h"

#include "run_ai_support.h"

#include <string>
using std::string;

class RunAIFixture : public testing::Test {
public:
	RunAIFixture() {
	}
};

TEST_F(RunAIFixture, run_one_game) {
	PenteGame p1;
	PenteGame p2;
	p1.setColour(P1);
	p2.setColour(P2);
	RunAIGame g(p1, p2);
	GameResult res = g.play(2, 19, 's');
	EXPECT_EQ(P1, res.getWinner());
	EXPECT_EQ(1, res.wasWonBy(P1));
	EXPECT_GT(1.0, res.getTime(P1));
	EXPECT_GT(1.0, res.getTime(P2));
	EXPECT_LT(0.0001, res.getTime(P1));
	EXPECT_LT(0.0001, res.getTime(P2));
	EXPECT_EQ("2", res._depth);
	EXPECT_EQ("19", res._size);
	EXPECT_EQ("s", res._rules);
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
