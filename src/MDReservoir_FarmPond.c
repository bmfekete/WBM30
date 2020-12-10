/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDSmallReservoirs.c

dominik.wisser@unh.edu

*******************************************************************************/

#include <math.h>
#include <MF.h>
#include <MD.h>

//Input
static int _MDInIrrigation_AreaFracID         = MFUnset;
static int _MDInRainSurfCore_RunoffID      = MFUnset;
static int _MDInIrrigation_GrossDemandID      = MFUnset;
static int _MDInSmallResCapacityID    = MFUnset;
static int _MDInPotEvapotransID       = MFUnset;

//Output
static int _MDOutSmallResReleaseID    = MFUnset;
static int _MDOutSmallResStorageID    = MFUnset;
static int _MDOutSmallResStorageChgID = MFUnset;
static int _MDOutSmallResUptakeID     = MFUnset;
static int _MDInSmallResStorageFracID = MFUnset;
static int _MDOutSmallResEvapoID      = MFUnset;

static void _MDSmallReservoirRelease (int itemID) {
// Input
	float irrAreaFraction;           // Irrigated Area fraction
	float surfRunoff;                // Surface runoff over non irrigated area [mm/dt]
	float grossDemand ;              // Current irrigation water requirement [mm/dt]
	float smallResCapacity;          // maximum storage [mm]
    float openWaterET;               // evaporation from open water surface
    float openWaterToReferenceET;    // factor converting open water et to reference etp
    float averageSmallResDepth;      // assumed depth of small reservoirs
    float smallResSizeFactor;        //relates small res volume to grid cell volume 
	// Output
	float smallResUptake      = 0.0; // current release [mm/dt]
	float smallResRelease     = 0.0; // current release [mm/dt]
	float smallResStorage     = 0.0; // current storage [mm/dt]
	float smallResStorageChg  = 0.0; // current storage change [mm]
	float smallResStorageFrac = 0.0; // determines what fraction of surplus is stored 
	float smallResActualET    = 0.0;
	// Local
	float in = 0.0, out;
	float remainingSurfaceRO = 0.0;
	float smallResPrevStorage;

	if ((irrAreaFraction    = MFVarGetFloat (_MDInIrrigation_AreaFracID,         itemID, 0.0)) > 0.0) {
		smallResCapacity    = MFVarGetFloat (_MDInSmallResCapacityID,    itemID, 0.0); 
		smallResPrevStorage = MFVarGetFloat (_MDOutSmallResStorageID,    itemID, 0.0);
		surfRunoff          = MFVarGetFloat (_MDInRainSurfCore_RunoffID,      itemID, 0.0);
		openWaterET         = MFVarGetFloat (_MDInPotEvapotransID,       itemID, 0.0);
		grossDemand         = MFVarGetFloat (_MDInIrrigation_GrossDemandID,      itemID, 0.0);
		smallResStorageFrac = MFVarGetFloat (_MDInSmallResStorageFracID, itemID, 1.0);

		openWaterToReferenceET = 0.6; // TODO
	    averageSmallResDepth   = 2.0; // TODO
	
		smallResSizeFactor = smallResCapacity / (1000 * averageSmallResDepth);
		openWaterET = openWaterET * openWaterToReferenceET * smallResSizeFactor;

		in = surfRunoff;

		smallResStorage = smallResPrevStorage + surfRunoff * smallResStorageFrac;
		if (smallResStorage >= smallResCapacity) {
			smallResStorage = smallResCapacity;
		    remainingSurfaceRO = surfRunoff - (smallResCapacity - smallResPrevStorage);
		}
		else {
			remainingSurfaceRO = (1.0 - smallResStorageFrac) * surfRunoff;
		}
		smallResUptake = smallResStorage - smallResPrevStorage;

		// ET Water from Reservoir
		smallResActualET = smallResStorage < openWaterET ? smallResStorage : openWaterET;
		smallResStorage  = smallResStorage - smallResActualET;
		
		// Release water from reservoir
		if (smallResStorage > grossDemand) {
			smallResRelease = grossDemand;
			smallResStorage = smallResStorage - smallResRelease;
		}
		else {
			smallResRelease = smallResStorage;
			smallResStorage = 0.0;
		}
		smallResStorageChg = smallResStorage - smallResPrevStorage;
      
		MFVarSetFloat (_MDOutSmallResUptakeID,     itemID, smallResUptake);
		MFVarSetFloat (_MDOutSmallResReleaseID,    itemID, smallResRelease);
		MFVarSetFloat (_MDOutSmallResStorageID,    itemID, smallResStorage);
		MFVarSetFloat (_MDOutSmallResStorageChgID, itemID, smallResStorageChg);
		MFVarSetFloat (_MDOutSmallResEvapoID,      itemID, smallResActualET);

		out = smallResStorageChg + remainingSurfaceRO + smallResRelease + smallResActualET;
		if (fabs (in - out) > 0.001) {
			printf ("ResStorage=%f\n",smallResStorage);
			printf("rest %f WaterBalance S Reser =%f  IN %f surf %f rel %f Dstor %f itemID %i\n",remainingSurfaceRO,in-out,in, surfRunoff, smallResRelease, smallResStorageChg,itemID);
		}

	} //no irrigation
	else {
		MFVarSetFloat (_MDOutSmallResUptakeID,     itemID, 0.0);
		MFVarSetFloat (_MDOutSmallResReleaseID,    itemID, 0.0);
		MFVarSetFloat (_MDOutSmallResStorageID,    itemID, 0.0);
		MFVarSetFloat (_MDOutSmallResStorageChgID, itemID, 0.0);
		MFVarSetFloat (_MDOutSmallResEvapoID,      itemID, 0.0);
	}
}

