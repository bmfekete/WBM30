
/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDDIN.c  - transfer and processing of nutrients (transient storage and lakes)

rob.stewart@unh.edu

*******************************************************************************/

#include <cm.h>
#include <MF.h>
#include <MD.h>
#include <math.h>
#include <stdlib.h>

// test branch
// input

static int _MDInDischarge0ID            	= MFUnset;		// RJS010609
static int _MDInDischargeID			= MFUnset;
static int _MDInRiverStorageID		    	= MFUnset;
static int _MDInRiverStorageChgID		= MFUnset;		// RJS 010609
static int _MDInRiverDepthID			= MFUnset;
static int _MDInRiverWidthID			= MFUnset;
static int _MDInLocalLoad_DINID			= MFUnset;		// RJS 091108
static int _MDFlux_DIN_denitID			= MFUnset;		// RJS 091108		changed from MDInFlux_DINID 091408
static int _MDFlux_DIN_assimID			= MFUnset;		// RJS 091108		changed from MDInFlux_DINID 091408
static int _MDStoreWater_DIN_denitID	    	= MFUnset;		// RJS 091108
static int _MDStoreWater_DIN_assimID	    	= MFUnset;		// RJS 091108
static int _MDFluxMixing_DINID	 	    	= MFUnset;		// RJS 091108			changed from MDInFluxMixing_DINID 091408
static int _MDStoreWaterMixing_DINID		= MFUnset;		// RJS 091108
static int _MDInRunoffVolID			= MFUnset;		// RJS 091108
static int _MDInTnQ10ID				= MFUnset;              // RJS 102410
static int _MDInRiverOrderID			= MFUnset;		// RJS 112211
static int _MDWTemp_QxTID			= MFUnset;
static int _MDInPlaceHolderID			= MFUnset;		//RJS 042913
static int _MDInVfAdjustID			= MFUnset;		//RJS 050213
static int _MDInRemovalOrderID                  = MFUnset;              // RJS 100213
static int _MDInWWTPpointSrcID                  = MFUnset;              // RJS 100413
static int _MDInLakeYesNoID                     = MFUnset;              // RJS 100413
static int _MDInLakePointAreaID                 = MFUnset;              // RJS 100413

static int _MDInWetlandsID                      = MFUnset;              // RJS 112513
static int _MDInHCWAID                          = MFUnset;              // RJS 112513
static int _MDInSTSUptakeMaxID                  = MFUnset;              // RJS 112513
static int _MDInSTSKsID                         = MFUnset;              // RJS 112513
static int _MDInContributingAreaID              = MFUnset;              // RJS 120213
static int _MDInBankFullID                      = MFUnset;              // RJS 120213
static int _MDInFloodplainSwitchID              = MFUnset;              // RJS 120213
static int _MDInScaleAlphaID                    = MFUnset;              // RJS 120413
static int _MDInTotalEvaporationID              = MFUnset;              // RJS 022413
static int _MDInLandUseSubID                    = MFUnset;              // RJS 022714
static int _MDInLandUseAgrID                    = MFUnset;              // RJS 022714
static int _MDInWidthAdjustID                   = MFUnset;              // RJS 041114
//static int _MDInRandomNumberID                  = MFUnset;              // RJS 041114
static int _MDInTransferAdjustID		= MFUnset;		// RJS 041314

// output

static int _MDOutTotalMassRemovedTS_DIN_denitID = MFUnset;
static int _MDOutTotalMassRemovedTS_DIN_assimID = MFUnset;
static int _MDOutPostConc_DIN_denitID           = MFUnset;
static int _MDOutPostConc_DIN_assimID		= MFUnset;
static int _MDOutTotalMassRemovedDZ_DIN_denitID = MFUnset;
static int _MDOutTotalMassRemovedHZ_DIN_denitID = MFUnset;
static int _MDOutTotalMassRemovedMC_DIN_denitID = MFUnset;
static int _MDOutTotalMassRemovedLK_DIN_denitID = MFUnset;
static int _MDOutTotalMassRemovedDZ_DIN_assimID = MFUnset;
static int _MDOutTotalMassRemovedHZ_DIN_assimID = MFUnset;
static int _MDOutTotalMassRemovedMC_DIN_assimID = MFUnset;
static int _MDOutTotalMassRemovedLK_DIN_assimID = MFUnset;
static int _MDOutTimeOfStorageMCID	        = MFUnset;	// RJS 120608
static int _MDOutTimeOfStorageDZID	        = MFUnset;
static int _MDOutTimeOfStorageHZID	        = MFUnset;
static int _MDOutTransferDZID		        = MFUnset;
static int _MDOutTransferHZID		        = MFUnset;
static int _MDOutWaterDZID		        = MFUnset;
static int _MDOutWaterHZID		        = MFUnset;
static int _MDOutUptakeVf_denitID	        = MFUnset;
static int _MDOutUptakeVf_assimID	        = MFUnset;
static int _MDOutTotalMassPre_DIN_denitID       = MFUnset;	// RJS 091408
static int _MDOutTotalMassPre_DIN_assimID       = MFUnset;	// RJS 091408
static int _MDOutConcMixing_DINID	        = MFUnset;	// RJS 091408
static int _MDOutMassBalance_DIN_denitID        = MFUnset;	// RJS 110308
static int _MDOutMassBalance_DIN_assimID        = MFUnset;	// RJS 110308
static int _MDOutMassBalanceMixing_DINID        = MFUnset;	// RJS 112008
static int _MDOutRemovalDZ_denitID              = MFUnset;	// RJS 110708
static int _MDOutRemovalHZ_denitID              = MFUnset;	// RJS 110708
static int _MDOutRemovalMC_denitID              = MFUnset;	// RJS 110708
static int _MDOutRemovalLK_denitID              = MFUnset;  // RJS 100413
static int _MDOutRemovalTotal_denitID		= MFUnset;	// RJS 03-01-09
static int _MDOutRemovalDZ_assimID		= MFUnset;	// RJS 110708
static int _MDOutRemovalHZ_assimID		= MFUnset;	// RJS 110708
static int _MDOutRemovalMC_assimID		= MFUnset;	// RJS 110708
static int _MDOutRemovalLK_assimID              = MFUnset;  // RJS 100413
static int _MDOutRemovalTotal_assimID		= MFUnset;	// RJS 03-01-09
static int _MDOutAaID                           = MFUnset;	// RJS 110708
static int _MDOutAsDZID                         = MFUnset;	// RJS 110708
static int _MDOutAsHZID                         = MFUnset;	// RJS 110708
static int _MDDeltaStoreWater_DIN_denitID	= MFUnset;  // RJS 112008
static int _MDDeltaStoreWater_DIN_assimID	= MFUnset;  // RJS 112008
static int _MDDeltaStoreWaterMixing_DINID       = MFUnset;  // RJS 112008
static int _MDFlowPathRemoval_DIN_denitID       = MFUnset;	// RJS 112008
static int _MDFlowPathRemoval_DIN_assimID       = MFUnset;	// RJS 112008
static int _MDFlowPathRemovalMixing_DINID       = MFUnset;	// RJS 112008
static int _MDOutVelocityID                     = MFUnset;	// RJS 112108
static int _MDOutPreConc_DIN_denitID            = MFUnset;	// RJS 120408
static int _MDOutPreConc_DIN_assimID            = MFUnset;	// RJS 120408
static int _MDOutTotalMassRemoved_DIN_denitID	= MFUnset;	// RJS 032509
static int _MDOutTotalMassRemoved_DIN_assimID	= MFUnset;	// RJS 032509
static int _MDOutLocalLoad_Rip_DINID	        = MFUnset;

static int _MDOutDINVf_denitID		        = MFUnset;	// RJS 112210
static int _MDOutDINKt_denitID                  = MFUnset; // RJS 112210
static int _MDOutDINVf_assimID		        = MFUnset;	// RJS 112210
static int _MDOutDINKt_assimID                  = MFUnset; // RJS 112210
static int _MDOutPreFlux_DIN_denitID		= MFUnset; // RJS 050911
static int _MDOutPreFlux_DIN_assimID		= MFUnset; // RJS 050911
static int _MDOutBackCalcR_denitID              = MFUnset; // RJS 100413
static int _MDOutBackCalcVf_denitID             = MFUnset; // RJS 100413
static int _MDOutBackCalcR_assimID              = MFUnset; // RJS 100413
static int _MDOutBackCalcVf_assimID             = MFUnset; // RJS 100413
static int _MDOutRiparianRemovalID		= MFUnset; // RJS 040714
static int _MDOutPercentDINIncreaseID           = MFUnset; // RJS 080614

static float _MDUptakeKtMC   = 0.60;		// RJS 033009
static float _MDUptakeKtSTS  = 0.60;		// RJS 033009
static float _MDUptakeKtHTS  = 0.60;		// RJS 033009

static float _MDAlphaSTS     = 0.0;		// RJS 032509
static float _MDAlphaHTS     = 0.0;		// RJS 032509
static float _MDASTSA	     = 0.0;		// RJS 032509
static float _MDAHTSA	     = 0.0;		// RJS 032509


