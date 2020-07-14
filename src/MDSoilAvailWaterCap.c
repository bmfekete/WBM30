/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDSoilAvailWaterCap.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

// Input
static int _MDInSoilFieldCapacityID  = MFUnset;
static int _MDInSoilWiltingPointID   = MFUnset;
static int _MDInSoilRootingDepthID   = MFUnset;

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

enum { MDinput, MDcalculate};
int MDSoilAvailWaterCapDef () {
	int  optID = MFUnset;
	char *optStr, *optName = MDOptSoilAvailableWaterCapacity;
	char *options [] = { MDInputStr, MDCalculateStr,  (char *) NULL };
	
	if (_MDOutSoilAvailWaterCapID != MFUnset) return (_MDOutSoilAvailWaterCapID);

	MFDefEntering ("Soil available water capacity");

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
		switch (optID) {
		case MDinput: _MDOutSoilAvailWaterCapID = MFVarGetID (MDVarSoilAvailWaterCap,         "mm",     MFInput,  MFState, MFBoundary); break;
		case MDcalculate:
			if (((_MDInSoilFieldCapacityID  = MFVarGetID (MDVarSoilFieldCapacity, "mm/m", MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInSoilWiltingPointID   = MFVarGetID (MDVarSoilWiltingPoint,  "mm/m", MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInSoilRootingDepthID   = MFVarGetID (MDVarSoilRootingDepth,  "mm",   MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutSoilAvailWaterCapID = MFVarGetID (MDVarSoilAvailWaterCap, "mm",   MFOutput, MFState, MFBoundary)) == CMfailed) ||
			    (MFModelAddFunction (_MDSoilAvailWaterCap) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
		}
	MFDefLeaving  ("Soil available water capacity");
	return (_MDOutSoilAvailWaterCapID);
}
