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

	GameResult play();
	
private:
	PenteGame *_players[2];
};

#endif