/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2014, UNH - CCNY/CUNY

MDRainSMoistChg.c

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <stdio.h>
#include <math.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

static float _MDSoilMoistALPHA = 5.0;

// Input
static int _MDInAirTMeanID            = MFUnset;
static int _MDInPrecipID              = MFUnset;
static int _MDInPotETID               = MFUnset;
static int _MDInInterceptID           = MFUnset;
static int _MDInSPackChgID            = MFUnset;
static int _MDInSoilAvailWaterCapID   = MFUnset;
static int _MDInIrrAreaFracID         = MFUnset;
static int _MDInRelativeIceContent    = MFUnset;
static int _MDInRunoffToPervID        = MFUnset;  // RJS 082812
static int _MDInImpFractionID         = MFUnset;  // RJS 082812
static int _MDInH2OFractionID         = MFUnset;  // RJS 100103
static int _MDInStormRunoffTotalID    = MFUnset;  // RJS 082812
// static int _MDInImperviousSoilID	  = MFUnset;  // RJS 091213

// Output
static int _MDOutEvaptrsID            = MFUnset;
static int _MDOutEvaptrsNotScaledID   = MFUnset;  // RJS 082812
static int _MDOutSoilMoistNotScaledID = MFUnset;  // renamed from 'Cell' to 'not scaled'
static int _MDOutSoilMoistID          = MFUnset;
static int _MDOutSMoistChgID          = MFUnset;
static int _MDOutLiquidSoilMoistureID = MFUnset;
static int _MDOutExcessID			  = MFUnset;  // RJS 091813
static int _MDOutExcessNotScaledID	  = MFUnset;
static int _MDOutSMoistChgNotScaledID = MFUnset; // RJS 091913

