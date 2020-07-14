/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDDischLevel3.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

enum { MDaccumulate, MDmuskingum, MDcascade };

static int _MDDischLevel3ID = MFUnset;

int MDDischLevel3Def() {
	int optID = MFUnset;
	char *optStr, *optName = MDOptRouting;
	char *options []    = { "accumulate", "muskingum", "cascade", (char *) NULL };

	if (_MDDischLevel3ID != MFUnset) return (_MDDischLevel3ID);

	MFDefEntering ("Discharge Level 3");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDaccumulate: _MDDischLevel3ID = MDDischLevel3AccumulateDef (); break;
		case MDmuskingum:  _MDDischLevel3ID = MDDischLevel3MuskingumDef  (); break;
		case MDcascade:    _MDDischLevel3ID = MDDischLevel3CascadeDef    (); break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	if (_MDDischLevel3ID == CMfailed) return (CMfailed);
	MFDefLeaving ("Discharge Level 3");
	return (_MDDischLevel3ID);
}
