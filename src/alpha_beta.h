#include "loc.h"
#include "defines.h"
#include "iab_bridge.h"

class AlphaBeta
{
public:
	AlphaBeta(IABBridge &bridge) : _bridge(bridge) {}

	Loc getBestMove();

private:
    UtilityValue maxValue(UtilityValue alpha, UtilityValue beta, Depth depth);
    UtilityValue minValue(UtilityValue alpha, UtilityValue beta, Depth depth);

	IABBridge &_bridge;
    Loc _bestTopLevelMove;
};

