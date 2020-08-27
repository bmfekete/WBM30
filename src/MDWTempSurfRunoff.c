/******************************************************************************

GHAAS Water Balance/Transport Model V2.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDTempSurfRunoff.c

wil.wollheim@unh.edu

EDITED: amiara@ccny.cuny.edu Sep 2016

This module calculates the temperature of surface runoff and infiltration to groundwater
Irrigation inputs are not accounted here.

*******************************************************************************/

#include <math.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInAirTempID  = MFUnset;
static int _MDInPrecipID   = MFUnset;
static int _MDInSnowMeltID = MFUnset;
static int _MDInSnowPackID = MFUnset;
static int _MDInWetBulbTempID = MFUnset;

// Output
static int _MDOutWTempSurfROID   = MFUnset;

static void _MDWTempSurfRunoff (int itemID) {
	float airT;
	float SnowT = 0;
	float SurfWatT;
	float SnowPack;
	float SnowIn;
	float wet_b_temp;
	float PrecipIn;


    wet_b_temp         = MFVarGetFloat (_MDInWetBulbTempID,     itemID, 0.0);
    airT               = MFVarGetFloat (_MDInAirTempID,         itemID, 0.0);
    SnowPack           = MFVarGetFloat (_MDInSnowPackID,        itemID, 0.0);
    SnowIn             = MFVarGetFloat (_MDInSnowMeltID,        itemID, 0.0);
    PrecipIn           = MFVarGetFloat (_MDInPrecipID,          itemID, 0.0);

//    if (!isnan(SnowIn) && !isnan(PrecipIn) && !isnan(SnowT) && !isnan(airT) && (SnowIn + PrecipIn) > 0.0){
//        if (SnowPack > 0) {SurfWatT = 0;}
//        else{
//    	SurfWatT = MDMaximum((SnowIn * SnowT + PrecipIn * airT) / (SnowIn + PrecipIn), 0.0);
//        }
//    	MFVarSetFloat (_MDOutWTempSurfROID,itemID,SurfWatT);
//    }
//    else{
//  	MFVarSetMissingVal(_MDOutWTempSurfROID,itemID);		// RJS 062011, commented out because it provided extremely high temperatures
//  	    SurfWatT = 15;										// RJS 062011, Need to assign SOME temperature to RO, but does not matter what temperature, as runoff == 0.0.
	//		printf("*** else, airT = %f, SnowPack = %f, SnowIn = %f, PrecipIn = %f\n", airT, SnowPack, SnowIn, PrecipIn);								// RJS 062011
//    	MFVarSetFloat (_MDOutWTempSurfROID, itemID, SurfWatT);	// RJS 062011, added so that when SnowIn + PrecipIn = 0, temperatures are not extremely high.  15 degrees was selected randomly, should not matter.
//    }

SurfWatT = wet_b_temp; // CHANGED TO EQUAL WET BULB --> Feb 22 2019 MIARA
SurfWatT = (SurfWatT >= airT) ? wet_b_temp : SurfWatT;

//if (SurfWatT > 0){
//printf("wet_b_temp = %f, SurfWatT =%f, airT = %f \n", wet_b_temp, SurfWatT, airT);
//}

MFVarSetFloat (_MDOutWTempSurfROID, itemID, SurfWatT);

}

int MDWTempSurfRunoffDef () {

	if (_MDOutWTempSurfROID != MFUnset) return (_MDOutWTempSurfROID);

	MFDefEntering ("Surface runoff temperature");

	if (((_MDInPrecipID       = MDPrecipitationDef ()) == CMfailed) ||
	    ((_MDInSnowMeltID     = MDSPackMeltDef     ()) == CMfailed) ||
	    ((_MDInWetBulbTempID  = MDWetBulbTempDef())    == CMfailed) ||
	    ((_MDInSnowPackID     = MFVarGetID (MDVarSnowPack,        "mm",   MFInput,  MFState, MFInitial))  == CMfailed) ||
	    ((_MDInAirTempID      = MFVarGetID (MDVarAirTemperature,  "degC", MFInput,  MFState, MFBoundary)) == CMfailed) ||
//            ((_MDWetBulbTempID    = MFVarGetID (MDVarWetBulbTemp,     "degC", MFInput,  MFState,  MFBoundary)) == CMfailed) ||
	    ((_MDOutWTempSurfROID = MFVarGetID (MDVarWTempSurfRunoff, "degC", MFOutput, MFState, MFBoundary)) == CMfailed) ||
        (MFModelAddFunction (_MDWTempSurfRunoff) == CMfailed)) return (CMfailed);

	MFDefLeaving ("Surface runoff temperature");
	return (_MDOutWTempSurfROID);
}
