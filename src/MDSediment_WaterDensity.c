/******************************************************************************
Water density as function of temperature and concentration
McCutcheon, S.C., Martin, J.L, Barnwell, T.O. Jr. 1993. Water 
Quality in Maidment, D.R. (Editor). Handbood of Hydrology, 
McGraw-Hill, New York, NY (p. 11.3 )

Water density as a function of temperature only
rho = density in kg/m^3 as a function of temperature
T = temperature in C
rho = 1000(1 - (T+288.9414)/(508929.2*(T+68.12963))*(T-3.9863)^2)

 
MDWaterDensity
Aug 2013
sagy.cohen@as.ua.edu
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>
#include <math.h>
#include <stdlib.h>

// Input
static int _MDWTempRiverRouteID = MFUnset;
static int _MDInSedimentFluxID  = MFUnset;
static int _MDInQsConcID 		= MFUnset;
static int _MDInWTemp_QxTID 	= MFUnset;
static int _MDInUpStreamWdID 	= MFUnset;
// Output
static int _MDOutWaterDensityID = MFUnset;
static int _MDOutDeltaWdID		= MFUnset;

static void _MDWaterDensity (int itemID) {
	float Qc, rho, T, Wd,deltaWd,upstream_Wd;

	
//Geting the suspended sediment concentration (from MDSedimentFlux.c)
	Qc = MFVarGetFloat (_MDInQsConcID, 	itemID, 0.0);	// in kg/m3	
	T = MFVarGetFloat (_MDInWTemp_QxTID, 	itemID, 0.0);   // in C
	
	if (Qc < 0.0 ){
		rho = -9999;
	} else {	
	rho = 1000*(1 - (T+288.9414)/(508929.2*(T+68.12963))* pow((T-3.9863),2));
	Wd = rho + Qc;
	}

//returning the value to pixel
//printf("PPpc:%f\n",PPpc);
	MFVarSetFloat (_MDOutWaterDensityID, itemID, Wd);	
	
/// Calculate Wd budget
	upstream_Wd =  MFVarGetFloat (_MDInUpStreamWdID,itemID, 0.0);
	deltaWd = upstream_Wd - Wd; //local  budget
	MFVarSetFloat (_MDOutDeltaWdID, itemID, deltaWd); 
	MFVarSetFloat (_MDInUpStreamWdID , itemID, (upstream_Wd*-1));
	MFVarSetFloat (_MDInUpStreamWdID , itemID, Wd); 
}

enum { MDinput, MDcalculate, MDcorrected };

int MDSediment_WaterDensityDef() {
	
	MFDefEntering ("WaterDensity");
	
	if (((_MDInSedimentFluxID  = MDSediment_FluxDef ())  == CMfailed) || 
	    ((_MDWTempRiverRouteID = MDTP2M_WTempRiverDef ()) == CMfailed) ||
	    ((_MDInQsConcID        = MFVarGetID (MDVarSediment_QsConc,       "kg/m3", MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInWTemp_QxTID     = MFVarGetID (MDVarTP2M_Temp_QxT,         "degC",  MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutWaterDensityID = MFVarGetID (MDVarSediment_WaterDensity, "degC",  MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInUpStreamWdID    = MFVarGetID (MDVarSediment_UpStreamWd,   "degC",  MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutDeltaWdID      = MFVarGetID (MDVarSediment_DeltaWd,      "degC",  MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    (MFModelAddFunction (_MDWaterDensity) == CMfailed)) return (CMfailed);
	
	MFDefLeaving  ("WaterDensity");
	return (_MDOutWaterDensityID);
}
