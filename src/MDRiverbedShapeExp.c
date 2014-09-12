/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2014, UNH - CCNY/CUNY

MDRiverbedShapeExp.c

bfekete@ccny.cuny.edu

*******************************************************************************/

#include <stdio.h>
#include <math.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInDischMeanID              = MFUnset;
static int _MDInRiverbedSlopeID          = MFUnset;
static int _MDInEtaID                    = MFUnset;
static int _MDInNuID                     = MFUnset;
static int _MDInTauID                    = MFUnset;
static int _MDInPhiID                    = MFUnset;
static int _MDInEta2ID                    = MFUnset;
static int _MDInNu2ID                     = MFUnset;
static int _MDInTau2ID                    = MFUnset;
static int _MDInPhi2ID                    = MFUnset;
static int _MDInEta3ID                    = MFUnset;
static int _MDInNu3ID                     = MFUnset;
static int _MDInTau3ID                    = MFUnset;
static int _MDInPhi3ID                    = MFUnset;
static int _MDInBasinID			  = MFUnset;

// Output
static int _MDOutRiverbedAvgDepthMeanID  = MFUnset;
static int _MDOutRiverbedWidthMeanID     = MFUnset;
static int _MDOutRiverbedVelocityMeanID  = MFUnset;
static int _MDOutRiverbedShapeExponentID = MFUnset;

static void _MDRiverbedShapeExponent (int itemID) {
// Input
	float slope;     // Riverbed slope [m/km]
	float discharge; // Mean annual discharge [m3/s]
        float eta = 0.0;
        float nu  = 0.0;
        float tau = 0.0;
        float phi = 0.0;
// Output
	float yMean;     // River average depth at mean discharge [m]
	float wMean;     // River width at mean discharge [m]
// Local
	float dL;        // Reach length [m]
//	float eta = 0.25, nu = 0.4, tau = 8.0, phi = 0.58;		//old
//	float eta = 0.36, nu = 0.37, tau = 3.55, phi = 0.51;	//new based on Knighton (avg)
//	float eta = 0.33, nu = 0.35, tau = 3.67, phi = 0.45;	// Hey and Thorn (1986) (used for ERL 2013)

        eta = MFVarGetFloat (_MDInEtaID,   itemID, 0.0);
        nu  = MFVarGetFloat (_MDInNuID,    itemID, 0.0);
        tau = MFVarGetFloat (_MDInTauID,   itemID, 0.0);
        phi = MFVarGetFloat (_MDInPhiID,   itemID, 0.0);
        
	if (MFVarTestMissingVal (_MDInDischMeanID, itemID)) {
		MFVarSetFloat (_MDOutRiverbedAvgDepthMeanID,  itemID, 0.0);
		MFVarSetFloat (_MDOutRiverbedWidthMeanID,     itemID, 0.0);
		MFVarSetFloat (_MDOutRiverbedVelocityMeanID,  itemID, 0.0);
		MFVarSetFloat (_MDOutRiverbedShapeExponentID, itemID, 2.0);
		return;
	}
	discharge = fabs(MFVarGetFloat(_MDInDischMeanID,  itemID, 0.0));
	dL        = MFModelGetLength (itemID);
	if (CMmathEqualValues (dL, 0.0) || (_MDInRiverbedSlopeID == MFUnset) || MFVarTestMissingVal (_MDInRiverbedSlopeID, itemID)) {
		// Slope independent riverbed geometry
		yMean = eta * pow (discharge, nu);
		wMean = tau * pow (discharge, phi);
//		printf("eta = %f, nu = %f, tau = %f, phi = %f\n", eta, nu, tau, phi);

		MFVarSetFloat (_MDOutRiverbedAvgDepthMeanID,  itemID, yMean);
		MFVarSetFloat (_MDOutRiverbedWidthMeanID,     itemID, wMean);
		MFVarSetFloat (_MDOutRiverbedVelocityMeanID,  itemID, discharge / (yMean * wMean));
		MFVarSetFloat (_MDOutRiverbedShapeExponentID, itemID, 2.0);
		return;	
	}
	// Slope dependent riverbed geometry
	slope     = MFVarGetFloat(_MDInRiverbedSlopeID,      itemID, 0.01) / 1000.0;

	yMean = eta * pow (discharge, nu);
	wMean = tau * pow (discharge, phi);

 //       printf("eta = %f, nu = %f, tau = %f, phi = %f\n", eta, nu, tau, phi);
        
	MFVarSetFloat (_MDOutRiverbedAvgDepthMeanID,      itemID, yMean);
	MFVarSetFloat (_MDOutRiverbedWidthMeanID,         itemID, wMean);
	MFVarSetFloat (_MDOutRiverbedVelocityMeanID,      itemID, discharge / (yMean * wMean));
	MFVarSetFloat (_MDOutRiverbedShapeExponentID,     itemID, 2.0);
}

