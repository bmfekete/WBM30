/******************************************************************************

GHAAS Water Balance/Transport Model V2.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDPotETTurc.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

static int _MDInAtMeanID = MFUnset;
static int _MDInSolRadID = MFUnset;
static int _MDOutPetID   = MFUnset;

static void _MDRainPotETTurc (int itemID) {
/* Turc (1961) PE in mm for day */
// Input
	float airT;    // air temperatur [degree C]
	float solRad;  // daily solar radiation on horizontal [MJ/m2]
// Output
	float pet;
	
	if ((MFVarTestMissingVal (_MDInAtMeanID, itemID)) ||
		 (MFVarTestMissingVal (_MDInSolRadID, itemID))) { MFVarSetMissingVal (_MDOutPetID,itemID);  return; }

	airT   = MFVarGetFloat (_MDInAtMeanID, itemID, 0.0);
	solRad = MFVarGetFloat (_MDInSolRadID, itemID, 0.0);

	pet = airT > 0.0 ? 0.313 * airT * (solRad + 2.1) / (airT + 15) : 0.0;
   MFVarSetFloat (_MDOutPetID,itemID,pet);
}

int MDRainPotETTurcDef () {
	if (_MDOutPetID != MFUnset) return (_MDOutPetID);

	MFDefEntering ("Rainfed Potential Evapotranspiration (Turc)");
	if (((_MDInSolRadID = MDSolarRadDef ()) == CMfailed) ||
	    ((_MDInAtMeanID = MFVarGetID (MDVarAirTemperature, "degC",  MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutPetID   = MFVarGetID (MDVarRainPotEvapotrans,  "mm",    MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    (MFModelAddFunction (_MDRainPotETTurc) == CMfailed)) return (CMfailed);
	MFDefLeaving  ("Rainfed Potential Evapotranspiration (Turc)");
	return (_MDOutPetID);
}
