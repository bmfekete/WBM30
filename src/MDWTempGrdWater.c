/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDTempGrdWater.c

wil.wollheim@unh.edu

Calculate groundwater temperature by mixing existing groundwater, rain recharge, and irrigation return flow.
Rain recharge temperature is calculated in MDWTempSurfRunoff
Irrigation return flow is assumed to have air temperature.
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>
#include <math.h>

// Input
static int _MDInAirTempID         = MFUnset;
static int _MDInWTempSurfRunoffID = MFUnset;
static int _MDInRainRechargeID    = MFUnset;
static int _MDInIrrReturnFlowID   = MFUnset;
static int _MDOutGrdWaterID       = MFUnset;
// Output
static int _MDOutWTempGrdWaterID  = MFUnset;

static void _MDWTempGrdWater (int itemID) {
	float airT;
	float RechargeT;
    float GrdWaterT;

	float RainRechargeIn;
	float IrrReturnFlow;
    float GrdWaterStorage;




	airT               = MFVarGetFloat (_MDInAirTempID,         itemID, 0.0);
	RechargeT          = MFVarGetFloat (_MDInWTempSurfRunoffID, itemID, 0.0);

	RainRechargeIn     = MFVarGetFloat (_MDInRainRechargeID,    itemID, 0.0);

	if (_MDInIrrReturnFlowID != MFUnset){
	IrrReturnFlow      = MFVarGetFloat (_MDInIrrReturnFlowID,   itemID, 0.0);
	}
	else { IrrReturnFlow = 0; }

	GrdWaterStorage    = MFVarGetFloat (_MDOutGrdWaterID,         itemID, 0.0);
	GrdWaterT          = MFVarGetFloat (_MDOutWTempGrdWaterID,   itemID, 0.0);
	//if (itemID == 233){
	//        	printf("Stop itemID %d day %d \n", itemID, MFDateGetCurrentDay());
	//   	 }

	//TODO: why is RainRechargeIn < 0 sometimes?
	RainRechargeIn = MDMaximum(0, RainRechargeIn);
	IrrReturnFlow = MDMaximum(0, IrrReturnFlow);
	GrdWaterStorage = MDMaximum(0, GrdWaterStorage);

	if (!isnan(GrdWaterStorage) && !isnan(RainRechargeIn) && !isnan(IrrReturnFlow) &&
			((GrdWaterStorage + RainRechargeIn + IrrReturnFlow) > 0) &&
			!isnan(GrdWaterT) && !isnan(RechargeT) && !isnan(airT)){


	    GrdWaterT = MDMaximum( (((GrdWaterStorage * GrdWaterT) + (RainRechargeIn * RechargeT) + (IrrReturnFlow * airT)) /
	                       (GrdWaterStorage + RainRechargeIn + IrrReturnFlow)), 0.0);
       // if (GrdWaterT > 30){
       // 	printf("Stop itemID %d day %d \n", itemID, MFDateGetCurrentDay());
       // }
	   // GrdWaterT = MDMinimum(GrdWaterT, 40);
	    //does this need to be properly mass balanced?  GRdwater T might just keep going up.

	    MFVarSetFloat (_MDOutWTempGrdWaterID,itemID,GrdWaterT);
	}
	else{
  	    MFVarSetMissingVal(_MDOutWTempGrdWaterID,itemID);

	}

	if (GrdWaterT > 100) printf("m = %d, d = %d, itemID = %d, GrdWaterT = %f, RainRechargeIn = %f\n", MFDateGetCurrentMonth (), MFDateGetCurrentDay (), itemID, GrdWaterT, RainRechargeIn);


}

int MDWTempGrdWaterDef () {

	if (_MDOutWTempGrdWaterID != MFUnset) return (_MDOutWTempGrdWaterID);

	MFDefEntering ("Groundwater temperature");

	if (((_MDInWTempSurfRunoffID = MDWTempSurfRunoffDef ()) == CMfailed) ||
	    ((_MDInRainRechargeID    = MDRainInfiltrationDef ()) == CMfailed) ||
	    ((_MDInIrrReturnFlowID   = MDIrrReturnFlowDef    ()) == CMfailed) ||
	    ((_MDOutGrdWaterID         = MDBaseFlowDef         ()) == CMfailed) ||
	    ((_MDInAirTempID         = MFVarGetID (MDVarAirTemperature,  "degC", MFInput,  MFState, MFBoundary))  == CMfailed) ||
	    ((_MDOutWTempGrdWaterID  = MFVarGetID (MDVarWTempGrdWater, "degC", MFOutput, MFState,  MFInitial)) == CMfailed) ||
	    (MFModelAddFunction (_MDWTempGrdWater) == CMfailed)) return (CMfailed);

	MFDefLeaving ("Groundwater temperature");
	return (_MDOutWTempGrdWaterID);
}
