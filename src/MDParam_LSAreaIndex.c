/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

MDLSAreaIndex.c

dominik.wisser@unh.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

static int _MDInCommon_CoverID         = MFUnset;
static int _MDInCommon_AtMeanID        = MFUnset;
static int _MDInParam_LPMaxID          = MFUnset;
static int _MDOutParam_LeafAreaIndexID = MFUnset;

static void _MDParam_LeafAreaIndex (int itemID) {
// projected leaf area index (lai) pulled out from cover dependent PET functions
// Input
	int   cover;  
	float lpMax;  // maximum projected leaf area index
	float airT;   // air temperature [degree C]
// Local
	float lai;

	cover = MFVarGetInt   (_MDInCommon_CoverID,       itemID, 7);
	airT  = MFVarGetFloat (_MDInCommon_AtMeanID,      itemID, 0.0);
	lpMax = MFVarGetFloat (_MDInParam_LPMaxID, itemID, 0.0);
	
	if (cover == 0) lai = lpMax;
	else if (airT > 8.0) lai = lpMax;
	else lai = 0.0;
	//if (itemID==104)printf ("CoverType %i  arit%f lpMax %f\n",cover, airT, lai);
 

   MFVarSetFloat (_MDOutParam_LeafAreaIndexID,itemID,0.001 > lai ? 0.001 : lai);
}

enum { MDinput, MDstandard };

int MDParam_LeafAreaIndexDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDVarCore_LeafAreaIndex;
	const char *options [] = { MDInputStr, "standard", (char *) NULL };

	if (_MDOutParam_LeafAreaIndexID != MFUnset) return (_MDOutParam_LeafAreaIndexID);

	MFDefEntering ("Leaf Area");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);
	switch (optID) {
		case MDinput:  _MDOutParam_LeafAreaIndexID = MFVarGetID (MDVarCore_LeafAreaIndex, MFNoUnit, MFInput, MFState, MFBoundary); break;
		case MDstandard:
			if (((_MDInParam_LPMaxID    = MDParam_LCLPMaxDef()) == CMfailed) ||
                ((_MDInCommon_CoverID          = MDParam_LandCoverMappingDef()) == CMfailed) ||
                ((_MDInCommon_AtMeanID         = MFVarGetID (MDVarCommon_AirTemperature, "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
                ((_MDOutParam_LeafAreaIndexID = MFVarGetID (MDVarCore_LeafAreaIndex, MFNoUnit, MFOutput, MFState, MFBoundary)) == CMfailed) ||
                (MFModelAddFunction(_MDParam_LeafAreaIndex) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Leaf Area");
	return (_MDOutParam_LeafAreaIndexID);
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

	if (MFVarTestMissingVal (_MDInParam_LPMaxID,   itemID) ||
		 MFVarTestMissingVal (_MDInCParamCHeightID, itemID)) { MFVarSetMissingVal (_MDOutStemAreaIndexID,itemID); return; }

	lpMax   = MFVarGetFloat (_MDInParam_LPMaxID,   itemID, 0.0);
	cHeight = MFVarGetFloat (_MDInCParamCHeightID, itemID, 0.0);

	sai = lpMax > MDConstLPC ? MDConstCS * cHeight : (lpMax / MDConstLPC) * MDConstCS * cHeight;
	MFVarSetFloat (_MDOutStemAreaIndexID,itemID,sai);
}

int MDParam_LCStemAreaIndexDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDVarCore_StemAreaIndex;
	const char *options [] = { MDInputStr, "standard", (char *) NULL };

	if (_MDOutStemAreaIndexID != MFUnset) return (_MDOutStemAreaIndexID);

	MFDefEntering ("Stem Area Index");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);
	switch (optID) {
		case MDinput:  _MDOutStemAreaIndexID = MFVarGetID (MDVarCore_StemAreaIndex, MFNoUnit, MFInput, MFState, MFBoundary); break;
		case MDstandard:
			if (((_MDInParam_LPMaxID    = MDParam_LCLPMaxDef()) == CMfailed) ||
                ((_MDInCParamCHeightID  = MDParam_LCHeightDef()) == CMfailed) ||
                ((_MDOutStemAreaIndexID = MFVarGetID (MDVarCore_StemAreaIndex, MFNoUnit, MFOutput, MFState, MFBoundary)) == CMfailed) ||
                (MFModelAddFunction (_MDStemAreaIndex) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Stem Area Index");
	return (_MDOutStemAreaIndexID);
}
