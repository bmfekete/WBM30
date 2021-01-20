/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

MDPotETPMdn.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <math.h>
#include <MF.h>
#include <MD.h>

static int _MDInDayLengthID     = MFUnset;
static int _MDInI0HDayID        = MFUnset;
static int _MDInCParamAlbedoID  = MFUnset;
static int _MDInCParamCHeightID = MFUnset;
static int _MDInCParamR5ID      = MFUnset;
static int _MDInCParamCDID      = MFUnset;
static int _MDInCParamCRID      = MFUnset;
static int _MDInCParamGLMaxID   = MFUnset;
static int _MDInCParamZ0gID     = MFUnset;
static int _MDInLeafAreaIndexID = MFUnset;
static int _MDInStemAreaIndexID = MFUnset;

static int _MDInCommon_AtMeanID        = MFUnset;
static int _MDInAtMinID         = MFUnset;
static int _MDInAtMaxID         = MFUnset;
static int _MDInSolRadID        = MFUnset;
static int _MDInVPressID        = MFUnset;
static int _MDInWSpeedID        = MFUnset;
static int _MDOutPetID          = MFUnset;

static void _MDRainPotETPMdn (int itemID) {
// day-night Penman-Monteith PE in mm for day
// Input
	float dayLen;  // daylength in fraction of day
 	float i0hDay;  //  daily potential insolation on horizontal [MJ/m2]
	float albedo;  // albedo 
	float height;  // canopy height [m]
	float r5;      // solar radiation at which conductance is halved [W/m2]
	float cd;      // vpd at which conductance is halved [kPa]
	float cr;      // light extinction coefficient for projected LAI
	float glMax;   // maximum leaf surface conductance for all sides of leaf [m/s]
	float z0g;     // z0g       - ground surface roughness [m]
 	float lai;     // projected leaf area index
	float sai;     // projected stem area index
	float airT;    // air temperatur [degree C]
	float airTMin; // daily minimum air temperature [degree C] 
	float airTMax; // daily maximum air temperature [degree C] 
	float solRad;  // daily solar radiation on horizontal [MJ/m2]
	float vPress;  // daily average vapor pressure [kPa]
	float wSpeed;  // average wind speed for the day [m/s] 
	float sHeat = 0.0; // average subsurface heat storage for day [W/m2]
// Local_MDOutPetID
	float solNet;  // average net solar radiation for daytime [W/m2]
	float airTDtm, airTNtm; // air temperature for daytime and nighttime [degC]
	float uaDtm,   uaNtm;	// average wind speed for daytime and nighttime [m/s]
	float lngDtm,	lngNtm;	// average net longwave radiation for daytime and nighttime [W/m2]
	float za;      // reference height [m]
 	float disp;    // height of zero-plane [m]
	float z0;      // roughness parameter [m] 
	float aa;		// available energy [W/m2]
	float es;      // vapor pressure at airT [kPa]
	float delta;   // dEsat/dTair [kPa/K]
 	float dd;      // vapor pressure deficit [kPa]
	float ra;		//	aerodynamic resistance [s/ma]
 	float rc;		// canopy resistance [s/m]
	float led, len;// daytime and nighttime latent heat [W/m2]
// Output
	float pet;

	if (MFVarTestMissingVal (_MDInDayLengthID,    itemID) ||
		 MFVarTestMissingVal (_MDInI0HDayID,       itemID) ||
		 MFVarTestMissingVal (_MDInCParamAlbedoID, itemID) ||
		 MFVarTestMissingVal (_MDInCParamCHeightID,itemID) ||
		 MFVarTestMissingVal (_MDInCParamR5ID,     itemID) ||
		 MFVarTestMissingVal (_MDInCParamCDID,     itemID) ||
		 MFVarTestMissingVal (_MDInCParamCRID,     itemID) ||
		 MFVarTestMissingVal (_MDInCParamGLMaxID,  itemID) ||
		 MFVarTestMissingVal (_MDInCParamZ0gID,    itemID) ||
		 MFVarTestMissingVal (_MDInLeafAreaIndexID,itemID) ||
		 MFVarTestMissingVal (_MDInStemAreaIndexID,itemID) ||
		 MFVarTestMissingVal (_MDInCommon_AtMeanID,       itemID) ||
		 MFVarTestMissingVal (_MDInAtMinID,        itemID) ||
		 MFVarTestMissingVal (_MDInAtMaxID,        itemID) ||
		 MFVarTestMissingVal (_MDInSolRadID,       itemID) ||
		 MFVarTestMissingVal (_MDInVPressID,       itemID) ||
		 MFVarTestMissingVal (_MDInWSpeedID,       itemID)) { MFVarSetMissingVal (_MDOutPetID,itemID); return; }

	dayLen  = MFVarGetFloat (_MDInDayLengthID,    itemID, 0.0);
	i0hDay  = MFVarGetFloat (_MDInI0HDayID,       itemID, 0.0);
	albedo  = MFVarGetFloat (_MDInCParamAlbedoID, itemID, 0.0);
	height  = MFVarGetFloat (_MDInCParamCHeightID,itemID, 0.0);
	r5      = MFVarGetFloat (_MDInCParamR5ID,     itemID, 0.0);
	cd      = MFVarGetFloat (_MDInCParamCDID,     itemID, 0.0);
	cr      = MFVarGetFloat (_MDInCParamCRID,     itemID, 0.0);
	glMax   = MFVarGetFloat (_MDInCParamGLMaxID,  itemID, 0.0);
	z0g     = MFVarGetFloat (_MDInCParamZ0gID,    itemID, 0.0);
	lai     = MFVarGetFloat (_MDInLeafAreaIndexID,itemID, 0.0);
	sai     = MFVarGetFloat (_MDInStemAreaIndexID,itemID, 0.0);
	airT    = MFVarGetFloat (_MDInCommon_AtMeanID,       itemID, 0.0);
	airTMin = MFVarGetFloat (_MDInAtMinID,        itemID, 0.0);
	airTMax = MFVarGetFloat (_MDInAtMaxID,        itemID, 0.0);
	solRad  = MFVarGetFloat (_MDInSolRadID,       itemID, 0.0);
	vPress  = MFVarGetFloat (_MDInVPressID,       itemID, 0.0);
	wSpeed  = fabs (MFVarGetFloat (_MDInWSpeedID, itemID, 0.0));
	if (wSpeed < 0.2) wSpeed = 0.2;

// daytime
	if (dayLen > 0) {
		solNet  = (1.0 - albedo) * solRad / (MDConstIGRATE * dayLen);

		airTDtm = airT + ((airTMax - airTMin) / (2 * M_PI * dayLen)) * sin (M_PI * dayLen);
		uaDtm   = wSpeed / (dayLen + (1.0 - dayLen) * MDConstWNDRAT);
		lngDtm  = MDSRadNETLong (i0hDay,airTDtm,solRad,vPress);

		za      = height + MDConstZMINH;
		disp    = MDPETlibZPDisplacement (height,lai,sai,z0g);
		z0      = MDPETlibRoughness (disp,height,lai,sai,z0g);
		
		aa      = solNet + lngDtm - sHeat;
		es      = MDPETlibVPressSat (airTDtm);
		delta   = MDPETlibVPressDelta (airTDtm);
		dd      = es - vPress; 
		ra      = log ((za - disp) / z0);
		ra      = ra * ra / (0.16 * uaDtm);
		rc      = MDPETlibCanopySurfResistance (airTDtm,solRad / dayLen,dd,lai,sai,r5,cd,cr,glMax);
		led     = MDPETlibPenmanMontieth (aa, dd, delta, ra, rc);
	}
	else {
		led = 0.0;
		uaDtm = wSpeed / MDConstWNDRAT;
	}

// nighttime
	if (dayLen < 1.0) {
		airTNtm = airT - ((airTMax - airTMin) / (2 * M_PI * (1 - dayLen))) * sin (M_PI * dayLen);
		uaNtm   = MDConstWNDRAT * uaDtm;
		lngNtm  = MDSRadNETLong (i0hDay,airTNtm,solRad,vPress);

		aa      = lngNtm - sHeat;
		es      = MDPETlibVPressSat (airTNtm);
		delta   = MDPETlibVPressDelta (airTNtm);
		dd      = es - vPress;
		rc      = 1 / (MDConstGLMIN * lai);
		ra      = log ((za - disp) / z0);
		ra      = (ra * ra) / (0.16 * uaNtm);

		len     = MDPETlibPenmanMontieth (aa, dd, delta, ra, rc);
	}
	else len = 0.0;

	pet = MDConstEtoM * MDConstIGRATE * (dayLen * led + (1.0 - dayLen) * len);
   MFVarSetFloat (_MDOutPetID,itemID,pet);
}

