/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDTempRiver.c

wil.wollheim@unh.edu

Calculate the temperature in runoff from the local grid cell.  Weight groundwater and surface water temperatures.

*******************************************************************************/

#include <string.h>
#include <math.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInSurfRunoffID     = MFUnset;
static int _MDInBaseFlowID       = MFUnset;
static int _MDInWTempSurfRunoffID = MFUnset;
static int _MDInWTempGrdWaterID   = MFUnset;
// Output
static int _MDOutWTempRiverID     = MFUnset;

static void _MDWTempRiver (int itemID) {
	 float RechargeT;
	 float GrdWaterT;
	 float SurfaceRO;
	 float GrdWaterRO;
	 float TemperatureRO;

   	 RechargeT          = MFVarGetFloat (_MDInWTempSurfRunoffID, itemID, 0.0);
  	 GrdWaterT          = MFVarGetFloat (_MDInWTempGrdWaterID, itemID, 0.0);
   	 SurfaceRO          = MFVarGetFloat (_MDInSurfRunoffID, itemID, 0.0);
   	 GrdWaterRO         = MFVarGetFloat (_MDInBaseFlowID, itemID, 0.0);
 


  // 	 if (itemID == 5132){
  //      	printf("Stop itemID %d day %d \n", itemID, MFDateGetCurrentDay());
  // 	 }
     //TODO: why is runoff volume < 0 sometimes?
        //esnure that if RO_Vol < 0, or RO_WTemp < 0, heat flux input = 0
        SurfaceRO = MDMaximum(0, SurfaceRO);
        GrdWaterRO = MDMaximum(0, GrdWaterRO);
                
// 	 if(!isnan(SurfaceRO) && !isnan(GrdWaterRO) && !isnan(RechargeT) && !isnan(GrdWaterT) && 			//here
// 			   (SurfaceRO + GrdWaterRO) > 0){															//here
 		// if (GrdWaterT > 30){
 		//        	printf("Stop itemID %d day %d \n", itemID, MFDateGetCurrentDay());
 		//        }//	if (itemID == 499){
 	//	        		    printf("Stop: itemID %d \n", itemID);}
   	    TemperatureRO = MDMaximum((((SurfaceRO * RechargeT) + (GrdWaterRO * GrdWaterT)) / (SurfaceRO + GrdWaterRO)),0.0);
   	    
    // 	 if (TemperatureRO > 20){
   	//        	printf("Stop itemID %d day %d \n", itemID, MFDateGetCurrentDay());
   //	    printf("itemID %d day %d TemperatureRO %f SurfaceRO %f RechargeT %f GrdWaterRO %f GrdWaterT %f \n", 
   //	    		itemID, MFDateGetCurrentDay(), TemperatureRO, SurfaceRO, RechargeT, GrdWaterRO, GrdWaterT);
   //	 }
   	   //TemperatureRO = MDMinimum(TemperatureRO, 40);
   	    MFVarSetFloat(_MDOutWTempRiverID,itemID,TemperatureRO); 

//if ( ((SurfaceRO>0) || (GrdWaterRO > 0)) && (RechargeT > 0) && (GrdWaterT > 0) ) {
//	printf("itemID = %d, SurfaceRO = %f, RechargeT = %f, GrdWaterRO = %f, GrdWaterT = %f, TemperatureRO = %f \n", itemID, SurfaceRO, RechargeT, GrdWaterRO, GrdWaterT, TemperatureRO);
//}

//	 }															//here
// 	 else {														//here
//  	    MFVarSetMissingVal(_MDOutWTempRiverID,itemID);		//here
// 	 }															//here

}

int MDWTempRiverDef () {

	if (_MDOutWTempRiverID != MFUnset) return (_MDOutWTempRiverID);

	MFDefEntering ("River temperature");

	if (((_MDInSurfRunoffID      = MDRainSurfRunoffDef ()) == CMfailed) ||
	    ((_MDInBaseFlowID        = MDBaseFlowDef       ()) == CMfailed) ||
	    ((_MDInWTempSurfRunoffID = MDWTempSurfRunoffDef ()) == CMfailed) ||
	    ((_MDInWTempGrdWaterID   = MDWTempGrdWaterDef   ()) == CMfailed) ||
	    ((_MDOutWTempRiverID     = MFVarGetID (MDVarWTempRiver, "degC", MFOutput, MFState, MFBoundary)) == CMfailed) ||	
	    (MFModelAddFunction (_MDWTempRiver) == CMfailed)) return (CMfailed);

	MFDefLeaving ("River temperature");
	return (_MDOutWTempRiverID);
}
