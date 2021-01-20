/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

MDIrrigation.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <MF.h>
#include <MD.h>

typedef struct MDIrrigatedCrop_s {
    int   ID;
    int   DW_ID;
    char  cropName [80];
    char  cropDistrFileName [80];
    int   cropIsRice;
    float cropSeasLength [4];
    float cropKc [4 - 1];
    float cropRootingDepth;
    float cropDepletionFactor;
} MDIrrigatedCrop;

static MDIrrigatedCrop *_MDirrigCropStruct = (MDIrrigatedCrop *) NULL;

//Input
static int  _MDInIrrigation_AreaFracID          = MFUnset;
static int *_MDInCropFractionIDs        = (int *) NULL;
static int  _MDInCommon_PrecipID               = MFUnset;
static int  _MDInSPackChgID             = MFUnset;
static int  _MDInIrrRefEvapotransID     = MFUnset;
static int  _MDInFldCapaID              = MFUnset;
static int  _MDInWltPntID               = MFUnset;
static int  _MDInGrowingSeason1ID       = MFUnset;
static int  _MDInGrowingSeason2ID       = MFUnset;
static int  _MDInRicePondingDepthID     = MFUnset;
static int  _MDInRicePercolationRateID  = MFUnset;
static int  _MDInIrrIntensityID         = MFUnset;
static int  _MDInIrrEfficiencyID        = MFUnset;
//Output
static int  _MDOutIrrEvapotranspID      = MFUnset;
static int  _MDOutIrrNetDemandID        = MFUnset;
static int  _MDOutIrrGrossDemandID      = MFUnset;
static int  _MDOutIrrSMoistID           = MFUnset;
static int  _MDOutIrrSMoistChgID        = MFUnset;
static int  _MDOutIrrPrecipitationID    = MFUnset;
static int  _MDOutIrrReturnFlowID       = MFUnset;
static int  _MDOutIrrRunoffID           = MFUnset;
static int *_MDOutCropSMoistIDs         = (int *) NULL;
static int *_MDOutCropActSMoistIDs      = (int *) NULL;

static int  _MDNumberOfIrrCrops;

static int _MDIrrDaysSincePlanting (int dayOfYearModel,int numGrowingSeasons, int *dayOfYearPlanting, int crop) {
	int i, ret = 0;
	int daysSincePlanted;

	if (crop >= _MDNumberOfIrrCrops) return (ret); // Bare soil
	for (i = 0; i < numGrowingSeasons; ++i) {
		daysSincePlanted = dayOfYearModel - dayOfYearPlanting [i];
		if (daysSincePlanted < 0)  daysSincePlanted = 365 + (dayOfYearModel - dayOfYearPlanting [i]);
		if (daysSincePlanted < _MDirrigCropStruct [crop].cropSeasLength[0]
		                     + _MDirrigCropStruct [crop].cropSeasLength[1]
							 + _MDirrigCropStruct [crop].cropSeasLength[2]
							 + _MDirrigCropStruct [crop].cropSeasLength[3]) ret = daysSincePlanted;
	}
	return (ret);
}

static int _MDIrCropStage (int daysSincePlanted, int crop) {
	int ret = 0;

	if (crop >= _MDNumberOfIrrCrops) return (ret); // Bare soil
    if      (daysSincePlanted <= _MDirrigCropStruct [crop].cropSeasLength[0]) ret = 1;
    else if (daysSincePlanted <= _MDirrigCropStruct [crop].cropSeasLength[0]
	                           + _MDirrigCropStruct [crop].cropSeasLength[1]) ret = 2;
    else if (daysSincePlanted <= _MDirrigCropStruct [crop].cropSeasLength[0]
	                           + _MDirrigCropStruct [crop].cropSeasLength[1]
	                           + _MDirrigCropStruct [crop].cropSeasLength[2]) ret = 3;
	else if (daysSincePlanted <= _MDirrigCropStruct [crop].cropSeasLength[0]
	                           + _MDirrigCropStruct [crop].cropSeasLength[1]
	                           + _MDirrigCropStruct [crop].cropSeasLength[2]
	                           + _MDirrigCropStruct [crop].cropSeasLength[3]) ret = 4;
	return (ret);
}

