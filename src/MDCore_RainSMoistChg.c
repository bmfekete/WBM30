/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDRainSMoistChg.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <math.h>
#include <MF.h>
#include <MD.h>

static float _MDSoilMoistALPHA = 5.0;

// Input
static int _MDInAirTemperatureID            = MFUnset;
static int _MDInCommon_PrecipID              = MFUnset;
static int _MDInPotETID               = MFUnset;
static int _MDInInterceptID           = MFUnset;
static int _MDInSPackChgID            = MFUnset;
static int _MDInSoilAvailWaterCapID   = MFUnset;
static int _MDInIrrigation_AreaFracID         = MFUnset;
static int _MDInRelativeIceContent    = MFUnset;
// Output
static int _MDOutEvaptrsID            = MFUnset;
static int _MDOutSoilMoistCellID      = MFUnset;
static int _MDOutSoilMoistID          = MFUnset;
static int _MDOutSMoistChgID          = MFUnset;
static int _MDOutLiquidSoilMoistureID = MFUnset;

static void _MDRainSMoistChg (int itemID) {	
// Input
	float airT;              // Air temperature [degreeC]
	float precip;            // Precipitation [mm/dt]
	float pet;               // Potential evapotranspiration [mm/dt]
	float intercept;         // Interception (when the interception module is turned on) [mm/dt]
	float sPackChg;          // Snow pack change [mm/dt]
	float irrAreaFrac = 0.0; // Irrigated area fraction
//	TODO float impAreaFrac = 0.0; // Impervious area fraction RJS 01-17-08
//	TODO float H2OAreaFrac = 0.0; // water area fraction RJS 01-17-08
//	float runofftoPerv;      // runoff from impervious to pervious [mm/dt]  RJS 01-17-08
	float sMoist      = 0.0; // Soil moisture [mm/dt]
// Output
	float sMoistChg   = 0.0; // Soil moisture change [mm/dt]
	float evapotrans;
// Local
	float waterIn;
	float awCap;
	float gm;
	float iceContent;
	airT         = MFVarGetFloat (_MDInAirTemperatureID,          itemID, 0.0);
	precip       = MFVarGetFloat (_MDInCommon_PrecipID,            itemID, 0.0);
 	sPackChg     = MFVarGetFloat (_MDInSPackChgID,          itemID, 0.0);
	pet          = MFVarGetFloat (_MDInPotETID,             itemID, 0.0);
	awCap        = MFVarGetFloat (_MDInSoilAvailWaterCapID, itemID, 0.0);
	sMoist       = MFVarGetFloat (_MDOutSoilMoistCellID,    itemID, 0.0);
	iceContent   = _MDInRelativeIceContent != MFUnset ? MFVarGetFloat (_MDInRelativeIceContent,  itemID, 0.0) : 0.0;
	intercept    = _MDInInterceptID        != MFUnset ? MFVarGetFloat (_MDInInterceptID,         itemID, 0.0) : 0.0;
	irrAreaFrac  = _MDInIrrigation_AreaFracID      != MFUnset ? MFVarGetFloat (_MDInIrrigation_AreaFracID,       itemID, 0.0) : 0.0;

	waterIn = precip - intercept - sPackChg;
	pet = pet > intercept ? pet - intercept : 0.0;

	if (awCap > 0.0) {
	    if (waterIn > pet) { sMoistChg = waterIn - pet < awCap - sMoist ? waterIn - pet : awCap - sMoist; }
	    else {
	        gm = (1.0 - exp (- _MDSoilMoistALPHA * sMoist / awCap)) / (1.0 - exp (- _MDSoilMoistALPHA));
	        sMoistChg = (waterIn - pet) * gm;
	    }
	    if (sMoist + sMoistChg > awCap) sMoistChg = awCap - sMoist;
	    if (sMoist + sMoistChg <   0.0) sMoistChg =       - sMoist;
	    sMoist = sMoist + sMoistChg;
	} else sMoist = sMoistChg = 0.0;

	evapotrans = pet + intercept < precip - sPackChg - sMoistChg ? pet + intercept : precip - sPackChg - sMoistChg;
	MFVarSetFloat (_MDOutSoilMoistCellID,     itemID, sMoist);
	MFVarSetFloat (_MDOutEvaptrsID,           itemID, evapotrans * (1.0 - irrAreaFrac)); //RJS 01-17-08 "- impAreaFrac - H2OAreaFrac"
	MFVarSetFloat (_MDOutSoilMoistID,         itemID, sMoist     * (1.0 - irrAreaFrac)); //RJS 01-17-08 "- impAreaFrac - H2OAreaFrac"
	MFVarSetFloat (_MDOutSMoistChgID,         itemID, sMoistChg  * (1.0 - irrAreaFrac)); //RJS 01-17-08 "- impAreaFrac - H2OAreaFrac"
	if (_MDOutLiquidSoilMoistureID != MFUnset) MFVarSetFloat (_MDOutLiquidSoilMoistureID,itemID, sMoist/awCap);
}

enum { MFnone, MFcalculate };

int MDCore_RainSMoistChgDef () {
	int ret = 0;
	float par;
	const char *optStr;
	if (_MDOutSMoistChgID != MFUnset) return (_MDOutSMoistChgID);
	const char *soilTemperatureOptions [] = { "none", "calculate", (char *) NULL };

	if (((optStr = MFOptionGet (MDParSoilMoistALPHA))  != (char *) NULL) && (sscanf (optStr,"%f",&par) == 1)) _MDSoilMoistALPHA = par;
	
	MFDefEntering ("Rainfed Soil Moisture");

	if ((ret = MDIrrigation_GrossDemandDef()) == CMfailed) return (CMfailed);
	if ((ret != MFUnset) && ((_MDInIrrigation_AreaFracID = MDIrrigation_IrrAreaDef()) == CMfailed) ) return (CMfailed);

	if (((_MDInCommon_PrecipID            = MDCommon_PrecipitationDef()) == CMfailed) ||
        ((_MDInSPackChgID          = MDCore_SnowPackChgDef()) == CMfailed) ||
        ((_MDInPotETID             = MDCore_RainPotETDef()) == CMfailed) ||
        ((_MDInInterceptID         = MDCore_RainInterceptDef()) == CMfailed) ||
        ((_MDInSoilAvailWaterCapID = MDCore_SoilAvailWaterCapDef()) == CMfailed) ||
        ((_MDInAirTemperatureID          = MFVarGetID (MDVarCommon_AirTemperature, "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutEvaptrsID          = MFVarGetID (MDVarCore_RainEvapotranspiration, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
        ((_MDOutSoilMoistCellID    = MFVarGetID (MDVarCore_RainSoilMoistureCell, "mm", MFOutput, MFState, MFInitial)) == CMfailed) ||
        ((_MDOutSoilMoistID        = MFVarGetID (MDVarCore_RainSoilMoisture, "mm", MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutSMoistChgID        = MFVarGetID (MDVarCore_RainSoilMoistChange, "mm", MFOutput, MFState, MFBoundary)) == CMfailed) ||
        (MFModelAddFunction (_MDRainSMoistChg) == CMfailed)) return (CMfailed);
	MFDefLeaving ("Rainfed Soil Moisture");
	return (_MDOutSMoistChgID);
}
