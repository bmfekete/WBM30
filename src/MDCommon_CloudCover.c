
/******************************************************************************
GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDCommon_CloudCover.c

amiara@ccny.cuny.edu

Converting solar radiation to cloud cover fraction (for use in water temperature model)

*******************************************************************************/

#include <string.h>
#include <math.h>
#include <MF.h>
#include <MD.h>

//Input
static int _MDInCommon_Common_SolarRadID    = MFUnset;
static int _MDInCommon_Common_GrossRadID    = MFUnset;
//Output
static int _MDOutCommon_CloudCoverID = MFUnset;

static void _MDCommon_CloudCover(int itemID) {             // should it be InCloudCover?
    float net_solar_in;
    float gross_rad;
    float cloud_cover = 0.0;
    float LHS         = 0.0;
    float a           = 0.0;
    float b           = 0.0;
    float c           = 0.0;

    net_solar_in = MFVarGetFloat(_MDInCommon_Common_SolarRadID, itemID, 0.0); // MJ/m2/d
    gross_rad	 = MFVarGetFloat(_MDInCommon_Common_GrossRadID, itemID, 0.0); // MJ/m2/d

    LHS = net_solar_in / gross_rad;
    a = 0.458;
    b = 0.340;
    c = LHS - 0.803;

    cloud_cover = (-b + pow((pow(b,2) - (4 * a * c)),0.5)) / (2 * a);

    if (cloud_cover < 0.0) cloud_cover = 0.0;
    if (cloud_cover > 1.0) cloud_cover = 1.0;
    cloud_cover = cloud_cover * 100;

    MFVarSetFloat(_MDOutCommon_CloudCoverID, itemID, cloud_cover); // should this be InCloudCover?
}

enum { MDnone, MDinput, MDcalculate };

int MDCommon_CloudCoverDef() {
    int optID = MFUnset;
    const char *optStr, *optName = MDOptWeather_CloudCover;
    const char *options [] = { MDNoneStr, MDInputStr, MDCalculateStr, (char *) NULL};

    if ((optStr = MFOptionGet(optName)) != (char *) NULL) optID = CMoptLookup(options, optStr, true);
    if ((optID == MDnone) || (_MDOutCommon_CloudCoverID != MFUnset)) return (_MDOutCommon_CloudCoverID);

    MFDefEntering("CloudCover");
    switch (optID) {
        case MDnone:
        case MDinput:
            if ((_MDOutCommon_CloudCoverID = MFVarGetID(MDVarCommon_CloudCover, "fraction", MFInput, MFState, MFBoundary)) == CMfailed) return (CMfailed);
            break;
        case MDcalculate:
            if (((_MDInCommon_Common_GrossRadID    = MDCommon_GrossRadDef()) == CMfailed) ||
                ((_MDInCommon_Common_SolarRadID    = MFVarGetID (MDVarCore_SolarRadiation, "MJ/m^2", MFInput,  MFState, MFBoundary)) == CMfailed) ||
                ((_MDOutCommon_CloudCoverID = MFVarGetID (MDVarCommon_CloudCover,   "%",     MFOutput, MFState, MFBoundary)) == CMfailed) ||
                ((MFModelAddFunction (_MDCommon_CloudCover) == CMfailed))) return (CMfailed);
            break;
        default: MFOptionMessage(optName, optStr, options); return (CMfailed);
    }
    MFDefLeaving("CloudCover");
    return (_MDOutCommon_CloudCoverID);
}