static void _MDNProcessing (int itemID) {

float localLoad_DIN          	= 0.0;		// mass of N from local non-point sources (kg/day) RJS 091108
float preFlux_DIN_denit		= 0.0;		// mass of N coming from upstream (kg/day) RJS 091108
float preFlux_DIN_assim		= 0.0;		// mass of N coming from upstream (kg/day) RJS 091108
float preFluxMixing_DIN		= 0.0;		// mass of N coming from upstream MIXING ONLY (kg/day) RJS 091108
float storeWaterMixing_DIN	= 0.0;		// store water DIN MIXING ONLY (kg/day)
float preConcMixingDIN		= 0.0;		// concentration MIXING ONLY (mg/L)
float runoffVol			= 0.0;		// m3/sec
float waterTotalVolume		= 0.0;   	// water total volume (m3)
float postFluxDIN_denit		= 0.0;		// post flux of DIN (kg/day)
float postFluxDIN_assim		= 0.0;		// post flux of DIN (kg/day)
float postFluxDINMixing		= 0.0;		// post flux of DIN mixing (kg/day)
float storeWater_DIN_denit	= 0.0;	  	// store water DIN (kg) ?
float storeWater_DIN_assim	= 0.0;	  	// store water DIN (kg) ?
float postStoreWater_DIN_denit	= 0.0;  	// store water DIN new time step (kg)		// RJS 110308
float postStoreWater_DIN_assim	= 0.0;  	// store water DIN new time step (kg)		// RJS 110308
float postStoreWaterMixing_DIN	= 0.0;		// store water DIN new time step (kg)
float DINDeltaStorage_denit	= 0.0;	  	// change in water DIN (kg)				// RJS 110308
float DINDeltaStorage_assim	= 0.0;	  	// change in water DIN (kg)				// RJS 110308
float DINDeltaStorageMixing	= 0.0; 		// change in mixed water DIN (kg)		// RJS 110508
float DINTotalIn_denit		= 0.0;		// mass DIN in (kg)				// RJS 110308
float DINTotalIn_assim		= 0.0;		// mass DIN in (kg)				// RJS 110308
float DINTotalInMixing		= 0.0;		// mass DIN in Mixing (kg)		// RJS 112008
float waterStorage		= 0.0;		// water storage (m3/day)
float waterStorageChg		= 0.0;		// water storage change (m3/day) 					// RJS 01-06-09
float waterStoragePrev		= 0.0;		// water storage previous timestep (m3/day)			// RJS 01-06-09
float totalMassRemovedTS_DIN_denit = 0.0;	// mass DIN removed in Transient Storage (kg/day)
float totalMassRemovedTS_DIN_assim = 0.0;	// mass DIN removed in Transient Storage (kg/day)
float preConcDIN_denit		= 0.0;		// pre concentration of DIN kg/m3/day
float preConcDIN_assim		= 0.0;		// pre concentration of DIN kg/m3/day
float postConcDIN_denit		= 0.0;		// post concentration of DIN kg/m3/day
float postConcDIN_assim		= 0.0;		// post concentration of DIN kg/m3/day
float postConcDINMixing		= 0.0;		// post concentration of DIN mixing (kg/m3/day)
float dischargePre		= 0.0;		// pre routing discharge that includes local cell runoff (m3/s)	// 010609
float discharge			= 0.0;		// instantaneous discharge (m3/s)
float dL			= 0.0;		// cell length (m)
float transferDZ		= 0.0;		// probability of water transfer from MC to DZ
float transferHZ		= 0.0;		// probability of water transfer from MC to HZ
float uptakeDZ_denit		= 0.0;		// probability of N uptake in DZ
float uptakeDZ_assim		= 0.0;		// probability of N uptake in DZ
float uptakeHZ_denit		= 0.0;		// probability of N uptake in HZ
float uptakeHZ_assim		= 0.0;		// probability of N uptake in HZ
float removalTotal_denit	= 0.0;		// Total removal (HZ + DZ) in grid cell reach
float removalTotal_assim	= 0.0;		// Total removal (HZ + DZ) in grid cell reach
float alphaDZ			= 0.0;		// exchange rate of water between MC and DZ
float alphaHZ			= 0.0;		// exchange rate of water between MC and HZ
float depth			= 0.0;		// river depth (m)
float width			= 0.0;		// river width (m)
float aA			= 0.0;		// main channel area (m2)
float asDZ			= 0.0;		// DZ storage area (m2)
float asHZ			= 0.0;		// HZ storage area (m2)
float tStorDZ			= 0.0;		// time of storage or residence time in DZ (days)
float tStorHZ			= 0.0;		// time of storage or residence time in HZ (days)
float tStorMC			= 0.0;		// time of storage or residence time in MC (days)
float vf_denit			= 0.0;		// areal uptake rate (m / sec)
float vf_assim			= 0.0;		// areal uptake rate (m / sec)
float hydLoad			= 0.0;		// hydraulic load or discharge / benthic surface area (m / day)
float uptakeMC_denit		= 0.0;		// probability of N uptake in MC
float uptakeMC_assim		= 0.0;		// probability of N uptake in MC
float removalDZ_denit		= 0.0;		// probability that a molecule in channel removed in DZ
float removalDZ_assim		= 0.0;		// probability that a molecule in channel removed in DZ
float removalHZ_denit		= 0.0;		// probability that a molecule in channel removed in HZ
float removalHZ_assim		= 0.0;		// probability that a molecule in channel removed in HZ
float removalMC_denit		= 0.0;		// probability that a molecule in channel removed in MC
float removalMC_assim		= 0.0;		// probability that a molecule in channel removed in MC
float removalLK_denit           = 0.0;          // probability that a molecule in lake is removed by lake
float removalLK_assim           = 0.0;          // probability that a molecule in lake is removed by lake
float riverOrder		= 0.0;		// river order
float removalOrder              = 0.0;          // river order where removal begins (i.e. 'real river', not terrestrial grid cell)
float R_denit			= 0.0;		// proportional removal
float R_assim			= 0.0;		// proportional removal
float totalMassRemovedDZ_DIN_denit = 0.0;		// total mass removed in DZ (kg/day)
float totalMassRemovedHZ_DIN_denit = 0.0;		// total mass removed in HZ (kg/day)
float totalMassRemovedMC_DIN_denit = 0.0;		// total mass removed in MC (kg/day)
float totalMassRemovedLK_DIN_denit = 0.0;          // total mass removed in LK (kg/day)
float totalMassRemoved_DIN_denit   = 0.0;		// total mass removed (LK, DZ, MC, HZ)	032509
float totalMassRemovedDZ_DIN_assim = 0.0;		// total mass removed in DZ (kg/day)
float totalMassRemovedHZ_DIN_assim = 0.0;		// total mass removed in HZ (kg/day)
float totalMassRemovedMC_DIN_assim = 0.0;		// total mass removed in MC (kg/day)
float totalMassRemovedLK_DIN_assim = 0.0;          // total mass removed in LK (kg/day)
float totalMassRemoved_DIN_assim   = 0.0;		// total mass removed (LK, DZ, MC, HZ)	032509
float waterDZ			   = 0.0;  	// water entering DZ
float waterHZ			   = 0.0;		// water entering HZ
float massBalance_DIN_denit;  			// RJS 110308
float massBalance_DIN_assim;  			// RJS 110308
float massBalanceMixing_DIN;			// RJS 112008
float flowPathRemoval_denit 	= 0.0;		// kg/day RJS 110508  ---  if order<3, discharge = 0, preflux is removed via flowPathRemoval
float flowPathRemoval_assim	= 0.0;		// kg/day RJS 110508  ---  if order<3, discharge = 0, preflux is removed via flowPathRemoval
float flowPathRemovalMixing     = 0.0;		// kg/day RJS 112008
float velocity			= 0.0;
float runoffConc		= 0.0;		// RJS 120208
float ktMC			= 0.0;		// MC volumetric uptake rate (1 / day)
float ktSTS			= 0.0;		// STS volumetric uptake rate (1 / day)
float ktHTS			= 0.0;		// HTS volumetric uptake rate (1 / day)
float tnQ10   			= 2.0;		//RJS 102410
float denit_int_vf		= -2.975;	//RJS 051011	calculated from avg vf = 0.137 m/day (LINX2), and average [] of 529 ug/L in LINX experiments, log(vf) = -m * log(C in ug/L) + e.
float denit_slope 		= -0.493;	//RJS 110110
float assim_int_vf		= -2.206;	//RJS 051011	calculated from avg vf = 0.137 m/day (LINX2), and average [] of 529 ug/L in LINX experiments, log(vf) = -m * log(C in ug/L) + e.
float assim_slope 		= -0.462;	//RJS 110110
float denit_int_kt		= 0.2319;	//MMM March 2013 New value calculated RJS 110110	calculated from kt = 0.64, and average [] of 529 ug/L in LINX experiments, log(kt) = -m * log(C in ug/L) + e
float assim_int_kt              = 0.2319;       // copied from line above (**** place holder ****)
float DIN_Vf_denit		= 0.0;		//RJS 110110
float DIN_Vf_ref_denit          = 0.0;		//RJS 110110
float DIN_Kt_denit		= 0.0;		//RJS 110110
float DIN_Kt_ref_denit		= 0.0;		//RJS 110110
float DIN_Vf_assim		= 0.0;		//RJS 110110
float DIN_Vf_ref_assim          = 0.0;		//RJS 110110
float DIN_Kt_assim              = 0.0;          //RJS 110413
float DIN_Kt_ref_assim          = 0.0;          //RJS 110413

float waterT			= 0.0;		//RJS 051211
float tnTref			= 20.0;
float placeHolder		= 0.0;		//RJS 042913
float VfAdjust			= 1.0;		//RJS 050213
float WWTPpointSrc              = 0.0;          //RJS 100413

float cell_1      		= 1293;         //255,138
float cell_2			= 999999;
float lakeYesNo                 = 0.0;
float lakeArea                  = 0.0;
float lake_HL                   = 0.0;
float wetlands                  = 0.0;
float HCWA                      = 0.0;
float wetlandArea               = 0.0;
float wetland_HL                = 0.0;
float STS_U                     = 0.0;
float STS_Umax                  = 0.0;
float STS_UmaxT                 = 0.0;
float STS_Ks                    = 0.0;
float contributingArea          = 0.0;
float bankfull_mm               = 0.0;
float floodplain_switch         = 0.0;
float scaleAlpha                = 0.0;
float totalEvap                 = 0.0;
float luSub                     = 0.0;
float luAgr                     = 0.0;
float RiparianRemCoef           = 0.0;
float localLoad_Rip_DIN         = 0.0;
float RiparianRemoval		= 0.0;
float widthAdjust               = 0.0;
float r1                        = 0.0;
float r2                        = 0.0;
float VfAdjust2                 = 0.0;
float randomNumber              = 0.0;
float transferAdjust		= 0.0;
float TransferCoef		= 0.0;
float DINTotalIn_denit2         = 0.0;
float preConcDIN_denit2         = 0.0;
float percentIncrease           = 0.0;


	riverOrder           = MFVarGetFloat (_MDInRiverOrderID,         itemID, 0.0);	 //RJS 112211
        removalOrder         = MFVarGetFloat (_MDInRemovalOrderID,       itemID, 0.0);   // RJS 100213
	localLoad_DIN  	     = MFVarGetFloat (_MDInLocalLoad_DINID,      itemID, 0.0);	 // kg/day RJS 091108
        WWTPpointSrc         = MFVarGetFloat (_MDInWWTPpointSrcID,       itemID, 0.0);   // kg/day RJS 100413
	preFlux_DIN_denit    = MFVarGetFloat (_MDFlux_DIN_denitID,       itemID, 0.0);	 // kg/day RJS 091108
	storeWater_DIN_denit = MFVarGetFloat (_MDStoreWater_DIN_denitID, itemID, 0.0);	 // kg/day RJS 091108
        preFlux_DIN_assim    = MFVarGetFloat (_MDFlux_DIN_assimID,       itemID, 0.0);	 // kg/day RJS 091108
	storeWater_DIN_assim = MFVarGetFloat (_MDStoreWater_DIN_assimID, itemID, 0.0);	 // kg/day RJS 091108
	preFluxMixing_DIN    = MFVarGetFloat (_MDFluxMixing_DINID,       itemID, 0.0);	 // kg/day RJS 091108 changed from MDInFluxMixing 091408
	storeWaterMixing_DIN = MFVarGetFloat (_MDStoreWaterMixing_DINID, itemID, 0.0);	 // kg/day RJS 091108
	runoffVol            = MFVarGetFloat (_MDInRunoffVolID,          itemID, 0.0); 	 // m3/sec
	waterStorage	     = MFVarGetFloat (_MDInRiverStorageID,       itemID, 0.0);	 // m3/sec!!! RJS 100213
	waterStorageChg	     = MFVarGetFloat (_MDInRiverStorageChgID,    itemID, 0.0);	 // m3/sec!!! RJS 100213
	discharge            = MFVarGetFloat (_MDInDischargeID,          itemID, 0.0);	 // m3/sec, discharge leaving the grid cell, after routing!
	dischargePre	     = MFVarGetFloat (_MDInDischarge0ID,         itemID, 0.0);	 // m3/sec, discharge from upstream PLUS local runoff, before routing!
	tnQ10                = MFVarGetFloat (_MDInTnQ10ID,              itemID, 0.0);	 // RJS 102410
	waterT		     = MFVarGetFloat (_MDWTemp_QxTID,		 itemID, 0.0);
   	lakeYesNo            = MFVarGetFloat (_MDInLakeYesNoID,          itemID, 0.0); 	 // 1 = lake, 0 = no lake
	lakeArea             = MFVarGetFloat (_MDInLakePointAreaID,      itemID, 0.0); 	 // lake area at outlet, km2
    	VfAdjust	     = MFVarGetFloat (_MDInVfAdjustID,           itemID, 1.0);	 // RJS 112211, adjusts loads, keep at 1 if nodata
    	depth		     = MFVarGetFloat (_MDInRiverDepthID, 	 itemID, 0.0);	 // m			// moved here 031209
	width	             = MFVarGetFloat (_MDInRiverWidthID,    	 itemID, 0.0);	 // m			// moved here 031209
        wetlands             = MFVarGetFloat (_MDInWetlandsID,           itemID, 0.0);   // proportion wetlands
        HCWA                 = MFVarGetFloat (_MDInHCWAID,               itemID, 0.0);   // hydrologically connected wetland area
        STS_Umax             = MFVarGetFloat (_MDInSTSUptakeMaxID,       itemID, 0.0);   // mg/m2/d
        STS_Ks               = MFVarGetFloat (_MDInSTSKsID,              itemID, 0.0);   // mg N/l
        contributingArea     = MFVarGetFloat (_MDInContributingAreaID,   itemID, 0.0);   // km2
        bankfull_mm          = MFVarGetFloat (_MDInBankFullID,           itemID, 0.0);   // mm/d (1.2 yr = 5.0 mm/d or 18.7 m3/s, 1.5 yr = 6.5 mm/d or 24.4 m3/s)
        floodplain_switch    = MFVarGetFloat (_MDInFloodplainSwitchID,   itemID, 0.0);   // 1 = on, 0 = off
        scaleAlpha           = MFVarGetFloat (_MDInScaleAlphaID,         itemID, 0.0);   // 0 = off, value indicates: alpha = X * Discharge
        totalEvap            = MFVarGetFloat (_MDInTotalEvaporationID,   itemID, 0.0);   // m3/day  RJS 022114
	luSub                = MFVarGetFloat (_MDInLandUseSubID,         itemID, 0.0) / 100;	//KAW 2013 05 08 Suburban Land Use
	luAgr                = MFVarGetFloat (_MDInLandUseAgrID,         itemID, 0.0) / 100;	//KAW 2013 05 08 Agricultural Land Use
        widthAdjust          = MFVarGetFloat (_MDInWidthAdjustID,        itemID, 0.0);  // width adjust
        transferAdjust	     = MFVarGetFloat (_MDInTransferAdjustID,     itemID, 0.0);  // RipTrans adjust
	
 // Final Riparian calcs:       
 //     RiparianRemCoef      = (-0.6737 * pow(luSub + luAgr, 2)) - (0.272 * (luSub + luAgr)) + 0.9867;	// original, REMOVAL, not transfer
 //     TransferCoef	   = (1.0 - RiparianRemCoef) * transferAdjust > 1.0 ? 1.0 : (1.0 - RiparianRemCoef) * transferAdjust;
 //     localLoad_Rip_DIN    = TransferCoef * localLoad_DIN;
 //     RiparianRemoval      = (1.0 - TransferCoef) * localLoad_DIN;
 //     localLoad_DIN        = localLoad_Rip_DIN;
      
   // Old Riparian calc attempts:
   //    RiparianRemCoef      = -0.8 * (luSub + luAgr) + 0.95;	// original, REMOVAL, not transfer
   //    TransferCoef	      = 0.045 * pow(2.71828183,3.2 * (luSub + luAgr)) > 1.0 ? 1.0 : 0.045 * pow(2.71828183,3.2 * (luSub + luAgr));
   //    TransferCoef	      = (0.9 * pow(luSub + luAgr, 1.3)) > 1.0 ? 1.0 : (0.9 * pow(luSub + luAgr, 1.3));  // BEST ONE!
   //	 RiparianRemCoef      = 0.4994 * pow(luSub + luAgr,1.5909);			// power law (r2=0.42), this is a transfer coefficient
   // 	 RiparianRemCoef      = 0.0113 * pow(2.7182818,(4.8511 * (luSub + luAgr)));	// exponential law (r2=0.39), this is a transfer coefficient
     
        
        dL                   = MFModelGetLength (itemID);               // km converted to m
	waterStoragePrev     = waterStorage - waterStorageChg;		// m3/sec!!! RJS 100213
	aA		     = width * depth;                           // m2	
        
        width                = width * widthAdjust;                     // m    RJS 041114
        depth                = aA / width;                              // m    RJS 041114
        
	ktMC		     = _MDUptakeKtMC;			 	// RJS 033009
	ktSTS		     = _MDUptakeKtSTS;				// RJS 033009
	ktHTS		     = _MDUptakeKtHTS;				// RJS 033009
        alphaDZ              = _MDAlphaSTS;                          
	alphaHZ              = _MDAlphaHTS; 
        
        if (scaleAlpha > 0.0) {
            alphaDZ          = scaleAlpha * discharge > alphaDZ ? alphaDZ : scaleAlpha * discharge;
        }
        
	DINTotalIn_denit     = localLoad_DIN + preFlux_DIN_denit + storeWater_DIN_denit + WWTPpointSrc;  // kg/day  RJS 100413
        DINTotalIn_assim     = localLoad_DIN + preFlux_DIN_assim + storeWater_DIN_assim + WWTPpointSrc;  // kg/day  RJS 110413
	DINTotalInMixing     = localLoad_DIN + preFluxMixing_DIN + storeWaterMixing_DIN + WWTPpointSrc;  // kg/day  RJS 100413

	runoffConc	     = runoffVol > 0.0 ? (localLoad_DIN * 1000000) / (runoffVol * 86400 * 1000) : 0.0;												
        waterTotalVolume     = (waterStoragePrev + dischargePre) * 86400;     // RJS 100213 waterStorage is in m3/s
        
        DINTotalIn_denit2    = localLoad_DIN + preFlux_DIN_denit + storeWater_DIN_denit;        // Total in WITHOUT WWTP
        
        
//        r1                   = rand();
//        r2                   = r1 / (RAND_MAX);
//        VfAdjust2            = randomNumber < 0.5 ? 1.0 / VfAdjust : VfAdjust;
                

// processing code


	if (discharge > 0.000001) {			//

		preConcDIN_denit     = DINTotalIn_denit / (waterTotalVolume) * 1000;                                    // mg/L
                preConcDIN_assim     = DINTotalIn_assim / (waterTotalVolume) * 1000;                                    // mg/L
		preConcMixingDIN     = DINTotalInMixing / (waterTotalVolume) * 1000;                                    // mg/L
		preConcDIN_denit2    = DINTotalIn_denit2 / (waterTotalVolume) * 1000;                                   // mg/L conc without WWTP
                
                percentIncrease      = (preConcDIN_denit - preConcDIN_denit2) / preConcDIN_denit2 * 100;                // percent increase in Conc due to WWTP
                
                velocity	     = discharge / (depth * width);                                                     // m/s
                DIN_Vf_denit         = 0.0;                                                                             // m/d
                DIN_Kt_denit         = 0.0;                                                                             // 1/d
                DIN_Vf_assim         = 0.0;                                                                             // m/d
                wetlandArea          = ((MFModelGetArea (itemID) * wetlands) - (dL * width)) * HCWA;                    // m2 
                wetlandArea          = wetlandArea < 0.0 ? MFModelGetArea (itemID) * wetlands * HCWA : wetlandArea;     // m2
                
              
                if (preConcDIN_denit > 0.0) {
                   
			DIN_Vf_ref_denit = preConcDIN_denit > 0.0 ? pow(10,(denit_int_vf + (log10(preConcDIN_denit * 1000) * denit_slope)))*86400/100 : 0.0;	// no conversion to m/d neccesary - already in m/d
			DIN_Vf_ref_denit = DIN_Vf_ref_denit * VfAdjust;                                                           // m/d RJS 050213
			DIN_Vf_denit	 = DIN_Vf_ref_denit * pow(tnQ10, (((waterT) - tnTref) / 10));                             // m/d	RJS 051211

                       	DIN_Vf_ref_assim = preConcDIN_assim > 0.0 ? pow(10,(assim_int_vf + (log10(preConcDIN_assim * 1000) * assim_slope)))*86400/100 : 0.0;	// no conversion to m/d neccesary - already in m/d
			DIN_Vf_ref_assim = DIN_Vf_ref_assim * VfAdjust;                                                           // m/d RJS 050213
			DIN_Vf_assim	 = DIN_Vf_ref_assim * pow(tnQ10, (((waterT) - tnTref) / 10));                             // m/d	RJS 051211

                        STS_UmaxT = STS_Umax * pow(tnQ10, ((waterT - tnTref) / 10));                                              // 112513
                        STS_U     = STS_UmaxT * preConcDIN_denit / (STS_Ks + preConcDIN_denit);                                   // 112513
                
                        DIN_Kt_denit     = ktHTS * pow(tnQ10, ((waterT - tnTref) / 10));                                          // 112513
                        DIN_Kt_assim     = ktHTS * pow(tnQ10, ((waterT - tnTref) / 10));                                          // 112513
                }


                
                if (lakeArea > 0.0) {
                    lake_HL   = discharge / (lakeArea * 1000 * 1000) * 86400;                 // m/day
                    removalLK_denit = 1.0 - pow(2.718281828, -1.0 * DIN_Vf_denit / lake_HL);  // proportional removal
                    removalLK_assim = 1.0 - pow(2.718281828, -1.0 * DIN_Vf_assim / lake_HL);  // proportional removal
                }

		if ((aA > 0.0) && (lakeYesNo < 1.0)) {			// if aA < 0.0, then no TS should be executed.  RJS 03-14-09		KYLE

				asHZ       = aA * _MDAHTSA;			// single value MEAN Scenario 061609	- ln transformed mean           
				tStorHZ    = alphaHZ == 0.0 ? 0.0 : (asHZ / (alphaHZ * aA)) / 86400; // days
				transferHZ = alphaHZ * aA * dL / discharge;		//RJS 072909
				waterHZ    = discharge * transferHZ; 		// m3/s

				hydLoad    = discharge / (width * dL) * 86400;	// m/day,
				tStorMC    = dL / velocity / 86400;             // days,

                        if ((floodplain_switch < 1) || (bankfull_mm < (discharge / contributingArea / 1000 * 86400))) {   // enters this for floodplain_switch == 0, or when bankfull is activated
                                
                                asDZ       = aA * _MDASTSA;			// single value MEAN Scenario 061609    - ln transformed mean
				tStorDZ    = alphaDZ == 0.0 ? 0.0 : (asDZ / (alphaDZ * aA)) / 86400; // days											// RJS 111710
                                transferDZ = alphaDZ * aA * dL / discharge;		//RJS 072909
                                waterDZ    = discharge * transferDZ; 		// m3/s
                                wetland_HL = wetlandArea > 0.0 ? (transferDZ * discharge) / wetlandArea * 86400 : 0.0;   // m/d
                                
                        }
                                
                                /* denitrification */
                                
                		uptakeMC_denit  = hydLoad > 0.0 ? 1.0 - pow(2.718281828, -1 * DIN_Vf_denit / hydLoad) : 0.0;                              // RJS 102410
                                uptakeDZ_denit  = (wetland_HL > 0.0) && (preConcDIN_denit > 0.0) ? 1.0 - pow(2.718281828, -1 * (STS_U / (preConcDIN_denit * 1000)) / wetland_HL) : 0.0;    // RJS 112413
				uptakeHZ_denit  = 1.0 - pow(2.718281828, -1 * DIN_Kt_denit * tStorHZ);                              // RJS 102410

                                removalMC_denit = uptakeMC_denit;		// reach scale proportional nutrient removal  // **** RJS 032309				
				removalDZ_denit = transferDZ * uptakeDZ_denit;  // reach scale proportional nutrient removal
				removalHZ_denit = transferHZ * uptakeHZ_denit;  // reach scale proportional nutrient removal

                                /* assimilation */
                                
                                uptakeMC_assim  = hydLoad > 0.0 ? 1.0 - pow(2.718281828, -1 * DIN_Vf_assim / hydLoad) : 0.0;                              // RJS 102410
				uptakeDZ_assim  = (wetland_HL > 0.0) && (preConcDIN_denit > 0.0) ? 1.0 - pow(2.718281828, -1 * (STS_U / (preConcDIN_assim * 1000)) / wetland_HL) : 0.0;    // RJS 112413
				uptakeHZ_assim  = 1.0 - pow(2.718281828, -1 * DIN_Kt_assim * tStorHZ);                              // RJS 102410

                                removalMC_assim = uptakeMC_assim;		// reach scale proportional nutrient removal  // **** RJS 032309				
				removalDZ_assim = transferDZ * uptakeDZ_assim;  // reach scale proportional nutrient removal
				removalHZ_assim = transferHZ * uptakeHZ_assim;  // reach scale proportional nutrient removal
                                
		}
                
                removalTotal_denit           = removalDZ_denit + removalHZ_denit + removalMC_denit + removalLK_denit;
		totalMassRemovedDZ_DIN_denit = (removalDZ_denit) * DINTotalIn_denit;                          // kg/day RJS 110308
		totalMassRemovedHZ_DIN_denit = (removalHZ_denit) * DINTotalIn_denit;                          // kg/day RJS 110308
		totalMassRemovedMC_DIN_denit = (removalMC_denit) * DINTotalIn_denit;                          // kg/day RJS 110308
                totalMassRemovedLK_DIN_denit = (removalLK_denit) * DINTotalIn_denit;                          // kg/day RJS 100413
                totalMassRemovedTS_DIN_denit = totalMassRemovedDZ_DIN_denit + totalMassRemovedHZ_DIN_denit;   // kg/day              
                totalMassRemoved_DIN_denit   = totalMassRemovedTS_DIN_denit + totalMassRemovedMC_DIN_denit + totalMassRemovedLK_DIN_denit;
                
                removalTotal_assim           = removalDZ_assim + removalHZ_assim + removalMC_assim + removalLK_assim;
		totalMassRemovedDZ_DIN_assim = (removalDZ_assim) * DINTotalIn_assim;                          // kg/day RJS 110308
		totalMassRemovedHZ_DIN_assim = (removalHZ_assim) * DINTotalIn_assim;                          // kg/day RJS 110308
		totalMassRemovedMC_DIN_assim = (removalMC_assim) * DINTotalIn_assim;                          // kg/day RJS 110308
                totalMassRemovedLK_DIN_assim = (removalLK_assim) * DINTotalIn_assim;                          // kg/day RJS 100413
                totalMassRemovedTS_DIN_assim = totalMassRemovedDZ_DIN_assim + totalMassRemovedHZ_DIN_assim;   // kg/day              
                totalMassRemoved_DIN_assim   = totalMassRemovedTS_DIN_assim + totalMassRemovedMC_DIN_assim + totalMassRemovedLK_DIN_assim;
                
                
             	postConcDIN_denit      = (DINTotalIn_denit - totalMassRemoved_DIN_denit - flowPathRemoval_denit) / (waterTotalVolume - totalEvap) * 1000;  // mg/L
                postConcDIN_assim      = (DINTotalIn_assim - totalMassRemoved_DIN_assim - flowPathRemoval_assim) / (waterTotalVolume - totalEvap) * 1000;  // mg/L
                postConcDINMixing      = (DINTotalInMixing - flowPathRemovalMixing) / (waterTotalVolume - totalEvap) * 1000;				    // mg/L
   
                R_denit  = preConcDIN_denit > 0.0 ? 1.0 - (postConcDIN_denit / preConcDIN_denit) : 0.0;                         // back-calculated removal
                vf_denit = lake_HL > 0.0 ? -1.0 * lake_HL * log(1.0 - R_denit) : -1.0 * hydLoad * log(1.0 - R_denit);	// back-calculated vf
                vf_denit = R_denit == 1.0 ? -9999 : vf_denit;                                                             // back-calculated vf
                
                R_assim  = preConcDIN_assim > 0.0 ? 1.0 - (postConcDIN_assim / preConcDIN_assim) : 0.0;                         // back-calculated removal
                vf_assim = lake_HL > 0.0 ? -1.0 * lake_HL * log(1.0 - R_assim) : -1.0 * hydLoad * log(1.0 - R_assim);	// back-calculated vf
                vf_assim = R_assim == 1.0 ? -9999 : vf_assim;    
                
		}
        
	else {

		flowPathRemoval_denit = DINTotalIn_denit;       // kg/day
                flowPathRemoval_assim = DINTotalIn_assim;       // kg/day
		flowPathRemovalMixing = DINTotalInMixing;       // kg/day
                postConcDIN_denit     = 0.0;                    // mg/L
                postConcDIN_assim     = 0.0;                    // mg/L
                postConcDINMixing     = 0.0;                    // mg/L
                R_denit               = 0.0;                    // back-calculated removal
                R_assim               = 0.0;                    // back-calculated removal
                vf_denit              = 0.0;                    // back-calculated Vf
                vf_assim              = 0.0;                    // back-calculated Vf

	}



        postFluxDIN_denit        = (discharge * MDConst_m3PerSecTOm3PerDay) * postConcDIN_denit / 1000;  	// kg/day
        postFluxDIN_assim        = (discharge * MDConst_m3PerSecTOm3PerDay) * postConcDIN_assim / 1000;  	// kg/day
        postFluxDINMixing 	 = (discharge * MDConst_m3PerSecTOm3PerDay) * postConcDINMixing / 1000;		// kg/day
        postStoreWater_DIN_denit = (waterStorage * MDConst_m3PerSecTOm3PerDay) * postConcDIN_denit / 1000;	// kg/day
        postStoreWater_DIN_assim = (waterStorage * MDConst_m3PerSecTOm3PerDay) * postConcDIN_assim / 1000;	// kg/day
        postStoreWaterMixing_DIN = (waterStorage * MDConst_m3PerSecTOm3PerDay) * postConcDINMixing / 1000;	// kg/day
        DINDeltaStorage_denit  	 = postStoreWater_DIN_denit - storeWater_DIN_denit;     			// kg/day
        DINDeltaStorage_assim  	 = postStoreWater_DIN_assim - storeWater_DIN_denit;     			// kg/day
        DINDeltaStorageMixing 	 = postStoreWaterMixing_DIN - storeWaterMixing_DIN;				// kg/day


        massBalance_DIN_denit 	 = ((localLoad_DIN + WWTPpointSrc + preFlux_DIN_denit + storeWater_DIN_denit) - (totalMassRemoved_DIN_denit + postFluxDIN_denit + postStoreWater_DIN_denit + flowPathRemoval_denit)) / (localLoad_DIN + WWTPpointSrc + storeWater_DIN_denit + preFlux_DIN_denit);		// RJS 111411
        massBalance_DIN_assim 	 = ((localLoad_DIN + WWTPpointSrc + preFlux_DIN_assim + storeWater_DIN_assim) - (totalMassRemoved_DIN_assim + postFluxDIN_assim + postStoreWater_DIN_assim + flowPathRemoval_assim)) / (localLoad_DIN + WWTPpointSrc + storeWater_DIN_assim + preFlux_DIN_assim);		// RJS 111411
        massBalanceMixing_DIN	 = ((localLoad_DIN + WWTPpointSrc + preFluxMixing_DIN + storeWaterMixing_DIN) - (postFluxDINMixing + postStoreWaterMixing_DIN + flowPathRemovalMixing)) / (localLoad_DIN + WWTPpointSrc + storeWaterMixing_DIN + preFluxMixing_DIN);

if (((massBalance_DIN_denit > 0.0003) || (massBalance_DIN_assim > 0.0003)) && (localLoad_DIN + storeWater_DIN_denit + preFlux_DIN_denit > 0.000001)) printf("******\n itemID = %d, y = %d, m = %d, d = %d, massBalance_DIN_denit = %f, MB_assim = %f, MB_mixing = %f\n, postConcDIN_denit = %f, postConcDIN_assim = %f, waterStorage = %f, localLoad_DIN = %f, preFlux_DIN_denit = %f, storeWater_DIN_denit = %f, storeWater_DIN_assim = %f\n totalMassRemovedTS_DIN_denit = %f, totalMassRemovedMC_DIN_denit = %f, postFluxDIN_denit = %f, postStoreWater_DIN_denit = %f, flowPathRemoval_denit = %f\n, totalMassRemovedTS_DIN_assim = %f, totalMassRemovedMC_DIN_assim = %f, postFluxDIN_assim = %f, postStoreWater_DIN_assim = %f, flowPathRemoval_assim = %f\n", itemID, MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), massBalance_DIN_denit, massBalance_DIN_assim, massBalanceMixing_DIN, postConcDIN_denit, postConcDIN_assim, waterStorage, localLoad_DIN, preFlux_DIN_denit, storeWater_DIN_denit, storeWater_DIN_assim, totalMassRemovedTS_DIN_denit, totalMassRemovedMC_DIN_denit, postFluxDIN_denit, postStoreWater_DIN_denit, flowPathRemoval_denit, totalMassRemovedTS_DIN_assim, totalMassRemovedMC_DIN_assim, postFluxDIN_assim, postStoreWater_DIN_assim, flowPathRemoval_assim);

