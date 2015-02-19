#ifndef _UTILITY_H
#define _UTILITY_H

typedef double UtilityValue;

#define INF 1e40
#define NEGINF -1e40

class IABState
{
public:
	IABState() {}
	virtual ~IABState() {}

    virtual bool isOurMove() =0;
};

// template <class State>
class UtilityCalculator
{
public:
	UtilityCalculator(IABState &s) : _state(s) {}

	UtilityValue getUtilityValue() const;

// private:
    IABState& _state;
};

#endif
