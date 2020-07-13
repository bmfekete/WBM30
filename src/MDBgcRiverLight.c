/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDWRiverLight.c

wil.wollheim@unh.edu

Estimate light inputs to water surface and to river bottom

*******************************************************************************/

#include <string.h>
#include <math.h>
#include <MF.h>
#include <MD.h>

// Model
static int _MDRiverLightID          = MFUnset;

// Input
static int _MDInDischargeID         = MFUnset;
static int _MDInFluxDOCID           = MFUnset; // here to get function call only
static int _MDInRiverWidthID        = MFUnset;
static int _MDInRiverbedWidthMeanID = MFUnset;
static int _MDInRiverDepthID        = MFUnset;
static int _MDInSolarRadID          = MFUnset;
static int _MDInResStorageChangeID  = MFUnset;
static int _MDInResStorageID        = MFUnset;
static int _MDInConcDOCID           = MFUnset;
static int _MDInKoppenID            = MFUnset;

// Output
static int _MDPAR2BottomID          = MFUnset;
static int _MDPAR2ReachID           = MFUnset;

static void _MDRiverLight (int itemID) {
     float discharge;
     float channelDepth;
     float channelWidth;
     float channelWidthMean;
     float width_canopy;   // mean width of overhanging trees from each bank
     float canopy_proportion = 0.7; // proportion of total bank length (stream length * 2) with canopy
     float LAI = 0.9;   // proportion of light shaded out
     float DOC;
   	 int koppen;
   	 

   	 //light parameters
   	 float solarRad;  //MJ/m2/
   	 float Ecan;
   	 float rad2par = 0.5;     // proprotion of total radiation as PAR
   	 float reflectance = 0.9; //proportion of PAR transmitted across air-water interface
   	 float shading;
   	 float turbidity = 10; //NTU
   	 float Kturb = 0.177;  // from Julian et al. 2008
   	 float Kdoc  = 0.05;  //assumption: assume every mg/l of DOC leads to 1% attenuation (m-1)
   	 float Kd;            // m-1
     float par2bottom;
     float par2reach;
   	
     discharge        = MFVarGetFloat ( _MDInDischargeID,        itemID, 0.0);
     solarRad         = MFVarGetFloat ( _MDInSolarRadID,         itemID, 0.0); //MJ/m2/d - CHECK UNITS - already accounts for clouds
     channelDepth     = MFVarGetFloat ( _MDInRiverDepthID,       itemID, 0.0);
     channelWidth     = MFVarGetFloat ( _MDInRiverWidthID,       itemID, 0.0);
     channelWidthMean = MFVarGetFloat (_MDInRiverbedWidthMeanID, itemID, 0.0);  //assume mean width = bankfull width
     DOC              = MFVarGetFloat (_MDInConcDOCID,           itemID, 0.0) * 1000; //converts kg/m3 to mg/l
     koppen           = MFVarGetInt   (_MDInKoppenID,            itemID, 0.0); // % TODO I think this is not % but category that should be read as integer FBM

     // KoppenID's : 1=Taiga, 2=semi-arid,3=tundra,4=temperate,5=tropics
     // Canopy width (m), assumption: Tundra=0,Taiga=2,Temperate=5,Semi-arid=0, WetTropics=10, DryTropics=0
     switch (koppen) { // TODO Wil originally read this as float and tested for nan which should never happen particularly, when it is read as integer.
     case 1: width_canopy = 2;  break;
     case 2: width_canopy = 0;  break;
     case 3: width_canopy = 0;  break;
     case 4: width_canopy = 5;  break;
     case 5: width_canopy = 10; break;
     }

	  Ecan = solarRad * rad2par;
	  if (!isnan(discharge) && (channelWidthMean > 0) && (channelWidth > 0)){
		  if ((channelWidthMean / 2) < width_canopy){
			  shading = canopy_proportion * MDMinimum(1, LAI);
		  }
		  else {
			  //proportion with canopy * proportion of half width not shaded
			  // does not include effect of variable width within channel; need to scale down based on actual water surface area
			  shading = canopy_proportion * (width_canopy / (channelWidthMean / 2)) * MDMinimum(1,LAI);
		  }
		  if (discharge > 0){
			  // turbidity = turbidity_const * pow(Q, turbidity_slope);
			  Kd = Kturb * turbidity + Kdoc * DOC; //Figure 5; Julian et al. 2008
			  par2bottom = Ecan * (1 - shading) * reflectance * pow(2.71828, -1 * Kd * channelDepth);
			  if (isnan(par2bottom)){
				  printf("Light: Q %f solarRad %f DOC %f Ecan %f shading %f reflectance %f Kd %f channelWidthMean %f channelDepth %f par2bottom %f \n",
						  discharge, solarRad, DOC, Ecan, shading, reflectance, Kd, channelWidthMean, channelDepth, par2bottom);
			  }
		  }
		  else {
			  par2bottom = Ecan * shading;
		  }
		  par2reach = par2bottom * channelWidth * MFModelGetLength(itemID);   //MJ/reach
		  MFVarSetFloat(_MDPAR2BottomID, itemID, par2bottom);
		  MFVarSetFloat(_MDPAR2ReachID, itemID, par2reach);
	  }
	  else{
		  MFVarSetFloat(_MDPAR2BottomID, itemID, 0.0);
		  MFVarSetFloat(_MDPAR2ReachID, itemID, 0.0);
	  }
}

