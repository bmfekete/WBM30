/******************************************************************************
GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDThermalInputs.c

rob.stewart@unh.edu

Thermal Inputs and withdrawals

*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>
#include <math.h>


// Input
static int _MDInDischargeID     = MFUnset;
static int _MDInThermalWdlID	= MFUnset;
//static int _MDInThermalConID	= MFUnset;
static int _MDInWarmingTempID   = MFUnset;

// Output
//static int _MDWTemp_QxTID       = MFUnset;
//static int _MDWTempMixing_QxTID = MFUnset;
static int _MDOutWdl_QxTID		= MFUnset;
//static int _MDFluxMixing_QxTID	= MFUnset;

static void _MDThermalInputs (int itemID) {

	float Q;
//	float Q_post;		// discharge after consumptive use (m3/s)
	float wdl_QxT;
//	float QxT_mix;
//	float QxT_post;
//	float QxT_mix_post;
//    float Q_WTemp;
//    float Q_WTemp_mix;
//    float Q_WTemp_post;
//    float Q_WTemp_mix_post;
    float thermal_wdl;	// water withdrawals of thermal power stations (m3/s)
//    float thermal_con;	// consumptive water use of thermal power stations (m3/s)
    float warmingTemp;	// temperature which river water will be warmed (deg C)

  	Q                     = MFVarGetFloat (_MDInDischargeID,         itemID, 0.0);
    //QxT                   = MFVarGetFloat (_MDFlux_QxTID,            itemID, 0.0);
    //QxT_mix               = MFVarGetFloat (_MDFluxMixing_QxTID,      itemID, 0.0);
    //Q_WTemp				  = MFVarGetFloat (_MDWTemp_QxTID,           itemID, 0.0);
    //Q_WTemp_mix			  = MFVarGetFloat (_MDWTempMixing_QxTID,     itemID, 0.0);
    thermal_wdl			  = MFVarGetFloat (_MDInThermalWdlID,        itemID, 0.0) * 1000000 / 365 / 24 / 86400;
//	thermal_con			  = MFVarGetFloat (_MDInThermalConID,        itemID, 0.0) * 1000000 / 365 / 24 / 86400;
//	warmingTemp			  = MFVarGetFloat (_MDInWarmingTempID,       itemID, 0.0);

//	if (Q <= thermal_con) {
//		thermal_con = 0.95 * Q;
//		thermal_wdl = 0.05 * Q;
//	}

//	Q_post = Q - thermal_con;			// RJS Not factoring in consumptive water yet
//	Q_post = Q;

	wdl_QxT = thermal_wdl * warmingTemp;
	
//	QxT_post     = (thermal_wdl * (Q_WTemp + warmingTemp)) + ((Q_post - thermal_wdl) * Q_WTemp);
//	QxT_mix_post = (thermal_wdl * (Q_WTemp_mix + warmingTemp)) + ((Q_post - thermal_wdl) * Q_WTemp_mix);
//	Q_WTemp_post     = QxT_post / Q_post;
//	Q_WTemp_mix_post = QxT_mix_post / Q_post;

//	 MFVarSetFloat(_MDWTemp_QxTID,       itemID, Q_WTemp_post);
//	 MFVarSetFloat(_MDWTempMixing_QxTID, itemID, Q_WTemp_mix_post);
	 MFVarSetFloat(_MDOutWdl_QxTID,        itemID, wdl_QxT);
//	 MFVarSetFloat(_MDFluxMixing_QxTID,  itemID, QxT_mix_post);
//	 MFVarSetFloat(_MDInDischargeID,     itemID, Q_post);

}


enum { MDnone, MDinput, MDcalculate };

int MDThermalInputsDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDOptThermalInputs;
	const char *options [] = { MDNoneStr, MDInputStr, MDCalculateStr, (char *) NULL };

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);

	MFDefEntering ("Thermal Inputs");
	switch (optID) {
		case MDinput:
		case MDcalculate:
			if (((_MDInDischargeID      = MDDischLevel2Def ()) == CMfailed) ||
			    ((_MDInThermalWdlID     = MDThermalPowerPlantDef ()) == CMfailed) ||
			    ((_MDInWarmingTempID    = MFVarGetID (MDVarWarmingTemp,            "degC", MFInput, MFState, MFBoundary)) == CMfailed)	||
			    ((_MDOutWdl_QxTID	    = MFVarGetID (MDVarWithdrawal_QxT,	      "m3*degC/d", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||

			    (MFModelAddFunction (_MDThermalInputs) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Thermal Inputs");
	return (_MDOutWdl_QxTID);
}

