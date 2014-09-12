/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

MDSpecCond.c  - Input and Routing of Specific Conductance

shan.zuidema@unh.edu  
 * 
 * Used in conjunction with chloride.  Interprets salinazation of freshwater networks.
 * 

*******************************************************************************/
#include <stdio.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>
#include <math.h>

// input
static int _MDInBaseFlowID             = MFUnset;
static int _MDInRunoffPoolReleaseID    = MFUnset;
static int _MDInStormRunoffTotalID     = MFUnset;
static int _MDInDischargeID            = MFUnset;
static int _MDInDischarge0ID           = MFUnset;
static int _MDInRiverStorageID         = MFUnset;
static int _MDInAirTemperatureID       = MFUnset;

// Unneeded Input
static int _MDInDINFluxID               = MFUnset;
static int _MDInWTempRiverID               = MFUnset;
static int _MDInRiverWidthID               = MFUnset;

// Loading calculation
static int _MDInSubFractionID          = MFUnset;

// output  - SC = Specific Conductance
static int _MDOutLocalLoadSCID      = MFUnset;
static int _MDOutFlux_SCID          = MFUnset; 
static int _MDOutStoreWater_SCID    = MFUnset;
static int _MDOutPostConc_SCID      = MFUnset;

static void _MDSpecCond (int itemID) {

    float stormflowVol                  = 0.0;
    float baseflowVol                   = 0.0;
    float runoffpoolreleaseVol          = 0.0;
    float discharge                     = 0.0;
    float waterStorage                  = 0.0;
    
    // New Variables // 
    float Developed               = 0.0;
    float preFlux_SC              = 0.0;
    float postFlux_SC             = 0.0;
    float storeWater_SC           = 0.0;
    float postStoreWater_SC       = 0.0;
    float SCTotalIn               = 0.0;
    float postConc_SC             = 0.0;
    float massBalance_SC          = 0.0;  
    
    float localLoad_SC            = 0.0;
    
    float baseflowConc_SC         = 0.0;
    float stormflowConc_SC        = 0.0;
    float surfflowConc_SC         = 0.0;
    float clean_intrcp            = 0.0;
    float airTemperature          = 0.0;
        
    baseflowVol          = MFVarGetFloat (_MDInBaseFlowID,            itemID,0.0) * MFModelGetArea (itemID) / (MFModelGet_dt () * 1000.0); // TODO: Either need to define BaseFlowVolumeDef or calculate internally here.
    stormflowVol         = MFVarGetFloat (_MDInStormRunoffTotalID,   itemID,0.0) * MFModelGetArea (itemID) / (MFModelGet_dt () * 1000.0); 
    runoffpoolreleaseVol = MFVarGetFloat (_MDInRunoffPoolReleaseID, itemID,0.0) * MFModelGetArea (itemID) / (MFModelGet_dt () * 1000.0);
    discharge            = MFVarGetFloat (_MDInDischargeID,          itemID, 0.0); // m3/sec, discharge leaving the grid cell, after routing!
    waterStorage         = MFVarGetFloat (_MDInRiverStorageID,       itemID, 0.0); // m3/sec, storage rate remaining in grid cell at end of timestep - routed to retention
    airTemperature       = MFVarGetFloat (_MDInAirTemperatureID,     itemID, 0.0); // degrees C
    // New Variables //
    Developed            = MFVarGetFloat (_MDInSubFractionID,        itemID, 0.0);  // proportion developed land

// DEFINE IONIC CONTENT:  ic = m3*(uS/cm)
    // Assumes that ionic strength behaves linearly and conservatively.  Strictly this is only valid when
    // ionic strength is controlled by a few conservative ions.  Therefore, this is valid for salt impacted
    // streams; however, this breaks down at low ionic strength waters.  Therefore - this deep in the chloride analysis
    // we are operating with the understanding that we are only really looking at chloride impacts - not chloride itself. - SZ 6/12/2014
    preFlux_SC           = MFVarGetFloat (_MDOutFlux_SCID,        itemID, 0.0); // ic/day 
    storeWater_SC        = MFVarGetFloat (_MDOutStoreWater_SCID,  itemID, 0.0); // ic/day       
   
    /// BASEFLOW LOADING OF SPECIFIC CONDUCTANCE  ///
    ///  attributed to the groundwater and surface runoff pools - impervious runof is attributed with clean water
    /// shan.zuidema@unh.edu for details on loading estimation ///
    //baseflowConc_SC       = 25.94 + 13.32 * (Developed); // Baseflow_SC relation (0.9 Quantile based) to Developed area.  Developed should already be in PER
    clean_intrcp = 21.72;//25.94; 
    baseflowConc_SC = clean_intrcp + 9.602 * (Developed); // Baseflow SC relation (regression to zero storm flow) to Devloped area.
    // Predicting dilution rate based on AWC and Ksat does not help time-series predictions.  Though there is an apparent dependence on these factors in the lovotecs data.
    surfflowConc_SC = MDMaximum(0.64 * baseflowConc_SC,clean_intrcp); // 0.8244
     // Average fraction of baseflow SC exhibited during storm events (at daily flow exceedance of 1% P1)
    stormflowConc_SC = (airTemperature < 2.0) ? 15.0*Developed/100.*baseflowConc_SC : MDMaximum(0.20 * baseflowConc_SC,0.8*clean_intrcp); // Winter-time salting.
    // Calculate input loadings
    localLoad_SC         = (baseflowConc_SC * baseflowVol + surfflowConc_SC * runoffpoolreleaseVol + stormflowVol*stormflowConc_SC ) * 86400 ; // ic/day
    
    SCTotalIn      = localLoad_SC + preFlux_SC  + storeWater_SC;         // ic/day  
    
    if (discharge > 0.0000001) {
        // Calculate pre-tranformation concentration - for cells with accumulated discharge
        postConc_SC    = SCTotalIn / (discharge * 86400.); // uS/cm
    } else {
        // Force concentrations to local values for minimal accumulation / dessicated cells
        postConc_SC     = baseflowConc_SC; // Force negligible discharge to the baseflow concentration
    }
    //Calculates the fluxes/storage for the mixing (appropriate for speccond) case
    postFlux_SC  = (discharge * MDConst_m3PerSecTOm3PerDay) * postConc_SC ;         // ic/day
    postStoreWater_SC  = (waterStorage ) * postConc_SC ;      // ic/day
    
    // Calculates the mass balances
    massBalance_SC  = (SCTotalIn - (postFlux_SC + postStoreWater_SC )) / SCTotalIn;
    
    // Print mass balance errors
    if (MFDateGetCurrentYear() > 0){
        if ( (massBalance_SC > 0.001)) {
           printf("itemID = %d, %d-%d-%d, MB_SC = %f\n",itemID, MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), massBalance_SC);
           printf("\tTotalIn: %f, Local: %f , UpFlux: %f, InStore: %f\n",SCTotalIn,localLoad_SC,preFlux_SC,storeWater_SC);
           printf("\tDownFlux: %f, OutStore: %f , flowPathRemove: %f\n",postFlux_SC,postStoreWater_SC,0.0);
        }
        // Debug prints
        //if ((itemID == 8310)) {
            //printf("itemID = %d, order %f, Store_SC %f, Store %f Store2 %f StoreDelta %f StorePrev %f StoreSCconc %f \n ",itemID, order, storeWater_SC, waterStorage*86400.,(discharge - dischargePre)*86400.,waterStorageChange*86400.,waterStoragePrev, storeWater_SC / waterStoragePrev ) ;
        //}
        //if (postConc_SC < 21.70){
            //printf("itemID = %d, order %f, Store_SC %f, Store %f Store2 %f StoreDelta %f StorePrev %f StoreSCconc %f \n ",itemID, order, storeWater_SC, waterStorage*86400.,(discharge - dischargePre)*86400.,waterStorageChange*86400.,waterStoragePrev, storeWater_SC / waterStoragePrev ) ;
          //  printf("itemID %d ro %f bf %f srf %f str %f bfSC %f srfSC %f strSC %f \n",itemID,runoffVol,baseflowVol,runoffpoolreleaseVol,stormflowVol,baseflowConc_SC,stormflowConc_SC,clean_intrcp);
        //}
    }
    
    // Set Output
    MFVarSetFloat (_MDOutFlux_SCID,              itemID, postFlux_SC);
    MFVarSetFloat (_MDOutLocalLoadSCID,             itemID, localLoad_SC);
    MFVarSetFloat (_MDOutPostConc_SCID,           itemID, postConc_SC);
    MFVarSetFloat (_MDOutStoreWater_SCID,        itemID, postStoreWater_SC);
}

