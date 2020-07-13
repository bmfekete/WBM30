/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

WBMMain.c

bfekete@gc.cuny.edu

*******************************************************************************/
#include "wbm.h"

enum { MDpet, MDsurplus, MDinfiltration, MDrunoff, MDdischarge,  MDwatertemp, MDbalance, MDgeometry, MDbgc, MDbgc_DIN, MDbgc_DINPLUSBIOMASS, MDbgc_DOC, MDfecal};

int main (int argc,char *argv []) {
	int argNum;
	int  optID = MDbalance;
	const char *optStr, *optName = MDOptModel;
	const char *options [] = { "pet", "surplus", "infiltration", "runoff", "discharge",  "watertemp", "balance", "geometry", "bgc", "bgc_DIN","bgc_DINPLUSBIOMASS", "bgc_DOC", "fecal", (char *) NULL };

	argNum = MFOptionParse (argc,argv);

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);

	switch (optID) {
		case MDpet:          return (MFModelRun (argc,argv,argNum,MDRainPotETDef));
		case MDsurplus:      return (MFModelRun (argc,argv,argNum,MDRainWaterSurplusDef));
		case MDinfiltration: return (MFModelRun (argc,argv,argNum,MDRainInfiltrationDef));
		case MDrunoff:       return (MFModelRun (argc,argv,argNum,MDRunoffDef));
		case MDdischarge:    return (MFModelRun (argc,argv,argNum,MDDischargeDef));
		case MDbalance:      return (MFModelRun (argc,argv,argNum,MDWaterBalanceDef));
		case MDwatertemp:    return (MFModelRun (argc,argv,argNum,MDWTempRiverRouteDef));
		case MDgeometry:     return (MFModelRun (argc,argv,argNum,MDRiverWidthDef));
		case MDbgc:          return (MFModelRun (argc,argv,argNum,MDBgcRoutingDef));
		case MDbgc_DOC:      return (MFModelRun (argc,argv,argNum,MDBgcDOCRoutingDef));
		case MDbgc_DIN:      return (MFModelRun (argc,argv,argNum,MDBgcDINRoutingDef));
		case MDbgc_DINPLUSBIOMASS:    return (MFModelRun (argc,argv,argNum,MDBgcDINPlusBiomassRoutingDef));
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	return (CMfailed);
}
