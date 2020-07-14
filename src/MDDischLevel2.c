/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDDischLevel2.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

// Inputs
static int _MDInDischLevel3ID       = MFUnset;
static int _MDInIrrUptakeExternalID = MFUnset;

// Outputs
static int _MDOutIrrUptakeRiverID   = MFUnset;
static int _MDOutIrrUptakeExcessID  = MFUnset;
static int _MDOutDischLevel2ID      = MFUnset;

static void _MDDischLevel2 (int itemID) {
// Inputs
	float discharge;       // Discharge [m3/s]
	float irrUptakeExt;    // External irrigational water uptake [mm/dt]
// Outputs
	float irrUptakeRiver;  // Irrigational water uptake from river [mm/dt]
	float irrUptakeExcess; // Irrigational water uptake from unsustainable source [mm/dt]
	float discharge_mm;

	discharge = MFVarGetFloat (_MDInDischLevel3ID, itemID, 0.0);

	if (_MDInIrrUptakeExternalID != MFUnset) {
		irrUptakeExt = MFVarGetFloat (_MDInIrrUptakeExternalID, itemID, 0.0);
		if (_MDOutIrrUptakeRiverID != MFUnset) {
		// River uptake is turned on
			discharge_mm = discharge * 1000.0 * MFModelGet_dt () / MFModelGetArea (itemID);
			if (discharge_mm > irrUptakeExt) {
			// Irrigation is satisfied from the nearby river
				irrUptakeRiver  = irrUptakeExt;
				irrUptakeExcess = 0.0;
				discharge_mm    = discharge_mm - irrUptakeRiver;
			}
			else {
			// Irrigation is paritally satisfied from river and the rest is from unsustainable sources
				irrUptakeRiver  = discharge_mm;
				irrUptakeExcess = irrUptakeExt - discharge_mm;
				discharge_mm    = 0.0;
			}
			MFVarSetFloat (_MDOutIrrUptakeRiverID,  itemID, irrUptakeRiver);
		}
		else
		// River uptake is turned off all irrigational demand is from unsustainable sources
			irrUptakeExcess = irrUptakeExt;
		MFVarSetFloat (_MDOutIrrUptakeExcessID, itemID, irrUptakeExcess);
		discharge = discharge_mm * MFModelGetArea (itemID) / (1000.0 * MFModelGet_dt ());
	}
	MFVarSetFloat (_MDOutDischLevel2ID,  itemID, discharge);
}

enum { MDnone, MDcalculate };

int MDDischLevel2Def() {
	int optID = MFUnset, ret;
	const char *optStr, *optName = "IrrUptakeRiver";
	const char *options [] = { MDNoneStr, MDCalculateStr, (char *) NULL };

	if (_MDOutDischLevel2ID != MFUnset) return (_MDOutDischLevel2ID);

	MFDefEntering ("Discharge Level 2");
	if (((_MDInDischLevel3ID  = MDDischLevel3Def ()) == CMfailed) ||
	    ((_MDOutDischLevel2ID = MFVarGetID ("__DischLevel2",  "m/3", MFOutput, MFState, false)) == CMfailed))
	    return (CMfailed);
	
	if ((ret = MDIrrGrossDemandDef ()) != MFUnset) {
		if (ret == CMfailed) return (CMfailed);
//		if (((optStr = MFOptionGet (MDOptIrrExcessWater)) == (char *) NULL) || ((optIDExcess = CMoptLookup (options, optStr, true)) == CMfailed)) {
//				CMmsgPrint(CMmsgUsrError,"Excess Water Demand Option not specifed! Options = 'None', 'Calculated' or 'Input' (not yet implemented!)\n");
//				return (CMfailed);
//			}
		
	 
		
		if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
		switch (optID) {
			case MDcalculate:
				if  ((_MDOutIrrUptakeRiverID   = MDIrrUptakeRiverDef ()) == CMfailed) return (CMfailed);
			case MDnone:
				if (((_MDInIrrUptakeExternalID = MFVarGetID (MDVarIrrUptakeExternal, "mm",  MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
				    ((_MDOutIrrUptakeExcessID  = MFVarGetID (MDVarIrrUptakeExcess,   "mm",  MFOutput, MFFlux,  MFBoundary)) == CMfailed))
					return (CMfailed);
				break;
			default: MFOptionMessage (optName, optStr, options); return (CMfailed);
		}
	}
	if (MFModelAddFunction(_MDDischLevel2) == CMfailed) return (CMfailed);
	MFDefLeaving ("Discharge Level 2");
	return (_MDOutDischLevel2ID);
}
