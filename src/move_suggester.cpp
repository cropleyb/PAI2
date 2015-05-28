
#include "move_suggester.h"
#include "candidate_cache.h"
#include "defines.h"
#include "bdebug.h"
#include "line_pattern.h"
#include "position_stats.h"

//#define MSDEBUG
#ifdef MSDEBUG
#define MSD(X) X
using namespace std;
#else
#define MSD(X)
#endif

MoveSuggester::MoveSuggester(PositionStats &ps)
	: _posStats(ps)
{
	// These values worked a little better,
	// but they can't be compared with PAI1. Or can they?
	_maxMovesShallow = 7;
	_maxMovesDeep = 4;
	//_maxMovesShallow = 9;
	//_maxMovesDeep = 5;
	_shallowCutoff = 4;
	_candCache = new CandidateCache();
}

MoveSuggester::~MoveSuggester()
{
	delete _candCache;
}

void MoveSuggester::reset()
{
	_candCache->reset();
}

bool MoveSuggester::isOnlyOneMove(Depth depth, Colour searchColour)
{
	if (_candCache->needsFilling(depth))
	{
		fillCache(depth, searchColour);
	}

	return _candCache->getNumMoves(depth) == 1;
}

Loc MoveSuggester::getNextMove(Depth depth, Colour searchColour)
{
	if (_candCache->needsFilling(depth))
	{
		fillCache(depth, searchColour);
	}

	return _candCache->getNextMove(depth);
}

void MoveSuggester::fillCache(Depth depth, Colour searchColour)
{
	MSD(cout << "Filling depth: " << (int)depth << " for searchColour: P" << (int)searchColour << endl;)

	Loc *moveBuffer = _candCache->getBuffer(depth);

	Breadth maxMoves = _maxMovesShallow;
	if (depth >= _shallowCutoff) maxMoves = _maxMovesDeep;
	//assert(maxMoves == 9);

	Breadth moveCount = filterCandidates(moveBuffer, depth, maxMoves, searchColour);
	MSD(cout << "Setting depth moves for depth " << (int)depth << " to " << (int)moveCount << endl;)
	_candCache->setDepthMoves(depth, moveCount);
}

