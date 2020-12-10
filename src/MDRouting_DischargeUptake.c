/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDRouting_DischargeUptake.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

// Inputs
static int _MDInRouting_DischargeInChannelID = MFUnset;
static int _MDInIrrigation_UptakeExternalID  = MFUnset;

// Outputs
static int _MDOutRouting_DischargeUptakeID  = MFUnset;
static int _MDOutIrrigation_UptakeRiverID   = MFUnset;
static int _MDOutIrrigation_UptakeExcessID  = MFUnset;

static void _MDRouting_DischargeUptake (int itemID) {
// Inputs
	float discharge;       // Discharge [m3/s]
	float irrUptakeExt;    // External irrigational water uptake [mm/dt]
// Outputs
	float irrUptakeRiver;  // Irrigational water uptake from river [mm/dt]
	float irrUptakeExcess; // Irrigational water uptake from unsustainable source [mm/dt]
	float discharge_mm;

	discharge = MFVarGetFloat (_MDInRouting_DischargeInChannelID, itemID, 0.0);

	if (_MDInIrrigation_UptakeExternalID != MFUnset) {
		irrUptakeExt = MFVarGetFloat (_MDInIrrigation_UptakeExternalID, itemID, 0.0);
		if (_MDOutIrrigation_UptakeRiverID != MFUnset) {
			// River uptake is turned on
			discharge_mm = discharge * 1000.0 * MFModelGet_dt () / MFModelGetArea (itemID);
			if (discharge_mm > irrUptakeExt) {
				// Irrigation is satisfied from the nearby river
				irrUptakeRiver  = irrUptakeExt;
				irrUptakeExcess = 0.0;
				discharge_mm    = discharge_mm - irrUptakeRiver;
			} else {
			    // Irrigation is paritally satisfied from river and the rest is from unsustainable sources
			    irrUptakeRiver  = discharge_mm;
			    irrUptakeExcess = irrUptakeExt - discharge_mm;
			    discharge_mm    = 0.0;
			}
			MFVarSetFloat (_MDOutIrrigation_UptakeRiverID,  itemID, irrUptakeRiver);
			discharge = discharge_mm * MFModelGetArea (itemID) / (1000.0 * MFModelGet_dt ());
		} else {
		    // River uptake is turned off all irrigational demand is from unsustainable sources
		    irrUptakeExcess = irrUptakeExt;
		}
		MFVarSetFloat (_MDOutIrrigation_UptakeExcessID, itemID, irrUptakeExcess);
	}
    MFVarSetFloat (_MDOutRouting_DischargeUptakeID,  itemID, discharge);
}

enum { MDnone, MDcalculate };

int MDRouting_DischargeUptake () {
	int optID = MFUnset, ret;
	const char *optStr, *optName = "IrrUptakeRiver";
	const char *options [] = { MDNoneStr, MDCalculateStr, (char *) NULL };

	if (_MDOutRouting_DischargeUptakeID != MFUnset) return (_MDOutRouting_DischargeUptakeID);

	MFDefEntering ("Discharge Level 2");
	if (((_MDInRouting_DischargeInChannelID  = MDRouting_DischargeInChannelDef()) == CMfailed) ||
        ((_MDOutRouting_DischargeUptakeID = MFVarGetID ("__DischLevel2",  "m/3", MFOutput, MFState, false)) == CMfailed))
	    return (CMfailed);
	
	if ((ret = MDIrrigation_GrossDemandDef()) != MFUnset) {
		if (ret == CMfailed) return (CMfailed);
		if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
		switch (optID) {
			case MDcalculate:
				if  ((_MDOutIrrigation_UptakeRiverID   = MDIrrigation_UptakeRiverDef()) == CMfailed) return (CMfailed);
			case MDnone:
				if (((_MDInIrrigation_UptakeExternalID = MFVarGetID (MDVarIrrigation_UptakeExternal, "mm", MFInput, MFFlux, MFBoundary)) == CMfailed) ||
                    ((_MDOutIrrigation_UptakeExcessID  = MFVarGetID (MDVarIrrigation_UptakeExcess, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed))
					return (CMfailed);
				break;
			default: MFOptionMessage (optName, optStr, options); return (CMfailed);
		}
	}
	if (MFModelAddFunction(_MDRouting_DischargeUptake) == CMfailed) return (CMfailed);
	MFDefLeaving ("Discharge Level 2");
	return (_MDOutRouting_DischargeUptakeID);
}
