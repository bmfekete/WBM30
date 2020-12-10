/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDSmallReservoirCap.c

dominik.wisser@unh.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

//Input
static int _MDInIrrAreaID                    = MFUnset;
static int _MDInRainSurfCore_RunoffID             = MFUnset;
static int _MDInIrrigation_GrossDemandID             = MFUnset;

//Output
static int _MDOutRainSurfRunoffAccumulatedID = MFUnset;
static int _MDOutIrrGrossDemandAccumulatedID = MFUnset;
static int _MDOutSmallResCapacityID          = MFUnset;
static int _MDInSmallResStorageFractionID    = MFUnset;

static void _MDSmallReservoirCapacity (int itemID) {
// Input
	float irrAreaFraction;      // Irrigated arrea fraction
	float accumSurfRunoff;      // Accumulated surface runoff [mm]
	float accumIrrDemand;       // Accumulated irrigational water demand [mm]
// Output
 	float smallResCapacity;     // maximum storage, m3
 // Local
 	float potResCapacity;
 	
    if ((irrAreaFraction = MFVarGetFloat (_MDInIrrAreaID, itemID, 0.0)) > 0.0) {
 		accumSurfRunoff  = MFVarGetFloat (_MDInRainSurfCore_RunoffID,          itemID, 0.0) *
 		                   MFVarGetFloat (_MDInSmallResStorageFractionID, itemID, 1.0);
 		accumIrrDemand   = MFVarGetFloat (_MDInIrrigation_GrossDemandID,          itemID, 0.0);
 		smallResCapacity = MFVarGetFloat (_MDOutSmallResCapacityID,       itemID, 0.0); 
 		 
 		if (MFDateGetDayOfYear () > 1) { 
 			accumSurfRunoff += MFVarGetFloat (_MDOutRainSurfRunoffAccumulatedID, itemID, 0.0);
 			accumIrrDemand  += MFVarGetFloat (_MDOutIrrGrossDemandAccumulatedID, itemID, 0.0);
 		}
 		potResCapacity   = accumSurfRunoff  < accumIrrDemand ? accumSurfRunoff  : accumIrrDemand;
 		smallResCapacity = smallResCapacity > potResCapacity ? smallResCapacity : potResCapacity;
 	}
 	else accumSurfRunoff = accumIrrDemand = smallResCapacity = 0.0;

 	MFVarSetFloat (_MDOutRainSurfRunoffAccumulatedID, itemID, accumSurfRunoff);
 	MFVarSetFloat (_MDOutIrrGrossDemandAccumulatedID, itemID, accumIrrDemand);
 	MFVarSetFloat (_MDOutSmallResCapacityID,          itemID, smallResCapacity);
}

enum { MDnone, MDinput, MDcalculate };

int MDReservoir_FarmPondCapacityDef () {

	int  optID = MFUnset;
	const char *optStr, *optName = MDVarReservoir_FarmPondSmallResCapacity;
	const char *options [] = { MDNoneStr, MDInputStr, MDCalculateStr, (char *) NULL };

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
		
	if ((optID == MDnone) || (_MDOutSmallResCapacityID != MFUnset)) return (_MDOutSmallResCapacityID);

	MFDefEntering("SmallReservoirCapacity");
	if ((_MDInIrrigation_GrossDemandID = MDIrrigation_GrossDemandDef()) != MFUnset) {
		switch (optID) {
			case MDinput:
			    if ((_MDOutSmallResCapacityID       = MFVarGetID (MDVarReservoir_FarmPondSmallResCapacity, "mm", MFInput, MFState, MFBoundary)) == CMfailed)
			    	return (CMfailed);
			    break;
			case MDcalculate:
				if ((_MDInIrrigation_GrossDemandID == CMfailed) ||
                    ((_MDInIrrAreaID                    = MDIrrigation_IrrAreaDef()) == CMfailed) ||
                    ((_MDInRainSurfCore_RunoffID             = MFVarGetID (MDVarCore_RainSurfRunoff, "mm", MFInput, MFFlux, MFBoundary)) == CMfailed) ||
                    ((_MDOutRainSurfRunoffAccumulatedID = MFVarGetID ("__SurfaceROAccumulated",       "mm",  MFOutput, MFState, MFInitial))  == CMfailed) ||
                    ((_MDOutIrrGrossDemandAccumulatedID = MFVarGetID ("__GrossDemandAccumulated",     "mm",  MFOutput, MFState, MFInitial))  == CMfailed) ||
                    ((_MDInSmallResStorageFractionID    = MFVarGetID (MDVarReservoir_FarmPondStorageFrac, "-", MFInput, MFState, MFBoundary)) == CMfailed) ||
                    ((_MDOutSmallResCapacityID          = MFVarGetID (MDVarReservoir_FarmPondSmallResCapacity, "mm", MFOutput, MFState, MFInitial)) == CMfailed) ||
                    (MFModelAddFunction (_MDSmallReservoirCapacity) == CMfailed)) return (CMfailed);
				break;
			default: MFOptionMessage (optName, optStr, options); return (CMfailed);
		}
	}
	MFDefLeaving("SmallReservoirCapacity");
	return (_MDOutSmallResCapacityID);
}
