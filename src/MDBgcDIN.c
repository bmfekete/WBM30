/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDBgc_DIN.c  - Routing and processing of DIN 

wil.wollheim@unh.edu

*******************************************************************************/

#include <math.h>
#include <MF.h>
#include <MD.h>

static int _MDInDischargeID            = MFUnset;
static int _MDInRiverWidthID           = MFUnset;
static int _MDInWTempRiverRouteID      = MFUnset;

static int _MDInRunoffVolumeID         = MFUnset;
static int _MDInRunoffID               = MFUnset;
static int _MDInRiverStorageChgID      = MFUnset;
static int _MDInRiverStorageID         = MFUnset;
//static int _MDInResCapacityID          = MFUnset;
static int _MDInResStorageID           = MFUnset;
static int _MDInResStorageChangeID     = MFUnset;

//static int _MDPointSources_UrbanPopID  = MFUnset;
static int _MDNonPoint_DINID           = MFUnset;
static int _MDPointSources_DINID       = MFUnset;
static int _MDInPointScenarioID        = MFUnset;

static int _MDConc_DINID               = MFUnset;
static int _MDFlux_DINID               = MFUnset;
static int _MDLocalIn_DINID            = MFUnset;
static int _MDRemoval_DINID            = MFUnset;
static int _MDStorage_DINID            = MFUnset;
static int _MDDeltaStorage_DINID       = MFUnset;

static int _MDConcMixing_DINID         = MFUnset;
static int _MDFluxMixing_DINID         = MFUnset;
static int _MDStorageMixing_DINID      = MFUnset;
static int _MDDeltaStorageMixing_DINID = MFUnset;