int MDBgcRiverLightDef () {
	int optID = MFUnset;
    const char *optStr;
		const char *options [] = { "none", "calculate", (char *) NULL };

	if (_MDRiverLightID != MFUnset) return (_MDRiverLightID);

	MFDefEntering ("Calculate river light");
	
	if (((optStr = MFOptionGet (MDOptReservoirs))  == (char *) NULL) || ((optID = CMoptLookup ( options, optStr, true)) == CMfailed)) {
				CMmsgPrint(CMmsgUsrError,"Reservoir Option not specified! Option none or calculate \n");
				return CMfailed;
			}
    if (optID==1){
  //  	printf ("Resoption=%i\n",optID);
    if (((_MDInResStorageID        = MFVarGetID (MDVarReservoirStorage,       "km3",   MFInput, MFState, MFInitial))  == CMfailed) ||
        ((_MDInResStorageChangeID  = MFVarGetID (MDVarReservoirStorageChange, "km3/s", MFInput, MFState, MFBoundary)) == CMfailed))
    	return CMfailed;
    }
	
		//input
	if (//((_MDInDischargeID          = MDDischargeDef    ()) == CMfailed) ||
         // ((_MDInSolarRadID         = MDSolarRadDef     ()) == CMfailed) ||
         ((_MDInFluxDOCID           = MDBgcDOCRoutingDef   ()) == CMfailed) ||
         ((_MDInSolarRadID          = MFVarGetID (MDVarSolarRadiation,     "MJ/m2/d", MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
         ((_MDInDischargeID         = MFVarGetID (MDVarDischarge,          "m3/s",    MFInput,  MFState, MFBoundary)) == CMfailed) ||
         ((_MDInConcDOCID           = MFVarGetID (MDVarDOCConcentration,   "kg/m3",   MFInput,  MFState, MFBoundary)) == CMfailed) ||
         ((_MDInRiverWidthID        = MFVarGetID (MDVarRiverWidth,         "m",       MFInput,  MFState, MFBoundary)) == CMfailed) ||
         ((_MDInRiverbedWidthMeanID = MFVarGetID (MDVarRiverbedWidthMean,  "m",       MFInput,  MFState, MFBoundary)) == CMfailed) ||
 	     ((_MDInRiverDepthID        = MFVarGetID (MDVarRiverDepth,         "m",       MFInput,  MFState, MFBoundary)) == CMfailed) ||
 	     ((_MDInKoppenID            = MFVarGetID (MDVarKoppen,             MFNoUnit,  MFInput,  MFState, MFBoundary)) == CMfailed) ||

        // output
        ((_MDPAR2BottomID           = MFVarGetID (MDVarPAR2Bottom,         "MJ/m2/d", MFOutput, MFState, MFBoundary))  == CMfailed) ||
        ((_MDPAR2ReachID            = MFVarGetID (MDVarPAR2Reach,          "MJ/d",    MFOutput, MFState, MFBoundary))  == CMfailed) ||

       (MFModelAddFunction (_MDRiverLight) == CMfailed)) return (CMfailed);
      
	   MFDefLeaving ("Calculate river light");
	   return (_MDRiverLightID);
}
