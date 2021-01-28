/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

WBMMain.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include "wbm.h"

typedef enum { MDpet,
               MDsurplus,
               MDinfiltration,
               MDrunoff,
               MDdischarge,
               MDwatertemp,
               MDthermal,
               MDbalance,
               MDgeometry,
               MDbankfullQcalc,
               MDRiverbedShape,
               MDsedimentflux,
               MDsedimentflux_noBF,
               MDbedloadflux,
               MDBQARTinputs,
               MDBQARTpreprocess,
               MDparticulatenutrients,
               MDparticulatenutrients_noBF,
               MDwaterdensity} MDoption;

int main (int argc,char *argv []) {
    int argNum;
    int optID = MDbalance;
    const char *optStr, *optName = MDOptConfig_Model;
    const char *options[] = { "pet",
                              "surplus",
                              "infiltration",
                              "runoff",
                              "discharge", 
                              "watertemp",
                              "thermal",
                              "balance",
                              "geometry",
                              "bankfullQcalc",
                              "riverbedshape",
                              "sedimentflux",
                              "sedimentflux_nobf",
                              "bedloadflux",
                              "BQARTinputs" ,
                              "BQARTpreprocess",
                              "particulatenutrients",
                              "particulatenutrients_nobf",
                              "waterdensity", (char *) NULL};

    argNum = MFOptionParse(argc, argv);

    if ((optStr = MFOptionGet(optName)) != (char *) NULL) optID = CMoptLookup(options, optStr, true);

    switch (optID) {
        case MDpet:                       return (MFModelRun(argc, argv, argNum, MDCore_RainPotETDef));
        case MDsurplus:                   return (MFModelRun(argc, argv, argNum, MDCore_RainWaterSurplusDef));
        case MDinfiltration:              return (MFModelRun(argc, argv, argNum, MDCore_RainInfiltrationDef));
        case MDrunoff:                    return (MFModelRun(argc, argv, argNum, MDCore_RunoffDef));
        case MDdischarge:                 return (MFModelRun(argc, argv, argNum, MDRouting_DischargeDef));
        case MDbalance:                   return (MFModelRun(argc, argv, argNum, MDCore_WaterBalanceDef));
        case MDgeometry:                  return (MFModelRun(argc, argv, argNum, MDRouting_RiverWidthDef));
        case MDbankfullQcalc:             return (MFModelRun(argc, argv, argNum, MDRouting_BankfullQcalcDef));
        case MDRiverbedShape:             return (MFModelRun(argc, argv, argNum, MDRouting_RiverShapeExponentDef));
        case MDwatertemp:                 return (MFModelRun(argc, argv, argNum, MDTP2M_WTempRiverRouteDef));
        case MDthermal:                   return (MFModelRun(argc, argv, argNum, MDTP2M_ThermalInputsDef));
        case MDBQARTpreprocess:           return (MFModelRun(argc, argv, argNum, MDSediment_BQARTpreprocessDef));
        case MDsedimentflux:              return (MFModelRun(argc, argv, argNum, MDSediment_FluxDef));
        case MDbedloadflux:               return (MFModelRun(argc, argv, argNum, MDSediment_BedloadFluxDef));
        case MDparticulatenutrients:      return (MFModelRun (argc,argv,argNum,  MDSediment_ParticulateNutrientsDef));
        case MDwaterdensity:              return (MFModelRun (argc,argv,argNum,  MDSediment_WaterDensityDef));
    }
    MFOptionMessage(optName, optStr, options);
    return (CMfailed);
}