int MDReservoir_FarmPondReleaseDef () {

	if (_MDOutSmallResReleaseID != MFUnset) return (_MDOutSmallResReleaseID);

	if ((_MDInIrrigation_GrossDemandID   = MDIrrigation_GrossDemandDef()) == CMfailed) return (CMfailed);
	if  (_MDInIrrigation_GrossDemandID   == MFUnset) return (MFUnset);
	if ((_MDInSmallResCapacityID = MDReservoir_FarmPondCapacityDef()) == CMfailed) return (CMfailed);
	if  (_MDInSmallResCapacityID == MFUnset) return (MFUnset);

	MFDefEntering("Small Reservoirs");
    if (((_MDInRainSurfCore_RunoffID      = MDCore_RainSurfRunoffDef()) == CMfailed) ||
        ((_MDInIrrigation_AreaFracID         = MDIrrigation_IrrAreaDef()) == CMfailed) ||
        ((_MDInPotEvapotransID       = MDIrrigation_ReferenceETDef()) == CMfailed) ||
        ((_MDInSmallResStorageFracID = MFVarGetID (MDVarReservoir_FarmPondStorageFrac, "-", MFInput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutSmallResUptakeID     = MFVarGetID (MDVarReservoir_FarmPontUptake, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
        ((_MDOutSmallResReleaseID    = MFVarGetID (MDVarReservoir_FarmPondRelease, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
        ((_MDOutSmallResStorageID    = MFVarGetID (MDVarReservoir_FarmPondStorage, "mm", MFOutput, MFState, MFInitial)) == CMfailed) ||
        ((_MDOutSmallResStorageID    = MFVarGetID (MDVarReservoir_FarmPondStorage, "mm", MFOutput, MFState, MFInitial)) == CMfailed) ||
        ((_MDOutSmallResEvapoID      = MFVarGetID (MDVarReservoir_FarmPondEvaporation, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
        ((_MDOutSmallResStorageChgID = MFVarGetID (MDVarReservoir_FarmPondStorageChange, "mm", MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((MFModelAddFunction (_MDSmallReservoirRelease) == CMfailed))) return (CMfailed);
	MFDefLeaving("Small Reservoirs");
	return (_MDOutSmallResReleaseID);
}
