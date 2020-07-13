/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDDischMean.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

static int _MDInAccumDischargeID = MFUnset;
static int _MDInAvgNStepsID      = MFUnset;

static int _MDOutDischMeanID     = MFUnset;

static void _MDDischMean (int itemID) {
	int   nSteps;
	float accumDisch;
	float dischMean;

	accumDisch = MFVarGetFloat (_MDInAccumDischargeID,  itemID, 0.0);
	nSteps     = MFVarGetInt   (_MDInAvgNStepsID,       itemID,   0);
	dischMean  = MFVarGetFloat (_MDOutDischMeanID,      itemID, 0.0);
	dischMean  = (float) (((double) dischMean * (double) nSteps + accumDisch) / ((double) (nSteps + 1)));
	MFVarSetFloat (_MDOutDischMeanID, itemID, dischMean);
}

enum { MDinput, MDcalculate };

int MDDischMeanDef () {
	int  optID = MFUnset;
	const char *optStr, *optName = MDVarDischMean;
	const char *options [] = { MDInputStr, MDCalculateStr, (char *) NULL };

	if (_MDOutDischMeanID != MFUnset) return (_MDOutDischMeanID);
	MFDefEntering ("Discharge Mean");

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);

	switch (optID) {
		case MDinput: _MDOutDischMeanID  = MFVarGetID (MDVarDischMean, "m3/s", MFInput,  MFState, MFBoundary); break;
		case MDcalculate:
			if (((_MDInAvgNStepsID       = MDAvgNStepsDef ())   == CMfailed) ||
			    ((_MDInAccumDischargeID  = MDAccumRunoffDef ()) == CMfailed) ||
			    ((_MDOutDischMeanID      = MFVarGetID (MDVarDischMean, "m3/s", MFOutput, MFState, MFInitial))  == CMfailed) ||
			    (MFModelAddFunction (_MDDischMean) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Discharge Mean");
	return (_MDOutDischMeanID);
}
