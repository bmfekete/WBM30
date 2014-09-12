
/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDRiverLakeAet.c

rob.stewart@unh.edu

*******************************************************************************/

#include <stdio.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

// Inputs
static int _MDInDischLevel3ID       = MFUnset;		//
static int _MDInPetID		    = MFUnset;		//
static int _MDInEvaptrsID	    = MFUnset;		//
static int _MDInH2OFractionID       = MFUnset;		//
static int _MDInRiverWidthID        = MFUnset;		//
static int _MDInLakeYesNoID	    = MFUnset;		//
static int _MDInLakePointAreaID     = MFUnset;		//

// Outputs
static int _MDOutRiverAetID	    = MFUnset;		//
static int _MDOutTotalAetID	    = MFUnset;		//
static int _MDOutLakeAetID	    = MFUnset;		//


static void _MDRiverLakeAet (int itemID) {
// Inputs
	float dischargePre  = 0.0;       			// Discharge [m3/s]
        float dischargePost = 0.0;
	float pet           = 0.0;	   		    // pet [mm]
	float aet           = 0.0;				// aet [mm]
	float H2OAreaFrac   = 0.0;	// h2o area fraction
	float riverAreaFrac = 0.0; // river surface area fraction
	float combinedFrac  = 0.0;	// combined river and h2o area frac
	float width	    = 0.0;	// river width
	float dL	    = 0.0; // river length
	float lakeyesno	    = 0.0;	//
	float lakearea      = 0.0; //

// Outputs

	float riverAet = 0.0;	// river aet [mm]
	float totalAet = 0.0;	// Aet + lakeAet + riverAet	[mm]
	float lakeAet  = 0.0;	// lake aet [mm]


	dischargePre   = MFVarGetFloat (_MDInDischLevel3ID,   itemID, 0.0);
	pet	       = MFVarGetFloat (_MDInPetID,           itemID, 0.0);			 //
	aet	       = MFVarGetFloat (_MDInEvaptrsID,       itemID, 0.0);			 //
	H2OAreaFrac    = MFVarGetFloat (_MDInH2OFractionID,   itemID, 0.0);			 //
	width	       = MFVarGetFloat (_MDInRiverWidthID,    itemID, 0.0);			 //
	lakeyesno      = MFVarGetFloat (_MDInLakeYesNoID,     itemID, 0.0); //
	lakearea       = MFVarGetFloat (_MDInLakePointAreaID, itemID, 0.0); //

	dL          = MFModelGetLength (itemID) / 1000;						 	 // km converted to m

	if (lakeyesno > 0.0 || lakearea > 0.0) {

		if (lakearea > 0.0) {

			if (pet - aet > 0.0) {
				lakeAet  = discharge - ((pet - aet) / 1000 * lakearea / 86400) > 0.0 ? (pet - aet) / 1000 * lakearea / 86400 : discharge - (aet / 1000 * 14400 / 86400);	// m3/s
				lakeAet  = lakeAet * 86400 * 1000 / 14400; // mm/d
				riverAet = 0.0;	// mm/d
			}

			else {

				lakeAet = 0.0;		// mm/d				//2011
				riverAet = 0.0;		// mm/d				//2011
			}
		}

		else lakeAet = 0.0;	// mm/d					//2011

	}

	else {

	if (riverOrder > 2.0 && discharge > 0.0) { 		// RJS 03-15-09

	riverAreaFrac = (dL * width) / 14400;	// 14400 is area,  03-15-09

	combinedFrac = riverAreaFrac + H2OAreaFrac > 1.0 ? 1.0 : riverAreaFrac + H2OAreaFrac;		// RJS 03-15-09	.. removed wetlandFrac

	dischargeDepthPre = discharge / 14400 * 86400 * 1000;  // mm/day....  discharge, 14400 = area (m^2), 86400 = sec in day, 1000 = mm in m  RJS 03-15-09

		if (pet - aet - wetlandAet > 0.0) {
//		riverAet = ((pet - aet - wetlandAet) * combinedFrac) > dischargeDepthPre * 0.99 ? (pet - aet - wetlandAet) * combinedFrac : dischargeDepthPre * 0.99;	// RJS 03-15-09  when riverAet = discharge, postConc -> nan

		riverAet = dischargeDepthPre - ((pet - aet - wetlandAet) * combinedFrac) > 0.0 ? (pet - aet - wetlandAet) * combinedFrac : dischargeDepthPre;	// RJS 03-15-09
		}

	}

	}


	totalAet = aet + riverAet + lakeAet;				//

//	printf("totalAet = %f, aet = %f, wetlandAet = %f, riverAet = %f/n", totalAet, aet, wetlandAet, riverAet);

//if (lakeAet > 0.0) {temID == 13
//if (itemID == 517 || itemID == 94) {
//	printf("********* %d, m = %d, d = %d, Q = %f, lakeyesno = %f, lakearea = %f, 1 = %f, 2 = %f, 3 = %f\nriverAet = %f, lakeAet= %f, pet = %f, aet = %f, wetlandAet = %f\n", itemID, MFDateGetCurrentMonth (), MFDateGetCurrentDay (), discharge, lakeyesno, lakearea, (discharge - ((pet - aet - wetlandAet) / 1000 * lakearea / 86400)) * 86400 * 1000 / 14400, ((pet - aet - wetlandAet) / 1000 * lakearea / 86400) * 86400 * 1000 / 14400, (discharge - ((aet + wetlandAet) / 1000 * 14400 / 86400)) * 86400 * 1000 / 14400, riverAet, lakeAet, pet, aet, wetlandAet);
//	}

	MFVarSetFloat (_MDOutRiverAetID,     itemID, riverAet);			// RJS 03-15-09
	MFVarSetFloat (_MDOutLakeAetID,      itemID, lakeAet);			// 2011
	MFVarSetFloat (_MDOutTotalAetID,     itemID, totalAet);			// RJS 03-15-09

}


