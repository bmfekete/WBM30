/******************************************************************************
GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2014, UNH - CCNY/CUNY

MDWTempRiverRoute.c

wil.wollheim@unh.edu

Route temperature through river network

*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>
#include <math.h>

// Model
static int _MDWTempRiverRouteID        = MFUnset;

// Input
static int _MDInDischargeID            = MFUnset;
static int _MDInDischargeIncomingID    = MFUnset;
static int _MDInRunoffVolumeID         = MFUnset;
static int _MDInWTempRiverID           = MFUnset;
static int _MDInRiverWidthID           = MFUnset;
static int _MDInRiverStorageChgID      = MFUnset;
static int _MDInRiverStorageID         = MFUnset;
static int _MDInSolarRadID             = MFUnset;
static int _MDInWindSpeedID            = MFUnset;
static int _MDInCloudCoverID           = MFUnset;
static int _MDInAirTemperatureID       = MFUnset;
static int _MDInSnowPackID             = MFUnset;
static int _MDInResStorageChangeID     = MFUnset;
static int _MDInResStorageID           = MFUnset;
static int _MDInResCapacityID     	   = MFUnset;
//static int _MDInWdl_QxTID              = MFUnset;	//RJS 082011	//commented out 013112
//static int _MDInThermalWdlID           = MFUnset;	//RJS 082011	//commented out 013112
//static int _MDInWarmingTempID          = MFUnset;	//RJS 082011	//commented out 013112
// Output
static int _MDLocalIn_QxTID            = MFUnset;
static int _MDRemoval_QxTID            = MFUnset;
static int _MDFlux_QxTID               = MFUnset;
static int _MDStorage_QxTID            = MFUnset;
static int _MDDeltaStorage_QxTID       = MFUnset;
static int _MDWTemp_QxTID              = MFUnset;
static int _MDWTempDeltaT_QxTID        = MFUnset;
static int _MDFluxMixing_QxTID         = MFUnset;
static int _MDStorageMixing_QxTID      = MFUnset;
static int _MDDeltaStorageMixing_QxTID = MFUnset;
static int _MDWTempMixing_QxTID        = MFUnset;
static int _MDInDingmanOnOffID		   = MFUnset;
static int _MDDeltaTID				   = MFUnset;



static void _MDWTempRiverRoute (int itemID) {
	 float Q;
	 float Q_incoming;
     float RO_Vol;
	 float RO_WTemp;
	 float QxT_input;
	 float QxT;
	 float QxTnew = 0;
	 float QxTout = 0;
     float Q_WTemp;
     float Q_WTemp_new;
     float StorexT;
     float StorexT_new;
     float DeltaStorexT;
     float SnowPack;

     float DingmanOnOff;

     //processing variables
     float channelWidth;
     float waterStorageChange;
     float waterStorage;
     float ResWaterStorageChange = 0;
     float ResWaterStorage = 0;
     float solarRad;
     float windSpeed;
     float cloudCover;
     float Tair;
     float Tequil = 0;
     float HeatLoss_int = 4396.14; // is intercept assuming no wind and clouds
     float HeatLoss_slope = 1465.38; // is slope assuming no wind and clouds
     float deltaT;
     
     float ReservoirArea;
     float ReservoirDepth;
     float ReservoirVelocity;

     float RO_PoolRelease;

     // conservative mixing variables (parallel to those above_
     float QxT_mix;
     float QxTnew_mix = 0;
     float QxTout_mix = 0;
     float Q_WTemp_mix;
     float StorexT_mix;  
     float StorexT_new_mix;  
     float DeltaStorexT_mix; 
     float QxTRemoval;
     int day;
     int month;
     
     float Q_upstream;		// RJS 030113
     float resCapacity;		//RJS 071511	Reservoir capacity [km3]

     day = MFDateGetCurrentDay();
     month = MFDateGetCurrentMonth();

   	 Q                     = MFVarGetFloat (_MDInDischargeID,         itemID, 0.0);
   	 Q_incoming            = MFVarGetFloat (_MDInDischargeIncomingID, itemID, 0.0); // already includes local runoff AND ROUTING (storage change in grid cell)
     RO_Vol                = MFVarGetFloat (_MDInRunoffVolumeID,      itemID, 0.0);
   	 RO_WTemp              = MFVarGetFloat (_MDInWTempRiverID,        itemID, 0.0);
     SnowPack              = MFVarGetFloat (_MDInSnowPackID,          itemID, 0.0);
     DingmanOnOff		   = MFVarGetFloat (_MDInDingmanOnOffID,      itemID, 0.0);

 	
     if (_MDInResStorageID != MFUnset){
         ResWaterStorageChange = MFVarGetFloat ( _MDInResStorageChangeID, itemID, 0.0) * pow(1000,3); // convert to m3/
         ResWaterStorage       = MFVarGetFloat ( _MDInResStorageID,       itemID, 0.0) * pow(1000,3); // convert to m3 
         resCapacity           = MFVarGetFloat (_MDInResCapacityID,       itemID, 0.0);	//RJS 071511
     }
     else
     {
         ResWaterStorageChange =
         ResWaterStorage       = 
         resCapacity           = 0.0;	//RJS 071511
    }
     
     waterStorageChange    = MFVarGetFloat ( _MDInRiverStorageChgID,  itemID, 0.0);
   	 waterStorage          = MFVarGetFloat ( _MDInRiverStorageID,     itemID, 0.0);
   	 channelWidth          = MFVarGetFloat ( _MDInRiverWidthID,       itemID, 0.0);
 	 solarRad              = MFVarGetFloat ( _MDInSolarRadID,         itemID, 0.0); //MJ/m2/d - CHECK UNITS
 	 windSpeed             = MFVarGetFloat ( _MDInWindSpeedID,        itemID, 0.0);
     cloudCover            = MFVarGetFloat ( _MDInCloudCoverID,       itemID, 0.0);
     Tair                  = MFVarGetFloat ( _MDInAirTemperatureID,   itemID, 0.0);
        	 
     QxT                   = MFVarGetFloat (_MDFlux_QxTID,            itemID, 0.0);
     StorexT               = MFVarGetFloat (_MDStorage_QxTID,         itemID, 0.0);
     QxT_mix               = MFVarGetFloat (_MDFluxMixing_QxTID,      itemID, 0.0);
     StorexT_mix           = MFVarGetFloat (_MDStorageMixing_QxTID,   itemID, 0.0);

     Q_upstream	 = Q + waterStorageChange - RO_Vol;		// RJS 030113	Amount of flow coming from upstream grid cell; Q is routed volume going downstream, waterStorageChange (-) is amount lost from WaterStorage, (+) amount added to waterStorage


     if(Q < 0.0)  Q = 0.0;							//RJS 120409
     if(Q_incoming < 0.0) Q_incoming = 0.0;			//RJS 120409

     if(resCapacity > 0.0){
    	 waterStorage = waterStorage + ResWaterStorage;
    	 waterStorageChange = waterStorageChange + ResWaterStorageChange;
    	 ReservoirArea = pow(((ResWaterStorage / pow(10,6)) / 9.208),(1 / 1.114)) * 1000 * 1000;  // m2, from Takeuchi 1997 - original equation has V in 10^6 m3 and A in km2
    	 ReservoirDepth = (ResWaterStorage / ReservoirArea); //m
    	 ReservoirVelocity = Q / (ReservoirArea); // m/s
    	 channelWidth = MDMaximum(channelWidth, (Q / (ReservoirDepth * ReservoirVelocity))); // m

    	 QxT_input = RO_Vol * RO_WTemp * 86400.0; 											//RJS 071511 					//m3*degC/d
    	 QxTnew = QxT + QxT_input + StorexT; 												//RJS 071511					//m3*degC/d
    	 QxTnew_mix = QxT_mix + QxT_input + StorexT_mix;									//RJS 071511

    	 if (Q_incoming > 0.000001) {
//    		 Q_WTemp = QxTnew / ((Q_incoming) * 86400 + (waterStorage - waterStorageChange)); 			//RJS 071511	 commented out 112112				//degC
//    		 Q_WTemp_mix = QxTnew_mix / ((Q_incoming) * 86400 + (waterStorage - waterStorageChange));	//RJS 071511	 commented out 112112			//degC
    		 Q_WTemp = QxTnew / ((Q_incoming) * 86400 + ((waterStorage - waterStorageChange) * 86400)); 			//RJS 112112					//degC
    		 Q_WTemp_mix = QxTnew_mix / ((Q_incoming) * 86400 + ((waterStorage - waterStorageChange) * 86400));		//RJS 112112					//degC

    	 }

    	 else {
    		 if (waterStorage > 0) {
 //   			 Q_WTemp	 = StorexT / waterStorage;		// RJS 071511	//degC		commented out 112112
 //   			 Q_WTemp_mix = StorexT_mix / waterStorage;	// RJS 071511	//degC		commented out 112112
     			 Q_WTemp	 = StorexT / (waterStorage * 86400);		// RJS 071511	//degC	RJS 112112
     			 Q_WTemp_mix = StorexT_mix / (waterStorage * 86400);	// RJS 071511	//degC	RJS 112112
    		 }
			 else {
				 Q_WTemp 	 = 0.0;			//RJS 071511
				 Q_WTemp_mix = 0.0;			//RJS 071511
			 }
    	 }

    	 Q_WTemp_new = Q_WTemp;														//RJS 071511

//    	 StorexT_new      = waterStorage * Q_WTemp_new; 							//RJS 071511	//m3*degC	commented out 112112
    	 StorexT_new      = (waterStorage * 86400) * Q_WTemp_new; 					//RJS 112112
    	 DeltaStorexT     = StorexT_new - StorexT; 									//RJS 071511
    	 QxTout           = Q * 86400.0 * Q_WTemp_new ; 							//RJS 071511	//m3*degC/d
    	 QxTRemoval       = QxTnew - (StorexT_new + QxTout); 						//RJS 071511	//m3*degC/d
//    	 StorexT_new_mix  = waterStorage * Q_WTemp_mix; 							//RJS 071511	//m3*degC
    	 StorexT_new_mix  = (waterStorage * 86400) * Q_WTemp_mix; 					//RJS 112112
    	 DeltaStorexT_mix = StorexT_new_mix - StorexT_mix;							//RJS 071511
    	 QxTout_mix       = Q * 86400.0 * Q_WTemp_mix; 								//RJS 071511	//m3*degC/s


    	          MFVarSetFloat(_MDLocalIn_QxTID, itemID, QxT_input);
    	          MFVarSetFloat(_MDFlux_QxTID, itemID, QxTout);
    	          MFVarSetFloat(_MDStorage_QxTID, itemID, StorexT_new);
    	          MFVarSetFloat(_MDDeltaStorage_QxTID, itemID, DeltaStorexT);
    	          MFVarSetFloat(_MDWTemp_QxTID, itemID, Q_WTemp_new);
    	          MFVarSetFloat(_MDWTempDeltaT_QxTID, itemID, deltaT);
    	          MFVarSetFloat(_MDFluxMixing_QxTID, itemID, QxTout_mix);
    	          MFVarSetFloat(_MDStorageMixing_QxTID, itemID, StorexT_new_mix);
    	          MFVarSetFloat(_MDDeltaStorageMixing_QxTID, itemID, DeltaStorexT_mix);
    	          MFVarSetFloat(_MDWTempMixing_QxTID, itemID, Q_WTemp_mix);

     }		// end Reservoirs

     else{
    	 ReservoirArea = 0.0;
    	 ReservoirVelocity = 0.0;
    	 ReservoirDepth = 0.0;

     QxT_input = RO_Vol * RO_WTemp * 86400.0; //m3*degC/d 

     if((Q_incoming) > 0.000001) {			 //do not include water storage in this check - will screw up mixing estimates
         QxTnew = QxT + QxT_input + StorexT; //m3*degC/d		StorexT is heat from YESTERDAY, w/YESTERDAY's volume
   	     QxTnew_mix = QxT_mix + QxT_input + StorexT_mix;
        
  // 	     Q_WTemp = QxTnew / ((Q_incoming) * 86400 + (waterStorage - waterStorageChange)); //degC				commented out 112112
  //	     Q_WTemp_mix = QxTnew_mix / ((Q_incoming) * 86400 + (waterStorage - waterStorageChange)); //degC		commented out 112112
  //       Q_WTemp = QxTnew / ((Q_incoming) * 86400 + ((waterStorage - waterStorageChange) * 86400)); //degC		commented out 030113	RJS 112112 water storage is in m3/s!!!
  //       Q_WTemp_mix = QxTnew_mix / ((Q_incoming) * 86400 + ((waterStorage - waterStorageChange) * 86400)); //degC commented out 030113	RJS 112112 water storage is in m3/s!!!

   	     Q_WTemp = QxTnew / ((Q_upstream + RO_Vol + waterStorage - waterStorageChange) * 86400); 			//degC		RJS 030113
         Q_WTemp_mix = QxTnew_mix / ((Q_upstream + RO_Vol + waterStorage - waterStorageChange) * 86400); 	//degC		RJS 030113

        ///Temperature Processing using Dingman 1972 
         if (cloudCover < 95){  // clear skies, assume cloud cover < 95% convertcalories / cm2 /d to kJ/m2/d
            HeatLoss_int = (105 + 23 *  windSpeed) * 4.1868 / 1000 * 100 * 100; // kJ/m2/d
            HeatLoss_slope = (35 + 4.2 * windSpeed) * 4.1868 / 1000 * 100 * 100;// kJ/m2/d/degC

         } else{                // cloudy skies, assume cloud cover > 95%
        	 HeatLoss_int = (-73 + 9.1 *  windSpeed) * 4.1868 / 1000 * 100 * 100;
        	 HeatLoss_slope = (37 + 4.6 * windSpeed) * 4.1868 / 1000 * 100 * 100;
         }
         Tequil = Tair + (((solarRad * 1000) - HeatLoss_int) / HeatLoss_slope); //solar rad converted from MJ to kJ/m2/d

         if (channelWidth > 0){
        //	 Q_WTemp_new = MDMaximum(0, (((Q_WTemp - Tequil) * exp((-HeatLoss_slope * MFModelGetLength(itemID)) / (999.73 * 4.1922 * (Q * 86400.0 / channelWidth)))) + Tequil));		// commented out 073012
        	 if (DingmanOnOff > 0.0) {
        	 Q_WTemp_new = MDMaximum(0, (((Q_WTemp - Tequil) * exp((-HeatLoss_slope * MFModelGetLength(itemID)) / (999.73 * 4.1922 * (Q * 86400.0 / channelWidth)))) + Tequil));				// RJS 073012
  //      	 Q_WTemp_new = (((Q_WTemp - Tequil) * exp((-HeatLoss_slope * MFModelGetLength(itemID)) / (999.73 * 4.1922 * (Q * 86400.0 / channelWidth)))) + Tequil);								// RJS 120612
        	 }
        	 else Q_WTemp_new = Q_WTemp;																																				// RJS 073012

         }
         else {

        	 if (DingmanOnOff > 0.0) {
        		 Q_WTemp_new = MDMaximum(0, Tequil);	// commented out 073012
        	//	 Q_WTemp_new = Tequil;					// RJS 120612
        	 }
        	 else Q_WTemp_new = Q_WTemp;				// RJS 073012
         }
         
         
         deltaT = Q_WTemp_new - Q_WTemp;

 //  	     StorexT_new  = waterStorage * Q_WTemp_new; //m3*degC		commented out 112112
         StorexT_new  = waterStorage * 86400 * Q_WTemp_new; //m3*degC	RJS 112112
         DeltaStorexT = StorexT_new - StorexT; //
         QxTout       = Q * 86400.0 * Q_WTemp_new ; //m3*degC/d
         QxTRemoval   = QxTnew - (StorexT_new + QxTout); //m3*degC/d
//         StorexT_new_mix  = waterStorage * Q_WTemp_mix; //m3*degC		commented out 112112
         StorexT_new_mix  = waterStorage * 86400 * Q_WTemp_mix; //m3*degC		RJS 112112
         DeltaStorexT_mix = StorexT_new_mix - StorexT_mix;
         QxTout_mix       = Q * 86400.0 * Q_WTemp_mix; //m3*degC/s

 //    	if ((itemID == 880) || (itemID == 389) || (itemID == 334) || (itemID == 233)) {
 //        if ((itemID == 482) || (itemID == 386) || (itemID == 881)) {
 //        if ((itemID == 38) || (itemID == 37)){
 //        printf("**** Runoff: ID = %d, y = %d, m = %d, d = %d, ROvol = %f, RO_WTemp = %f, QxT_input = %f\n", itemID, MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), RO_Vol, RO_WTemp, QxT_input);
 //     	 printf("**** Upstream: Q_incoming = %f, QxT = %f, Q_fromUpstream = %f, T_fromUpstream = %f\n", Q_incoming, QxT, Q_incoming - RO_Vol, QxT / ((Q_incoming - RO_Vol)*86400));
 //     	 printf("**** Pre-Ding: QxTnew = %f, Q_WTemp = %f\n", QxTnew, Q_WTemp);
//       	 printf("**** Post-Ding: QxTout = %f, Q_WTemp_new = %f, Q = %f\n,", QxTout, Q_WTemp_new, Q);		// RJS 113012
//         printf("**** airT = %f, Tequil = %f, GJ_out = %f\n", Tair, Tequil, (Q_WTemp_new + 273.15) * Q * 86400 * 4.18 * 0.001);
 //        }

 //        printf("solarRad = %f,  dL = %f\n ", solarRad, MFModelGetLength(itemID));

         MFVarSetFloat(_MDDeltaTID,      itemID, deltaT);
         MFVarSetFloat(_MDLocalIn_QxTID, itemID, QxT_input);
         MFVarSetFloat(_MDRemoval_QxTID, itemID, QxTRemoval);
         MFVarSetFloat(_MDFlux_QxTID, itemID, QxTout);
         MFVarSetFloat(_MDStorage_QxTID, itemID, StorexT_new);
         MFVarSetFloat(_MDDeltaStorage_QxTID, itemID, DeltaStorexT);
         MFVarSetFloat(_MDWTemp_QxTID, itemID, Q_WTemp_new);
         MFVarSetFloat(_MDWTempDeltaT_QxTID, itemID, deltaT);
         MFVarSetFloat(_MDFluxMixing_QxTID, itemID, QxTout_mix);
         MFVarSetFloat(_MDStorageMixing_QxTID, itemID, StorexT_new_mix);
         MFVarSetFloat(_MDDeltaStorageMixing_QxTID, itemID, DeltaStorexT_mix);
         MFVarSetFloat(_MDWTempMixing_QxTID, itemID, Q_WTemp_mix);
  	     }
         else{
        	 if (waterStorage > 0){
        		   QxTnew = QxT_input + StorexT; //m3*degC
        		   QxTnew_mix = QxT_input + StorexT_mix;
        	 }
        	 else{
        		 QxTnew = 0; 
        		 QxTnew_mix = 0;
             }
        	 StorexT_new  = 0.0; //m3*degC
        	 DeltaStorexT = StorexT_new - StorexT; //
        	 QxTout       = 0.0; //m3*degC/dStorexT_new_mix  = 0; //m3*degC
             QxTRemoval   = 0.0; //m3*degC/d
             StorexT_new_mix  = 0.0; //m3*degC
        	 DeltaStorexT_mix = StorexT_new_mix - StorexT_mix;
        	 QxTout_mix       = 0.0; //m3*degC/s




	//             printf("m = %d, d = %d, itemID = %d, QxT_input = %f, RO_Vol = %f, RO_WTemp = %f\n Q_WTemp = %f, Q_WTemp_new = %f, Q_WTemp_mix = %f, Q = %f, Q_incoming = %f\n", MFDateGetCurrentMonth (), MFDateGetCurrentDay (), itemID, QxT_input, RO_Vol, RO_WTemp, Q_WTemp, Q_WTemp_new, Q_WTemp_mix, Q, Q_incoming);

             MFVarSetFloat(_MDLocalIn_QxTID, itemID, 0.0);
             MFVarSetFloat(_MDRemoval_QxTID, itemID, QxTRemoval);
        	 MFVarSetFloat(_MDFlux_QxTID, itemID, QxTout);
        	 MFVarSetFloat(_MDStorage_QxTID, itemID, StorexT_new);
        	 MFVarSetFloat(_MDDeltaStorage_QxTID, itemID, DeltaStorexT);
           	 MFVarSetFloat(_MDFluxMixing_QxTID, itemID, QxTout_mix);
        	 MFVarSetFloat(_MDStorageMixing_QxTID, itemID, StorexT_new_mix);
        	 MFVarSetFloat(_MDDeltaStorageMixing_QxTID, itemID, DeltaStorexT_mix);
        
        	 MFVarSetMissingVal(_MDWTemp_QxTID, itemID);
             MFVarSetMissingVal(_MDWTempDeltaT_QxTID, itemID);
             MFVarSetMissingVal(_MDWTempMixing_QxTID, itemID);
         }
  	float mb;
  	float mbmix;
  	mb = QxT_input + QxT - QxTRemoval - QxTout - DeltaStorexT;
  	mbmix = (QxT_input + QxT_mix - QxTout_mix - DeltaStorexT_mix);
     }	//RJS 071511
// 1145, 524 or 1224, 531
//    if (itemID == 83 || itemID == 82) printf("**** ID = %d, %d %d %d, Incoming_T = %f, RO_WTemp = %f,  Store_WTemp = %f, Q_WTemp = %f,\n", itemID, MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), QxT / ((Q_incoming - RO_Vol) * 86400), RO_WTemp, StorexT / ((waterStorage - waterStorageChange) * 86400), Q_WTemp);
//    if (itemID == 83 || itemID == 82) printf("**temp** ID = %d, %d %d %d, Q_upstream = %f, RO_Vol = %f, Q_incoming = %f, waterStorage - waterStorageChange = %f\n", itemID, MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), Q_upstream * 86400, RO_Vol * 86400, Q_incoming * 86400, (waterStorage - waterStorageChange) * 86400);
//    if (itemID == 83 || itemID == 82) printf("QxT = %f, QxT_input = %f, StorexT = %f, QxTnew = %f\n", QxT, QxT_input, StorexT, QxTnew);
//    if (itemID == 83 || itemID == 82) printf("Exit:  Q_WTemp_new = %f, Store_WTemp_new = %f\n", Q_WTemp_new, StorexT_new / (waterStorage * 86400));
//    if (itemID == 83 || itemID == 82) printf("Exit:  Q = %f, waterStorage = %f, waterStorageChange = %f\n", Q * 86400, waterStorage * 86400, waterStorageChange * 86400);
//    if (itemID == 83 || itemID == 82) printf("Exit:  QxTout = %f, StorexT_new = %f\n", QxTout, StorexT_new);
    //    if (itemID == 1145 || itemID == 524) printf("**** ID = %d, %d %d %d, Q_WTemp_new = %f, QxTnew = %f, Q_incoming = %f, Q = %f, RO_Vol = %f, waterStorage = %f, waterStorageChange = %f, L=%f, \n", itemID, MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), Q_WTemp_new, QxTnew, Q_incoming, Q, RO_Vol, waterStorage, waterStorageChange, MFModelGetLength(itemID));
//   if (itemID == 1145 || itemID == 524) printf("T_river = %f, T_runoff = %f, T_storage_new = %f, T_storage_old = %f\n", Q_WTemp_new, RO_WTemp, StorexT_new / (waterStorage * 86400), StorexT / ((waterStorage - waterStorageChange) * 86400));
//    if (itemID == 1145 || itemID == 524) printf("*** m = %d, d = %d, resCapacity = %f, waterStorage = %f, waterStorageChange = %f\n", MFDateGetCurrentMonth(), MFDateGetCurrentDay(), resCapacity, waterStorage, waterStorageChange);
//    if (itemID == 1145 || itemID == 524) printf("Q_incoming = %f, Q = %f, RO_vol = %f\n", Q_incoming, Q, RO_Vol);
//    if (itemID == 1145 || itemID == 524) printf("m = %d, d = %d, m3 degC: QxT = %f, StorexT = %f, QxT_input = %f\n", MFDateGetCurrentMonth(), MFDateGetCurrentDay(), QxT, StorexT, QxT_input);
//    if (itemID == 1145 || itemID == 524) printf("volume: Q_incoming = %f, waterStorage - change = %f, RO_Vol = %f\n", Q_incoming * 86400, waterStorage - waterStorageChange, RO_Vol * 86400);
//    if (itemID == 1145 || itemID == 524) printf("flux = %f, storage = %f, RO_WTemp = %f\n", QxT / (Q_incoming * 86400), StorexT / (waterStorage - waterStorageChange), RO_WTemp);
//    if (itemID == 5033) printf("END: itemID = %d, m = %d, d= %d, Q = %f, QxT_pt = %f, Q_WTemp_pt = %f, QxTout = %f, \n", itemID, MFDateGetCurrentMonth(), MFDateGetCurrentDay(), Q, QxT_postThermal, Q_WTemp_postThermal, QxTout);
//    if (itemID == 4704) printf("END: itemID = %d, m = %d, d= %d, Q = %f, QxT_pt = %f, Q_WTemp_pt = %f, QxTout = %f, \n", itemID, MFDateGetCurrentMonth(), MFDateGetCurrentDay(), Q, QxT_postThermal, Q_WTemp_postThermal, QxTout);


}

int MDWTempRiverRouteDef () {
   int optID = MFUnset, waterBalanceID;
   const char *optStr;
   const char *options [] = { "none", "calculate", (char *) NULL };
  
	if (_MDWTempRiverRouteID != MFUnset) return (_MDWTempRiverRouteID);

	MFDefEntering ("Route river temperature");
	
	if (((optStr = MFOptionGet (MDOptReservoirs))  == (char *) NULL) ||
       ((optID  = CMoptLookup ( options, optStr, true)) == CMfailed)) {
      CMmsgPrint(CMmsgUsrError,"Reservoir Option not specified! Option none or calculate");
		return CMfailed;
   }
   if (optID==1) {
      if ((( waterBalanceID             = MDWaterBalanceDef  ()) == CMfailed) ||
          ((_MDInResStorageChangeID     = MFVarGetID (MDVarReservoirStorageChange, "km3",     MFInput,  MFState, MFBoundary)) == CMfailed) ||	//RJS 071511
          ((_MDInResStorageID           = MFVarGetID (MDVarReservoirStorage,       "km3",     MFInput,  MFState, MFInitial))  == CMfailed) ||	//RJS 121311 changed from MFBoundary to MFInitial
          ((_MDInResCapacityID          = MFVarGetID (MDVarReservoirCapacity,      "km3",     MFInput,  MFState, MFBoundary)) == CMfailed))
         return (CMfailed);
    }
	//input
	if (
	   ((_MDInDischargeID            = MDDischargeDef     ()) == CMfailed) ||		//commented out 030113
       (( waterBalanceID             = MDWaterBalanceDef  ()) == CMfailed) ||
       ((_MDInSolarRadID             = MDSolarRadDef      ()) == CMfailed) ||
       ((_MDInWTempRiverID           = MDWTempRiverDef    ()) == CMfailed) ||
       ((_MDInRiverWidthID           = MDRiverWidthDef    ()) == CMfailed) ||
       ((_MDInRunoffVolumeID         = MDRunoffVolumeDef  ()) == CMfailed) ||
 //      ((_MDInWdl_QxTID	             = MDThermalInputsDef ()) == CMfailed) ||	//RJS 072011	// commented out 013112
 //      ((_MDInDischargeID            = MFVarGetID (MDVarDischarge,              "m3/s",       MFInput,  MFState, MFBoundary)) == CMfailed) ||	// RJS 030113
       ((_MDInDischargeIncomingID    = MFVarGetID (MDVarDischarge0,             "m3/s",       MFInput,  MFState, MFInitial)) == CMfailed) ||		// changed from flux to state 113012, changed from Boundary to Initial
       ((_MDInWindSpeedID            = MFVarGetID (MDVarWindSpeed,              "m/s",        MFInput,  MFState, MFBoundary)) == CMfailed) ||
       ((_MDInAirTemperatureID       = MFVarGetID (MDVarAirTemperature,         "degC",       MFInput,  MFState, MFBoundary)) == CMfailed) ||
       ((_MDInCloudCoverID           = MFVarGetID (MDVarCloudCover,             "%",          MFInput,  MFState, MFBoundary)) == CMfailed) ||
       ((_MDInRiverStorageChgID      = MFVarGetID (MDVarRiverStorageChg,        "m3/s",       MFInput,  MFState, MFBoundary)) == CMfailed) ||
       ((_MDInRiverStorageID         = MFVarGetID (MDVarRiverStorage,           "m3",         MFInput,  MFState, MFInitial))  == CMfailed) ||
       ((_MDInDingmanOnOffID         = MFVarGetID (MDVarDingmanOnOff,           "-",          MFInput,  MFState, MFInitial))  == CMfailed) ||
       ((_MDInSnowPackID             = MFVarGetID (MDVarSnowPack,               "mm",         MFInput,  MFState, MFBoundary)) == CMfailed) ||
//       ((_MDInWarmingTempID	       = MFVarGetID (MDVarWarmingTemp,		        "degC",	    MFInput,  MFState, MFBoundary)) == CMfailed) ||	//RJS 072011		//commented out 013112
//       ((_MDInThermalWdlID           = MFVarGetID (MDVarThermalWdl, 		        "-",          MFInput,  MFState, MFBoundary)) == CMfailed) ||	//RJS 072011	//commented out 013112
       // output
       ((_MDLocalIn_QxTID            = MFVarGetID (MDVarBgcLocalIn_QxT,         "m3*degC/d", MFOutput, MFFlux,  MFBoundary)) == CMfailed)   ||
       ((_MDRemoval_QxTID            = MFVarGetID (MDVarRemoval_QxT,            "m3*degC/d", MFOutput, MFFlux,  MFBoundary)) == CMfailed)   ||
       ((_MDFlux_QxTID               = MFVarGetID (MDVarFlux_QxT,               "m3*degC/d", MFRoute,  MFFlux,  MFBoundary)) == CMfailed)   ||
       ((_MDStorage_QxTID            = MFVarGetID (MDVarStorage_QxT,            "m3*degC",   MFOutput, MFState, MFInitial))  == CMfailed)   ||
       ((_MDDeltaStorage_QxTID       = MFVarGetID (MDVarDeltaStorage_QxT,       "m3*degC/d", MFOutput, MFFlux,  MFBoundary)) == CMfailed)   ||
       ((_MDWTemp_QxTID              = MFVarGetID (MDVarWTemp_QxT,              "degC",      MFOutput, MFState, MFBoundary)) == CMfailed)   ||
       ((_MDWTempDeltaT_QxTID        = MFVarGetID (MDVarWTempDeltaT_QxT,        "degC",      MFOutput, MFState, MFBoundary)) == CMfailed)   ||
       //output mixing
       ((_MDFluxMixing_QxTID         = MFVarGetID (MDVarFluxMixing_QxT,         "m3*degC/d", MFRoute,  MFFlux,  MFBoundary)) == CMfailed)   ||
       ((_MDStorageMixing_QxTID      = MFVarGetID (MDVarStorageMixing_QxT,      "m3*degC",   MFOutput, MFState, MFInitial))  == CMfailed)   ||
       ((_MDDeltaStorageMixing_QxTID = MFVarGetID (MDVarDeltaStorageMixing_QxT, "m3*degC/d", MFOutput, MFFlux,  MFBoundary)) == CMfailed)   ||
       ((_MDWTempMixing_QxTID        = MFVarGetID (MDVarWTempMixing_QxT,        "degC",      MFOutput, MFState, MFBoundary)) == CMfailed)   ||
       ((_MDDeltaTID                 = MFVarGetID (MDVarDeltaT,                 "degC",      MFOutput, MFState, MFBoundary)) == CMfailed)   ||	// RJS 030613
       (MFModelAddFunction (_MDWTempRiverRoute) == CMfailed)) return (CMfailed);
      
	   MFDefLeaving ("Route river temperature");
	   return (_MDWTemp_QxTID);
}