static float _MDIrrCropKc (int daysSincePlanted, int crop) {
	float kc = 0.0;

   //Returns kc depending on the current stage of the growing season
	if (crop >= _MDNumberOfIrrCrops) return (kc); // Bare soil
	switch (_MDIrCropStage (daysSincePlanted, crop)) {
		default:
		case 0: kc = 0.0; break; //crop is not currently grown
		case 1: kc =  _MDirrigCropStruct [crop].cropKc [0]; break;
		case 2: kc =  _MDirrigCropStruct [crop].cropKc [0]
		           + (_MDirrigCropStruct [crop].cropKc [1] - _MDirrigCropStruct [crop].cropKc [0])
		           * (daysSincePlanted - _MDirrigCropStruct [crop].cropSeasLength [0]) / _MDirrigCropStruct [crop].cropSeasLength [1];
			break;
 		case 3: kc =  _MDirrigCropStruct [crop].cropKc [1]; break;
		case 4: kc =  _MDirrigCropStruct [crop].cropKc [1]
		           + (_MDirrigCropStruct [crop].cropKc [2] - _MDirrigCropStruct [crop].cropKc [1])
		           * (daysSincePlanted - _MDirrigCropStruct [crop].cropSeasLength [0]
		                               - _MDirrigCropStruct [crop].cropSeasLength [1]
		                               - _MDirrigCropStruct [crop].cropSeasLength [2]) / _MDirrigCropStruct [crop].cropSeasLength [3];
			break;
	}
 	return (kc);
}

static float _MDIrrCropRootingDepth (int daysSincePlanted, int crop) {
	float cropRootingDepth = 0.0;
	float totalSeasonLenth;

	if (crop >= _MDNumberOfIrrCrops) return (cropRootingDepth); // Bare soil
	totalSeasonLenth = _MDirrigCropStruct [crop].cropSeasLength [0]
                     + _MDirrigCropStruct [crop].cropSeasLength [1]
	                 + _MDirrigCropStruct [crop].cropSeasLength [2]
	                 + _MDirrigCropStruct [crop].cropSeasLength [3];

    cropRootingDepth = _MDirrigCropStruct [crop].cropRootingDepth * (0.5 + 0.5 * sin (3.03 * (daysSincePlanted  /  totalSeasonLenth) - 1.47));
 	if (0.15 > cropRootingDepth) cropRootingDepth = 0.15; // TODO
	return (cropRootingDepth);
}

static float _MDIrrCorrDeplFactor (float cropETP, int crop) {
	float cropDeplFactor = 0.0;

	if (crop >= _MDNumberOfIrrCrops) return (cropDeplFactor); // Bare soil
	cropDeplFactor = _MDirrigCropStruct [crop].cropDepletionFactor + 0.04 * (5.0 - cropETP);
    if (0.1 >= cropDeplFactor) cropDeplFactor = 0.1;
	if (0.8 <= cropDeplFactor) cropDeplFactor = 0.8;
	return (cropDeplFactor);
}

