/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDPotET.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

static int _MDPotETID = MFUnset;

enum { MDinput, MDHamon, MDJensen, MDPsTaylor, MDPstd, MDPMday, MDPMdn, MDSWGday, MDSWGdn, MDTurc };

int MDRainPotETDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDVarRainPotEvapotrans;
	const char *options [] = { MDInputStr, "Hamon", "Jensen", "PsTaylor", "Pstd", "PMday", "PMdn", "SWGday", "SWGdn", "Turc", (char *) NULL };

	if (_MDPotETID != MFUnset) return (_MDPotETID);

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);
	MFDefEntering ("Rainfed Potential Evapotranspiration");

	switch (optID) {
		case MDinput:    _MDPotETID = MFVarGetID (MDVarRainPotEvapotrans, "mm", MFInput, MFFlux, false); break;
		case MDHamon:    _MDPotETID = MDRainPotETHamonDef    (); break;
		case MDJensen:   _MDPotETID = MDRainPotETJensenDef   (); break;
		case MDPsTaylor: _MDPotETID = MDRainPotETPsTaylorDef (); break;
		case MDPstd:     _MDPotETID = MDRainPotETPstdDef     (); break;
		case MDPMday:    _MDPotETID = MDRainPotETPMdayDef    (); break;
		case MDPMdn:     _MDPotETID = MDRainPotETPMdnDef     (); break;
		case MDSWGday:   _MDPotETID = MDRainPotETSWGdayDef   (); break;
		case MDSWGdn:    _MDPotETID = MDRainPotETSWGdnDef    (); break;
		case MDTurc:     _MDPotETID = MDRainPotETTurcDef     (); break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Rainfed Potential Evapotranspiration");
	return (_MDPotETID);
}
