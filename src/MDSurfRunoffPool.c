/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDSurfRunoffPool.c

rob.stewart@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <math.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInRainSurfRunoffID           = MFUnset;

// Output
static int _MDOutRunoffPoolChgID           = MFUnset;
static int _MDOutRunoffPoolID              = MFUnset;
static int _MDOutRunoffPoolRechargeID      = MFUnset;
static int _MDOutRunoffPoolReleaseID       = MFUnset;

static float _MDSurfRunoffPoolBETA 		   = 0.67;

static void _MDSurfRunoffPool (int itemID) {

// Output
	float runoffPool          = 0.0; 	// Pool size   [mm]
	float runoffPoolChg       = 0.0; 	// Pool change [mm/dt]
	float runoffPoolRecharge  = 0.0; 	// Groundwater recharge [mm/dt]
	float runoffPoolRelease   = 0.0; 	// Release from runoff pool [mm/dt]

	runoffPoolChg 		= runoffPool = MFVarGetFloat (_MDOutRunoffPoolID,   itemID, 0.0);
	runoffPoolRecharge  = MFVarGetFloat (_MDInRainSurfRunoffID,  itemID, 0.0);
	runoffPool          = runoffPool + runoffPoolRecharge;

	if (runoffPool > 0.0) {

		runoffPoolRelease    = runoffPool * _MDSurfRunoffPoolBETA;

		runoffPool           = runoffPool - runoffPoolRelease;
		runoffPoolChg        = runoffPool - runoffPoolChg;
	}
	
	else runoffPoolChg = runoffPoolRelease = 0.0;
	
//	printf("BETA2 = %f\n", _MDSurfRunoffPoolBETA);
        
  //      if (itemID == 1) printf("Beta2 = %f\n", _MDSurfRunoffPoolBETA);

	MFVarSetFloat (_MDOutRunoffPoolID,          itemID, runoffPool);
    MFVarSetFloat (_MDOutRunoffPoolChgID,       itemID, runoffPoolChg);
    MFVarSetFloat (_MDOutRunoffPoolRechargeID,  itemID, runoffPoolRecharge);
	MFVarSetFloat (_MDOutRunoffPoolReleaseID,   itemID, runoffPoolRelease);
	
}

static void _MDNoSurfRunoffPool (int itemID) {

	float runoffPoolRelease = 0.0;
	float runoffPoolChange  = 0.0;

	runoffPoolRelease = MFVarGetFloat (_MDInRainSurfRunoffID, itemID, 0.0);

	MFVarSetFloat (_MDOutRunoffPoolReleaseID,  itemID, runoffPoolRelease);
	MFVarSetFloat (_MDOutRunoffPoolChgID,   itemID, runoffPoolChange);	//RJS 091212

}


static void _MDSurfRunoffPoolInput2 (int itemID) {      // RJS 060214

// Output
	float runoffPool          = 0.0; 	// Pool size   [mm]
	float runoffPoolChg       = 0.0; 	// Pool change [mm/dt]
	float runoffPoolRecharge  = 0.0; 	// Groundwater recharge [mm/dt]
	float runoffPoolRelease   = 0.0; 	// Release from runoff pool [mm/dt]

	runoffPoolChg 	    = runoffPool = MFVarGetFloat (_MDOutRunoffPoolID,   itemID, 0.0);
	runoffPoolRecharge  = MFVarGetFloat (_MDInRainSurfRunoffID,  itemID, 0.0);
	runoffPool          = runoffPool + runoffPoolRecharge;

	if (runoffPool > 0.0) {

		runoffPoolRelease    = runoffPool * _MDSurfRunoffPoolBETA;

		runoffPool           = runoffPool - runoffPoolRelease;
		runoffPoolChg        = runoffPool - runoffPoolChg;
	}
	
	else runoffPoolChg = runoffPoolRelease = 0.0;
	

        
	MFVarSetFloat (_MDOutRunoffPoolID,          itemID, runoffPool);
        MFVarSetFloat (_MDOutRunoffPoolChgID,       itemID, runoffPoolChg);
        MFVarSetFloat (_MDOutRunoffPoolRechargeID,  itemID, runoffPoolRecharge);
	MFVarSetFloat (_MDOutRunoffPoolReleaseID,   itemID, runoffPoolRelease);
	
}

enum {MDnone, MDcalculate, MDinput2};
int MDSurfRunoffPoolDef () {
 
	float par;
	int  optID = MFUnset;
	const char *optStr, *optName = MDOptSurfRunoffPool;
	const char *options [] = { MDNoneStr, MDCalculateStr, MDInput2Str, (char *) NULL };             // RJS 060214 added input2 
if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
if (((optStr = MFOptionGet (MDParSurfWaterBETA2))   != (char *) NULL) && (sscanf (optStr,"%f",&par) == 1)) _MDSurfRunoffPoolBETA = par;

	MFDefEntering ("Surface Runoff Pool");

	switch (optID) {
	
	case MDcalculate:

	if 	   (((_MDInRainSurfRunoffID          = MDRainSurfRunoffDef  ()) == CMfailed)  ||
			((_MDOutRunoffPoolID             = MFVarGetID (MDVarRunoffPool,         "mm", MFOutput, MFState, MFInitial))  == CMfailed) ||
			((_MDOutRunoffPoolChgID          = MFVarGetID (MDVarRunoffPoolChg,      "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
			((_MDOutRunoffPoolRechargeID     = MFVarGetID (MDVarRunoffPoolRecharge, "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
			((_MDOutRunoffPoolReleaseID      = MFVarGetID (MDVarRunoffPoolRelease,  "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
			(MFModelAddFunction (_MDSurfRunoffPool) == CMfailed)) return (CMfailed);
			break;

	case MDnone:

	if 	   (((_MDInRainSurfRunoffID          = MDRainSurfRunoffDef  ()) == CMfailed)  ||
			((_MDOutRunoffPoolChgID          = MFVarGetID (MDVarRunoffPoolChg,      "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||	//RJS 091212
			((_MDOutRunoffPoolReleaseID      = MFVarGetID (MDVarRunoffPoolRelease,  "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
			(MFModelAddFunction (_MDNoSurfRunoffPool) == CMfailed)) return (CMfailed);
			break;
        case MDinput2:

	if 	   (((_MDInRainSurfRunoffID              = MFVarGetID (MDVarRainSurfRunoff,     "mm", MFInput,  MFFlux, MFBoundary)) == CMfailed) ||
			((_MDOutRunoffPoolID             = MFVarGetID (MDVarRunoffPool,         "mm", MFOutput, MFState, MFInitial))  == CMfailed) ||
			((_MDOutRunoffPoolChgID          = MFVarGetID (MDVarRunoffPoolChg,      "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
			((_MDOutRunoffPoolRechargeID     = MFVarGetID (MDVarRunoffPoolRecharge, "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
			((_MDOutRunoffPoolReleaseID      = MFVarGetID (MDVarRunoffPoolRelease,  "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
			(MFModelAddFunction (_MDSurfRunoffPoolInput2) == CMfailed)) return (CMfailed);
			break;
	default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	
	MFDefLeaving ("Surface Runoff Pool");
	return (_MDOutRunoffPoolReleaseID);
}






