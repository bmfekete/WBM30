/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDPeatlands.c

dominik.wisser@unh.edu

*******************************************************************************/

#include <math.h>
#include <MF.h>
#include <MD.h>

enum { FAO_ID = 0, IWMI_ID = 1 };

//Input
static int  _MDInPrecipID					=MFUnset;
static int  _MDInSPackChgID					=MFUnset;
static int  _MDInWetlandAreaFracID = 		MFUnset;
static int _MDInPotETPID = MFUnset;
static int _MDInWetlandCatchAreaFactorID =MFUnset;

//Output
static int _MDOutWetlandEvapotranspID		=			MFUnset;
static int _MDOutWetlandStorageChangeID		=			MFUnset;
static int _MDOutWetlandRunoffID			=			MFUnset;
static int _MDOutWetlandSurfRunoffUptakeID	=			MFUnset;
static int _MDInRainSurfRunoffID			=		MFUnset;
static int _MDOutWetlandStorageID = MFUnset;
static int _MDOutWetlandLevelWaterDepthID = MFUnset;
static void _MDWetlandRunoff (int itemID) {
	
	//Input
	float precipitation;
	float snowMelt;
	float wetlandAreaFraction;
	float surfRunoff;
	float wetlandsPrevStorage;
	float refETP;
	float catchmentFactor;
	float wetlandStorageChange;
	float wetlandPrevWaterLevelDepth;
	wetlandAreaFraction = MFVarGetFloat(_MDInWetlandAreaFracID, itemID,0.0);
	wetlandAreaFraction=1.0;
	if (wetlandAreaFraction>0){
	
	precipitation = MFVarGetFloat (_MDInPrecipID,      itemID, 0.0);
	surfRunoff          = MFVarGetFloat (_MDInRainSurfRunoffID,      itemID, 0.0);
	refETP       	    = MFVarGetFloat (_MDInPotETPID,      itemID, 0.0);
	wetlandsPrevStorage = MFVarGetFloat (_MDOutWetlandStorageID,    itemID, 0.0);
	wetlandsPrevStorage=wetlandsPrevStorage/wetlandAreaFraction;
	wetlandPrevWaterLevelDepth = MFVarGetFloat (_MDOutWetlandLevelWaterDepthID,    itemID, 0.0);
	catchmentFactor = MFVarGetFloat (_MDInWetlandCatchAreaFactorID,    itemID, 0.0);
	//Output
	
	float wetlandWaterLevelDepth;
	float wetlandETP;
	float wetlandRunoff;
	float wetlandStorage;
	float wetlandSORunoffUptake;
	float wetlandRunoffFactor= 0.2;
	float in;
	float out;
	float balance;
	float remainingSurfaceRO;
	float snowpackChg;

	snowpackChg = MFVarGetFloat (_MDInSPackChgID, itemID, 0.0);
	snowMelt=0;
	if (snowpackChg >  0.0) precipitation = 0.0; //Snow Accumulation, no liquid precipitation
	if (snowpackChg <= 0.0) snowMelt = fabs (snowpackChg);
		 

	
	
	wetlandETP = wetlandsPrevStorage > refETP  ? refETP : wetlandsPrevStorage;
	
	wetlandStorage =wetlandsPrevStorage  - wetlandETP + precipitation+snowMelt;
		
	wetlandSORunoffUptake = surfRunoff * catchmentFactor;

	wetlandStorage = wetlandStorage +  wetlandSORunoffUptake;
	
	wetlandRunoff = wetlandRunoffFactor * wetlandStorage;

	
	wetlandStorage = wetlandStorage -  wetlandRunoff;
	
	// Peatland Water Table
	
	
	if (wetlandStorage < -0.001)printf("wtland storgage < 0!! %f\n",wetlandStorage) ;
	
	wetlandStorageChange = wetlandStorage - wetlandsPrevStorage;
	wetlandWaterLevelDepth=wetlandStorage;
	remainingSurfaceRO = (1.0 - catchmentFactor) * surfRunoff;
	in = precipitation+snowMelt+surfRunoff;
	out = wetlandStorageChange + wetlandRunoff + wetlandETP+remainingSurfaceRO;
 
	balance = in- out ;
	 if (fabs(balance) > 0.01 ){
		printf ("WaterBalance in Wetlands for item %i, balance %f in %f out %f \n",itemID,balance, in , out);
		printf ("in %f precip %f deltaS %f wetlandET %f refET %f curStor %f prevStorage %f\n", in, precipitation, wetlandStorageChange,wetlandETP, refETP,wetlandStorage,wetlandsPrevStorage);
		
	}
	 
	 
	 	// these vars are related to to the wetlands only and are not converted to the total grid cell area. 
	    MFVarSetFloat(_MDOutWetlandStorageID, itemID, wetlandStorage * wetlandAreaFraction );
	 	MFVarSetFloat(_MDOutWetlandLevelWaterDepthID, itemID, wetlandWaterLevelDepth  );
	 	
	    
	 	// relate all fluxes to total grid cell area!
 		MFVarSetFloat(_MDOutWetlandEvapotranspID,   itemID, wetlandETP * wetlandAreaFraction);
	
		MFVarSetFloat(_MDOutWetlandRunoffID,  itemID, (wetlandRunoff+remainingSurfaceRO) * wetlandAreaFraction);
		MFVarSetFloat(_MDOutWetlandSurfRunoffUptakeID,  itemID, wetlandSORunoffUptake*wetlandAreaFraction);
	}
	else
	{
				MFVarSetFloat(_MDOutWetlandStorageID, itemID, 0.0);
			 	MFVarSetFloat(_MDOutWetlandLevelWaterDepthID, itemID, 0.0 );
			 	
			    
			 	// relate all fluxes to total grid cell area!
		 		MFVarSetFloat(_MDOutWetlandEvapotranspID,   itemID, 0.0);
			
				MFVarSetFloat(_MDOutWetlandRunoffID,  itemID, 0.0);
				MFVarSetFloat(_MDOutWetlandSurfRunoffUptakeID,  itemID, 0.0);
		
	}
	
	
}



