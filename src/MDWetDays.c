/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDWetDays.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <math.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInPrecipID   = MFUnset;
static int _MDInAlphaID    = MFUnset;
static int _MDInBetaID     = MFUnset;
// Output
static int _MDOutWetDaysID = MFUnset;

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

	if (MFVarTestMissingVal (_MDInPrecipID, itemID) ||
		 MFVarTestMissingVal (_MDInAlphaID,  itemID) ||
		 MFVarTestMissingVal (_MDInBetaID,   itemID)) { MFVarSetMissingVal (_MDOutWetDaysID,itemID); return; }

	precip = MFVarGetFloat (_MDInPrecipID, itemID, 0.0);
	alpha  = MFVarGetFloat (_MDInAlphaID,  itemID, 1.0);
	beta   = MFVarGetFloat (_MDInBetaID,   itemID, 0.0);

	nDays   = MFDateGetMonthLength ();
	wetDays = (int) ((float) nDays * alpha * (1.0 - exp ((double) (beta * precip))));
	if (wetDays > nDays) wetDays = nDays;
	if (wetDays < 1)     wetDays = 1;

	MFVarSetInt (_MDOutWetDaysID,itemID,wetDays);
	}

enum { MDinput, MDlbg };

int MDWetDaysDef ()
	{
	int optID = MFUnset;
	char *optStr, *optName = MDVarWetDays;
	char *options [] = { MDInputStr, "LBG", (char *) NULL };

	if (_MDOutWetDaysID != MFUnset) return (_MDOutWetDaysID);

	MFDefEntering ("Wet Days");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID)
		{
		case MDinput: _MDOutWetDaysID = MFVarGetID (MDVarWetDays,   MFNoUnit, MFInput,  MFState, MFBoundary); break;
		case MDlbg:
			if (((_MDInPrecipID   = MFVarGetID (MDVarPrecipMonthly, "mm",     MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
			    ((_MDInAlphaID    = MFVarGetID (MDVarWetDaysAlpha,  MFNoUnit, MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInBetaID     = MFVarGetID (MDVarWetDaysBeta,   MFNoUnit, MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutWetDaysID = MFVarGetID (MDVarWetDays,       MFNoUnit, MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
			    (MFModelAddFunction (_MDWetDays) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
		}
	MFDefLeaving ("Wet Days");
	return (_MDOutWetDaysID);
	}
