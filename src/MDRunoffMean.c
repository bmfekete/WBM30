/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDDischMean.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

static int _MDInRunoffID = MFUnset;
static int _MDInAvgNStepsID      = MFUnset;

static int _MDOutRunoffMeanID     = MFUnset;

static void _MDRunoffMean (int itemID) {
	int   nSteps;
	float runoff;
	float runoffMean;

	runoff = MFVarGetFloat (_MDInRunoffID,  itemID, 0.0);
	nSteps     = MFVarGetInt   (_MDInAvgNStepsID,       itemID,   0);
	runoffMean  = MFVarGetFloat (_MDOutRunoffMeanID,      itemID, 0.0);
	runoffMean  = (float) (((double) runoffMean * (double) nSteps + runoff) / ((double) (nSteps + 1)));
	MFVarSetFloat (_MDOutRunoffMeanID, itemID, runoffMean);
}

enum { MDinput, MDcalculate };

int MDRunoffMeanDef () {
	int  optID = MFUnset;
	char *optStr, *optName = MDVarRunoffMean;
	char *options [] = { MDInputStr, MDCalculateStr, (char *) NULL };

	if (_MDOutRunoffMeanID != MFUnset) return (_MDOutRunoffMeanID);
	MFDefEntering ("Runoff Mean");

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);

	switch (optID) {
		case MDinput: _MDOutRunoffMeanID  = MFVarGetID (MDVarRunoffMean, "mm/d", MFInput,  MFState, MFBoundary); break;
		case MDcalculate:
			if (((_MDInAvgNStepsID       = MDAvgNStepsDef ())   == CMfailed) ||
			    ((_MDInRunoffID  = MDRunoffDef ()) == CMfailed) ||
			    ((_MDOutRunoffMeanID      = MFVarGetID (MDVarRunoffMean, "mm/d", MFOutput, MFState, MFInitial))  == CMfailed) ||
			    (MFModelAddFunction (_MDRunoffMean) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Runoff Mean");
	return (_MDOutRunoffMeanID);
}
