/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDWRiverGPP.c

wil.wollheim@unh.edu

Calculate GPP in rivers

*******************************************************************************/

#include <string.h>
#include <math.h>
#include <MF.h>
#include <MD.h>

// Model
static int _MDRiverGPPID               = MFUnset;

// Input
static int _MDInRiverLightID           = MFUnset;
static int _MDInDischargeID            = MFUnset;
static int _MDInRiverWidthID           = MFUnset;
static int _MDInPAR2BottomID           = MFUnset;
static int _MDInResStorageID           = MFUnset;
static int _MDInResStorageChangeID     = MFUnset;

// Output
static int _MDBenthicAlgaeCHLID        = MFUnset;
static int _MDBenthicAlgaeCID          = MFUnset;
static int _MDBenthicGPPID             = MFUnset;
static int _MDBenthicRaID              = MFUnset;
static int _MDBenthicNPPID             = MFUnset;
static int _MDBenthicMortalityID       = MFUnset;
static int _MDBenthicAlgaeC_REACHID    = MFUnset;
static int _MDBenthicGPP_REACHID       = MFUnset;
static int _MDBenthicRa_REACHID        = MFUnset;
static int _MDBenthicNPP_REACHID       = MFUnset;
static int _MDBenthicMortality_REACHID = MFUnset;

static void _MDRiverGPP (int itemID) {
     float channelWidth;
   	 //GPP drivers/parameters
   	 float par2bottom; //MJ / m2 /d
   	 float DIN = 1; //mg/l
   	 float DIP = 0.05; //mg/l
   	 float Ks_N = 0.1;
   	 float Ks_P = 0.01;
   	 float Km = 0.01; // algal mortality d-1
   	 float Ks = 0.01; // algal sloughing d-1
     float Ben_C2CHL = 50;
   	 //Benthic Carbon variables
   	 float uEinsteinperm2stoWattperm2 = 0.2174;  //see websitehttp://www.seabird.com/application_notes/AN11General.htm based on Morel and Smith 1974, valid for sun altitudes > 22 degrees
     float daylength = 12; //hours of light per day needed to convert daily par to per second
     float Ed;
     float Ek = 100;  // PAR in microEinsteins / m2/s at onset of saturation - in Table 10.1 of Kirk, ueinsteins / m2 / s
     float Ben_GPPmax;  //g C / h / g chl at light saturation
     float Ben_GPP; // g C / m2 /d
     float Ben_MORTALITY;
     float Ben_Ra;
     float Ben_NPP;
     float Ben_AlgaeC;
     float Ben_AlgaeCHL;
     float Ben_AlgaeC_REACH;
     float Ben_GPP_REACH;
     float Ben_Ra_REACH;
     float Ben_NPP_REACH;
     float Ben_Mortality_REACH;
     float day;
     day = MFDateGetCurrentDay();
     float month;
     month = MFDateGetCurrentMonth();

     channelWidth       = MFVarGetFloat ( _MDInRiverWidthID,  itemID, 0.0);
     par2bottom         = MFVarGetFloat ( _MDInPAR2BottomID, itemID, 0.0);
     Ben_AlgaeCHL       = MFVarGetFloat ( _MDBenthicAlgaeCHLID, itemID, 0.0); // g Chl / m2
     Ben_AlgaeC         = MFVarGetFloat ( _MDBenthicAlgaeCID, itemID, 0.0); // g C / m2
     
     //TODO:  How to get Chl spun up - uptake depends on CHL, but CHL starts at 0. The following is a place holder:
     Ben_AlgaeC = ((day == 1 && Ben_AlgaeC == 0.0) ? 0.1 : Ben_AlgaeC);
     Ben_AlgaeCHL = ((day == 1 && Ben_AlgaeC == 0.0) ? 0.1 * Ben_C2CHL : Ben_AlgaeCHL); 
     Ed = (par2bottom / (daylength * 60 * 60)) * pow(10,6) * 1 * (1 / uEinsteinperm2stoWattperm2);    // uEinsteins / m2 / s, average for day assuming daylight hours and converted from radiation units of MJ/m2/d
     Ben_GPPmax = MDMaximum(0.0, MDMaximum(1, (3.7 - (0.1 * Ben_AlgaeCHL)))); //g C / hr / g chl at light saturation Ryther and Yentsch 1957, but add a self shading control
     //refs for the eqn Ryther and Yentsch 1957;  Kirk 1994 (EQN 10.4)
     Ben_GPP = Ben_AlgaeCHL * Ben_GPPmax * daylength * 
               DIN / (DIN + Ks_N) * DIP / (DIP + Ks_P) * (1 - exp(-Ed / Ek)); //g C / m2 / d
     //          N limitation          P limitation    decay from light saturation

     float Ben_RespRate = 0.1; //d-1
     Ben_Ra        = MDMinimum(Ben_AlgaeC * 0.9, Ben_AlgaeC * Ben_RespRate);  // g C / m2 / day    autotrophic respiration
     Ks = 0.05 + 0.01 * (Ben_AlgaeC + Ben_GPP - Ben_Ra);
     Ben_MORTALITY = MDMinimum((Ben_AlgaeC - Ben_Ra), (Ben_AlgaeC - Ben_Ra) * (Km + Ks)); //gC / m2 / day;
     Ben_NPP = Ben_GPP - Ben_Ra; 
     Ben_AlgaeC = Ben_AlgaeC + Ben_GPP - Ben_MORTALITY - Ben_Ra ; // g C / m2 / day 
     Ben_AlgaeCHL = Ben_AlgaeC / Ben_C2CHL; //g Chl / m2
     //if (par2bottom > 0.0){
     if (itemID == 175){
       printf("itemID %d month %f day %f par2bottom %f Ed %f Ben_GPP %f Ben_Ra %f Ben_NPP %f Ben_Mortality %f Ben_AlgaeC %f Ben_AlgaeCHL %f \n", 
    		 itemID, month, day, par2bottom, Ed, Ben_GPP, Ben_Ra, Ben_NPP, Ben_MORTALITY, Ben_AlgaeC, Ben_AlgaeCHL );
     }
     
     Ben_AlgaeC_REACH = Ben_AlgaeC * channelWidth * MFModelGetLength(itemID);
     Ben_GPP_REACH = Ben_GPP * channelWidth * MFModelGetLength(itemID);
     Ben_Ra_REACH = Ben_Ra * channelWidth * MFModelGetLength(itemID);
     Ben_NPP_REACH = Ben_NPP * channelWidth * MFModelGetLength(itemID);
     Ben_Mortality_REACH = Ben_MORTALITY * channelWidth * MFModelGetLength(itemID);
     
         	MFVarSetFloat(_MDBenthicAlgaeCHLID, itemID, Ben_AlgaeCHL);
         	MFVarSetFloat(_MDBenthicAlgaeCID, itemID, Ben_AlgaeC);
         	MFVarSetFloat(_MDBenthicGPPID, itemID, Ben_GPP);
         	MFVarSetFloat(_MDBenthicRaID, itemID, Ben_Ra);
         	MFVarSetFloat(_MDBenthicNPPID, itemID, Ben_NPP);
         	MFVarSetFloat(_MDBenthicMortalityID, itemID, Ben_MORTALITY);
         	MFVarSetFloat(_MDBenthicAlgaeC_REACHID, itemID, Ben_AlgaeC_REACH);
         	MFVarSetFloat(_MDBenthicGPP_REACHID, itemID, Ben_GPP_REACH);
         	MFVarSetFloat(_MDBenthicRa_REACHID, itemID, Ben_Ra_REACH);
         	MFVarSetFloat(_MDBenthicNPP_REACHID, itemID, Ben_NPP_REACH);
         	MFVarSetFloat(_MDBenthicMortality_REACHID, itemID, Ben_Mortality_REACH);
    }

