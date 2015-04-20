#define private public

#include "pente_game.h"
#include "loc.h"
#include "span_lookup_table.h"
#include "alpha_beta.h"
#include "run_ai_support.h"

#include <iostream>

// RunAI program - for setting up battles between tweaked versions of the AI.

void runAI()
{
	PenteGame contender;
	PenteGame defender;
	Match match(contender, defender);
    //defender._moveSuggester._shallowCutoff = 2;
    //contender._utilCalc._blockedFourBase = 700;
    //contender._utilCalc._blockedFourBase = 200;
    //contender._utilCalc._checkerboardValue = 800;
    //contender._utilCalc._lengthFactor = 45;
    //contender._utilCalc._captureScoreBase = 200;
    //contender._moveSuggester._shallowCutoff = 3;
    //contender._moveSuggester._shallowCutoff = 5;
    //contender._moveSuggester._maxMovesShallow = 7;
    //contender._moveSuggester._maxMovesDeep = 5;
    //cout << "Contender util calc modified: " << &(contender._utilCalc) << endl;
    match.setSilent(true);
    match.setShowReport(true);
	match.setDepthRange(1, 6);
	match.setRulesTypes("st");
	match.setSizes(19);
	match.play();
}

int main(int argc, char *argv[])
{
	runAI();
    return 0;
}

