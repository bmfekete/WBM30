/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDBgc_DINPLUSBIOMASS.c  - Routing and processing of DIN 

wil.wollheim@unh.edu

*******************************************************************************/

#include <math.h>
#include <MF.h>
#include <MD.h>

static int _MDInDischargeID       = MFUnset;
static int _MDInRiverWidthID      = MFUnset;
static int _MDInWTempRiverRouteID = MFUnset; // water temperature function
// TODO static int _MDInRiverLightID      = MFUnset;
// TODO static int _MDInRiverGPPID      = MFUnset;
static int _MDInWTempID         = MFUnset; // water temperature
// TODO static int _MDInSubGridNetworkID         = MFUnset; //define lengths of stream

static int _MDInRunoffVolumeID    = MFUnset;
static int _MDInRunoffID          = MFUnset;
static int _MDInRiverStorageChgID = MFUnset;
static int _MDInRiverStorageID    = MFUnset;
//static int _MDInResCapacityID     = MFUnset;
static int _MDInResStorageID      = MFUnset;
static int _MDInResStorageChangeID      = MFUnset;

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

static int _MDDenit_DINID              = MFUnset;
static int _MDAssim_DINID              = MFUnset;
static int _MDRemin_DINID              = MFUnset;
static int _MDBiomass_DINID            = MFUnset;
static int _MDDeltaBiomass_DINID       = MFUnset;
  
static int _MDConcMixing_DINID         = MFUnset;
static int _MDFluxMixing_DINID         = MFUnset;
static int _MDStorageMixing_DINID      = MFUnset;
static int _MDDeltaStorageMixing_DINID = MFUnset;

static int _MDDrying_DINID             = MFUnset;
static int _MDDryingMixing_DINID       = MFUnset;

