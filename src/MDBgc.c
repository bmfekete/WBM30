 /******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDBgc.c  - Routing and processing of nutrients

wil.wollheim@unh.edu

*******************************************************************************/

#include <math.h>
#include <MF.h>
#include <MD.h>

static int _MDInAirTemperatureID  = MFUnset;
static int _MDInDischargeID       = MFUnset;
//static int _MDWTempRiverID       = MFUnset;
static int _MDInWTempRiverRouteID = MFUnset;
static int _MDInRiverWidthID      = MFUnset;
static int _MDInRunoffVolumeID    = MFUnset;
static int _MDInRunoffID    = MFUnset;
//static int _MDInRunoffMeanID          = MFUnset;
//static int _MDInSubGridNetworkID          = MFUnset;
static int _MDInRiverStorageChgID = MFUnset;
static int _MDInRiverStorageID    = MFUnset;
static int _MDInResCapacityID     = MFUnset;
static int _MDInResStorageID      = MFUnset;

static int _MDNonPoint_NO3ID = MFUnset;
static int _MDPointSources_NO3ID    = MFUnset;
static int _MDNonPoint_NH4ID = MFUnset;
static int _MDPointSources_NH4ID    = MFUnset;
static int _MDNonPoint_DONID = MFUnset;
static int _MDNonPoint_PONID = MFUnset;

static int _MDFlux_NO3ID                = MFUnset;
static int _MDFlux_NH4ID                = MFUnset;
static int _MDFlux_DONID                = MFUnset;
static int _MDFlux_PONID                = MFUnset;


static int _MDLocalLoad_NO3ID           = MFUnset;
static int _MDStoreWater_NO3ID          = MFUnset;
static int _MDLocalLoad_NH4ID           = MFUnset;
static int _MDStoreWater_NH4ID          = MFUnset;
static int _MDLocalLoad_DONID           = MFUnset;
static int _MDStoreWater_DONID          = MFUnset;
static int _MDLocalLoad_PONID           = MFUnset;
static int _MDStoreWater_PONID          = MFUnset;
static int _MDStoreSeds_PONID          = MFUnset;

