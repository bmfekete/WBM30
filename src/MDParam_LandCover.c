/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDParam_LandCover.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

// Input
static int _MDInCommon_CoverID    = MFUnset;
static int _MDInCommon_SnowPackID = MFUnset;
// Output
static int _MDOutParam_AlbedoID   = MFUnset;

static void _MDParam_Albedo (int itemID) {
// Input
	int   cover;
	float snowPack;
// Local
	static float albedo []     = { 0.14, 0.18, 0.18, 0.20, 0.20, 0.22, 0.26, 0.10 };
	static float albedoSnow [] = { 0.14, 0.23, 0.35, 0.50, 0.50, 0.50, 0.50, 0.50 };

	cover    = MFVarGetInt   (_MDInCommon_CoverID,    itemID,   7); // defaulting missing value to water.
	if ((cover < 0) || (cover >= (int) (sizeof (albedo) / sizeof (albedo [0])))) {
		CMmsgPrint (CMmsgWarning,"Warning: Invalid cover [%d] in: %s:%d\n",cover,__FILE__,__LINE__);
		return;
	}
	snowPack = MFVarGetFloat (_MDInCommon_SnowPackID, itemID, 0.0);
	MFVarSetFloat (_MDOutParam_AlbedoID,itemID,snowPack > 0.0 ? albedoSnow[cover] : albedo[cover]);	
}

enum { MDinput, MDlookup };

