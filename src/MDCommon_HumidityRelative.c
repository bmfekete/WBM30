/******************************************************************************
 GHAAS Water Balance/Transport Model
 Global Hydrologic Archive and Analysis System
 Copyright 1994-2021, UNH - ASRC/CUNY

 MDCommon_HumidityRelative.c

 amiara@ccny.cuny.edu

 *******************************************************************************/

/********************************************************************************
 * Calculates Specific Humidity from pressure, air temperature and rh
 * ******************************************************************************/

#include <math.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInCommon_AirTemperatureID    = MFUnset;
static int _MDInCommon_HumiditySpecificID  = MFUnset;
static int _MDInCommon_AirPressureID       = MFUnset;
// Output
static int _MDOutCommon_HumidityRelativeID = MFUnset;

static void _MDRelativeHumidity (int itemID) {
    float airtemp;
    float sh;
    float airpressure;
    float es;
    float e;
    float relativehumidity;
    float v1;
    float v2;

    airtemp     = MFVarGetFloat (_MDInCommon_AirTemperatureID,   itemID, 0.0) + 273.16;
    airpressure = MFVarGetFloat (_MDInCommon_AirPressureID,      itemID, 0.0); //pressure (Pa)
    sh          = MFVarGetFloat (_MDInCommon_HumiditySpecificID, itemID, 0.0); // jan27 2017 - divide by 100 for NCEP

    v1 = exp( (17.67 * (airtemp - 273.16) / (airtemp - 29.65) ) );
    v2 = 0.263 * airpressure;
    relativehumidity = (sh * v2)/v1;
    MFVarSetFloat(_MDOutCommon_HumidityRelativeID, itemID, relativehumidity);
}

enum { MDnone, MDinput, MDcalculate };

int MDCommon_HumidityRelativeDef () {
    int optID = MFUnset;
    const char *optStr, *optName = MDOptWeather_RelativeHumidity;
    const char *options [] = { MDNoneStr, MDInputStr, MDCalculateStr, (char *) NULL};
    
    if ((optStr = MFOptionGet(optName)) != (char *) NULL) optID = CMoptLookup(options, optStr, true);
    if ((optID == MDnone) || (_MDOutCommon_HumidityRelativeID != MFUnset)) return (_MDOutCommon_HumidityRelativeID);
    
    MFDefEntering ("RelativeHumidity");
    
    switch (optID) {
        case MDinput:
            if      ((_MDOutCommon_HumidityRelativeID = MFVarGetID (MDVarCommon_HumidityRelative, "degC", MFInput, MFState, MFBoundary)) == CMfailed) return (CMfailed);
            break;
        case MDcalculate:
            if (((_MDInCommon_AirTemperatureID    = MFVarGetID (MDVarCommon_AirTemperature,   "degC", MFInput,  MFState, MFBoundary)) == CMfailed) ||
                ((_MDInCommon_AirPressureID       = MFVarGetID (MDVarCommon_AirPressure,      "kPa",  MFInput,  MFState, MFBoundary)) == CMfailed) ||
                ((_MDInCommon_HumiditySpecificID  = MFVarGetID (MDVarCommon_HumiditySpecific, "%",    MFInput,  MFState, MFBoundary)) == CMfailed) ||
                ((_MDOutCommon_HumidityRelativeID = MFVarGetID (MDVarCommon_HumidityRelative, "%",    MFOutput, MFState, MFBoundary)) == CMfailed) ||
                ((MFModelAddFunction (_MDRelativeHumidity) == CMfailed))) return (CMfailed);
            break;
        default: MFOptionMessage (optName, optStr, options);
            return (CMfailed);
    }
    MFDefLeaving ("RelativeHumidity");
    return (_MDOutCommon_HumidityRelativeID);
}
