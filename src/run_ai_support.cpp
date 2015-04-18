// TEMP for debugging...?
#define private public

#include "pente_game.h"
#include "loc.h"
#include "defines.h"
#include "span_lookup_table.h"
#include "alpha_beta.h"
#include "run_ai_support.h"

#include <iostream>
#include <strstream>

#include <chrono>
#include <time.h>       /* time_t, struct tm, difftime, time, mktime */

// RunAI program - for setting up battles between tweaked versions of the AI.

class Timer
{
	// Downloaded from the net
public:
    Timer() : beg_(clock_::now()) {}
    void reset() { beg_ = clock_::now(); }
    double elapsed() const { 
        return std::chrono::duration_cast<second_>
            (clock_::now() - beg_).count(); }

private:
    typedef std::chrono::high_resolution_clock clock_;
    typedef std::chrono::duration<double, std::ratio<1> > second_;
    std::chrono::time_point<clock_> beg_;
};

//////////////////////////////////
// RunAIGame class
//////////////////////////////////

GameResult RunAIGame::play(Depth depth, BoardWidth size, RulesType rules, bool contenderFirst)
{
	for (int i=0; i<=1; i++)
	{
		PenteGame *p = _players[i];
		p->restartGame();
		p->setColour(i+1);
		p->setMaxDepth(depth);
		p->setBoardSize(size);
		p->setRules(rules);
	}

	int toMove = P1;

	AlphaBeta ab_games[2] = {
		AlphaBeta(*_players[0]),
		AlphaBeta(*_players[1])
	};

	time_t before, after;

	GameResult res = GameResult();
	res._depth = std::to_string((int)depth);
	res._size = std::to_string((int)size);
	res._rules = rules;
	res._contenderP = (contenderFirst ? "P1" : "P2");

	Colour winner = EMPTY;

	while (winner == EMPTY)
	{
		Timer tmr;
		Loc bestMove = ab_games[toMove-1].getBestMove();
		if (!_silent) {
			std::cout << bestMove << std::endl;
		}
		res._times[toMove-1] += tmr.elapsed();
		assert(_players[0]->isLegalMove(bestMove));

		_players[0]->makeMove(bestMove, toMove);
		_players[1]->makeMove(bestMove, toMove);

		toMove = otherPlayer(toMove);
		winner = _players[1]->getWonBy();
		if (!_silent) {
			// TODO: Use another flag for games
			_players[0]->print();

#if 0
			const PriorityLevel &p1Threes
				= _players[0]->_posStats.getPriorityLevel(P1, Line3);
			const PriorityLevel &p2Threes
				= _players[0]->_posStats.getPriorityLevel(P2, Line3);
			const PriorityLevel &p1Fours
				= _players[0]->_posStats.getPriorityLevel(P1, Line4);
			const PriorityLevel &p2Fours
				= _players[0]->_posStats.getPriorityLevel(P2, Line4);

			cout << "P1 3s: " << p1Threes.getNumCands()
				 << "; P2 3s: " << p2Threes.getNumCands()
				 << "; P1 4s: " << p1Fours.getNumCands()
				 << "; P2 4s: " << p2Fours.getNumCands() << endl;
#endif
		}
	}
	res._winnerWasContender = ((winner==P2) xor contenderFirst);

	return res;
}

void RunAIGame::printHistory()
{
	_players[0]->printHistory();
}

//////////////////////////////////
// GameResult class
//////////////////////////////////

string GameResult::getValue(const string& categoryName) const
{
	if (categoryName == "Player") return _contenderP;
	else if (categoryName == "Size") return _size;
	else if (categoryName == "Depth") return _depth;
	else if (categoryName == "Rules") return _rules;
	else if (categoryName == "Overall") return "C/D";
	// Anything else?
	return "C/D";
}

//////////////////////////////////
// CategoryType class
//////////////////////////////////

void CategoryType::addGameResult(const GameResult &gr)
{
	string categoryName = _catName;
	string value = gr.getValue(categoryName);
    auto search = _values.find(value);
 
    if (search == _values.end()) {
		_values[value] = GameCounts();
    }
	_values[value].add(gr);
}
	
GameCounts &CategoryType::getCounts(const string &val)
{
	return _values[val];
}

string CategoryType::getHeader()
{
	strstream ss;
	ss << setw(11) << left << _catName << "t rat." << ends;
	string ret = ss.str();
	ss.freeze(false);
	return ret;
}

//////////////////////////////////
// AllStats class
//////////////////////////////////

void AllStats::addGameResult(const GameResult &gr)
{
	for (int c=0; c<5; c++)
	{
		_categoryTypes[c].addGameResult(gr);
	}
}