static void _MDRainSMoistChg (int itemID) {	
// Input
	float airT;              	  // Air temperature [degreeC]
	float precip;            	  // Precipitation [mm/dt]
	float pet;               	  // Potential evapotranspiration [mm/dt]
	float intercept;         	  // Interception (when the interception module is turned on) [mm/dt]
	float sPackChg;          	  // Snow pack change [mm/dt]
	float irrAreaFrac      = 0.0; // Irrigated area fraction
	float impAreaFrac      = 0.0; // Impervious area fraction // RJS 082812
        float h2oAreaFrac      = 0.0; // H2O area fraction RJS 100103
	float runoffToPerv     = 0.0; // runoff from impervious to pervious [mm/dt]  RJS 082812
	float stormRunoffTotal = 0.0; // RJS 082812
	float excess	       = 0.0; // RJS 082812
	float sMoist           = 0.0; // Soil moisture [mm/dt]
// Output
	float sMoistChg        = 0.0; // Soil moisture change [mm/dt]
	float evapotrans;
// Local
	float waterIn;
	float awCap;
	float gm;
	float iceContent;
//	float imperviousSoil;		  // 1.0 =  soil AWC layer takes into account impervious surfaces already, 2.0 = soil AWC layer does not take into account imp data

//	imperviousSoil   = MFVarGetFloat (_MDInImperviousSoilID,    itemID, 0.0);	// RJS 091213
	airT             = MFVarGetFloat (_MDInAirTMeanID,          itemID, 0.0);
	precip           = MFVarGetFloat (_MDInPrecipID,            itemID, 0.0);
 	sPackChg         = MFVarGetFloat (_MDInSPackChgID,          itemID, 0.0);
	pet              = MFVarGetFloat (_MDInPotETID,             itemID, 0.0);
	awCap            = MFVarGetFloat (_MDInSoilAvailWaterCapID, itemID, 0.0);
	impAreaFrac      = MFVarGetFloat (_MDInImpFractionID,  	    itemID, 0.0);	// RJS 082812
        h2oAreaFrac      = MFVarGetFloat (_MDInH2OFractionID,       itemID, 0.0);       // RJS 100113
	runoffToPerv     = MFVarGetFloat (_MDInRunoffToPervID,      itemID, 0.0);	// RJS 082812
	stormRunoffTotal = MFVarGetFloat (_MDInStormRunoffTotalID,  itemID, 0.0);	// RJS 082812
	sMoist       	 = MFVarGetFloat (_MDOutSoilMoistNotScaledID,    itemID, 0.0);
	iceContent   	 = _MDInRelativeIceContent != MFUnset ? MFVarGetFloat (_MDInRelativeIceContent,  itemID, 0.0) : 0.0;
	intercept   	 = _MDInInterceptID        != MFUnset ? MFVarGetFloat (_MDInInterceptID,         itemID, 0.0) : 0.0;
	irrAreaFrac  	 = _MDInIrrAreaFracID      != MFUnset ? MFVarGetFloat (_MDInIrrAreaFracID,       itemID, 0.0) : 0.0;

	
	//if (iceContent> 0.0) printf ("IceContent upper Layer = %f\n",iceContent);
	
	//reduce available water capacity by ice content
//	awCap= awCap - (awCap * iceContent);
	
	waterIn = precip - intercept - sPackChg + runoffToPerv;	// RJS 082812

//*	if (airT > 0.0) {											// this was commented out prior to 082812, uncommented 082812

//		waterIn = precip - intercept - sPackChg;		// RJS commented out 082812
		pet = pet > intercept ? pet - intercept : 0.0;

		if (sPackChg <= 0.0) {

		if (awCap > 0.0) {
			if (waterIn > pet) {
				sMoistChg = waterIn - pet < awCap - sMoist ? waterIn - pet : awCap - sMoist;
				excess	  = precip - intercept - sPackChg + runoffToPerv - pet - sMoistChg;	// RJS 082812
			}
			else {
				gm = (1.0 - exp (- _MDSoilMoistALPHA * sMoist / awCap)) / (1.0 - exp (- _MDSoilMoistALPHA));
				sMoistChg = (waterIn - pet) * gm;
			}
			if (sMoist + sMoistChg > awCap) {
				sMoistChg = awCap - sMoist;
				excess	  = precip - intercept - sPackChg + runoffToPerv - pet - sMoistChg;    // RJS 082812
			}
			if (sMoist + sMoistChg <   0.0) sMoistChg =       - sMoist;
			sMoist = sMoist + sMoistChg;

		}
		else {														// RJS 082812 added brackets
			sMoist = sMoistChg = 0.0;								// RJS 082812 moved into brackets
			if (waterIn - sMoistChg > pet + intercept) excess = precip - intercept - sPackChg + runoffToPerv - pet - sMoistChg;	// RJS 082812
			else excess = 0.0;	// RJS 082812
		}

//		evapotrans = pet + intercept < precip - sPackChg - sMoistChg ?		// RJS commented out 082812
//		             pet + intercept : precip - sPackChg - sMoistChg;		// RJS commented out 082812

		evapotrans = pet + intercept < waterIn - sMoistChg?		// RJS 082812
		             pet + intercept : waterIn - sMoistChg;			// RJS 082812

		}

		else {
			sMoistChg = evapotrans = excess = 0.0;
		}

//*	}												// this was commented out prior to 082812.  Uncommented 082812
//	else  { sMoistChg = 0.0; evapotrans = 0.0; }
//*	else  {
//*		sMoistChg  = 0.0; 												// RJS 082812
//*		evapotrans = 0.0; 												// RJS 082812
//*		excess 	   = precip - intercept - sPackChg + runoffToPerv;		// RJS 082812
//*	}																	// RJS 082812

	float balance = waterIn - evapotrans - sMoistChg - excess;	// RJS 082812

//if (itemID == 132) {
//		printf("**** itemID = %d, month = %d, day = %d, airT = %f, surplus = %f, stormRunoff = %f, intercept = %f\n", itemID, MFDateGetCurrentMonth (), MFDateGetCurrentDay (), airT, precip - sPackChg - evapotrans - sMoistChg - stormRunoffTotal, stormRunoffTotal, intercept);
//		printf("Alpha = %f, sMoist = %f, sMoistChg = %f, precip = %f, transp = %f, waterIn = %f\n", _MDSoilMoistALPHA, sMoist, sMoistChg, precip, evapotrans, waterIn);
//		printf("pet = %f, awCap = %f, precip = %f, sPackChg = %f, excess = %f, airT = %f\n", pet, awCap, precip, sPackChg, excess, airT);
//		printf("balance = %f, gm = %f, impAreaFrac = %f, runofftoPerv(basin) = %f, runofftoPerv(perv) = %f\n", balance, gm, impAreaFrac, runoffToPerv * (1.00 - impAreaFrac), runoffToPerv);
//	}

    //    if (itemID == 1) printf("Alpha = %f\n",_MDSoilMoistALPHA);
        
if (fabs(balance) > 0.00001) {
	printf("**** itemID = %d, month = %d, day = %d, airT = %f, surplus = %f, stormRunoff = %f, intercept = %f\n", itemID, MFDateGetCurrentMonth (), MFDateGetCurrentDay (), airT, precip - sPackChg - evapotrans - sMoistChg - stormRunoffTotal, stormRunoffTotal, intercept);
	printf("Alpha = %f, sMoist = %f, sMoistChg = %f, precip = %f, transp = %f, waterIn = %f\n", _MDSoilMoistALPHA, sMoist, sMoistChg, precip, evapotrans, waterIn);
	printf("pet = %f, awCap = %f, precip = %f, sPackChg = %f, excess = %f, airT = %f\n", pet, awCap, precip, sPackChg, excess, airT);
	printf("balance = %f, gm = %f, impAreaFrac = %f, runofftoPerv(basin) = %f, runofftoPerv(perv) = %f\n", balance, gm, impAreaFrac, runoffToPerv * (1.00 - impAreaFrac), runoffToPerv);
	}
	
//if ((itemID == 486)) printf("y = %d, m = %d, d = %d, ******* precip = %f, transp = %f, sPackChg = %f, sMoist = %f, sMoistChg = %f, awCap = %f, excess = %f\n", MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), precip, evapotrans, sPackChg, sMoist, sMoistChg, awCap, excess);	//RJS 071511

	MFVarSetFloat (_MDOutExcessNotScaledID,   itemID, excess);	//RJS 091813
	MFVarSetFloat (_MDOutEvaptrsNotScaledID,  itemID, evapotrans);	// RJS 082812
	MFVarSetFloat (_MDOutSoilMoistNotScaledID,     itemID, sMoist);
	MFVarSetFloat (_MDOutSMoistChgNotScaledID, itemID, sMoistChg);
	MFVarSetFloat (_MDOutExcessID,            itemID, excess     * (1.0 - irrAreaFrac - impAreaFrac - h2oAreaFrac));        //RJS 100113 "-h2oAreaFrac"
	MFVarSetFloat (_MDOutEvaptrsID,           itemID, evapotrans * (1.0 - irrAreaFrac - impAreaFrac - h2oAreaFrac)); 	//RJS 100113 "-h2oAreaFrac"
	MFVarSetFloat (_MDOutSoilMoistID,         itemID, sMoist     * (1.0 - irrAreaFrac - impAreaFrac - h2oAreaFrac)); 	//RJS 100113 "-h2oAreaFrac"
	MFVarSetFloat (_MDOutSMoistChgID,         itemID, sMoistChg  * (1.0 - irrAreaFrac - impAreaFrac - h2oAreaFrac)); 	//RJS 100113 "-h2oAreaFrac"
	if (_MDOutLiquidSoilMoistureID != MFUnset) MFVarSetFloat (_MDOutLiquidSoilMoistureID,itemID, sMoist/awCap);
}

