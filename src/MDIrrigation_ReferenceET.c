/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDIrrRefET.c

dominink.wisser@unh.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

static int _MDOutIrrRefEvapotransID = MFUnset;

enum { MDinput, MDhamon, MDfao };

int MDIrrigation_ReferenceETDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDOptIrrigation_ReferenceET;
	const char *options [] = { MDInputStr, "Hamon", "FAO", (char *) NULL };

	if (_MDOutIrrRefEvapotransID != MFUnset) return (_MDOutIrrRefEvapotransID);

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	MFDefEntering ("Irrigation Reference Evapotranspiration");
	if (MDCore_SnowPackChgDef() == CMfailed) return (CMfailed);

	switch (optID) {
		case MDinput: _MDOutIrrRefEvapotransID = MFVarGetID (MDVarIrrigation_RefEvapotrans, "mm", MFInput, MFFlux, false); break;
		case MDhamon: _MDOutIrrRefEvapotransID = MDIrrigation_Reference_ETHamonDef(); break;
		case MDfao:   _MDOutIrrRefEvapotransID = MDIrrigation_ReferenceETFAODef(); break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Irrigation Reference Evapotranspiration");
	return (_MDOutIrrRefEvapotransID);
}
