#include "game_loader.h"
#include "pente_game.h"

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <cstddef>


std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

void GameLoader::loadStr(PenteGame &g, const string &gameStr)
{
	std::vector<std::string> lines;
    split(gameStr, '\n', lines);

	Colour currColour = P1;

	for (std::string line : lines)
	{
		// e.g.
		// 1. (5,3)
		string::size_type openParenInd = line.find("(");
		string::size_type commaInd = line.find(",");
		string::size_type closeParenInd = line.find(")");
		string coord1str(line.substr(openParenInd+1, commaInd-openParenInd-1));
		string coord2str(line.substr(commaInd+1, closeParenInd-commaInd-1));
		 
		int coord1 = std::atoi(coord1str.c_str());
		int coord2 = std::atoi(coord2str.c_str());
		g.makeMove(Loc(coord1,coord2), currColour);
		currColour = otherPlayer(currColour);
	}
}

