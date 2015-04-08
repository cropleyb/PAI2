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
	match.setDepthRange(8, 8);
	match.setRulesTypes("t");
	match.setSizes(19);
	match.play();
}

int main(int argc, char *argv[])
{
	runAI();
    return 0;
}

