#include "game_loader.h"
#include "pente_game.h"
#include "loc.h"
#include "span_lookup_table.h"
#include "alpha_beta.h"

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <cstddef>

using namespace std;

vector<string> &split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
}

void loadGameStr(PenteGame &g, const string &gameStr)
{
	cout << " !!!!!!!!!!!------------ Received: " << gameStr << endl;
	vector<string> lines;
    split(gameStr, '\n', lines);

	Colour currColour = P1;

	for (string line : lines)
	{
		// e.g.
		// 1. (5,3)
		string::size_type colonInd = line.find(':');
		if (colonInd != std::string::npos)
		{
			string key(line.substr(0, colonInd));
			string val(line.substr(colonInd+1));
			int valI = atoi(val.c_str());
			cout << " Key: " << key << " Val: " << val << endl;
			if (key == "depth") g.setMaxDepth(valI);
			else if (key == "boardsize") g.setBoardSize(valI);
#if 0
			else if (key == "rules") setRules(val);
"depth:4\n"
"boardsize:19\n"
"rules:tournament\n"
#endif
			continue;
		}

		string::size_type openParenInd = line.find("(");
		string::size_type commaInd = line.find(",");
		string::size_type closeParenInd = line.find(")");
		string coord1str(line.substr(openParenInd+1, commaInd-openParenInd-1));
		string coord2str(line.substr(commaInd+1, closeParenInd-commaInd-1));
		 
		int coord1 = atoi(coord1str.c_str());
		int coord2 = atoi(coord2str.c_str());
		g.makeMove(Loc(coord1,coord2), currColour);
		cout << " P" << (int)currColour;
		currColour = otherPlayer(currColour);
	}
    g.setColour(currColour);
}

Loc doTheSearch(const string &gameStr, PenteGame *game)
{
	// buildSpanTable(19);

	bool allocated = false;
	if (game == 0) 
	{
		game = new PenteGame();
		allocated = true;
	}
	loadGameStr(*game, gameStr);

	AlphaBeta ab(*game);
	Loc bestMove = ab.getBestMove();
	if (allocated)
	{
		delete game;
	}
	return bestMove;
}

const char *getMoveFromStr(const char *gameChars, PenteGame *game)
{
	string gameStr(gameChars);
	Loc move = doTheSearch(gameStr, game);

	static char moveBuf[100];
	sprintf(moveBuf, "%d,%d\n", move[0], move[1]);
	return moveBuf;
}
