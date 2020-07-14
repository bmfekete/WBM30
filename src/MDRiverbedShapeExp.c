/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDRiverbedShapeExp.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <math.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInDischMeanID              = MFUnset;
static int _MDInRiverbedSlopeID          = MFUnset;
// Output
static int _MDOutRiverbedAvgDepthMeanID  = MFUnset;
static int _MDOutRiverbedWidthMeanID     = MFUnset;
static int _MDOutRiverbedVelocityMeanID  = MFUnset;
static int _MDOutRiverbedShapeExponentID = MFUnset;

static void _MDRiverbedShapeExponent (int itemID) {
// Input
	float slope;     // Riverbed slope [m/km]
	float discharge; // Mean annual discharge [m3/s]
// Output
	float yMean;     // River average depth at mean discharge [m]
	float wMean;     // River width at mean discharge [m]
// Local
	float dL;        // Reach length [m]
//	float eta = 0.25, nu = 0.4,  tau = 8.0,  phi = 0.58;    //old
//	float eta = 0.36, nu = 0.37, tau = 3.55, phi = 0.51;	//new based on Knighton (avg)
	float eta = 0.33, nu = 0.35, tau = 3.67, phi = 0.45;	// Hey and Thorn (1986)

	if (MFVarTestMissingVal (_MDInDischMeanID, itemID)) {
		MFVarSetFloat (_MDOutRiverbedAvgDepthMeanID,  itemID, 0.0);
		MFVarSetFloat (_MDOutRiverbedWidthMeanID,     itemID, 0.0);
		MFVarSetFloat (_MDOutRiverbedVelocityMeanID,  itemID, 0.0);
		MFVarSetFloat (_MDOutRiverbedShapeExponentID, itemID, 2.0);
		return;
	}
	discharge = fabs(MFVarGetFloat(_MDInDischMeanID,  itemID, 0.0));
	dL        = MFModelGetLength (itemID);
	if (CMmathEqualValues (dL, 0.0) || (_MDInRiverbedSlopeID == MFUnset) || MFVarTestMissingVal (_MDInRiverbedSlopeID, itemID)) {
		// Slope independent riverbed geometry
		yMean = eta * pow (discharge, nu);
		wMean = tau * pow (discharge, phi);
		MFVarSetFloat (_MDOutRiverbedAvgDepthMeanID,  itemID, yMean);
		MFVarSetFloat (_MDOutRiverbedWidthMeanID,     itemID, wMean);
		MFVarSetFloat (_MDOutRiverbedVelocityMeanID,  itemID, discharge / (yMean * wMean));
		MFVarSetFloat (_MDOutRiverbedShapeExponentID, itemID, 2.0);
		return;	
	}
	// Slope dependent riverbed geometry
	slope     = MFVarGetFloat(_MDInRiverbedSlopeID,      itemID, 0.01) / 1000.0;

	yMean = eta * pow (discharge, nu);
	wMean = tau * pow (discharge, phi);
	MFVarSetFloat (_MDOutRiverbedAvgDepthMeanID,      itemID, yMean);
	MFVarSetFloat (_MDOutRiverbedWidthMeanID,         itemID, wMean);
	MFVarSetFloat (_MDOutRiverbedVelocityMeanID,      itemID, discharge / (yMean * wMean));
	MFVarSetFloat (_MDOutRiverbedShapeExponentID,     itemID, 2.0);
}

enum { MDinput, MDindependent, MDdependent };

int MDRiverbedShapeExponentDef () {
	int  optID = MFUnset;
	char *optStr, *optName = MDOptRiverbed;
	char *options [] = { MDInputStr, "slope-independent", "slope-dependent", (char *) NULL };

	if (_MDOutRiverbedShapeExponentID != MFUnset) return (_MDOutRiverbedShapeExponentID);

	MFDefEntering ("Riverbed Geometry");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDinput:
			if (((_MDOutRiverbedAvgDepthMeanID  = MFVarGetID (MDVarRiverbedAvgDepthMean,  "m",      MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutRiverbedWidthMeanID     = MFVarGetID (MDVarRiverbedWidthMean,     "m",      MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutRiverbedVelocityMeanID  = MFVarGetID (MDVarRiverbedVelocityMean,  "m/s",    MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutRiverbedShapeExponentID = MFVarGetID (MDVarRiverbedShapeExponent, MFNoUnit, MFInput,  MFState, MFBoundary)) == CMfailed))
				return (CMfailed);
			break;
		case MDdependent:
			if ((_MDInRiverbedSlopeID           = MFVarGetID (MDVarRiverbedSlope,         "m/km",   MFInput,  MFState, MFBoundary)) == CMfailed)
				return (CMfailed);
		case MDindependent:
			if (((_MDInDischMeanID = MDDischMeanDef ()) == CMfailed) ||
			    ((_MDOutRiverbedAvgDepthMeanID  = MFVarGetID (MDVarRiverbedAvgDepthMean,  "m",      MFOutput, MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutRiverbedWidthMeanID     = MFVarGetID (MDVarRiverbedWidthMean,     "m",      MFOutput, MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutRiverbedVelocityMeanID  = MFVarGetID (MDVarRiverbedVelocityMean,  "m/s",    MFOutput, MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutRiverbedShapeExponentID = MFVarGetID (MDVarRiverbedShapeExponent, MFNoUnit, MFOutput, MFState, MFBoundary)) == CMfailed) ||
			    (MFModelAddFunction (_MDRiverbedShapeExponent) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Riverbed Geometry");
	return (_MDOutRiverbedShapeExponentID);
}
