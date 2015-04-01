#ifndef _run_ai_support_h
#define _run_ai_support_h

class PenteGame;

class GameResult
{
public:
	GameResult() {}
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
