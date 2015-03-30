#ifndef _game_loader_h
#define _game_loader_h

class PenteGame;
#include "loc.h"

extern const char *getMoveFromStrWithGame(const char *gameChars, PenteGame *game);
extern const char *getMoveFromStr(const char *gameChars);
extern Loc doTheSearch(const std::string &gameStr, PenteGame *game);

#endif