bool MoveSuggester::getPriorityLevels(Colour ourColour)
{
	bool onePoss;

	const bool takeTargeting = _posStats._takeTargeting;
	Colour theirColour = otherPlayer(ourColour);

	const PriorityLevel &ourFours
		= _posStats.getPriorityLevel(ourColour, Line4);
	if (ourFours.getNumCands() > 0)
	{
		// This will win
		MSD(cout << "P" << (int)ourColour << " win by fours " << endl;)
		_toSearchLevels[0] = &ourFours;
		_numSearchLevels = 1;
		onePoss = true;
		return onePoss;
	}

	bool cwbc = _posStats.getCanWinByCaptures();

	CapCount ourCaptureCount = _posStats.getCaptured(ourColour);
	const PriorityLevel &ourTakes
		= _posStats.getPriorityLevel(ourColour, Take);

	if (cwbc and ourCaptureCount >= 8 and ourTakes.getNumCands() > 0) {
		// This will win too
		MSD(cout << "P" << (int)ourColour << " win by takes" << endl;)
		_toSearchLevels[0] = &ourTakes;
		_numSearchLevels = 1;
		onePoss = true;
		return onePoss;
	}

	CapCount theirCaptureCount = _posStats.getCaptured(theirColour);
	const PriorityLevel &theirTakes
		= _posStats.getPriorityLevel(theirColour, Take);

	const PriorityLevel &theirFours
		= _posStats.getPriorityLevel(theirColour, Line4);

	if (cwbc and theirCaptureCount >= 8 and theirTakes.getNumCands() > 0) {
		// Block their takes, or capture one of the ends of an
		// attacker, or lose.
		const PriorityLevel &ourTakeTakes
			= _posStats.getPriorityLevel(ourColour, TakeTake);
		_numSearchLevels = 0;
		if (takeTargeting and ourTakeTakes.getNumCands() > 0) {
		    // Only those that are attacking our pairs
			_toSearchLevels[_numSearchLevels++] = &ourTakeTakes;
		}
		if (not takeTargeting and ourTakes.getNumCands() > 0) {
			// Old version for comparison
			_toSearchLevels[_numSearchLevels++] = &ourTakes;
		}
		// Always try to block at least one of their takes
		_toSearchLevels[_numSearchLevels++] = &theirTakes;
		onePoss = false;
		return onePoss;
	}

	if (theirFours.getNumCands() > 0) {
		const PriorityLevel &ourFourTakes
			= _posStats.getPriorityLevel(ourColour, FourTake);
		if (theirFours.getNumCands() > 1) {
			// They have more than one location where they can win, so blocking is impossible.
			_numSearchLevels = 0;
			onePoss = true;
			if (not takeTargeting and ourTakes.getNumCands() > 0) {
				_toSearchLevels[_numSearchLevels++] = &ourTakes;
				// We will lose unless we capture
				MSD(cout << "P" << (int)ourColour << " lose by 5 in a row unless we take" << endl;)
				onePoss = false;
			}
			if (takeTargeting and ourFourTakes.getNumCands() > 0) {
				// TODO: only those that take at least one stone from EACH of their 4s
				_toSearchLevels[_numSearchLevels++] = &ourFourTakes;
				onePoss = false;
			}

			if (_numSearchLevels == 0) {
				MSD(cout << " lose by 5 in a row imminently." << endl;)
			}
			// Might as well block one of them, can't stop 'em all
			_emergencySearchLevel = &theirFours;
			return onePoss;
		}

		// else: They have exactly one 4 attack
		// We will lose unless we block or capture part of it.
		MSD(cout << "P" << (int)ourColour << " lose by 5 in a row unless we block or capture part of it." << endl;)
		_numSearchLevels = 0;
		onePoss = true;

		if (takeTargeting and ourFourTakes.getNumCands() > 0) {
		   	// Only include caps that take from a four.
			// Since we know this take will help, order it first
			_toSearchLevels[_numSearchLevels++] = &ourFourTakes;
			onePoss = false;
		}
		_toSearchLevels[_numSearchLevels++] = &theirFours;
		if (not takeTargeting and ourTakes.getNumCands() > 0) {
			// Old behaviour for comparison...
			_toSearchLevels[_numSearchLevels++] = &ourTakes;
			onePoss = false;
		}
		return onePoss;
	}
		
	// Defaulting to many levels
	fillPriorityLevels(ourColour, theirColour);
	_numSearchLevels = 10;
	onePoss = false;
	return onePoss;
}

#if 0
	// TODO
	const PriorityLevel &ourThreats
		= _posStats.getPriorityLevel(ourColour, Threat);

	if (cwbc and ourCaptureCount >= 8 and ourThreats.getNumCands() > 0) {
		// We have only one capture left to win, so a threat is threatening to win next move.
		const PriorityLevel &ourThrees
			= _posStats.getPriorityLevel(ourColour, Line3);
		const PriorityLevel &theirThrees
			= _posStats.getPriorityLevel(theirColour, Line3);
		const PriorityLevel &ourTwos
			= _posStats.getPriorityLevel(ourColour, Line2);
		const PriorityLevel &theirTwos
			= _posStats.getPriorityLevel(theirColour, Line2);

		_toSearchLevels[0] = &ourThreats;
		_toSearchLevels[1] = &ourThrees;
		_toSearchLevels[2] = &theirThrees;
		_toSearchLevels[3] = &ourTwos;
		_toSearchLevels[4] = &theirTwos;
		//_emergencySearchLevel = &theirFours;
		_numSearchLevels = 5;
		_includeAllSearchLevels = 2; // We must look at all of these.
		onePoss = false;
		return onePoss;
	}
#endif
	
