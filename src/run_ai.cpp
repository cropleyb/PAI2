#include "pente_game.h"
#include "loc.h"
#include "span_lookup_table.h"
#include "alpha_beta.h"
#include "run_ai_support.h"

#include <iostream>

// RunAI program - for setting up battles between tweaked versions of the AI.

void runAI()
{
	PenteGame p1;
	PenteGame p2;
	Match match(p1, p2);
	match.setDepthRange(1, 2);
	match.setRulesTypes("st");
	match.setSizes(13, 19);
	match.play();
	AllStats &allStats = match.getAllStats();
	string wholeReport = allStats.report();
	std::cout << wholeReport << endl;
}

int main(int argc, char *argv[])
{
	runAI();
    return 0;
}

