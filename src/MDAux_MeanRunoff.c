/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

MDAyx_MeanDischarge.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

static int _MDInCore_RunoffID = MFUnset;
static int _MDAux_InAvgNStepsID      = MFUnset;

static int _MDOutAux_MeanDischargeID     = MFUnset;

static void _MDAux_MeanRunoff (int itemID) {
	int   nSteps;
	float runoff;
	float runoffMean;

	runoff      = MFVarGetFloat (_MDInCore_RunoffID,        itemID, 0.0);
	nSteps      = MFVarGetInt   (_MDAux_InAvgNStepsID,      itemID, 0);
	runoffMean  = MFVarGetFloat (_MDOutAux_MeanDischargeID, itemID, 0.0);
	runoffMean  = (float) (((double) runoffMean * (double) nSteps + runoff) / ((double) (nSteps + 1)));
	MFVarSetFloat (_MDOutAux_MeanDischargeID, itemID, runoffMean);
}

enum { MDinput, MDcalculate };

int MDAux_MeanRunoffDef () {
	int  optID = MFUnset;
	const char *optStr, *optName = MDVarCore_RunoffMean;
	const char *options [] = { MDInputStr, MDCalculateStr, (char *) NULL };

	if (_MDOutAux_MeanDischargeID != MFUnset) return (_MDOutAux_MeanDischargeID);
	MFDefEntering ("Runoff Mean");

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);

	switch (optID) {
		case MDinput: _MDOutAux_MeanDischargeID  = MFVarGetID (MDVarCore_RunoffMean, "mm/d", MFInput, MFState, MFBoundary); break;
		case MDcalculate:
			if (((_MDAux_InAvgNStepsID      = MDAux_AvgNStepsDef()) == CMfailed) ||
                ((_MDInCore_RunoffID        = MDCore_RunoffDef())   == CMfailed) ||
                ((_MDOutAux_MeanDischargeID = MFVarGetID (MDVarCore_RunoffMean, "mm/d", MFOutput, MFState, MFInitial)) == CMfailed) ||
                (MFModelAddFunction(_MDAux_MeanRunoff) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Runoff Mean");
	return (_MDOutAux_MeanDischargeID);
}