int MDBgcRiverGPPDef () {
	int optID = MFUnset;
    const char *optStr;
		const char *options [] = { "none", "calculate", (char *) NULL };

	if (_MDRiverGPPID != MFUnset) return (_MDRiverGPPID);

	MFDefEntering ("Calculate river GPP");
	
	if (((optStr = MFOptionGet (MDOptReservoirs))  == (char *) NULL) || ((optID = CMoptLookup ( options, optStr, true)) == CMfailed)) {
				CMmsgPrint(CMmsgUsrError,"Reservoir Option not specified! Option none or calculate \n");
				return CMfailed;
			}
    if (optID==1){
//  	printf ("Resoption=%i\n",optID);
    if (((_MDInResStorageID           = MFVarGetID (MDVarReservoirStorage,       "km3",     MFInput, MFState, MFInitial))   == CMfailed) ||
        ((_MDInResStorageChangeID     = MFVarGetID (MDVarReservoirStorageChange, "km3/s",   MFInput, MFState, MFBoundary))  == CMfailed))
    	return CMfailed;
    }
	
//input
	if (//((_MDInDischargeID            = MDDischargeDef    ()) == CMfailed) ||
		 ((_MDInRiverLightID          = MDBgcRiverLightDef ()) == CMfailed) ||
         ((_MDInDischargeID           = MFVarGetID (MDVarDischarge,              "m3/s",    MFInput, MFState,  MFBoundary)) == CMfailed) ||
         ((_MDInRiverWidthID          = MFVarGetID (MDVarRiverWidth,             "m",       MFInput, MFState,  MFBoundary)) == CMfailed) ||
         ((_MDInPAR2BottomID          = MFVarGetID (MDVarPAR2Bottom,             "MJ/m2/d", MFInput,  MFState, MFBoundary)) == CMfailed) ||
// output
     	((_MDBenthicAlgaeCHLID        = MFVarGetID (MDVarBenthicAlgaeCHL,        "g/m2",    MFOutput, MFState, MFInitial))  == CMfailed) ||
        ((_MDBenthicAlgaeCID          = MFVarGetID (MDVarBenthicAlgaeC,          "g/m2",    MFOutput, MFState, MFInitial))  == CMfailed) ||
        ((_MDBenthicGPPID             = MFVarGetID (MDVarBenthicGPP,             "g/m2/d",  MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDBenthicRaID              = MFVarGetID (MDVarBenthicRa,              "g/m2/d",  MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDBenthicNPPID             = MFVarGetID (MDVarBenthicNPP,             "g/m2/d",  MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDBenthicMortalityID       = MFVarGetID (MDVarBenthicMortality,       "g/m2/d",  MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDBenthicAlgaeC_REACHID    = MFVarGetID (MDVarBenthicAlgaeC_REACH,    "g/m2",    MFOutput, MFState, MFInitial))  == CMfailed) ||
        ((_MDBenthicGPP_REACHID       = MFVarGetID (MDVarBenthicGPP_REACH,       "g/m2/d",  MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDBenthicRa_REACHID        = MFVarGetID (MDVarBenthicRa_REACH,        "g/m2/d",  MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDBenthicNPP_REACHID       = MFVarGetID (MDVarBenthicNPP_REACH,       "g/m2/d",  MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDBenthicMortality_REACHID = MFVarGetID (MDVarBenthicMortality_REACH, "g/m2/d",  MFOutput, MFState, MFBoundary)) == CMfailed) ||

       (MFModelAddFunction (_MDRiverGPP) == CMfailed)) return (CMfailed);
      
	   MFDefLeaving ("Calculate river GPP");
	   return (_MDRiverGPPID);
}
