/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDStormRunoff.c

rob.stewart@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <cm.h>
#include <math.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInImpFractionID       = MFUnset;
static int _MDInH2OFractionID       = MFUnset;
static int _MDInPrecipID            = MFUnset;
static int _MDInSPackChgID          = MFUnset;
static int _MDInHCIAID		    = MFUnset;

// Output
static int _MDOutStormRunoffH2OID 	= MFUnset;
static int _MDOutStormRunoffImpID 	= MFUnset;
static int _MDOutStormRunoffTotalID = MFUnset;
static int _MDOutRunofftoPervID 	= MFUnset;
static int _MDOutPrecipPervID       = MFUnset;
static int _MDOutImpFractionID          = MFUnset;
static int _MDOutH2OFractionID          = MFUnset;

static void _MDStormRunoff (int itemID) {

	// Input
	float precip 		= 0.0;
	float runoffImp		= 0.0;
	float runoffH2O		= 0.0;
	float runoffTotal	= 0.0;
	float runofftoPerv	= 0.0;
	float hcia			= 0.0;
        float h2oAreaFrac 	= 0.0;
	float impAreaFrac 	= 0.0;
	float snowpackChg	= 0.0;
	float precipPerv	= 0.0;
	float cell			= 689;

	snowpackChg = MFVarGetFloat (_MDInSPackChgID, 	 itemID, 0.0);
	precip      = MFVarGetFloat (_MDInPrecipID,  	 itemID, 0.0);
	hcia        = MFVarGetFloat (_MDInHCIAID,     	 itemID, 0.0);
	impAreaFrac = MFVarGetFloat (_MDInImpFractionID, itemID, 0.0);
	h2oAreaFrac = MFVarGetFloat (_MDInH2OFractionID, itemID, 0.0);

//	impAreaFrac = impAreaFrac > 0.99 ? 0.99 : impAreaFrac;		// commented out 091713

	if (snowpackChg >  0.0) precip = 0.0; //Snow Accumulation, no liquid precipitation
	if (snowpackChg <= 0.0) precip = precip + fabs (snowpackChg); //Add Snowmelt

	if (h2oAreaFrac + impAreaFrac > 1.0) {
		h2oAreaFrac = h2oAreaFrac / (h2oAreaFrac + impAreaFrac);
		impAreaFrac = 1.0 - h2oAreaFrac;
	}

	if (h2oAreaFrac + impAreaFrac > 0.99) {
		h2oAreaFrac = h2oAreaFrac < 0.005 ? 0.0 : h2oAreaFrac - 0.005;
		impAreaFrac = impAreaFrac < 0.005 ? 0.0 : impAreaFrac - 0.005;
	}

	runoffImp   = (impAreaFrac * hcia) * precip;   					// depth scaled to entire cell
	runoffH2O   = (h2oAreaFrac * 1.00) * precip;   					// depth scaled to entire cell
	runoffTotal = runoffImp + runoffH2O;                         	// depth scaled to entire cell
//	runoffTotal = runoffImp;										// depth scaled to entire cell
	precipPerv  = precip * (1.00 - impAreaFrac - h2oAreaFrac);   	// depth scaled to entire cell
//	precipPerv  = precip * (1.00 - impAreaFrac);					// depth scaled to entire cell

	runofftoPerv = (impAreaFrac * (1.00 - hcia)) * precip; 			// depth scaled to entire cell

	MFVarSetFloat (_MDOutStormRunoffImpID,   itemID, runoffImp);
	MFVarSetFloat (_MDOutStormRunoffH2OID,   itemID, runoffH2O);
	MFVarSetFloat (_MDOutStormRunoffTotalID, itemID, runoffTotal);
	MFVarSetFloat (_MDOutRunofftoPervID,     itemID, runofftoPerv);
	MFVarSetFloat (_MDOutPrecipPervID,       itemID, precipPerv);
        MFVarSetFloat (_MDOutImpFractionID,      itemID, impAreaFrac);  // 10/01/13
        MFVarSetFloat (_MDOutH2OFractionID,      itemID, h2oAreaFrac);  // 10/01/13

	float balance = precip - runoffTotal - runofftoPerv - precipPerv;

//	if (itemID == cell) printf("m = %d, d = %d, precip = %f, snowpackChg = %f, FracImp = %f, FracH2o = %f\n runoffImp = %f, runoffH2O = %f, runoffTotal = %f, runofftoPerv = %f\n", MFDateGetCurrentMonth(), MFDateGetCurrentDay(), precip, snowpackChg, impAreaFrac, h2oAreaFrac, runoffImp, runoffH2O, runoffTotal, runofftoPerv);		//RJS 030510


	if (fabs (balance) > 0.001) {
	printf("BALANCE: FracImp = %f, FracH2o = %f, runoffImp = %f, runoffH2O = %f, runoffTotal = %f, runofftoPerv = %f\n", impAreaFrac, h2oAreaFrac, runoffImp, runoffH2O, runoffTotal, runofftoPerv);		//RJS 030510
	if(itemID == cell) printf("balance = %f, precip = %f, precipPerv = %f, ImpAreaFrac = %f\n", balance, precip, precipPerv, impAreaFrac);
	}

}


int MDStormRunoffDef () {

	if (_MDOutStormRunoffTotalID != MFUnset) return (_MDOutStormRunoffTotalID);

	MFDefEntering ("Storm Runoff");

	if (((_MDInPrecipID             = MDPrecipitationDef ()) == CMfailed) ||
            ((_MDInH2OFractionID        = MFVarGetID (MDVarH2OFracSpatial,  "-",   MFInput,  MFState, MFBoundary)) == CMfailed) ||		//commented out 082812
	    ((_MDInImpFractionID        = MFVarGetID (MDVarImpFracSpatial,  "-",    MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInSPackChgID           = MFVarGetID (MDVarSnowPackChange,  "mm",   MFInput,  MFFlux, MFBoundary)) == CMfailed) ||
	    ((_MDInHCIAID               = MFVarGetID (MDVarHCIA,            "-",    MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutStormRunoffImpID    = MFVarGetID (MDVarStormRunoffImp,  "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDOutStormRunoffH2OID    = MFVarGetID (MDVarStormRunoffH2O,  "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||		//commented out 082812
	    ((_MDOutPrecipPervID        = MFVarGetID (MDVarPrecipPerv,      "mm",   MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
	    ((_MDOutStormRunoffTotalID  = MFVarGetID (MDVarStormRunoffTotal,"mm",   MFOutput, MFFlux, MFBoundary))  == CMfailed) ||
            ((_MDOutH2OFractionID       = MFVarGetID (MDVarH2OFracSpatial,  "-",    MFOutput,  MFState, MFBoundary)) == CMfailed) ||		// RJS 100113
	    ((_MDOutImpFractionID       = MFVarGetID (MDVarImpFracSpatial,  "-",    MFOutput,  MFState, MFBoundary)) == CMfailed) ||           // RJS 100113
            ((_MDOutRunofftoPervID      = MFVarGetID (MDVarRunofftoPerv,    "mm",   MFOutput, MFFlux, MFBoundary))  == CMfailed) ||
	    (MFModelAddFunction (_MDStormRunoff) == CMfailed)) return (CMfailed);

	MFDefLeaving ("Storm Runoff");
	return (_MDOutRunofftoPervID);
}



