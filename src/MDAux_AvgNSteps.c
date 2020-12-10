/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDAux_AvgNSteps.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

// Output
static int _MDOutAux_AvgNStepsID = MFUnset;

static void _MDAux_AvgNSteps (int itemID) {
	int nStep;
	
	nStep = MFVarGetInt (_MDOutAux_AvgNStepsID, itemID, 0);
	MFVarSetInt (_MDOutAux_AvgNStepsID, itemID, nStep + 1);
}

int MDAux_AvgNStepsDef () {

	if (_MDOutAux_AvgNStepsID != MFUnset) return (_MDOutAux_AvgNStepsID);
	MFDefEntering ("Average NSteps");
	if (((_MDOutAux_AvgNStepsID = MFVarGetID (MDVarAux_AvgNSteps, MFNoUnit, MFInt, MFState, MFInitial)) == CMfailed) ||
        (MFModelAddFunction(_MDAux_AvgNSteps) == CMfailed)) return (CMfailed);
	MFDefLeaving ("Average NSteps");
	return (_MDOutAux_AvgNStepsID);
}