static void _MDBgcRouting (int itemID) {
//input	
	float airT;
	float discharge;
	float runoff;
	float runoffVol;
    float channelWidth;
	float waterStorageChange;
	float waterStorage;
	float TotalVol;

	float NonpointLoadConc_NO3; 
	float PointLoadFlux_NO3; 
	float NonpointLoadConc_NH4; 
	float PointLoadFlux_NH4; 
	float NonpointLoadConc_DON; 
	float NonpointLoadConc_PON; 

//route
	float Flux_NO3;
	float Flux_NH4;
	float Flux_DON;
	float Flux_PON;

//output
	float LocalLoad_NO3;
	float StoreWater_NO3;
    float ArealUptake_NO3_assim;
    float TotalUptake_NO3_assim;
    float ArealUptake_NO3_denitr;
    float TotalUptake_NO3_denitr;

    
	float LocalLoad_NH4;
	float StoreWater_NH4;
	float ArealUptake_NH4_assim;
	float TotalUptake_NH4_assim;
	float ArealUptake_NH4_nitrif;
	float TotalUptake_NH4_nitrif;

	
	float LocalLoad_DON;
	float StoreWater_DON;
	float ArealUptake_DON;
	float TotalUptake_DON;

	float LocalLoad_PON;
	float StoreWater_PON;
	float StoreSeds_PON;
	float ArealUptake_PON;
	float TotalUptake_PON;
	
	float ArealRemin_PON2NH4;
	float TotalRemin_PON2NH4;
	float ArealResus_PONSed2Water;
	float TotalResus_PONSed2Water;
	
// Local
	float TotalMassPre_NO3;
	float TotalMassPost_NO3;
	float ConcPre_NO3;
	float ConcPost_NO3;
	
	float TotalMassPre_NH4;
	float TotalMassPost_NH4;
	float ConcPre_NH4;
	float ConcPost_NH4;
	
	float TotalMassPre_DON;
	float TotalMassPost_DON;
	float ConcPre_DON;
	float ConcPost_DON;
	
	float TotalMassPre_PON;
	float TotalMassPost_PON;
	float ConcPre_PON;
	float ConcPost_PON;
	
	float waterT;

	//
	//float tnUmax = 2.9;
	//float tnKs = 0.416;
	float Vfref_NH4_assim = 0; //400; //4730; /* m/yr*/
	float Vfref_NH4_nitrif = 0; //400; /* m/yr*/
    float Vfref_NO3_assim = 0; //200;
	float Vfref_NO3_denitr = 0; //47;
	float Vfref_DON = 0; //20;
	float Vfref_PON = 0;//4730 / 2;  /* settling from Wollheim et al 2001, assume 50% is resuspended write away */
	float K_PON = 0;  /* decay rate d-1 */
	float K_RESUS = 0; // resuspension rate d-1

	airT               = MFVarGetFloat (_MDInAirTemperatureID,         itemID, 0.0);
	discharge          = MFVarGetFloat (_MDInDischargeID,              itemID, 0.0);
	runoff             = MFVarGetFloat (_MDInRunoffID,           itemID, 0.0);
	runoffVol          = MFVarGetFloat (_MDInRunoffVolumeID,           itemID, 0.0);
	waterStorageChange = MFVarGetFloat ( _MDInRiverStorageChgID,    itemID, 0.0);
	waterStorage       = MFVarGetFloat ( _MDInRiverStorageID,       itemID, 0.0);
    channelWidth       = MFVarGetFloat ( _MDInRiverWidthID,  itemID, 0.0);
    
	NonpointLoadConc_NO3 = MFVarGetFloat (_MDNonPoint_NO3ID,  itemID, 0.0);
	PointLoadFlux_NO3    = MFVarGetFloat (_MDPointSources_NO3ID,         itemID, 0.0);
	NonpointLoadConc_NH4 = MFVarGetFloat (_MDNonPoint_NH4ID,  itemID, 0.0);
	PointLoadFlux_NH4    = MFVarGetFloat (_MDPointSources_NH4ID,         itemID, 0.0);
	NonpointLoadConc_DON = MFVarGetFloat (_MDNonPoint_DONID,  itemID, 0.0);
	NonpointLoadConc_PON = MFVarGetFloat (_MDNonPoint_PONID,  itemID, 0.0);
	
	Flux_NO3             = MFVarGetFloat ( _MDFlux_NO3ID,                itemID, 0.0);
	StoreWater_NO3       = MFVarGetFloat ( _MDStoreWater_NO3ID,          itemID, 0.0);
	Flux_NH4             = MFVarGetFloat ( _MDFlux_NH4ID,                itemID, 0.0);
	StoreWater_NH4       = MFVarGetFloat ( _MDStoreWater_NH4ID,          itemID, 0.0);
	Flux_DON             = MFVarGetFloat ( _MDFlux_DONID,                itemID, 0.0);
	StoreWater_DON       = MFVarGetFloat ( _MDStoreWater_DONID,          itemID, 0.0);
	Flux_PON             = MFVarGetFloat ( _MDFlux_PONID,                itemID, 0.0);
	StoreWater_PON       = MFVarGetFloat ( _MDStoreWater_PONID,          itemID, 0.0);
    StoreSeds_PON        = MFVarGetFloat ( _MDStoreSeds_PONID,           itemID, 0.0);
//////////////////////////////  GUTS

	waterT = 0.8 + ((26.2 - 0.8) / (1 + exp(0.18 * (13.3 - airT))));

 
// The conversion needs attention! Balazs
	LocalLoad_NO3= runoff * MFModelGetArea(itemID) / 1000 * NonpointLoadConc_NO3 + PointLoadFlux_NO3; //kg/d
	LocalLoad_NH4= runoff * MFModelGetArea(itemID) / 1000 * NonpointLoadConc_NH4 + PointLoadFlux_NH4; //kg/d
	LocalLoad_DON= runoff * MFModelGetArea(itemID) / 1000 * NonpointLoadConc_DON; //kg/d
	LocalLoad_PON= runoff * MFModelGetArea(itemID) / 1000 * NonpointLoadConc_PON; //kg/d
//	if (itemID == 1) printf ("itemID %i, day %i, runoff %f, NonpointLoadConc_NH4 %f, PointLoadFlux_NH4 %f, LocalLoad_NH4 %f \n", 
//			                       itemID, MFDateGetDayOfYear(), runoff, NonpointLoadConc_NH4, PointLoadFlux_NH4, LocalLoad_NH4);
	//waterStorage needs to be corrected to have a positive value (using mean depth across length of stream
	TotalMassPre_NH4 = LocalLoad_NH4 + StoreWater_NH4 + Flux_NH4;  //kg
	TotalMassPre_NO3 = LocalLoad_NO3 + StoreWater_NO3 + Flux_NO3;  //kg
	TotalMassPre_DON = LocalLoad_DON + StoreWater_DON + Flux_DON;  //kg
	TotalMassPre_PON = LocalLoad_PON + StoreWater_PON + Flux_PON;  //kg
	
	TotalVol = (discharge + runoffVol) * 86400 + waterStorage;  //m3
//	if (itemID == 1) printf ("waterStorage %f, TotalMassPre_NH4 %f, StoreWater_NH4 %f, Flux_NH4 %f \n", waterStorage, TotalMassPre_NH4, StoreWater_NH4, Flux_NH4);
		
	
	if (!isnan(TotalVol) && TotalVol != 0){
		ConcPre_NO3 = TotalMassPre_NO3 / TotalVol;  //kg/m3
		ConcPre_NH4 = TotalMassPre_NH4 / TotalVol;  //kg/m3
		ConcPre_DON = TotalMassPre_DON / TotalVol;  //kg/m3
		ConcPre_PON = TotalMassPre_PON / TotalVol;  //kg/m3
     }
	else {
		ConcPre_NO3 = 0.0;
		ConcPre_NH4 = 0.0;
		ConcPre_DON = 0.0;
		ConcPre_PON = 0.0;
	}
//	if (itemID == 1) printf ("TotalMassPre_NH4 %f, waterStorage %f, ConcPre_NH4 %f \n", TotalMassPre_NH4, waterStorage, ConcPre_NH4);
	
	/*tnVfAdj = tnVfref * pow(tnQ10, ((waterT - tnTref) / 10)); */
	
	ArealUptake_NH4_assim =  (Vfref_NH4_assim *  ConcPre_NH4) / 365;  /*kg/m2/d */
	TotalUptake_NH4_assim =  MDMinimum(TotalMassPre_NH4, (ArealUptake_NH4_assim * MFModelGetLength(itemID) * channelWidth));
	ArealUptake_NH4_nitrif =  (Vfref_NH4_nitrif *  ConcPre_NH4) / 365;  /*kg/m2/d */
	TotalUptake_NH4_nitrif =  MDMinimum((TotalMassPre_NH4 - TotalUptake_NH4_assim), (ArealUptake_NH4_nitrif * MFModelGetLength(itemID) * channelWidth));
	    
	ArealUptake_NO3_assim =  (Vfref_NO3_assim *  ConcPre_NO3) / 365;  /* kg/m2/d */
	TotalUptake_NO3_assim =  MDMinimum(TotalMassPre_NO3,(ArealUptake_NO3_assim * MFModelGetLength(itemID) * channelWidth));	
	ArealUptake_NO3_denitr =  (Vfref_NO3_denitr *  ConcPre_NO3) / 365;  /* kg/m2/d */
	TotalUptake_NO3_denitr =  MDMinimum((TotalMassPre_NO3 - TotalUptake_NO3_assim), (ArealUptake_NO3_denitr * MFModelGetLength(itemID) * channelWidth));
		    
	ArealUptake_DON =  (Vfref_DON *  ConcPre_DON) / 365;  /* kg/m2/d */
	TotalUptake_DON =  MDMinimum(TotalMassPre_DON, (ArealUptake_DON * MFModelGetLength(itemID) * channelWidth));
	
	ArealUptake_PON =  (Vfref_PON *  ConcPre_PON) / 365;  /* kg/m2/d */
	TotalUptake_PON =  MDMinimum(TotalMassPre_PON,(ArealUptake_PON * MFModelGetLength(itemID) * channelWidth));

	
	
	ArealRemin_PON2NH4 = StoreSeds_PON * K_PON;  /* kg/m2/d */
	TotalRemin_PON2NH4 = ArealRemin_PON2NH4 * MFModelGetLength(itemID) * channelWidth;
    ArealResus_PONSed2Water = StoreSeds_PON * K_RESUS;
    TotalResus_PONSed2Water = ArealResus_PONSed2Water * MFModelGetLength(itemID) * channelWidth;
    
	// if (itemID == 1) printf ("ArealUptake_NH4_assim %f ArealUptake_NO3_assim %f ArealUptake_PON %f ArealRemin_PON2NH4 %f StoreSeds_PON %f ArealResus_PONSed2Water %f, \n", 
	//		ArealUptake_NH4_assim, ArealUptake_NO3_assim, ArealUptake_PON, ArealRemin_PON2NH4, StoreSeds_PON, ArealResus_PONSed2Water );
//	if (itemID == 1) printf ("ArealUptake_NH4_assim %f TotalUptake_NH4_assim %f ArealUptake_NH4_nitrif %f MFModelGetLength(itemID) %f channelWidth %f, \n",
//		                 ArealUptake_NH4_assim, TotalUptake_NH4_assim,ArealUptake_NH4_nitrif,MFModelGetLength(itemID), channelWidth);
	//if (isnan(Flux_NO3)) printf ("NAN! Vfref_NO3_assim %f ConcPre_NO3 %f Vfref_NO3_denitr %f, \n",Vfref_NO3_assim, ConcPre_NO3,Vfref_NO3_denitr);

	TotalMassPost_NH4 = TotalMassPre_NH4 - TotalUptake_NH4_assim - TotalUptake_NH4_nitrif + TotalRemin_PON2NH4 + TotalUptake_DON;
	//TotalMassPost_NH4 = TotalMassPre_NH4 - TotalUptake_NH4_assim - TotalUptake_NH4_nitrif;
    TotalMassPost_NO3 = TotalMassPre_NO3 + TotalUptake_NH4_nitrif - TotalUptake_NO3_assim - TotalUptake_NO3_denitr;
	//TotalMassPost_NO3 = TotalMassPre_NO3 - TotalUptake_NO3_assim - TotalUptake_NO3_denitr;
     TotalMassPost_DON = TotalMassPre_DON - TotalUptake_DON;
	TotalMassPost_PON = TotalMassPre_PON + TotalResus_PONSed2Water - TotalUptake_PON ;
 
	
	/* sed storage needs consideration given the changing widths with flow - how do you account for more material at certain times?
	 * Currently it assumes the same per m2 storage occurs in the current time step as in the previous time step.
	 */
	StoreSeds_PON     = StoreSeds_PON +  ArealUptake_NH4_assim + ArealUptake_NO3_assim + ArealUptake_PON - ArealRemin_PON2NH4 - ArealResus_PONSed2Water;
	/* if (itemID == 1) printf ("StoreSeds_PON %f, \n", StoreSeds_PON); */
	
	if (!isnan(TotalVol) && TotalVol != 0){
	ConcPost_NO3 = TotalMassPost_NO3 / TotalVol;
	ConcPost_NH4= TotalMassPost_NH4 / TotalVol;
	ConcPost_DON = TotalMassPost_DON / TotalVol;
	ConcPost_PON = TotalMassPost_PON / TotalVol;
	}
	else { 
	ConcPost_NO3 = 0.0;
	ConcPost_NH4 = 0.0;
	ConcPost_DON = 0.0;
	ConcPost_PON = 0.0;
	}
	// does flux out need to be discharge + runoff volume + delta storage?  DO MASS BALANCE!!!
	Flux_NO3 = (discharge * MDConst_m3PerSecTOm3PerDay) * ConcPost_NO3;
	Flux_NH4 = (discharge * MDConst_m3PerSecTOm3PerDay) * ConcPost_NH4;
	Flux_DON = (discharge * MDConst_m3PerSecTOm3PerDay) * ConcPost_DON;
	Flux_PON = (discharge * MDConst_m3PerSecTOm3PerDay) * ConcPost_PON;
	
	StoreWater_NO3 = waterStorage * ConcPost_NO3;
	StoreWater_NH4 = waterStorage * ConcPost_NH4;
	StoreWater_DON = waterStorage * ConcPost_DON;
	StoreWater_PON = waterStorage * ConcPost_PON;

	
	/* (Variable ID , itemID, value) */
	MFVarSetFloat (_MDFlux_NO3ID,itemID,Flux_NO3); 
	MFVarSetFloat (_MDFlux_NH4ID,itemID,Flux_NH4); 
	MFVarSetFloat (_MDFlux_DONID,itemID,Flux_DON); 
	MFVarSetFloat (_MDFlux_PONID,itemID,Flux_PON); 
	 	
	MFVarSetFloat (_MDLocalLoad_NO3ID,itemID,LocalLoad_NO3);
	MFVarSetFloat (_MDLocalLoad_NH4ID,itemID,LocalLoad_NH4);
	MFVarSetFloat (_MDLocalLoad_DONID,itemID,LocalLoad_DON);
	MFVarSetFloat (_MDLocalLoad_PONID,itemID,LocalLoad_PON);

	MFVarSetFloat (_MDStoreWater_NO3ID,itemID,StoreWater_NO3);
	MFVarSetFloat (_MDStoreWater_NH4ID,itemID,StoreWater_NH4);
	MFVarSetFloat (_MDStoreWater_DONID,itemID,StoreWater_DON);
	MFVarSetFloat (_MDStoreWater_PONID,itemID,StoreWater_PON);

	MFVarSetFloat (_MDStoreSeds_PONID,itemID,StoreSeds_PON); 
}	

