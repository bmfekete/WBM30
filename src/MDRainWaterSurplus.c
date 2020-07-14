/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDWaterSurplus.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

// Input
static int _MDInSPackChgID          = MFUnset;
static int _MDInRainSMoistChgID     = MFUnset;
static int _MDInRainEvapoTransID    = MFUnset;
static int _MDInPrecipID            = MFUnset;
static int _MDInIrrAreaFracID       = MFUnset;
// Output
static int _MDOutRainWaterSurplusID = MFUnset;

static void _MDRainWaterSurplus (int itemID) {
// Input
	float sPackChg;
	float sMoistChg;
	float evapoTrans; 
	float precip;
	float irrAreaFrac;
// Output
	float surplus;
 
	irrAreaFrac = _MDInIrrAreaFracID != MFUnset ? MFVarGetFloat (_MDInIrrAreaFracID, itemID, 0.0) : 0.0;
	sPackChg    = MFVarGetFloat (_MDInSPackChgID,       itemID, 0.0) * (1.0 - irrAreaFrac);
	sMoistChg   = MFVarGetFloat (_MDInRainSMoistChgID,  itemID, 0.0);
	evapoTrans  = MFVarGetFloat (_MDInRainEvapoTransID, itemID, 0.0);
	precip      = MFVarGetFloat (_MDInPrecipID,         itemID, 0.0) * (1.0 - irrAreaFrac);
	
	surplus = precip - sPackChg - evapoTrans - sMoistChg;
	
	if (surplus < 0.0) surplus = 0.0;	//RJS 071811

//	if(itemID == 25014) printf("surplus = %f, precip = %f, sPackChg = %f, evapoTrans = %f, sMoistChg = %f\n", surplus * 100000, precip * 100000, sPackChg * 100000, evapoTrans * 100000, sMoistChg * 100000);

	MFVarSetFloat (_MDOutRainWaterSurplusID, itemID, surplus);
}

int MDRainWaterSurplusDef () {
	int ret = 0;

	if (_MDOutRainWaterSurplusID != MFUnset) return (_MDOutRainWaterSurplusID);

	const char *optStr;
	const char *soilTemperatureOptions [] = { "none", "calculate", (char *) NULL };

 
	const char *soilMoistureOptions [] = { "bucket", "layers", (char *) NULL };
	int soilMoistureOptionID;
	int soilTemperatureID; 
		if (((optStr = MFOptionGet (MDOptSoilMoisture))  == (char *) NULL) || ((soilMoistureOptionID = CMoptLookup (soilMoistureOptions, optStr, true)) == CMfailed)) {
					CMmsgPrint(CMmsgUsrError," Soil Moisture mode not specifed! Options = 'bucket' or 'layers'\n");
					return CMfailed;
				}
		if (((optStr = MFOptionGet (MDOptSoilTemperature))  == (char *) NULL) || ((soilTemperatureID = CMoptLookup (soilTemperatureOptions, optStr, true)) == CMfailed)) {
					CMmsgPrint(CMmsgUsrError," Soil TemperatureOption not specifed! Options = 'none' or 'calculate'\n");
					return CMfailed;
				}
		
		
		if (soilTemperatureID == 1 ){

				
			
		 		if ((ret = MDPermafrostDef()) == CMfailed){ 
		 		printf ("Permafrost failed!\n");
		 			return CMfailed;
		 		}
	 	}
	
	
	MFDefEntering ("Rainfed Water Surplus");
	if ((ret = MDIrrGrossDemandDef ()) == CMfailed) return (CMfailed);
	if ((ret != MFUnset) &&
			  ((_MDInIrrAreaFracID         = MDIrrigatedAreaDef    ())==  CMfailed) )
	     return (CMfailed);	
 	
	
	if (soilMoistureOptionID ==0){ //bucket
		 
		if ((ret = MDRainSMoistChgDef()) == CMfailed) return CMfailed;
			
		   if (( ret != MFUnset) && 
		    		((_MDInRainSMoistChgID      = MDRainSMoistChgDef ()) == CMfailed))
			return CMfailed;
		}
	
	
	
	if (((_MDInPrecipID             = MDPrecipitationDef ()) == CMfailed)) return CMfailed;
	if (((_MDInSPackChgID           = MDSPackChgDef      ()) == CMfailed)) return CMfailed;
	if (((_MDInRainEvapoTransID     = MFVarGetID (MDVarRainEvapotranspiration, "mm", MFInput,  MFFlux,  MFBoundary)) == CMfailed)) return CMfailed;
	

	
	if (((_MDOutRainWaterSurplusID  = MFVarGetID (MDVarRainWaterSurplus,       "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed)) return CMfailed;
	if ((MFModelAddFunction (_MDRainWaterSurplus) == CMfailed)) return (CMfailed);
	MFDefLeaving ("Rainfed Water Surplus");
	return (_MDOutRainWaterSurplusID);
}
