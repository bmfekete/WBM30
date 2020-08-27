/******************************************************************************
GHAAS Water Balance/Transport Model V2.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDWTempRiverRoute.c

wil.wollheim@unh.edu

amiara@ccny.cuny.edu - updated river temperature calculations as of Sep 2016

Route temperature through river network

*******************************************************************************/

#include <string.h>
#include <math.h>
#include <MF.h>
#include <MD.h>

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
static int _MDEquil_Temp	       = MFUnset;

static int _MDInRelativeHumidityID       = MFUnset; // FOR NEW TEMP MODULE


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
//     warmingTemp	   = MFVarGetFloat (_MDInWarmingTempID,    itemID, 0.0);	//RJS 072011						//commented out 013112
//     wdl_QxT		   = MFVarGetFloat (_MDInWdl_QxTID,        itemID, 0.0);	//RJS 072011						//commented out 013112
//     thermal_wdl	   = MFVarGetFloat (_MDInThermalWdlID, 	   itemID, 0.0)* 1000000 / 365 / 86400;	//RJS 072011	//commented out 013112

     if(Q < 0.0)  Q = 0.0;							//RJS 120409
     if(Q_incoming < 0.0) Q_incoming = 0.0;			//RJS 120409

     if(resCapacity < 0.0){
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
    	 }

    	 else {
    		 if (waterStorage > 0) {
    			 Q_WTemp	 = StorexT / waterStorage;		// RJS 071511	//degC
    			 Q_WTemp_mix = StorexT_mix / waterStorage;	// RJS 071511	//degC
    		 }
			 else {
				 Q_WTemp 	 = 0.0;			//RJS 071511
				 Q_WTemp_mix = 0.0;			//RJS 071511
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


    	          //end

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

     }

     else{
    	 ReservoirArea = 0.0;
    	 ReservoirVelocity = 0.0;
    	 ReservoirDepth = 0.0;
 //    }								 = %f							//RJS commented out 071511
    
     //TODO: RO_Vol has been set to never be less than 0 in MDWRunoff
     QxT_input = RO_Vol * RO_WTemp * 86400.0; //m3*degC/d 

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


////////// NEW EQUILIBRIUM TEMP MODEL ///// Edinger et al. 1974: Heat Exchange and Transport in the Environment /////

float e2;
float es2;
float dew_point;
float relative_h;
float wind_f;
float Tm;
float beta;
float kay;
float solar_KJ;
float equil2;
float initial_riverT;
float RivTemp;
int x;

initial_riverT = Q_WTemp;
//if (Q > 2.0){
//printf("FIRST FIRST FIRST \n");
//printf("initial_riverT = %f \n", initial_riverT);
//}

relative_h = MFVarGetFloat (_MDInRelativeHumidityID,         itemID, 0.0);
es2 = 6.112 * exp((17.67*Tair)/(243.5+Tair));
e2 = relative_h * es2 / 100;
dew_point =(243.5*log(e2/6.112))/(17.67-log(e2/6.112)); //dew point temp (C)
dew_point = (e2 <= 0) ? Tair : dew_point;
wind_f = 9.2+(0.46*pow(windSpeed,2)); // wind function
solar_KJ = solarRad * 1000; // solar radiation in KJ/m2/day

for (x = 0; x < 4; x++) {
	Tm = (dew_point + initial_riverT) / 2; // mean of rivertemp initial and dew point
	beta = 0.35+(0.015*Tm)+(0.0012*pow(Tm,2)); //beta
	kay = (4.5+(0.05*initial_riverT)+(beta*wind_f)+(0.47*wind_f)) * (3600*24) / 1000; // K in daily KJ
	equil2 = dew_point + (solar_KJ / kay);
	initial_riverT = equil2;
}

RivTemp = equil2 + (Q_WTemp - equil2) * exp(-kay * MFModelGetLength(itemID)/(4181.3*(Q * 86400.0 / channelWidth)));

if (channelWidth == 0) RivTemp = initial_riverT;

/// Resetting outgoing temperature:
Q_WTemp_new = MDMaximum(0, RivTemp);
MFVarSetFloat(_MDEquil_Temp, itemID, equil2);
//Q_WTemp_new = (MFModelGetLength(itemID) == 0.0) ? initial_riverT : Q_WTemp_new;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//if ((Q_WTemp_new > Tequil) && (Q > 1.0) && (Q_WTemp_new > 20) && (QxT_input > (QxT*1.1)) && (QxT > 0)) {
//if ((Q_WTemp_new > 60) && (Q > 2.0)){
if (isnan(Q_WTemp_new)){
//if( (equil2 > 20) && (Q > 0.0) && (MFModelGetLength(itemID) > 0)){
	printf(" NEW NEW NEW NEW NEW NEW \n");
	printf("Q_WTemp = %f, Tequil = %f, HeatLoss_slope = %f, length = %f, width = %f, cloudCover = %f, Q_WTemp_new = %f \n", Q_WTemp, Tequil, HeatLoss_slope, MFModelGetLength(itemID), channelWidth, cloudCover, Q_WTemp_new);
	printf("Tair = %f, HeatLoss_int = %f, windSpeed = %f, solarRad = %f, Q = %f \n", Tair, HeatLoss_int, windSpeed, solarRad, Q);
	printf("QxT_input = %f, RO_Vol =% f, RO_WTemp = %f, QxT_mix = %f, StorexT_mix = %f, QxT = %f, StorexT = %f \n", QxT_input, RO_Vol, RO_WTemp, QxT_mix, StorexT_mix, QxT, StorexT);
	printf("Tair = %f, e2 = %f, es2 = %f, dew_point = %f, relative_h = %f \n", Tair, e2, es2, dew_point, relative_h);
	printf("wind_f = %f, solar_KJ = %f, solarRad = %f, windSpeed = %f \n", wind_f, solar_KJ, solarRad, windSpeed);
	printf("Tm = %f, initial_riverT = %f, beta = %f, kay = %f, equil2 = %f \n", Tm, initial_riverT, beta, kay, equil2);
	printf("RivTemp = %f, Q_WTemp_new = %f \n", RivTemp, Q_WTemp_new);
	printf("ResCap = %f", resCapacity);
}


         deltaT = Q_WTemp_new - Q_WTemp;
 	 StorexT_new  = waterStorage * Q_WTemp_new; //m3*degC
         DeltaStorexT = StorexT_new - StorexT; //
         QxTout       = Q * 86400.0 * Q_WTemp_new ; //m3*degC/d
         QxTRemoval   = QxTnew - (StorexT_new + QxTout); //m3*degC/d
         StorexT_new_mix  = waterStorage * Q_WTemp_mix; //m3*degC
         DeltaStorexT_mix = StorexT_new_mix - StorexT_mix;
         QxTout_mix       = Q * 86400.0 * Q_WTemp_mix; //m3*degC/s


         //end

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
       ((_MDInRelativeHumidityID     = MDRelativeHumidityDef ()) == CMfailed) ||
       ((_MDInRiverWidthID           = MDRiverWidthDef    ()) == CMfailed) ||
       ((_MDInRunoffVolumeID         = MDRunoffVolumeDef  ()) == CMfailed) ||
       ((_MDInCloudCoverID           = MDCloudCoverDef ()) == CMfailed) ||
 //      ((_MDInWdl_QxTID	             = MDThermalInputsDef ()) == CMfailed) ||	//RJS 072011	// commented out 013112
       ((_MDInDischargeIncomingID    = MFVarGetID (MDVarDischarge0,             "m3/s",       MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
       ((_MDInWindSpeedID            = MFVarGetID (MDVarWindSpeed,              "m/s",        MFInput,  MFState, MFBoundary)) == CMfailed) ||
       ((_MDInAirTemperatureID       = MFVarGetID (MDVarAirTemperature,         "degC",       MFInput,  MFState, MFBoundary)) == CMfailed) ||
//       ((_MDInRelativeHumidityID       = MFVarGetID (MDVarRelativeHumidity,         "%",      MFInput,  MFState, MFBoundary)) == CMfailed) ||
       ((_MDInRiverStorageChgID      = MFVarGetID (MDVarRiverStorageChg,        "m3/s",       MFInput,  MFState, MFBoundary)) == CMfailed) ||
       ((_MDInRiverStorageID         = MFVarGetID (MDVarRiverStorage,           "m3",         MFInput,  MFState, MFInitial))  == CMfailed) ||
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
       ((_MDEquil_Temp   	     = MFVarGetID (MDVarEquil_Temp,        	"degC",      MFOutput, MFState, MFBoundary)) == CMfailed)   ||
       //output mixing
       ((_MDFluxMixing_QxTID         = MFVarGetID (MDVarFluxMixing_QxT,         "m3*degC/d", MFRoute,  MFFlux,  MFBoundary)) == CMfailed)   ||
       ((_MDStorageMixing_QxTID      = MFVarGetID (MDVarStorageMixing_QxT,      "m3*degC",   MFOutput, MFState, MFInitial))  == CMfailed)   ||
       ((_MDDeltaStorageMixing_QxTID = MFVarGetID (MDVarDeltaStorageMixing_QxT, "m3*degC/d", MFOutput, MFFlux,  MFBoundary)) == CMfailed)   ||
       ((_MDWTempMixing_QxTID        = MFVarGetID (MDVarWTempMixing_QxT,        "degC",      MFOutput, MFState, MFBoundary)) == CMfailed)   ||
       (MFModelAddFunction (_MDWTempRiverRoute) == CMfailed)) return (CMfailed);
      
	   MFDefLeaving ("Route river temperature");
	   return (_MDWTemp_QxTID);
}