int MDParam_LCAlbedoDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDVarParam_Albedo;
	const char *options [] = { MDInputStr, MDLookupStr, (char *) NULL };

	if (_MDOutParam_AlbedoID != MFUnset) return (_MDOutParam_AlbedoID);

	MFDefEntering ("Albedo");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDinput:  _MDOutParam_AlbedoID = MFVarGetID (MDVarParam_Albedo, MFNoUnit, MFInput, MFState, MFBoundary); break;
		case MDlookup:
			if (((_MDInCommon_CoverID    = MDParam_LandCoverMappingDef()) == CMfailed) ||
                ((_MDInCommon_SnowPackID = MDCore_SnowPackChgDef()) == CMfailed) ||
                ((_MDOutParam_AlbedoID = MFVarGetID (MDVarParam_Albedo, MFNoUnit, MFOutput, MFState, MFBoundary)) == CMfailed) ||
                (MFModelAddFunction(_MDParam_Albedo) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Albedo");
	return (_MDOutParam_AlbedoID); 
}

static int _MDOutCParamCHeightID = MFUnset; 

static void _MDCParamCHeight (int itemID) {
// Input
	int cover;
// Local
	static float lookup [] = { 25.0, 25.0, 8.0, 0.5, 0.3, 0.3, 0.1, 0.01}; 

	cover = MFVarGetInt (_MDInCommon_CoverID, itemID, 7); // defaulting missing value to water.
	if ((cover < 0) || (cover >= (int) (sizeof (lookup) / sizeof (lookup [0])))) {
		CMmsgPrint (CMmsgWarning ,"Warning: Invalid cover [%d] in: %s:%d\n",cover,__FILE__,__LINE__);
		return;
	}

	MFVarSetFloat (_MDOutCParamCHeightID,itemID, lookup [cover]);	
}

int MDParam_LCHeightDef ()
	{
	int optID = MFUnset;
	const char *optStr, *optName = MDVarParam_CHeight;
	const char *options [] = { MDInputStr, MDLookupStr, (char *) NULL };

	if (_MDOutCParamCHeightID != MFUnset) return (_MDOutCParamCHeightID);

	MFDefEntering ("Canopy Height");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDinput:  _MDOutCParamCHeightID = MFVarGetID (MDVarParam_CHeight, "m", MFInput, MFState, MFBoundary); break;
		case MDlookup:
			if (((_MDInCommon_CoverID = MDParam_LandCoverMappingDef()) == CMfailed) ||
                ((_MDOutCParamCHeightID = MFVarGetID (MDVarParam_CHeight, "m", MFOutput, MFState, MFBoundary)) == CMfailed) ||
                (MFModelAddFunction (_MDCParamCHeight) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Canopy Height");
	return (_MDOutCParamCHeightID); 
}

static int _MDOutCParamLWidthID = MFUnset; 

static void _MDCParamLWidth (int itemID) {
// Input
	int cover;
// Local
	static float lookup [] = { 0.004,0.1,  0.03, 0.01, 0.01, 0.1,  0.02, 0.001};

	cover = MFVarGetInt (_MDInCommon_CoverID, itemID, 7); // defaulting missing value to water.
	if ((cover < 0) || (cover >= (int) (sizeof (lookup) / sizeof (lookup [0])))) {
		CMmsgPrint (CMmsgWarning,"Warning: Invalid cover [%d] in: %s:%d\n",cover,__FILE__,__LINE__);
		return;
	}

	MFVarSetFloat (_MDOutCParamLWidthID,itemID, lookup [cover]);	
}

int MDParam_LCLeafWidthDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDVarParam_LWidth;
	const char *options [] = { MDInputStr, MDLookupStr, (char *) NULL };

	if (_MDOutCParamLWidthID != MFUnset) return (_MDOutCParamLWidthID);

	MFDefEntering ("Leaf Width");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDinput:  _MDOutCParamLWidthID = MFVarGetID (MDVarParam_LWidth, "mm", MFInput, MFState, MFBoundary); break;
		case MDlookup:
			if (((_MDInCommon_CoverID         = MDParam_LandCoverMappingDef()) == CMfailed) ||
                ((_MDOutCParamLWidthID = MFVarGetID (MDVarParam_LWidth, "mm", MFOutput, MFState, MFBoundary)) == CMfailed) ||
                (MFModelAddFunction (_MDCParamLWidth) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Leaf Width");
	return (_MDOutCParamLWidthID); 
}

static int _MDOutCParamRSSID = MFUnset; 

static void _MDCParamRSS (int itemID) { MFVarSetFloat (_MDOutCParamRSSID,itemID, MDConstRSS); }

int MDParam_LCRSSDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDVarParam_RSS;
	const char *options [] = { MDInputStr, MDLookupStr, (char *) NULL };

	if (_MDOutCParamRSSID != MFUnset) return (_MDOutCParamRSSID);

	MFDefEntering ("RSS");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDinput:  _MDOutCParamRSSID = MFVarGetID (MDVarParam_RSS, "s/m", MFInput, MFState, MFBoundary); break;
		case MDlookup:
			if (((_MDInCommon_CoverID = MDParam_LandCoverMappingDef()) == CMfailed) ||
                ((_MDOutCParamRSSID = MFVarGetID (MDVarParam_RSS, "s/m", MFOutput, MFState, MFBoundary)) == CMfailed) ||
                (MFModelAddFunction (_MDCParamRSS) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("RSS");
	return (_MDOutCParamRSSID); 
}

static int _MDOutCParamR5ID = MFUnset; 

static void _MDCParamR5 (int itemID) {
// Input
	int cover;
// Local
	static float lookup []     = { 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 10.0 };

	cover = MFVarGetInt (_MDInCommon_CoverID, itemID, 7); // defaulting missing value to water.
	if ((cover < 0) || (cover >= (int) (sizeof (lookup) / sizeof (lookup [0])))) {
		CMmsgPrint (CMmsgWarning,"Warning: Invalid cover [%d] in: %s:%d\n",cover,__FILE__,__LINE__);
		return;
	}

	MFVarSetFloat (_MDOutCParamR5ID,itemID, lookup [cover]);	
}

int MDParam_LCR5Def () {
	int optID = MFUnset;
	const char *optStr, *optName = MDVarParam_R5;
	const char *options [] = { MDInputStr, MDLookupStr, (char *) NULL };

	if (_MDOutCParamR5ID != MFUnset) return (_MDOutCParamR5ID);

	MFDefEntering ("R5");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDinput:  _MDOutCParamR5ID = MFVarGetID (MDVarParam_R5, "W/m2", MFInput, MFState, MFBoundary); break;
		case MDlookup:
			if (((_MDInCommon_CoverID = MDParam_LandCoverMappingDef()) == CMfailed) ||
                ((_MDOutCParamR5ID = MFVarGetID (MDVarParam_R5, "W/m2", MFOutput, MFState, MFBoundary)) == CMfailed) ||
                (MFModelAddFunction (_MDCParamR5) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("R5");
	return (_MDOutCParamR5ID); 
}

static int _MDOutCParamCDID = MFUnset; 

static void _MDCParamCD (int itemID) {
// Input
	int cover;
// Local
	static float lookup []     = { 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 0.10 };

	cover = MFVarGetInt (_MDInCommon_CoverID, itemID, 7); // defaulting missing value to water.
	if ((cover < 0) || (cover >= (int) (sizeof (lookup) / sizeof (lookup [0])))) {
		CMmsgPrint (CMmsgWarning,"Warning: Invalid cover [%d] in: %s:%d\n",cover,__FILE__,__LINE__);
		return;
	}
	MFVarSetFloat (_MDOutCParamCDID,itemID, lookup [cover]);	
}

int MDParam_LCCDDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDVarParam_CD;
	const char *options [] = { MDInputStr, MDLookupStr, (char *) NULL };

	if (_MDOutCParamCDID != MFUnset) return (_MDOutCParamCDID);

	MFDefEntering ("CD");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDinput:  _MDOutCParamCDID = MFVarGetID (MDVarParam_CD, "kPa", MFInput, MFState, MFBoundary); break;
		case MDlookup:
			if (((_MDInCommon_CoverID = MDParam_LandCoverMappingDef()) == CMfailed) ||
                ((_MDOutCParamCDID = MFVarGetID (MDVarParam_CD, "kPa", MFOutput, MFState, MFBoundary)) == CMfailed) ||
                (MFModelAddFunction (_MDCParamCD) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("CD");
	return (_MDOutCParamCDID); 
}

static int _MDOutCParamCRID = MFUnset; 

static void _MDCParamCR (int itemID) {
// Input
	int cover;
// Local
	static float lookup [] = { 0.5, 0.6, 0.6, 0.7, 0.7, 0.7, 0.7, 0.01 };

	cover = MFVarGetInt (_MDInCommon_CoverID, itemID, 7); // defaulting missing value to water.
	if ((cover < 0) || (cover >= (int) (sizeof (lookup) / sizeof (lookup [0])))) {
		CMmsgPrint (CMmsgWarning,"Warning: Invalid cover [%d] in: %s:%d\n",cover,__FILE__,__LINE__);
		return;
	}

	MFVarSetFloat (_MDOutCParamCRID,itemID, lookup [cover]);	
}

int MDParam_LCCRDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDVarParam_CR;
	const char *options [] = { MDInputStr, MDLookupStr, (char *) NULL };

	if (_MDOutCParamCRID != MFUnset) return (_MDOutCParamCRID);

	MFDefEntering ("CR");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDinput:  _MDOutCParamCRID = MFVarGetID (MDVarParam_CR, MFNoUnit, MFInput, MFState, MFBoundary); break;
		case MDlookup:
			if (((_MDInCommon_CoverID = MDParam_LandCoverMappingDef()) == CMfailed) ||
                ((_MDOutCParamCRID = MFVarGetID (MDVarParam_CR, MFNoUnit, MFOutput, MFState, MFBoundary)) == CMfailed) ||
                (MFModelAddFunction (_MDCParamCR) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("CR");
	return (_MDOutCParamCRID); 
}

static int _MDOutCParamGLMaxID = MFUnset; 

static void _MDCParamGLMax (int itemID) {
// Input
	int cover;
// Local
	static float lookup []     = { 0.0053, 0.0053, 0.0053, 0.008, 0.0066, 0.011, 0.005, 0.001 };
//in m/s !!!!!!!!!!!!!!
	cover = MFVarGetInt (_MDInCommon_CoverID, itemID, 7); // defaulting missing value to water.
	if ((cover < 0) || (cover >= (int) (sizeof (lookup) / sizeof (lookup [0])))) {
		CMmsgPrint (CMmsgWarning,"Warning: Invalid cover [%d] in: %s:%d\n",cover,__FILE__,__LINE__);
		return;
	}

	MFVarSetFloat (_MDOutCParamGLMaxID,itemID, lookup [cover]);	
}

int MDParam_LCGLMaxDef ()
	{
	int optID = MFUnset;
	const char *optStr, *optName = MDVarParam_GLMax;
	const char *options [] = { MDInputStr, MDLookupStr, (char *) NULL };

	if (_MDOutCParamGLMaxID != MFUnset) return (_MDOutCParamGLMaxID);

	MFDefEntering ("GLMax");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDinput:  _MDOutCParamGLMaxID = MFVarGetID (MDVarParam_GLMax, "m/s", MFInput, MFState, MFBoundary); break;
		case MDlookup:
			if (((_MDInCommon_CoverID = MDParam_LandCoverMappingDef()) == CMfailed) ||
                ((_MDOutCParamGLMaxID = MFVarGetID (MDVarParam_GLMax, "m/s", MFOutput, MFState, MFBoundary)) == CMfailed) ||
                (MFModelAddFunction (_MDCParamGLMax) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("GLMax");
	return (_MDOutCParamGLMaxID); 
}

static int _MDOutCParamLPMaxID = MFUnset; 

static void _MDCParamLPMax (int itemID) {
// Input
	int cover;
// Local
	static float lookup []     = { 6, 6, 3, 3, 4, 3, 1, 0.00001 };

	cover = MFVarGetInt (_MDInCommon_CoverID, itemID, 7); // defaulting missing value to water.
	if ((cover < 0) || (cover >= (int) (sizeof (lookup) / sizeof (lookup [0])))) {
		CMmsgPrint (CMmsgWarning,"Warning: Invalid cover [%d] in: %s:%d\n",cover,__FILE__,__LINE__);
		return;
	}
	MFVarSetFloat (_MDOutCParamLPMaxID,itemID, lookup [cover]);	
}

int MDParam_LCLPMaxDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDVarParam_LPMax;
	const char *options [] = { MDInputStr, MDLookupStr, (char *) NULL };

	if (_MDOutCParamLPMaxID != MFUnset) return (_MDOutCParamLPMaxID);

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	MFDefEntering ("LPMax");
	switch (optID) {
		case MDinput:  _MDOutCParamLPMaxID = MFVarGetID (MDVarParam_LPMax, MFNoUnit, MFInput, MFState, false); break;
		case MDlookup:
			if (((_MDInCommon_CoverID = MDParam_LandCoverMappingDef()) == CMfailed) ||
                ((_MDOutCParamLPMaxID = MFVarGetID (MDVarParam_LPMax, MFNoUnit, MFOutput, MFState, false)) == CMfailed) ||
                (MFModelAddFunction (_MDCParamLPMax) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("LPMax");
	return (_MDOutCParamLPMaxID); 
}

static int _MDOutCParamZ0gID = MFUnset; 

static void _MDCParamZ0g (int itemID) {
// Input
	int cover;
// Local
	static float lookup []     = { 0.02, 0.02, 0.02, 0.01, 0.01, 0.005, 0.001, 0.001 };

	cover = MFVarGetInt (_MDInCommon_CoverID, itemID, 7); // defaulting missing value to water.
	if ((cover < 0) || (cover >= (int) (sizeof (lookup) / sizeof (lookup [0])))) {
		CMmsgPrint (CMmsgWarning,"Warning: Invalid cover [%d] in: %s:%d\n",cover,__FILE__,__LINE__);
		return;
	}
	MFVarSetFloat (_MDOutCParamZ0gID,itemID, lookup [cover]);	
}

int MDParam_LCZ0gDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDVarParam_Z0g;
	const char *options [] = { MDInputStr, MDLookupStr, (char *) NULL };

	if (_MDOutCParamZ0gID != MFUnset) return (_MDOutCParamZ0gID);

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	MFDefEntering ("Z0g");
	switch (optID) {
		case MDinput: _MDOutCParamZ0gID = MFVarGetID (MDVarParam_Z0g, "m", MFInput, MFState, false); break;
		case MDlookup:
			if (((_MDInCommon_CoverID = MDParam_LandCoverMappingDef()) == CMfailed) ||
                ((_MDOutCParamZ0gID = MFVarGetID (MDVarParam_Z0g, "m", MFOutput, MFState, false)) == CMfailed) ||
                (MFModelAddFunction (_MDCParamZ0g) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Z0g");
	return (_MDOutCParamZ0gID); 
}
