#ifndef _run_ai_support_h
#define _run_ai_support_h

#include <iostream>
#include <iomanip>

using namespace std;

class PenteGame;

class GameResult
{
public:
	GameResult() : _times { 0 } {}

	bool winnerWasContender() { return _winnerWasContender; }

	string getValue(const string& categoryName) const;

	double _times[2];
	bool _winnerWasContender;

	string _contenderP;
	string _depth;
	string _size;
	string _rules;
};

class RunAIGame
{
public:
	RunAIGame(PenteGame &defender, PenteGame &contender) :
		_players {&defender, &contender} {}

	GameResult play(Depth depth, BoardWidth size, RulesType rules, bool contenderFirst);
	
private:
	PenteGame *_players[2];
};

#include <strstream>

class GameCounts
{
public:
	GameCounts() : _wins(0), _losses(0), _ourTotalTime(0), _theirTotalTime(0) {}
	GameCounts(const GameCounts &orig) : _wins(orig._wins), _losses(orig._losses), _ourTotalTime(orig._ourTotalTime), _theirTotalTime(orig._theirTotalTime) {}

	void add(const GameResult &gr) {
		// cout << "add to " << this << endl;
		if (gr._winnerWasContender) _wins++;
		else _losses++;
		_ourTotalTime += gr._times[0];
		_theirTotalTime += gr._times[1];
	}

	string getWinStr() {
		strstream ss;
		ss << _wins << "/" << _losses;
		return ss.str();
	}

	string getTimeStr() {
		strstream ss;
		if (_theirTotalTime <= 0) _theirTotalTime = 0.0001;
		double ratio = _ourTotalTime / _theirTotalTime;

		ss << std::fixed << std::setprecision(4) << ratio;
		return ss.str();
	}

	string getStr() {
		return getWinStr() + "    " + getTimeStr();
	}

//private:
	// By contender
	int _wins;
	int _losses;
	double _ourTotalTime;
	double _theirTotalTime;
};

#include <map>

class CategoryType
{
public:
	CategoryType(string name) : _catName(name) {}
	void addGameResult(const GameResult &gr);
	GameCounts &getCounts(const string &val);
	string getHeader();

	int getNumRows()
	{
		return 1 + _values.size();
	}

	string getRow(int rn)
	{
		if (rn == 0)
			return getHeader();
		if (rn > (int)_values.size())
			return "                 ";
		auto ki = _values.begin();
		rn--;
		while (rn > 0) {
			rn--;
			++ki;
		}
		string keyStr = ki->first + ":";
		strstream ss;
		ss << setw(4) << left << keyStr;
		ss << ki->second.getStr();
		return ss.str();
	}

private:
	string _catName;
	map<string, GameCounts> _values;
};

class AllStats
{
public:
	AllStats() : _categoryTypes {
		CategoryType("Player"),
		CategoryType("Size"),
		CategoryType("Depth"),
		CategoryType("Rules"),
		CategoryType("Overall") } {}

    CategoryType &getCategory(int catNum) { return _categoryTypes[catNum]; }
	void addGameResult(const GameResult &gr);
	string report();

	//: {Depth, Size, Player, Rules, Overall} - Name, CategoryValues
private:
	CategoryType _categoryTypes[5];
};

class Match
{
public:
	Match(PenteGame &p1, PenteGame &p2) : _players {&p1, &p2} {}

	void setDepthRange(Depth minDepth, Depth maxDepth) { _minDepth = minDepth; _maxDepth = maxDepth; }

	void setRulesTypes(const char *rulesTypes) { _rulesTypes = rulesTypes; }

	void setSizes() {}
	template<typename ... Types>
	void setSizes(BoardWidth s, Types ... rest) { _sizes.push_back(s); setSizes(rest...); }

	void play();
	AllStats &getAllStats() {return _allStats; }
	
private:
	PenteGame *_players[2];
	Depth _minDepth;
	Depth _maxDepth;
	string _rulesTypes;
	vector<BoardWidth> _sizes;

	AllStats _allStats;
};

#endif