//if (itemID==809) printf ("***** itemID=%d, month=%d, day=%d, year=%d, Q=%f, hydLoad=%f, waterT=%f, localLoad_DIN = %f \n",itemID,MFDateGetCurrentMonth(),MFDateGetCurrentDay(),MFDateGetCurrentYear(),discharge,hydLoad,waterT,localLoad_DIN);
//if (itemID==809) printf ("preConcDIN=%f, postConcDIN=%f, preConcMixingDIN=%f, postConcMixingDIN=%f \n",preConcDIN,postConcDIN,preConcMixingDIN,postConcDINMixing);
//if (itemID==809) printf ("DIN_Vf_ref=%f, DIN_Vf=%f, uptakeMC=%f, removalTotal=%f,totalMassRemovedMC_DIN=%f \n",DIN_Vf_ref,DIN_Vf,uptakeMC,removalTotal, totalMassRemovedMC_DIN);

//if (itemID == cell_1 || itemID == cell_2) printf("**** itemID = %d, y = %d, m = %d, d = %d, discharge = %f, dischargePre = %f, waterStoragePrev = %f, waterTotalVolume = %f\n", itemID, MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), discharge, dischargePre, waterStoragePrev, waterTotalVolume);
//if (itemID == cell_1 || itemID == cell_2) printf("ktMC = %f, DINTotalIn = %f,localLoad_DIN = %f, preFlux_DIN = %f, storeWater_DIN = %f, preConcDIN = %f\n", ktMC, DINTotalIn, localLoad_DIN, preFlux_DIN, storeWater_DIN, preConcDIN);
//if (itemID == cell_1 || itemID == cell_2) printf("aA = %f, width = %f, depth = %f, dL = %f, asDZ = %f, asHZ = %f, alphaDZ = %f, alphaHZ = %f, tstorDZ = %f, tstorHZ = %f, transferDZ = %f, transferHZ = %f\n", aA, width, depth, dL, asDZ, asHZ, alphaDZ, alphaHZ, tStorDZ, tStorHZ, transferDZ, transferHZ);
//if (itemID == cell_1 || itemID == cell_2) printf("DIN_Vf = %f, DIN_Kt = %f, hydLoad = %f, vf = %f, uptakeMC = %f, uptakeDZ = %f, uptakeHZ = %f, removalMC = %f, removalDZ = %f, removalHZ = %f\n", DIN_Vf, DIN_Kt, hydLoad, vf, uptakeMC, uptakeDZ, uptakeHZ, removalMC, removalDZ, removalHZ);
//if (itemID == cell_1 || itemID == cell_2) printf("totalMassRemovedDZ_DIN = %f, totalMassRemovedHZ_DIN = %f, totalMassRemovedMC_DIN = %f, postConcDIN = %f, postFluxDIN = %f\n", totalMassRemovedDZ_DIN, totalMassRemovedHZ_DIN, totalMassRemovedMC_DIN, postConcDIN, postFluxDIN);
//if (itemID == cell_1 || itemID == cell_2) printf("waterT = %f, postConcDINMixing = %f, tnTref = %f, DIN_Vf_ref = %f, DIN_Kt_ref = %f\n", waterT, postConcDINMixing, tnTref, DIN_Vf_ref, DIN_Kt_ref);

