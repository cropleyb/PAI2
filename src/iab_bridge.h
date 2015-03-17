#ifndef _iab_bridge_h
#define _iab_bridge_h

#include "loc.h"
#include "defines.h"

class IABBridge
{
public:
	IABBridge() {}
	virtual ~IABBridge() {}

    virtual bool isOnlyOneMove() =0;
	virtual UtilityValue getUtility() =0;

	virtual Loc makeNextMove() =0;// Get it and do it on the board
	virtual Loc getNextMove() =0; // Get it without performing it
	virtual void undoLastMove() =0;
    virtual bool isCutoff() const =0;
};

#endif