#if 0
	const PriorityLevel &ourThrees
		= _posStats.getPriorityLevel(ourColour, Line3);
	const PriorityLevel &theirThrees
		= _posStats.getPriorityLevel(theirColour, Line3);

	if (theirThrees.getNumCands() > 0) {
		const PriorityLevel &ourThreats
			= _posStats.getPriorityLevel(ourColour, Threat);

		Loc multi3Cands[3]; // If there are more than 3 we don't care.
		Breadth numMulti3Cands = theirThrees.getMultiCands(multi3Cands);
		if (numMulti3Cands >= 3) {
			// i.e. three or more double 3 attacks cannot be blocked;
			// we must extend a 3 of our own, capture or threaten
			MSD(cout << "P" << (int)ourColour << " lose by double four attack unless we attack first." << endl;)
			_toSearchLevels[0] = &ourThrees;
			_toSearchLevels[1] = &ourTakes;
			_toSearchLevels[2] = &ourThreats;
			_emergencySearchLevel = &theirThrees; // We only want one of these as an emergency
			_numSearchLevels = 3;
			onePoss = false;
			return onePoss;
		}
#if 0
		else if (numMulti3Cands >= 1) {
			// Could block if we place on one of the threes.
			MSD(cout << "P" << (int)ourColour << " lose by double four attack unless we block or attack first." << endl;)
			_toSearchLevels[0] = &ourThrees;
			_toSearchLevels[1] = &theirThrees;
			_toSearchLevels[2] = &ourTakes;
			_toSearchLevels[3] = &theirTakes;
			_toSearchLevels[4] = &ourThreats;
			_numSearchLevels = 5;
			onePoss = false;
			return onePoss;
		}
		//if (numMulti3Cands == 2) { // TODO
			// Could block if we place on one of the multi cands.
		//}
#endif
	}
#endif


// TODO This could be cached.
void MoveSuggester::fillPriorityLevels(Colour ourColour, Colour theirColour)
{
	int i=0;
	int level;
#if 0
	if (_posStats._takeTargeting) {
		// Targetted takes should be searched first
		// There is no point including FourTakes as they will have been used above if necessary
		for (level=Blocked4Take; level>=TakeTake; level--)
		//for (level=FourTake; level>=TakeTake; level--)
		{
			_toSearchLevels[i++] = &_posStats.getPriorityLevel(ourColour, (LinePatternType)level);
			_toSearchLevels[i++] = &_posStats.getPriorityLevel(theirColour, (LinePatternType)level);
		}
	}
#endif
	for (level=Take; level>=Line1; level--)
	{
		_toSearchLevels[i++] = &_posStats.getPriorityLevel(ourColour, (LinePatternType)level);
		_toSearchLevels[i++] = &_posStats.getPriorityLevel(theirColour, (LinePatternType)level);
	}
}

Breadth MoveSuggester::filterCandidates(Loc *moveBuffer, Depth depth, Breadth maxMoves, Colour ourColour)
{
	Breadth found = 0;
	U64 seen[MAX_WIDTH] = { (U64)0 };
	
	Colour turnColour = ourColour;
	if (depth % 2) {
		turnColour = otherPlayer(ourColour);
	}

	_emergencySearchLevel = 0;

	bool onePoss = getPriorityLevels(turnColour);
	if (onePoss) {
		maxMoves = 1;
	}

	// cout << "_numSearchLevels: " << _numSearchLevels << endl;
	for (int slotInd=0; slotInd<_numSearchLevels; slotInd++)
	{
		const PriorityLevel *pl = _toSearchLevels[slotInd];
		Breadth foundFromPL = pl->getCands(moveBuffer, maxMoves-found, seen);
#ifdef MSDEBUG
		cout << "MS Found " << (int)foundFromPL << " in PL " << pl->getLevelName();
		if (foundFromPL > 0) {
			cout << ": ";
			for (int ii=0; ii<foundFromPL; ii++) {
				cout << " " << moveBuffer[ii];
			}
			cout << endl;
		}
#endif

		found += foundFromPL;
		if (found >= maxMoves)
		{
			MSD(cout << "Found enough in MS" << endl;)
			return found;
		}
		moveBuffer += foundFromPL;
	}
	if (found == 0 && depth < 1 && _emergencySearchLevel != 0)
	{
		// We need at least one move at the top level, or things go wrong.
		MSD(cout << "Resorting to emergency level in MS" << endl;)
		const PriorityLevel *pl = _emergencySearchLevel;
		found = pl->getCands(moveBuffer, 1, seen);
	}
	return found;
}