//        if (isnan(preFlux_DIN_denit)) printf("ID = %d, %d-%d-%d\n", itemID,MFDateGetCurrentYear(),MFDateGetCurrentMonth(),MFDateGetCurrentDay());

//        printf("m = %d, d = %d, ID = %d, randomNum = %f, VfAdjust = %f, VfAdjust2 = %f, WidthAdjust = %f\n",MFDateGetCurrentMonth(),MFDateGetCurrentDay(),itemID, randomNumber, VfAdjust, VfAdjust2, widthAdjust);
    /*    
//if (MFDateGetCurrentYear() >= 2000) {
if ((itemID == 1063) || (itemID == 1063)) {
   printf ("%i, %d, %d, %d, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f,", itemID, MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), discharge, dischargePre, waterStoragePrev, waterStorage, waterStorageChg, waterTotalVolume, runoffVol, depth, width, aA, dL,localLoad_DIN, runoffConc);
   printf ("%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f,",  storeWater_DIN_denit, storeWater_DIN_assim, storeWaterMixing_DIN, preFlux_DIN_denit, preFlux_DIN_assim, preFluxMixing_DIN, DINTotalIn_denit, DINTotalIn_assim, DINTotalInMixing, preConcDIN_denit, preConcDIN_assim, preConcMixingDIN);
   printf ("%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f,", waterT, denit_int_vf, denit_slope, assim_int_vf, assim_slope, tnQ10, tnTref, VfAdjust, DIN_Vf_ref_denit, DIN_Vf_denit, DIN_Vf_ref_assim, DIN_Vf_assim, STS_Umax, STS_UmaxT, STS_Ks, STS_U, ktHTS, DIN_Kt_denit, DIN_Kt_assim, alphaDZ, alphaHZ, transferDZ, transferHZ, tStorHZ, wetlands, HCWA, wetlandArea, wetland_HL, hydLoad, uptakeMC_denit, uptakeDZ_denit, uptakeHZ_denit, removalMC_denit, removalDZ_denit, removalHZ_denit, removalTotal_denit, totalMassRemovedMC_DIN_denit, totalMassRemovedDZ_DIN_denit, totalMassRemovedHZ_DIN_denit, flowPathRemoval_denit, uptakeMC_assim, removalMC_assim, removalTotal_assim, totalMassRemovedMC_DIN_assim, flowPathRemoval_assim);
   printf ("%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n", postFluxDIN_denit, postFluxDIN_assim, postFluxDINMixing, postConcDIN_denit, postConcDIN_assim, postConcDINMixing, postStoreWater_DIN_denit, postStoreWater_DIN_assim, postStoreWaterMixing_DIN, DINDeltaStorage_denit, DINDeltaStorage_assim, DINDeltaStorageMixing, massBalance_DIN_denit, massBalance_DIN_assim, massBalanceMixing_DIN);
//}
}  
*/

        
MFVarSetFloat (_MDOutPreFlux_DIN_denitID,            itemID, preFlux_DIN_denit);				// RJS 050911
MFVarSetFloat (_MDOutTotalMassRemoved_DIN_denitID,   itemID, totalMassRemoved_DIN_denit);		// RJS 032509
MFVarSetFloat (_MDStoreWater_DIN_denitID,            itemID, postStoreWater_DIN_denit);
MFVarSetFloat (_MDFlux_DIN_denitID,           	     itemID, postFluxDIN_denit);		// changed from MDOutFlux_DINID 091408
MFVarSetFloat (_MDOutTotalMassRemovedTS_DIN_denitID, itemID, totalMassRemovedTS_DIN_denit);
MFVarSetFloat (_MDOutPostConc_DIN_denitID,	     itemID, postConcDIN_denit);
MFVarSetFloat (_MDOutPreConc_DIN_denitID,            itemID, preConcDIN_denit);		// RJS 120408
MFVarSetFloat (_MDOutTotalMassRemovedDZ_DIN_denitID, itemID, totalMassRemovedDZ_DIN_denit);
MFVarSetFloat (_MDOutTotalMassRemovedHZ_DIN_denitID, itemID, totalMassRemovedHZ_DIN_denit);
MFVarSetFloat (_MDOutTotalMassRemovedMC_DIN_denitID, itemID, totalMassRemovedMC_DIN_denit);
MFVarSetFloat (_MDOutTotalMassRemovedLK_DIN_denitID, itemID, totalMassRemovedLK_DIN_denit);
MFVarSetFloat (_MDOutUptakeVf_denitID, 		     itemID, DIN_Vf_denit);
MFVarSetFloat (_MDOutTotalMassPre_DIN_denitID, 	     itemID, DINTotalIn_denit);		// RJS 091408
MFVarSetFloat (_MDOutMassBalance_DIN_denitID, 	     itemID, massBalance_DIN_denit);		// RJS 110308
MFVarSetFloat (_MDOutRemovalDZ_denitID,              itemID, removalDZ_denit);		//RJS 110708
MFVarSetFloat (_MDOutRemovalHZ_denitID,              itemID, removalHZ_denit);		//RJS 110708
MFVarSetFloat (_MDOutRemovalMC_denitID, 	     itemID, removalMC_denit);
MFVarSetFloat (_MDOutRemovalLK_denitID,              itemID, removalLK_denit);            // RJS 100413
MFVarSetFloat (_MDOutRemovalTotal_denitID,           itemID, removalTotal_denit);		// RJS 03-01-09
MFVarSetFloat (_MDDeltaStoreWater_DIN_denitID,       itemID, DINDeltaStorage_denit);
MFVarSetFloat (_MDFlowPathRemoval_DIN_denitID,       itemID, flowPathRemoval_denit);
MFVarSetFloat (_MDOutBackCalcR_denitID,              itemID, R_denit);
MFVarSetFloat (_MDOutBackCalcVf_denitID,             itemID, vf_denit);
MFVarSetFloat (_MDOutRiparianRemovalID,              itemID, RiparianRemoval);					// RJS 040714
MFVarSetFloat (_MDOutLocalLoad_Rip_DINID,	     itemID, localLoad_Rip_DIN);