int MDCore_RainPotETPMdnDef () {
	if (_MDOutPetID != MFUnset) return (_MDOutPetID);

	MFDefEntering ("Rainfed Potential Evapotranspiration (Penman - Montieth [day-night])");
	if (((_MDInDayLengthID     = MDCommon_SolarRadDayLengthDef()) == CMfailed) ||
        ((_MDInI0HDayID        = MDCommon_SolarRadI0HDayDef()) == CMfailed) ||
        ((_MDInCParamAlbedoID  = MDParam_LCAlbedoDef()) == CMfailed) ||
        ((_MDInCParamCHeightID = MDParam_LCHeightDef()) == CMfailed) ||
        ((_MDInCParamR5ID      = MDParam_LCR5Def()) == CMfailed) ||
        ((_MDInCParamCDID      = MDParam_LCCDDef()) == CMfailed) ||
        ((_MDInCParamCRID      = MDParam_LCCRDef()) == CMfailed) ||
        ((_MDInCParamGLMaxID   = MDParam_LCGLMaxDef()) == CMfailed) ||
        ((_MDInCParamZ0gID     = MDParam_LCZ0gDef()) == CMfailed) ||
        ((_MDInLeafAreaIndexID = MDParam_LeafAreaIndexDef()) == CMfailed) ||
        ((_MDInStemAreaIndexID = MDParam_LCStemAreaIndexDef()) == CMfailed) ||
        ((_MDInSolRadID        = MDCommon_SolarRadDef()) == CMfailed) ||
            ((_MDInCommon_AtMeanID  = MFVarGetID (MDVarCommon_AirTemperature, "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
			((_MDInAtMinID   = MFVarGetID (MDVarCommon_AirTempMinimum, "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
			((_MDInAtMaxID   = MFVarGetID (MDVarCommon_AirTempMaximum, "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
			((_MDInVPressID  = MFVarGetID (MDVarCore_VaporPressure, "kPa", MFInput, MFState, MFBoundary)) == CMfailed) ||
			((_MDInWSpeedID  = MFVarGetID (MDVarCommon_WindSpeed, "m/s", MFInput, MFState, MFBoundary)) == CMfailed) ||
			((_MDOutPetID    = MFVarGetID (MDVarCore_RainPotEvapotrans, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
            (MFModelAddFunction (_MDRainPotETPMdn) == CMfailed)) return (CMfailed);
	MFDefLeaving  ("Rainfed Potential Evapotranspiration (Penman - Montieth [day-night])");
	return(_MDOutPetID);
}
