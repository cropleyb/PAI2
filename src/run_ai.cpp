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
	// Match match(contender, defender);
	Match match(defender, contender);
	defender.setPAI1();
	//contender.setPAI1();
	//defender.setPartPAI2();
	contender.setPartPAI2();
	//contender._utilCalc._multiFactor = .1;
	//contender._utilCalc._multiFactor = 2.2;
	
	//defender.setVCTDepth(3);
	//contender.setVCTDepth(3);
    //defender._moveSuggester._shallowCutoff = 2;
	//defender._utilCalc._moveFactor = 45.0;
	//contender._utilCalc._moveFactor = 50.0;
    //contender._utilCalc._blockedFourBase = 100;
    //contender._utilCalc._blockedFourBase = 0;
    //contender._utilCalc._checkerboardValue = 500;
    //contender._utilCalc._stripeValue = 600;
    //contender._utilCalc._stripeValue = -200;
    //contender._utilCalc._spreadValue = 100;
    //contender._utilCalc._stripeValue = 0;
    //contender._utilCalc._lengthFactor = 0;
    // contender._utilCalc._captureScoreBase = 200;
    //contender._moveSuggester._shallowCutoff = 3;
    //contender._moveSuggester._shallowCutoff = 5;
    //contender._moveSuggester._maxMovesShallow = 8;
    //contender._moveSuggester._maxMovesDeep = 5;
    //cout << "Contender util calc modified: " << &(contender._utilCalc) << endl;
    match.setSilent(true);
    match.setShowReport(true);
	match.setDepthRange(1, 7);
	//match.setDepthRange(3, 4);
	match.setRulesTypes("st");
	//match.setRulesTypes("r");
	//match.setRulesTypes("t");
	//match.setSizes(13,19);
	match.setSizes(13,19);
	match.play();
}

int main(int argc, char *argv[])
{
	runAI();
    return 0;
}