static void _MDDINPLUSBIOMASSRouting (int itemID) {
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

        float DIN_TotalRemoval;
        float DIN_Denit;
        float DIN_Assim;
        float DIN_Remin;
        float DIN_Drying;
        float DIN_DryingMixing;
        float massbalance = 0;
		float massbalanceMixing = 0;
		float waterT;
		
		float ConcPre_DIN;

		//LINX2 rates, EL model (log vf = a log NO3 ^ b - vf in cm/s, NO3 in ug/l)
	    // Mulholland et al. 2008
		float denit_int = -2.975;
	    float denit_slope = -0.493;	
	    //float total_int = -2.206;
	    //float total_slope = -0.462;
		float assim_int = -2.2858;
		float assim_slope = -0.4569;
		float DIN_Kremin_ref  = 0.01; //d-1 
		float DIN_Tref = 20;
        float DIN_Q10 = 2;
	    float DIN_ArealUdenit;
	    float DIN_ArealUassim;
	    float DIN_Vfdenit = 0;
		float DIN_Vfassim = 0;
		float DIN_Kremin = 0;
        float N_Biomass;
        float N_BiomassNew;
        float N_DeltaBiomass;
        
		float DIN_Vfdenit_ref = 0;
		float DIN_Vfassim_ref = 0;
		
		float NonpointLoadConc_DIN;
		float PointLoadFlux_DIN;
		float NonPointLoadFlux_DIN;
		float TotalVol;
		float TotalArea;
        int   day;
        float month;
        float temp; 
        float temp2;
	    month = MFDateGetCurrentMonth();

	    day = MFDateGetCurrentDay();

		discharge          = MFVarGetFloat (_MDInDischargeID, itemID, 0.0);
		width              = MFVarGetFloat (_MDInRiverWidthID, itemID, 0.0);
		runoff             = MFVarGetFloat (_MDInRunoffID,           itemID, 0.0);   //mm
		runoffVol          = MFVarGetFloat (_MDInRunoffVolumeID,           itemID, 0.0); //m3/s
		waterStorageChange = MFVarGetFloat (_MDInRiverStorageChgID,    itemID, 0.0);
		waterStorage       = MFVarGetFloat (_MDInRiverStorageID,       itemID, 0.0);
	    PointScenario      = MFVarGetFloat (_MDInPointScenarioID,  itemID, 0.0);

		waterT			     = MFVarGetFloat (_MDInWTempID, itemID, 0.0);
		NonpointLoadConc_DIN = MFVarGetFloat (_MDNonPoint_DINID,  itemID, 0.0);
		PointLoadFlux_DIN    = MFVarGetFloat (_MDPointSources_DINID,         itemID, 0.0) * PointScenario;
		
		DINFlux                = MFVarGetFloat (_MDFlux_DINID, itemID, 0.0);
        DINStorage			   = MFVarGetFloat (_MDStorage_DINID, itemID, 0.0);
        DINFluxMixing          = MFVarGetFloat (_MDFluxMixing_DINID, itemID, 0.0);
        DINStorageMixing	   = MFVarGetFloat (_MDStorageMixing_DINID, itemID, 0.0);
        DIN_Drying              = MFVarGetFloat (_MDDrying_DINID, itemID, 0.0);
        DIN_DryingMixing        = MFVarGetFloat (_MDDryingMixing_DINID, itemID, 0.0);
        N_Biomass			   = MFVarGetFloat (_MDBiomass_DINID, itemID, 0.0); //kg/reach
       //TODO add reservoir and water withdrawal options - see TempRiverRoute
        
        //TODO what to do when discharge = 0, but storage > 0?
        if (discharge > 0.0){
    	   TotalVol = (discharge) * 86400.0 + waterStorage;  //m3 note:local runoff already included in discharge
    	   TotalArea = MFModelGetLength(itemID) * width;  // How to account for mass balance with changing width
        }
        else {
           TotalVol = 0.0;
           TotalArea = 0.0;
           width = 0.0;
        }
        //TODO understand how waterStorage > 0 when no discharge
       //if (MFMathEqualValues (discharge,  0.0) && waterStorage > 0.0){
    //	   printf("stop: discharge %f waterStorage %f width %f \n", discharge, waterStorage, width);
    //   }
               
    	DIN_Kremin = DIN_Kremin_ref * pow(DIN_Q10, ((waterT - DIN_Tref) / 10.0)); //1/d        
		DIN_Remin = MDMinimum(N_Biomass, DIN_Kremin * N_Biomass); // this makes sure remin is never greater than available biomass
		
		//if (runoffVol < 0.0 || width < 0.0)
		//            printf("stop runoffVol %f \n", runoffVol);
		//TODO fix negative runoff volumes - fix here for now
		if (runoffVol > 0.0) {NonPointLoadFlux_DIN =  runoffVol * 86400.0 * NonpointLoadConc_DIN;}
		else {NonPointLoadFlux_DIN = 0.0;}
				
		DINLocalIn = NonPointLoadFlux_DIN + PointLoadFlux_DIN;
			
		if ((discharge > 0.0)){
			
		   DINTotalIn = DINFlux + DINLocalIn + DINStorage + DIN_Remin;
		   DINTotalInMixing = DINFluxMixing + DINLocalIn + DINStorageMixing;  //no effect of biomass - mixing is to check routing
		   ConcPre_DIN = MDMinimum(0.1, (DINTotalIn / TotalVol));  //kg/m3, assume max concentration for vf is 100mg/l
		
		   if (ConcPre_DIN > 0.0){
		   		//EL function //DIN concentration needs to be in ug/l for the specified parameters
		   		DIN_Vfdenit_ref = pow(10,(denit_int + (log10(ConcPre_DIN * 1000 * 1000) * denit_slope))) * 86400.0 / 100; //convert vf to m/d
		   		DIN_Vfassim_ref = pow(10,(assim_int + (log10(ConcPre_DIN * 1000 * 1000) * assim_slope))) * 86400.0 / 100; //convert vf to m/d		
		        DIN_Vfdenit = DIN_Vfdenit_ref * pow(DIN_Q10, ((waterT - DIN_Tref) / 10)); //m/d
		   		DIN_Vfassim = DIN_Vfassim_ref * pow(DIN_Q10, ((waterT - DIN_Tref) / 10)); //m/d
		   		DIN_ArealUdenit = DIN_Vfdenit * ConcPre_DIN; //kg/m2/d
		   		DIN_ArealUassim = DIN_Vfassim * ConcPre_DIN; //kg/m2/d					
		           }
		   		else {
		   			DIN_ArealUdenit = 0.0;
		   			DIN_Vfdenit = 0.0;
		   			DIN_ArealUassim = 0.0;
		   			DIN_Vfassim = 0.0;
		   		}
		   if (TotalArea > 0.0){
			    HL = (discharge / TotalArea) * 86400.0; //m/d
                DIN_Denit = DINTotalIn * (1 - exp(-1 * DIN_Vfdenit / HL));
		   	    DIN_Assim = DINTotalIn * (1 - exp(-1 * DIN_Vfassim / HL));
		        }
		   else {
		      	DIN_Denit = 0.0;
		      	DIN_Assim = 0.0;
		        }
		  
		   // if the result of multiple processes is more removal than is N is available, proportion each process according to relative rates
		   if ((DIN_Denit + DIN_Assim) > DINTotalIn){
			   DIN_Denit = (DIN_Denit / (DIN_Denit + DIN_Assim)) * DINTotalIn;
			   DIN_Assim = (DIN_Assim / (DIN_Denit + DIN_Assim)) * DINTotalIn;
           		}
		   	    
    	    DIN_TotalRemoval = DIN_Assim + DIN_Denit;
        	N_BiomassNew = N_Biomass + DIN_Assim - DIN_Remin;
	   		N_DeltaBiomass = N_BiomassNew - N_Biomass;			
            // if TotalVol > 0, then discharge > 0.  If
	   		if((TotalVol > 0)){
			    DINConcentration = ((DINTotalIn - DIN_TotalRemoval) / (TotalVol)); // kg / m3
			    DINConcentrationMixing = ((DINTotalInMixing) / (TotalVol)); // kg/d
	   		}
			else{
			  	DINConcentration = 0.0;
			   	DINConcentrationMixing = 0.0;
			   }
			   
			DINStorageNew = waterStorage * DINConcentration;
			DINDeltaStorage = DINStorageNew - DINStorage;
			DINFluxNew = discharge * 86400 * DINConcentration; //kg/d
			DINStorageNewMixing = waterStorage * (DINConcentrationMixing); //kg
			DINFluxNewMixing = discharge * 86400 * (DINConcentrationMixing); //kg/d
			DINDeltaStorageMixing = DINStorageNewMixing - DINStorageMixing;
			DIN_Drying = 0;
	   		DIN_DryingMixing = 0;
			    
		    }
		//if there is no dicharge
		else {
			//DINLocalIn = 0.0;
			//DINTotalIn = DINFlux + DINLocalIn + DINStorage + DIN_Remin;
			//DINTotalInMixing = DINFluxMixing + DINLocalIn + DINStorageMixing;
			ConcPre_DIN = 0.0;
		    DINFluxNew = 0.0;
		    DINStorageNew = 0.0;
		    DINDeltaStorage = -1 * DINStorage;
		    DIN_Denit = 0.0;
		    DIN_Assim = 0.0;
		    DIN_TotalRemoval = 0.0;
            DINConcentration = 0.0;
		    DINFluxNewMixing = 0.0;
		    DINStorageNewMixing = 0.0;
		    DINConcentrationMixing = 0.0;
		    DINDeltaStorageMixing = -1 * DINStorageNewMixing;
		    DIN_Drying = DINTotalIn;
		    DIN_DryingMixing = DINTotalInMixing;
		}
		
		
		MFVarSetFloat(_MDLocalIn_DINID,itemID,DINLocalIn); 
		MFVarSetFloat(_MDFlux_DINID,itemID,DINFluxNew); 
		MFVarSetFloat(_MDStorage_DINID,itemID,DINStorageNew); 
		MFVarSetFloat(_MDDeltaStorage_DINID,itemID,DINDeltaStorage); 
		MFVarSetFloat(_MDRemoval_DINID,itemID,DIN_TotalRemoval); 
		MFVarSetFloat(_MDDenit_DINID,itemID,DIN_Denit); 
		MFVarSetFloat(_MDAssim_DINID,itemID,DIN_Assim); 
		MFVarSetFloat(_MDRemin_DINID,itemID,DIN_Remin);             
		MFVarSetFloat(_MDBiomass_DINID,itemID,N_BiomassNew); 
		MFVarSetFloat(_MDDeltaBiomass_DINID,itemID,N_DeltaBiomass); 
		MFVarSetFloat(_MDConc_DINID,itemID,DINConcentration);
		MFVarSetFloat(_MDFluxMixing_DINID,itemID,DINFluxNewMixing); 
        MFVarSetFloat(_MDStorageMixing_DINID,itemID,DINStorageNewMixing); 
		MFVarSetFloat(_MDConcMixing_DINID,itemID,DINConcentrationMixing);
		MFVarSetFloat(_MDDeltaStorageMixing_DINID,itemID,DINDeltaStorageMixing); 
		MFVarSetFloat(_MDDrying_DINID, itemID, DIN_Drying);
		MFVarSetFloat(_MDDryingMixing_DINID, itemID, DIN_DryingMixing);

	//if (isnan(DINConcentration)){
//	if (discharge > 0){
   //	if (DINConcentration > 1){
	//	printf("itemID %d day %d discharge %f TotalVol %f N_Biomass %f runoffVol %f DIN_TotalRemoval %f DINFlux %f DINFluxNew %f ConcPre_DIN %f DINTotalIn %f DINLocalIn %f DINTotalInMixing %f DINConcentration %f DINConcentrationMixing %f DINStorage %f DIN_Remin %f \n", 
    //     		itemID, day, discharge, TotalVol, N_Biomass, runoffVol, DIN_TotalRemoval, DINFlux, DINFluxNew, ConcPre_DIN, DINTotalIn, DINLocalIn, DINTotalInMixing, DINConcentration, DINConcentrationMixing, DINStorage, DIN_Remin);
    // }
    if (DINTotalIn > 0.0){
	massbalance = (DINFlux + DINLocalIn + DIN_Remin - (DIN_Denit + DIN_Assim + DINDeltaStorage + DINFluxNew + DIN_Drying)) / DINTotalIn;
    } else {massbalance = 0;}
    if (DINTotalInMixing > 0.0){
	massbalanceMixing = (DINFluxMixing + DINLocalIn - (DINDeltaStorageMixing + DINFluxNewMixing + DIN_DryingMixing)) / DINTotalInMixing;
    temp = (DINFluxMixing + DINLocalIn - (DINDeltaStorageMixing + DINFluxNewMixing + DIN_DryingMixing));
    temp2 = temp / DINTotalInMixing;
    } else {massbalanceMixing = 0;}
    
    if(!isnan(discharge) && (discharge > 0.0)) {
       //if (itemID == 1245 && ((massbalance > 0.001) || (massbalanceMixing > 0.001))) {
       if ((massbalance > 0.001) || (massbalanceMixing > 0.001)) {

           printf (" MassBalance %f itemID %d Day %d DINTotalIn %f DIN_Kremin %f \n", massbalance, itemID, day, DINTotalIn, DIN_Kremin);
           printf (" MassBalanceMixing %f itemID %d Day %d DINTotalInMixing %f \n", massbalanceMixing, itemID, day, DINTotalInMixing);
    }
	}    
}

