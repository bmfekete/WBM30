/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDIrrigation.c

dominik.wisser@unh.edu

*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

static int _MDInIrrAreaFracSeason1ID          = MFUnset;
static int _MDInIrrAreaFracSeason2ID          = MFUnset;
static int _MDIrrigatedAreaFracID					=MFUnset; 
static int _MDGrowingSeason1ID				=MFUnset;
static int _MDGrowingSeason2ID				=MFUnset;
static void _MDIrrigatedAreaIWMI (int itemID) {
float irrAreaFrac;	
float irrAreaFracSeason1;
float irrAreaFracSeason2;
float Season1Doy;
float Season2Doy;

Season1Doy= MFVarGetFloat(_MDGrowingSeason1ID,      itemID, 100);
Season2Doy= MFVarGetFloat(_MDGrowingSeason2ID,      itemID, 250);
irrAreaFracSeason1 = MFVarGetFloat(_MDInIrrAreaFracSeason1ID, itemID, 0.0);
irrAreaFracSeason2 = MFVarGetFloat(_MDInIrrAreaFracSeason2ID, itemID, 0.0);

if (Season1Doy < Season2Doy)
{
if (MFDateGetDayOfYear()< Season1Doy)irrAreaFrac=irrAreaFracSeason2;
if (MFDateGetDayOfYear()>=Season1Doy && MFDateGetDayOfYear() < Season2Doy)irrAreaFrac=irrAreaFracSeason1;
if (MFDateGetDayOfYear()>= Season2Doy) irrAreaFrac=irrAreaFracSeason2;

}
else
{
irrAreaFrac=irrAreaFracSeason2;
if (MFDateGetDayOfYear() > Season2Doy && MFDateGetDayOfYear() <= Season1Doy) irrAreaFrac=irrAreaFracSeason1;

}

if (irrAreaFrac < 0 || irrAreaFrac > 1.0)printf ("Doy %i IrriAreaFraction= %f, season1Doy %f Season2Doy %f irrAreaFrac1 %f irrAReaFrac2 %f\n",MFDateGetDayOfYear(),irrAreaFrac, Season1Doy, Season2Doy,irrAreaFracSeason1,irrAreaFracSeason2);
MFVarSetFloat(_MDIrrigatedAreaFracID,     itemID, irrAreaFrac);	
}


enum { FAO_ID = 0, IWMI_ID = 1 };

int MDIrrigatedAreaDef (){
	const char *mapOptions [] = { "FAO", "IWMI", (char *) NULL };
	int optionID;
	const char *optStr;
	if (((optStr = MFOptionGet (MDOptIrrigatedAreaMap))  == (char *) NULL) || ((optionID = CMoptLookup (mapOptions, optStr, true)) == CMfailed)) {
				CMmsgPrint(CMmsgUsrError,"Typ of Irr Area not specifed! Options = 'FAO' or 'IWMI'\n");
				return CMfailed;
			}
	MFDefEntering ("Irrigated Area");
	if (_MDIrrigatedAreaFracID != MFUnset) return (_MDIrrigatedAreaFracID);

	if (optionID == 0) 
	{
	if (((_MDIrrigatedAreaFracID           = MFVarGetID (MDVarIrrAreaFraction,        "-",    MFInput,  MFState, MFBoundary)) == CMfailed))return CMfailed;
		return _MDIrrigatedAreaFracID;
	}
	if (((_MDIrrigatedAreaFracID           = MFVarGetID (MDVarIrrAreaFraction,        "-",    MFOutput,  MFState, MFBoundary)) == CMfailed))return CMfailed;
	if(((_MDInIrrAreaFracSeason1ID         = MFVarGetID (MDVarIrrAreaFractionSeason1,        "-",    MFInput,  MFState, MFBoundary)) == CMfailed)) return CMfailed;
	if(((_MDInIrrAreaFracSeason2ID         = MFVarGetID (MDVarIrrAreaFractionSeason2,        "-",    MFInput,  MFState, MFBoundary)) == CMfailed)) return CMfailed;
    if (((_MDGrowingSeason1ID              = MFVarGetID (MDVarIrrGrowingSeason1Start, "DoY",  MFInput,  MFState, MFBoundary)) == CMfailed))return CMfailed;
    if (((_MDGrowingSeason2ID              = MFVarGetID (MDVarIrrGrowingSeason2Start, "DoY",  MFInput,  MFState, MFBoundary)) == CMfailed))return CMfailed;

	
	
	if (MFModelAddFunction (_MDIrrigatedAreaIWMI) == CMfailed) return (CMfailed);
	MFDefLeaving("IrrigatedAreas");
	return (_MDIrrigatedAreaFracID);
	
}
