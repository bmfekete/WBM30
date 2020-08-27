
/******************************************************************************
GHAAS Water Balance/Transport Model V2.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDCloudCover.c

amiara@ccny.cuny.edu

Converting solar radiation to cloud cover fraction (for use in water temperature model)

*******************************************************************************/

#include <string.h>
#include <math.h>
#include <MF.h>
#include <MD.h>

//Input
static int _MDInSolarRadID = MFUnset;
static int _MDInGrossRadID = MFUnset ;
//static int _MDInPrecipitationID = MFUnset ;

//static int _MDInputID, _MDGrossRadID; //taken out to match style of wetbulb

//Output
static int _MDOutCloudCoverID = MFUnset;

static void _MDCloudCover(int itemID) {             // should it be InCloudCover?
    float net_solar_in;
    float gross_rad;
    float cloud_cover           = 0.0;
    float LHS                   = 0.0;
    float a                     = 0.0;
    float b                     = 0.0;
    float c                     = 0.0;
//    float precip;

//    precip	 = MFVarGetFloat(_MDInPrecipitationID, itemID, 0.0);
    net_solar_in = MFVarGetFloat(_MDInSolarRadID, itemID, 0.0);			  //MJ/m2/d
    gross_rad	 = MFVarGetFloat(_MDInGrossRadID, itemID, 0.0);                   //MJ/m2/d

    LHS = net_solar_in / gross_rad;
    a = 0.458;
    b = 0.340;
    c = LHS - 0.803;

    cloud_cover = (-b + pow((pow(b,2) - (4 * a * c)),0.5)) / (2 * a);
    //cloud_cover = (-b - pow((pow(b,2) - (4 * a * c)),0.5)) / (2 * a); // gives wrong answer

    if (cloud_cover < 0.0) cloud_cover = 0.0;
    if (cloud_cover > 1.0) cloud_cover = 1.0;
    cloud_cover = cloud_cover * 100;

//    cloud_cover = (precip == 0) ? 0.0 : cloud_cover;         //check to make sure its raining

    MFVarSetFloat(_MDOutCloudCoverID, itemID, cloud_cover);		//should this be InCloudCover?

}


enum {
    MDnone, MDinput, MDcalculate
};

int MDCloudCoverDef() {
    int optID = MFUnset;
    const char *optStr, *optName = MDOptCloudCover;
    const char *options [] = {MDNoneStr, MDInputStr, MDCalculateStr, (char *) NULL};

    if ((optStr = MFOptionGet(optName)) != (char *) NULL) optID = CMoptLookup(options, optStr, true);
    if ((optID == MDnone) || (_MDOutCloudCoverID != MFUnset)) return (_MDOutCloudCoverID);

    MFDefEntering("CloudCover");

    switch (optID) {
        case MDnone:
        case MDinput:
            if      ((_MDOutCloudCoverID = MFVarGetID(MDVarCloudCover, "fraction", MFInput, MFState, MFBoundary)) == CMfailed) return (CMfailed);
            break;
        case MDcalculate:
            if (    ((_MDInGrossRadID        = MDGrossRadDef ()) == CMfailed) ||
		    ((_MDInSolarRadID        = MFVarGetID(MDVarSolarRadiation,   	"MJ/m2",  	MFInput, MFState, MFBoundary)) == CMfailed) ||
//                    ((_MDInPrecipitationID     = MFVarGetID (MDVarPrecipitation,     "degC",  MFInput,  MFState, MFBoundary)) == CMfailed) ||
                    ((_MDOutCloudCoverID     = MFVarGetID(MDVarCloudCover,      "%",  	MFOutput,MFState, MFBoundary)) == CMfailed) ||
                    ((MFModelAddFunction(_MDCloudCover) == CMfailed))
                    ) return (CMfailed);
            break;
        default: MFOptionMessage(optName, optStr, options);
            return (CMfailed);
    }
    MFDefLeaving("CloudCover");
    return (_MDOutCloudCoverID);
}






