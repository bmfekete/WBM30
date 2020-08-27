/******************************************************************************

GHAAS Water Balance/Transport Model V2.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDSPackChg.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <math.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInAtMeanID    = MFUnset;
static int _MDInPrecipID    = MFUnset;
static int _MDInWinterOnsetID = MFUnset;

// Output
static int _MDOutSnowPackID = MFUnset;
static int _MDOutSPackChgID = MFUnset;
static int _MDOutSnowMeltID = MFUnset;
static int _MDOutSnowFallID = MFUnset;
static int _MDOutSnowDensityID = MFUnset;
static int _MDOutSnowDepthID = MFUnset;
static int _MDCalculateSoilTempID = MFUnset;
static float _MDSnowMeltThreshold = 1.0;
static float _MDFallThreshold = -1.0;

static void _MDSPackChg (int itemID) {
// Input
	float airT;
	float precip;
	float winterOnsetDoY;
	float initialDensity = 150;
	float sDensitySlope = 3;
	if( _MDCalculateSoilTempID==1)winterOnsetDoY = MFVarGetFloat(_MDInWinterOnsetID, itemID,1.0);

	// Local
	float sPack;
	float sPackChg = 0.0;
	float sDensity=0.0;
	float sDepth = 0.0;
	int snowAge=0;
	float densityOfWater =1000;
	if (MFDateGetDayOfYear()  - winterOnsetDoY > 0){
	snowAge = MFDateGetDayOfYear() - winterOnsetDoY;
	}else{
		snowAge = 365 - winterOnsetDoY + MFDateGetDayOfYear();
	}
	
	sPack  = MFVarGetFloat (_MDOutSnowPackID, itemID, 0.0);
	if (MFVarTestMissingVal (_MDInAtMeanID,itemID) || MFVarTestMissingVal (_MDInPrecipID, itemID)) {
		MFVarSetFloat (_MDOutSnowFallID, itemID, 0.0);
		MFVarSetFloat (_MDOutSnowMeltID, itemID, 0.0);
		MFVarSetFloat (_MDOutSnowPackID, itemID, sPack);	
		MFVarSetFloat (_MDOutSPackChgID, itemID, 0.0);
		return; 
	}

	airT   = MFVarGetFloat (_MDInAtMeanID,    itemID, 0.0);
	precip = MFVarGetFloat (_MDInPrecipID,    itemID, 0.0);

	if (airT < _MDFallThreshold) {  /* Accumulating snow pack */
		MFVarSetFloat (_MDOutSnowFallID, itemID, precip);
		MFVarSetFloat (_MDOutSnowMeltID, itemID, 0.0);
		MFVarSetFloat (_MDOutSnowPackID, itemID, sPack + precip);
		MFVarSetFloat (_MDOutSPackChgID, itemID, precip);
	}
	else if (airT > _MDSnowMeltThreshold) { /* Melting snow pack */
		sPackChg = 2.63 + 2.55 * airT + 0.0912 * airT * precip;
		sPackChg = - (sPack < sPackChg ? sPack : sPackChg);
		MFVarSetFloat (_MDOutSnowFallID, itemID, 0.0);
		MFVarSetFloat (_MDOutSnowMeltID, itemID, fabs(sPackChg));
		MFVarSetFloat (_MDOutSPackChgID, itemID, sPackChg);
		MFVarSetFloat (_MDOutSnowPackID, itemID, sPack + sPackChg);
	}
	else { /* No change when air temperature is in [-1.0,1.0] range */
		MFVarSetFloat (_MDOutSnowFallID, itemID, 0.0);
		MFVarSetFloat (_MDOutSnowMeltID, itemID, 0.0);
		MFVarSetFloat (_MDOutSnowPackID, itemID, sPack);	
		MFVarSetFloat (_MDOutSPackChgID, itemID, 0.0);
	}

		sDensity = (initialDensity + (snowAge * sDensitySlope));
		if (sPack > 0.0 ) sDepth = sPack  * densityOfWater / sDensity; //in mm

		MFVarSetFloat(_MDOutSnowDensityID,itemID,sDensity);  
		MFVarSetFloat(_MDOutSnowDepthID,itemID, sDepth); 
}

int MDSPackChgDef () {

	if (_MDOutSPackChgID != MFUnset) return (_MDOutSPackChgID);
	MFDefEntering ("Snow Pack Change");
	const char *optStr;
	const char *soilTemperatureOptions [] = { "none", "calculate", (char *) NULL };
	int soilTemperatureID;
	float par;
	if (((optStr = MFOptionGet (MDParSnowMeltThreshold))  != (char *) NULL) && (sscanf (optStr,"%f",&par) == 1))
		_MDSnowMeltThreshold = par;
	
	if (((optStr = MFOptionGet (MDParFallThreshold))  != (char *) NULL) && (sscanf (optStr,"%f",&par) == 1))
		_MDFallThreshold= par;

	if (((optStr = MFOptionGet (MDOptSoilTemperature))  == (char *) NULL) || ((soilTemperatureID = CMoptLookup (soilTemperatureOptions, optStr, true)) == CMfailed)) {
					CMmsgPrint(CMmsgUsrError," Soil TemperatureOption not specifed! Options = 'none' or 'calculate'\n");
					return CMfailed;
				}

	if (soilTemperatureID == 1 ){
		_MDCalculateSoilTempID=1;
		if ((_MDInWinterOnsetID    = MFVarGetID (MDVarWinterOnsetDoy, "DoY", MFInput,  MFState, MFBoundary)) == CMfailed) return CMfailed;
	}

	if (((_MDInPrecipID       = MDPrecipitationDef ()) == CMfailed) ||
	    ((_MDInAtMeanID       = MFVarGetID (MDVarAirTemperature, "degC", MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutSnowFallID    = MFVarGetID (MDVarSnowFall,       "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDOutSnowMeltID    = MFVarGetID (MDVarSnowMelt,       "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDOutSnowDensityID = MFVarGetID (MDVarSnowDensity,    "mm",   MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutSnowDepthID   = MFVarGetID (MDVarSnowDepth,      "mm",   MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutSnowPackID    = MFVarGetID (MDVarSnowPack,       "mm",   MFOutput, MFState, MFInitial))  == CMfailed) ||
	    ((_MDOutSPackChgID    = MFVarGetID (MDVarSnowPackChange, "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    (MFModelAddFunction (_MDSPackChg) == CMfailed)) return (CMfailed);
	MFDefLeaving ("Snow Pack Change");
	return (_MDOutSPackChgID);
}

int MDSPackMeltDef () {

	if (_MDOutSnowMeltID != MFUnset) return (_MDOutSnowMeltID);

	if ((MDSPackChgDef () == CMfailed) ||
	    ((_MDOutSnowMeltID   = MFVarGetID (MDVarSnowMelt, "mm", MFInput, MFFlux, MFBoundary)) == CMfailed))
		return (CMfailed);
	return (_MDOutSnowMeltID);
}
