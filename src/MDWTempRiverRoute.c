/******************************************************************************
GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

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
static int _MDInWdl_QxTID              = MFUnset;	//RJS 082011
static int _MDInThermalWdlID           = MFUnset;	//RJS 082011
static int _MDInWarmingTempID          = MFUnset;	//RJS 082011
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
     
     float resCapacity;		//RJS 071511	Reservoir capacity [km3]

     float QxT_postThermal;			//RJS 081311
     float QxT_mix_postThermal;		//RJS 081311
     float Q_WTemp_postThermal;		//RJS 081311
     float Q_WTemp_mix_postThermal;	//RJS 081311
     float warmingTemp;				//RJS 081311
     float wdl_QxT;					//RJS 081311
     float thermal_wdl;				//RJS 081311

     float StorexT_postThermal;				//RJS 081711
     float DeltaStorexT_postThermal;		//RJS 081711
     float StorexT_mix_postThermal;			//RJS 081711
     float DeltaStorexT_mix_postThermal;	//RJS 081711
     float deltaT_postThermal;				//RJS 081711

     day = MFDateGetCurrentDay();
     month = MFDateGetCurrentMonth();

   	 Q                     = MFVarGetFloat (_MDInDischargeID,         itemID, 0.0);
   	 Q_incoming            = MFVarGetFloat (_MDInDischargeIncomingID, itemID, 0.0); // already includes local runoff
     RO_Vol                = MFVarGetFloat (_MDInRunoffVolumeID,      itemID, 0.0);
   	 RO_WTemp              = MFVarGetFloat (_MDInWTempRiverID,        itemID, 0.0);
     SnowPack              = MFVarGetFloat (_MDInSnowPackID,          itemID, 0.0);
 	
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
     warmingTemp	   = MFVarGetFloat (_MDInWarmingTempID,    itemID, 0.0);	//RJS 072011
     wdl_QxT		   = MFVarGetFloat (_MDInWdl_QxTID,        itemID, 0.0);	//RJS 072011
     thermal_wdl	   = MFVarGetFloat (_MDInThermalWdlID, 	   itemID, 0.0)* 1000000 / 365 / 86400;	//RJS 072011

    // if (itemID == 5132){
    //        	printf("Stop itemID %d day %d \n", itemID, MFDateGetCurrentDay());
    //   	 }
     //TODO: combine with reservoir check above - also make reservoir hydraulics generally accessible
     //TODO: add effect of water withdrawals

     if(Q < 0.0)  Q = 0.0;							//RJS 120409
     if(Q_incoming < 0.0) Q_incoming = 0.0;			//RJS 120409
//     if(RO_Vol < 0.0) RO_Vol = 0.0;					//RJS 071511

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
    		 Q_WTemp = QxTnew / ((Q_incoming) * 86400 + (waterStorage - waterStorageChange)); 			//RJS 071511					//degC
    		 Q_WTemp_mix = QxTnew_mix / ((Q_incoming) * 86400 + (waterStorage - waterStorageChange));	//RJS 071511					//degC
 //   		 if (itemID == 25014) printf("Q_incoming > 0.000001\n");
    	 }

    	 else {
    		 if (waterStorage > 0) {
    			 Q_WTemp	 = StorexT / waterStorage;		// RJS 071511	//degC
    			 Q_WTemp_mix = StorexT_mix / waterStorage;	// RJS 071511	//degC
 //   			 if (itemID == 25014) printf("waterStorage > 0\n");
    		 }
			 else {
				 Q_WTemp 	 = 0.0;			//RJS 071511
				 Q_WTemp_mix = 0.0;			//RJS 071511
 //				 if (itemID == 25014) printf("else\n");
			 }
    	 }

    	 Q_WTemp_new = Q_WTemp;														//RJS 071511

    	 StorexT_new      = waterStorage * Q_WTemp_new; 							//RJS 071511	//m3*degC
    	 DeltaStorexT     = StorexT_new - StorexT; 									//RJS 071511
    	 QxTout           = Q * 86400.0 * Q_WTemp_new ; 							//RJS 071511	//m3*degC/d
    	 QxTRemoval       = QxTnew - (StorexT_new + QxTout); 						//RJS 071511	//m3*degC/d
    	 StorexT_new_mix  = waterStorage * Q_WTemp_mix; 							//RJS 071511	//m3*degC
    	 DeltaStorexT_mix = StorexT_new_mix - StorexT_mix;							//RJS 071511
    	 QxTout_mix       = Q * 86400.0 * Q_WTemp_mix; 								//RJS 071511	//m3*degC/s

  //      if (itemID == 5033) printf("m = %d, d = %d, itemID = %d, QxTout = %f, QxTout_mix = %f, Q = %f, Q_WTemp_new = %f\n", MFDateGetCurrentMonth (), MFDateGetCurrentDay (), itemID, QxTout, QxTout_mix, Q, Q_WTemp_new);
    //    if (itemID == 4704) printf("m = %d, d = %d, itemID = %d, QxTout = %f, QxTout_mix = %f, Q = %f, Q_WTemp_new = %f\n", MFDateGetCurrentMonth (), MFDateGetCurrentDay (), itemID, QxTout, QxTout_mix, Q, Q_WTemp_new);


    	 //New experimental
    	          	QxT_postThermal          = thermal_wdl > Q ? 86400 * Q * (Q_WTemp_new + warmingTemp) : 86400 * ((thermal_wdl * (Q_WTemp_new + warmingTemp)) + ((Q - thermal_wdl) * Q_WTemp_new));
         		QxT_mix_postThermal      = thermal_wdl > Q ? 86400 * Q * (Q_WTemp_mix + warmingTemp) : 86400 * ((thermal_wdl * (Q_WTemp_mix + warmingTemp)) + ((Q - thermal_wdl) * Q_WTemp_mix));
         		Q_WTemp_postThermal      = Q > 0.000001 ? QxT_postThermal / (Q * 86400) : 0.0;
    	          	Q_WTemp_mix_postThermal  = Q > 0.000001 ? QxT_mix_postThermal / (Q * 86400) : 0.0;
    	          	StorexT_postThermal		 = waterStorage * Q_WTemp_postThermal;
    	          	DeltaStorexT_postThermal = StorexT_postThermal - StorexT;
    	          	StorexT_mix_postThermal  = waterStorage * Q_WTemp_mix_postThermal;
    	          	DeltaStorexT_mix_postThermal = StorexT_mix_postThermal - StorexT_mix;
    	          	deltaT_postThermal = Q_WTemp_postThermal - Q_WTemp;

//	if (itemID == 5033) printf("QxT_pt = %f, Q_WTemp_pt = %f\n", QxT_postThermal, Q_WTemp_postThermal);
	//if (itemID == 4704) printf("QxT_pt = %f, Q_WTemp_pt = %f\n", QxT_postThermal, Q_WTemp_postThermal);
 

    	          //end

    	          MFVarSetFloat(_MDLocalIn_QxTID, itemID, QxT_input);
    	 //         MFVarSetFloat(_MDRemoval_QxTID, itemID, QxTRemoval);
    	 //         MFVarSetFloat(_MDFlux_QxTID, itemID, QxTout);
    	          MFVarSetFloat(_MDFlux_QxTID, itemID, QxT_postThermal);					//RJS new
    	 //         MFVarSetFloat(_MDStorage_QxTID, itemID, StorexT_new);
    	          MFVarSetFloat(_MDStorage_QxTID, itemID, StorexT_postThermal);			//RJS new
    	 //         MFVarSetFloat(_MDDeltaStorage_QxTID, itemID, DeltaStorexT);
    	          MFVarSetFloat(_MDDeltaStorage_QxTID, itemID, DeltaStorexT_postThermal); //RJS new
    	 //         MFVarSetFloat(_MDWTemp_QxTID, itemID, Q_WTemp_new);
    	          MFVarSetFloat(_MDWTemp_QxTID, itemID, Q_WTemp_postThermal);			//RJS new
    	 //         MFVarSetFloat(_MDWTempDeltaT_QxTID, itemID, deltaT);
    	 //         MFVarSetFloat(_MDWTempDeltaT_QxTID, itemID, deltaT_postThermal);		//RJS new
    	 //         MFVarSetFloat(_MDFluxMixing_QxTID, itemID, QxTout_mix);
    	          MFVarSetFloat(_MDFluxMixing_QxTID, itemID, QxT_mix_postThermal);		//RJS new
    	 //         MFVarSetFloat(_MDStorageMixing_QxTID, itemID, StorexT_new_mix);
    	          MFVarSetFloat(_MDStorageMixing_QxTID, itemID, StorexT_mix_postThermal);	//RJS new
    	 //         MFVarSetFloat(_MDDeltaStorageMixing_QxTID, itemID, DeltaStorexT_mix);
    	          MFVarSetFloat(_MDDeltaStorageMixing_QxTID, itemID, DeltaStorexT_mix_postThermal);	//RJS new
    	 //         MFVarSetFloat(_MDWTempMixing_QxTID, itemID, Q_WTemp_mix);
    	          MFVarSetFloat(_MDWTempMixing_QxTID, itemID, Q_WTemp_mix_postThermal);				//RJS new

//    	 MFVarSetFloat(_MDLocalIn_QxTID, itemID, QxT_input);						//RJS 071511
////	       MFVarSetFloat(_MDRemoval_QxTID, itemID, QxTRemoval);						//RJS 071511
//    	 MFVarSetFloat(_MDFlux_QxTID, itemID, QxTout);								//RJS 071511
//   	 MFVarSetFloat(_MDStorage_QxTID, itemID, StorexT_new);						//RJS 071511
//    	 MFVarSetFloat(_MDDeltaStorage_QxTID, itemID, DeltaStorexT);				//RJS 071511
//   	 MFVarSetFloat(_MDWTemp_QxTID, itemID, Q_WTemp_new);						//RJS 071511
////     	   MFVarSetFloat(_MDWTempDeltaT_QxTID, itemID, deltaT);						//RJS 071511
//    	 MFVarSetFloat(_MDFluxMixing_QxTID, itemID, QxTout_mix);					//RJS 071511
//    	 MFVarSetFloat(_MDStorageMixing_QxTID, itemID, StorexT_new_mix);			//RJS 071511
//    	 MFVarSetFloat(_MDDeltaStorageMixing_QxTID, itemID, DeltaStorexT_mix);		//RJS 071511
//    	 MFVarSetFloat(_MDWTempMixing_QxTID, itemID, Q_WTemp_mix);					//RJS 071511

     }

     else{
    	 ReservoirArea = 0.0;
    	 ReservoirVelocity = 0.0;
    	 ReservoirDepth = 0.0;
 //    }								 = %f							//RJS commented out 071511
    
     //TODO: RO_Vol has been set to never be less than 0 in MDWRunoff
     QxT_input = RO_Vol * RO_WTemp * 86400.0; //m3*degC/d 



     	//if (itemID == 188 && month == 5 && day == 1){
        //  		printf("Stop: Q %f RO_Vol %f QxT %f QxT_input %f \n", Q, RO_Vol, QxT, QxT_input);
        //}
     //note: calculation for input concentration is changed from previous iterations 
     // to use incoming Q.  Also use WaterStorage from previous time step/
     // TODO: Need to include a variable that accounts for losses due to discharge disappearing (Drying)
     // TODO:  Make all these changes for other bgc flux models
     // Q_incoming includes local runoff!!!
     if((Q_incoming) > 0.000001) {			 //do not include water storage in this check - will screw up mixing estimates
         QxTnew = QxT + QxT_input + StorexT; //m3*degC/d
   	     QxTnew_mix = QxT_mix + QxT_input + StorexT_mix;
        
   	     Q_WTemp = QxTnew / ((Q_incoming) * 86400 + (waterStorage - waterStorageChange)); //degC
         Q_WTemp_mix = QxTnew_mix / ((Q_incoming) * 86400 + (waterStorage - waterStorageChange)); //degC

        ///Temperature Processing using Dingman 1972 
         if (cloudCover < 95){  // clear skies, assume cloud cover < 95% convertcalories / cm2 /d to kJ/m2/d
            HeatLoss_int = (105 + 23 *  windSpeed) * 4.1868 / 1000 * 100 * 100; // kJ/m2/d
            HeatLoss_slope = (35 + 4.2 * windSpeed) * 4.1868 / 1000 * 100 * 100;// kJ/m2/d/degC

         } else{                // cloudy skies, assume cloud cover > 95%
        	 HeatLoss_int = (-73 + 9.1 *  windSpeed) * 4.1868 / 1000 * 100 * 100;
        	 HeatLoss_slope = (37 + 4.6 * windSpeed) * 4.1868 / 1000 * 100 * 100;
         }
         Tequil = Tair + (((solarRad * 1000) - HeatLoss_int) / HeatLoss_slope); //solar rad converted from MJ to kJ/m2/d
         // use exponential form
         //TODO channelWidth can equal 0 when waterStorage > 0.0, so need to check here
         // Apply model only to large enough discharges, otherwise assume temperature equils equilibrium
        // if (channelWidth > 0 && Q > 0.001){
         if (channelWidth > 0){
	 Q_WTemp_new = MDMaximum(0, (((Q_WTemp - Tequil) * exp((-HeatLoss_slope * MFModelGetLength(itemID)) / (999.73 * 4.1922 * (Q * 86400.0 / channelWidth)))) + Tequil));
         }
         else {
        	 Q_WTemp_new = MDMaximum(0, Tequil);
         }
         
         //if cell has reservoir, assume reservoir exchange dominates
         //if(ResWaterStorage > 0){
         //    Q_WTemp_new = MDMaximum(0, (((Q_WTemp - Tequil) * exp((-HeatLoss_slope * ReservoirArea) / (999.73 * 4.1922 * (Q * 86400.0)))) + Tequil));
         //}

         
         deltaT = Q_WTemp_new - Q_WTemp;
         //if (Q_WTemp_new > 50){
       //  if (Q_WTemp_mix > 30){
       //  printf("Toggle");
       //  printf("Stop WaterTemp > 50 itemID %d XCoord %f YCoord %f month %d day %d Q %f Q_incoming %f waterStorage %f "
       //     		"RO_Vol %f RO_WTemp %f QxT %f QxT_mix %f StorexT %f Storext_mix %f QxT_input %f QxTnew %f Q_WTemp %f Q_WTemp_mix %f Q_WTemp_new %f Tequil %f \n", 
       //     		 itemID, MFModelGetXCoord(itemID),MFModelGetYCoord(itemID), month, day, Q, Q_incoming, waterStorage, 
       //     		 RO_Vol, RO_WTemp, QxT, QxT_mix, StorexT, StorexT_mix, QxT_input, QxTnew, Q_WTemp, Q_WTemp_mix, Q_WTemp_new, Tequil);
       //  }
   	     StorexT_new  = waterStorage * Q_WTemp_new; //m3*degC
         DeltaStorexT = StorexT_new - StorexT; //
         QxTout       = Q * 86400.0 * Q_WTemp_new ; //m3*degC/d
         QxTRemoval   = QxTnew - (StorexT_new + QxTout); //m3*degC/d
         StorexT_new_mix  = waterStorage * Q_WTemp_mix; //m3*degC
         DeltaStorexT_mix = StorexT_new_mix - StorexT_mix;
         QxTout_mix       = Q * 86400.0 * Q_WTemp_mix; //m3*degC/s

	

 //        if (itemID == 5033) printf("m = %d, d = %d, itemID = %d, QxTout = %f, QxTout_mix = %f, Q = %f, Q_WTemp_new = %f\n", MFDateGetCurrentMonth (), MFDateGetCurrentDay (), itemID, QxTout, QxTout_mix, Q, Q_WTemp_new);
   //      if (itemID == 4704) printf("m = %d, d = %d, itemID = %d, QxTout = %f, QxTout_mix = %f, Q = %f, Q_WTemp_new = %f\n", MFDateGetCurrentMonth (), MFDateGetCurrentDay (), itemID, QxTout, QxTout_mix, Q, Q_WTemp_new);

//        if (QxT_input > 1000) printf("m = %d, d = %d, itemID = %d, QxT_input = %f, RO_Vol = %f, RO_WTemp = %f, QxT = %f, QxT_mix = %f\n, QxTout = %f, QxTout_mix = %f, Q = %f\n", MFDateGetCurrentMonth (), MFDateGetCurrentDay (), itemID, QxT_input, RO_Vol, RO_WTemp, QxT, QxT_mix, QxTout, QxTout_mix, Q);


 //       if (Q_WTemp_new > 50) printf("m = %d, d = %d, itemID = %d, QxT_input = %f, RO_Vol = %f, RO_WTemp = %f\n channelWidth = %f, Q_WTemp = %f, Q_WTemp_new = %f, Q_WTemp_mix = %f, Tair = %f, Q = %f, Q_incoming = %f\n", MFDateGetCurrentMonth (), MFDateGetCurrentDay (), itemID, QxT_input, RO_Vol, RO_WTemp, channelWidth, Q_WTemp, Q_WTemp_new, Q_WTemp_mix, Tair, Q, Q_incoming);


         //New experimental
         	QxT_postThermal          = thermal_wdl > Q ? 86400 * Q * (Q_WTemp_new + warmingTemp) : 86400 * ((thermal_wdl * (Q_WTemp_new + warmingTemp)) + ((Q - thermal_wdl) * Q_WTemp_new));
         	QxT_mix_postThermal      = thermal_wdl > Q ? 86400 * Q * (Q_WTemp_mix + warmingTemp) : 86400 * ((thermal_wdl * (Q_WTemp_mix + warmingTemp)) + ((Q - thermal_wdl) * Q_WTemp_mix));
         	Q_WTemp_postThermal      = Q > 0.000001 ? QxT_postThermal / (Q * 86400) : 0.0;
    	    Q_WTemp_mix_postThermal  = Q > 0.000001 ? QxT_mix_postThermal / (Q * 86400) : 0.0;
         	StorexT_postThermal		 = waterStorage * Q_WTemp_postThermal;
         	DeltaStorexT_postThermal = StorexT_postThermal - StorexT;
         	StorexT_mix_postThermal  = waterStorage * Q_WTemp_mix_postThermal;
         	DeltaStorexT_mix_postThermal = StorexT_mix_postThermal - StorexT_mix;
         	deltaT_postThermal = Q_WTemp_postThermal - Q_WTemp;

//	if (itemID == 5033) printf("QxT_pt = %f, Q_WTemp_pt = %f\n", QxT_postThermal, Q_WTemp_postThermal);
	//if (itemID == 4704) printf("QxT_pt = %f, Q_WTemp_pt = %f\n", QxT_postThermal, Q_WTemp_postThermal);
 

         //end

         MFVarSetFloat(_MDLocalIn_QxTID, itemID, QxT_input);
         MFVarSetFloat(_MDRemoval_QxTID, itemID, QxTRemoval);
//         MFVarSetFloat(_MDFlux_QxTID, itemID, QxTout);
         MFVarSetFloat(_MDFlux_QxTID, itemID, QxT_postThermal);					//RJS new
//         MFVarSetFloat(_MDStorage_QxTID, itemID, StorexT_new);
         MFVarSetFloat(_MDStorage_QxTID, itemID, StorexT_postThermal);			//RJS new
//         MFVarSetFloat(_MDDeltaStorage_QxTID, itemID, DeltaStorexT);
         MFVarSetFloat(_MDDeltaStorage_QxTID, itemID, DeltaStorexT_postThermal); //RJS new
//         MFVarSetFloat(_MDWTemp_QxTID, itemID, Q_WTemp_new);
         MFVarSetFloat(_MDWTemp_QxTID, itemID, Q_WTemp_postThermal);			//RJS new
//         MFVarSetFloat(_MDWTempDeltaT_QxTID, itemID, deltaT);
         MFVarSetFloat(_MDWTempDeltaT_QxTID, itemID, deltaT_postThermal);		//RJS new
//         MFVarSetFloat(_MDFluxMixing_QxTID, itemID, QxTout_mix);
         MFVarSetFloat(_MDFluxMixing_QxTID, itemID, QxT_mix_postThermal);		//RJS new
//         MFVarSetFloat(_MDStorageMixing_QxTID, itemID, StorexT_new_mix);
         MFVarSetFloat(_MDStorageMixing_QxTID, itemID, StorexT_mix_postThermal);	//RJS new
//         MFVarSetFloat(_MDDeltaStorageMixing_QxTID, itemID, DeltaStorexT_mix);
         MFVarSetFloat(_MDDeltaStorageMixing_QxTID, itemID, DeltaStorexT_mix_postThermal);	//RJS new
//         MFVarSetFloat(_MDWTempMixing_QxTID, itemID, Q_WTemp_mix);
         MFVarSetFloat(_MDWTempMixing_QxTID, itemID, Q_WTemp_mix_postThermal);				//RJS new
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
    //if (mbmix > 100000){
  	//printf("mass balance = mb %f mbmix %f \n", mb, mbmix);
    //}



//    if (itemID == 25014) printf("Q_WTemp_new = %f, QxTnew = %f, Q_incoming = %f, Q_m3 = %f, waterStorage = %f, waterStorageChange = %f, \n", Q_WTemp_new, QxTnew, Q_incoming, Q_incoming * 86400, waterStorage, waterStorageChange);
//    if (itemID == 25014) printf("T_river = %f, T_runoff = %f, T_storage = %f\n", QxT/Q_incoming, RO_WTemp, StorexT/(waterStorage - waterStorageChange));
//     if (itemID == 25014) printf("*** m = %d, d = %d, resCapacity = %f, waterStorage = %f, waterStorageChange = %f\n", MFDateGetCurrentMonth(), MFDateGetCurrentDay(), resCapacity, waterStorage, waterStorageChange);
 //    if (itemID == 25014) printf("Q_incoming = %f, Q = %f, RO_vol = %f\n", Q_incoming, Q, RO_Vol);
 //   if (itemID == 25014) printf("m = %d, d = %d, m3 degC: QxT = %f, StorexT = %f, QxT_input = %f\n", MFDateGetCurrentMonth(), MFDateGetCurrentDay(), QxT, StorexT, QxT_input);
//     if (itemID == 25014) printf("volume: Q_incoming = %f, waterStorage - change = %f, RO_Vol = %f\n", Q_incoming * 86400, waterStorage - waterStorageChange, RO_Vol * 86400);
//     if (itemID == 25014) printf("flux = %f, storage = %f, RO_WTemp = %f\n", QxT / (Q_incoming * 86400), StorexT / (waterStorage - waterStorageChange), RO_WTemp);
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
	if (((_MDInDischargeID            = MDDischargeDef     ()) == CMfailed) ||
       (( waterBalanceID             = MDWaterBalanceDef  ()) == CMfailed) ||
       ((_MDInSolarRadID             = MDSolarRadDef      ()) == CMfailed) ||
       ((_MDInWTempRiverID           = MDWTempRiverDef    ()) == CMfailed) ||
       ((_MDInRiverWidthID           = MDRiverWidthDef    ()) == CMfailed) ||
       ((_MDInRunoffVolumeID         = MDRunoffVolumeDef  ()) == CMfailed) ||
       ((_MDInWdl_QxTID	             = MDThermalInputsDef ()) == CMfailed) ||	//RJS 072011
       ((_MDInDischargeIncomingID    = MFVarGetID (MDVarDischarge0,             "m3/s",       MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
       ((_MDInWindSpeedID            = MFVarGetID (MDVarWindSpeed,              "m/s",        MFInput,  MFState, MFBoundary)) == CMfailed) ||
       ((_MDInAirTemperatureID       = MFVarGetID (MDVarAirTemperature,         "degC",       MFInput,  MFState, MFBoundary)) == CMfailed) ||
       ((_MDInCloudCoverID           = MFVarGetID (MDVarCloudCover,             "%",          MFInput,  MFState, MFBoundary)) == CMfailed) ||
       ((_MDInRiverStorageChgID      = MFVarGetID (MDVarRiverStorageChg,        "m3/s",       MFInput,  MFState, MFBoundary)) == CMfailed) ||
       ((_MDInRiverStorageID         = MFVarGetID (MDVarRiverStorage,           "m3",         MFInput,  MFState, MFInitial))  == CMfailed) ||
       ((_MDInSnowPackID             = MFVarGetID (MDVarSnowPack,               "mm",         MFInput,  MFState, MFBoundary)) == CMfailed) ||
       ((_MDInWarmingTempID	       = MFVarGetID (MDVarWarmingTemp,		        "degC",	    MFInput,  MFState, MFBoundary)) == CMfailed) ||	//RJS 072011
       ((_MDInThermalWdlID           = MFVarGetID (MDVarThermalWdl, 		        "-",          MFInput,  MFState, MFBoundary)) == CMfailed) ||	//RJS 072011
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
       (MFModelAddFunction (_MDWTempRiverRoute) == CMfailed)) return (CMfailed);
      
	   MFDefLeaving ("Route river temperature");
	   return (_MDWTemp_QxTID);
}