int MDBgcDINPlusBiomassRoutingDef () {
	int optID = MFUnset;
	const char *optStr;
	const char *options [] = { "none", "calculate", (char *) NULL };
     //const char *options [] = { MDNoneStr, (char *) NULL };

	if (_MDFlux_DINID != MFUnset)	return (_MDFlux_DINID);

	MFDefEntering ("DINPLUSBIOMASS Routing Calculation");
	if (((optStr = MFOptionGet (MDOptReservoirs))  == (char *) NULL) || ((optID = CMoptLookup ( options, optStr, true)) == CMfailed)) {
						CMmsgPrint(CMmsgUsrError,"Reservoir Option not specified! Option none or calculate \n");
						return CMfailed;
					}
		    if (optID==1){
		  //  	printf ("Resoption=%i\n",optID);
		    if (((_MDInResStorageID       = MFVarGetID (MDVarReservoirStorage,       "km3",     MFInput, MFState, MFInitial))  == CMfailed) ||
		        ((_MDInResStorageChangeID = MFVarGetID (MDVarReservoirStorageChange, "km3/s",   MFInput, MFState, MFBoundary)) == CMfailed))
		    	return CMfailed;
		    }	
		//TODO add parallel for irrigation    
   // Input
	if (((_MDInDischargeID           = MDDischargeDef    ()) == CMfailed) ||
        ((_MDInRiverWidthID          = MDRiverWidthDef   ()) == CMfailed) ||
	    ((_MDInRunoffVolumeID        = MDRunoffVolumeDef ()) == CMfailed) ||
	    ((_MDInRunoffID              = MDRunoffDef ()) == CMfailed) ||
	    //((_MDInSubGridNetworkID      = MDSubGridNetworkDef ()) == CMfailed) ||
        ((_MDInWTempRiverRouteID     = MDWTempRiverRouteDef ()) == CMfailed) ||
		//((_MDInRiverLightID          = MDRiverLightDef ()) == CMfailed) ||
        //((_MDInRiverGPPID            = MDRiverGPPDef ()) == CMfailed) ||
        ((_MDInWTempID               = MFVarGetID (MDVarWTemp_QxT,                   "degC",   MFInput, MFState, MFBoundary))  == CMfailed) ||
        ((_MDInRiverStorageChgID     = MFVarGetID (MDVarRiverStorageChg,             "m3/s",   MFInput, MFState, MFBoundary))  == CMfailed) ||
	    ((_MDInRiverStorageID        = MFVarGetID (MDVarRiverStorage,                "m3",     MFInput, MFState, MFInitial))   == CMfailed) ||
	    ((_MDInPointScenarioID       = MFVarGetID (MDVarPointScenario,               MFNoUnit, MFInput, MFState, MFBoundary))  == CMfailed) ||
        ((_MDNonPoint_DINID          = MFVarGetID (MDVarBGCNonPoint_DIN,             "kg/m3",  MFInput, MFState, MFBoundary))  == CMfailed) ||
	    ((_MDPointSources_DINID      = MFVarGetID (MDVarBGCPointSources_DIN,         "kg/day", MFInput, MFState, MFBoundary))  == CMfailed) ||
//TODO for feedbacks you should use initial	 - could refer to stocks or conditions like mean discharge  
	    // Output
	    ((_MDLocalIn_DINID            = MFVarGetID (MDVarDINLocalIn,                 "kg/d",   MFOutput, MFFlux,  MFBoundary))  == CMfailed) ||
	    ((_MDRemoval_DINID            = MFVarGetID (MDVarDINRemoval,                 "kg/d",   MFOutput, MFFlux,  MFBoundary))  == CMfailed) ||
	    ((_MDDenit_DINID              = MFVarGetID (MDVarDINDenit,                   "kg/d",   MFOutput, MFFlux,  MFBoundary))  == CMfailed) ||
        ((_MDAssim_DINID              = MFVarGetID (MDVarDINAssim,                   "kg/d",   MFOutput, MFFlux,  MFBoundary))  == CMfailed) ||
        ((_MDRemin_DINID              = MFVarGetID (MDVarDINRemin,                   "kg/d",   MFOutput, MFFlux,  MFBoundary))  == CMfailed) ||
        ((_MDBiomass_DINID            = MFVarGetID (MDVarDINBiomass,                 "kg",     MFOutput, MFState, MFInitial))   == CMfailed) ||
        ((_MDDeltaBiomass_DINID       = MFVarGetID (MDVarDINDeltaBiomass,            "kg/day", MFOutput, MFFlux,  MFBoundary))  == CMfailed) ||
        ((_MDConc_DINID               = MFVarGetID (MDVarDINConcentration,           "kg/m3",  MFOutput, MFState, MFBoundary))  == CMfailed) ||
	    ((_MDStorage_DINID            = MFVarGetID (MDVarDINStorage,                 "kg",     MFOutput, MFState, MFInitial))   == CMfailed) ||
	    ((_MDDeltaStorage_DINID       = MFVarGetID (MDVarDINDeltaStorage,            "kg/day", MFOutput, MFFlux,  MFBoundary))  == CMfailed) ||
        ((_MDFlux_DINID               = MFVarGetID (MDVarDINFlux  ,                  "kg/day", MFRoute,  MFFlux,  MFBoundary))  == CMfailed) ||
        ((_MDConcMixing_DINID         = MFVarGetID (MDVarDINConcentration_Mixing,    "kg/m3",  MFOutput, MFState, MFBoundary))  == CMfailed) ||
        ((_MDStorageMixing_DINID      = MFVarGetID (MDVarDINStorage_Mixing,          "kg",     MFOutput, MFState, MFInitial))   == CMfailed) ||
        ((_MDDeltaStorageMixing_DINID = MFVarGetID (MDVarDINDeltaStorage_Mixing,     "kg/day", MFOutput, MFFlux,  MFBoundary))  == CMfailed) ||
        ((_MDDrying_DINID             = MFVarGetID (MDVarDINDrying,                  "kg/day", MFOutput, MFFlux,  MFBoundary))  == CMfailed) ||
        ((_MDDryingMixing_DINID       = MFVarGetID (MDVarDINDrying_Mixing,           "kg/day", MFOutput, MFFlux,  MFBoundary))  == CMfailed) ||
        ((_MDFluxMixing_DINID         = MFVarGetID (MDVarDINFlux_Mixing ,            "kg/day", MFRoute,  MFFlux,  MFBoundary))  == CMfailed) ||
        (MFModelAddFunction (_MDDINPLUSBIOMASSRouting) == CMfailed)) return (CMfailed); 

	MFDefLeaving ("DINPLUSBIOMASS Routing Calculation");
	return (_MDFlux_DINID); 
}
