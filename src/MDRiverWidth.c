/******************************************************************************

GHAAS Water Balance/Transport Model V2.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDRiverWidth.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <math.h>
#include <MF.h>
#include <MD.h>

// Inputs
static int _MDInDischargeID             = MFUnset;
static int _MDInRiverbedShapeExponentID = MFUnset;
static int _MDInRiverbedAvgDepthMeanID  = MFUnset;
static int _MDInRiverbedWidthMeanID     = MFUnset;
static int _MDInRiverbedVelocityMeanID  = MFUnset;

// Outputs
static int _MDOutRiverDepthID           = MFUnset;
static int _MDOutRiverWidthID           = MFUnset;

static void _MDRiverWidth (int itemID) {
// Input
	float discharge; // Discharge [m3/s]
	float shapeExp;  // Riverbed shape exponent
	float avgDepth;  // Average depth at mean discharge [m]
	float avgWidth;  // Average width at mean discharge [m]
	float velocity;  // Flow velocity [m/s]
// Output
	float depth;     // Flow depth at current discharge [m]
	float width;     // Flow width at current discharge [m]
// Local
	float alpha;     // Shape coefficient
	float area;      // Cross-section area [m2]

	discharge = MFVarGetFloat (_MDInDischargeID,              itemID, 0.0);
	shapeExp  = MFVarGetFloat (_MDInRiverbedShapeExponentID,  itemID, 0.0);
	avgDepth  = MFVarGetFloat (_MDInRiverbedAvgDepthMeanID,   itemID, 0.0);
	avgWidth  = MFVarGetFloat (_MDInRiverbedWidthMeanID,      itemID, 0.0);
	velocity  = MFVarGetFloat (_MDInRiverbedVelocityMeanID,   itemID, 0.0);

	if (CMmathEqualValues (discharge, 0.0) ||
	    CMmathEqualValues (avgDepth,  0.0) ||
	    CMmathEqualValues (avgWidth,  0.0) ||
	    CMmathEqualValues (velocity,  0.0)) {
		width = depth = 0.0;
	}
	else	{
		alpha = avgDepth / pow (avgWidth, shapeExp);

		area  = discharge / velocity;
		width = pow (((shapeExp + 1.0) * area) / (shapeExp * alpha), 1.0 / (shapeExp + 1));
		depth = alpha * pow (width, shapeExp);
	}
	MFVarSetFloat (_MDOutRiverDepthID,   itemID, depth);
	MFVarSetFloat (_MDOutRiverWidthID,   itemID, width);
}

int MDRiverWidthDef () {

	if (_MDOutRiverWidthID != MFUnset) return (_MDOutRiverWidthID);

	MFDefEntering ("River Geometry");

	if (((_MDInDischargeID             = MDDischargeDef ())             == CMfailed) ||
	    ((_MDInRiverbedShapeExponentID = MDRiverbedShapeExponentDef ()) == CMfailed) ||
	    ((_MDInRiverbedAvgDepthMeanID  = MFVarGetID (MDVarRiverbedAvgDepthMean,  "m",      MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInRiverbedWidthMeanID     = MFVarGetID (MDVarRiverbedWidthMean,     "m",      MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInRiverbedVelocityMeanID  = MFVarGetID (MDVarRiverbedVelocityMean,  "m/s",    MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutRiverDepthID           = MFVarGetID (MDVarRiverDepth,            "m",      MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutRiverWidthID           = MFVarGetID (MDVarRiverWidth,            "m",      MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    (MFModelAddFunction (_MDRiverWidth) == CMfailed)) return (CMfailed);
	MFDefLeaving ("River Geometry");

	return (_MDOutRiverWidthID);
}
