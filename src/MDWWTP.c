/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDPointSource.c

rob.stewart@unh.edu
m.m.mineau@unh.edu

*******************************************************************************/


#include <stdio.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

// Inputs

static int _MDInWWTP_InKgPerDayID = MFUnset;
static int _MDInWWTP_PopServedID   = MFUnset;
static int _MDInWWTP_RemovalID	   = MFUnset;
static int _MDInWWTP_TreatmentID   = MFUnset;
static int _MDInWWTP_FracDINID     = MFUnset;

// Outputs

static int _MDOutWWTP_OutKgPerDayID   = MFUnset;

static void _MDPointSource (int itemID) {

	float WWTP_InKgPerDay   = 0.0;	// Total daily DIN input 
	float WWTP_PopServed	= 0.0;	// Total population served
	float WWTP_Treatment	= 0.0;	// Type of treatment
	float WWTP_Removal	= 0.0;	// Proportion removal at WWTP
	float WWTP_OutKgPerDay	= 0.0;	// Total Kg effluent at WWTP
        float WWTP_FracDIN      = 0.0;  // Fraction of total effluent N that is DIN

	int day, month, year;

	WWTP_InKgPerDay    	   = MFVarGetFloat (_MDInWWTP_InKgPerDayID,    itemID, 0.0) * 365;
	WWTP_PopServed		   = MFVarGetFloat (_MDInWWTP_PopServedID,     itemID, 0.0);
	WWTP_Treatment		   = MFVarGetFloat (_MDInWWTP_TreatmentID,     itemID, 0.0);
        WWTP_FracDIN               = MFVarGetFloat (_MDInWWTP_FracDINID,       itemID, 0.0);
        
	WWTP_Removal = WWTP_Treatment == 1.0 ? 0.311765 : WWTP_Removal;		// Primary treatment
	WWTP_Removal = WWTP_Treatment == 2.0 ? 0.760735 : WWTP_Removal;		// Secondary treatment
	WWTP_Removal = WWTP_Treatment == 3.0 ? 0.808088 : WWTP_Removal;		// Advanced treatment I
	WWTP_Removal = WWTP_Treatment == 4.0 ? 0.845588 : WWTP_Removal;		// Advanced treatment II

	WWTP_OutKgPerDay = WWTP_InKgPerDay * (1.0 - WWTP_Removal) * WWTP_FracDIN;        // 50% of N is DIN (added 110413)

	day   = MFDateGetCurrentDay ();
	month = MFDateGetCurrentMonth ();
	year  = MFDateGetCurrentYear ();

	if ((WWTP_InKgPerDay > 0.0) && (WWTP_Treatment == 0.0)) printf("itemID = %f, %d-%d-%d, WWTP_InKgPerDay = %f, WWTP_PopServed = %f, WWTP_Treatment = %f, WWTP_Removal = %f\n", itemID, year, month, day, WWTP_InKgPerDay, WWTP_PopServed, WWTP_Treatment, WWTP_Removal);

	MFVarSetFloat (_MDOutWWTP_OutKgPerDayID,   itemID, WWTP_OutKgPerDay);

}


int MDPointSourceDef() {


	MFDefEntering ("WWTP Point Souce");

	 if (((_MDInWWTP_InKgPerDayID	    = MFVarGetID (MDVarWWTPInKgPerDay,	      "kg/d",  MFInput,   MFFlux,  MFBoundary)) == CMfailed) ||
	     ((_MDInWWTP_PopServedID	    = MFVarGetID (MDVarWWTPPopServed,         "-",     MFInput,   MFFlux,  MFBoundary)) == CMfailed) ||
             ((_MDInWWTP_TreatmentID	    = MFVarGetID (MDVarWWTPTreatment,         "-",     MFInput,   MFState, MFBoundary)) == CMfailed) || 
             ((_MDInWWTP_FracDINID          = MFVarGetID (MDVarWWTPFracDIN,           "-",     MFInput,   MFState, MFBoundary)) == CMfailed) ||
             ((_MDOutWWTP_OutKgPerDayID	    = MFVarGetID (MDVarWWTPOutKgPerDay,       "kg/d",  MFOutput,  MFFlux,  MFBoundary)) == CMfailed) ||
             (MFModelAddFunction(_MDPointSource) == CMfailed)) return (CMfailed);

	MFDefLeaving ("WWTP Point Source");
	return (_MDOutWWTP_OutKgPerDayID);

}

