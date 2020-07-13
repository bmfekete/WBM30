/******************************************************************************

GHAAS Water Balance Model Library V1.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDIrrRefEtHamon.c

dominik.wisser@unh.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

static int _MDInDayLengthID = MFUnset;
static int _MDInAtMeanID    = MFUnset;
static int _MDOutPetID      = MFUnset;

static void _MDIrrRefEvapotransHamon (int itemID) {
// Hamon (1963) PE in mm for day
// Input
	float dayLen;  // daylength in fraction of day
	float airT;		// air temperatur [degree C]
// Local
	float rhoSat;	// saturated vapor density [kg/m3]
// Output
	float pet;

	dayLen = MFVarGetFloat (_MDInDayLengthID, itemID, 12.0);
	airT   = MFVarGetFloat (_MDInAtMeanID,    itemID,  0.0);

	rhoSat = 2.167 * MDPETlibVPressSat (airT) / (airT + 273.15);
	pet = 165.1 * 2.0 * dayLen * rhoSat; // 2 * DAYLEN = daylength as fraction of 12 hours
//	if (pet ==0) printf ("PET Null ? pet %f rhoSat %f dayLen %f \n", pet, rhoSat, dayLen); 
	MFVarSetFloat (_MDOutPetID,itemID,pet);
}

int MDIrrRefEvapotransHamonDef () {
	if (_MDOutPetID != MFUnset) return (_MDOutPetID);

	MFDefEntering ("Hamon as ReferenceETP");
	if (((_MDInDayLengthID = MDSRadDayLengthDef ()) == CMfailed) ||
	    ((_MDInAtMeanID    = MFVarGetID (MDVarAirTemperature,   "degC", MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutPetID      = MFVarGetID (MDVarIrrRefEvapotrans, "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    (MFModelAddFunction (_MDIrrRefEvapotransHamon) == CMfailed)) return (CMfailed);
	MFDefLeaving ("Hamon as ReferenceETP");
	return (_MDOutPetID);
}
