/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2014, UNH - CCNY/CUNY

MDSoilAvailWaterCap.c

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <stdio.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInSoilFieldCapacityID  = MFUnset;
static int _MDInSoilWiltingPointID   = MFUnset;
static int _MDInSoilRootingDepthID   = MFUnset;
static int _MDInImpFractionID	     = MFUnset; 		// RJS 091213
static int _MDInH2OFractionID        = MFUnset;			// RJS 091213
static int _MDInImperviousSoilID     = MFUnset;			// RJS 091213
static int _MDInSoilAvailWaterCapID  = MFUnset;			// RJS 091213
static int _MDInLawnFractionID       = MFUnset;
static int _MDInLawnAWCID            = MFUnset;

// Output
static int _MDOutSoilAvailWaterCapID = MFUnset;

static void _MDSoilAvailWaterCap (int itemID) {
	float fieldCapacity; // Field capacity [m/m]
	float wiltingPoint;  // Wilting point  [m/m]
	float rootingDepth;  // Rooting depth  [mm]

	fieldCapacity = MFVarGetFloat (_MDInSoilFieldCapacityID, itemID, 0.0);
	wiltingPoint  = MFVarGetFloat (_MDInSoilWiltingPointID,  itemID, 0.0);
	rootingDepth  = MFVarGetFloat (_MDInSoilRootingDepthID,  itemID, 0.0);
	if (fieldCapacity < wiltingPoint) fieldCapacity = wiltingPoint;
	
	MFVarSetFloat (_MDOutSoilAvailWaterCapID, itemID, rootingDepth * (fieldCapacity - wiltingPoint));
}


static void _MDSoilAvailWaterCapInput (int itemID) {
	float imperviousSoil;	// 1.0 = soil AWC input layer already incorporates impervious surfaces
	float impAreaFrac;
	float h2oAreaFrac;
	float inputAWC;
	float outputAWC;
        float lawnAreaFrac;
        float lawnAWC;

	imperviousSoil = MFVarGetFloat (_MDInImperviousSoilID,    itemID, 0.0);
        impAreaFrac    = MFVarGetFloat (_MDInImpFractionID,       itemID, 0.0);
        h2oAreaFrac    = MFVarGetFloat (_MDInH2OFractionID,       itemID, 0.0);
        inputAWC       = MFVarGetFloat (_MDInSoilAvailWaterCapID, itemID, 0.0);
        lawnAreaFrac  = MFVarGetFloat (_MDInLawnFractionID,      itemID, 0.0);
        lawnAWC       = MFVarGetFloat (_MDInLawnAWCID,           itemID, 0.0);

    if (imperviousSoil == 1.0) {
 //   	outputAWC = (1.0 - impAreaFrac - h2oAreaFrac) > 0.0 ? inputAWC / (1.0 - impAreaFrac - h2oAreaFrac) : 0.0;
    	outputAWC = (1.0 - impAreaFrac - h2oAreaFrac) > 0.0 ? ((inputAWC / (1.0 - impAreaFrac - h2oAreaFrac)) * (1.0 - lawnAreaFrac)) + (lawnAreaFrac * lawnAWC): 0.0;
    }

    else if (imperviousSoil == 2.0) {
    	outputAWC = inputAWC;
    }

 // printf("lawnAWC = %f, lawnAreaFrac = %f, impAreaFrac = %f, h2oAreaF = %f, inputAWC = %f, outputAWC = %f\n", lawnAWC, lawnAreaFrac, impAreaFrac, h2oAreaFrac, inputAWC, outputAWC);

	MFVarSetFloat (_MDOutSoilAvailWaterCapID, itemID, outputAWC);	// this is AWC (true depth) for locations with no lakes/impervious

}

/*static void _MDSoilAvailWaterCapInput (int itemID) {
	
	MFVarSetFloat (_MDOutSoilAvailWaterCapID, itemID, MFVarGetFloat(_MDOutSoilAvailWaterCapID,itemID,0.0));
}*/
enum { MDinput, MDcalculate};
int MDSoilAvailWaterCapDef () {
	int  optID = MFUnset;
	const char *optStr, *optName = MDOptSoilAvailableWaterCapacity;
	const char *options [] = { MDInputStr, MDCalculateStr,  (char *) NULL };
	
	if (_MDOutSoilAvailWaterCapID != MFUnset) return (_MDOutSoilAvailWaterCapID);

	MFDefEntering ("Soil available water capacity");

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
		switch (optID) {
		case MDinput:
			if (((_MDInSoilAvailWaterCapID  = MFVarGetID (MDVarSoilAvailWaterCapInput,         "mm",     MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInImperviousSoilID     = MFVarGetID (MDVarImperviousSoil,             "-",     MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInImpFractionID        = MFVarGetID (MDVarImpFracSpatial,             "-",     MFInput,  MFState, MFBoundary)) == CMfailed) ||
		            ((_MDInH2OFractionID        = MFVarGetID (MDVarH2OFracSpatial,             "-",     MFInput,  MFState, MFBoundary)) == CMfailed) ||
 			    ((_MDInLawnFractionID       = MFVarGetID (MDVarLawnFraction,               "-",     MFInput,  MFState, MFBoundary)) == CMfailed) ||
                            ((_MDInLawnAWCID            = MFVarGetID (MDVarLawnAWC,                   "mm",     MFInput,  MFState, MFBoundary)) == CMfailed) ||
                               ((_MDOutSoilAvailWaterCapID = MFVarGetID (MDVarSoilAvailWaterCap,          "mm",    MFOutput,  MFFlux, MFInitial)) == CMfailed) ||
				(MFModelAddFunction (_MDSoilAvailWaterCapInput) == CMfailed)) return (CMfailed);
			break;
		case MDcalculate:
			if (((_MDInSoilFieldCapacityID  = MFVarGetID (MDVarSoilFieldCapacity, "mm/m", MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInSoilWiltingPointID   = MFVarGetID (MDVarSoilWiltingPoint,  "mm/m", MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInSoilRootingDepthID   = MFVarGetID (MDVarSoilRootingDepth,  "mm",   MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutSoilAvailWaterCapID = MFVarGetID (MDVarSoilAvailWaterCap, "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
			    (MFModelAddFunction (_MDSoilAvailWaterCap) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
		}
	MFDefLeaving  ("Soil available water capacity");
	return (_MDOutSoilAvailWaterCapID);
}