static void _MDDINRouting (int itemID) {
//input	
		float discharge;
		float width;
		float runoff;
		float runoffVol;
        float waterStorageChange;
		float waterStorage;
		float PointScenario;

		float DINLocalIn;
		float DINFlux;
		float DINFluxNew = 0;
        float DINStorage;
		float DINStorageNew;
        float DINDeltaStorage;
        float DINTotalIn;
        float DINConcentration; // # / 100 ml
        
        float DINFluxMixing;
        float DINFluxNewMixing = 0;
        float DINStorageMixing;
        float DINStorageNewMixing;
        float DINDeltaStorageMixing;
        float DINTotalInMixing;
        float DINConcentrationMixing;
        float HL;

        float DINRemoval;
		float massbalance;
		float massbalanceMixing;
		float waterT;
		float ConcPre_DIN;
		//LINX2 rates, EL model (log vf = a log NO3 ^ b - vf in cm/s, NO3 in ug/l)
		//float denit_int = -2.975;
		//float denit_slope = -0.493;
		float denit_int = -2.206;
		float denit_slope = -0.462;
		float NonpointLoadConc_DIN;
		float PointLoadFlux_DIN;
		float TotalVol;
		
		discharge            = MFVarGetFloat (_MDInDischargeID,       itemID, 0.0);
		width                = MFVarGetFloat (_MDInRiverWidthID,      itemID, 0.0);
		runoff               = MFVarGetFloat (_MDInRunoffID,          itemID, 0.0);   //mm
		runoffVol            = MFVarGetFloat (_MDInRunoffVolumeID,    itemID, 0.0); //m3/s
		waterStorageChange   = MFVarGetFloat (_MDInRiverStorageChgID, itemID, 0.0);
		waterStorage         = MFVarGetFloat (_MDInRiverStorageID,    itemID, 0.0);
	    PointScenario        = MFVarGetFloat (_MDInPointScenarioID,   itemID, 0.0);

		waterT			     = MFVarGetFloat (_MDInWTempRiverRouteID, itemID, 0.0);
		NonpointLoadConc_DIN = MFVarGetFloat (_MDNonPoint_DINID,      itemID, 0.0);
		PointLoadFlux_DIN    = MFVarGetFloat (_MDPointSources_DINID,  itemID, 0.0) * PointScenario;
	    
		DINFlux              = MFVarGetFloat (_MDFlux_DINID,          itemID, 0.0);
        DINStorage			 = MFVarGetFloat (_MDStorage_DINID,       itemID, 0.0);
        DINFluxMixing        = MFVarGetFloat (_MDFluxMixing_DINID,    itemID, 0.0);
        DINStorageMixing     = MFVarGetFloat (_MDStorageMixing_DINID, itemID, 0.0);
        
        DINLocalIn = runoffVol * 86400 * NonpointLoadConc_DIN + PointLoadFlux_DIN; //kg/d
        
        //if (!isnan(DINFlux)){
        DINTotalIn = DINFlux + DINLocalIn + DINStorage;
        //}
        
		DINTotalInMixing = DINFluxMixing + DINLocalIn + DINStorageMixing;

		TotalVol = (discharge) * 86400 + waterStorage;  //m3 note:local runoff already included in discharge
		
		if (!isnan(TotalVol) && TotalVol != 0){
			//ConcPre_DIN = 1;
			ConcPre_DIN = DINTotalIn / TotalVol;  //kg/m3
	     }
		else {
			ConcPre_DIN = 0.0;
			}

		float DIN_Tref = 20;
        float DIN_Q10 = 2;
		//float DIN_Q10 = 1;
        float DIN_ArealU;
		float DIN_Vf ;
		float DIN_Vf_ref;

		if (!isnan(ConcPre_DIN) && ConcPre_DIN > 0){
			//MM function //DIN concentration needs to be in mg/l for the specified paramerters
			//DIN_UmaxAdj = DIN_Umax * pow(DIN_Q10, ((waterT - DIN_Tref) / 10));
			//DIN_ArealU = (DIN_UmaxAdj * 24 * ConcPre_DIN * 1000 / (DIN_Ks + ConcPre_DIN * 1000)) / (1000 * 1000) ; //kg/m2/d
			//DIN_Vf = DIN_ArealU / ConcPre_DIN; //m/d
			
			//EL function //DIN concentration needs to be in ug/l for the specified parameters
			//DIN_Vf_ref = pow(10,(denit_int + (log10(ConcPre_DIN * 1000 * 1000) * denit_slope))) * 86400 / 100; //convert vf to m/d
            
			//1st order function
			//DIN_Vf_ref = 0.001 * 864; //LINX2 high denit-vf, m/d
			DIN_Vf_ref = 0.0001 * 864; //LINX2 medium denit-vf, m/d

			DIN_Vf = DIN_Vf_ref * pow(DIN_Q10, ((waterT - DIN_Tref) / 10)); //m/d
			DIN_ArealU = DIN_Vf * ConcPre_DIN; //kg/m2/d
					
			//
        }
		else {
			DIN_ArealU = 0;
			DIN_Vf = 0;
		}
		if (isnan(DIN_Vf)){
		printf ("DIN_Vf_ref %f denit_int %f denit_slope %f ConcPre_DIN %f DIN_Vf %f \n",
				DIN_Vf_ref, denit_int, denit_slope, ConcPre_DIN, DIN_Vf);
		}
		
		HL = (discharge * MDConst_m3PerSecTOm3PerDay) / (MFModelGetLength(itemID) * width);
	     
	    //if (isnan(DINFlux) || isnan(DINRemoval) || isnan(DINConcentration)) printf ("itemID %i DINFLux %f HL %f \n", itemID, DINFlux, HL);

	    // if (isnan(DINFlux)) printf ("itemID %i DINFLux %f HL %f \n", itemID, DINFlux, HL);
	     //printf ("DINFLux %f \n", DINFlux);
		//if (itemID == 576) printf ("WaterT %f DIN_Vf %f HL %f VF_HL %f TE %f, Discharge %f MFModelGetLength %f sinuosity %f width %f runoffVol %f NonpointLoadConc_DIN %f PointLoadFlux_DIN %f DIN_TotalIn %f DINFlux %f DINFLuxMixing %f DINStorage %f waterStorage %f TotalVol %f DIN_UmaxAdj %f DIN_ArealU %f ConcPre_DIN %f waterT %f DIN_Vf %f \n", 
		//		waterT, DIN_Vf, HL, (DIN_Vf / HL), exp(-1 * DIN_Vf / HL), discharge, MFModelGetLength(itemID), sinuosity, width, runoffVol, NonpointLoadConc_DIN, PointLoadFlux_DIN, DINTotalIn, DINFlux, DINFluxMixing, DINStorage, waterStorage, TotalVol, DIN_UmaxAdj, DIN_ArealU, ConcPre_DIN, waterT, DIN_Vf);
		
	    //if (itemID == 576) printf ("WaterT %f DIN_Umax %f DIN_UmaxAdj %f ConcPre_DIN %f DIN_ArealU %f DIN_Vf %f HL %f \n",
	    //				waterT, DIN_Umax, DIN_UmaxAdj, (ConcPre_DIN * 1000), DIN_ArealU, DIN_Vf, HL);
	    				
	    if(!isnan(TotalVol) && (TotalVol != 0)) {
			if (!isnan(HL) && HL > 0){
	    	DINRemoval = DINTotalIn * (1 - exp(-1 * DIN_Vf / HL));
			}
			else DINRemoval = DINTotalIn;
	    	DINConcentration = ((DINTotalIn - DINRemoval) / (TotalVol)); // kg / m3
		    DINStorageNew = waterStorage * (DINConcentration);
		    DINDeltaStorage = DINStorageNew - DINStorage;
		    DINFluxNew = discharge * 86400 * (DINConcentration); //kg/d
		    
		    DINConcentrationMixing = ((DINTotalInMixing) / (TotalVol)); // kg/d
		    DINStorageNewMixing = waterStorage * (DINConcentrationMixing); //kg
		    DINFluxNewMixing = discharge * 86400 * (DINConcentrationMixing); //kg/d
		    DINDeltaStorageMixing = DINStorageNewMixing - DINStorageMixing;
            
	 	    MFVarSetFloat(_MDLocalIn_DINID,itemID,DINLocalIn); 
	 	    MFVarSetFloat(_MDFlux_DINID,itemID,DINFluxNew); 
	 	    MFVarSetFloat(_MDStorage_DINID,itemID,DINStorageNew); 
	 	    MFVarSetFloat(_MDDeltaStorage_DINID,itemID,DINDeltaStorage); 
            MFVarSetFloat(_MDRemoval_DINID,itemID,DINRemoval); 
            MFVarSetFloat(_MDConc_DINID,itemID,DINConcentration);
            
            MFVarSetFloat(_MDFluxMixing_DINID,itemID,DINFluxNewMixing); 
            MFVarSetFloat(_MDStorageMixing_DINID,itemID,DINStorageNewMixing); 
            MFVarSetFloat(_MDConcMixing_DINID,itemID,DINConcentrationMixing);
	 	    MFVarSetFloat(_MDDeltaStorageMixing_DINID,itemID,DINDeltaStorageMixing); 

	    }
		else {
			DINRemoval = DINTotalIn;
			DINDeltaStorage = -DINStorage; 
			DINDeltaStorageMixing = -DINStorageMixing;

			MFVarSetFloat(_MDLocalIn_DINID,itemID,DINLocalIn); 
	 	    MFVarSetFloat(_MDFlux_DINID,itemID,0.0); 
	 	    MFVarSetFloat(_MDStorage_DINID,itemID,0.0); 
	 	    MFVarSetFloat(_MDDeltaStorage_DINID,itemID,DINDeltaStorage); 
            MFVarSetFloat(_MDRemoval_DINID,itemID,DINRemoval); 
            MFVarSetMissingVal(_MDConc_DINID,itemID);
            MFVarSetFloat(_MDFluxMixing_DINID,itemID,0.0); 
            MFVarSetFloat(_MDStorageMixing_DINID,itemID,0.0); 
            MFVarSetMissingVal(_MDConcMixing_DINID,itemID);
	 	    MFVarSetFloat(_MDDeltaStorageMixing_DINID,itemID,DINDeltaStorageMixing); 

		}
	massbalance = (DINFlux + DINLocalIn - (DINRemoval + DINDeltaStorage + DINFluxNew)) / DINTotalIn;
    massbalanceMixing = (DINFluxMixing + DINLocalIn - (DINDeltaStorageMixing + DINFluxNewMixing)) / DINTotalInMixing;
	if(isnan(DINConcentration))
		printf("discharge %f DINFlux %f \n", discharge, DINFlux);
	
    if(!isnan(discharge) && (discharge > 0.001)) {
       if ((massbalance > 0.001) || (massbalanceMixing > 0.001)) {
    	   //printf ("discharge %f DINFlux %f DINLocalIn %f DINStorage %f DINRemoval %f DINStorageNew %f DINFluxNew %f MassBalance %f \n", 
       //       							discharge, DINFlux / 1000000, DINLocalIn / 1000000 , DINStorage / 1000000, DINRemoval / 1000000, DINStorageNew / 1000000, DINFluxNew / 1000000, massbalance);
          //if (itemID == 32) printf ("discharge %f DINFlux %f DINLocalIn %f DINStorage %f DINRemoval %f DINStorageNew %f DINFluxNew %f MassBalance %f \n", 
          //							discharge, DINFlux, DINLocalIn, DINStorage, DINRemoval, DINStorageNew, DINFluxNew, massbalance);
           printf (" MassBalance %f \n", massbalance);
           printf (" MassBalanceMixing %f \n", massbalanceMixing);
       }
	}    
	//if (itemID == 576) printf ("discharge %f DINFlux %f DINLocalIn %f DINStorage %f DINRemoval %f DINStorageNew %f DINFluxNew %f MassBalance %f \n\n", 
	//          							discharge, DINFlux, DINLocalIn, DINStorage, DINRemoval, DINStorageNew, DINFluxNew, massbalance);
}

