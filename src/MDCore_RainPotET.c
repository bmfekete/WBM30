/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

MDPotET.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

static int _MDPotETID = MFUnset;

enum { MDinput, MDHamon, MDJensen, MDPsTaylor, MDPstd, MDPMday, MDPMdn, MDSWGday, MDSWGdn, MDTurc };

int MDCore_RainPotETDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDVarCore_RainPotEvapotrans;
	const char *options [] = { MDInputStr, "Hamon", "Jensen", "PsTaylor", "Pstd", "PMday", "PMdn", "SWGday", "SWGdn", "Turc", (char *) NULL };

	if (_MDPotETID != MFUnset) return (_MDPotETID);

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);
	MFDefEntering ("Rainfed Potential Evapotranspiration");

	switch (optID) {
		case MDinput:    _MDPotETID = MFVarGetID (MDVarCore_RainPotEvapotrans, "mm", MFInput, MFFlux, false); break;
		case MDHamon:    _MDPotETID = MDCore_RainPotETHamonDef(); break;
		case MDJensen:   _MDPotETID = MDCore_RainPotETJensenDef(); break;
		case MDPsTaylor: _MDPotETID = MDCore_RainPotETPsTaylorDef(); break;
		case MDPstd:     _MDPotETID = MDCore_RainPotETPstdDef(); break;
		case MDPMday:    _MDPotETID = MDCore_RainPotETPMdayDef(); break;
		case MDPMdn:     _MDPotETID = MDCore_RainPotETPMdnDef(); break;
		case MDSWGday:   _MDPotETID = MDCore_RainPotETSWGdayDef(); break;
		case MDSWGdn:    _MDPotETID = MDCore_RainPotETSWGdnDef(); break;
		case MDTurc:     _MDPotETID = MDCore_RainPotETTurcDef(); break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Rainfed Potential Evapotranspiration");
	return (_MDPotETID);
}