static int _MDIrrReadCropParameters (const char *filename) {
	FILE *cropFILE;
	char buffer [512];
	int  cropID = 0;

	if ((cropFILE = fopen (filename, "r")) == (FILE *) NULL) {
		CMmsgPrint (CMmsgUsrError,"Crop Parameter file could not be opned, filename: %s\n", filename);
		return (CMfailed);
	}
	else {
		fgets (buffer,sizeof (buffer),cropFILE); // read headings..

		while (fgets(buffer, sizeof (buffer), cropFILE) != NULL) {
			_MDirrigCropStruct = (MDIrrigatedCrop *) realloc (_MDirrigCropStruct, (cropID + 1) * sizeof (MDIrrigatedCrop));
			_MDInCropFractionIDs     = (int *) realloc (_MDInCropFractionIDs,     (cropID + 1) * sizeof (int));
			_MDOutCropSMoistIDs      = (int *) realloc (_MDOutCropSMoistIDs,      (cropID + 1) * sizeof (int));
			_MDOutCropActSMoistIDs   = (int *) realloc (_MDOutCropActSMoistIDs,   (cropID + 1) * sizeof (int));
			_MDInCropFractionIDs [cropID] =  _MDOutCropSMoistIDs [cropID] = _MDOutCropActSMoistIDs [cropID] = MFUnset;
			if (sscanf (buffer, "%i" "%i" "%s" "%s" "%f" "%f" "%f" "%f" "%f" "%f" "%f" "%f" "%f",
		       &(_MDirrigCropStruct [cropID].ID),
		       &(_MDirrigCropStruct [cropID].DW_ID),
		         _MDirrigCropStruct [cropID].cropName,
		         _MDirrigCropStruct [cropID].cropDistrFileName,
		       &(_MDirrigCropStruct [cropID].cropKc [0]),
		       &(_MDirrigCropStruct [cropID].cropKc [1]),
		       &(_MDirrigCropStruct [cropID].cropKc [2]),
		       &(_MDirrigCropStruct [cropID].cropSeasLength [0]),
		       &(_MDirrigCropStruct [cropID].cropSeasLength [1]),
		       &(_MDirrigCropStruct [cropID].cropSeasLength [2]),
		       &(_MDirrigCropStruct [cropID].cropSeasLength [3]),
		       &(_MDirrigCropStruct [cropID].cropRootingDepth),
		       &(_MDirrigCropStruct [cropID].cropDepletionFactor)) != 13) break;
			_MDirrigCropStruct [cropID].cropIsRice = strcmp (_MDirrigCropStruct [cropID].cropName , "Rice") == 0 ? 1 : 0;
			cropID += 1;
		}
	}
	fclose (cropFILE);
	return (cropID);
}

