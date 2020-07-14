/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDBgc_DOC.c  - Routing and processing of DOC 

wil.wollheim@unh.edu

*******************************************************************************/

#include <math.h>
#include <MF.h>
#include <MD.h>

static int _MDInDischargeID            = MFUnset;
static int _MDInRiverWidthID           = MFUnset;
static int _MDInWTempID                = MFUnset; // water temperature
//static int _MDInWTempRiverRouteID      = MFUnset;

static int _MDInRunoffVolumeID         = MFUnset;
static int _MDInRunoffID               = MFUnset;
static int _MDInRunoffMeanID           = MFUnset;
static int _MDInRiverStorageChgID      = MFUnset;
static int _MDInRiverStorageID         = MFUnset;
//static int _MDInResCapacityID          = MFUnset;
static int _MDInResStorageID           = MFUnset;
static int _MDInResStorageChangeID     = MFUnset;

//static int _MDPointSources_UrbanPopID  = MFUnset;
//static int _MDNonPoint_DOCID           = MFUnset;
//static int _MDPointSources_DOCID       = MFUnset;
static int _MDInWetlandsID             = MFUnset;
static int _MDInKoppenID               = MFUnset;

static int _MDConc_DOCID               = MFUnset;
static int _MDFlux_DOCID               = MFUnset;
static int _MDLocalIn_DOCID            = MFUnset;
static int _MDRemoval_DOCID            = MFUnset;
static int _MDStorage_DOCID            = MFUnset;
static int _MDDeltaStorage_DOCID       = MFUnset;

static int _MDConcMixing_DOCID         = MFUnset;
static int _MDFluxMixing_DOCID         = MFUnset;
static int _MDStorageMixing_DOCID      = MFUnset;
static int _MDDeltaStorageMixing_DOCID = MFUnset;