int MDRiverLakeAetDef() {




	MFDefEntering ("River AET");
	if (((_MDInDischLevel3ID  	   = MDDischLevel3Def ()) == CMfailed) ||
		((_MDInH2OFractionID       = MFVarGetID (MDVarH2OFracSpatial,       "-",   MFInput,  MFState, MFBoundary)) == CMfailed) ||		// RJS 03-03-09		commented out 030510
		((_MDInRiverOrderID        = MFVarGetID (MDVarRiverOrder,  			"-",   MFInput,  MFState, MFBoundary)) == CMfailed)  ||		// RJS 03-13-09
		((_MDInRiverWidthID        = MDRiverWidthDef ())   == CMfailed) ||			// RJS 03-03-09
		((_MDInPetID               = MFVarGetID (MDVarRainPotEvapotrans,  "-", MFInput, MFFlux, MFBoundary)) == CMfailed) ||		// RJS 03-03-09
	    ((_MDInEvaptrsID           = MFVarGetID (MDVarRainEvapotranspiration,     "-", MFInput, MFFlux, MFBoundary)) == CMfailed) ||	// RJS 03-03-09
	    ((_MDOutRiverAetID    	   = MFVarGetID (MDVarRiverAet,       "mm",  MFOutput, MFFlux, MFBoundary)) == CMfailed) ||		// RJS 03-03-09
	    ((_MDInLakeYesNoID         = MFVarGetID (MDVarLakeYesNo, 		     "-",   MFInput, MFState,  MFBoundary)) == CMfailed)  ||		// RJS 091108
	    ((_MDInLakePointAreaID     = MFVarGetID (MDVarLakePointArea, 		 "m2",   MFInput, MFState,  MFBoundary)) == CMfailed)  ||		// RJS 091108
	    ((_MDOutLakeAetID          = MFVarGetID (MDVarLakeAet,               "mm",   MFOutput, MFFlux, MFBoundary)) == CMfailed)  ||	// 2011
	    ((_MDOutTotalAetID   	   = MFVarGetID (MDVarTotalAet,       "mm",  MFOutput, MFFlux, MFBoundary)) == CMfailed)) 		// RJS 03-03-09
	    return (CMfailed);

	if (MFModelAddFunction(_MDRiverLakeAet) == CMfailed) return (CMfailed);
	MFDefLeaving ("River AET");
	return (_MDOutRiverAetID);
}