enum { MFnone, MFcalculate };

int MDRainSMoistChgDef () {
	int ret = 0;
	float par;
	const char *optStr;
	if (_MDOutSMoistChgID != MFUnset) return (_MDOutSMoistChgID);
	const char *soilTemperatureOptions [] = { "none", "calculate", (char *) NULL };

	int soilTemperatureID;
	if (((optStr = MFOptionGet (MDOptSoilTemperature))  == (char *) NULL) ||
	    ((soilTemperatureID = CMoptLookup (soilTemperatureOptions, optStr, true)) == CMfailed)) {
		CMmsgPrint(CMmsgUsrError," Soil TemperatureOption not specifed! Options = 'none' or 'calculate'\n");
		return CMfailed;
	}
	if (((optStr = MFOptionGet (MDParSoilMoistALPHA))  != (char *) NULL) && (sscanf (optStr,"%f",&par) == 1)) _MDSoilMoistALPHA = par;
	
	MFDefEntering ("Rainfed Soil Moisture");
	if (soilTemperatureID == MFcalculate ) {
		if (((ret                        = MDPermafrostDef()) == CMfailed) ||
		    ((_MDInRelativeIceContent    = MFVarGetID ("SoilIceContent_01",     "mm",   MFOutput,  MFState, MFBoundary)) == CMfailed) ||
			((_MDOutLiquidSoilMoistureID = MFVarGetID (MDVarLiquidSoilMoisture, "-",    MFOutput,  MFState, MFBoundary)) == CMfailed)) return CMfailed;
	}

	if ((ret = MDIrrGrossDemandDef ()) == CMfailed) return (CMfailed);
	if ((ret != MFUnset) && ((_MDInIrrAreaFracID = MDIrrigatedAreaDef ())==  CMfailed) ) return (CMfailed);

	if (((_MDInPrecipID            = MDPrecipitationDef     ()) == CMfailed) ||
	    ((_MDInSPackChgID          = MDSPackChgDef          ()) == CMfailed) ||
	    ((_MDInPotETID             = MDRainPotETDef         ()) == CMfailed) ||
	    ((_MDInInterceptID         = MDRainInterceptDef     ()) == CMfailed) ||
	    ((_MDInSoilAvailWaterCapID = MDSoilAvailWaterCapDef ()) == CMfailed) ||
	    ((_MDInAirTMeanID          = MFVarGetID (MDVarAirTemperature,             "degC", MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInRunoffToPervID      = MDStormRunoffDef ())  == CMfailed) ||  								// RJS 082812
 //	    ((_MDInH2OFractionID       = MFVarGetID (MDVarH2OFracSpatial,             "mm",   MFInput,  MFState, MFBoundary)) == CMfailed) ||   // RJS 082812
        ((_MDInStormRunoffTotalID  = MFVarGetID (MDVarStormRunoffTotal,           "mm",   MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||	// RJS 082812
	    ((_MDInImpFractionID       = MFVarGetID (MDVarImpFracSpatial,             "-",   MFInput,  MFState, MFBoundary)) == CMfailed) ||   // RJS 082812
 	    ((_MDInH2OFractionID       = MFVarGetID (MDVarH2OFracSpatial,             "-",   MFInput,  MFState, MFBoundary)) == CMfailed) ||   // RJS 082812
            ((_MDOutEvaptrsID          = MFVarGetID (MDVarRainEvapotranspiration,     "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
 	    ((_MDOutExcessID           = MFVarGetID (MDVarExcess,                     "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||	// RJS 091813
 	    ((_MDOutExcessNotScaledID  = MFVarGetID (MDVarExcessNotScaled,            "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||	// RJS 091813
 	    ((_MDOutEvaptrsNotScaledID = MFVarGetID (MDVarRainETnotScaled,            "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||	// RJS 082812
	    ((_MDOutSoilMoistNotScaledID    = MFVarGetID (MDVarRainSoilMoistureNotScaled,       "mm",   MFOutput, MFState, MFInitial))  == CMfailed) ||
	    ((_MDOutSoilMoistID        = MFVarGetID (MDVarRainSoilMoisture,           "mm",   MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutSMoistChgNotScaledID   = MFVarGetID (MDVarRainSoilMoistureChangeNotScaled,       "mm",   MFOutput, MFState, MFInitial))  == CMfailed) ||
	    ((_MDOutSMoistChgID        = MFVarGetID (MDVarRainSoilMoistChange,        "mm",   MFOutput, MFState, MFBoundary)) == CMfailed) ||
        (MFModelAddFunction (_MDRainSMoistChg) == CMfailed)) return (CMfailed);
	MFDefLeaving ("Rainfed Soil Moisture");
	return (_MDOutSMoistChgID);
}