static void _MDMultiRiverbedShapeExponent (int itemID) {
// Input
	float slope;     // Riverbed slope [m/km]
	float discharge; // Mean annual discharge [m3/s]
        float eta  = 0.0;
        float nu   = 0.0;
        float tau  = 0.0;
        float phi  = 0.0;
	float eta2 = 0.0;
        float nu2  = 0.0;
        float tau2 = 0.0;
        float phi2 = 0.0; 
	float eta3 = 0.0;
        float nu3  = 0.0;
        float tau3 = 0.0;
        float phi3 = 0.0;
	float basinID = 0.0;
// Output
	float yMean;     // River average depth at mean discharge [m]
	float wMean;     // River width at mean discharge [m]
// Local
	float dL;        // Reach length [m]
//	float eta = 0.25, nu = 0.4, tau = 8.0, phi = 0.58;		//old
//	float eta = 0.36, nu = 0.37, tau = 3.55, phi = 0.51;	//new based on Knighton (avg)
//	float eta = 0.33, nu = 0.35, tau = 3.67, phi = 0.45;	// Hey and Thorn (1986) (used for ERL 2013)

        eta = MFVarGetFloat (_MDInEtaID,   itemID, 0.0);
        nu  = MFVarGetFloat (_MDInNuID,    itemID, 0.0);
        tau = MFVarGetFloat (_MDInTauID,   itemID, 0.0);
        phi = MFVarGetFloat (_MDInPhiID,   itemID, 0.0);
        
	basinID = MFVarGetFloat (_MDInBasinID,   itemID, 0.0);

	if (basinID == 2) {
	
	eta = MFVarGetFloat (_MDInEta2ID,   itemID, 0.0);
        nu  = MFVarGetFloat (_MDInNu2ID,    itemID, 0.0);
        tau = MFVarGetFloat (_MDInTau2ID,   itemID, 0.0);
        phi = MFVarGetFloat (_MDInPhi2ID,   itemID, 0.0);
        
	}
	
	if (basinID == 3) {
	
	eta = MFVarGetFloat (_MDInEta3ID,   itemID, 0.0);
        nu  = MFVarGetFloat (_MDInNu3ID,    itemID, 0.0);
        tau = MFVarGetFloat (_MDInTau3ID,   itemID, 0.0);
        phi = MFVarGetFloat (_MDInPhi3ID,   itemID, 0.0);
        
	}
	
	
	if (MFVarTestMissingVal (_MDInDischMeanID, itemID)) {
		MFVarSetFloat (_MDOutRiverbedAvgDepthMeanID,  itemID, 0.0);
		MFVarSetFloat (_MDOutRiverbedWidthMeanID,     itemID, 0.0);
		MFVarSetFloat (_MDOutRiverbedVelocityMeanID,  itemID, 0.0);
		MFVarSetFloat (_MDOutRiverbedShapeExponentID, itemID, 2.0);
		return;
	}
	
	discharge = fabs(MFVarGetFloat(_MDInDischMeanID,  itemID, 0.0));
	dL        = MFModelGetLength (itemID);

	yMean = eta * pow (discharge, nu);
	wMean = tau * pow (discharge, phi);

			
 //	if ((itemID == 393) || (itemID == 1374) || (itemID == 3296))   printf("basinID = %f, eta = %f, nu = %f, tau = %f, phi = %f\n", basinID, eta, nu, tau, phi);
        
	MFVarSetFloat (_MDOutRiverbedAvgDepthMeanID,      itemID, yMean);
	MFVarSetFloat (_MDOutRiverbedWidthMeanID,         itemID, wMean);
	MFVarSetFloat (_MDOutRiverbedVelocityMeanID,      itemID, discharge / (yMean * wMean));
	MFVarSetFloat (_MDOutRiverbedShapeExponentID,     itemID, 2.0);
}

enum { MDinput, MDindependent, MDdependent, MDmultibasin };