int MDBgcDINRoutingDef () {
	int optID = MFUnset;
	char *optStr;
	char *options [] = { "none", "calculate", (char *) NULL };
     //char *options [] = { MDNoneStr, (char *) NULL };

	if (_MDFlux_DINID != MFUnset)	return (_MDFlux_DINID);

	MFDefEntering ("DIN Routing Calculation");
	if (((optStr = MFOptionGet (MDOptReservoirs))  == (char *) NULL) || ((optID = CMoptLookup ( options, optStr, true)) == CMfailed)) {
						CMmsgPrint(CMmsgUsrError,"Reservoir Option not specified! Option none or calculate \n");
						return CMfailed;
					}
		    if (optID==1){
		  //  	printf ("Resoption=%i\n",optID);
		    if (((_MDInResStorageID       = MFVarGetID (MDVarReservoirStorage,       "km3",   MFInput, MFState, MFInitial))  == CMfailed) ||
		        ((_MDInResStorageChangeID = MFVarGetID (MDVarReservoirStorageChange, "km3/s", MFInput, MFState, MFBoundary)) == CMfailed))
		    	return CMfailed;
		    }	
		    
   // Input
	if (((_MDInDischargeID            = MDDischargeDef    ()) == CMfailed) ||
        ((_MDInRiverWidthID           = MDRiverWidthDef   ()) == CMfailed) ||
	    ((_MDInRunoffVolumeID         = MDRunoffVolumeDef ()) == CMfailed) ||
	    ((_MDInRunoffID               = MDRunoffDef ()) == CMfailed) ||
	    ((_MDInWTempRiverRouteID      = MDWTempRiverRouteDef ()) == CMfailed) ||
	    ((_MDInRiverStorageChgID      = MFVarGetID (MDVarRiverStorageChg,         "m3/s",   MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInRiverStorageID         = MFVarGetID (MDVarRiverStorage,            "m3",     MFInput,  MFState, MFInitial))  == CMfailed) ||
	    ((_MDInPointScenarioID        = MFVarGetID (MDVarPointScenario,           MFNoUnit, MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDNonPoint_DINID           = MFVarGetID (MDVarBGCNonPoint_DIN,         "kg/m3",  MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDPointSources_DINID       = MFVarGetID (MDVarBGCPointSources_DIN,     "kg/day", MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    	    
	    // Output
	    ((_MDLocalIn_DINID            = MFVarGetID (MDVarDINLocalIn,              "kg/d",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDRemoval_DINID            = MFVarGetID (MDVarDINRemoval,              "kg/d",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDConc_DINID               = MFVarGetID (MDVarDINConcentration,        "kg/m3",  MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDStorage_DINID            = MFVarGetID (MDVarDINStorage,              "kg",     MFOutput, MFState, MFInitial))  == CMfailed) ||
	    ((_MDDeltaStorage_DINID       = MFVarGetID (MDVarDINDeltaStorage,         "kg/day", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDFlux_DINID               = MFVarGetID (MDVarDINFlux  ,               "kg/day", MFRoute,  MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDConcMixing_DINID         = MFVarGetID (MDVarDINConcentration_Mixing, "kg/m3",  MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDStorageMixing_DINID      = MFVarGetID (MDVarDINStorage_Mixing,       "kg",     MFOutput, MFState, MFInitial))  == CMfailed) ||
        ((_MDDeltaStorageMixing_DINID = MFVarGetID (MDVarDINDeltaStorage_Mixing,  "kg/day", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDFluxMixing_DINID         = MFVarGetID (MDVarDINFlux_Mixing ,         "kg/day", MFRoute,  MFFlux,  MFBoundary)) == CMfailed) ||
        (MFModelAddFunction (_MDDINRouting) == CMfailed)) return (CMfailed); 

	MFDefLeaving ("DIN Routing Calculation");
	return (_MDFlux_DINID); 
}
