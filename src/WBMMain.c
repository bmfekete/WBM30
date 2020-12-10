/******************************************************************************

GHAAS Water Balance/Transport Model
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
    const char *optStr, *optName = MDOptConfig_Model;
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
        case MDpet:             return (MFModelRun(argc, argv, argNum, MDCore_RainPotETDef));
        case MDsurplus:         return (MFModelRun(argc, argv, argNum, MDCore_RainWaterSurplusDef));
        case MDinfiltration:    return (MFModelRun(argc, argv, argNum, MDCore_RainInfiltrationDef));
        case MDrunoff:          return (MFModelRun(argc, argv, argNum, MDCore_RunoffDef));
        case MDdischarge:       return (MFModelRun(argc, argv, argNum, MDRouting_DischargeDef));
        case MDbankfullQcalc:   return (MFModelRun(argc, argv, argNum, MDSediment_BankfullQcalcDef));
        case MDRiverbedShape:   return (MFModelRun(argc, argv, argNum, MDRouting_RiverbedShapeExponentDef));
        case MDgeometry:        return (MFModelRun(argc, argv, argNum, MDRouting_RiverWidthDef));
        case MDbalance:         return (MFModelRun(argc, argv, argNum, MDCore_WaterBalanceDef));
        case MDwatertemp:       return (MFModelRun(argc, argv, argNum, MDTP2M_WTempRiverRouteDef));
        case MDthermal:         return (MFModelRun(argc, argv, argNum, MDTP2M_ThermalInputsDef));
        case MDBQARTpreprocess: return (MFModelRun(argc, argv, argNum, MDSediment_BQARTpreprocessDef));
        case MDsedimentflux:    return (MFModelRun(argc, argv, argNum, MDSediment_FluxDef));
        case MDbedloadflux:     return (MFModelRun(argc, argv, argNum, MDSediment_BedloadFluxDef));
    }
    MFOptionMessage(optName, optStr, options);
    return (CMfailed);
}
