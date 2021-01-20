/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

MDIrrRefETFAO.c

dominink.wisser@unh.edu

*******************************************************************************/

#include <math.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInCommon_AtMeanID            = MFUnset;
static int _MDInAtMaxID             = MFUnset;
static int _MDInAtMinID             = MFUnset;
static int _MDInSolRadID            = MFUnset;
static int _MDInVPressID            = MFUnset;
static int _MDInWSpeedID            = MFUnset;
static int _MDInElevationID         = MFUnset;
static int _MDInDayLengthID         = MFUnset;
static int _MDInI0HDayID            = MFUnset;
// Output
static int _MDOutIrrRefEvapotransID = MFUnset;

static void _MDIrrRefEvapotransFAO (int itemID) {
/* day-night Penman-Monteith PE in mm for day */
// Input
//	float dayLen;  // daylength in fraction of day
 	float i0hDay;  //  daily potential insolation on horizontal [MJ/m2]

	float airT;    // air temperatur [degree C]
	float airTMin; // daily minimum air temperature [degree C]
	float airTMax; // daily maximum air temperature [degree C]
	float solRad;  // daily solar radiation on horizontal [MJ/m2]
	float vPress;  // daily average vapor pressure [kPa]
	float wSpeed;  // average wind speed for the day [m/s]
// Local
	float solNet;  // average net solar radiation for daytime [W/m2]
	float es;      // vapor pressure at airT [kPa]
	float es_min;  // vapor pressure at airT [kPa]
	float es_max;  // vapor pressure at airT [kPa]
	float delta;   // dEsat/dTair [kPa/K]
	float psychometricConstant;
	float FAOEtp;
	float solNet_MJm2d;
	float nom;
	float denom;
	float meanAirTemp;
	float atmosPressure;
	float elevation;
	float nen;
	float temp;
 
//	dayLen    = MFVarGetFloat (_MDInDayLengthID,    itemID,0);
	i0hDay    = MFVarGetFloat (_MDInI0HDayID,       itemID,0);
	elevation = MFVarGetFloat (_MDInElevationID,    itemID,0); 
	airT      = MFVarGetFloat (_MDInCommon_AtMeanID,       itemID,0);
	 
	solRad    = MFVarGetFloat (_MDInSolRadID,       itemID,0);
	vPress    = MFVarGetFloat (_MDInVPressID,       itemID,0);
	wSpeed    = fabs (MFVarGetFloat (_MDInWSpeedID, itemID,0));
	if (wSpeed < 0.2) wSpeed = 0.2;
	
	airTMin = MFVarGetFloat (_MDInAtMinID,        itemID,  0.0);
	airTMax = MFVarGetFloat (_MDInAtMaxID,        itemID,  0.0);
	
	atmosPressure = (293.0 - 0.0065 * elevation) / 293.0;
	atmosPressure = pow (atmosPressure, 5.26);
	atmosPressure = atmosPressure * 101.3;
	psychometricConstant = 0.665 * atmosPressure / 1000.0;

	meanAirTemp  = (airTMin + airTMax) / 2.0;
	solNet       = (1.0 - 0.23) * solRad ;// in MJ/m2 
	es_min       = MDPETlibVPressSat (airTMin);
 	es_max       = MDPETlibVPressSat (airTMax);
 	es           = (es_min + es_max) / 2.0;
	solNet_MJm2d = solNet;  // 0.0864; FAO equation wants SolNet in MJ/m2.
	nen          = 4098 * (0.6108 * exp (17.27 * airT / (airT + 237.3)));
	delta        = nen / ((airT + 237.3)*(airT + 237.3));

 	temp         = es-vPress;
	nom          = 0.408 * delta * solNet_MJm2d + psychometricConstant * 900 / (273.3 + airT) * wSpeed * temp; 
	//FBM nimmt vapor pressure in kPA!
	denom = delta + psychometricConstant*(1+0.34 * wSpeed);

	FAOEtp = nom / denom;
	MFVarSetFloat (_MDOutIrrRefEvapotransID,itemID,FAOEtp);
}

int MDIrrigation_ReferenceETFAODef () {
	if (_MDOutIrrRefEvapotransID != MFUnset) return (_MDOutIrrRefEvapotransID);

	MFDefEntering ("Irrigation Reference ETP (FAO)");
	if (((_MDInDayLengthID          = MDCommon_SolarRadDayLengthDef()) == CMfailed) ||
        ((_MDInI0HDayID            = MDCommon_SolarRadI0HDayDef()) == CMfailed) ||
        ((_MDInSolRadID            = MDCommon_SolarRadDef()) == CMfailed) ||
        ((_MDInElevationID         = MFVarGetID (MDVarCommon_MeanElevation, "m", MFInput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDInCommon_AtMeanID            = MFVarGetID (MDVarCommon_AirTemperature, "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDInAtMinID             = MFVarGetID (MDVarCommon_AirTempMinimum, "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDInAtMaxID             = MFVarGetID (MDVarCommon_AirTempMaximum, "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDInVPressID            = MFVarGetID (MDVarCore_VaporPressure, "kPa", MFInput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDInWSpeedID            = MFVarGetID (MDVarCommon_WindSpeed, "m/s", MFInput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutIrrRefEvapotransID = MFVarGetID (MDVarIrrigation_RefEvapotrans, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed)) return (CMfailed);
    if (MFModelAddFunction (_MDIrrRefEvapotransFAO)== CMfailed) return (CMfailed);

	MFDefLeaving ("Irrigation Reference ETP (FAO)");
	return(_MDOutIrrRefEvapotransID);
}
