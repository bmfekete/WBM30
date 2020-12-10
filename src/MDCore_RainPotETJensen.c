/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDPotETJensen.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

static int _MDInCommon_AtMeanID = MFUnset;
static int _MDInSolRadID = MFUnset;
static int _MDOutPetID   = MFUnset;

static void _MDRainPotETJensen (int itemID) {
// Jensen-Haise (1963) PE in mm for day
// Input
	float airT;		// air temperatur [degree C]
	float solRad;  // daily solar radiation on horizontal [MJ/m2]
// Output 
	float pet;
	
	if ((MFVarTestMissingVal (_MDInCommon_AtMeanID, itemID)) ||
		 (MFVarTestMissingVal (_MDInSolRadID, itemID))) { MFVarSetMissingVal (_MDOutPetID,itemID);  return; }

	airT   = MFVarGetFloat (_MDInCommon_AtMeanID, itemID, 0.0);
	solRad = MFVarGetFloat (_MDInSolRadID, itemID, 0.0);

	pet = 0.41 * (0.025 * airT + .078) * solRad;
   MFVarSetFloat (_MDOutPetID,itemID,pet);
}

int MDCore_RainPotETJensenDef () {
	if (_MDOutPetID != MFUnset) return (_MDOutPetID);

	MFDefEntering ("Rainfed Potential Evapotranspiration (Jensen)");
	if (((_MDInSolRadID = MDCommon_SolarRadDef()) == CMfailed) ||
        ((_MDInCommon_AtMeanID = MFVarGetID (MDVarCommon_AirTemperature, "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutPetID   = MFVarGetID (MDVarCore_RainPotEvapotrans, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
        (MFModelAddFunction (_MDRainPotETJensen) == CMfailed)) return (CMfailed);
	MFDefLeaving  ("Rainfed Potential Evapotranspiration (Jensen)");
	return (_MDOutPetID);
}