string AllStats::report()
{
	// TODO: Games in order.
	//: {Depth, Size, Player, Rules, Overall} - Name, CategoryValues
	// work out how many rows we have.
	int numRows = 6; // 3 for Player + Blank + 2 for Overall

	for (int cI=1; cI < 4; cI++)
	{
		int currNumRows = _categoryTypes[cI].getNumRows();
	    if (currNumRows > numRows) numRows = currNumRows;
	}

	// Iterate over rows, and get the relevant category info
	strstream ss;
	for (int currRowNum=0; currRowNum<numRows; currRowNum++)
	{
		for (int catInd=0; catInd<4; catInd++)
		{
			int realCatInd = catInd;
			int realRowNum = currRowNum;

			if (currRowNum >= 4 and catInd == 0) {
				realCatInd = 4;
				realRowNum = currRowNum - 4;
			}
			ss << "| " << _categoryTypes[realCatInd].getRow(realRowNum) << " ";
		}
		ss << "\n";
	}
	ss << ends;
	string ret = ss.str();
	ss.freeze(false);
	return ret;
}
#if 0
"| Player     t rat. | Size       t rat. | Depth      t rat. | Rules     t rat."
"| P2: 1/0    0.0149 | 13: 4/4    5.2345 | 2: 3/5     0.9634 | T: 3/2    0.4626"
"|                   |                   |                   |                 "
"| Overall    t rat. |                   |                   |                 "
"| C/D 16/16  0.7658 |                   |                   |                 ";
#endif
//	CategoryType _categoryTypes[5];

//////////////////////////////////
// Match class
//////////////////////////////////

void Match::play()
{
	Timer tmr;
	int gameNum = 0;
	for (int depth = _minDepth; depth<=_maxDepth; depth++)
	{
		for (BoardWidth size : _sizes)
		{
			for (char rules : _rulesTypes)
			{
				int swap = 0;
				while (swap < 2)
				{
					gameNum++;
					_players[0]->setBoardSize(size);
					_players[1]->setBoardSize(size);
					_players[0]->restartGame();
					_players[1]->restartGame();

					if (!_silent) {
						cout << "Game: " << gameNum << " ";
						cout << "boardsize: " << (int)size;
						cout << " rules: " << rules;
						cout << " depth: " << (int)depth;
						cout << " swap: " << (int)swap << endl;
					}
	
					RunAIGame rag(*(_players[0]), *(_players[1]));
					rag.setSilent(_silent);
					GameResult res = rag.play(depth, size, rules, swap);
					swap += 1;
					_allStats.addGameResult(res);
					if (!_silent) {
						// rag.printHistory(); // TODO: another option
						std::cout << _allStats.report() << endl;
					}
				}
			}
		}
	}
	std::cout << "Total time: " << setprecision(4) << tmr.elapsed() << endl;
}

#if 0
Game 19 3 t was won by: B Contender
B: 0.04s, W: 1.31s, B/W: 0.03
D vs. C: Defender (19x19 s2) B: 0.15s, W: 0.02s, B/W: 6.77
D vs. C: Contender (19x19 t2) B: 0.08s, W: 0.01s, B/W: 5.76
C vs. D: Defender (19x19 s2) B: 0.02s, W: 0.13s, B/W: 0.13
C vs. D: Defender (19x19 t2) B: 0.01s, W: 0.09s, B/W: 0.15
D vs. C: Contender (19x19 s3) B: 1.17s, W: 0.05s, B/W: 24.17
D vs. C: Contender (19x19 t3) B: 1.04s, W: 0.05s, B/W: 20.42
C vs. D: Contender (19x19 s3) B: 0.06s, W: 1.21s, B/W: 0.05
C vs. D: Contender (19x19 t3) B: 0.04s, W: 1.31s, B/W: 0.03
B/W:[0, 3, 5] won: {'Defender': 3, 'Contender': 5} C/D avg. time: 0.09692
Depth      t rat. | Size       t rat. | Player     t rat. | Rules     t rat. | 
1: 3/5     0.9634 | 13: 4/4    5.2345 | P1: 2/6    0.0149 | s: 3/2    0.4626 | 
2: 4/4     12.875 | 19: 5/3    4.2455 | P2: 3/3    0.0161 | t: 4/4    0.6127 | 
Overall
C/D 16/16  0.7658 |

classes:
Game: 2 PenteGame instances
Match: times and runs games, passes them to the AllStats as GameResults
GameResult: Depth, Size, Player, Rules, Winner, C time, D time, C/D time()
AllStats: Contains all the CategoryTypes
CategoryType: {Depth, Size, Player, Rules, Overall} - Name, CategoryValues
CategoryValue: Name, GameCounts
GameCounts - wins, total time, total played
Reporter: Formats the AllStats instance.

Need two players ("PenteGame")
Override defaults before match
Timing
Counts by several categories: depth, board size, rules, first player, overall
Full Report so far at end of each game.
#endif


