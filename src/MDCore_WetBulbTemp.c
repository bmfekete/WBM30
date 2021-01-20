/******************************************************************************
NIMA EHSANI
GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

MDWetBulbTemp.c

nehsani@ccny.cuny.edu
brosenzweig@ccny.cuny.edu
amiara@ccny.cuny.edu

 *******************************************************************************/
/********************************************************************************
 * Calculates wet-bulb temperature from temperature, pressure and specific
humidity using the approximation described in Chappell, et al 1973. 
This approximation is used as a first guess and the tradition Clausius- 
Clapeyron solution using Newton-Raphson Iteration Method is applied for 
better results.  If this solution does not converge,the Chappell 
approximation is returned. 
 * ******************************************************************************/

#include <math.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInCommon_AirTemperatureID = MFUnset;
static int _MDInCommon_HumiditySpecificID = MFUnset;
static int _MDInCommon_AirPressureID = MFUnset;
static int _MDInCommon_HumidityRelativeID = MFUnset;

// Output
static int _MDOutWetBulbTempID = MFUnset;

static void _MDWetBulbTemp(int itemID) {
    float relativehumidity;
    float airtemp;
    float specifichumidity;
    float airpressure;
    float wetbulbtemp;
    float eps = 0.1; //threshold value for iteration 
    float cp = 1005; //specific heat of air (J/(kg*K)
    float e; //e (HPa)
    float es; //Sat. vapor press. (hPa) 
    float w; //Mixing ratio from specific humidity (kg/kg)
    float ws; //Mixing ratio at saturation (kg/kg)
    float td; //dewpoint (Deg. C)
    float a; //Dewpoint depression
    float b; //Mixing ratio depression
    float c;
    float twn; //Chappell Approximation for Wet Bulb Temperature
    float twn2;
    float lv; //latent heat of vaporization (J/kg)
    float firstsol;
    float delta;
    float Q;
    float S;
    float pl1;
    float pl2;
    float pl3;
    float pl4;
    float pl5;
    float pl6;
    int x;
    float esw; //Sat. vapor press. (hPa) at Twn

    relativehumidity = MFVarGetFloat(_MDInCommon_HumidityRelativeID, itemID, 0.0);
    airtemp          = MFVarGetFloat(_MDInCommon_AirTemperatureID,   itemID, 0.0);
    airpressure      = MFVarGetFloat(_MDInCommon_AirPressureID,      itemID, 0.0) / 100; //pressure (HPa)
    specifichumidity = MFVarGetFloat(_MDInCommon_HumiditySpecificID, itemID, 0.0) * 1000; //specific humidity (g/kg) took out *1000 jan27 2017 (divide by 100 for NCEP)

    e  = (specifichumidity * airpressure) / 622; //e (HPa)
    es = 6.112 * exp(17.27 * (airtemp / (237.3 + airtemp))); //Sat. vapor press. (hPa) 

    if (e >= es) {//at or above saturation, wetbulbtemp=airtemp
        wetbulbtemp = airtemp;
    } else {
        if (airtemp == 32800 || airpressure == 32800 || specifichumidity == 32800) { //missing data
            wetbulbtemp = 32800;
        } else {
            w   = specifichumidity / (1000) / (1 - (specifichumidity / 1000)); //Mixing ratio from specific humidity (kg/kg)
            ws  = 0.62197 * (es / (airpressure - es)); //Mixing ratio at saturation (kg/kg)
            td  = (243.5 * log(exp(1) / 6.11)) / (17.67 - log(exp(1) / 6.11)); //dewpoint (Deg. C)
            a   = (airtemp - td); //Dewpoint depression
            b   = (ws - w); //Mixing ratio depression
            c   = (597.3 - 0.566 * (airtemp - 273.16)) / 0.24;
            twn = airtemp - ((a * b * c) / (a + b * c)); //Chappell Approximation for Wet Bulb Temperature

            //Iterate for a better solution
            w = specifichumidity / (1000) / (1 - (specifichumidity / 1000)); //Mixing ratio from specific humidity (kg/kg)
            lv = 1918460*pow(((airtemp + 273) / ((airtemp + 273) - 33.91)),2); //latent heat of vaporization (J/kg)
            firstsol = twn;
            delta = eps + 1;

            for (x = 0; x < 10; x++) {
                esw = 611 * exp(17.27 * (twn / (237.3 + twn))) / 100; //Sat. vapor press. (hPa) at Twn
                ws = 0.62197 * (esw / (airpressure - esw)); //dimensionless mixing ratio at saturation
                twn2 = (twn + airtemp + lv / cp * (w - ws)) / 2; //average with prev. value for stability
                delta = fabs(twn2 - twn);
                if (delta <= eps) {
                    break;
                } else {
                    if (x == 9) { //%alternate solution for nonconvergence
                        //twn2 = firstsol; //Return the Chappell approximation Deleted by Ariel in place of method in Adalberto Tejeda Martinez 1994
			        S = 662.23 + (0.97 * airpressure);
			        Q = 8264.65 - ((1480.45 * (relativehumidity / 100)) * es) - (0.966 * airtemp * airpressure);
			        pl1=pow(Q,2) / 4;
			        pl2=pow(S,3) / 27;
			        pl4=cbrt((-Q/2.0)+(pow((pl1+pl2),0.5)));
			        pl5=cbrt((-Q/2.0)-(pow((pl1+pl2),0.5)));
			        twn2 = pl4 + pl5 - 1.0;
                    } else {
                        twn = twn2;
                    }
                }
            }
            wetbulbtemp = twn2; //Return Final Wet Bulb Temperature
        }
    }
    wetbulbtemp = (airtemp < 0.0) ? 0.0 : wetbulbtemp;
    MFVarSetFloat(_MDOutWetBulbTempID, itemID, wetbulbtemp);
}

enum { MDnone, MDinput, MDcalculate};

int MDCommon_WetBulbTempDef () {
    int optID = MFUnset;
    const char *optStr, *optName = MDOptWeather_WetBulbTemp;
    const char *options [] = { MDNoneStr, MDInputStr, MDCalculateStr, (char *) NULL};

    if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
    if ((optID == MDnone) || (_MDOutWetBulbTempID != MFUnset)) return (_MDOutWetBulbTempID);

    MFDefEntering("WetBulbTemp");

    switch (optID) {
        case MDinput:
            if ((_MDOutWetBulbTempID = MFVarGetID (MDVarCommon_WetBulbTemp, "degC", MFInput, MFState, MFBoundary)) == CMfailed) return (CMfailed);
            break;
        case MDcalculate:
            if (((_MDInCommon_HumiditySpecificID = MDCommon_HumiditySpecificDef()) == CMfailed) ||
                ((_MDInCommon_HumidityRelativeID = MDCommon_HumidityRelativeDef()) == CMfailed) ||
                ((_MDInCommon_AirTemperatureID   = MFVarGetID (MDVarCommon_AirTemperature, "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
                ((_MDInCommon_AirPressureID      = MFVarGetID (MDVarCommon_AirPressure, "kPa", MFInput, MFState, MFBoundary)) == CMfailed) ||
                ((_MDOutWetBulbTempID     = MFVarGetID (MDVarCommon_WetBulbTemp, "degC", MFOutput, MFState, MFBoundary)) == CMfailed) ||
                ((MFModelAddFunction (_MDWetBulbTemp) == CMfailed))) return (CMfailed);
            break;
        default: MFOptionMessage (optName, optStr, options); return (CMfailed);
    }
    MFDefLeaving ("WetBulbTemp");
    return (_MDOutWetBulbTempID);
}
