
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
	MSD(cout << "Filling depth: " << (int)depth << endl;)
	Loc *moveBuffer = _candCache->getBuffer(depth);

	Breadth maxMoves = 9;
	if (depth > 3) maxMoves = 4;

	Breadth moveCount = filterCandidates(moveBuffer, depth, maxMoves, searchColour);
	MSD(cout << "Setting depth moves for depth " << (int)depth << " to " << (int)moveCount << endl;)
	_candCache->setDepthMoves(depth, moveCount);
}

bool MoveSuggester::getPriorityLevels(Colour ourColour)
{
	bool onePoss;

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
		// attacker, or lose. Include blocking one of their fours or we
		// may have no moves
		_toSearchLevels[0] = &ourTakes;
		_toSearchLevels[1] = &theirTakes;
		_toSearchLevels[2] = &theirFours;
		_numSearchLevels = 3;
		onePoss = false;
		return onePoss;
	}

	if (theirFours.getNumCands() > 0) {
		if (theirFours.getNumCands() > 1) {
			if (ourTakes.getNumCands() > 0) {
				// We will lose unless we capture
				MSD(cout << "P" << (int)ourColour << " lose by 5 in a row unless we take" << endl;)
				_toSearchLevels[0] = &ourTakes;
				_toSearchLevels[1] = &theirFours; // This loses, but we need a move
				_numSearchLevels = 2;
				onePoss = false;
				return onePoss;
			} else {
				// Might as well block one of them, can't stop 'em all
				MSD(cout << " lose by 5 in a row imminently." << endl;)
				_toSearchLevels[0] = &theirFours;
				_numSearchLevels = 1;
				onePoss = true;
				return onePoss;
			}
		}

		// else: They have one 4 attack or 2 or more winning threat attacks.
		// We will lose unless we block or capture 
		// Provide a threat blocking alternative so we always have at least
		// one move.
		MSD(cout << "P" << (int)ourColour << " lose by 5 in a row unless we block or capture." << endl;)
		_toSearchLevels[0] = &theirFours;
		_toSearchLevels[1] = &ourTakes;
		_toSearchLevels[2] = &theirTakes;
		_numSearchLevels = 3;
		onePoss = false;
		return onePoss;
	}
	// Defaulting to many levels
	fillPriorityLevels(ourColour, theirColour);
	_numSearchLevels = 10;
	onePoss = false;
	return onePoss;
}

// TODO This could be cached.
void MoveSuggester::fillPriorityLevels(Colour ourColour, Colour theirColour)
{
	int i=0;
	for (int level=Take; level>=Line1; level--)
	{
		_toSearchLevels[i++] = &_posStats.getPriorityLevel(ourColour, (PatternType)level);
		_toSearchLevels[i++] = &_posStats.getPriorityLevel(theirColour, (PatternType)level);
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
			for (int ii=found; ii<found+foundFromPL; ii++) {
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
	return found;
}

