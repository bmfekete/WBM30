/******************************************************************************
Calculating the Particulate Particulate Nutrients based on 
Beusen et al. (2005; Global Biogeochemical Cycles)
Mayorga et al.(2010; Env. modeling & software)

Organic Carbon: POCpc = -0.16[log(Qc)]^3 + 2.83[log(Qc)]^2 - 13.6log(Qc) + 20.3
Qc in mg/L

Particulate Nitrogen: PNpc = 0.116 * POCpc - 0.019 

Particulate Phosphorus: ln(1000 * PP) = -3.096 + 1.002*ln(1000*POC) 
POC and PP are loads in Mg/yr ot t/yr
 
MDParticulateNutrients
May 2013
sagy.cohen@as.ua.edu
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>
#include <math.h>
#include <stdlib.h>

// Input
static int _MDInQsConcID = MFUnset;
static int _MDInQsYieldID = MFUnset;
static int _MDInSedimentFluxID = MFUnset;

// Output
static int _MDOutPOCarbonPercentID 			= MFUnset;
static int _MDOutPOCarbonConcentrationID 	= MFUnset;
static int _MDOutPOCarbonYieldID	 		= MFUnset;
static int _MDOutPOCarbonFluxID 			= MFUnset;

static int _MDOutPNitrogenPercentID 		= MFUnset;
static int _MDOutPNitrogenConcentrationID 	= MFUnset;
static int _MDOutPNitrogenYieldID	 		= MFUnset;
static int _MDOutPNitrogenFluxID 			= MFUnset;

static int _MDOutPPhosphorusPercentID 		= MFUnset;
static int _MDOutPPhosphorusConcentrationID 	= MFUnset;
static int _MDOutPPhosphorusYieldID	 		= MFUnset;
static int _MDOutPPhosphorusFluxID 			= MFUnset;

static void _MDParticulateNutrients (int itemID) {
	float Qc,Qsy,Qs, POCpc, POCc,POCy,POCf;
	float PNpc, PNc, PNy, PNf;
	float PPpc, PPc, PPy, PPf;
	
//Geting the suspended sediment concentration, yield and flux (from MDSedimentFlux.c)
	Qc = MFVarGetFloat (_MDInQsConcID, 	itemID, 0.0) * 1000;	// orig kg/m3 -> in mg/L	
	Qsy= MFVarGetFloat (_MDInQsYieldID, 	itemID, 0.0) *(60*60*24*365) ;	// in orig kg/km2/s -> kg/km2/y
	Qs = MFVarGetFloat (_MDInSedimentFluxID, 	itemID, 0.0)*(60*60*24*365);// in kg/y
	
	if (Qs < 1.0 ){
		POCpc = 0.0;
		POCc = 0.0;
		POCy = 0.0;
		POCf = 0.0;
		PNpc = 0.0;
		PNc = 0.0;
		PNy = 0.0;
		PNf	= 0.0;
		PPpc = 0.0;
		PPc = 0.0;
		PPy = 0.0;
		PPf = 0.0;
	} else {	
	// Calculating percent POC from Qc
		if (Qc > 2250){//mg/L
			POCpc = 0.5;
		} else {
			POCpc = -0.16*pow(log10(Qc),3) + 2.83*pow(log10(Qc),2) - 13.6*log10(Qc) + 20.3;
		}
	// Calculating PNpc from POCpc
		PNpc = 0.116 * POCpc - 0.019; 
	
	//Calculating actual POC
		POCc = (POCpc/100) * (Qc); // in units of mg/L 
 		POCy = (POCpc/100) * (Qsy); // in units of kg/km2/y
 		POCf = (POCpc/100) * (Qs); // in units of kg/y
	//Calculating actual PN
		PNc = (PNpc/100) * (Qc); // in units of mg/L 
 		PNy = (PNpc/100) * (Qsy); // in units of kg/km2/y
 		PNf = (PNpc/100) * (Qs); // in units of kg/y
 	
	// Calculating Particulate Phosphorus load in kg/yr
		// ln(1000 * PPf) = -3.096 + 1.002*ln(1000*POCf) 
		//PPf  = exp(-3.09614 + 1.001769 * log(POCf)) / 1000.0; //in kg/y
		PPf  = exp(-3.09614 + 1.001769 * log(POCf)); //in kg/y
		PPpc = (100 * PPf) / (Qs); // in (%) percent 
		PPy  = (PPpc/100) * (Qsy); //in kg/km2/y
		PPc  = (PPpc/100) * (Qc); // in mg/L
	} //en else
//returning the value to pixel
	MFVarSetFloat (_MDOutPOCarbonPercentID, itemID, POCpc);	
	MFVarSetFloat (_MDOutPOCarbonConcentrationID, itemID, POCc);	
	MFVarSetFloat (_MDOutPOCarbonYieldID, itemID, POCy);	
	MFVarSetFloat (_MDOutPOCarbonFluxID, itemID, POCf);	

	MFVarSetFloat (_MDOutPNitrogenPercentID, itemID, PNpc);	
	MFVarSetFloat (_MDOutPNitrogenConcentrationID, itemID, PNc);	
	MFVarSetFloat (_MDOutPNitrogenYieldID, itemID, PNy);	
	MFVarSetFloat (_MDOutPNitrogenFluxID, itemID, PNf);

//printf("PPpc:%f\n",PPpc);
	MFVarSetFloat (_MDOutPPhosphorusPercentID, itemID, PPpc);	
//printf("PPp:%f\n",PPc);	
	MFVarSetFloat (_MDOutPPhosphorusConcentrationID, itemID, PPc);
//printf("PPy:%f\n",PPy);	
	MFVarSetFloat (_MDOutPPhosphorusYieldID, itemID, PPy);
//printf("PPf:%f\n",PPf);		
	MFVarSetFloat (_MDOutPPhosphorusFluxID, itemID, PPf);	
	
}

enum { MDinput, MDcalculate, MDcorrected };

int MDSediment_ParticulateNutrientsDef() {
	
	MFDefEntering ("ParticulateNutrients");
	
	if (((_MDInSedimentFluxID          = MDSediment_FluxDef ()) == CMfailed) || 
	((_MDInQsConcID                    = MFVarGetID (MDVarSediment_QsConc,                  "g/L",       MFInput,  MFState, MFBoundary)) == CMfailed) ||
	((_MDInQsYieldID                   = MFVarGetID (MDVarSediment_QsYield,                  "kg/s/km2", MFInput,  MFState, MFBoundary)) == CMfailed) ||
	((_MDOutPOCarbonPercentID          = MFVarGetID (MDVarSediment_POCarbonPercent,          "%",        MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutPOCarbonConcentrationID    = MFVarGetID (MDVarSediment_POCarbonConcentration,    "mg/L",     MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutPOCarbonYieldID            = MFVarGetID (MDVarSediment_POCarbonYield,            "kg/km2/y", MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutPOCarbonFluxID             = MFVarGetID (MDVarSediment_POCarbonFlux,             "kg/y",     MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutPNitrogenPercentID         = MFVarGetID (MDVarSediment_PNitrogenPercent,         "%",        MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutPNitrogenConcentrationID   = MFVarGetID (MDVarSediment_PNitrogenConcentration,   "mg/L",     MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutPNitrogenYieldID           = MFVarGetID (MDVarSediment_PNitrogenYield,           "kg/km2/y", MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutPNitrogenFluxID            = MFVarGetID (MDVarSediment_PNitrogenFlux,            "kg/y",     MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutPPhosphorusPercentID       = MFVarGetID (MDVarSediment_PPhosphorusPercent,       "%",        MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutPPhosphorusConcentrationID = MFVarGetID (MDVarSediment_PPhosphorusConcentration, "mg/L",     MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutPPhosphorusYieldID         = MFVarGetID (MDVarSediment_PPhosphorusYield,         "kg/km2/y", MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutPPhosphorusFluxID          = MFVarGetID (MDVarSediment_PPhosphorusFlux,          "kg/y",     MFOutput, MFState, MFBoundary)) == CMfailed) ||
	(MFModelAddFunction (_MDParticulateNutrients) == CMfailed)) return (CMfailed);

	MFDefLeaving  ("ParticulateNutrients");
	return (_MDOutPOCarbonFluxID);
}
