/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

MDWetDays.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <math.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInCommon_PrecipID      = MFUnset;
static int _MDInParam_WetDaysAlphaID = MFUnset;
static int _MDInParam_WetDaysBetaID  = MFUnset;
// Output
static int _MDOutCommon_WetDaysID = MFUnset;

static void _MDWetDays (int itemID)
	{
// Input
	float precip;
	float alpha;
	float beta;
// Output 
	int wetDays;
// Local 
	int nDays;

	if (MFVarTestMissingVal (_MDInCommon_PrecipID, itemID) ||
        MFVarTestMissingVal (_MDInParam_WetDaysAlphaID, itemID) ||
        MFVarTestMissingVal (_MDInParam_WetDaysBetaID, itemID)) { MFVarSetMissingVal (_MDOutCommon_WetDaysID, itemID); return; }

	precip = MFVarGetFloat (_MDInCommon_PrecipID, itemID, 0.0);
	alpha  = MFVarGetFloat (_MDInParam_WetDaysAlphaID, itemID, 1.0);
	beta   = MFVarGetFloat (_MDInParam_WetDaysBetaID, itemID, 0.0);

	nDays   = MFDateGetMonthLength ();
	wetDays = (int) ((float) nDays * alpha * (1.0 - exp ((double) (beta * precip))));
	if (wetDays > nDays) wetDays = nDays;
	if (wetDays < 1)     wetDays = 1;

	MFVarSetInt (_MDOutCommon_WetDaysID,itemID,wetDays);
	}

enum { MDinput, MDlbg };

int MDCommon_WetDaysDef ()
	{
	int optID = MFUnset;
	const char *optStr, *optName = MDVarCommon_WetDays;
	const char *options [] = { MDInputStr, "LBG", (char *) NULL };

	if (_MDOutCommon_WetDaysID != MFUnset) return (_MDOutCommon_WetDaysID);

	MFDefEntering ("Wet Days");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID)
		{
		case MDinput: _MDOutCommon_WetDaysID = MFVarGetID (MDVarCommon_WetDays, MFNoUnit, MFInput, MFState, MFBoundary); break;
		case MDlbg:
			if (((_MDInCommon_PrecipID      = MFVarGetID (MDVarCommon_PrecipMonthly, "mm",     MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
                ((_MDInParam_WetDaysAlphaID = MFVarGetID (MDVarParam_WetDaysAlpha,   MFNoUnit, MFInput,  MFState, MFBoundary)) == CMfailed) ||
                ((_MDInParam_WetDaysBetaID  = MFVarGetID (MDVarParam_WetDaysBeta,    MFNoUnit, MFInput,  MFState, MFBoundary)) == CMfailed) ||
                ((_MDOutCommon_WetDaysID    = MFVarGetID (MDVarCommon_WetDays,       MFNoUnit, MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
				(MFModelAddFunction (_MDWetDays) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
		}
	MFDefLeaving ("Wet Days");
	return (_MDOutCommon_WetDaysID);
	}
