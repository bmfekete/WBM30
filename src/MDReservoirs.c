/******************************************************************************

GHAAS Water Balance/Transport Model V2.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDReservoirs.c

dominik.wisser@unh.edu

Updated with a residence time dependent function 
Feb. 2009 DW

*******************************************************************************/

#include <MF.h>
#include <MD.h>
 
// Input
static int _MDInDischargeID       = MFUnset;
static int _MDInDischMeanID       = MFUnset;
static int _MDInResCapacityID     = MFUnset;

// Output
static int _MDOutResStorageID        = MFUnset;
static int _MDOutResStorageChgID     = MFUnset;
static int _MDOutResReleaseID        = MFUnset;


static void _MDReservoir (int itemID) {

// Input
	float discharge;         // Current discharge [m3/s]
	float meanDischarge;     // Long-term mean annual discharge [m3/s]
	float resCapacity;       // Reservoir capacity [km3]

// Output
	float resStorage;     // Reservoir storage [km3]
	float resStorageChg;  // Reservoir storage change [km3/dt]
	float resRelease;     // Reservoir release [m3/s] 

// local
	float prevResStorage; // Reservoir storage from the previous time step [km3]
	float dt;             // Time step length [s]
	float balance;		  // water balance [m3/s]

// Parameters
	float drySeasonPct = 0.60;	// RJS 071511
	float wetSeasonPct = 0.16;	// RJS 071511
	float year = 0;				// RJS 082311
	
	discharge     = MFVarGetFloat (_MDInDischargeID,    itemID, 0.0);
	meanDischarge = MFVarGetFloat (_MDInDischMeanID,    itemID, discharge);
	year 		  = MFDateGetCurrentYear();

	if ((resCapacity = MFVarGetFloat (_MDInResCapacityID, itemID, 0.0)) <= 0.0) { 
		MFVarSetFloat (_MDOutResStorageID,    itemID, 0.0); 
		MFVarSetFloat (_MDOutResStorageChgID, itemID, 0.0); 
		MFVarSetFloat (_MDOutResReleaseID,    itemID, discharge);

//		if (itemID == 25014) printf("@@@ m= %d, d= %d, balance = %f, resCapacity = %f, Q = %f, meanQ = %f, resRelease = %f, resStorage = %f, prevResStorage = %f\n", MFDateGetCurrentMonth(), MFDateGetCurrentDay(), balance, resCapacity, discharge, meanDischarge, resRelease, resStorage*1000000000, prevResStorage*1000000000);
		return;
	}


//	beta = resCapacity /(meanDischarge * 3600 * 24 * 365/1e9);
	dt = MFModelGet_dt ();
	prevResStorage = MFVarGetFloat(_MDOutResStorageID, itemID, 0.0);

	resRelease = discharge > meanDischarge ?
					wetSeasonPct * discharge  :
	 				drySeasonPct * discharge + (meanDischarge - discharge);

//			- 0.19 * beta + 0.88  * discharge  :
//		           0.47 * beta + 1.12 * discharge;

 	resStorage = prevResStorage + (discharge - resRelease) * 86400.0 / 1e9;

	if (resStorage > resCapacity) {
		resRelease = discharge * dt / 1e9 + prevResStorage - resCapacity;
		resRelease = resRelease * 1e9 / dt;
		resStorage = resCapacity;
	}

	else if (resStorage < 0.0) {
		resRelease = prevResStorage + discharge  * dt / 1e9;
		resRelease = resRelease * 1e9 / dt;
		resStorage=0;
	}
			
	resStorageChg = resStorage - prevResStorage;

	balance = discharge - resRelease - (resStorageChg / 86400 * 1e9);	// water balance

	MFVarSetFloat (_MDOutResStorageID,    itemID, resStorage);
	MFVarSetFloat (_MDOutResStorageChgID, itemID, resStorageChg);
	MFVarSetFloat (_MDOutResReleaseID,    itemID, resRelease);

//	if ((itemID == 25014) && (year < 1980)) printf("y = %f, m= %d, d= %d, dt = %f, balance = %f, resCapacity = %f, Q = %f, meanQ = %f, resRelease = %f, resStorage = %f, prevResStorage = %f\n", year, MFDateGetCurrentMonth(), MFDateGetCurrentDay(), dt, balance, resCapacity, discharge, meanDischarge, resRelease, resStorage*1000000000, prevResStorage*1000000000);


}

enum { MDnone, MDcalculate };

int MDReservoirDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDOptReservoirs;
	const char *options [] = { MDNoneStr, MDCalculateStr, (char *) NULL };
 
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
 
	if ((optID == MDnone) || (_MDOutResReleaseID != MFUnset)) return (_MDOutResReleaseID);

	MFDefEntering ("Reservoirs");
	switch (optID) {
		case MDcalculate:
			if (((_MDInDischMeanID      = MDDischMeanDef ())   == CMfailed) ||
			    ((_MDInDischargeID      = MDDischLevel2Def ()) == CMfailed) ||
			    ((_MDInResCapacityID    = MFVarGetID (MDVarReservoirCapacity,      "km3",  MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutResStorageID    = MFVarGetID (MDVarReservoirStorage,       "km3",  MFOutput, MFState, MFInitial))  == CMfailed) ||
			    ((_MDOutResStorageChgID = MFVarGetID (MDVarReservoirStorageChange, "km3",  MFOutput, MFState, MFBoundary)) == CMfailed) ||		//RJS, changed MFBoundary o MFIniial
			    ((_MDOutResReleaseID    = MFVarGetID (MDVarReservoirRelease,       "m3/s", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
			    (MFModelAddFunction (_MDReservoir) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Reservoirs");
	return (_MDOutResReleaseID); 
}
