/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

MDDOC.c  - Input and Routing of DOC 

rob.stewart@unh.edu  

*******************************************************************************/
#include <stdio.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>
#include <math.h>

// input

static int _MDInDischargeID            = MFUnset;
static int _MDInDischarge0ID           = MFUnset;
static int _MDInRunoffVolumeID         = MFUnset;
static int _MDInRunoffID               = MFUnset;
static int _MDInRiverStorageChgID      = MFUnset;
static int _MDInRiverStorageID         = MFUnset;
static int _MDInLocalLoad_DOCID        = MFUnset;
static int _MDInLitterFall_POCID       = MFUnset;
static int _MDFlux_DIN_denitID         = MFUnset;
static int _MDInTotalEvaporationID     = MFUnset;
static int _MDInWetlandsID             = MFUnset;
static int _MDInImpFractionID          = MFUnset;
static int _MDInHumanLandUseID         = MFUnset;
static int _MDInRiverWidthID           = MFUnset;
static int _MDInRiverOrderID           = MFUnset;
static int _MDInDOCmID                 = MFUnset;
static int _MDInDOCbID                 = MFUnset;
static int _MDInClmID                  = MFUnset;
static int _MDInClbID                  = MFUnset;
static int _MDInDINmID                 = MFUnset;
static int _MDInDINbID                 = MFUnset;



// output

static int _MDOutLocalLoadDOCID        = MFUnset;
static int _MDOutLocalLoadDINID        = MFUnset;
static int _MDOutLocalLoadClID         = MFUnset;

static int _MDFluxMixing_DOCID         = MFUnset;
static int _MDFluxMixing_POCID         = MFUnset;
static int _MDFluxMixing_DINID         = MFUnset; 
static int _MDFluxMixing_ClID          = MFUnset; 
static int _MDFlux_DINID               = MFUnset; 
static int _MDFlux_ClID                = MFUnset; 
static int _MDFlux_DOCID               = MFUnset; 
static int _MDOutConcMixing_DOCID      = MFUnset;
static int _MDOutConcMixing_POCID      = MFUnset;
static int _MDOutConcMixing_DINID      = MFUnset;
static int _MDOutConcMixing_ClID       = MFUnset;
static int _MDStoreWaterMixing_DOCID   = MFUnset;
static int _MDStoreWaterMixing_POCID   = MFUnset;
static int _MDStoreWaterMixing_DINID   = MFUnset;
static int _MDStoreWaterMixing_ClID    = MFUnset;
static int _MDStoreWater_DOCID         = MFUnset;
static int _MDStoreWater_DINID         = MFUnset;
static int _MDStoreWater_ClID          = MFUnset;
static int _MDDeltaStorageMixing_DOCID = MFUnset;
static int _MDDeltaStorageMixing_POCID = MFUnset;
static int _MDPostConcMixing_DOCID     = MFUnset;
static int _MDPostConcMixing_POCID     = MFUnset;
static int _MDOutPostConc_DOCID        = MFUnset;
static int _MDOutPostConc_DINID        = MFUnset;
static int _MDOutPostConc_ClID         = MFUnset;
static int _MDRunoffConc_DOCID         = MFUnset;

