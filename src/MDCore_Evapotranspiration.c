/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

MDEvapotransp.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

// Input
static int _MDInRainEvapotranspID = MFUnset;
static int _MDInIrrEvapotranspID  = MFUnset;
static int _MDInSmallResEvapoID   = MFUnset;
// Output
static int _MDOutEvapotranspID    = MFUnset;

static void _MDEvapotransp (int itemID) {	
// Input
	float et=0;    // Evapotranspiration [mm/dt]
	
	et = MFVarGetFloat (_MDInRainEvapotranspID,     itemID, 0.0)
	   + (_MDInIrrEvapotranspID != MFUnset ? MFVarGetFloat (_MDInIrrEvapotranspID, itemID, 0.0) : 0.0)
	   + (_MDInSmallResEvapoID  != MFUnset ? MFVarGetFloat (_MDInSmallResEvapoID,  itemID, 0.0) : 0.0);
//	if (itemID==104)printf ("ETP hier %f irrET %f\n",et,MFVarGetFloat (_MDInIrrEvapotranspID, itemID, 0.0) );
	MFVarSetFloat (_MDOutEvapotranspID,  itemID, et);
}

int MDCore_EvapotranspirationDef () {
	int ret;
	if (_MDOutEvapotranspID != MFUnset) return (_MDOutEvapotranspID);

	MFDefEntering ("Evapotranspiration");
	if (((ret = MDIrrigation_GrossDemandDef()) != MFUnset) &&
        ((ret == CMfailed) ||
	     ((_MDInIrrEvapotranspID = MFVarGetID (MDVarIrrigation_Evapotranspiration, "mm", MFInput, MFFlux, MFBoundary)) == CMfailed)))
	     return (CMfailed);
	if (((ret = MDReservoir_FarmPondReleaseDef()) != MFUnset) &&
        ((ret == CMfailed) ||
	     ((_MDInSmallResEvapoID  = MFVarGetID (MDVarReservoir_FarmPondEvaporation, "mm", MFInput, MFFlux, MFBoundary)) == CMfailed)))
		return (CMfailed);
	if (((_MDInRainEvapotranspID = MFVarGetID (MDVarCore_RainEvapotranspiration, "mm", MFInput, MFFlux, MFBoundary)) == CMfailed) ||
		((_MDOutEvapotranspID    = MFVarGetID (MDVarCore_Evapotranspiration, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
		(MFModelAddFunction (_MDEvapotransp) == CMfailed)) return (CMfailed);
	MFDefLeaving ("Evapotranspiration");
	return (_MDOutEvapotranspID);
}
