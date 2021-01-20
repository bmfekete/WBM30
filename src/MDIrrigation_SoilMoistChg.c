/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

MDIrrSoilMoisture.c

dominik.wisser@unh.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

static int _MDOutIrrSoilMoistChgID = MFUnset;

enum { MDnone, MDinput, MDcalculate };

int MDIrrigation_SoilMoistChgDef() {
	int optID = MFUnset, ret;
	const char *optStr, *optName = MDOptConfig_Irrigation;
	const char *options [] = { MDNoneStr, MDInputStr, MDCalculateStr, (char *) NULL };

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
		
	if ((optID == MDnone) || (optID == MDinput) || (_MDOutIrrSoilMoistChgID != MFUnset)) return (_MDOutIrrSoilMoistChgID);

	if ((ret = MDIrrigation_GrossDemandDef()) == CMfailed) return (CMfailed);
	if (ret == MFUnset) return (MFUnset);
	_MDOutIrrSoilMoistChgID = MFVarGetID (MDVarIrrigation_SoilMoistChange, "mm", MFInput, MFFlux, MFBoundary);
    return (_MDOutIrrSoilMoistChgID);
}
