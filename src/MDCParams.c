/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDCParams.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

static int _MDInCoverID         = MFUnset;
static int _MDInSnowPackID      = MFUnset;

static int _MDOutCParamAlbedoID = MFUnset; 

static void _MDCParamAlbedo (int itemID) {
// Input
	int   cover;
	float snowPack;
// Local
	static float albedo []     = { 0.14, 0.18, 0.18, 0.20, 0.20, 0.22, 0.26, 0.10 };
	static float albedoSnow [] = { 0.14, 0.23, 0.35, 0.50, 0.50, 0.50, 0.50, 0.50 };

	cover    = MFVarGetInt   (_MDInCoverID,    itemID,   7); // defaulting missing value to water.
	if ((cover < 0) || (cover >= (int) (sizeof (albedo) / sizeof (albedo [0])))) {
		CMmsgPrint (CMmsgWarning,"Warning: Invalid cover [%d] in: %s:%d\n",cover,__FILE__,__LINE__);
		return;
	}
	snowPack = MFVarGetFloat (_MDInSnowPackID, itemID, 0.0);
	MFVarSetFloat (_MDOutCParamAlbedoID,itemID,snowPack > 0.0 ? albedoSnow[cover] : albedo[cover]);	
}

enum { MDinput, MDlookup };

int MDCParamAlbedoDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDVarCParamAlbedo;
	const char *options [] = { MDInputStr, MDLookupStr, (char *) NULL };

	if (_MDOutCParamAlbedoID != MFUnset) return (_MDOutCParamAlbedoID);

	MFDefEntering ("Albedo");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDinput:  _MDOutCParamAlbedoID = MFVarGetID (MDVarCParamAlbedo,  MFNoUnit, MFInput, MFState, MFBoundary); break;
		case MDlookup:
			if (((_MDInCoverID    = MDLandCoverDef ()) == CMfailed) ||
			    ((_MDInSnowPackID = MDSPackChgDef  ()) == CMfailed) ||
			    ((_MDOutCParamAlbedoID = MFVarGetID (MDVarCParamAlbedo, MFNoUnit, MFOutput, MFState, MFBoundary)) == CMfailed) ||
			    (MFModelAddFunction (_MDCParamAlbedo) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Albedo");
	return (_MDOutCParamAlbedoID); 
}

static int _MDOutCParamCHeightID = MFUnset; 

static void _MDCParamCHeight (int itemID) {
// Input
	int cover;
// Local
	static float lookup [] = { 25.0, 25.0, 8.0, 0.5, 0.3, 0.3, 0.1, 0.01}; 

	cover = MFVarGetInt (_MDInCoverID, itemID, 7); // defaulting missing value to water.
	if ((cover < 0) || (cover >= (int) (sizeof (lookup) / sizeof (lookup [0])))) {
		CMmsgPrint (CMmsgWarning ,"Warning: Invalid cover [%d] in: %s:%d\n",cover,__FILE__,__LINE__);
		return;
	}

	MFVarSetFloat (_MDOutCParamCHeightID,itemID, lookup [cover]);	
}

int MDCParamCHeightDef ()
	{
	int optID = MFUnset;
	const char *optStr, *optName = MDVarCParamCHeight;
	const char *options [] = { MDInputStr, MDLookupStr, (char *) NULL };

	if (_MDOutCParamCHeightID != MFUnset) return (_MDOutCParamCHeightID);

	MFDefEntering ("Canopy Height");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDinput:  _MDOutCParamCHeightID = MFVarGetID (MDVarCParamCHeight, "m", MFInput, MFState, MFBoundary); break;
		case MDlookup:
			if (((_MDInCoverID = MDLandCoverDef ()) == CMfailed) ||
			    ((_MDOutCParamCHeightID = MFVarGetID (MDVarCParamCHeight, "m", MFOutput, MFState, MFBoundary)) == CMfailed) ||
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

	cover = MFVarGetInt (_MDInCoverID, itemID, 7); // defaulting missing value to water.
	if ((cover < 0) || (cover >= (int) (sizeof (lookup) / sizeof (lookup [0])))) {
		CMmsgPrint (CMmsgWarning,"Warning: Invalid cover [%d] in: %s:%d\n",cover,__FILE__,__LINE__);
		return;
	}

	MFVarSetFloat (_MDOutCParamLWidthID,itemID, lookup [cover]);	
}

int MDCParamLWidthDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDVarCParamLWidth;
	const char *options [] = { MDInputStr, MDLookupStr, (char *) NULL };

	if (_MDOutCParamLWidthID != MFUnset) return (_MDOutCParamLWidthID);

	MFDefEntering ("Leaf Width");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDinput:  _MDOutCParamLWidthID = MFVarGetID (MDVarCParamLWidth,  "mm", MFInput, MFState, MFBoundary); break;
		case MDlookup:
			if (((_MDInCoverID         = MDLandCoverDef ()) == CMfailed) ||
			    ((_MDOutCParamLWidthID = MFVarGetID (MDVarCParamLWidth, "mm", MFOutput, MFState, MFBoundary)) == CMfailed) ||
			    (MFModelAddFunction (_MDCParamLWidth) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Leaf Width");
	return (_MDOutCParamLWidthID); 
}

static int _MDOutCParamRSSID = MFUnset; 

static void _MDCParamRSS (int itemID) { MFVarSetFloat (_MDOutCParamRSSID,itemID, MDConstRSS); }

int MDCParamRSSDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDVarCParamRSS;
	const char *options [] = { MDInputStr, MDLookupStr, (char *) NULL };

	if (_MDOutCParamRSSID != MFUnset) return (_MDOutCParamRSSID);

	MFDefEntering ("RSS");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDinput:  _MDOutCParamRSSID = MFVarGetID (MDVarCParamRSS,  "s/m", MFInput, MFState, MFBoundary); break;
		case MDlookup:
			if (((_MDInCoverID = MDLandCoverDef ()) == CMfailed) ||
				 ((_MDOutCParamRSSID = MFVarGetID (MDVarCParamRSS, "s/m", MFOutput, MFState, MFBoundary)) == CMfailed) ||
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

	cover = MFVarGetInt (_MDInCoverID, itemID, 7); // defaulting missing value to water.
	if ((cover < 0) || (cover >= (int) (sizeof (lookup) / sizeof (lookup [0])))) {
		CMmsgPrint (CMmsgWarning,"Warning: Invalid cover [%d] in: %s:%d\n",cover,__FILE__,__LINE__);
		return;
	}

	MFVarSetFloat (_MDOutCParamR5ID,itemID, lookup [cover]);	
}

int MDCParamR5Def () {
	int optID = MFUnset;
	const char *optStr, *optName = MDVarCParamR5;
	const char *options [] = { MDInputStr, MDLookupStr, (char *) NULL };

	if (_MDOutCParamR5ID != MFUnset) return (_MDOutCParamR5ID);

	MFDefEntering ("R5");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDinput:  _MDOutCParamR5ID = MFVarGetID (MDVarCParamR5,  "W/m2", MFInput, MFState, MFBoundary); break;
		case MDlookup:
			if (((_MDInCoverID = MDLandCoverDef ()) == CMfailed) ||
				 ((_MDOutCParamR5ID = MFVarGetID (MDVarCParamR5, "W/m2", MFOutput, MFState, MFBoundary)) == CMfailed) ||
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

	cover = MFVarGetInt (_MDInCoverID, itemID, 7); // defaulting missing value to water.
	if ((cover < 0) || (cover >= (int) (sizeof (lookup) / sizeof (lookup [0])))) {
		CMmsgPrint (CMmsgWarning,"Warning: Invalid cover [%d] in: %s:%d\n",cover,__FILE__,__LINE__);
		return;
	}
	MFVarSetFloat (_MDOutCParamCDID,itemID, lookup [cover]);	
}

int MDCParamCDDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDVarCParamCD;
	const char *options [] = { MDInputStr, MDLookupStr, (char *) NULL };

	if (_MDOutCParamCDID != MFUnset) return (_MDOutCParamCDID);

	MFDefEntering ("CD");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDinput:  _MDOutCParamCDID = MFVarGetID (MDVarCParamCD,  "kPa", MFInput, MFState, MFBoundary); break;
		case MDlookup:
			if (((_MDInCoverID = MDLandCoverDef ()) == CMfailed) ||
	    		 ((_MDOutCParamCDID = MFVarGetID (MDVarCParamCD, "kPa", MFOutput, MFState, MFBoundary)) == CMfailed) ||
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

	cover = MFVarGetInt (_MDInCoverID, itemID, 7); // defaulting missing value to water.
	if ((cover < 0) || (cover >= (int) (sizeof (lookup) / sizeof (lookup [0])))) {
		CMmsgPrint (CMmsgWarning,"Warning: Invalid cover [%d] in: %s:%d\n",cover,__FILE__,__LINE__);
		return;
	}

	MFVarSetFloat (_MDOutCParamCRID,itemID, lookup [cover]);	
}

int MDCParamCRDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDVarCParamCR;
	const char *options [] = { MDInputStr, MDLookupStr, (char *) NULL };

	if (_MDOutCParamCRID != MFUnset) return (_MDOutCParamCRID);

	MFDefEntering ("CR");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDinput:  _MDOutCParamCRID = MFVarGetID (MDVarCParamCR,  MFNoUnit, MFInput, MFState, MFBoundary); break;
		case MDlookup:
			if (((_MDInCoverID = MDLandCoverDef ()) == CMfailed) ||
	    		((_MDOutCParamCRID = MFVarGetID (MDVarCParamCR, MFNoUnit, MFOutput, MFState, MFBoundary)) == CMfailed) ||
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
	cover = MFVarGetInt (_MDInCoverID, itemID, 7); // defaulting missing value to water.
	if ((cover < 0) || (cover >= (int) (sizeof (lookup) / sizeof (lookup [0])))) {
		CMmsgPrint (CMmsgWarning,"Warning: Invalid cover [%d] in: %s:%d\n",cover,__FILE__,__LINE__);
		return;
	}

	MFVarSetFloat (_MDOutCParamGLMaxID,itemID, lookup [cover]);	
}

int MDCParamGLMaxDef ()
	{
	int optID = MFUnset;
	const char *optStr, *optName = MDVarCParamGLMax;
	const char *options [] = { MDInputStr, MDLookupStr, (char *) NULL };

	if (_MDOutCParamGLMaxID != MFUnset) return (_MDOutCParamGLMaxID);

	MFDefEntering ("GLMax");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDinput:  _MDOutCParamGLMaxID = MFVarGetID (MDVarCParamGLMax,  "m/s", MFInput, MFState, MFBoundary); break;
		case MDlookup:
			if (((_MDInCoverID = MDLandCoverDef ()) == CMfailed) ||
	   		    ((_MDOutCParamGLMaxID = MFVarGetID (MDVarCParamGLMax, "m/s", MFOutput, MFState, MFBoundary)) == CMfailed) ||
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

	cover = MFVarGetInt (_MDInCoverID, itemID, 7); // defaulting missing value to water.
	if ((cover < 0) || (cover >= (int) (sizeof (lookup) / sizeof (lookup [0])))) {
		CMmsgPrint (CMmsgWarning,"Warning: Invalid cover [%d] in: %s:%d\n",cover,__FILE__,__LINE__);
		return;
	}
	MFVarSetFloat (_MDOutCParamLPMaxID,itemID, lookup [cover]);	
}

int MDCParamLPMaxDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDVarCParamLPMax;
	const char *options [] = { MDInputStr, MDLookupStr, (char *) NULL };

	if (_MDOutCParamLPMaxID != MFUnset) return (_MDOutCParamLPMaxID);

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	MFDefEntering ("LPMax");
	switch (optID) {
		case MDinput:  _MDOutCParamLPMaxID = MFVarGetID (MDVarCParamLPMax,  MFNoUnit, MFInput, MFState, false); break;
		case MDlookup:
			if (((_MDInCoverID = MDLandCoverDef ()) == CMfailed) ||
	    	    ((_MDOutCParamLPMaxID = MFVarGetID (MDVarCParamLPMax, MFNoUnit, MFOutput, MFState, false)) == CMfailed) ||
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

	cover = MFVarGetInt (_MDInCoverID, itemID, 7); // defaulting missing value to water.
	if ((cover < 0) || (cover >= (int) (sizeof (lookup) / sizeof (lookup [0])))) {
		CMmsgPrint (CMmsgWarning,"Warning: Invalid cover [%d] in: %s:%d\n",cover,__FILE__,__LINE__);
		return;
	}
	MFVarSetFloat (_MDOutCParamZ0gID,itemID, lookup [cover]);	
}

int MDCParamZ0gDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDVarCParamZ0g;
	const char *options [] = { MDInputStr, MDLookupStr, (char *) NULL };

	if (_MDOutCParamZ0gID != MFUnset) return (_MDOutCParamZ0gID);

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	MFDefEntering ("Z0g");
	switch (optID) {
		case MDinput: _MDOutCParamZ0gID = MFVarGetID (MDVarCParamZ0g,  "m", MFInput, MFState, false); break;
		case MDlookup:
			if (((_MDInCoverID = MDLandCoverDef ()) == CMfailed) ||
	    		 ((_MDOutCParamZ0gID = MFVarGetID (MDVarCParamZ0g, "m", MFOutput, MFState, false)) == CMfailed) ||
	    		 (MFModelAddFunction (_MDCParamZ0g) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Z0g");
	return (_MDOutCParamZ0gID); 
}
