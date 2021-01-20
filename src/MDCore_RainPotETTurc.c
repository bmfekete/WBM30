/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

MDPotETTurc.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

static int _MDInCommon_AtMeanID = MFUnset;
static int _MDInSolRadID = MFUnset;
static int _MDOutPetID   = MFUnset;

static void _MDRainPotETTurc (int itemID) {
/* Turc (1961) PE in mm for day */
// Input
	float airT;    // air temperatur [degree C]
	float solRad;  // daily solar radiation on horizontal [MJ/m2]
// Output
	float pet;
	
	if ((MFVarTestMissingVal (_MDInCommon_AtMeanID, itemID)) ||
		 (MFVarTestMissingVal (_MDInSolRadID, itemID))) { MFVarSetMissingVal (_MDOutPetID,itemID);  return; }

	airT   = MFVarGetFloat (_MDInCommon_AtMeanID, itemID, 0.0);
	solRad = MFVarGetFloat (_MDInSolRadID, itemID, 0.0);

	pet = airT > 0.0 ? 0.313 * airT * (solRad + 2.1) / (airT + 15) : 0.0;
   MFVarSetFloat (_MDOutPetID,itemID,pet);
}

int MDCore_RainPotETTurcDef () {
	if (_MDOutPetID != MFUnset) return (_MDOutPetID);

	MFDefEntering ("Rainfed Potential Evapotranspiration (Turc)");
	if (((_MDInSolRadID = MDCommon_SolarRadDef()) == CMfailed) ||
        ((_MDInCommon_AtMeanID = MFVarGetID (MDVarCommon_AirTemperature, "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutPetID   = MFVarGetID (MDVarCore_RainPotEvapotrans, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
        (MFModelAddFunction (_MDRainPotETTurc) == CMfailed)) return (CMfailed);
	MFDefLeaving  ("Rainfed Potential Evapotranspiration (Turc)");
	return (_MDOutPetID);
}