enum {MDcalculate, MDinput, MDinput2, MDnone};

int MDSpecCondDef () {

    int  optID = MFUnset;											//SZ 08212014
    const char *optStr, *optName = MDOptSpecConductance;								//SZ 08212014
    const char *options [] = { MDCalculateStr, MDInputStr, MDInput2Str, MDNoneStr, (char *) NULL };		//SZ 08212014

	MFDefEntering ("Specific Conductance Routing");
	
    if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);  //SZ 08212014

    switch (optID) {	//SZ 08212014

        case MDcalculate:
        if (
            ((_MDInDINFluxID                    = MDDINDef ()) == CMfailed) ||    
	    ((_MDInWTempRiverID                 = MFVarGetID (MDVarWTemp_QxT,              "degC",      MFInput, MFState, MFBoundary)) == CMfailed)   ||
            ((_MDInRiverWidthID                 = MDRiverWidthDef ())     == CMfailed) ||
//          ((_MDInLitterFall_POCID             = MDLitterFallDef ()) == CMfailed) ||
//          ((_MDInLocalLoad_DOCID              = MFVarGetID (MDVarLocalLoadDOC,                             "kg/d",    MFInput,  MFFlux,  MFBoundary))   == CMfailed) ||
            ((_MDInDischarge0ID                 = MFVarGetID (MDVarDischarge0,                               "m3/s",    MFInput,  MFState, MFBoundary))   == CMfailed) ||	
	    ((_MDInDischargeID                  = MFVarGetID (MDVarDischarge,                                "m3/s",    MFInput,  MFState, MFBoundary))   == CMfailed) ||
            ((_MDInRiverStorageID               = MFVarGetID (MDVarRiverStorage,                           "m3/day",    MFInput,  MFState, MFInitial))    == CMfailed) ||	
                ((_MDInBaseFlowID                   = MFVarGetID (MDVarBaseFlow,                                  "mm",    MFInput,  MFFlux,MFBoundary))    == CMfailed) ||
            ((_MDInStormRunoffTotalID           = MFVarGetID (MDVarStormRunoffTotal,                          "mm",   MFInput, MFFlux, MFBoundary))  == CMfailed) ||
            ((_MDInRunoffPoolReleaseID          = MFVarGetID (MDVarRunoffPoolRelease,                        "mm",  MFInput, MFFlux, MFBoundary))  == CMfailed) ||
            ((_MDInAirTemperatureID             = MFVarGetID (MDVarAirTemperature,         "degC",       MFInput,  MFState, MFBoundary)) == CMfailed) ||
            ((_MDInSubFractionID                = MFVarGetID (MDVarLandUseSpatialSub,                         "-",    MFInput,  MFState, MFBoundary))   == CMfailed) ||
            ((_MDOutLocalLoadSCID               = MFVarGetID (MDVarLocalLoadSC,               "ic/d",   MFOutput,  MFFlux,  MFBoundary))   == CMfailed) ||
             ((_MDOutPostConc_SCID               = MFVarGetID (MDVarPostSpecCond,    	      "uS/cm",   MFOutput,  MFFlux, MFBoundary))   == CMfailed) ||	               
            ((_MDOutStoreWater_SCID             = MFVarGetID (MDVarStoreWaterSC,              "ic/day",   MFOutput,  MFState, MFInitial))    == CMfailed) ||	
            ((_MDOutFlux_SCID                   = MFVarGetID (MDVarFluxSC,                    "ic/day",   MFRoute,   MFFlux,  MFBoundary))   == CMfailed) ||	  
            (MFModelAddFunction (_MDSpecCond) == CMfailed)) return (CMfailed);
            break;

        default: 
            MFOptionMessage (optName, optStr, options); return (CMfailed);
    }
    
	MFDefLeaving ("Specific Conductance Routing");
    return (_MDOutFlux_SCID);
}