static void _MDDOCRouting (int itemID) {
//input	
		float discharge;
		float width;
		float runoff;
		float runoffMean;
        float runoffVol;
        float waterStorageChange;
		float waterStorage;
		float wetlands;
		float koppen;
		
		float DOCLocalIn;
		float DOCFlux;
		float DOCFluxNew = 0;
        float DOCStorage;
		float DOCStorageNew;
        float DOCDeltaStorage;
        float DOCTotalIn;
        float DOCConcentration; // # / 100 ml
        
        float DOCFluxMixing;
        float DOCFluxNewMixing = 0;
        float DOCStorageMixing;
        float DOCStorageNewMixing;
        float DOCDeltaStorageMixing;
        float DOCTotalInMixing;
        float DOCConcentrationMixing;
        float HL;
        float Local_QFactor;

        float DOCRemoval;
		float massbalance;
		float massbalanceMixing;
		float waterT;
		
		float ConcPre_DOC;
		
		//removal rates, assuming saturation
		float DOC_Umax = 0;  // mg/m2/h
		float DOC_Ks = 0;  // mg/l
		float NonpointLoadConc_DOC;
		float PointLoadFlux_DOC;
		float TotalVol;
				//Loading parameters - uses equation DOC_conc = aQ^b + (c + dlnQFactor) * wetlands  ; mg C / l
		float a; //mean annual concentration with no wetlands - vary by biome using table 1, Mulholland 2003
		float b = 0.282; // rate ar which DOC concentration changes with no wetlands
		float c = 3.2;   //slope of DOC vs wetland relationship
		float d = -0.6949; // rate at which slope of DOC vs. wetlands changes with increasing flow
				
				
		discharge          = MFVarGetFloat (_MDInDischargeID,       itemID, 0.0);
		width              = MFVarGetFloat (_MDInRiverWidthID,      itemID, 0.0);
		runoff             = MFVarGetFloat (_MDInRunoffID,          itemID, 0.0); //mm
		runoffMean         = MFVarGetFloat (_MDInRunoffMeanID,      itemID, 0.0); //mm
        runoffVol          = MFVarGetFloat (_MDInRunoffVolumeID,    itemID, 0.0); //m3/s
		waterStorageChange = MFVarGetFloat (_MDInRiverStorageChgID, itemID, 0.0);
		waterStorage       = MFVarGetFloat (_MDInRiverStorageID,    itemID, 0.0);
        waterT			   = MFVarGetFloat (_MDInWTempID,           itemID, 0.0);
		wetlands           = MFVarGetFloat (_MDInWetlandsID,        itemID, 0.0); //%
		koppen             = MFVarGetFloat (_MDInKoppenID,          itemID, 0.0); //%
        //wetlands = 0;
		//NonpointLoadConc_DOC = MFVarGetFloat (_MDNonPoint_DOCID,  itemID, 0.0);
		PointLoadFlux_DOC    = 0;

		//KoppenID's : 1=Taiga, 2=semi-arid,3=tundra,4=temperate,5=tropics
		//Mullholland DOC mg/l Tundra=2,Taiga=7,Temperate=4,Semi-arid=1, WetTropics=8, DryTropics=3
		if (koppen == 1) a = 7;
		if (koppen == 2) a = 1;
		if (koppen == 3) a = 2;
		if (koppen == 4) a = 4;
		if (koppen == 5) a = 8;
		if (isnan(koppen)) a = 4;

		DOCFlux          = MFVarGetFloat (_MDFlux_DOCID, itemID, 0.0);
        DOCStorage       = MFVarGetFloat (_MDStorage_DOCID, itemID, 0.0);
        DOCFluxMixing    = MFVarGetFloat (_MDFluxMixing_DOCID, itemID, 0.0);
        DOCStorageMixing = MFVarGetFloat (_MDStorageMixing_DOCID, itemID, 0.0);
        
        if (runoffMean > 0){
        Local_QFactor = runoff / runoffMean;  //ratio of runoff to runoff mean is used to drive loading
        }
        else Local_QFactor = 1;
        
        if (Local_QFactor > 0){
        //Local_QFactor = 1;  //ratio of runoff to runoff mean is used to drive loading
        NonpointLoadConc_DOC = a * pow(Local_QFactor,b) + (c + d * log(Local_QFactor)) * wetlands; //mg C / l
        
        DOCLocalIn = (runoffVol * 86400) * (NonpointLoadConc_DOC / 1000) + PointLoadFlux_DOC; //kg/d
        }
        else DOCLocalIn = 0;
        
        
        //if (!isnan(DOCFlux)){
        DOCTotalIn = DOCFlux + DOCLocalIn + DOCStorage;
        //}
        
		DOCTotalInMixing = DOCFluxMixing + DOCLocalIn + DOCStorageMixing;

		TotalVol = (discharge) * 86400 + waterStorage;  //m3 note:local runoff already included in discharge
		
		if (!isnan(TotalVol) && TotalVol != 0){
			//ConcPre_DOC = 1;
			ConcPre_DOC = DOCTotalIn / TotalVol;  //kg/m3
	     }
		else {
			ConcPre_DOC = 0.0;
			}

		float DOC_UmaxAdj;
		float DOC_Tref = 20;
		float DOC_Q10 = 2;
		float DOC_ArealU;
		float DOC_Vf = 0.096; //m/d
		
		// the following is entered only if DOC-vf is initialized to 0
		if (DOC_Vf == 0) {
		DOC_UmaxAdj = DOC_Umax * pow(DOC_Q10, ((waterT - DOC_Tref) / 10));
		if (!isnan(ConcPre_DOC) && ConcPre_DOC > 0){
			DOC_ArealU = (DOC_UmaxAdj * 24 * ConcPre_DOC * 1000 / (DOC_Ks + ConcPre_DOC * 1000)) / (1000 * 1000) ; //kg/m2/d
			DOC_Vf = DOC_ArealU / ConcPre_DOC; //m/d
        }
		else {
			DOC_ArealU = 0;
			DOC_Vf = 0;
		}
		}
	    HL = (discharge * MDConst_m3PerSecTOm3PerDay) / (MFModelGetLength(itemID) * width); //m/d
	     
	    //if (isnan(DOCFlux) || isnan(DOCRemoval) || isnan(DOCConcentration)) printf ("itemID %i DOCFLux %f HL %f \n", itemID, DOCFlux, HL);

	    // if (isnan(DOCFlux)) printf ("itemID %i DOCFLux %f HL %f \n", itemID, DOCFlux, HL);
	     //printf ("DOCFLux %f \n", DOCFlux);
		//if (itemID == 576) printf ("WaterT %f DOC_Vf %f HL %f VF_HL %f TE %f, Discharge %f MFModelGetLength %f sinuosity %f width %f runoffVol %f NonpointLoadConc_DOC %f PointLoadFlux_DOC %f DOC_TotalIn %f DOCFlux %f DOCFLuxMixing %f DOCStorage %f waterStorage %f TotalVol %f DOC_UmaxAdj %f DOC_ArealU %f ConcPre_DOC %f waterT %f DOC_Vf %f \n", 
		//		waterT, DOC_Vf, HL, (DOC_Vf / HL), exp(-1 * DOC_Vf / HL), discharge, MFModelGetLength(itemID), sinuosity, width, runoffVol, NonpointLoadConc_DOC, PointLoadFlux_DOC, DOCTotalIn, DOCFlux, DOCFluxMixing, DOCStorage, waterStorage, TotalVol, DOC_UmaxAdj, DOC_ArealU, ConcPre_DOC, waterT, DOC_Vf);
		
	    //if (itemID == 575) printf ("runoff %f runoffMean %f wetlands %f Local_QFactor %f NonpointLoadConc_DOC %f DOCLocalIn %f DOCFlux %f \n",
	   	//			runoff, runoffMean, wetlands, Local_QFactor, NonpointLoadConc_DOC, DOCLocalIn, DOCFlux);
	    				
	    if(!isnan(TotalVol) && (TotalVol != 0)) {
			if (!isnan(HL) && HL > 0){
	    	DOCRemoval = DOCTotalIn * (1 - exp(-1 * DOC_Vf / HL));
			}
			else DOCRemoval = DOCTotalIn;
	    	DOCConcentration = ((DOCTotalIn - DOCRemoval) / (TotalVol)); // kg / m3
		    DOCStorageNew = waterStorage * (DOCConcentration);
		    DOCDeltaStorage = DOCStorageNew - DOCStorage;
		    DOCFluxNew = discharge * 86400 * (DOCConcentration); //kg/d
		    
		    DOCConcentrationMixing = ((DOCTotalInMixing) / (TotalVol)); // kg/d
		    DOCStorageNewMixing = waterStorage * (DOCConcentrationMixing); //kg
		    DOCFluxNewMixing = discharge * 86400 * (DOCConcentrationMixing); //kg/d
		    DOCDeltaStorageMixing = DOCStorageNewMixing - DOCStorageMixing;
            
	 	    MFVarSetFloat(_MDLocalIn_DOCID,itemID,DOCLocalIn); 
	 	    MFVarSetFloat(_MDFlux_DOCID,itemID,DOCFluxNew); 
	 	    MFVarSetFloat(_MDStorage_DOCID,itemID,DOCStorageNew); 
	 	    MFVarSetFloat(_MDDeltaStorage_DOCID,itemID,DOCDeltaStorage); 
            MFVarSetFloat(_MDRemoval_DOCID,itemID,DOCRemoval); 
            MFVarSetFloat(_MDConc_DOCID,itemID,DOCConcentration);
            
            MFVarSetFloat(_MDFluxMixing_DOCID,itemID,DOCFluxNewMixing); 
            MFVarSetFloat(_MDStorageMixing_DOCID,itemID,DOCStorageNewMixing); 
            MFVarSetFloat(_MDConcMixing_DOCID,itemID,DOCConcentrationMixing);
	 	    MFVarSetFloat(_MDDeltaStorageMixing_DOCID,itemID,DOCDeltaStorageMixing); 

	    }
		else {
			DOCRemoval = DOCTotalIn;
			DOCDeltaStorage = -DOCStorage; 
			DOCDeltaStorageMixing = -DOCStorageMixing;

			MFVarSetFloat(_MDLocalIn_DOCID,itemID,DOCLocalIn); 
	 	    MFVarSetFloat(_MDFlux_DOCID,itemID,0.0); 
	 	    MFVarSetFloat(_MDStorage_DOCID,itemID,0.0); 
	 	    MFVarSetFloat(_MDDeltaStorage_DOCID,itemID,DOCDeltaStorage); 
            MFVarSetFloat(_MDRemoval_DOCID,itemID,DOCRemoval); 
            MFVarSetMissingVal(_MDConc_DOCID,itemID);
            MFVarSetFloat(_MDFluxMixing_DOCID,itemID,0.0); 
            MFVarSetFloat(_MDStorageMixing_DOCID,itemID,0.0); 
            MFVarSetMissingVal(_MDConcMixing_DOCID,itemID);
	 	    MFVarSetFloat(_MDDeltaStorageMixing_DOCID,itemID,DOCDeltaStorageMixing); 

		}
	massbalance = (DOCFlux + DOCLocalIn - (DOCRemoval + DOCDeltaStorage + DOCFluxNew)) / DOCTotalIn;
    massbalanceMixing = (DOCFluxMixing + DOCLocalIn - (DOCDeltaStorageMixing + DOCFluxNewMixing)) / DOCTotalInMixing;
	if(!isnan(discharge) && (discharge > 0.001)) {
       if ((massbalance > 0.001) || (massbalanceMixing > 0.001)) {
    	   //printf ("discharge %f DOCFlux %f DOCLocalIn %f DOCStorage %f DOCRemoval %f DOCStorageNew %f DOCFluxNew %f MassBalance %f \n", 
       //       							discharge, DOCFlux / 1000000, DOCLocalIn / 1000000 , DOCStorage / 1000000, DOCRemoval / 1000000, DOCStorageNew / 1000000, DOCFluxNew / 1000000, massbalance);
          //if (itemID == 32) printf ("discharge %f DOCFlux %f DOCLocalIn %f DOCStorage %f DOCRemoval %f DOCStorageNew %f DOCFluxNew %f MassBalance %f \n", 
          //							discharge, DOCFlux, DOCLocalIn, DOCStorage, DOCRemoval, DOCStorageNew, DOCFluxNew, massbalance);
           printf (" MassBalance %f \n", massbalance);
           printf (" MassBalanceMixing %f \n", massbalanceMixing);
       }
	}    
	//if (itemID == 576) printf ("discharge %f DOCFlux %f DOCLocalIn %f DOCStorage %f DOCRemoval %f DOCStorageNew %f DOCFluxNew %f MassBalance %f \n\n", 
	//          							discharge, DOCFlux, DOCLocalIn, DOCStorage, DOCRemoval, DOCStorageNew, DOCFluxNew, massbalance);
}

