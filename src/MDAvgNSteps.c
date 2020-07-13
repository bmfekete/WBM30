/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDAvgNSteps.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

// Output
static int _MDOutAvgNStepsID = MFUnset;

static void _MDAvgNSteps (int itemID) {
	int nStep;
	
	nStep = MFVarGetInt (_MDOutAvgNStepsID, itemID, 0);
	MFVarSetInt (_MDOutAvgNStepsID, itemID, nStep + 1);
}

int MDAvgNStepsDef () {

	if (_MDOutAvgNStepsID != MFUnset) return (_MDOutAvgNStepsID);
	MFDefEntering ("Average NSteps");
	if (((_MDOutAvgNStepsID = MFVarGetID (MDVarAvgNSteps, MFNoUnit, MFInt,    MFState,  MFInitial)) == CMfailed) ||
	    (MFModelAddFunction(_MDAvgNSteps) == CMfailed)) return (CMfailed);
	MFDefLeaving ("Average NSteps");
	return (_MDOutAvgNStepsID);
}
