/******************************************************************************
Calculating the input layers for the BQART model: Qs=w*B*Q^n1*A^n2*R*T
where Q is discharge [km3/yr] (calculated by WBM), A is the contributing area [km2],
R is the maximum Relief [km], T is average temperature [c] and B is for geological 
and human factors.

Writen Sep 3 2010
Modified: 
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>
#include <math.h>

// Input
static int _MDInAirTempID   = MFUnset;
static int _MDInElevationID = MFUnset;
static int _MDInBQART_LithologyID  = MFUnset;
static int _MDInPopulationID = MFUnset; 
static int _MDInBQART_GNPID = MFUnset; 

// Output
static int _MDInContributingAreaID   = MFUnset;
static int _MDOutBFactorID           = MFUnset;
static int _MDOutLithologyAreaAccID  = MFUnset;
static int _MDOutNumPixelsID         = MFUnset;
static int _MDOutLithologyMeanID     = MFUnset;
static int _MDOutPopulationAreaAccID = MFUnset;
static int _MDOutPopulationDensityID = MFUnset;

static void _MDBQARTinputs (int itemID) {

	float upStreamArea;
	int AccPixels;
	float meanLithology,upLithologyArea ;
	float PopulationAreaAcc,PopuDesity;

//Calculating accumulating area	
	upStreamArea = MFVarGetFloat (_MDInContributingAreaID, itemID, 0.0) + (MFModelGetArea (itemID));//(pow(1000,2)));// convert from m2 to km2  
	AccPixels = MFVarGetInt (_MDOutNumPixelsID, itemID, 0.0) + 1; //Accumulative pixels
// Accumulating pixelArea*Lithology factor
	upLithologyArea =  MFVarGetFloat(_MDOutLithologyAreaAccID, itemID, 0.0) + (MFVarGetFloat (_MDInBQART_LithologyID, itemID, 0.0))*(MFModelGetArea (itemID)); //the lithology factor times the area of the pixel
	meanLithology = upLithologyArea/upStreamArea/AccPixels;	//mean lithology
	
	MFVarSetFloat (_MDOutLithologyAreaAccID, itemID, upLithologyArea);
	MFVarSetFloat (_MDOutLithologyMeanID, itemID, meanLithology);

// Calculating mean population density
	PopulationAreaAcc = MFVarGetFloat(_MDOutPopulationAreaAccID, itemID, 0.0) + (MFVarGetFloat(_MDInPopulationID, itemID, 0.0)*MFModelGetArea (itemID));
	MFVarSetFloat (_MDOutPopulationAreaAccID, itemID, PopulationAreaAcc);
	PopuDesity = PopulationAreaAcc/upStreamArea/AccPixels;
	MFVarSetFloat (_MDOutPopulationDensityID, itemID, PopuDesity);
// Calculating mean GNP

// exporting outputs flux to files
	MFVarSetFloat (_MDInContributingAreaID, itemID, upStreamArea);
	MFVarSetInt   (_MDOutNumPixelsID, itemID, AccPixels); 	
}


int MDSediment_BQARTinputsDef() {
	
	MFDefEntering ("BQARTinputs");
	
	if (((_MDInAirTempID            = MFVarGetID (MDVarCommon_AirTemperature,      "degC",   MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInElevationID          = MFVarGetID (MDVarCommon_Elevation,           "m",      MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInBQART_LithologyID    = MFVarGetID (MDVarSediment_BQART_Lithology,   MFNoUnit, MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInPopulationID         = MFVarGetID (MDVarSediment_Population,        MFNoUnit, MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInBQART_GNPID          = MFVarGetID (MDVarSediment_BQART_GNP,         MFNoUnit, MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInContributingAreaID   = MFVarGetID (MDVarSediment_ContributingArea,  "km2", 	 MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutPopulationAreaAccID = MFVarGetID (MDVarSediment_PopulationAcc,     "",       MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutNumPixelsID         = MFVarGetID (MDVarSediment_NumPixels,         "",       MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutLithologyAreaAccID  = MFVarGetID (MDVarSediment_LithologyAreaAcc,  "" ,      MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutLithologyMeanID  	= MFVarGetID (MDVarSediment_LithologyMean,	   "" ,      MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutPopulationDensityID = MFVarGetID (MDVarSediment_PopulationDensity, "km2",    MFOutput, MFState, MFBoundary)) == CMfailed) ||
       (MFModelAddFunction (_MDBQARTinputs) == CMfailed)) return (CMfailed);

	MFDefLeaving  ("BQARTinputsBQARTinputs");
	return (_MDInContributingAreaID);
}