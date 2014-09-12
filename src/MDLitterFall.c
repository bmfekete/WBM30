/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

MDLitterFall.c  - Input of POC

rob.stewart@unh.edu  

*******************************************************************************/
#include <stdio.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>
#include <math.h>

// input

static int _MDInLocalLoad_LTRID        = MFUnset;
static int _MDInRiverWidthID           = MFUnset;
static int _MDInFracForestedAreaID     = MFUnset;

// output

static int _MDOutLitterFall_POCID      = MFUnset;
static int _MDOutOrder1Load_POCID      = MFUnset;
static int _MDOutOrder2Load_POCID      = MFUnset;
static int _MDOutOrderXLoad_POCID      = MFUnset;


static void _MDLitterFall (int itemID) {

    float riverOrder    = 0.0;
    float localLoad_LTR = 0.0;          // kg/day
    float width         = 0.0;          // m
    float cellArea      = 0.0;
    float Rb            = 4.5;          // stream number ratio, Leopold et al. 1964
    float Rl            = 2.3;          // mean length ratio, Leopold et al. 1964
    float overHang      = 1.0;          // length of canopy overhang on one side of the river
    float totalLoad     = 0.0;          // kg/day
    
    float order1_meanLength = 1.03;     // km (calculated using the mean length of STN 1st order streams)
    float order2_meanLength = 2.37;     // km (calculated using the mean length of STN 1st order streams)
    
    float order1_meanWidth  = 1.25;    // m (calculated using the mean width of STN 1st order streams and Rw = 2 from Little Tennessee)
    float order2_meanWidth  = 2.50;    // m (calculated using the mean width of STN 1st order streams and Rw = 2 from Little Tennessee)
    float orderX_meanWidth  = 0.0;     // m (read-in)
    
    float order1_load       = 0.0;      // kg/day
    float order2_load       = 0.0;      // kg/day
    float orderX_load       = 0.0;      // local river's input load (kg/day))
    
    float order1_length     = 0.0;      // km
    float order2_length     = 0.0;      // km
    float orderX_length     = 0.0;      // local river grid cell length (km))
    float fracForested      = 0.0;      // proportion of grid cell that is forested
    
    localLoad_LTR        = MFVarGetFloat (_MDInLocalLoad_LTRID,    itemID, 0.0);        // g/m2/day
    orderX_meanWidth     = MFVarGetFloat (_MDInRiverWidthID,       itemID, 0.0);
    cellArea             = MFModelGetArea (itemID);
    fracForested         = MFVarGetFloat (_MDInFracForestedAreaID, itemID, 0.0);        // spatial fraction
    
    order1_length = order1_meanLength * Rb * Rb;        // km
    order2_length = order2_meanLength * Rb;             // km
    orderX_length = MFModelGetLength (itemID) / 1000;   // km
    
    order1_load = order1_meanWidth > (2 * overHang) ? 2 * overHang * order1_length * 1000 * localLoad_LTR / 1000 * fracForested : order1_meanWidth * order1_length * 1000 * localLoad_LTR / 1000 * fracForested;
    order2_load = order2_meanWidth > (2 * overHang) ? 2 * overHang * order2_length * 1000 * localLoad_LTR / 1000 * fracForested : order2_meanWidth * order2_length * 1000 * localLoad_LTR / 1000 * fracForested;
    orderX_load = orderX_meanWidth > (2 * overHang) ? 2 * overHang * orderX_length * 1000 * localLoad_LTR / 1000 * fracForested : orderX_meanWidth * orderX_length * 1000 * localLoad_LTR / 1000 * fracForested;
    
    totalLoad = order1_load + order2_load + orderX_load;       // kg/day
    
 //   if ((itemID == 2736) || (itemID == 141))  {
 //       printf("itemID = %d, %d-%d-%d, localLoad_LTR = %f, widthX = %f, len1 = %f, len2 = %f, lenX = %f\n load1 = %f, load2 = %f, loadX = %f,totalLoad = %f\n",itemID, MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(),localLoad_LTR, orderX_meanWidth, order1_length, order2_length, orderX_length, order1_load, order2_load, orderX_load, totalLoad);              
 //   }  
    
    MFVarSetFloat (_MDOutLitterFall_POCID,             itemID, totalLoad);
    MFVarSetFloat (_MDOutOrder1Load_POCID,             itemID, order1_load);
    MFVarSetFloat (_MDOutOrder2Load_POCID,             itemID, order2_load);
    MFVarSetFloat (_MDOutOrderXLoad_POCID,             itemID, orderX_load);
    
    }

int MDLitterFallDef () {

	MFDefEntering ("Litter Fall");
	
        if (
            ((_MDInRiverWidthID                 = MDRiverWidthDef ())     == CMfailed) ||
            ((_MDInLocalLoad_LTRID              = MFVarGetID (MDVarLocalLoadLTR,              "g/m2/d",  MFInput,  MFFlux,  MFBoundary))   == CMfailed) ||
            ((_MDInFracForestedAreaID           = MFVarGetID (MDVarFracForestedArea,          "-",       MFInput,  MFState, MFBoundary))   == CMfailed) ||
            ((_MDOutLitterFall_POCID         	= MFVarGetID (MDVarLocalLoadPOC,    	      "kg/day", MFOutput,  MFFlux,  MFBoundary))   == CMfailed) ||	  
            ((_MDOutOrder1Load_POCID         	= MFVarGetID (MDVarOrder1LoadPOC,    	      "kg/day", MFOutput,  MFFlux,  MFBoundary))   == CMfailed) ||	  
            ((_MDOutOrder2Load_POCID         	= MFVarGetID (MDVarOrder2LoadPOC,    	      "kg/day", MFOutput,  MFFlux,  MFBoundary))   == CMfailed) ||	  
            ((_MDOutOrderXLoad_POCID         	= MFVarGetID (MDVarOrderXLoadPOC,    	      "kg/day", MFOutput,  MFFlux,  MFBoundary))   == CMfailed) ||	  


	(MFModelAddFunction (_MDLitterFall) == CMfailed)) return (CMfailed);
        
	MFDefLeaving ("Litter Fall");
	return (_MDOutLitterFall_POCID); 
}
