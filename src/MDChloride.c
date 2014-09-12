/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2011, UNH - CCNY/CUNY

MDSpecCond.c  - Input and Routing of Specific Conductance

shan.zuidema@unh.edu  
 * 
 * Dependent on SpecCond.c
 * 

*******************************************************************************/
#include <stdio.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>
#include <math.h>

// input
static int _MDInDischargeID            = MFUnset;
// Chloride dependent only on specific conductance conc.
static int _MDInPostConc_SCID         = MFUnset;
static int _MDInFlux_SCID             = MFUnset;
static int _MDInConc_SCID             = MFUnset;

// Chloride conc and flux
static int _MDOutPostConc_ClID          = MFUnset;
static int _MDOutFlux_ClID                 = MFUnset;

static void _MDChloride (int itemID) {
    
    float discharge                     = 0.0; // streamflow m3/s
    float postConc_SC                   = 0.0; // ionic strength uS/cm
    float postConc_Cl                   = 0.0; // chloride conc. (mg / L )
    float postFlux_Cl                   = 0.0; // chloride flux (in streamflow from cell) (kg/day)
   
    discharge            = MFVarGetFloat (_MDInDischargeID,          itemID, 0.0); // m3/sec, discharge leaving the grid cell, after routing!
    postConc_SC          = MFVarGetFloat (_MDInPostConc_SCID,  itemID, 0.0); // uS/cm       
    
    // RELATION BETWEEN SPECIFIC CONDUCTANCE AND CHLORIDE CONCENTRATION
    //  FROM 2013 LOVOTECS SNAPSHOTS
    float m = 0.23348651;    float b = -2.29974005;
    postConc_Cl = m*postConc_SC + b > 0.000001 ? m*postConc_SC + b : 0.000001;
    
    // calculate fluxes
    postFlux_Cl        = (discharge * MDConst_m3PerSecTOm3PerDay) * postConc_Cl / 1000. ;     // kg/day

    MFVarSetFloat (_MDOutPostConc_ClID,                    itemID, postConc_Cl);

    MFVarSetFloat (_MDOutFlux_ClID,            itemID, postFlux_Cl);               
}

int MDChlorideDef () {

	MFDefEntering ("Chloride Calculation");
	// TODO: FIX DECLARATIONS HERE THEN INTEGRATE'
        // TODO: Add option to calculate with specific conductance or via previous method (in DIN?)
        if (
	    ((_MDInFlux_SCID                    = MDSpecCondDef ()) == CMfailed) ||	
	    ((_MDInDischargeID                  = MFVarGetID (MDVarDischarge,                  "m3/s",    MFInput,  MFState, MFBoundary))   == CMfailed) ||
            ((_MDInPostConc_SCID                = MFVarGetID (MDVarPostSpecCond,              "uS/cm",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||	               
            ((_MDOutPostConc_ClID         	= MFVarGetID (MDVarPostConcCl,    	       "mg/L",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||	               
            ((_MDOutFlux_ClID              	= MFVarGetID (MDVarFluxCl,                   "kg/day",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||	               
    

	(MFModelAddFunction (_MDChloride) == CMfailed)) return (CMfailed);
        
	MFDefLeaving ("Chloride Calculation");
	return (_MDOutPostConc_ClID);
}
