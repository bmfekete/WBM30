/****************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

MDPotETPsTaylor.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

static int _MDInDayLengthID    = MFUnset;
static int _MDInI0HDayID       = MFUnset;
static int _MDInCParamAlbedoID = MFUnset;

static int _MDInCommon_AtMeanID       = MFUnset;
static int _MDInSolRadID       = MFUnset;
static int _MDInVPressID       = MFUnset;
static int _MDOutPetID         = MFUnset;

static void _MDRainPotETPsTaylor (int itemID) {
// Priestley and Taylor (1972) PE in mm for day
// Input
	float dayLen; // daylength in fraction of day
	float i0hDay; // daily potential insolation on horizontal [MJ/m2]
	float albedo; // albedo
	float airT;   // air temperatur [degree C]
	float solRad; // daily solar radiation on horizontal [MJ/m2]
	float vPress; // daily average vapor pressure [kPa]
	float sHeat = 0.0;  // average subsurface heat storage for day [W/m2]
// Local	
	float solNet;  // average net solar radiation for daytime [W/m2]
	float lngNet;  // average net longwave radiation for day  [W/m2]
	float aa;      // available energy [W/m2]
	float es;      // vapor pressure at airT [kPa]
	float delta;   // dEsat/dTair [kPa/K]
 	float dd;      // vapor pressure deficit [kPa]
	float le;      // latent heat [W/m2]
// Output
	float pet;

	if (MFVarTestMissingVal (_MDInDayLengthID,    itemID) ||
		 MFVarTestMissingVal (_MDInI0HDayID,       itemID) ||
		 MFVarTestMissingVal (_MDInCParamAlbedoID, itemID) ||
		 MFVarTestMissingVal (_MDInCommon_AtMeanID,       itemID) ||
		 MFVarTestMissingVal (_MDInSolRadID,       itemID) ||
		 MFVarTestMissingVal (_MDInVPressID,       itemID)) { MFVarSetMissingVal (_MDOutPetID,itemID); return; }

	dayLen  = MFVarGetFloat (_MDInDayLengthID,    itemID, 0.0);
	i0hDay  = MFVarGetFloat (_MDInI0HDayID,       itemID, 0.0);
	albedo  = MFVarGetFloat (_MDInCParamAlbedoID, itemID, 0.0);
	airT    = MFVarGetFloat (_MDInCommon_AtMeanID,       itemID, 0.0);
	solRad  = MFVarGetFloat (_MDInSolRadID,       itemID, 0.0);
	vPress  = MFVarGetFloat (_MDInVPressID,       itemID, 0.0);

	solNet = (1.0 - albedo) * solRad / MDConstIGRATE;
	lngNet = MDSRadNETLong (i0hDay,airT,solRad,vPress);

	aa     = solNet + lngNet - sHeat;
	es     = MDPETlibVPressSat (airT);
	delta  = MDPETlibVPressDelta (airT);

	dd     = es - vPress; 
   le     = MDConstPTALPHA * delta * aa / (delta + MDConstPSGAMMA);

	pet = MDConstEtoM * MDConstIGRATE * le; 
   MFVarSetFloat (_MDOutPetID,itemID,pet);
}

int MDCore_RainPotETPsTaylorDef () {
	if (_MDOutPetID != MFUnset) return (_MDOutPetID);

	MFDefEntering ("Rainfed Potential Evapotranspiration (Priestley - Taylor)");
	if (((_MDInDayLengthID     = MDCommon_SolarRadDayLengthDef()) == CMfailed) ||
        ((_MDInI0HDayID        = MDCommon_SolarRadI0HDayDef()) == CMfailed) ||
        ((_MDInCParamAlbedoID  = MDParam_LCAlbedoDef()) == CMfailed) ||
        ((_MDInSolRadID        = MDCommon_SolarRadDef()) == CMfailed) ||
        ((_MDInCommon_AtMeanID  = MFVarGetID (MDVarCommon_AirTemperature, "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDInVPressID  = MFVarGetID (MDVarCore_VaporPressure, "kPa", MFInput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutPetID    = MFVarGetID (MDVarCore_RainPotEvapotrans, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
        (MFModelAddFunction (_MDRainPotETPsTaylor) == CMfailed)) return (CMfailed);
	MFDefLeaving  ("Rainfed Potential Evapotranspiration (Priestley - Taylor)");
	return (_MDOutPetID);
}
