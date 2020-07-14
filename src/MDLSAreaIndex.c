/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDLSAreaIndex.c

dominik.wisser@unh.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

static int _MDInCoverID          = MFUnset;
static int _MDInAtMeanID         = MFUnset;
static int _MDInCParamLPMaxID    = MFUnset;
static int _MDOutLeafAreaIndexID = MFUnset;

static void _MDLeafAreaIndex (int itemID) {
// projected leaf area index (lai) pulled out from cover dependent PET functions
// Input
	int   cover;  
	float lpMax;  // maximum projected leaf area index
	float airT;   // air temperature [degree C]
// Local
	float lai;

	cover = MFVarGetInt   (_MDInCoverID,       itemID, 7);
	airT  = MFVarGetFloat (_MDInAtMeanID,      itemID, 0.0);
	lpMax = MFVarGetFloat (_MDInCParamLPMaxID, itemID, 0.0);
	
	if (cover == 0) lai = lpMax;
	else if (airT > 8.0) lai = lpMax;
	else lai = 0.0;
	//if (itemID==104)printf ("CoverType %i  arit%f lpMax %f\n",cover, airT, lai);
 

   MFVarSetFloat (_MDOutLeafAreaIndexID,itemID,0.001 > lai ? 0.001 : lai);
}

enum { MDinput, MDstandard };

int MDLeafAreaIndexDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDVarLeafAreaIndex;
	const char *options [] = { MDInputStr, "standard", (char *) NULL };

	if (_MDOutLeafAreaIndexID != MFUnset) return (_MDOutLeafAreaIndexID);

	MFDefEntering ("Leaf Area");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);
	switch (optID) {
		case MDinput:  _MDOutLeafAreaIndexID = MFVarGetID (MDVarLeafAreaIndex, MFNoUnit, MFInput, MFState, MFBoundary); break;
		case MDstandard:
			if (((_MDInCParamLPMaxID    = MDCParamLPMaxDef ()) == CMfailed) ||
				((_MDInCoverID          = MDLandCoverDef   ()) == CMfailed) ||
				((_MDInAtMeanID         = MFVarGetID (MDVarAirTemperature, "degC",   MFInput,  MFState, MFBoundary)) == CMfailed) ||
				((_MDOutLeafAreaIndexID = MFVarGetID (MDVarLeafAreaIndex,  MFNoUnit, MFOutput, MFState, MFBoundary)) == CMfailed) ||
				(MFModelAddFunction (_MDLeafAreaIndex) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Leaf Area");
	return (_MDOutLeafAreaIndexID);
}

static int _MDInCParamCHeightID;
static int _MDOutStemAreaIndexID = MFUnset;

static void _MDStemAreaIndex (int itemID) {
// Projected Stem area index (sai) pulled out from McNaugthon and Black PET function
// Input
 	float lpMax;   // maximum projected leaf area index
	float cHeight; // canopy height [m]
// Local
	float sai;

	if (MFVarTestMissingVal (_MDInCParamLPMaxID,   itemID) ||
		 MFVarTestMissingVal (_MDInCParamCHeightID, itemID)) { MFVarSetMissingVal (_MDOutStemAreaIndexID,itemID); return; }

	lpMax   = MFVarGetFloat (_MDInCParamLPMaxID,   itemID, 0.0);
	cHeight = MFVarGetFloat (_MDInCParamCHeightID, itemID, 0.0);

	sai = lpMax > MDConstLPC ? MDConstCS * cHeight : (lpMax / MDConstLPC) * MDConstCS * cHeight;
	MFVarSetFloat (_MDOutStemAreaIndexID,itemID,sai);
}

int MDStemAreaIndexDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDVarStemAreaIndex;
	const char *options [] = { MDInputStr, "standard", (char *) NULL };

	if (_MDOutStemAreaIndexID != MFUnset) return (_MDOutStemAreaIndexID);

	MFDefEntering ("Stem Area Index");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);
	switch (optID) {
		case MDinput:  _MDOutStemAreaIndexID = MFVarGetID (MDVarStemAreaIndex, MFNoUnit, MFInput, MFState, MFBoundary); break;
		case MDstandard:
			if (((_MDInCParamLPMaxID    = MDCParamLPMaxDef   ()) == CMfailed) ||
			    ((_MDInCParamCHeightID  = MDCParamCHeightDef ()) == CMfailed) ||
			    ((_MDOutStemAreaIndexID = MFVarGetID (MDVarStemAreaIndex,  MFNoUnit, MFOutput, MFState, MFBoundary)) == CMfailed) ||
				(MFModelAddFunction (_MDStemAreaIndex) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Stem Area Index");
	return (_MDOutStemAreaIndexID);
}
