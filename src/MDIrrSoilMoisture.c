/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDIrrSoilMoisture.c

dominik.wisser@unh.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

static int _MDOutIrrSoilMoistureID = MFUnset;

enum { MDnone, MDinput, MDcalculate };

int MDIrrSoilMoistureDef() {
	int optID = MFUnset, ret;
	char *optStr, *optName = MDOptIrrigation;
	char *options [] = { MDNoneStr, MDInputStr, MDCalculateStr, (char *) NULL };

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
		
	if ((optID == MDnone) || (optID == MDinput) || (_MDOutIrrSoilMoistureID != MFUnset)) return (_MDOutIrrSoilMoistureID);

	if ((ret = MDIrrGrossDemandDef ()) == CMfailed) return (CMfailed);
	if (ret == MFUnset) return (MFUnset);
	_MDOutIrrSoilMoistureID = MFVarGetID (MDVarIrrSoilMoisture,     "mm",   MFInput, MFFlux,  MFBoundary);
    return (_MDOutIrrSoilMoistureID);
}