int MDRiverbedShapeExponentDef () {
	int  optID = MFUnset;
	const char *optStr, *optName = MDOptRiverbed;
	const char *options [] = { MDInputStr, "slope-independent", "slope-dependent", "multi-basin", (char *) NULL };

	if (_MDOutRiverbedShapeExponentID != MFUnset) return (_MDOutRiverbedShapeExponentID);

	MFDefEntering ("Riverbed Geometry");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDinput:
			if (((_MDOutRiverbedAvgDepthMeanID  = MFVarGetID (MDVarRiverbedAvgDepthMean,  "m",      MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutRiverbedWidthMeanID     = MFVarGetID (MDVarRiverbedWidthMean,     "m",      MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutRiverbedVelocityMeanID  = MFVarGetID (MDVarRiverbedVelocityMean,  "m/s",    MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutRiverbedShapeExponentID = MFVarGetID (MDVarRiverbedShapeExponent, MFNoUnit, MFInput,  MFState, MFBoundary)) == CMfailed))
				return (CMfailed);
			break;
		case MDdependent:
			if ((_MDInRiverbedSlopeID           = MFVarGetID (MDVarRiverbedSlope,         "m/km",   MFInput,  MFState, MFBoundary)) == CMfailed)
				return (CMfailed);
		case MDindependent:
			if (((_MDInDischMeanID = MDDischMeanDef ()) == CMfailed) ||
			    ((_MDInEtaID                    = MFVarGetID (MDVarEta,                   "-",      MFInput,  MFState, MFBoundary)) == CMfailed) ||
                            ((_MDInNuID                     = MFVarGetID (MDVarNu,                    "-",      MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInTauID                    = MFVarGetID (MDVarTau,                   "-",      MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInPhiID                    = MFVarGetID (MDVarPhi,                   "-",      MFInput,  MFState, MFBoundary)) == CMfailed) ||  
                            ((_MDOutRiverbedAvgDepthMeanID  = MFVarGetID (MDVarRiverbedAvgDepthMean,  "m",      MFOutput, MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutRiverbedWidthMeanID     = MFVarGetID (MDVarRiverbedWidthMean,     "m",      MFOutput, MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutRiverbedVelocityMeanID  = MFVarGetID (MDVarRiverbedVelocityMean,  "m/s",    MFOutput, MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutRiverbedShapeExponentID = MFVarGetID (MDVarRiverbedShapeExponent, MFNoUnit, MFOutput, MFState, MFBoundary)) == CMfailed) ||
			    (MFModelAddFunction (_MDRiverbedShapeExponent) == CMfailed)) return (CMfailed);
			break;
		case MDmultibasin:
			if (((_MDInDischMeanID = MDDischMeanDef ()) == CMfailed) ||
			    ((_MDInEtaID                   = MFVarGetID (MDVarEta,                   "-",      MFInput,  MFState, MFBoundary)) == CMfailed) ||
                            ((_MDInNuID                     = MFVarGetID (MDVarNu,                    "-",      MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInTauID                    = MFVarGetID (MDVarTau,                   "-",      MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInPhiID                    = MFVarGetID (MDVarPhi,                   "-",      MFInput,  MFState, MFBoundary)) == CMfailed) ||  
                            ((_MDInEta2ID                   = MFVarGetID (MDVarEta2,                   "-",      MFInput,  MFState, MFBoundary)) == CMfailed) ||
                            ((_MDInNu2ID                    = MFVarGetID (MDVarNu2,                    "-",      MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInTau2ID                   = MFVarGetID (MDVarTau2,                   "-",      MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInPhi2ID                   = MFVarGetID (MDVarPhi2,                   "-",      MFInput,  MFState, MFBoundary)) == CMfailed) ||  
                            ((_MDInEta3ID                   = MFVarGetID (MDVarEta3,                   "-",      MFInput,  MFState, MFBoundary)) == CMfailed) ||
                            ((_MDInNu3ID                    = MFVarGetID (MDVarNu3,                    "-",      MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInTau3ID                   = MFVarGetID (MDVarTau3,                   "-",      MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInPhi3ID                   = MFVarGetID (MDVarPhi3,                   "-",      MFInput,  MFState, MFBoundary)) == CMfailed) ||   
			    ((_MDInBasinID                  = MFVarGetID (MDVarBasinID,                "-",      MFInput,  MFState, MFBoundary)) == CMfailed) ||   
			    ((_MDOutRiverbedAvgDepthMeanID  = MFVarGetID (MDVarRiverbedAvgDepthMean,  "m",      MFOutput, MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutRiverbedWidthMeanID     = MFVarGetID (MDVarRiverbedWidthMean,     "m",      MFOutput, MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutRiverbedVelocityMeanID  = MFVarGetID (MDVarRiverbedVelocityMean,  "m/s",    MFOutput, MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutRiverbedShapeExponentID = MFVarGetID (MDVarRiverbedShapeExponent, MFNoUnit, MFOutput, MFState, MFBoundary)) == CMfailed) ||
			    (MFModelAddFunction (_MDMultiRiverbedShapeExponent) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Riverbed Geometry");
	return (_MDOutRiverbedShapeExponentID);
}
