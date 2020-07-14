/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDDichRouteMuskCoeff.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

// Input
static int _MDInRiverbedAvgDepthMeanID  = MFUnset;
static int _MDInRiverbedWidthMeanID     = MFUnset;
static int _MDInRiverbedVelocityMeanID  = MFUnset;
static int _MDInRiverbedShapeExponentID = MFUnset;
static int _MDInRiverbedSlopeID         = MFUnset;
// Output
static int _MDOutMuskingumC0ID          = MFUnset;
static int _MDOutMuskingumC1ID          = MFUnset;
static int _MDOutMuskingumC2ID          = MFUnset;
static int _MDOutCourantID              = MFUnset;

static void _MDDischRouteMuskingumCoeff (int itemID) {
// Input
	float yMean;            // Average depth at mean discharge [m]
	float wMean;            // River width at mean discharge [m]
	float vMean;            // Mean velocity
	float beta;             // Riverbed shape exponent []
	float slope;            // Riverbed slope [m/km]
// Output
	float C0;               // Muskingum C0 coefficient (current inflow)
	float C1;               // Muskingum C1 coefficient (previous inflow)
	float C2;               // MUskingum C2 coefficient (previous outflow)
// Local
	float xi;               // Flood-wave/flow velocity ratio
	float C;                // Cell Courant number;
	float D;                // Cell Reynolds number;
	float dt;               // time step length [s]
	float dL;               // Cell length [m]
	
	dL        = MFModelGetLength (itemID);
	slope     = MFVarGetFloat (_MDInRiverbedSlopeID,         itemID, 0.0) / 1000.0;
	yMean     = MFVarGetFloat (_MDInRiverbedAvgDepthMeanID,  itemID, 0.0);
	wMean     = MFVarGetFloat (_MDInRiverbedWidthMeanID,     itemID, 0.0);
	vMean     = MFVarGetFloat (_MDInRiverbedVelocityMeanID,  itemID, 0.0);
	beta      = MFVarGetFloat (_MDInRiverbedShapeExponentID, itemID, 0.0);
	if (CMmathEqualValues (vMean,     0.0)) {
		MFVarSetFloat (_MDOutMuskingumC0ID, itemID, 0.0);
		MFVarSetFloat (_MDOutMuskingumC1ID, itemID, 0.0);
		MFVarSetFloat (_MDOutMuskingumC2ID, itemID, 0.0);
		MFVarSetFloat (_MDOutCourantID,     itemID, 0.0);
		return;
	}
	if (CMmathEqualValues (dL,        0.0) ||
	    CMmathEqualValues (slope,     0.0) ||
	    CMmathEqualValues (yMean,     0.0) ||
	    CMmathEqualValues (wMean,     0.0) ||
	    (beta  < 0.0)) { 
	    // Falling back to flow-accumulation
		MFVarSetFloat (_MDOutMuskingumC0ID, itemID, 1.0);
		MFVarSetFloat (_MDOutMuskingumC1ID, itemID, 0.0);
		MFVarSetFloat (_MDOutMuskingumC2ID, itemID, 0.0);
		MFVarSetFloat (_MDOutCourantID,     itemID, 0.0);
		return;
	}
	dt = MFModelGet_dt (); 

	xi = 1 + beta * (2.0 / 3.0) / (beta + 1);
	C = xi * vMean * dt / dL;
	D = yMean / (dL * slope * xi);

	C0 = (-1 + C + D) / (1 + C + D);
	C1 = ( 1 + C - D) / (1 + C + D);
	C2 = ( 1 - C + D) / (1 + C + D);
//	if ((C0 < 0.0) || (C1 < 0.0) || (C2 < 0.0)) { C0 = 1.0; C1 = 0; C2 = 0; } // According to Pounce C1 and C2 can be negative

	MFVarSetFloat (_MDOutMuskingumC0ID, itemID, C0);
	MFVarSetFloat (_MDOutMuskingumC1ID, itemID, C1);
	MFVarSetFloat (_MDOutMuskingumC2ID, itemID, C2);
	MFVarSetFloat (_MDOutCourantID,     itemID, C);
}

enum { MDinput, MDstatic };

int MDDischLevel3MuskingumCoeffDef () {
	int  optID = MFUnset;
	const char *optStr, *optName = MDOptMuskingum;
	const char *options [] = { MDInputStr, "static", (char *) NULL };

	if (_MDOutMuskingumC0ID != MFUnset) return (_MDOutMuskingumC0ID);

	MFDefEntering ("Muskingum Coefficients");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
	switch (optID) {
		case MDinput:
			if (((_MDOutMuskingumC0ID = MFVarGetID (MDVarMuskingumC0, MFNoUnit,   MFInput,  MFState,  MFBoundary)) == CMfailed) ||
			    ((_MDOutMuskingumC1ID = MFVarGetID (MDVarMuskingumC1, MFNoUnit,   MFInput,  MFState,  MFBoundary)) == CMfailed) ||
			    ((_MDOutMuskingumC2ID = MFVarGetID (MDVarMuskingumC2, MFNoUnit,   MFInput,  MFState,  MFBoundary)) == CMfailed))
				return (CMfailed);
			break;
		case MDstatic:
			if (((_MDInRiverbedShapeExponentID  = MDRiverbedShapeExponentDef ()) == CMfailed) ||
			    ((_MDInRiverbedWidthMeanID      = MFVarGetID (MDVarRiverbedWidthMean,    "m",      MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInRiverbedAvgDepthMeanID   = MFVarGetID (MDVarRiverbedAvgDepthMean, "m",      MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInRiverbedVelocityMeanID   = MFVarGetID (MDVarRiverbedVelocityMean, "m/s",    MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInRiverbedSlopeID          = MFVarGetID (MDVarRiverbedSlope,        "m/km",   MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutMuskingumC0ID           = MFVarGetID (MDVarMuskingumC0,          MFNoUnit, MFOutput, MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutMuskingumC1ID           = MFVarGetID (MDVarMuskingumC1,          MFNoUnit, MFOutput, MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutMuskingumC2ID           = MFVarGetID (MDVarMuskingumC2,          MFNoUnit, MFOutput, MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutCourantID               = MFVarGetID ("Courant",                 MFNoUnit, MFOutput, MFState, MFBoundary)) == CMfailed) ||
			    (MFModelAddFunction (_MDDischRouteMuskingumCoeff) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Muskingum Coefficients");
	return (_MDOutMuskingumC0ID);
}