static void _MDDOC (int itemID) {

    float runoffVol                     = 0.0;  // m3/s
    float waterStorage                  = 0.0;  // m3/s
    float waterStorageChange            = 0.0;  // m3/s
    float localLoad_DOC                 = 0.0;  // kg/d
    float localLoad_POC                 = 0.0;  // kg/d
    float preFluxMixing_DOC             = 0.0;  // kg/d
    float preFluxMixing_POC             = 0.0;  // kg/d
    float postFluxMixing_DOC            = 0.0;  // kg/d
    float postFluxMixing_POC            = 0.0;  // kg/d
    float storeWaterMixing_DOC          = 0.0;  // kg/d
    float storeWaterMixing_POC          = 0.0;  // kg/d
    float postStoreWaterMixing_DOC      = 0.0;  // kg/d
    float postStoreWaterMixing_POC      = 0.0;  // kg/d
    float discharge                     = 0.0;  // m3/s
    float dischargePre                  = 0.0;  // m3/s
    float waterStoragePrev              = 0.0;  // m3/s
    float DOCTotalInMixing              = 0.0;  // kg/d
    float POCTotalInMixing              = 0.0;  // kg/d
    float runoffConc_DOC                = 0.0;  // mg/L
    float waterTotalVolume              = 0.0;	// m3/d
    float DIN                           = 0.0;  // kg/d
    float flowPathRemovalMixing_DOC     = 0.0;  // kg/d
    float flowPathRemovalMixing_POC     = 0.0;  // kg/d
    float postConcMixing_DOC            = 0.0;  // mg/L
    float postConcMixing_POC            = 0.0;  // mg/L
    float massBalanceMixing_DOC         = 0.0;  // kg/d
    float massBalanceMixing_POC         = 0.0;  // kg/d
    float preConcMixing_DOC             = 0.0;  // mg/L
    float preConcMixing_POC             = 0.0;  // mg/L
    float litterFall_POC                = 0.0;  // kg/d
    float wetlands                      = 0.0;  // proportion wetlands
    
    // New Variables // 
    
    float imp                           = 0.0;  // proportion impervious
    float human                         = 0.0;  // proportion human land use
    float preFluxMixing_DIN             = 0.0;  // kg/d
    float preFluxMixing_Cl              = 0.0;  // kg/d
    float postFluxMixing_DIN            = 0.0;  // kg/d
    float postFluxMixing_Cl             = 0.0;  // kg/d
    float storeWaterMixing_DIN          = 0.0;  // kg/d
    float storeWaterMixing_Cl           = 0.0;  // kg/d
    float postStoreWaterMixing_DIN      = 0.0;  // kg/d
    float postStoreWaterMixing_Cl       = 0.0;  // kg/d
    float DINTotalInMixing              = 0.0;  // kg/d
    float ClTotalInMixing               = 0.0;  // kg/d
    float flowPathRemovalMixing_DIN     = 0.0;  // kg/d
    float flowPathRemovalMixing_Cl      = 0.0;  // kg/d
    float postConcMixing_DIN            = 0.0;  // kg/d
    float postConcMixing_Cl             = 0.0;  // kg/d
    float massBalanceMixing_DIN         = 0.0;  // kg/d
    float massBalanceMixing_Cl          = 0.0;  // kg/d
    float preConcMixing_DIN             = 0.0;  // kg/d
    float preConcMixing_Cl              = 0.0;  // kg/d
    float preFlux_DIN                   = 0.0;  // kg/d
    float storeWater_DIN                = 0.0;  // kg/d
    float preFlux_Cl                    = 0.0;  // kg/d
    float storeWater_Cl                 = 0.0;  // kg/d
    float preFlux_DOC                   = 0.0;  // kg/d
    float storeWater_DOC                = 0.0;  // kg/d
    float runoffConc_DIN                = 0.0;  // mg/L
    float runoffConc_Cl                 = 0.0;  // mg/L
    float DOCTotalIn                    = 0.0;  // kg/d
    float DINTotalIn                    = 0.0;  // kg/d
    float ClTotalIn                     = 0.0;  // kg/d
    float preConc_DIN                   = 0.0;  // mg/L
    float preConc_DOC                   = 0.0;  // mg/L
    float preConc_Cl                    = 0.0;  // mg/L
    float Vf                            = 35.0; // m/yr
    float HL                            = 0.0;  // m/d
    float width                         = 0.0;  // m
    float removal                       = 0.0;  // proportional removal
    float totalMassRemoved_DIN          = 0.0;  // kg/d
    float totalMassRemoved_DOC          = 0.0;  // kg/d
    float totalMassRemoved_Cl           = 0.0;  // kg/d
    float flowPathRemoval_DOC           = 0.0;  // kg/d
    float flowPathRemoval_DIN           = 0.0;  // kg/d
    float flowPathRemoval_Cl            = 0.0;  // kg/d
    float postConc_DIN                  = 0.0;  // mg/L
    float postConc_DOC                  = 0.0;  // mg/L
    float postConc_Cl                   = 0.0;  // mg/L
    float postFlux_DIN                  = 0.0;  // kg/d
    float postFlux_DOC                  = 0.0;  // kg/d
    float postFlux_Cl                   = 0.0;  // kg/d
    float postStoreWater_DOC            = 0.0;  // kg/d
    float postStoreWater_DIN            = 0.0;  // kg/d
    float postStoreWater_Cl             = 0.0;  // kg/d
    float massBalance_DOC               = 0.0;  // kg/d
    float massBalance_DIN               = 0.0;  // kg/d
    float massBalance_Cl                = 0.0;  // kg/d
    float localLoad_DIN                 = 0.0;  // kg/d
    float localLoad_Cl                  = 0.0;  // kg/d
    float order                         = 0.0;  // river order
    float length                        = 0.0;  // m
    float ClTotalInMixing_Conc          = 0.0;  // kg/d
    
    float DIN_m                         = 0.0;  // slope DIN
    float DIN_b                         = 0.0;  // intercept DIN
    float Cl_m                          = 0.0;  // slope chloride
    float Cl_b                          = 0.0;  // intercept chloride
    float DOC_m                         = 0.0;  // slope DOC
    float DOC_b                         = 0.0;  // intercept DOC
    
 //             localLoad_DOC  	     = MFVarGetFloat (_MDInLocalLoad_DOCID,      itemID, 0.0); // kg/day TEM inputs
 //             localLoad_POC        = MFVarGetFloat (_MDInLitterFall_POCID,     itemID, 0.0); // kg/day TEM inputs
 //             preFluxMixing_POC    = MFVarGetFloat (_MDFluxMixing_POCID,       itemID, 0.0); // kg/day TEM inputs
 //             storeWaterMixing_POC = MFVarGetFloat (_MDStoreWaterMixing_POCID, itemID, 0.0); // kg/day TEM inputs
 
                runoffVol            = MFVarGetFloat (_MDInRunoffVolumeID,       itemID, 0.0); // m3/s
		waterStorageChange   = MFVarGetFloat (_MDInRiverStorageChgID,    itemID, 0.0); // m3/s
		waterStorage         = MFVarGetFloat (_MDInRiverStorageID,       itemID, 0.0); // m3/s 
                preFluxMixing_DOC    = MFVarGetFloat (_MDFluxMixing_DOCID,       itemID, 0.0); // kg/day 
                storeWaterMixing_DOC = MFVarGetFloat (_MDStoreWaterMixing_DOCID, itemID, 0.0); // kg/day 
                discharge            = MFVarGetFloat (_MDInDischargeID,          itemID, 0.0); // m3/sec, discharge leaving the grid cell, after routing!
                dischargePre	     = MFVarGetFloat (_MDInDischarge0ID,         itemID, 0.0); // m3/sec, discharge from upstream PLUS local runoff, before routing!
                DIN                  = MFVarGetFloat (_MDFlux_DIN_denitID,       itemID, 0.0); // to start DIN model 
                wetlands             = MFVarGetFloat (_MDInWetlandsID,           itemID, 0.0); // proportion wetlands
                order                = MFVarGetFloat (_MDInRiverOrderID,         itemID, 0.0); // strahler order
                DIN_m                = MFVarGetFloat (_MDInDINmID,               itemID, 0.0); // slope DIN
                DIN_b                = MFVarGetFloat (_MDInDINbID,               itemID, 0.0); // intercept DIN
                DOC_m                = MFVarGetFloat (_MDInDOCmID,               itemID, 0.0); // slope DOC
                DOC_b                = MFVarGetFloat (_MDInDOCbID,               itemID, 0.0); // intercept DOC
                Cl_m                 = MFVarGetFloat (_MDInClmID,                itemID, 0.0); // slope chloride
                Cl_b                 = MFVarGetFloat (_MDInClbID,                itemID, 0.0); // intercept chloride

                // New Variables //
                
                human                = MFVarGetFloat (_MDInHumanLandUseID,       itemID, 0.0);  // percent human land use
                imp                  = MFVarGetFloat (_MDInImpFractionID,        itemID, 0.0);  // proportion impervious surface
                preFluxMixing_DIN    = MFVarGetFloat (_MDFluxMixing_DINID,       itemID, 0.0);  // kg/day 
                preFluxMixing_Cl     = MFVarGetFloat (_MDFluxMixing_ClID,        itemID, 0.0);  // kg/day 
                storeWaterMixing_DIN = MFVarGetFloat (_MDStoreWaterMixing_DINID, itemID, 0.0);  // kg/day 
                storeWaterMixing_Cl  = MFVarGetFloat (_MDStoreWaterMixing_ClID,  itemID, 0.0);  // kg/day       
                preFlux_DIN          = MFVarGetFloat (_MDFlux_DINID,             itemID, 0.0);	// kg/day RJS 091108
                storeWater_DIN       = MFVarGetFloat (_MDStoreWater_DINID,       itemID, 0.0);	// kg/day RJS 091108
                preFlux_Cl           = MFVarGetFloat (_MDFlux_ClID,              itemID, 0.0);	// kg/day RJS 091108
                storeWater_Cl        = MFVarGetFloat (_MDStoreWater_ClID,        itemID, 0.0);	// kg/day RJS 091108
                preFlux_DOC          = MFVarGetFloat (_MDFlux_DOCID,             itemID, 0.0);	// kg/day RJS 091108
                storeWater_DOC       = MFVarGetFloat (_MDStoreWater_DOCID,       itemID, 0.0);	// kg/day RJS 091108
                width	             = MFVarGetFloat (_MDInRiverWidthID,    	 itemID, 0.0);	// m			// moved here 031209
                length               = MFModelGetLength(itemID) / 1000;
                
                
                
                waterStoragePrev     = waterStorage - waterStorageChange;                                    // m3/sec     
                waterTotalVolume     = discharge * 86400;                                                    // m3/d
                HL                   = discharge > 0.0001 ? discharge / (width * length) * 86400 : 0.0;      // m/d
 //               runoffConc_DOC	     = runoffVol > 0.0 ? (localLoad_DOC * 1000000) / (runoffVol * 86400 * 1000) : 0.0;	// TEM INPUTS
 //               DOCTotalInMixing     = localLoad_DOC + preFluxMixing_DOC + storeWaterMixing_DOC;        // kg/day                TEM INPUTS
  //              if (itemID == 100) printf("HL = %f, width = %f, discharge = %f, length = %f\n", HL, width, discharge, length);
  //              runoffConc_DOC       = (5.4 + (60.0 * wetlands * 100)) / 1000000 * 12 * 1000;               
  //              DOCTotalInMixing     = (runoffConc_DOC * runoffVol * 86400 / 1000) + preFluxMixing_DOC + storeWaterMixing_DOC;        // kg/day  
  //              POCTotalInMixing     = localLoad_POC + preFluxMixing_POC + storeWaterMixing_POC;        // kg/day
 
                runoffConc_DOC         = pow(10, (DOC_m * (wetlands * 100)) + DOC_b) > 0.0 ? pow(10, (DOC_m * (wetlands * 100)) + DOC_b) : 0.0; // mg/L
                runoffConc_DIN         = pow(10, (DIN_m * (human) + DIN_b)) > 0.0 ? pow(10, (DIN_m * (human) + DIN_b)) : 0.0;                   // mg/L
                runoffConc_Cl          = (Cl_m * (imp * 100)) + Cl_b;                                                                           // mg/L
                
//                runoffConc_DOC         = 0.065 + 0.671 * (wetlands * 100);
//                runoffConc_DIN         = (-0.016 + 0.013 * human) > 0.0 ? -0.016 + 0.013 * human : 0.0;
//                runoffConc_Cl          = 3.83 + 8.9 * (imp * 100);
              
                localLoad_DOC        = runoffConc_DOC * runoffVol * 86400 / 1000;                       // kg/d
                localLoad_DIN        = runoffConc_DIN * runoffVol * 86400 / 1000;                       // kg/d
                localLoad_Cl         = runoffConc_Cl * runoffVol * 86400 / 1000;                        // kg/d
                
                DOCTotalInMixing     = localLoad_DOC + preFluxMixing_DOC + storeWaterMixing_DOC;        // kg/d  
                DINTotalInMixing     = localLoad_DIN + preFluxMixing_DIN + storeWaterMixing_DIN;        // kg/d  
                ClTotalInMixing      = localLoad_Cl + preFluxMixing_Cl  + storeWaterMixing_Cl;          // kg/d  
                
                ClTotalInMixing_Conc = ClTotalInMixing / waterTotalVolume * 1000;                       // mg/L
                
                DOCTotalIn           = localLoad_DOC + preFlux_DOC + storeWater_DOC;                    // kg/day  
                DINTotalIn           = localLoad_DIN + preFlux_DIN + storeWater_DIN;                    // kg/day  
                ClTotalIn            = localLoad_Cl + preFlux_Cl  + storeWater_Cl;                      // kg/day  

    //            printf("waterStoragePrev = %f, waterStorage = %f, waterStorageChange = %f\n", waterStoragePrev, waterStorage, waterStorageChange);
                
                if (discharge > 0.0000001) {
                
                  preConcMixing_DOC    = DOCTotalInMixing / waterTotalVolume * 1000;                    // mg/L      
                  preConcMixing_DIN    = DINTotalInMixing / waterTotalVolume * 1000;                    // mg/L                        
                  preConcMixing_Cl     = ClTotalInMixing / waterTotalVolume * 1000;                     // mg/L
                   
                  preConc_DOC    = DOCTotalIn / waterTotalVolume * 1000;                                // mg/L          
                  preConc_DIN    = DINTotalIn / waterTotalVolume * 1000;                                // mg/L                                             
                  preConc_Cl     = ClTotalIn  / waterTotalVolume * 1000;                                // mg/L
                  
                        if (order > 2.0) {
                  
                                removal = 1.0 - pow(2.718281828, -1.0 * (Vf / 365) / HL);               // proportional removal

                                totalMassRemoved_DIN = removal * DINTotalIn;                            // kg/d
                                totalMassRemoved_DOC = removal * DOCTotalIn;                            // kg/d
                                totalMassRemoved_Cl  = removal * ClTotalIn;                             // kg/d
                                        
 //                             preConcMixing_DOC    = DOCTotalInMixing / waterTotalVolume * 1000;            // TEM version
 //                             preConcMixing_POC    = POCTotalInMixing / waterTotalVolume * 1000;            // TEM version                                                                     
 //                             postConcMixing_DOC   = DOCTotalInMixing / (waterTotalVolume - totalEvap) * 1000;	// TEM version		    // mg/L
 //                             postConcMixing_POC   = POCTotalInMixing / (waterTotalVolume - totalEvap) * 1000;      // TEM version
          
                        }
                  
                   postConcMixing_DOC   = DOCTotalInMixing / waterTotalVolume * 1000;                   // mg/L
                   postConcMixing_DIN   = DINTotalInMixing / waterTotalVolume * 1000;                   // mg/L
                   postConcMixing_Cl    = ClTotalInMixing / waterTotalVolume * 1000;                    // mg/L
                                   
                }
                
                else {
                  
                  flowPathRemoval_DOC       = DOCTotalIn;                       // kg/d
                  flowPathRemoval_DIN       = DINTotalIn;                       // kg/d
                  flowPathRemoval_Cl        = ClTotalIn;                        // kg/d
                    
                  flowPathRemovalMixing_DOC = DOCTotalInMixing;                 // kg/d
                  flowPathRemovalMixing_DIN = DINTotalInMixing;                 // kg/d
                  flowPathRemovalMixing_Cl  = ClTotalInMixing;                  // kg/d
                  
                  postConc_DOC        = 0.0;                                    // mg/L                            
                  postConc_DIN        = 0.0;                                    // mg/L
                  postConc_Cl         = 0.0;                                    // mg/L
                  
                  postConcMixing_DOC        = 0.0;                              // mg/L
                  postConcMixing_DIN        = 0.0;                              // mg/L
                  postConcMixing_Cl         = 0.0;                              // mg/L
                    
//                flowPathRemovalMixing_DOC = DOCTotalInMixing;         // TEM version
//                flowPathRemovalMixing_POC = POCTotalInMixing;         // TEM version
                
//                postConcMixing_DOC        = 0.0;                      // TEM version
//                postConcMixing_POC        = 0.0;                      // TEM version
                
                }
   
                postConc_DOC   = discharge > 0.0000001 ? (DOCTotalIn - totalMassRemoved_DOC - flowPathRemoval_DOC) / waterTotalVolume * 1000 : 0.0;     // mg/L
                postConc_DIN   = discharge > 0.0000001 ? (DINTotalIn - totalMassRemoved_DIN - flowPathRemoval_DIN) / waterTotalVolume * 1000 : 0.0;     // mg/L
                postConc_Cl    = discharge > 0.0000001 ? (ClTotalIn - totalMassRemoved_Cl - flowPathRemoval_Cl) / waterTotalVolume * 1000 : 0.0;        // mg/L 
                
                postFlux_DOC 	   = (discharge * MDConst_m3PerSecTOm3PerDay) * postConc_DOC / 1000;        // kg/day
                postFlux_DIN       = (discharge * MDConst_m3PerSecTOm3PerDay) * postConc_DIN / 1000;        // kg/day
                postFlux_Cl        = (discharge * MDConst_m3PerSecTOm3PerDay) * postConc_Cl / 1000;         // kg/day

                postStoreWater_DOC = (waterStorage * MDConst_m3PerSecTOm3PerDay) * postConc_DOC / 1000;     // kg/day
                postStoreWater_DIN = (waterStorage * MDConst_m3PerSecTOm3PerDay) * postConc_DIN / 1000;     // kg/day
                postStoreWater_Cl  = (waterStorage * MDConst_m3PerSecTOm3PerDay) * postConc_Cl / 1000;      // kg/day

                postFluxMixing_DOC 	 = (discharge * MDConst_m3PerSecTOm3PerDay) * postConcMixing_DOC / 1000;	 // kg/day
                postFluxMixing_DIN       = (discharge * MDConst_m3PerSecTOm3PerDay) * postConcMixing_DIN / 1000;         // kg/day
                postFluxMixing_Cl        = (discharge * MDConst_m3PerSecTOm3PerDay) * postConcMixing_Cl / 1000;          // kg/day

                postStoreWaterMixing_DOC = (waterStorage * MDConst_m3PerSecTOm3PerDay) * postConcMixing_DOC / 1000;	 // kg/day
                postStoreWaterMixing_DIN = (waterStorage * MDConst_m3PerSecTOm3PerDay) * postConcMixing_DIN / 1000;      // kg/day
                postStoreWaterMixing_Cl  = (waterStorage * MDConst_m3PerSecTOm3PerDay) * postConcMixing_Cl / 1000;       // kg/day

                massBalanceMixing_DOC = (DOCTotalInMixing - (postFluxMixing_DOC + postStoreWaterMixing_DOC + flowPathRemovalMixing_DOC)) / DOCTotalInMixing;    // proportion of total kg in
                massBalanceMixing_DIN = (DINTotalInMixing - (postFluxMixing_DIN + postStoreWaterMixing_DIN + flowPathRemovalMixing_DIN)) / DINTotalInMixing;    // proportion of total kg in
                massBalanceMixing_Cl  = (ClTotalInMixing - (postFluxMixing_Cl + postStoreWaterMixing_Cl + flowPathRemovalMixing_Cl));                           // proportion of total kg in

                massBalance_DOC = DOCTotalIn > 0.00001 ? (DOCTotalIn - (postFlux_DOC + postStoreWater_DOC + flowPathRemoval_DOC + totalMassRemoved_DOC)) / DOCTotalIn : 0.0;    // proportion of total kg in
                massBalance_DIN = DINTotalIn > 0.00001 ? (DINTotalIn - (postFlux_DIN + postStoreWater_DIN + flowPathRemoval_DIN + totalMassRemoved_DIN)) / DINTotalIn : 0.0;    // proportion of total kg in
                massBalance_Cl  = ClTotalIn > 0.00001 ? (ClTotalIn - (postFlux_Cl + postStoreWater_Cl + flowPathRemoval_Cl + totalMassRemoved_Cl)) : 0.0;                       // proportion of total kg in

 //               if ((massBalance_DOC > 0.003) || (massBalance_DIN > 0.003) || (massBalance_Cl > 0.003)) {
 //                   printf("itemID = %d, %d-%d-%d, MB_DOC = %f, MB_DIN = %f, MB_Cl = %f\n",itemID, MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), massBalance_DOC, massBalance_DIN, massBalance_Cl);                  
 //                   printf("Q = %f, DINTotalIn=%f, postFlux_DIN=%f, postStoreWater_DIN=%f, flowPathRemoval_DIN=%f\n",discharge,DINTotalIn,postFlux_DIN,postStoreWater_DIN,flowPathRemoval_DIN);
  //              }              
  //              if ((massBalanceMixing_DOC > 0.00003) || (massBalanceMixing_DIN > 0.00003) || (massBalanceMixing_Cl > 0.00003)) {
  //                  printf("itemID = %d, %d-%d-%d, MIXING_DOC = %f, MIXING_DIN = %f, MIXING_Cl = %f\n",itemID, MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), massBalanceMixing_DOC, massBalanceMixing_DIN, massBalanceMixing_Cl);                  
  //                  printf("Q = %f, DOCTotalInMix = %f, postFluxMix_DOC = %f, postStoreWaterMix_DOC = %f, flowPathRemovalMix_DOC = %f\n", discharge, DOCTotalInMixing, postFluxMixing_DOC, postStoreWaterMixing_DOC, flowPathRemovalMixing_DOC);
  //              }    
                
 //               if ((itemID == 1576) || (itemID == 1568)) {
 //                printf("ID = %d, %d-%d-%d Q = %f, QPre = %f, ROVol = %f, runoffConc_Cl = %f, localLoad_Cl = %f, diff1 = %f, diff2 = %f\n",itemID, MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(),discharge, dischargePre, runoffVol, runoffConc_Cl, localLoad_Cl, postConcMixing_Cl - ClTotalInMixing_Conc, postFluxMixing_Cl - ClTotalInMixing);
 //                printf("preFluxMixing_Cl = %f, ClTotalInMixing = %f, preConcMixing_Cl = %f, postFluxMixing_Cl = %f, postConcMixing_Cl = %f, MB = %f\n", preFluxMixing_Cl, ClTotalInMixing, preConcMixing_Cl, postFluxMixing_Cl, postConcMixing_Cl,massBalanceMixing_Cl);
                
 //               }
                
 //               postFluxMixing_DOC 	 = (discharge * MDConst_m3PerSecTOm3PerDay) * postConcMixing_DOC / 1000;		// kg/day
 //               postFluxMixing_POC       = (discharge * MDConst_m3PerSecTOm3PerDay) * postConcMixing_POC / 1000;         // kg/day
 //               postStoreWaterMixing_DOC = (waterStorage * MDConst_m3PerSecTOm3PerDay) * postConcMixing_DOC / 1000;	// kg/day
 //               postStoreWaterMixing_POC = (waterStorage * MDConst_m3PerSecTOm3PerDay) * postConcMixing_POC / 1000;      // kg/day

 //               massBalanceMixing_DOC = ((localLoad_DOC + preFluxMixing_DOC + storeWaterMixing_DOC) - (postFluxMixing_DOC + postStoreWaterMixing_DOC + flowPathRemovalMixing_DOC)) / (localLoad_DOC + storeWaterMixing_DOC + preFluxMixing_DOC);
 //               massBalanceMixing_POC = ((localLoad_POC + preFluxMixing_POC + storeWaterMixing_POC) - (postFluxMixing_POC + postStoreWaterMixing_POC + flowPathRemovalMixing_POC)) / (localLoad_POC + storeWaterMixing_POC + preFluxMixing_POC);
 
 //               if ((itemID == 31) || (itemID == 32)) {
 //               if (((massBalanceMixing_DOC > 0.0003) || (massBalanceMixing_POC > 0.0003)) && (localLoad_DOC + storeWaterMixing_DOC + preFluxMixing_DOC > 0.000001)) {
 //                   printf("******itemID = %d, y = %d, m = %d, d = %d, massBalance_DOC = %f, massBalance_POC = %f, preConc_DOC = %f, preConc_POC = %f\n", itemID, MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), massBalanceMixing_DOC, massBalanceMixing_POC, preConcMixing_DOC, preConcMixing_POC);
 //                   printf("Evap = %f, dischargePre = %f, discharge = %f, runoffVol = %f, waterStoragePrev = %f, waterStorage = %f, waterTotalVol = %f\n", totalEvap, dischargePre, discharge, runoffVol, waterStoragePrev, waterStorage, waterTotalVolume);
 //                   printf("totalIn_DOC = %f, totalIn_POC = %f, localLoad_DOC = %f, localLoad_POC = %f, preFlux_DOC = %f, preFlux_POC = %f, storeWater_DOC = %f, storeWater_POC = %f\n", DOCTotalInMixing, POCTotalInMixing, localLoad_DOC, localLoad_POC, preFluxMixing_DOC, preFluxMixing_POC, storeWaterMixing_DOC, storeWaterMixing_POC);
 //                   printf("postConc_DOC = %f, postConc_POC = %f, postFlux_DOC = %f, postFlux_POC = %f, postStore_DOC = %f, postStore_POC = %f, flowPathRem_DOC = %f, flowPathRem_POC = %f\n", postConcMixing_DOC, postConcMixing_POC, postFluxMixing_DOC, postFluxMixing_POC, postStoreWaterMixing_DOC, postStoreWaterMixing_POC, flowPathRemovalMixing_DOC, flowPathRemovalMixing_POC);
 //               }
  //              }
               
  //              MFVarSetFloat (_MDFluxMixing_DOCID,             itemID, postFluxMixing_DOC);
  //              MFVarSetFloat (_MDFluxMixing_POCID,             itemID, postFluxMixing_POC);
  //              MFVarSetFloat (_MDOutConcMixing_DOCID,          itemID, postConcMixing_DOC);
  //              MFVarSetFloat (_MDOutConcMixing_POCID,          itemID, postConcMixing_POC);
  //             MFVarSetFloat (_MDStoreWaterMixing_DOCID,       itemID, postStoreWaterMixing_DOC);
  //              MFVarSetFloat (_MDStoreWaterMixing_POCID,       itemID, postStoreWaterMixing_POC);
  //              MFVarSetFloat (_MDRunoffConc_DOCID,             itemID, runoffConc_DOC);
  
                MFVarSetFloat (_MDFluxMixing_DOCID,             itemID, postFluxMixing_DOC);
                MFVarSetFloat (_MDFluxMixing_DINID,             itemID, postFluxMixing_DIN);
                MFVarSetFloat (_MDFluxMixing_ClID,              itemID, postFluxMixing_Cl);
                MFVarSetFloat (_MDFlux_DOCID,                   itemID, postFlux_DOC);
                MFVarSetFloat (_MDFlux_DINID,                   itemID, postFlux_DIN);
                MFVarSetFloat (_MDFlux_ClID,                    itemID, postFlux_Cl);

                MFVarSetFloat (_MDOutLocalLoadDOCID,           itemID, localLoad_DOC);
                MFVarSetFloat (_MDOutLocalLoadDINID,           itemID, localLoad_DIN);
                MFVarSetFloat (_MDOutLocalLoadClID,            itemID, localLoad_Cl);
                
                MFVarSetFloat (_MDOutConcMixing_DOCID,          itemID, postConcMixing_DOC);
                MFVarSetFloat (_MDOutConcMixing_DINID,          itemID, postConcMixing_DIN);
                MFVarSetFloat (_MDOutConcMixing_ClID,           itemID, postConcMixing_Cl);
                MFVarSetFloat (_MDOutPostConc_DOCID,            itemID, postConc_DOC);
                MFVarSetFloat (_MDOutPostConc_DINID,            itemID, postConc_DIN);
                MFVarSetFloat (_MDOutPostConc_ClID,             itemID, postConc_Cl);
                
                MFVarSetFloat (_MDStoreWaterMixing_DOCID,       itemID, postStoreWaterMixing_DOC);
                MFVarSetFloat (_MDStoreWaterMixing_DINID,       itemID, postStoreWaterMixing_DIN);
                MFVarSetFloat (_MDStoreWaterMixing_ClID,        itemID, postStoreWaterMixing_Cl);
                MFVarSetFloat (_MDStoreWater_DOCID,             itemID, postStoreWater_DOC);
                MFVarSetFloat (_MDStoreWater_DINID,             itemID, postStoreWater_DIN);
                MFVarSetFloat (_MDStoreWater_ClID,              itemID, postStoreWater_Cl);

   
                
}

