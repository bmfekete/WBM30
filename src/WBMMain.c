/******************************************************************************

GHAAS Water Balance/Transport Model V2.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

WBMMain.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include "wbm.h"

typedef enum { MDpet,
    MDsurplus,
    MDinfiltration,
    MDrunoff,
    MDdischarge,
    MDbankfullQcalc,
    MDRiverbedShape,
    MDgeometry,
    MDbalance,
    MDwatertemp,
    MDthermal,
    MDBQARTpreprocess,
    MDsedimentflux,
    MDbedloadflux} MDoption;

int main (int argc,char *argv []) {
    int argNum;
    int optID = MDbalance;
    const char *optStr, *optName = MDOptModel;
    const char *options[] = {"pet",
                             "surplus",
                             "infiltration",
                             "runoff",
                             "discharge",
                             "bankfullQcalc",
                             "riverbedshape",
                             "geometry",
                             "balance",
                             "watertemp",
                             "thermal",
                             "BQARTpreprocess",
                             "BQARTinputs",
                             "sedimentflux",
                             "bedloadflux",
                             (char *) NULL};

    argNum = MFOptionParse(argc, argv);

    if ((optStr = MFOptionGet(optName)) != (char *) NULL) optID = CMoptLookup(options, optStr, true);

    switch (optID) {
        case MDpet:             return (MFModelRun(argc, argv, argNum, MDRainPotETDef));
        case MDsurplus:         return (MFModelRun(argc, argv, argNum, MDRainWaterSurplusDef));
        case MDinfiltration:    return (MFModelRun(argc, argv, argNum, MDRainInfiltrationDef));
        case MDrunoff:          return (MFModelRun(argc, argv, argNum, MDRunoffDef));
        case MDdischarge:       return (MFModelRun(argc, argv, argNum, MDDischargeDef));
        case MDbankfullQcalc:   return (MFModelRun(argc, argv, argNum, MDBankfullQcalcDef));
        case MDRiverbedShape:   return (MFModelRun(argc, argv, argNum, MDRiverbedShapeExponentDef));
        case MDgeometry:        return (MFModelRun(argc, argv, argNum, MDRiverWidthDef));
        case MDbalance:         return (MFModelRun(argc, argv, argNum, MDWaterBalanceDef));
        case MDwatertemp:       return (MFModelRun(argc, argv, argNum, MDWTempRiverRouteDef));
        case MDthermal:         return (MFModelRun(argc, argv, argNum, MDThermalInputs3Def));
        case MDBQARTpreprocess: return (MFModelRun(argc, argv, argNum, MDBQARTpreprocessDef));
        case MDsedimentflux:    return (MFModelRun(argc, argv, argNum, MDSedimentFluxDef));
        case MDbedloadflux:     return (MFModelRun(argc, argv, argNum, MDBedloadFluxDef));
    }
    MFOptionMessage(optName, optStr, options);
    return (CMfailed);
}
