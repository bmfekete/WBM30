/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDDischMean.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

static int _MDInAux_AccumRunoffID = MFUnset;
static int _MDAux_InAvgNStepsID   = MFUnset;

static int _MDOutAux_MeanDischargeID     = MFUnset;

static void _MDAux_MeanDischarge (int itemID) {
	int   nSteps;
	float accumDisch;
	float dischMean;

	accumDisch = MFVarGetFloat (_MDInAux_AccumRunoffID,  itemID, 0.0);
	nSteps     = MFVarGetInt   (_MDAux_InAvgNStepsID,       itemID,   0);
	dischMean  = MFVarGetFloat (_MDOutAux_MeanDischargeID,      itemID, 0.0);
	dischMean  = (float) (((double) dischMean * (double) nSteps + accumDisch) / ((double) (nSteps + 1)));
	MFVarSetFloat (_MDOutAux_MeanDischargeID, itemID, dischMean);
}

enum { MDinput, MDcalculate };

int MDAux_MeanDiscargehDef () {
	int  optID = MFUnset;
	const char *optStr, *optName = MDVarAux_DischMean;
	const char *options [] = { MDInputStr, MDCalculateStr, (char *) NULL };

	if (_MDOutAux_MeanDischargeID != MFUnset) return (_MDOutAux_MeanDischargeID);
	MFDefEntering ("Discharge Mean");

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);

	switch (optID) {
		case MDinput: _MDOutAux_MeanDischargeID  = MFVarGetID (MDVarAux_DischMean, "m3/s", MFInput, MFState, MFBoundary); break;
		case MDcalculate:
			if (((_MDInAux_AccumRunoffID    = MDAux_AccumRunoffDef()) == CMfailed) ||
			    ((_MDAux_InAvgNStepsID      = MDAux_AvgNStepsDef())   == CMfailed) ||
                ((_MDOutAux_MeanDischargeID = MFVarGetID (MDVarAux_DischMean, "m3/s", MFOutput, MFState, MFInitial)) == CMfailed) ||
                (MFModelAddFunction(_MDAux_MeanDischarge) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Discharge Mean");
	return (_MDOutAux_MeanDischargeID);
}
