/******************************************************************************

GHAAS Water Balance/Transport Model V2.0
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
static int _MDInAirTMeanID            = MFUnset;
static int _MDInPrecipID              = MFUnset;
static int _MDInPotETID               = MFUnset;
static int _MDInInterceptID           = MFUnset;
static int _MDInSPackChgID            = MFUnset;
static int _MDInSoilAvailWaterCapID   = MFUnset;
static int _MDInIrrAreaFracID         = MFUnset;
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
	airT         = MFVarGetFloat (_MDInAirTMeanID,          itemID, 0.0);
	precip       = MFVarGetFloat (_MDInPrecipID,            itemID, 0.0);
 	sPackChg     = MFVarGetFloat (_MDInSPackChgID,          itemID, 0.0);
	pet          = MFVarGetFloat (_MDInPotETID,             itemID, 0.0);
	awCap        = MFVarGetFloat (_MDInSoilAvailWaterCapID, itemID, 0.0);
	sMoist       = MFVarGetFloat (_MDOutSoilMoistCellID,    itemID, 0.0);
	iceContent   = _MDInRelativeIceContent != MFUnset ? MFVarGetFloat (_MDInRelativeIceContent,  itemID, 0.0) : 0.0;
	intercept    = _MDInInterceptID        != MFUnset ? MFVarGetFloat (_MDInInterceptID,         itemID, 0.0) : 0.0;
	irrAreaFrac  = _MDInIrrAreaFracID      != MFUnset ? MFVarGetFloat (_MDInIrrAreaFracID,       itemID, 0.0) : 0.0;

	
	//if (iceContent> 0.0) printf ("IceContent upper Layer = %f\n",iceContent);
	
	//reduce available water capacity by ice content
//	awCap= awCap - (awCap * iceContent);
	
//	if (airT > 0.0) {
		waterIn = precip - intercept - sPackChg;
		pet = pet > intercept ? pet - intercept : 0.0;

		if (awCap > 0.0) {
			if (waterIn > pet) {
				sMoistChg = waterIn - pet < awCap - sMoist ? waterIn - pet : awCap - sMoist;
			}
			else {
				gm = (1.0 - exp (- _MDSoilMoistALPHA * sMoist / awCap)) / (1.0 - exp (- _MDSoilMoistALPHA));
				sMoistChg = (waterIn - pet) * gm;
			}
			if (sMoist + sMoistChg > awCap) sMoistChg = awCap - sMoist;
			if (sMoist + sMoistChg <   0.0) sMoistChg =       - sMoist;
			sMoist = sMoist + sMoistChg;

		}
		else sMoist = sMoistChg = 0.0;

		evapotrans = pet + intercept < precip - sPackChg - sMoistChg ?
		             pet + intercept : precip - sPackChg - sMoistChg;	
//	}
//	else  { sMoistChg = 0.0; evapotrans = 0.0; }

//		if (itemID == 25014) printf("! sMoist cell = %f, sMoist = %f, evapotrans = %f, sMoistChg = %f, pet = %f, awCap = %f, waterIn = %f\n", sMoist, sMoist * (1.0 - irrAreaFrac), evapotrans, sMoistChg, pet, awCap, waterIn);
//		if (itemID == 25014) printf("! precip = %f, intercept = %f, sPackChg = %f\n", precip, intercept, sPackChg);

	MFVarSetFloat (_MDOutSoilMoistCellID,     itemID, sMoist);
	MFVarSetFloat (_MDOutEvaptrsID,           itemID, evapotrans * (1.0 - irrAreaFrac)); //RJS 01-17-08 "- impAreaFrac - H2OAreaFrac"
	MFVarSetFloat (_MDOutSoilMoistID,         itemID, sMoist     * (1.0 - irrAreaFrac)); //RJS 01-17-08 "- impAreaFrac - H2OAreaFrac"
	MFVarSetFloat (_MDOutSMoistChgID,         itemID, sMoistChg  * (1.0 - irrAreaFrac)); //RJS 01-17-08 "- impAreaFrac - H2OAreaFrac"
	if (_MDOutLiquidSoilMoistureID != MFUnset) MFVarSetFloat (_MDOutLiquidSoilMoistureID,itemID, sMoist/awCap);
}

enum { MFnone, MFcalculate };

int MDRainSMoistChgDef () {
	int ret = 0;
	float par;
	const char *optStr;
	if (_MDOutSMoistChgID != MFUnset) return (_MDOutSMoistChgID);
	const char *soilTemperatureOptions [] = { "none", "calculate", (char *) NULL };

	int soilTemperatureID;
	if (((optStr = MFOptionGet (MDOptSoilTemperature))  == (char *) NULL) ||
	    ((soilTemperatureID = CMoptLookup (soilTemperatureOptions, optStr, true)) == CMfailed)) {
		CMmsgPrint(CMmsgUsrError," Soil TemperatureOption not specifed! Options = 'none' or 'calculate'\n");
		return CMfailed;
	}
	if (((optStr = MFOptionGet (MDParSoilMoistALPHA))  != (char *) NULL) && (sscanf (optStr,"%f",&par) == 1)) _MDSoilMoistALPHA = par;
	
	MFDefEntering ("Rainfed Soil Moisture");
	if (soilTemperatureID == MFcalculate ) {
		if (((ret                        = MDPermafrostDef()) == CMfailed) ||
		    ((_MDInRelativeIceContent    = MFVarGetID ("SoilIceContent_01",     "mm",   MFOutput,  MFState, MFBoundary)) == CMfailed) ||
			((_MDOutLiquidSoilMoistureID = MFVarGetID (MDVarLiquidSoilMoisture, "-",    MFOutput,  MFState, MFBoundary)) == CMfailed)) return CMfailed;
	}

	if ((ret = MDIrrGrossDemandDef ()) == CMfailed) return (CMfailed);
	if ((ret != MFUnset) && ((_MDInIrrAreaFracID = MDIrrigatedAreaDef ())==  CMfailed) ) return (CMfailed);

	if (((_MDInPrecipID            = MDPrecipitationDef     ()) == CMfailed) ||
	    ((_MDInSPackChgID          = MDSPackChgDef          ()) == CMfailed) ||
	    ((_MDInPotETID             = MDRainPotETDef         ()) == CMfailed) ||
	    ((_MDInInterceptID         = MDRainInterceptDef     ()) == CMfailed) ||
	    ((_MDInSoilAvailWaterCapID = MDSoilAvailWaterCapDef ()) == CMfailed) ||
	    ((_MDInAirTMeanID          = MFVarGetID (MDVarAirTemperature,             "degC", MFInput,  MFState, MFBoundary)) == CMfailed) ||
 	    ((_MDOutEvaptrsID          = MFVarGetID (MDVarRainEvapotranspiration,     "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
 	  	((_MDOutSoilMoistCellID    = MFVarGetID (MDVarRainSoilMoistureCell,       "mm",   MFOutput, MFState, MFInitial))  == CMfailed) ||
	    ((_MDOutSoilMoistID        = MFVarGetID (MDVarRainSoilMoisture,           "mm",   MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutSMoistChgID        = MFVarGetID (MDVarRainSoilMoistChange,        "mm",   MFOutput, MFState, MFBoundary)) == CMfailed) ||
        (MFModelAddFunction (_MDRainSMoistChg) == CMfailed)) return (CMfailed);
	MFDefLeaving ("Rainfed Soil Moisture");
	return (_MDOutSMoistChgID);
}
