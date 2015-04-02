#ifndef _run_ai_support_h
#define _run_ai_support_h

#include <iostream>

using namespace std;

class PenteGame;

class GameResult
{
public:
	GameResult() : _times { 0 }, _winner(EMPTY) {}

	Colour getWinner() { return _winner; }
	bool wasWonBy(Colour p) { return _winner == p; }
	double getTime(Colour p) { return _times[p]; }

	Colour _winner;
	double _times[3];

	string _depth;
	string _size;
	string _rules;
};

class RunAIGame
{
public:
	RunAIGame(PenteGame &p1, PenteGame &p2) : _players {&p1, &p2} {}

	GameResult play(Depth depth, BoardWidth size, RulesType rules);
	
private:
	PenteGame *_players[2];
};

class CategoryType
{
public:
	CategoryType(string name) : _catName(name) {}
	string _catName;
};

class AllStats
{
public:
	AllStats() : _categoryTypes {
		CategoryType("Depth"),
		CategoryType("Size"),
		CategoryType("Player"),
		CategoryType("Rules"),
		CategoryType("Overall") } {}

    const CategoryType &getCategory(int catNum) const {return _categoryTypes[catNum]; }
	void addGameResult(const GameResult &gr);
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
	const AllStats &getAllStats() const {return _allStats; }
	
private:
	PenteGame *_players[2];
	Depth _minDepth;
	Depth _maxDepth;
	string _rulesTypes;
	vector<BoardWidth> _sizes;

	AllStats _allStats;
};

#endif
