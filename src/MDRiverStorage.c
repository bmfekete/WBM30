/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

MDDischarge.c

balazs.fekete@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInDischargeID            = MFUnset;
static int _MDInRiverbedShapeExpID     = MFUnset;
static int _MDInRiverbedAvgDepthMeanID = MFUnset;
static int _MDInRiverbedWidthMeanID    = MFUnset;
// Output
static int _MDOutRiverStorChgID        = MFUnset;
static int _MDOutRiverStorageID        = MFUnset;
static int _MDOutRiverWidthID          = MFUnset;
static int _MDOutRiverDepthID          = MFUnset;

static void _MDRiverDepth (int itemID) {

}

int MDRiverDepthDef() {

	MFDefEntering ("River Storage");
	if (((_MDInDischargeID            = MDDischargeDef ())     == CMfailed) ||
	    ((_MDInRiverbedShapeExpID     = MDRiverbedShapeExp ()) == CMfailed) ||
		((_MDInRiverbedAvgDepthMeanID = MFVarGetID (MDVarRiverbedAvgDepthMean,  "m",  MFInput,  MFState, MFBoundary)) == CMfailed) ||
		((_MDInRiverbedWidthMeanID    = MFVarGetID (MDVarRiverbedWidthMean,     "m",  MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutRiverStorChgID        = MFVarGetID (MDVarRiverStorageChg,       "m3", MDOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDOutRiverStorageID        = MFVarGetID (MDVarRiverStorage,          "m3", MDOutput, MFState, MFIntial))   == CMfailed) ||
	    ((_MDOutRiverWidthID          = MFVarGetID (MDVarRiverWidth,            "m",  MDOutput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutRiverDepthID          = MFVarGetID (MDVarRiverDepth,            "m",  MDOutput, MFState, MFBoundary)) == CMfailed))
	return (CMfailed);

	MFModelAddFunction (_MDDischLevel1) == CMfailed)) return (CMfailed);
	MFDefLeaving ("River Storage");
	return (_MDOutDischLevel1ID);
}