static void _MDIrrGrossDemand (int itemID) {
//Input
	float precip;
	float snowpackChg;
	float wltPnt;
	float fldCap;
	float irrAreaFrac;
	float cropFraction [_MDNumberOfIrrCrops + 1];
	int   seasStart [2];
	float ricePercolation;
	float ricePondingDepth;
    float irrIntensity;
	float irrEfficiency;
//Output:
	float irrGrossDemand;
	float irrNetDemand;
	float irrCropETP;
 	float irrReturnFlow;
    float irrRunoff;
    float irrSMoist;
	float irrSMoistChg;
// Local
	int   cropID, curDay, numGrowingSeasons, daysSincePlanted;
	float cropAvlWater, cropMinSMoist;
	float cropETP;
	float cropNetDemand, cropGrossDemand;
    float cropReturnFlow, cropRunoff;
	float cropCurRootingDepth, cropPrevRootingDepth, cropMaxRootingDepth;
	float cropSMoist, cropActSMoist, cropPrevSMoist, cropPrevActSMoist;
	float cropSMoistChg;
	float bareSoil;
	float sumOfCropFractions;
	float refETP;
	
	curDay = MFDateGetDayOfYear ();

	irrAreaFrac = MFVarGetFloat (_MDInIrrigation_AreaFracID, itemID, 0.0);
	
	if (0.0 < irrAreaFrac) {
        irrCropETP = irrNetDemand = irrGrossDemand = irrRunoff = irrReturnFlow = irrSMoist = irrSMoistChg = sumOfCropFractions = 0.0;
		for (cropID = 0; cropID < _MDNumberOfIrrCrops; ++cropID) {
			cropFraction [cropID] = MFVarGetFloat (_MDInCropFractionIDs [cropID],itemID, 0.0);
			sumOfCropFractions += cropFraction [cropID];
		}
		// default to bare soil when there is no irrigated crop in grid cell
		if (0.0 < sumOfCropFractions) cropFraction [_MDNumberOfIrrCrops] = 0.0;
		else cropFraction [_MDNumberOfIrrCrops] = sumOfCropFractions = irrAreaFrac;

		for (cropID = 0; cropID <= _MDNumberOfIrrCrops; ++cropID) cropFraction [cropID] = cropFraction [cropID] / sumOfCropFractions;

		irrIntensity        = MFVarGetFloat (_MDInIrrIntensityID,        itemID, 100.00) / 100.0;
		irrEfficiency       = MFVarGetFloat (_MDInIrrEfficiencyID,       itemID,  38.00);
        seasStart [0]       = MFVarGetFloat (_MDInGrowingSeason1ID,      itemID, -100);
        seasStart [1]       = MFVarGetFloat (_MDInGrowingSeason2ID,      itemID, -100);
		precip              = MFVarGetFloat (_MDInCommon_PrecipID,              itemID,   0.00);
		snowpackChg         = MFVarGetFloat (_MDInSPackChgID,            itemID,   0.00);
        ricePondingDepth    = MFVarGetFloat (_MDInRicePondingDepthID,    itemID,   2.00);
	 	ricePercolation     = MFVarGetFloat (_MDInRicePercolationRateID, itemID,   3.00);
	 	wltPnt              = MFVarGetFloat (_MDInWltPntID,              itemID,   0.15);
		fldCap              = MFVarGetFloat (_MDInFldCapaID,             itemID,   0.25);
		refETP              = MFVarGetFloat (_MDInIrrRefEvapotransID,    itemID,   0.00);
		if (0.0 >= refETP)               refETP =  0.0;
		if (0.0 >= irrEfficiency) irrEfficiency = 38.0;
		if (2.0 < irrIntensity)    irrIntensity =  2.0; // TODO irrIntensity dictates cropping seasons this limits it to 2
		if (0.0 >= fldCap) { fldCap = 0.35; wltPnt = 0.2; }

		precip = 0.0 >= snowpackChg ? precip + fabs (snowpackChg) : 0.0;
		numGrowingSeasons = ceil (irrIntensity);
		for (cropID = 0; cropID <= _MDNumberOfIrrCrops; ++cropID) {
			daysSincePlanted = _MDIrrDaysSincePlanting (curDay, numGrowingSeasons, seasStart, cropID);
            cropPrevSMoist       = MFVarGetFloat (_MDOutCropSMoistIDs    [cropID], itemID, 0.0);
            cropPrevActSMoist    = MFVarGetFloat (_MDOutCropActSMoistIDs [cropID], itemID, 0.0);
            if (0 < daysSincePlanted) { // Growing season
				if (curDay < seasStart [1] || (daysSincePlanted > seasStart [1] - seasStart [0])) // First growing season
					bareSoil = 1.0 > irrIntensity ? cropFraction [cropID] * (1.0 - irrIntensity) : 0.0;
				else  // Second growing season
					bareSoil = 1.0 < irrIntensity ? cropFraction [cropID] * (2.0 - irrIntensity) : cropFraction [cropID];
				cropFraction [cropID] -= bareSoil;
				cropFraction [_MDNumberOfIrrCrops] += bareSoil;
				if (0.0 < cropFraction [cropID]) {
					cropETP = refETP * _MDIrrCropKc (daysSincePlanted, cropID);
/* Rice */			if (_MDirrigCropStruct [cropID].cropIsRice == 1) {
	/* Rainfed */		if (precip >= cropETP + ricePercolation) {
							cropNetDemand  = cropGrossDemand = 0.0;
							cropReturnFlow = 0.0;
                            cropRunoff     = precip - cropETP; // actual percolation can be higher then the rice percolation input
    /* Irrigated */	    } else {
							cropNetDemand  = cropGrossDemand = cropETP + ricePercolation - precip;
							cropReturnFlow = ricePercolation;
                            cropRunoff     = 0.0;
 						}
                        cropSMoistChg = 0.0;
						cropActSMoist = cropSMoist = ricePondingDepth;
/* Non-rice */		} else {
						cropMaxRootingDepth  = _MDirrigCropStruct [cropID].cropRootingDepth;
						cropPrevRootingDepth = _MDIrrCropRootingDepth (daysSincePlanted - 1, cropID);
						cropCurRootingDepth  = _MDIrrCropRootingDepth (daysSincePlanted, cropID);
						cropPrevActSMoist += (cropCurRootingDepth - cropPrevRootingDepth) * (cropPrevSMoist - cropPrevActSMoist) / (cropMaxRootingDepth - cropPrevRootingDepth);
						cropAvlWater  = (fldCap - wltPnt) * cropCurRootingDepth;
						cropMinSMoist = cropAvlWater * _MDIrrCorrDeplFactor (cropETP, cropID);
	/* Rainfed */		if (precip + cropPrevActSMoist > cropETP + cropMinSMoist) {
							cropActSMoist = cropPrevActSMoist + precip - cropETP > cropMinSMoist ? cropPrevSMoist + precip - cropETP : cropMinSMoist;
							if (cropActSMoist > cropAvlWater) cropActSMoist = cropAvlWater;
							cropNetDemand  = cropGrossDemand = 0.0;
							cropReturnFlow = 0.0;
                            cropRunoff     = precip + cropPrevActSMoist - cropETP - cropActSMoist;
 	/* Irrigated */		} else {
							cropActSMoist   = cropMinSMoist;
							cropNetDemand   = cropMinSMoist + cropETP - precip - cropPrevActSMoist;
							cropGrossDemand = cropNetDemand * 100.0 / irrEfficiency;
							cropReturnFlow  = cropGrossDemand - cropNetDemand;
                            cropRunoff      = 0.0;
 						}
						cropSMoist    = cropActSMoist + (cropMaxRootingDepth - cropCurRootingDepth) * (cropPrevSMoist - cropPrevActSMoist) / (cropMaxRootingDepth - cropPrevRootingDepth);
						cropSMoistChg = cropPrevSMoist - cropSMoist;
					}
                    MFVarSetFloat (_MDOutCropSMoistIDs    [cropID], itemID, cropSMoist);
				 	MFVarSetFloat (_MDOutCropActSMoistIDs [cropID], itemID, cropActSMoist);
/* Nothing */	} else {
                    MFVarSetFloat (_MDOutCropSMoistIDs    [cropID], itemID, cropPrevSMoist);
                    MFVarSetFloat (_MDOutCropActSMoistIDs [cropID], itemID, cropPrevActSMoist);
				    continue;
				}
/* Bare */	} else {
			    cropETP         = precip < refETP >= 0.0 ? precip : refETP;
			    cropNetDemand   = cropGrossDemand = cropSMoist = cropSMoistChg   = 0.0;
			    cropRunoff      = precip - cropETP;
			    cropReturnFlow  = 0.0;
                MFVarSetFloat (_MDOutCropSMoistIDs    [cropID], itemID, cropPrevSMoist);
                MFVarSetFloat (_MDOutCropActSMoistIDs [cropID], itemID, cropPrevActSMoist);
            }
			irrCropETP       += cropETP         * cropFraction [cropID];
			irrNetDemand     += cropNetDemand   * cropFraction [cropID];
			irrGrossDemand   += cropGrossDemand * cropFraction [cropID];
            irrReturnFlow    += cropReturnFlow  * cropFraction [cropID];
            irrRunoff        += cropRunoff      * cropFraction [cropID];
			irrSMoist        += cropSMoist      * cropFraction [cropID];
			irrSMoistChg     += cropSMoistChg   * cropFraction [cropID];
		}
		MFVarSetFloat (_MDOutIrrEvapotranspID,   itemID, irrCropETP     * irrAreaFrac);
		MFVarSetFloat (_MDOutIrrNetDemandID,     itemID, irrNetDemand   * irrAreaFrac);
		MFVarSetFloat (_MDOutIrrGrossDemandID,   itemID, irrGrossDemand * irrAreaFrac);
        MFVarSetFloat (_MDOutIrrPrecipitationID, itemID, precip         * irrAreaFrac);
        MFVarSetFloat (_MDOutIrrReturnFlowID,    itemID, irrReturnFlow  * irrAreaFrac);
        MFVarSetFloat (_MDOutIrrRunoffID,        itemID, irrRunoff      * irrAreaFrac);
		MFVarSetFloat (_MDOutIrrSMoistID,        itemID, irrSMoist      * irrAreaFrac);
		MFVarSetFloat (_MDOutIrrSMoistChgID,     itemID, irrSMoistChg   * irrAreaFrac);
	} else { // cell is not irrigated
		MFVarSetFloat (_MDOutIrrEvapotranspID,   itemID, 0.0);
 		MFVarSetFloat (_MDOutIrrNetDemandID,     itemID, 0.0);
		MFVarSetFloat (_MDOutIrrGrossDemandID,   itemID, 0.0);
        MFVarSetFloat (_MDOutIrrPrecipitationID, itemID, 0.0);
        MFVarSetFloat (_MDOutIrrReturnFlowID,    itemID, 0.0);
        MFVarSetFloat (_MDOutIrrRunoffID,        itemID, 0.0);
		MFVarSetFloat (_MDOutIrrSMoistID,        itemID, 0.0);
		MFVarSetFloat (_MDOutIrrSMoistChgID,     itemID, 0.0);
		for (cropID = 0; cropID <= _MDNumberOfIrrCrops; ++cropID) {
			MFVarSetFloat (_MDOutCropSMoistIDs    [cropID], itemID, 0.0);
			MFVarSetFloat (_MDOutCropActSMoistIDs [cropID], itemID, 0.0);
		}
	}
}

