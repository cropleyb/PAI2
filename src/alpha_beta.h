#include "loc.h"
#include "ab_utility.h"
#include <utility>

class IABBridge
{
public:
	IABBridge() {}
	virtual ~IABBridge() {}

    virtual bool isOneMove() const =0;
    virtual Loc getOnlyMove() =0;
	virtual std::pair<UtilityValue, Loc> getUtilityAndMove() =0;

	virtual Loc makeNextMove() =0;
	virtual void undoLastMove() =0;
    virtual bool isCutoff(unsigned char depth) const =0;
};

class AlphaBeta
{
public:
	AlphaBeta(IABBridge &bridge) : _bridge(bridge) {}

	Loc getBestMove();

private:
    std::pair<UtilityValue, Loc> maxValue(UtilityValue alpha, UtilityValue beta, unsigned char depth);
    std::pair<UtilityValue, Loc> minValue(UtilityValue alpha, UtilityValue beta, unsigned char depth);

	IABBridge &_bridge;
};

