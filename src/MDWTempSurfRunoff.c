/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2014, UNH - CCNY/CUNY

MDTempSurfRunoff.c

wil.wollheim@unh.edu

This module calculates the temperature of surface runoff and infiltration to groundwater
Irrigation inputs are not accounted here.

*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>
#include <math.h>

// Input
static int _MDInAirTempID  = MFUnset;
static int _MDInPrecipID   = MFUnset;
static int _MDInSnowMeltID = MFUnset;
static int _MDInSnowPackID = MFUnset;

// Output
static int _MDOutWTempSurfROID   = MFUnset;

static void _MDWTempSurfRunoff (int itemID) {
	float airT;
	float SnowT = 0;
	float SurfWatT;
	float SnowPack;

	float SnowIn;
	float PrecipIn;


    airT               = MFVarGetFloat (_MDInAirTempID,         itemID, 0.0);
    SnowPack           = MFVarGetFloat (_MDInSnowPackID,        itemID, 0.0);
    SnowIn             = MFVarGetFloat (_MDInSnowMeltID,        itemID, 0.0);
    PrecipIn           = MFVarGetFloat (_MDInPrecipID,          itemID, 0.0);

    if (!isnan(SnowIn) && !isnan(PrecipIn) && !isnan(SnowT) && !isnan(airT) && (SnowIn + PrecipIn) > 0.0){
        if (SnowPack > 0) {SurfWatT = 0;}
        else{
    	SurfWatT = MDMaximum((SnowIn * SnowT + PrecipIn * airT) / (SnowIn + PrecipIn), 0.0);
        }
    	MFVarSetFloat (_MDOutWTempSurfROID,itemID,SurfWatT);
    }
    else{
//  	MFVarSetMissingVal(_MDOutWTempSurfROID,itemID);		// RJS 062011, commented out because it provided extremely high temperatures
  	    SurfWatT = 15;										// RJS 062011, Need to assign SOME temperature to RO, but does not matter what temperature, as runoff == 0.0.
	//		printf("*** else, airT = %f, SnowPack = %f, SnowIn = %f, PrecipIn = %f\n", airT, SnowPack, SnowIn, PrecipIn);								// RJS 062011
    	MFVarSetFloat (_MDOutWTempSurfROID, itemID, SurfWatT);	// RJS 062011, added so that when SnowIn + PrecipIn = 0, temperatures are not extremely high.  15 degrees was selected randomly, should not matter.
    }
    //if (airT > 25){
   // 	printf("SurfWaterT = SurfWatT %f \n", SurfWatT);
   // }

//	  if (SurfWatT > 1000) printf("m = %d, d = %d, itemID = %d, SurfWatT = %f, airT = %f, SnowIn = %f, SnowT = %f, PrecipIn = %f, SnowPack = %f\n", MFDateGetCurrentMonth (), MFDateGetCurrentDay (), itemID, SurfWatT, airT, SnowIn, SnowT, PrecipIn, SnowPack);


}

int MDWTempSurfRunoffDef () {

	if (_MDOutWTempSurfROID != MFUnset) return (_MDOutWTempSurfROID);

	MFDefEntering ("Surface runoff temperature");

	if (((_MDInPrecipID       = MDPrecipitationDef ()) == CMfailed) ||
	    ((_MDInSnowMeltID     = MDSPackMeltDef     ()) == CMfailed) ||
	    ((_MDInSnowPackID     = MFVarGetID (MDVarSnowPack,        "mm",   MFInput,  MFState, MFInitial))  == CMfailed) ||
	    ((_MDInAirTempID      = MFVarGetID (MDVarAirTemperature,  "degC", MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutWTempSurfROID = MFVarGetID (MDVarWTempSurfRunoff, "degC", MFOutput, MFState, MFBoundary)) == CMfailed) ||
        (MFModelAddFunction (_MDWTempSurfRunoff) == CMfailed)) return (CMfailed);

	MFDefLeaving ("Surface runoff temperature");
	return (_MDOutWTempSurfROID);
}