#define MDParIrrigationCropFileName "CropParameterFileName"
 
enum { MDnone, MDinput, MDcalculate };

int MDIrrigation_GrossDemandDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDOptConfig_Irrigation;
	const char *options [] = { MDNoneStr, MDInputStr, MDCalculateStr, (char *) NULL };
	const char *cropParameterFileName;
	int cropID;
	char cropFractionName  [128];
	char cropETName        [128];
	char cropSMoistName    [128];
	char cropActSMoistName [128];

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	if ((optID == MDnone) || (_MDOutIrrGrossDemandID != MFUnset)) return (_MDOutIrrGrossDemandID);

	MFDefEntering ("Irrigation Gross Demand");

	switch (optID) {
		case MDinput:
			if (((_MDOutIrrGrossDemandID = MFVarGetID (MDVarIrrigation_GrossDemand, "mm", MFInput, MFFlux, MFBoundary)) == CMfailed) ||
                ((_MDOutIrrReturnFlowID  = MFVarGetID (MDVarIrrigation_ReturnFlow, "mm", MFInput, MFFlux, MFBoundary)) == CMfailed) ||
                ((_MDOutIrrEvapotranspID = MFVarGetID (MDVarIrrigation_Evapotranspiration, "mm", MFInput, MFFlux, MFBoundary)) == CMfailed))
				return (CMfailed);
			break;
		case MDcalculate:
			if (((_MDInCommon_PrecipID       = MDCommon_PrecipitationDef())   == CMfailed) ||
                ((_MDInSPackChgID            = MDCore_SnowPackChgDef())       == CMfailed) ||
                ((_MDInIrrRefEvapotransID    = MDIrrigation_ReferenceETDef()) == CMfailed) ||
                ((_MDInIrrigation_AreaFracID = MDIrrigation_IrrAreaDef())     == CMfailed) ||
                ((_MDInIrrIntensityID        = MFVarGetID (MDVarIrrigation_Intensity,               MFNoUnit, MFInput,  MFState, MFBoundary)) == CMfailed) ||
                ((_MDInIrrEfficiencyID       = MFVarGetID (MDVarIrrigation_Efficiency,              MFNoUnit, MFInput,  MFState, MFBoundary)) == CMfailed) ||
                ((_MDInGrowingSeason1ID      = MFVarGetID (MDVarIrrigation_GrowingSeason1Start,     "DoY",    MFInput,  MFState, MFBoundary)) == CMfailed) ||
                ((_MDInGrowingSeason2ID      = MFVarGetID (MDVarIrrigation_GrowingSeason2Start,     "DoY",    MFInput,  MFState, MFBoundary)) == CMfailed) ||
                ((_MDInWltPntID              = MFVarGetID (MDVarCore_SoilWiltingPoint,              "mm/m",   MFInput,  MFState, MFBoundary)) == CMfailed) ||
                ((_MDInFldCapaID             = MFVarGetID (MDVarCore_SoilFieldCapacity,             "mm/m",   MFInput,  MFState, MFBoundary)) == CMfailed) ||
                ((_MDInRicePondingDepthID    = MFVarGetID (MDVarIrrigation_RicePondingDepth,        "mm",     MFInput,  MFState, MFBoundary)) == CMfailed) ||
                ((_MDInRicePercolationRateID = MFVarGetID (MDVarIrrigation_DailyRicePerolationRate, "mm/day", MFInput , MFState, MFBoundary)) == CMfailed) ||
                ((_MDOutIrrEvapotranspID     = MFVarGetID (MDVarIrrigation_Evapotranspiration,      "mm",     MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
                ((_MDOutIrrNetDemandID       = MFVarGetID (MDVarIrrigation_NetDemand,               "mm",     MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
                ((_MDOutIrrGrossDemandID     = MFVarGetID (MDVarIrrigation_GrossDemand,             "mm",     MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
                ((_MDOutIrrPrecipitationID   = MFVarGetID (MDVarIrrigation_Precipitation,           "mm",     MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
                ((_MDOutIrrReturnFlowID      = MFVarGetID (MDVarIrrigation_ReturnFlow,              "mm",     MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
                ((_MDOutIrrRunoffID          = MFVarGetID (MDVarIrrigation_Runoff,                  "mm",     MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
                ((_MDOutIrrSMoistID          = MFVarGetID (MDVarIrrigation_SoilMoisture,            "mm",     MFOutput, MFState, MFBoundary)) == CMfailed) ||
                ((_MDOutIrrSMoistChgID       = MFVarGetID (MDVarIrrigation_SoilMoistChange,         "mm",     MFOutput, MFFlux,  MFBoundary)) == CMfailed))
				return (CMfailed);

			if (((cropParameterFileName = MFOptionGet (MDParIrrigationCropFileName)) == (char *) NULL)  ||
			    ((_MDNumberOfIrrCrops = _MDIrrReadCropParameters (cropParameterFileName)) == CMfailed)) {
				CMmsgPrint(CMmsgUsrError,"Error reading crop parameter file");
				return (CMfailed);
			}
			for (cropID = 0; cropID < _MDNumberOfIrrCrops; ++cropID) {
				sprintf (cropFractionName,  "CropFraction_%s",     _MDirrigCropStruct [cropID].cropName); // Input Fraction of crop type per cell
				sprintf (cropSMoistName,    "CropSoilMoist_%s",    _MDirrigCropStruct [cropID].cropName); // Output Soil Moisture
				sprintf (cropActSMoistName, "CropActSoilMoist_%s", _MDirrigCropStruct [cropID].cropName); // Output Active Soil Moisture
			    if (((_MDInCropFractionIDs   [cropID] = MFVarGetID (cropFractionName,  MFNoUnit, MFInput,  MFState, MFBoundary)) == CMfailed) ||
			        ((_MDOutCropSMoistIDs    [cropID] = MFVarGetID (cropSMoistName,    "mm",     MFOutput, MFState, MFInitial))  == CMfailed) ||
			        ((_MDOutCropActSMoistIDs [cropID] = MFVarGetID (cropActSMoistName, "mm",     MFOutput, MFState, MFInitial))  == CMfailed)) return (CMfailed);
			}
            sprintf (cropSMoistName,    "CropSoilMoist_%s",    "Bare"); // Output Soil Moisture
            sprintf (cropActSMoistName, "CropActSoilMoist_%s", "Bare"); // Output Active Soil Moisture
            if (((_MDOutCropSMoistIDs    [cropID] = MFVarGetID (cropSMoistName,    "mm",     MFOutput, MFState, MFInitial))  == CMfailed) ||
                ((_MDOutCropActSMoistIDs [cropID] = MFVarGetID (cropActSMoistName, "mm",     MFOutput, MFState, MFInitial))  == CMfailed)) return (CMfailed);
			if (MFModelAddFunction (_MDIrrGrossDemand) == CMfailed) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving("Irrigation Gross Demand");
	return (_MDOutIrrGrossDemandID);
}

int MDIrrigation_ReturnFlowDef () {
	switch (MDIrrigation_GrossDemandDef()) {
        case CMfailed: return (CMfailed);
	    case MFUnset:  return (MFUnset);
        default:
            if (MDIrrigation_IrrAreaDef() == CMfailed) return (CMfailed);
            else return (MFVarGetID (MDVarIrrigation_ReturnFlow, "mm", MFInput, MFFlux, MFBoundary));
    }
}

int MDIrrigation_RunoffDef () {
    switch (MDIrrigation_GrossDemandDef()) {
        case CMfailed: return (CMfailed);
        case MFUnset:  return (MFUnset);
        default:
            if (MDIrrigation_IrrAreaDef() == CMfailed) return (CMfailed);
            else return (MFVarGetID (MDVarIrrigation_Runoff, "mm", MFInput, MFFlux, MFBoundary));
    }
}
