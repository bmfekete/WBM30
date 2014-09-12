/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2014, UNH - CCNY/CUNY

MDWTempSurfRunoffPool.c

rob.stewart@unh.edu


*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>
#include <math.h>

// Input

static int _MDInWTempSurfRunoffID 		  = MFUnset;
static int _MDInRainSurfRunoffID          = MFUnset;
static int _MDInRunoffPoolID              = MFUnset;

// Output
static int _MDOutWTempSurfRunoffPoolID  = MFUnset;


static void _MDWTempSurfRunoffPool (int itemID) {

	float SurfRunoffT;
        float SurfRunoffPoolT;

	float SurfRunoff;
        float SurfRunoffPool;

	SurfRunoffT     = MFVarGetFloat (_MDInWTempSurfRunoffID,      itemID, 0.0);
	SurfRunoffPoolT = MFVarGetFloat (_MDOutWTempSurfRunoffPoolID, itemID, 0.0);
	SurfRunoff      = MFVarGetFloat (_MDInRainSurfRunoffID,       itemID, 0.0);
	SurfRunoffPool  = MFVarGetFloat (_MDInRunoffPoolID,           itemID, 0.0);

	SurfRunoff      = MDMaximum(0, SurfRunoff);
	SurfRunoffPool  = MDMaximum(0, SurfRunoffPool);

	if (!isnan(SurfRunoffPool) && !isnan(SurfRunoff) &&
			((SurfRunoffPool + SurfRunoff) > 0) &&
			!isnan(SurfRunoffPoolT) && !isnan(SurfRunoffT)){

	    SurfRunoffPoolT = MDMaximum(((SurfRunoffPool * SurfRunoffPoolT) + (SurfRunoff * SurfRunoffT)) /
	                       (SurfRunoffPool + SurfRunoff), 0.0);

	    MFVarSetFloat (_MDOutWTempSurfRunoffPoolID, itemID, SurfRunoffPoolT);

	}
	else{

  	    MFVarSetMissingVal(_MDOutWTempSurfRunoffPoolID,itemID);

	}

}


static void _MDWTempNoSurfRunoffPool (int itemID) {

	float SurfRunoffT;
    float SurfRunoffPoolT;

	SurfRunoffT     = MFVarGetFloat (_MDInWTempSurfRunoffID,      itemID, 0.0);
	SurfRunoffPoolT = SurfRunoffT;

	MFVarSetFloat (_MDOutWTempSurfRunoffPoolID, itemID, SurfRunoffPoolT);

}


enum {MDnone, MDcalculate, MDinput, MDinput2};																							// RJS 061312 added MDinput
int MDWTempSurfRunoffPoolDef () {
	int  optID = MDnone;	// was MFUnset
	const char *optStr, *optName = MDOptGrdWaterTemp;
	const char *options [] = { MDNoneStr, MDCalculateStr, MDInputStr, MDInput2Str, (char *) NULL };											// RJS 061312 added MDInputStr

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);

	MFDefEntering ("Surface Runoff Pool Temperature");

	switch (optID) {

	case MDcalculate:

	if (((_MDInWTempSurfRunoffID       = MDWTempSurfRunoffDef ()) == CMfailed) ||
		((_MDInRainSurfRunoffID        = MDRainSurfRunoffDef  ()) == CMfailed) ||
		((_MDInRunoffPoolID            = MFVarGetID (MDVarRunoffPool,            "mm", MFInput,  MFState, MFBoundary))  == CMfailed) ||
//		((_MDInRainSurfRunoffID        = MFVarGetID (MDVarRainSurfRunoff,        "mm", MFInput,   MFFlux, MFBoundary)) == CMfailed) ||
	    ((_MDOutWTempSurfRunoffPoolID  = MFVarGetID (MDVarWTempSurfRunoffPool, "degC", MFOutput, MFState, MFInitial)) == CMfailed) ||
	    (MFModelAddFunction (_MDWTempSurfRunoffPool) == CMfailed)) return (CMfailed);
		break;

	case MDnone:

	if (((_MDInWTempSurfRunoffID       = MDWTempSurfRunoffDef ()) == CMfailed) ||
		((_MDOutWTempSurfRunoffPoolID  = MFVarGetID (MDVarWTempSurfRunoffPool, "degC", MFOutput, MFState, MFInitial)) == CMfailed) ||
		(MFModelAddFunction (_MDWTempNoSurfRunoffPool) == CMfailed)) return (CMfailed);
		break;

	case MDinput:	_MDOutWTempSurfRunoffPoolID = MFVarGetID (MDVarWTempSurfRunoffPool, "degC", MFInput,  MFState, MFBoundary); break;		// RJS 061312	MFInitial changed to MFBoundary in order to read in																														// RJS 061312

        case MDinput2:	_MDOutWTempSurfRunoffPoolID = MFVarGetID (MDVarWTempSurfRunoffPool, "degC", MFInput,  MFState, MFBoundary); break;		// RJS 061312	MFInitial changed to MFBoundary in order to read in																														// RJS 061312
        
	default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}

	MFDefLeaving ("Surface Runoff Pool Temperature");
	return (_MDOutWTempSurfRunoffPoolID);
}
