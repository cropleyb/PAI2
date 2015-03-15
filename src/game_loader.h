#ifndef _game_loader_h
#define _game_loader_h

#include <string>
using std::string;

class PenteGame;

class GameLoader
{
public:
	GameLoader() {}
    void loadStr(PenteGame &g, const string &gameStr);
};

#endif