int MDWetlandRunoffDef () {
 
	if (_MDOutWetlandRunoffID != MFUnset) return (_MDOutWetlandRunoffID);
	
	if ((_MDInWetlandAreaFracID = MDWetlandAreaDef ()) == CMfailed) return (CMfailed);
		if  (_MDInWetlandAreaFracID == MFUnset) return (MFUnset);
	
		MFDefEntering ("Wetlands");

	   if (((_MDInRainSurfRunoffID      = MDRainSurfRunoffDef   ()) == CMfailed) ||
	    	((_MDInWetlandAreaFracID         = MDWetlandAreaDef    ()) == CMfailed) ||
	    	((_MDInPrecipID              = MDPrecipitationDef    ()) == CMfailed) ||
	    	((_MDInSPackChgID            = MDSPackChgDef         ()) == CMfailed) ||
		    ((_MDInPotETPID           = MDRainPotETDef     ()) == CMfailed) ||
	        ((_MDInWetlandCatchAreaFactorID = MFVarGetID (MDVarWetlandCatchmentAreaFactor, "-",  MFInput,  MFState, MFBoundary)) == CMfailed) ||
	        ((_MDOutWetlandLevelWaterDepthID = MFVarGetID (MDVarWetlandWaterLevelDepth, "-",  MFOutput,  MFState, MFInitial)) == CMfailed) ||
	   	   ((_MDOutWetlandSurfRunoffUptakeID     = MFVarGetID (MDVarWetlandSurfROUptake,            "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	        ((_MDOutWetlandRunoffID   				  = MFVarGetID (MDVarWetlandRunoff,         		  "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	        ((_MDOutWetlandStorageID    = MFVarGetID (MDVarWetlandStorage,           "mm", MFOutput, MFState, MFInitial))  == CMfailed) ||
	        ((_MDOutWetlandEvapotranspID      = MFVarGetID (MDVarWetlandEvapotranspiration,       "mm", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	        ((_MDOutWetlandStorageChangeID = MFVarGetID (MDVarWetlandStorageChange,     "mm", MFOutput, MFState, MFBoundary)) == CMfailed) ||
	        ((MFModelAddFunction (_MDWetlandRunoff) == CMfailed))) return (CMfailed);
		MFDefLeaving("Wetlands");
		return (_MDOutWetlandRunoffID);
	}

