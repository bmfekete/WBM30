/******************************************************************************

GHAAS Water Balance/Transport Model V2.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDPelatlandArea.c

dominik.wisser@unh.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

static int _MDWetlandAreaFracID = MFUnset;
 
static void _MDWetlandAreaFraction (int itemID) {
// TODO Not Implememnted!
//MFVarSetFloat(_MDWetlandAreaFracID,     itemID, peatllandAreaFrac);
}
enum { MDnone, MDinput, MDcalculate };

int MDWetlandAreaDef (){
	
	int  optID = MFUnset;
	const char *optStr, *optName = MDVarWetlandAreaFraction;
	const char *options [] = { MDNoneStr, MDInputStr, MDCalculateStr, (char *) NULL };
    if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
	if ((optID == MDnone) || (_MDWetlandAreaFracID != MFUnset)) return (_MDWetlandAreaFracID);

	MFDefEntering ("WetlandArea");
//	printf ("Wetland Area optID  =%i  \n",optID);
	if (_MDWetlandAreaFracID != MFUnset) return (_MDWetlandAreaFracID);
	switch (optID) {
				case MDinput:
				    if ((_MDWetlandAreaFracID       = MFVarGetID (MDVarWetlandAreaFraction,          "[-]",   MFInput, MFState, MFBoundary))  == CMfailed)
				    	return (CMfailed);
				    break;
				case MDcalculate:
					
					if ((MFModelAddFunction (_MDWetlandAreaFraction) == CMfailed)) return (CMfailed);
					break;
				default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
		MFDefLeaving("WetlandArea");
	return (_MDWetlandAreaFracID);
	
}
