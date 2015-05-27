
#include "defines.h"
#include "position_stats.h"
#include "line_pattern.h"
#include "span_entry.h"

// Test code only
void PositionStats::reportCandidate(Colour colour, LinePatternType pt, Loc loc, Step inc)
{
	_patternCounts[colour][pt] += inc;

	PriorityLevel &level = _levels[colour][pt];
	level.addOrRemoveCandidate(loc, inc);
}

// Test code only
void PositionStats::reportCandidates(Colour colour, LinePatternType pt, const vector<Loc> &locArr, Step inc)
{
	_patternCounts[colour][pt] += inc;

	PriorityLevel &level = _levels[colour][pt];
	for(Loc loc : locArr)
	{
		level.addOrRemoveCandidate(loc, inc);
	}
}

//#include <assert.h>
#include <iostream>
using namespace std;

void PositionStats::maintainTake(const SpanEntry &spanEntry, const LinePattern &patternEntry, int inc)
{
	Colour c = patternEntry._colour;
	Colour oc = otherPlayer(c);
	const SpecialOccsTable &sot = _specialOccsTable[oc];

	Loc trigger = spanEntry.convertIndToLoc(patternEntry._inds[0]);
	cout << "MAINTAINTAKE Trigger: " << trigger << " (" << trigger._value << ")" << endl;
	for (int dist=1; dist<=2; dist++) {
		CompressedLoc vuln = _takeTable[c].addOneCap(trigger, spanEntry._direction, spanEntry._offsetPerIndex, dist, inc);
		cout << "vuln: " << Loc(vuln) << " (" << vuln << ")" << endl;

		SpecialOccCounts soc = sot.getCounts(vuln);

		if (soc.all > 0) {
			int numFTs = soc.fours;
			if (numFTs > 0) {
				cout << "MAINTAINTAKE still" << endl;
				cout << "numFTs: " << numFTs << endl;
				PriorityLevel &level = _levels[c][FourTake];
				cout << "adjust: P" << (int)c << " trigger: " << (int)trigger._value << " with diff: " << inc*numFTs << endl;
				cout << "count before: " << level.getCount(trigger) << " benefits for trigger." << endl;
				level.addOrRemoveCandidate(trigger, inc*numFTs);
				cout << "leaving: " << level.getCount(trigger) << " benefits for trigger." << endl;
				cout << "-> " << (int)level.getNumCands() << " candidates" << endl;
			}
			int numBFTs = soc.blocked4s;
			if (numBFTs > 0) {
				// FIXME copy & paste
				cout << "MAINTAINTAKE still" << endl;
				cout << "numBFTs: " << numBFTs << endl;
				PriorityLevel &level = _levels[c][Blocked4Take];
				cout << "adjust: P" << (int)c << " trigger: " << (int)trigger._value << " with diff: " << inc*numFTs << endl;
				cout << "count before: " << level.getCount(trigger) << " benefits for trigger." << endl;
				level.addOrRemoveCandidate(trigger, inc*numBFTs);
				cout << "leaving: " << level.getCount(trigger) << " benefits for trigger." << endl;
				cout << "-> " << (int)level.getNumCands() << " candidates" << endl;
			}
			int numTakes = soc.takes;
			if (numTakes > 0) {
				// FIXME copy & paste
				cout << "MAINTAINTAKE still" << endl;
				cout << "numTakes: " << numTakes << endl;
				PriorityLevel &level = _levels[c][TakeTake];
				cout << "adjust: P" << (int)c << " trigger: " << (int)trigger._value << " with diff: " << inc*numFTs << endl;
				cout << "count before: " << level.getCount(trigger) << " benefits for trigger." << endl;
				level.addOrRemoveCandidate(trigger, inc*numTakes);
				cout << "leaving: " << level.getCount(trigger) << " benefits for trigger." << endl;
				cout << "-> " << (int)level.getNumCands() << " candidates" << endl;
			}
		}
	}
}