int MDDOCDef () {

	MFDefEntering ("DOC Routing");
	
        if (
	    ((_MDFlux_DIN_denitID               = MDDINDef ()) == CMfailed) ||	
            ((_MDInRiverWidthID                 = MDRiverWidthDef ())     == CMfailed) ||
//          ((_MDInLitterFall_POCID             = MDLitterFallDef ()) == CMfailed) ||
//          ((_MDInLocalLoad_DOCID              = MFVarGetID (MDVarLocalLoadDOC,                             "kg/d",    MFInput,  MFFlux,  MFBoundary))   == CMfailed) ||
            ((_MDInDischarge0ID                 = MFVarGetID (MDVarDischarge0,                               "m3/s",    MFInput,  MFState, MFBoundary))   == CMfailed) ||	
	    ((_MDInDischargeID                  = MFVarGetID (MDVarDischarge,                                "m3/s",    MFInput,  MFState, MFBoundary))   == CMfailed) ||
            ((_MDInRiverStorageID               = MFVarGetID (MDVarRiverStorage,                           "m3/day",    MFInput,  MFState, MFInitial))    == CMfailed) ||	
	    ((_MDInRiverStorageChgID            = MFVarGetID (MDVarRiverStorageChg,                        "m3/day",    MFInput,  MFState, MFBoundary))   == CMfailed) ||    
            ((_MDInRunoffVolumeID               = MFVarGetID (MDVarRunoffVolume, 		             "m3/s",    MFInput,  MFState, MFBoundary))   == CMfailed) ||
            ((_MDInTotalEvaporationID           = MFVarGetID (MDVarTotalEvaporation,                           "m3",    MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
            ((_MDInWetlandsID                   = MFVarGetID (MDVarFracWetlandArea,                             "-",    MFInput,  MFState, MFBoundary))   == CMfailed) ||           // RJS 112513
            ((_MDInHumanLandUseID               = MFVarGetID (MDVarHumanLandUse,                                "-",    MFInput,  MFState, MFBoundary))   == CMfailed) ||
            ((_MDInRiverOrderID                 = MFVarGetID (MDVarRiverOrder,                                "-",    MFInput,  MFState, MFBoundary))   == CMfailed) ||
            ((_MDRunoffConc_DOCID       	= MFVarGetID (MDVarRunoffConcDOC,               "mg/L",     MFOutput,  MFState, MFInitial))    == CMfailed) ||	
            ((_MDInImpFractionID                = MFVarGetID (MDVarImpFracSpatial,                              "-",    MFInput,  MFState, MFBoundary))   == CMfailed) ||
            ((_MDInDOCmID                       = MFVarGetID (MDVarDOCm,                                        "-",    MFInput,  MFState, MFBoundary))   == CMfailed) ||
            ((_MDInDOCbID                       = MFVarGetID (MDVarDOCb,                                        "-",    MFInput,  MFState, MFBoundary))   == CMfailed) ||
            ((_MDInDINmID                       = MFVarGetID (MDVarDINm,                                        "-",    MFInput,  MFState, MFBoundary))   == CMfailed) ||
            ((_MDInDINbID                       = MFVarGetID (MDVarDINb,                                        "-",    MFInput,  MFState, MFBoundary))   == CMfailed) ||
            ((_MDInClmID                        = MFVarGetID (MDVarClm,                                         "-",    MFInput,  MFState, MFBoundary))   == CMfailed) ||
            ((_MDInClbID                        = MFVarGetID (MDVarClb,                                         "-",    MFInput,  MFState, MFBoundary))   == CMfailed) ||
            ((_MDOutLocalLoadDOCID              = MFVarGetID (MDVarLocalLoadDOC2,               "kg/d",   MFOutput,  MFFlux,  MFBoundary))   == CMfailed) ||
            ((_MDOutLocalLoadDINID              = MFVarGetID (MDVarLocalLoadDIN2,               "kg/d",   MFOutput,  MFFlux,  MFBoundary))   == CMfailed) ||
            ((_MDOutLocalLoadClID               = MFVarGetID (MDVarLocalLoadCl2,                "kg/d",   MFOutput,  MFFlux,  MFBoundary))   == CMfailed) ||
            ((_MDOutConcMixing_DOCID         	= MFVarGetID (MDVarConcMixingDOC,    	        "mg/L",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||	  
//          ((_MDOutConcMixing_POCID         	= MFVarGetID (MDVarConcMixingPOC,    	        "mg/L",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||	               
            ((_MDOutConcMixing_DINID         	= MFVarGetID (MDVarConcMixingDIN2,    	        "mg/L",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||	               
            ((_MDOutConcMixing_ClID         	= MFVarGetID (MDVarConcMixingCl,    	        "mg/L",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||	               
	    ((_MDStoreWaterMixing_DOCID      	= MFVarGetID (MDVarStoreWaterMixingDOC,       "kg/day",   MFOutput,  MFState, MFInitial))    == CMfailed) ||	
//	    ((_MDStoreWaterMixing_POCID      	= MFVarGetID (MDVarStoreWaterMixingPOC,       "kg/day",   MFOutput,  MFState, MFInitial))    == CMfailed) ||	
	    ((_MDStoreWaterMixing_DINID      	= MFVarGetID (MDVarStoreWaterMixingDIN2,      "kg/day",   MFOutput,  MFState, MFInitial))    == CMfailed) ||	
	    ((_MDStoreWaterMixing_ClID      	= MFVarGetID (MDVarStoreWaterMixingCl,        "kg/day",   MFOutput,  MFState, MFInitial))    == CMfailed) ||	
            ((_MDFluxMixing_DOCID         	= MFVarGetID (MDVarFluxMixingDOC,    	      "kg/day",   MFRoute,   MFFlux,  MFBoundary))   == CMfailed) ||	 
//          ((_MDFluxMixing_POCID         	= MFVarGetID (MDVarFluxMixingPOC,    	      "kg/day",   MFRoute,   MFFlux,  MFBoundary))   == CMfailed) ||	  
            ((_MDFluxMixing_DINID         	= MFVarGetID (MDVarFluxMixingDIN2,    	      "kg/day",   MFRoute,   MFFlux,  MFBoundary))   == CMfailed) ||	  
            ((_MDFluxMixing_ClID         	= MFVarGetID (MDVarFluxMixingCl,    	      "kg/day",   MFRoute,   MFFlux,  MFBoundary))   == CMfailed) ||	  

            ((_MDOutPostConc_DOCID         	= MFVarGetID (MDVarPostConcDOC,    	        "mg/L",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||	  
            ((_MDOutPostConc_DINID         	= MFVarGetID (MDVarPostConcDIN2,    	        "mg/L",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||	               
            ((_MDOutPostConc_ClID         	= MFVarGetID (MDVarPostConcCl,    	        "mg/L",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||	               
	    ((_MDStoreWater_DOCID               = MFVarGetID (MDVarStoreWaterDOC,             "kg/day",   MFOutput,  MFState, MFInitial))    == CMfailed) ||	
	    ((_MDStoreWater_DINID               = MFVarGetID (MDVarStoreWaterDIN2,            "kg/day",   MFOutput,  MFState, MFInitial))    == CMfailed) ||	
	    ((_MDStoreWater_ClID                = MFVarGetID (MDVarStoreWaterCl,              "kg/day",   MFOutput,  MFState, MFInitial))    == CMfailed) ||	
            ((_MDFlux_DOCID                     = MFVarGetID (MDVarFluxDOC,                   "kg/day",   MFRoute,   MFFlux,  MFBoundary))   == CMfailed) ||	 
            ((_MDFlux_DINID                     = MFVarGetID (MDVarFluxDIN2,    	      "kg/day",   MFRoute,   MFFlux,  MFBoundary))   == CMfailed) ||	  
            ((_MDFlux_ClID                      = MFVarGetID (MDVarFluxCl,                    "kg/day",   MFRoute,   MFFlux,  MFBoundary))   == CMfailed) ||	  
    

	(MFModelAddFunction (_MDDOC) == CMfailed)) return (CMfailed);
        
	MFDefLeaving ("DOC Routing");
	return (_MDFluxMixing_DOCID); 
}