MFVarSetFloat (_MDOutPreFlux_DIN_assimID,            itemID, preFlux_DIN_assim);				// RJS 050911
MFVarSetFloat (_MDOutTotalMassRemoved_DIN_assimID,   itemID, totalMassRemoved_DIN_assim);		// RJS 032509
MFVarSetFloat (_MDStoreWater_DIN_assimID,            itemID, postStoreWater_DIN_assim);
MFVarSetFloat (_MDFlux_DIN_assimID,           	     itemID, postFluxDIN_assim);		// changed from MDOutFlux_DINID 091408
MFVarSetFloat (_MDOutTotalMassRemovedTS_DIN_assimID, itemID, totalMassRemovedTS_DIN_assim);
MFVarSetFloat (_MDOutPostConc_DIN_assimID,           itemID, postConcDIN_assim);
MFVarSetFloat (_MDOutPreConc_DIN_assimID,            itemID, preConcDIN_assim);		// RJS 120408
MFVarSetFloat (_MDOutTotalMassRemovedDZ_DIN_assimID, itemID, totalMassRemovedDZ_DIN_assim);
MFVarSetFloat (_MDOutTotalMassRemovedHZ_DIN_assimID, itemID, totalMassRemovedHZ_DIN_assim);
MFVarSetFloat (_MDOutTotalMassRemovedMC_DIN_assimID, itemID, totalMassRemovedMC_DIN_assim);
MFVarSetFloat (_MDOutTotalMassRemovedLK_DIN_assimID, itemID, totalMassRemovedLK_DIN_assim);
MFVarSetFloat (_MDOutUptakeVf_assimID, 		     itemID, DIN_Vf_assim);
MFVarSetFloat (_MDOutTotalMassPre_DIN_assimID, 	     itemID, DINTotalIn_assim);		// RJS 091408
MFVarSetFloat (_MDOutMassBalance_DIN_assimID, 	     itemID, massBalance_DIN_assim);		// RJS 110308
MFVarSetFloat (_MDOutRemovalDZ_assimID,              itemID, removalDZ_assim);		//RJS 110708
MFVarSetFloat (_MDOutRemovalHZ_assimID,              itemID, removalHZ_assim);		//RJS 110708
MFVarSetFloat (_MDOutRemovalMC_assimID,              itemID, removalMC_assim);
MFVarSetFloat (_MDOutRemovalLK_assimID,              itemID, removalLK_assim);            // RJS 100413
MFVarSetFloat (_MDOutRemovalTotal_assimID,           itemID, removalTotal_assim);		// RJS 03-01-09
MFVarSetFloat (_MDDeltaStoreWater_DIN_assimID,       itemID, DINDeltaStorage_assim);
MFVarSetFloat (_MDFlowPathRemoval_DIN_assimID,       itemID, flowPathRemoval_assim);
MFVarSetFloat (_MDOutBackCalcR_assimID,              itemID, R_assim);
MFVarSetFloat (_MDOutBackCalcVf_assimID,             itemID, vf_assim);