void PositionStats::maintainSpecial(const SpanEntry &spanEntry, const LinePattern &patternEntry, int inc)
{
	cout << "MAINTAINSPECIAL" << endl;
	Colour c = patternEntry._colour;
	LinePatternType pt = patternEntry._patternType;
	int numInds = 1;
	if (pt == Line4) {
		numInds = 4;
	} else if (pt == Blocked4) {
		c = otherPlayer(c);
	}
	Colour oc = otherPlayer(c);
	SpecialOccsTable &sot = _specialOccsTable[c];

	for (int ind=0; ind<numInds; ind++) {
		Loc target = spanEntry.convertIndToLoc(patternEntry.target(ind));
		CompressedLoc cTarget = target._value;
		cout << "ind: " << ind << endl;
		cout << "cTarget: " << cTarget << endl;

		switch (pt) {
			case Line4:
				sot._counts[cTarget].fours += inc;
				break;
			case Blocked4:
				sot._counts[cTarget].blocked4s += inc;
				break;
			case Take:
				sot._counts[cTarget].takes += inc;
				break;
			default:
				break;
		}
			
		CapTable &ct = _takeTable[oc];
		CompressedLoc takeDirs = ct.getTakes(cTarget);
		cout << "hasTake: " << takeDirs << endl;
		if (takeDirs) {
			ExtraPriorityLevels epl;

			switch (pt) {
				case Line4:
					epl = FourTake;
					break;
				case Blocked4:
					epl = Blocked4Take;
					break;
				case Take:
					epl = TakeTake;
					break;
				default:
					// Error
					break;
			}
			PriorityLevel &level = _levels[oc][epl];
			int dir;
			for (dir=0; dir<MAX_DIR; dir++) {
				Loc trigger = ct.getTriggerInDirection(cTarget, (DirectionType)dir);
				if (trigger.isValid()) {
					cout << "MAINTAINSPECIAL still" << endl;
					cout << "adjust: P" << (int)oc << " trigger: " << (int)trigger._value << " with diff: " << inc << endl;
					level.addOrRemoveCandidate(trigger, inc);
					cout << "leaving: " << level.getCount(trigger) << " benefits for trigger" << endl;
					cout << "-> " << (int)level.getNumCands() << " candidates." << endl;
				}
			}
		}
	}
}

void PositionStats::maintainTakePLs(const SpanEntry &spanEntry, const LinePattern &patternEntry, int inc)
{
	// Colour c = patternEntry._colour;
	LinePatternType pt = patternEntry._patternType;

	switch (pt) {
		case Line4:
			maintainSpecial(spanEntry, patternEntry, inc); // TEMP -- PUT IT BACK!
			break;
		case Blocked4:
			maintainSpecial(spanEntry, patternEntry, inc);
			break;
		case Take:
			maintainSpecial(spanEntry, patternEntry, inc);
			maintainTake(spanEntry, patternEntry, inc);
			break;
		default:
			break;
	}
}

// Convert each of the indices in patternEntry into Locs using span,
// and update appropriately
void PositionStats::report(const SpanEntry &spanEntry, const LinePattern &patternEntry, int inc)
{
	Colour c = patternEntry._colour;
	LinePatternType pt = patternEntry._patternType;
	Loc loc = spanEntry.convertIndToLoc(patternEntry._inds[0]);
	//cout << "REPORT: type: " << (int)pt << " inc: " << (int)inc << " for: " << loc << " dir: " << spanEntry._direction << endl;
	//assert(pt < MAX_LINE_PATTERN_TYPE);

	if (_takeTargeting) {
		maintainTakePLs(spanEntry, patternEntry, inc);
	}

#if 0
	StId stId = -1;

    if (pt == Line4 or pt == Blocked4 or pt == Line3)
    {
        if (inc > 0) {
			Structure &str = _structureMap.add(pt);
			str._pattern = pt;
			stId = str._id;
			_structureLookup[pt][spanEntry._baseLoc] = stId;
		} else {
			// TODO _structureLookup needs to include direction as well
	// spanEntry._direction; // Direction that this strip goes - up to MAX_DIR (i.e. 4)
			stId = _structureLookup[pt][spanEntry._baseLoc];
			_structureMap.remove(stId);
		}
	}
#endif

	_patternCounts[c][pt] += inc;
	if (pt == Line5) updateWonBy(c);

	PriorityLevel &level = _levels[c][pt];
	for (int i=0; i<patternEntry._numCands; i++)
	{
		Loc loc = spanEntry.convertIndToLoc(patternEntry._inds[i]);
		level.addOrRemoveCandidate(loc, inc);
	}
}

