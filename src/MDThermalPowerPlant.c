/******************************************************************************
GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDThermalInputs.c

rob.stewart@unh.edu

Thermal Inputs and withdrawals

*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>
#include <math.h>


// Input
static int _MDInDischargeID     = MFUnset;
static int _MDInPowerPlantCapID = MFUnset;

// Output
static int _MDOutThermalWdlID	= MFUnset;
static int _MDOutWarmingTempID   = MFUnset;

static void _MDPowerPlant (int itemID) {
	float Q;
	float thermal_wdl = 10.0;
  	Q  = MFVarGetFloat (_MDInDischargeID,     itemID, 0.0);

    	MFVarSetFloat (_MDOutThermalWdlID,        itemID, thermal_wdl);
}


enum { MDnone, MDinput, MDcalculate };

int MDThermalPowerPlantDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDOptThermalInputs;
	const char *options [] = { MDNoneStr, MDInputStr, MDCalculateStr,(char *) NULL };

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);

	MFDefEntering ("Thermal Power Plants");
	switch (optID) {
		case MDinput:
			if (((_MDOutThermalWdlID    = MFVarGetID (MDVarThermalWdl,            "m3", MFInput, MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutWarmingTempID   = MFVarGetID (MDVarWarmingTemp,            "degC", MFInput, MFState, MFBoundary)) == CMfailed))
				_MDOutThermalWdlID = CMfailed;
			break;
		case MDcalculate:
			if (((_MDInDischargeID      = MDDischLevel2Def ()) == CMfailed) ||
			    ((_MDInPowerPlantCapID  = MFVarGetID (MDVarPowerPlantCap,         "kW", MFInput, MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutThermalWdlID    = MFVarGetID (MDVarThermalWdl,            "m3", MFOutput, MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutWarmingTempID   = MFVarGetID (MDVarWarmingTemp,            "degC", MFOutput, MFState, MFBoundary)) == CMfailed) ||
			    (MFModelAddFunction (_MDPowerPlant)  == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Thermal Power Plants");
	return (_MDOutThermalWdlID);
}