// define the variable needed by the function
int MDBgcRoutingDef () {
	char *optStr;
	char *options [] = { MDNoneStr, (char *) NULL };

	if (_MDFlux_NO3ID != MFUnset)	return (_MDFlux_NO3ID);

	MFDefEntering ("Nutrient Calculation");
			
   // Input
	if (((_MDInDischargeID           = MDDischargeDef    ()) == CMfailed) ||
        ((_MDInRiverWidthID          = MDRiverWidthDef   ()) == CMfailed) ||
	    ((_MDInRunoffVolumeID        = MDRunoffVolumeDef ()) == CMfailed) ||
	    ((_MDInWTempRiverRouteID     = MDWTempRiverRouteDef ()) == CMfailed) ||
        ((_MDInRunoffID              = MDRunoffDef ()) == CMfailed) ||
        //((_MDInSubGridNetworkID        = MDSubGridNetworkDef ()) == CMfailed) ||
        //((_MDInRunoffMeanID          = MDRunoffMeanDef ()) == CMfailed) ||
        ((_MDInAirTemperatureID      = MFVarGetID (MDVarAirTemperature,         "degC",    MFInput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInRiverStorageChgID     = MFVarGetID (MDVarRiverStorageChg,        "m3/s",    MFInput, MFState, MFBoundary))  == CMfailed) ||
	    ((_MDInRiverStorageID        = MFVarGetID (MDVarRiverStorage,           "m3",      MFInput, MFState, MFInitial))   == CMfailed) ||
	    
	    ((_MDNonPoint_NO3ID          = MFVarGetID (MDVarBGCNonPoint_NO3,             "kg/m3",   MFInput, MFState, MFBoundary))  == CMfailed) ||
	    ((_MDPointSources_NO3ID      = MFVarGetID (MDVarBGCPointSources_NO3,         "kg/day",  MFInput, MFState, MFBoundary))  == CMfailed) ||
	    ((_MDNonPoint_NH4ID          = MFVarGetID (MDVarBGCNonPoint_NH4,             "kg/m3",   MFInput, MFState, MFBoundary))  == CMfailed) ||
	    ((_MDPointSources_NH4ID      = MFVarGetID (MDVarBGCPointSources_NH4,         "kg/day",  MFInput, MFState, MFBoundary))  == CMfailed) ||
	    ((_MDNonPoint_DONID          = MFVarGetID (MDVarBGCNonPoint_DON,             "kg/m3",   MFInput, MFState, MFBoundary))  == CMfailed) ||
	    ((_MDNonPoint_PONID          = MFVarGetID (MDVarBGCNonPoint_PON,             "kg/m3",   MFInput, MFState, MFBoundary))  == CMfailed) ||
	  	 // Output
	    ((_MDLocalLoad_NO3ID           = MFVarGetID (MDVarBgcLocalLoad_NO3,         "kg/day",  MFOutput, MFState, MFBoundary))  == CMfailed) ||
	    ((_MDStoreWater_NO3ID          = MFVarGetID (MDVarBGCStoreWater_NO3,        "kg",      MFOutput, MFState, MFInitial))   == CMfailed) ||
	    
	    ((_MDLocalLoad_NH4ID           = MFVarGetID (MDVarBGCLocalLoad_NH4,         "kg/day",  MFOutput, MFState, MFBoundary))  == CMfailed) ||
	 	((_MDStoreWater_NH4ID          = MFVarGetID (MDVarBGCStoreWater_NH4,        "kg",      MFOutput, MFState, MFInitial))   == CMfailed) ||
	    
	    ((_MDLocalLoad_DONID           = MFVarGetID (MDVarBGCLocalLoad_DON,         "kg/day",  MFOutput, MFState, MFBoundary))  == CMfailed) ||
	    ((_MDStoreWater_DONID          = MFVarGetID (MDVarBGCStoreWater_DON,        "kg",      MFOutput, MFState, MFInitial))   == CMfailed) ||
	    
	    ((_MDLocalLoad_PONID           = MFVarGetID (MDVarBGCLocalLoad_PON,         "kg/day",  MFOutput, MFState, MFBoundary))  == CMfailed) ||
	    ((_MDStoreWater_PONID          = MFVarGetID (MDVarBGCStoreWater_PON,        "kg",      MFOutput, MFState, MFInitial))   == CMfailed) ||
	    ((_MDStoreSeds_PONID           = MFVarGetID (MDVarBGCStoreSeds_PON,         "kg/m2",      MFOutput, MFState, MFInitial))   == CMfailed) ||

	    ((_MDFlux_NH4ID                = MFVarGetID (MDVarBGCFlux_NH4  ,            "kg/day",  MFRoute,  MFState, MFBoundary))  == CMfailed) ||
	    ((_MDFlux_DONID                = MFVarGetID (MDVarBGCFlux_DON  ,            "kg/day",  MFRoute,  MFState, MFBoundary))  == CMfailed) ||
	    ((_MDFlux_PONID                = MFVarGetID (MDVarBGCFlux_PON  ,            "kg/day",  MFRoute,  MFState, MFBoundary))  == CMfailed) ||
	    ((_MDFlux_NO3ID                = MFVarGetID (MDVarBGCFlux_NO3  ,            "kg/day",  MFRoute,  MFState, MFBoundary))  == CMfailed) |
   (MFModelAddFunction (_MDBgcRouting) == CMfailed)) return (CMfailed); 
	if (((optStr = MFOptionGet (MDOptReservoirs)) != (char *) NULL) && (CMoptLookup (options,optStr,true) == CMfailed)) {
		if (((_MDInResCapacityID       = MFVarGetID (MDVarReservoirCapacity,      "km3",     MFInput, MFState, MFBoundary))  == CMfailed) ||
		    ((_MDInResStorageID        = MFVarGetID (MDVarReservoirStorage,       "km3",     MFInput, MFState, MFInitial))   == CMfailed))
	    	return (CMfailed);
	}
	MFDefLeaving ("Nutrient Calculation");
	return (_MDFlux_NO3ID); 
}
