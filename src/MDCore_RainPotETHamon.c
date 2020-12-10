/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDPotETHamon.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

static int _MDInDayLengthID = MFUnset;
static int _MDInCommon_AtMeanID    = MFUnset;
static int _MDOutPetID      = MFUnset;

static void _MDRainPotETHamon (int itemID) {
// Hamon (1963) PE in mm for day
// Input
	
//	printf ("Anf Hamon \n");
	float dayLen=0;  // daylength in fraction of day
	float airT;		// air temperatur [degree C]
// Local
	float rhoSat;	// saturated vapor density [kg/m3]
// Output
	float pet;

	dayLen = MFVarGetFloat (_MDInDayLengthID, itemID, 0.1);
	airT   = MFVarGetFloat (_MDInCommon_AtMeanID,    itemID,  0.0);

   rhoSat  = 2.167 * MDPETlibVPressSat (airT) / (airT + 273.15);
   pet     = 165.1 * 2.0 * dayLen * rhoSat; // 2 * DAYLEN = daylength as fraction of 12 hours
   MFVarSetFloat (_MDOutPetID,itemID,pet);
  // printf ("End Hamon\n");
   }

int MDCore_RainPotETHamonDef () {
	if (_MDOutPetID != MFUnset) return (_MDOutPetID);

	MFDefEntering ("Rainfed Potential Evapotranspiration (Hamon)");
	if (((_MDInDayLengthID = MDCommon_SolarRadDayLengthDef()) == CMfailed) ||
        ((_MDInCommon_AtMeanID    = MFVarGetID (MDVarCommon_AirTemperature, "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutPetID      = MFVarGetID (MDVarCore_RainPotEvapotrans, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
        (MFModelAddFunction (_MDRainPotETHamon) == CMfailed)) return (CMfailed);
	MFDefLeaving  ("Rainfed Potential Evapotranspiration (Hamon)");
	return (_MDOutPetID);
}
