/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

MDPrecipitation.c

bfekete@gc.cuny.edu
******************************************************************************/

#include <math.h>
#include <MF.h>
#include <MD.h>

bool MDEvent (int nSteps,int nEvents,int step) {
  	bool inv = false;
	int event;
	float freq;

	if (nSteps == nEvents) return (true);

	if (nEvents > nSteps / 2) { nEvents = nSteps - nEvents; inv = true; }
		
  	freq = (float) nSteps / (float) nEvents;
	for (event = 0;event < step;++event)
		if ((int) (rint (event * freq + freq / 2.0)) == step) return (inv ? false : true);

	return (inv ? true : false);
}

static int _MDInCommon_PrecipID     = MFUnset;
static int _MDInCommon_WetDaysID    = MFUnset;
static int _MDInCommon_PrecipFracID = MFUnset;
static int _MDOutCommon_PrecipID    = MFUnset;

static void _MDPrecipWetDays (int itemID) {
// Input 
	float precipIn;
	int  wetDays;
// Output 
	float precipOut; 
// Local 
	int day, nDays; 

	day      = MFDateGetCurrentDay ();
	nDays    = MFDateGetMonthLength ();
	precipIn = MFVarGetFloat (_MDInCommon_PrecipID,  itemID,  0.0);
	wetDays  = MFVarGetInt   (_MDInCommon_WetDaysID, itemID, 31.0);

	precipOut = MDEvent (nDays,wetDays,day) ? precipIn * (float) nDays / (float) wetDays : 0.0;

	MFVarSetFloat (_MDOutCommon_PrecipID, itemID, precipOut);
}

static void _MDPrecipFraction (int itemID) {
// Input 
	float precipIn;
	float precipFrac;
// Output 
	float precipOut; 
// Local 
	int nDays    = MFDateGetMonthLength ();

	if (MFVarTestMissingVal (_MDInCommon_PrecipID,     itemID) || MFVarTestMissingVal (_MDInCommon_PrecipFracID, itemID)) {
		MFVarSetMissingVal (_MDOutCommon_PrecipID, itemID);
		return;
	}

	precipIn   = MFVarGetFloat (_MDInCommon_PrecipID,     itemID, 0.0);
	precipFrac = MFVarGetFloat (_MDInCommon_PrecipFracID, itemID, 1.0 / nDays);

	precipOut = precipIn *  precipFrac * nDays;
	if (precipOut < 0.0){ CMmsgPrint (CMmsgUsrError, "Precip negative! itemID=%d precipIn=%f precipFrac =%fprecipFrac", itemID, precipIn, precipFrac);}
	MFVarSetFloat (_MDOutCommon_PrecipID, itemID, precipOut);
}

enum { MDinput, MDwetdays, MDfraction };

int MDCommon_PrecipitationDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDVarCommon_Precipitation;
	const char *options [] = { MDInputStr, "wetdays", "fraction",(char *) NULL };

	if (_MDOutCommon_PrecipID != MFUnset) return (_MDOutCommon_PrecipID);

	MFDefEntering ("Precipitation");
	
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);
 
	switch (optID)
		{
		case MDinput: _MDOutCommon_PrecipID = MFVarGetID (MDVarCommon_Precipitation, "mm", MFInput, MFFlux, MFBoundary); break;
		case MDwetdays:
			if (((_MDInCommon_WetDaysID    = MDCommon_WetDaysDef()) == CMfailed) ||
                ((_MDInCommon_PrecipID     = MFVarGetID (MDVarCommon_PrecipMonthly, "mm", MFInput, MFFlux, MFBoundary)) == CMfailed) ||
                ((_MDOutCommon_PrecipID    = MFVarGetID (MDVarCommon_Precipitation, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
                (MFModelAddFunction (_MDPrecipWetDays) == CMfailed)) return (CMfailed);
			break;
		case MDfraction:
			if (((_MDInCommon_PrecipID     = MFVarGetID (MDVarCommon_PrecipMonthly,  "mm", MFInput, MFFlux,  MFBoundary)) == CMfailed) ||
                ((_MDInCommon_PrecipFracID = MFVarGetID (MDVarCommon_PrecipFraction, "mm", MFInput, MFState, MFBoundary)) == CMfailed) ||
                ((_MDOutCommon_PrecipID    = MFVarGetID (MDVarCommon_Precipitation,  "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
                (MFModelAddFunction (_MDPrecipFraction) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
		}
	MFDefLeaving ("Precipitation");
	return (_MDOutCommon_PrecipID);
}