MFVarSetFloat (_MDFlowPathRemovalMixing_DINID,       itemID, flowPathRemovalMixing);
MFVarSetFloat (_MDDeltaStoreWaterMixing_DINID,       itemID, DINDeltaStorageMixing);
MFVarSetFloat (_MDOutAaID,                           itemID, aA);		//RJS 110808
MFVarSetFloat (_MDOutAsDZID,                         itemID, asDZ);		//RJS 110808
MFVarSetFloat (_MDOutAsHZID,                         itemID, asHZ);		//RJS 110808
MFVarSetFloat (_MDOutMassBalanceMixing_DINID,        itemID, massBalanceMixing_DIN);	// RJS 112008
MFVarSetFloat (_MDOutConcMixing_DINID,               itemID, postConcDINMixing);			// RJS 091408
MFVarSetFloat (_MDFluxMixing_DINID, 		     itemID, postFluxDINMixing);		// changed from MDOutFluxMixing_DINID 091408
MFVarSetFloat (_MDStoreWaterMixing_DINID,            itemID, postStoreWaterMixing_DIN);
MFVarSetFloat (_MDOutTimeOfStorageDZID, 	     itemID, tStorDZ);
MFVarSetFloat (_MDOutTimeOfStorageHZID, 	     itemID, tStorHZ);
MFVarSetFloat (_MDOutTimeOfStorageMCID,              itemID, tStorMC);		
MFVarSetFloat (_MDOutTransferDZID,                   itemID, transferDZ);
MFVarSetFloat (_MDOutTransferHZID,        	     itemID, transferHZ);
MFVarSetFloat (_MDOutWaterDZID, 		     itemID, waterDZ);
MFVarSetFloat (_MDOutWaterHZID, 		     itemID, waterHZ);
MFVarSetFloat (_MDOutPercentDINIncreaseID, 	     itemID, percentIncrease);

}

