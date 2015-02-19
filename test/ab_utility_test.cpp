#include "gmock/gmock.h"

#define private public

#include "loc.h"
#include "alpha_beta.h"
#include "bdebug.h"

class ABUtilityFixture : public testing::Test {
public:

};

class MockABState : public IABState
{
public:
    MOCK_METHOD0(isOurMove, bool());
#if 0
    MOCK_CONST_METHOD0(isOneMove, bool());
	MOCK_METHOD0(getUtilityAndMove, std::pair<UtilityValue, Loc>());
    MOCK_METHOD0(makeNextMove, Loc());
    MOCK_METHOD0(undoLastMove, void());
    MOCK_CONST_METHOD1(isCutoff, bool(unsigned char depth));
#endif
};

using ::testing::AtLeast;
using ::testing::Return;
using ::testing::InSequence;

TEST_F(ABUtilityFixture, GetOurUtility) {
	// Give a value to a state based on a set of criteria
	MockABState ms;
	
	EXPECT_CALL(ms, isOurMove())
      .WillOnce(Return(true))
      ;

    //UtilityCalculator<MockABState> uc;
    UtilityCalculator uc(ms);

	// TODO: UtilityValue utilVal =
	uc.getUtilityValue();
	// EXPECT_EQ(100.0, utilVal);
}