int MDBgcDOCRoutingDef () {
	int optID = MFUnset;
	char *optStr;
	char *options [] = { "none", "calculate", (char *) NULL };
     //char *options [] = { MDNoneStr, (char *) NULL };

	if (_MDFlux_DOCID != MFUnset)	return (_MDFlux_DOCID);

	MFDefEntering ("DOC Routing Calculation");
	if (((optStr = MFOptionGet (MDOptReservoirs))  == (char *) NULL) || ((optID = CMoptLookup ( options, optStr, true)) == CMfailed)) {
						CMmsgPrint(CMmsgUsrError,"Reservoir Option not specified! Option none or calculate \n");
						return CMfailed;
					}
		    if (optID==1){
		  //  	printf ("Resoption=%i\n",optID);
		    if (((_MDInResStorageID              = MFVarGetID (MDVarReservoirStorage,       "km3",   MFInput, MFState, MFInitial))  == CMfailed) ||
		        ((_MDInResStorageChangeID        = MFVarGetID (MDVarReservoirStorageChange, "km3/s", MFInput, MFState, MFBoundary)) == CMfailed))
		    	return CMfailed;
		    }	
		    
   // Input
	if (//((_MDInDischargeID           = MDDischargeDef    ()) == CMfailed) ||
        //((_MDInRiverWidthID          = MDRiverWidthDef   ()) == CMfailed) ||
	    //((_MDInRunoffVolumeID        = MDRunoffVolumeDef ()) == CMfailed) ||
		((_MDInRunoffMeanID           = MDRunoffMeanDef ()) == CMfailed) ||
        //((_MDInRunoffID               = MDRunoffDef ()) == CMfailed) ||
	    //((_MDInWTempRiverRouteID      = MDWTempRiverRouteDef ()) == CMfailed) ||
        ((_MDInDischargeID            = MFVarGetID (MDVarDischarge,              "m3/s",    MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDInRiverWidthID           = MFVarGetID (MDVarRiverWidth,             "m",       MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInRunoffVolumeID         = MFVarGetID (MDVarRunoffVolume,           "m3/s",    MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDInRunoffID               = MFVarGetID (MDVarRunoff,                 "mm",      MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDInWTempID                = MFVarGetID (MDVarWTemp_QxT,              "degC",    MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInRiverStorageChgID      = MFVarGetID (MDVarRiverStorageChg,        "m3/s",    MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInRiverStorageID         = MFVarGetID (MDVarRiverStorage,           "m3",      MFInput,  MFState, MFInitial))  == CMfailed) ||
	    ((_MDInWetlandsID             = MFVarGetID (MDVarWetlandProp,             MFNoUnit, MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInKoppenID               = MFVarGetID (MDVarKoppen,                  MFNoUnit, MFInput,  MFState, MFBoundary)) == CMfailed) ||
        //((_MDNonPoint_DOCID           = MFVarGetID (MDVarNonPoint_DOC,             "kg/m3",   MFInput, MFState, MFBoundary))  == CMfailed) ||
	    //((_MDPointSources_DOCID       = MFVarGetID (MDVarPointSources_DOC,         "kg/day",  MFInput, MFState, MFBoundary))  == CMfailed) ||
	    	    
	    // Output
	    ((_MDLocalIn_DOCID            = MFVarGetID (MDVarDOCLocalIn,              "kg/d",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDRemoval_DOCID            = MFVarGetID (MDVarDOCRemoval,              "kg/d",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDConc_DOCID               = MFVarGetID (MDVarDOCConcentration,        "kg/m3",  MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDStorage_DOCID            = MFVarGetID (MDVarDOCStorage,              "kg",     MFOutput, MFState, MFInitial))  == CMfailed) ||
	    ((_MDDeltaStorage_DOCID       = MFVarGetID (MDVarDOCDeltaStorage,         "kg/day", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDFlux_DOCID               = MFVarGetID (MDVarDOCFlux  ,               "kg/day", MFRoute,  MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDConcMixing_DOCID         = MFVarGetID (MDVarDOCConcentration_Mixing, "kg/m3",  MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDStorageMixing_DOCID      = MFVarGetID (MDVarDOCStorage_Mixing,       "kg",     MFOutput, MFState, MFInitial))  == CMfailed) ||
        ((_MDDeltaStorageMixing_DOCID = MFVarGetID (MDVarDOCDeltaStorage_Mixing,  "kg/day", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDFluxMixing_DOCID         = MFVarGetID (MDVarDOCFlux_Mixing ,         "kg/day", MFRoute,  MFFlux,  MFBoundary)) == CMfailed) ||
        (MFModelAddFunction (_MDDOCRouting) == CMfailed)) return (CMfailed); 

	MFDefLeaving ("DOC Routing Calculation");
	return (_MDFlux_DOCID); 
}
