#ifndef _utility_calc_h
#define _utility_calc_h

// class PositionStats;

template <class PS>
class UtilityCalc
{
public:
	UtilityCalc(const PS &ps) : _posStats(ps) {}

private:
	const PS &_posStats;
};

#endif