int MDDINDef () {


	float par1;		//RJS 032509
	float par2;		//RJS 032509
	float par3;		//RJS 032509
	float par4;		//RJS 032509
	float par5;		//RJS 032509
	float par6;		//RJS 033009
	float par7;		//RJS 033009

	const char *optStr1, *optStr2, *optStr3, *optStr4, *optStr5, *optStr6, *optStr7;		//RJS 032509

	if (((optStr1 = MFOptionGet (MDParAlphaSTS))    != (char *) NULL) && (sscanf (optStr1,"%f",&par1) == 1)) _MDAlphaSTS = par1;  // RJS 032509
	if (((optStr2 = MFOptionGet (MDParAlphaHTS))    != (char *) NULL) && (sscanf (optStr2,"%f",&par2) == 1)) _MDAlphaHTS = par2;  // RJS 032509
	if (((optStr3 = MFOptionGet (MDParASTSA))       != (char *) NULL) && (sscanf (optStr3,"%f",&par3) == 1)) _MDASTSA = par3;  // RJS 032509
	if (((optStr4 = MFOptionGet (MDParAHTSA))       != (char *) NULL) && (sscanf (optStr4,"%f",&par4) == 1)) _MDAHTSA = par4;  // RJS 032509
	if (((optStr5 = MFOptionGet (MDParUptakeKtMC))  != (char *) NULL) && (sscanf (optStr5,"%f",&par5) == 1)) _MDUptakeKtMC = par5; // RJS 033009
	if (((optStr6 = MFOptionGet (MDParUptakeKtSTS)) != (char *) NULL) && (sscanf (optStr6,"%f",&par6) == 1)) _MDUptakeKtSTS = par6; // RJS 033009
	if (((optStr7 = MFOptionGet (MDParUptakeKtHTS)) != (char *) NULL) && (sscanf (optStr7,"%f",&par7) == 1)) _MDUptakeKtHTS = par7; // RJS 033009


	MFDefEntering ("N Processing");

  

   // Input
	if (
	    ((_MDWTemp_QxTID 		    = MDThermalInputs3Def ()) == CMfailed) ||		// comment out for no plants
//          ((_MDWTemp_QxTID 		    = MDWTempRiverRouteDef()) == CMfailed) ||
            ((_MDInRiverWidthID             = MDRiverWidthDef ())     == CMfailed) ||
            ((_MDInLocalLoad_DINID	    = MDNitrogenInputsDef())  == CMfailed) ||	// RJS 091108
            ((_MDInWWTPpointSrcID           = MDPointSourceDef())     == CMfailed) ||   // RJS 100413
            ((_MDInDischarge0ID             = MFVarGetID (MDVarDischarge0,                               "m3/s",    MFRoute,  MFState, MFBoundary))   == CMfailed) ||					// RJS 01-06-09     includes local cells runoff
	    ((_MDInDischargeID              = MFVarGetID (MDVarDischarge,                                "m3/s",    MFRoute,  MFState, MFBoundary))   == CMfailed) ||
	    ((_MDInRiverStorageID           = MFVarGetID (MDVarRiverStorage,                           "m3/day",    MFInput,  MFState, MFInitial))    == CMfailed) ||		// RJS 01-06-09		m3/day instead of m3/s
	    ((_MDInRiverStorageChgID        = MFVarGetID (MDVarRiverStorageChg,                        "m3/day",    MFInput,  MFState, MFBoundary))   == CMfailed) ||     // RJS 01-06-09
	    ((_MDInRiverOrderID             = MFVarGetID (MDVarRiverOrder,                                  "-",    MFInput,  MFState, MFBoundary))   == CMfailed) ||			//RJS 112211
	    ((_MDInLandUseAgrID             = MFVarGetID (MDVarLandUseSpatialAg,                            "-",    MFInput,  MFState, MFBoundary))   == CMfailed) ||			//RJS 112211
	    ((_MDInLandUseSubID             = MFVarGetID (MDVarLandUseSpatialSub,                           "-",    MFInput,  MFState, MFBoundary))   == CMfailed) ||			//RJS 112211
            ((_MDInRemovalOrderID           = MFVarGetID (MDVarRemovalOrder,                                "-",    MFInput,  MFState, MFBoundary))   == CMfailed) ||
	    ((_MDInRiverDepthID		    = MFVarGetID (MDVarRiverDepth,   	   	                    "m",    MFInput,  MFState, MFBoundary))   == CMfailed) ||
	    ((_MDInVfAdjustID               = MFVarGetID (MDVfAdjust,                                       "-",    MFInput,  MFState, MFBoundary))   == CMfailed) ||
	    ((_MDInWidthAdjustID            = MFVarGetID (MDVarWidthAdjust,                                 "-",    MFInput,  MFState, MFBoundary))   == CMfailed) ||           // RJS 041114
 //           ((_MDInRandomNumberID           = MFVarGetID (MDVarRandomNumber,                                "-",    MFInput,  MFState, MFBoundary))   == CMfailed) ||
            ((_MDInTransferAdjustID         = MFVarGetID (MDVarTransferAdjust,                              "-",    MFInput,  MFState, MFBoundary))   == CMfailed) ||
	    ((_MDFlux_DIN_denitID           = MFVarGetID (MDVarFluxDIN_denit,                          "kg/day",    MFRoute,   MFFlux, MFBoundary))   == CMfailed) ||		// RJS 091408
	    ((_MDStoreWater_DIN_denitID     = MFVarGetID (MDVarStoreWaterDIN_denit,                    "kg/day",   MFOutput,  MFState, MFInitial))    == CMfailed) ||		// RJS 091108
	    ((_MDFlux_DIN_assimID           = MFVarGetID (MDVarFluxDIN_assim,                          "kg/day",    MFRoute,   MFFlux, MFBoundary))   == CMfailed) ||		// RJS 091408
	    ((_MDStoreWater_DIN_assimID     = MFVarGetID (MDVarStoreWaterDIN_assim,                    "kg/day",   MFOutput,  MFState, MFInitial))    == CMfailed) ||		// RJS 091108
	    ((_MDInRunoffVolID              = MFVarGetID (MDVarRunoffVolume, 		                 "m3/s",    MFInput,  MFState, MFBoundary))   == CMfailed) ||		// RJS 091108
	    ((_MDInTnQ10ID		    = MFVarGetID (MDVarTnQ10,                                       "-",    MFInput,  MFState, MFBoundary))   == CMfailed) ||           // RJS 102410
	    ((_MDInLakeYesNoID              = MFVarGetID (MDVarLakeYesNo, 		                    "-",    MFInput,  MFState, MFBoundary))   == CMfailed) ||		// RJS 091108
	    ((_MDInLakePointAreaID          = MFVarGetID (MDVarLakePointArea, 		                  "km2",    MFInput,  MFState, MFBoundary))   == CMfailed) ||		// RJS 091108
            ((_MDInWetlandsID               = MFVarGetID (MDVarFracWetlandArea,                             "-",    MFInput,  MFState, MFBoundary))   == CMfailed) ||           // RJS 112513
            ((_MDInHCWAID                   = MFVarGetID (MDVarHCWA,                                        "-",    MFInput,  MFState, MFBoundary))   == CMfailed) ||           // RJS 112513 
            ((_MDInSTSUptakeMaxID           = MFVarGetID (MDVarSTSUptakeMax,                          "mg/m2/s",    MFInput,  MFState, MFBoundary))   == CMfailed) ||           // RJS 112513
            ((_MDInSTSKsID                  = MFVarGetID (MDVarSTSKs,                                    "mg/l",    MFInput,  MFState, MFBoundary))   == CMfailed) ||           // RJS 112513
            ((_MDInContributingAreaID       = MFVarGetID (MDVarContributingArea,                          "km2",    MFInput,  MFState, MFBoundary))   == CMfailed) ||           // RJS 120213
            ((_MDInBankFullID               = MFVarGetID (MDVarBankFullDepth,                            "mm/d",    MFInput,  MFState, MFBoundary))   == CMfailed) ||           // RJS 120213
            ((_MDInFloodplainSwitchID       = MFVarGetID (MDVarFloodplainSwitch,                            "-",    MFInput,  MFState, MFBoundary))   == CMfailed) ||           // RJS 120213
            ((_MDInScaleAlphaID             = MFVarGetID (MDVarScaleAlpha,                                  "-",    MFInput,  MFState, MFBoundary))   == CMfailed) ||           // RJS 120413
            ((_MDInTotalEvaporationID           = MFVarGetID (MDVarTotalEvaporation,                           "m3",    MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
                
   // Output
            // denitrification    
	    ((_MDOutPostConc_DIN_denitID           = MFVarGetID (MDVarPostConcDIN_denit,	         "mg/L",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||
	    ((_MDOutPreConc_DIN_denitID		   = MFVarGetID (MDVarPreConcDIN_denit,	                 "mg/L",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||
	    ((_MDOutTotalMassRemovedTS_DIN_denitID = MFVarGetID (MDVarTotalMassRemovedTSDIN_denit,     "kg/day",   MFOutput,   MFFlux, MFBoundary))   == CMfailed) ||
	    ((_MDOutTotalMassRemovedDZ_DIN_denitID = MFVarGetID (MDVarTotalMassRemovedDZDIN_denit,     "kg/day",   MFOutput,   MFFlux, MFBoundary))   == CMfailed) ||
	    ((_MDOutTotalMassRemovedHZ_DIN_denitID = MFVarGetID (MDVarTotalMassRemovedHZDIN_denit,     "kg/day",   MFOutput,   MFFlux, MFBoundary))   == CMfailed) ||
	    ((_MDOutTotalMassRemovedMC_DIN_denitID = MFVarGetID (MDVarTotalMassRemovedMCDIN_denit,     "kg/day",   MFOutput,   MFFlux, MFBoundary))   == CMfailed) ||
	    ((_MDOutTotalMassRemovedLK_DIN_denitID = MFVarGetID (MDVarTotalMassRemovedLKDIN_denit,     "kg/day",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||
            ((_MDOutTotalMassRemoved_DIN_denitID   = MFVarGetID (MDVarTotalMassRemovedDIN_denit,       "kg/day",   MFOutput,   MFFlux, MFBoundary))   == CMfailed) ||		// RJS 032509
	    ((_MDOutRiparianRemovalID              = MFVarGetID (MDVarRiparianRemoval,       		"kg/day",  MFOutput,   MFFlux, MFBoundary))   == CMfailed) ||		// RJS 032509
	    ((_MDOutPreFlux_DIN_denitID            = MFVarGetID (MDVarPreFluxDIN_denit,                "kg/day",   MFOutput,  MFState, MFBoundary))   == CMfailed) || 	//RJS 050911
	    ((_MDOutUptakeVf_denitID               = MFVarGetID (MDVarUptakeVf_denit,                   "m/day",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||	// RJS 090508
	    ((_MDOutTotalMassPre_DIN_denitID       = MFVarGetID (MDVarTotalMassPreDIN_denit,           "kg/day",   MFOutput,   MFFlux, MFBoundary))   == CMfailed) ||	// RJS 091408
	    ((_MDOutMassBalance_DIN_denitID	   = MFVarGetID (MDVarMassBalanceDIN_denit,             "kg/kg",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||	// RJS 110308
	    ((_MDOutRemovalDZ_denitID              = MFVarGetID (MDVarRemovalDZ_denit,                  "kg/kg",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||	// RJS 110708
	    ((_MDOutRemovalHZ_denitID              = MFVarGetID (MDVarRemovalHZ_denit,                  "kg/kg",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||	// RJS 110708
	    ((_MDOutRemovalMC_denitID              = MFVarGetID (MDVarRemovalMC_denit,                  "kg/kg",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||	// RJS 110708
	    ((_MDOutRemovalLK_denitID              = MFVarGetID (MDVarRemovalLK_denit,                  "kg/kg",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||
            ((_MDOutRemovalTotal_denitID           = MFVarGetID (MDVarRemovalTotal_denit,               "kg/kg",   MFOutput,  MFState, MFBoundary))   == CMfailed) || 	// RJS 030109
	    ((_MDOutDINVf_denitID                  = MFVarGetID (MDVarDINVf_denit,                          "-",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||	// RJS 112210
	    ((_MDOutDINKt_denitID                  = MFVarGetID (MDVarDINKt_denit,                          "-",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||	// RJS 112210
  	    ((_MDOutBackCalcR_denitID              = MFVarGetID (MDVarBackCalcR_denit,                      "-",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||
            ((_MDOutBackCalcVf_denitID             = MFVarGetID (MDVarBackCalcVf_denit,                     "-",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||
            ((_MDDeltaStoreWater_DIN_denitID       = MFVarGetID (MDVarDeltaStoreWaterDIN_denit,        "kg/day",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||   // RJS 112008
	    ((_MDFlowPathRemoval_DIN_denitID       = MFVarGetID (MDVarFlowPathRemovalDIN_denit,        "kg/day",   MFOutput,   MFFlux, MFBoundary))   == CMfailed) ||   // RJS 112008
            // assimulation    
	    ((_MDOutPostConc_DIN_assimID           = MFVarGetID (MDVarPostConcDIN_assim,	         "mg/L",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||
	    ((_MDOutPreConc_DIN_assimID		   = MFVarGetID (MDVarPreConcDIN_assim,	                 "mg/L",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||
	    ((_MDOutTotalMassRemovedTS_DIN_assimID = MFVarGetID (MDVarTotalMassRemovedTSDIN_assim,     "kg/day",   MFOutput,   MFFlux, MFBoundary))   == CMfailed) ||
	    ((_MDOutTotalMassRemovedDZ_DIN_assimID = MFVarGetID (MDVarTotalMassRemovedDZDIN_assim,     "kg/day",   MFOutput,   MFFlux, MFBoundary))   == CMfailed) ||
	    ((_MDOutTotalMassRemovedHZ_DIN_assimID = MFVarGetID (MDVarTotalMassRemovedHZDIN_assim,     "kg/day",   MFOutput,   MFFlux, MFBoundary))   == CMfailed) ||
	    ((_MDOutTotalMassRemovedMC_DIN_assimID = MFVarGetID (MDVarTotalMassRemovedMCDIN_assim,     "kg/day",   MFOutput,   MFFlux, MFBoundary))   == CMfailed) ||
	    ((_MDOutTotalMassRemovedLK_DIN_assimID = MFVarGetID (MDVarTotalMassRemovedLKDIN_assim,     "kg/day",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||
            ((_MDOutTotalMassRemoved_DIN_assimID   = MFVarGetID (MDVarTotalMassRemovedDIN_assim,       "kg/day",   MFOutput,   MFFlux, MFBoundary))   == CMfailed) ||		// RJS 032509
	    ((_MDOutPreFlux_DIN_assimID            = MFVarGetID (MDVarPreFluxDIN_assim,                "kg/day",   MFOutput,  MFState, MFBoundary))   == CMfailed) || 	//RJS 050911
	    ((_MDOutUptakeVf_assimID               = MFVarGetID (MDVarUptakeVf_assim,                   "m/day",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||	// RJS 090508
	    ((_MDOutTotalMassPre_DIN_assimID       = MFVarGetID (MDVarTotalMassPreDIN_assim,           "kg/day",   MFOutput,   MFFlux, MFBoundary))   == CMfailed) ||	// RJS 091408
	    ((_MDOutMassBalance_DIN_assimID	   = MFVarGetID (MDVarMassBalanceDIN_assim,             "kg/kg",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||	// RJS 110308
	    ((_MDOutRemovalDZ_assimID              = MFVarGetID (MDVarRemovalDZ_assim,                  "kg/kg",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||	// RJS 110708
	    ((_MDOutRemovalHZ_assimID              = MFVarGetID (MDVarRemovalHZ_assim,                  "kg/kg",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||	// RJS 110708
	    ((_MDOutRemovalMC_assimID              = MFVarGetID (MDVarRemovalMC_assim,                  "kg/kg",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||	// RJS 110708
	    ((_MDOutRemovalLK_assimID              = MFVarGetID (MDVarRemovalLK_assim,                  "kg/kg",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||
            ((_MDOutRemovalTotal_assimID           = MFVarGetID (MDVarRemovalTotal_assim,               "kg/kg",   MFOutput,  MFState, MFBoundary))   == CMfailed) || 	// RJS 030109
	    ((_MDOutDINVf_assimID                  = MFVarGetID (MDVarDINVf_assim,                          "-",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||	// RJS 112210
	    ((_MDOutDINKt_assimID                  = MFVarGetID (MDVarDINKt_assim,                          "-",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||	// RJS 112210
  	    ((_MDOutBackCalcR_assimID              = MFVarGetID (MDVarBackCalcR_assim,                      "-",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||
            ((_MDOutBackCalcVf_assimID             = MFVarGetID (MDVarBackCalcVf_assim,                     "-",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||
            ((_MDDeltaStoreWater_DIN_assimID       = MFVarGetID (MDVarDeltaStoreWaterDIN_assim,        "kg/day",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||   // RJS 112008
	    ((_MDFlowPathRemoval_DIN_assimID       = MFVarGetID (MDVarFlowPathRemovalDIN_assim,        "kg/day",   MFOutput,   MFFlux, MFBoundary))   == CMfailed) ||   // RJS 112008
	    ((_MDOutLocalLoad_Rip_DINID            = MFVarGetID (MDVarLocalLoadRipDIN,        	       "kg/day",   MFOutput,   MFFlux, MFBoundary))   == CMfailed) ||   // RJS 112008

            ((_MDOutMassBalanceMixing_DINID	= MFVarGetID (MDVarMassBalanceMixingDIN,       "kg/kg",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||	// RJS 110308
            ((_MDOutConcMixing_DINID         	= MFVarGetID (MDVarConcMixing,    	        "mg/L",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||	// RJS 091108   
            ((_MDOutWaterDZID			= MFVarGetID (MDVarWaterDZ, 		          "m3",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||
	    ((_MDOutWaterHZID			= MFVarGetID (MDVarWaterHZ, 		          "m3",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||
	    ((_MDOutTimeOfStorageDZID		= MFVarGetID (MDVarTimeOfStorageDZ,             "days",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||
	    ((_MDOutTimeOfStorageHZID		= MFVarGetID (MDVarTimeOfStorageHZ,             "days",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||
	    ((_MDOutTimeOfStorageMCID		= MFVarGetID (MDVarTimeOfStorageMC,             "days",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||	// RJS 120608
	    ((_MDOutTransferDZID		= MFVarGetID (MDVarTransferDZ,                  "m3/s",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||
	    ((_MDOutTransferHZID		= MFVarGetID (MDVarTransferHZ,                  "m3/s",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||
	    ((_MDFluxMixing_DINID		= MFVarGetID (MDVarFluxMixingDIN,  	      "kg/day",    MFRoute,   MFFlux, MFBoundary))   == CMfailed) ||	// RJS 091108
	    ((_MDStoreWaterMixing_DINID      	= MFVarGetID (MDVarStoreWaterMixingDIN,       "kg/day",   MFOutput,  MFState, MFInitial))    == CMfailed) ||	// RJS 091108
	    ((_MDOutAsDZID                   	= MFVarGetID (MDVarAsDZ,                          "m2",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||   // RJS 110808
	    ((_MDOutAsHZID                   	= MFVarGetID (MDVarAsHZ,                          "m2",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||   // RJS 110808
	    ((_MDOutAaID                     	= MFVarGetID (MDVarAa,                            "m2",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||   // RJS 110808
	    ((_MDOutVelocityID                  = MFVarGetID (MDVarVelocity,                     "m/s",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||   // RJS 112108
	    ((_MDDeltaStoreWaterMixing_DINID    = MFVarGetID (MDVarDeltaStoreWaterMixingDIN,  "kg/day",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||   // RJS 112008
	    ((_MDFlowPathRemovalMixing_DINID    = MFVarGetID (MDVarFlowPathRemovalMixingDIN,  "kg/day",   MFOutput,   MFFlux, MFBoundary))   == CMfailed) ||   // RJS 112008
	    ((_MDOutPercentDINIncreaseID        = MFVarGetID (MDVarPercentDINIncrease,             "-",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||   // RJS 112008


	(MFModelAddFunction (_MDNProcessing) == CMfailed)) return (CMfailed);

	MFDefLeaving ("N Processing");

	return (_MDFlux_DIN_denitID);		// RJS 091408
}




