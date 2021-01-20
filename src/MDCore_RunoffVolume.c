/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

MDRunoffVolume.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

// Input
static int _MDInCore_RunoffID        = MFUnset;
// Output
static int _MDOutRunoffVolumeID = MFUnset;

static void _MDRunoffVolume (int itemID) {
// Input
	float runoff;

	runoff = MFVarGetFloat (_MDInCore_RunoffID, itemID, 0.0) * MFModelGetArea (itemID) / (MFModelGet_dt () * 1000.0);
//	if((itemID == 25014) && (runoff * 86400 < -0.000009)) printf("############ runoff = %f\n", runoff * 86400);  //runoff = 0.0;							//RJS 071511
	MFVarSetFloat (_MDOutRunoffVolumeID, itemID, runoff);
}
 
enum { MDinput, MDcalculate };

int MDCore_RunoffVolumeDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDVarCore_RunoffVolume;
	const char *options [] = { MDInputStr, MDCalculateStr, (char *) NULL };

	if (_MDOutRunoffVolumeID != MFUnset) return (_MDOutRunoffVolumeID);

	MFDefEntering ("Runoff Volume");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
	switch (optID) {
		case MDinput:
			_MDOutRunoffVolumeID = MFVarGetID (MDVarCore_RunoffVolume, "m3/s", MFInput, MFState, MFBoundary);
			break;
		case MDcalculate:
			if (((_MDInCore_RunoffID        = MDCore_RunoffDef()) == CMfailed) ||
                ((_MDOutRunoffVolumeID = MFVarGetID (MDVarCore_RunoffVolume, "m3/s", MFOutput, MFState, MFBoundary)) == CMfailed) ||
                (MFModelAddFunction (_MDRunoffVolume) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving  ("Runoff Volume");
	return (_MDOutRunoffVolumeID);
}
