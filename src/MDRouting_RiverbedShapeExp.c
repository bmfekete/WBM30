/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

MDRiverbedShapeExp.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <math.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInAux_MeanDischargeID              = MFUnset;
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

	if (MFVarTestMissingVal (_MDInAux_MeanDischargeID, itemID)) {
		MFVarSetFloat (_MDOutRiverbedAvgDepthMeanID,  itemID, 0.0);
		MFVarSetFloat (_MDOutRiverbedWidthMeanID,     itemID, 0.0);
		MFVarSetFloat (_MDOutRiverbedVelocityMeanID,  itemID, 0.0);
		MFVarSetFloat (_MDOutRiverbedShapeExponentID, itemID, 2.0);
		return;
	}
	discharge = fabs(MFVarGetFloat(_MDInAux_MeanDischargeID,  itemID, 0.0));
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

int MDRouting_RiverbedShapeExponentDef () {
	int  optID = MFUnset;
	const char *optStr, *optName = MDOptRouting_Riverbed;
	const char *options [] = { MDInputStr, "slope-independent", "slope-dependent", (char *) NULL };

	if (_MDOutRiverbedShapeExponentID != MFUnset) return (_MDOutRiverbedShapeExponentID);

	MFDefEntering ("Riverbed Shape Exponent");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDinput:
			if (((_MDOutRiverbedAvgDepthMeanID  = MFVarGetID (MDVarRouting_RiverbedAvgDepthMean, "m", MFInput, MFState, MFBoundary)) == CMfailed) ||
                ((_MDOutRiverbedWidthMeanID     = MFVarGetID (MDVarRouting_RiverbedWidthMean, "m", MFInput, MFState, MFBoundary)) == CMfailed) ||
                ((_MDOutRiverbedVelocityMeanID  = MFVarGetID (MDVarRouting_RiverbedVelocityMean, "m/s", MFInput, MFState, MFBoundary)) == CMfailed) ||
                ((_MDOutRiverbedShapeExponentID = MFVarGetID (MDVarRouting_RiverbedShapeExponent, MFNoUnit, MFInput, MFState, MFBoundary)) == CMfailed))
				return (CMfailed);
			break;
		case MDdependent:
			if ((_MDInRiverbedSlopeID           = MFVarGetID (MDVarRouting_RiverbedSlope, "m/km", MFInput, MFState, MFBoundary)) == CMfailed)
				return (CMfailed);
		case MDindependent:
			if (((_MDInAux_MeanDischargeID = MDAux_MeanDiscargehDef()) == CMfailed) ||
                ((_MDOutRiverbedAvgDepthMeanID  = MFVarGetID (MDVarRouting_RiverbedAvgDepthMean, "m", MFOutput, MFState, MFBoundary)) == CMfailed) ||
                ((_MDOutRiverbedWidthMeanID     = MFVarGetID (MDVarRouting_RiverbedWidthMean, "m", MFOutput, MFState, MFBoundary)) == CMfailed) ||
                ((_MDOutRiverbedVelocityMeanID  = MFVarGetID (MDVarRouting_RiverbedVelocityMean, "m/s", MFOutput, MFState, MFBoundary)) == CMfailed) ||
                ((_MDOutRiverbedShapeExponentID = MFVarGetID (MDVarRouting_RiverbedShapeExponent, MFNoUnit, MFOutput, MFState, MFBoundary)) == CMfailed) ||
                (MFModelAddFunction (_MDRiverbedShapeExponent) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Riverbed Shape Exponent");
	return (_MDOutRiverbedShapeExponentID);
}
