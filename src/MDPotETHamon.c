/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2014, UNH - CCNY/CUNY

MDPotETHamon.c

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <stdio.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

static int _MDInDayLengthID = MFUnset;
static int _MDInAtMeanID    = MFUnset;
static int _MDOutPetID      = MFUnset;

static void _MDPotETHamon (int itemID) {
// Hamon (1963) PE in mm for day
// Input
	float dayLen;  // daylength in fraction of day
	float airT;		// air temperatur [degree C]
// Local
	float rhoSat;	// saturated vapor density [kg/m3]
// Output
	float pet;

	if (MFVarTestMissingVal (_MDInDayLengthID, itemID) ||
		 MFVarTestMissingVal (_MDInAtMeanID,    itemID)) { MFVarSetMissingVal (_MDOutPetID,itemID);  return; }

	dayLen = MFVarGetFloat (_MDInDayLengthID, itemID, 12.0);
//	dayLen = 1;
	airT   = MFVarGetFloat (_MDInAtMeanID,    itemID,  0.0);
   rhoSat = 2.167 * MDPETlibVPressSat (airT) / (airT + 273.15);
   pet = 165.1 * 2.0 * dayLen * rhoSat; // 2 * DAYLEN = daylength as fraction of 12 hours
//   printf("Day length = %f Temp = %f PET = %f Day = %i\n", dayLen, airT, pet, MFDateGetDayOfYear ());
   MFVarSetFloat (_MDOutPetID,itemID,pet);
}

int MDPotETHamonDef () {
	if (_MDOutPetID != MFUnset) return (_MDOutPetID);

	MFDefEntering ("PotET Hamon");
	if (((_MDInDayLengthID = MDSRadDayLengthDef ()) == CMfailed) ||
	    ((_MDInAtMeanID    = MFVarGetID (MDVarAirTemperature, "degC", MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutPetID      = MFVarGetID (MDVarPotEvapotrans,  "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
            (MFModelAddFunction (_MDPotETHamon) == CMfailed)) return (CMfailed);
	MFDefLeaving ("PotET Hamon");
	return (_MDOutPetID);
}
