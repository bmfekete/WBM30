/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDIrrigation.c

rob.stewart@unh.edu


*******************************************************************************/


#include <stdio.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>

// Inputs

static int _MDInPlaceHolderID       = MFUnset;
static int _MDInPotETID             = MFUnset;
static int _MDInEvaptrsID           = MFUnset;
static int _MDInEvaptrsNotScaledID  = MFUnset;
static int _MDInIrrFractionID       = MFUnset;
static int _MDInDischargeID         = MFUnset;
static int _MDInResScenarioID       = MFUnset;
static int _MDInResVolumeID         = MFUnset;
static int _MDInPropReservoirID     = MFUnset;

// Outputs

static int _MDOutIrrWdlsID             = MFUnset;
static int _MDOutIrrGrdWaterWdlsID     = MFUnset;
static int _MDOutIrrDischargeWdlsID    = MFUnset;
static int _MDOutGrdWatID              = MFUnset;
static int _MDOutIrrDeficitID          = MFUnset;
static int _MDOutIrrDeficitNotScaledID = MFUnset;
static int _MDOutDischargeWdlsID       = MFUnset;
static int _MDOutGrdWatWdlsID          = MFUnset;
static int _MDOutDischargePreID        = MFUnset;
static int _MDOutIrrETVolID            = MFUnset;
static int _MDOutIrrDischargeWdlsVolID = MFUnset;
static int _MDOutIrrGrdWaterWdlsVolID  = MFUnset;
static int _MDOutIrrDeficitVolID       = MFUnset;
static int _MDOutIrrResWdlsVolID       = MFUnset;
static int _MDOutIrrResAccVolID        = MFUnset;

static void _MDIrrigation (int itemID) {

    float pet                     = 0.0;  // potential evapotranspiration (mm/d)
    float et                      = 0.0;  // evapotranspiration for entire grid cell(mm/d)
    float et_notscaled            = 0.0;  // evapotranspiration for pervious area
    float discharge               = 0.0;  // discharge (m3/s)
    float irrFraction             = 0.0;  // fraction of grid cell needing irrigation (km2/km2)
    float irrDeficit_volume       = 0.0;  // irrigation deficit for cell (m3/d)
    float irrDeficit_mm_notscaled = 0.0;  // irrigation deficit for fields (mm/d)
    float irrDeficit_mm           = 0.0;  // irrigation deficit for entire cell (mm/d))
    float groundWater             = 0.0;  // groundwater stock depth for entire cell (mm)
    float irrDemand_notscaled     = 0.0;  // depth of irrigation demand for agriculture area only
    float irrDemand               = 0.0;  // depth of irrigation demand scaled for entire grid cell
    float irrDemand_total         = 0.0;  // total volume of irrigation required (m3/d)
    float irrDemand_volume        = 0.0;  // volume of irrigation required (m3/d)
    float discharge_volume        = 0.0;  // volume of river discharge (m3/d)
    float grdWater_volume         = 0.0;  // volume of ground water (m3/d)
    float area                    = 0.0;  // area of grid cell (m2))
    float grdWater_out            = 0.0;  // depth of groundwater after irrigation (mm)
    float discharge_out           = 0.0;  // discharge after irrigation (m3/s))
    float grdWater_wdls           = 0.0;  // amount of GW withdrawn for irrigation (mm)
    float discharge_wdls          = 0.0;  // amount of discharge withdrawn for irrigation (m3/s)
    float IrrET                   = 0.0;  // ET depth from irrigation for entire grid cell (mm/d)
    float IrrET_notscaled         = 0.0;  // ET depth from irrigation for Agriculture land (mm/d)
    float placeholder             = 0.0;  // placeholder to read run MDDIN.c
    float balance                 = 0.0;  // balance for entire grid cell
    float balance_notscaled       = 0.0;  // balance for irrigated area only
    float IrrET_m3d               = 0.0;  // volume of irrigation for ET (m3/d))
    float discharge_wdls_m3d      = 0.0;  // volume of discharge wdls (m3/d))
    float grdWater_wdls_m3d       = 0.0;  // volume of grdwater wdls (m3/d)
    float IrrDeficit_m3d          = 0.0;  // volume of Irrigation deficit (m3/d)
    float ResScenario             = 0.0;  // on/off switch for reservoir scenario (1 = on, 0 = off))
    float ResVolume_km3           = 0.0;  // netAccum volume of 0.75 res storage
    float ResVolume_m3d           = 0.0;  // netAccum volume of 0.75 res storage m3/d (June July Aug Sep)
    float ResWdls_m3d             = 0.0;  // volume of reservoir withdrawals (m3/d)
    float PropReservoir           = 0.0;  // proportion of Irrigation provided by reservoirs (solved via IrrReservoirCheck.sh)
    
        int day, month, year;

	pet              = MFVarGetFloat (_MDInPotETID,            itemID, 0.0);
	et               = MFVarGetFloat (_MDInEvaptrsID,          itemID, 0.0);
        et_notscaled     = MFVarGetFloat (_MDInEvaptrsNotScaledID, itemID, 0.0);
        discharge        = MFVarGetFloat (_MDInDischargeID,        itemID, 0.0);
        irrFraction      = MFVarGetFloat (_MDInIrrFractionID,      itemID, 0.0);      
        groundWater      = MFVarGetFloat (_MDOutGrdWatID,          itemID, 0.0);
        placeholder      = MFVarGetFloat (_MDInPlaceHolderID,      itemID, 0.0);
        ResScenario      = MFVarGetFloat (_MDInResScenarioID,      itemID, 0.0);
        ResVolume_km3    = MFVarGetFloat (_MDInResVolumeID,        itemID, 0.0);
        PropReservoir    = MFVarGetFloat (_MDInPropReservoirID,    itemID, 0.0);
        
        day   = MFDateGetCurrentDay ();
	month = MFDateGetCurrentMonth ();
	year  = MFDateGetCurrentYear ();
        area  = MFModelGetArea (itemID);
               
   
        discharge_out       = discharge;                // m3/s
        grdWater_out        = groundWater;              // mm
        
        discharge_volume    = discharge * 86400;                                // m3/d
        grdWater_volume     = groundWater * area / 1000;                        // m3/d
        
if ((month >= 6) && (month <= 9)) {
        
        irrDemand_notscaled = pet - et_notscaled;       
        irrDemand_total     = irrDemand_notscaled * area * irrFraction / 1000;                    // m3/d
        irrDemand_volume    = irrDemand_total;                                                    // m3/d
        irrDemand_volume    = (1.0 - PropReservoir) * irrDemand_volume;                           // m3/d
        ResVolume_m3d       = ResScenario > 0.0 ? ResVolume_km3 * 1000 * 1000 * 1000 / 122 : 0.0; // m3/d
     
	if (irrDemand_volume > 0.0) {
            
            if (ResScenario > 0.0) {
                if (irrDemand_volume > ResVolume_m3d) {
//                  ResWdls_m3d      = 0.0;
                    ResWdls_m3d      = ResVolume_m3d;
                    irrDemand_volume = irrDemand_volume - ResWdls_m3d;

                }
                
                else {
                    ResWdls_m3d      = irrDemand_volume;
                    irrDemand_volume = 0.0;
                }
            }
                    
            
            if (irrDemand_volume > discharge_volume * 0.95) {
                irrDemand_volume = irrDemand_volume - (discharge_volume * 0.95);
                discharge_out    = 0.05 * discharge;
                discharge_wdls   = discharge_volume * 0.95 / 86400;
                
                if (irrDemand_volume > grdWater_volume * 0.99) {
                    irrDemand_volume        = irrDemand_volume - (grdWater_volume * 0.99);
                    grdWater_out            = groundWater * 0.01;
                    grdWater_wdls           = groundWater * 0.99;
                    irrDeficit_volume       = irrDemand_volume;
                    irrDeficit_mm_notscaled = irrDeficit_volume / (area * irrFraction) * 1000;
                    irrDeficit_mm           = irrDeficit_volume / area * 1000;
                }
                
                else {
                    grdWater_out      = groundWater - (irrDemand_volume / area * 1000); // mm
                    grdWater_wdls     = irrDemand_volume / area * 1000;                 // mm/d
                }
            }
            
            else {
                discharge_out  = (discharge_volume - irrDemand_volume) / 86400;         // m3/s
                discharge_wdls = irrDemand_volume / 86400;                              // m3/s
            }
            
        }
   
          IrrET_notscaled = irrDemand_notscaled;            // mm/d
          IrrET           = IrrET_notscaled * irrFraction;  // mm/d       

          IrrET_m3d          = IrrET / 1000 * area;            // m3/d
          discharge_wdls_m3d = discharge_wdls * 86400;         // m3/d
          grdWater_wdls_m3d  = grdWater_wdls / 1000 * area;    // m3/d
          IrrDeficit_m3d     = irrDeficit_mm / 1000 * area;    // m3/d
          
          balance_notscaled = IrrET_notscaled - (discharge_wdls * 86400 * 1000 / area / irrFraction) - (grdWater_wdls / irrFraction) - irrDeficit_mm_notscaled;
          balance           = IrrET - (discharge_wdls * 86400 * 1000 / area) - grdWater_wdls - irrDeficit_mm;

}
        
      
        
/*                
//if (MFDateGetCurrentYear() >= 2000) {        
        if ((itemID == 1287) || (itemID == 537)) {
            printf("%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n",pet, et, et_notscaled, irrFraction, discharge, groundWater, irrDemand_volume, discharge_volume, grdWater_volume, discharge_wdls, discharge_out, grdWater_wdls, grdWater_out, IrrET_notscaled, IrrET, balance_notscaled, balance, IrrET_m3d, discharge_wdls_m3d, grdWater_wdls_m3d, IrrDeficit_m3d);  
        }
//}
*/  
        
	MFVarSetFloat (_MDInDischargeID,            itemID, discharge_out);
        MFVarSetFloat (_MDOutDischargeWdlsID,       itemID, discharge_wdls);
        MFVarSetFloat (_MDOutGrdWatID,              itemID, grdWater_out);
        MFVarSetFloat (_MDOutGrdWatWdlsID,          itemID, grdWater_wdls);
        MFVarSetFloat (_MDOutIrrDeficitNotScaledID, itemID, irrDeficit_mm_notscaled);
        MFVarSetFloat (_MDOutIrrDeficitID,          itemID, irrDeficit_mm);
        MFVarSetFloat (_MDOutDischargePreID,        itemID, discharge);
        MFVarSetFloat (_MDOutIrrETVolID,            itemID, IrrET_m3d);
        MFVarSetFloat (_MDOutIrrDischargeWdlsVolID, itemID, discharge_wdls_m3d);
        MFVarSetFloat (_MDOutIrrGrdWaterWdlsVolID,  itemID, grdWater_wdls_m3d);
        MFVarSetFloat (_MDOutIrrDeficitVolID,       itemID, IrrDeficit_m3d);
        MFVarSetFloat (_MDOutIrrResWdlsVolID,       itemID, ResWdls_m3d);
        MFVarSetFloat (_MDOutIrrResAccVolID,        itemID, ResVolume_m3d);
}


int MDIrrigationDef() {

    


	MFDefEntering ("Irrigation");

	 if (
             ((_MDInPlaceHolderID	   = MDDINDef ()) == CMfailed) ||
             ((_MDInDischargeID            = MDDischargeDef ()) == CMfailed) ||
	     ((_MDInPotETID                = MFVarGetID (MDVarRainPotEvapotrans,       "mm",   MFInput, MFFlux,   MFBoundary)) == CMfailed) ||	     
             ((_MDInEvaptrsID              = MFVarGetID (MDVarRainEvapotranspiration,  "mm",   MFInput, MFFlux,   MFBoundary)) == CMfailed) ||
 	     ((_MDInEvaptrsNotScaledID     = MFVarGetID (MDVarRainETnotScaled,         "mm",   MFInput, MFFlux,   MFBoundary)) == CMfailed) ||	
             ((_MDInIrrFractionID	   = MFVarGetID (MDVarIrrAreaFraction,          "-",   MFInput, MFState,  MFBoundary)) == CMfailed) ||
             ((_MDInResScenarioID	   = MFVarGetID (MDVarResScenario,              "-",   MFInput, MFState,  MFBoundary)) == CMfailed) ||
             ((_MDInResVolumeID            = MFVarGetID (MDVarResVolume,              "km3",   MFInput, MFState,  MFBoundary)) == CMfailed) ||        
             ((_MDInPropReservoirID        = MFVarGetID (MDVarPropReservoir,            "-",  MFInput,  MFState,  MFBoundary)) == CMfailed) || 
             ((_MDOutDischargePreID        = MFVarGetID (MDVarIrrDischargePre,       "m3/s",  MFOutput, MFState,  MFBoundary)) == CMfailed) ||
             ((_MDOutDischargeWdlsID	   = MFVarGetID (MDVarIrrDischargeWdls,      "m3/s",  MFOutput, MFState,  MFBoundary)) == CMfailed) ||
             ((_MDOutGrdWatID              = MFVarGetID (MDVarGroundWater,             "mm",  MFOutput, MFState,  MFInitial))  == CMfailed) ||
             ((_MDOutGrdWatWdlsID	   = MFVarGetID (MDVarIrrGrdWatWdls,         "mm/d",  MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
             ((_MDOutIrrDeficitNotScaledID = MFVarGetID (MDVarIrrDeficitNotScaled,   "mm/d",  MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
             ((_MDOutIrrDeficitID          = MFVarGetID (MDVarIrrDeficit,            "mm/d",  MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
             ((_MDOutIrrETVolID            = MFVarGetID (MDVarIrrETVol,              "m3/d",  MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
             ((_MDOutIrrDischargeWdlsVolID = MFVarGetID (MDVarIrrQWdlsVol,           "m3/d",  MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
             ((_MDOutIrrGrdWaterWdlsVolID  = MFVarGetID (MDVarIrrGrdWatWdlsVol,      "m3/d",  MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
             ((_MDOutIrrDeficitVolID       = MFVarGetID (MDVarIrrDeficitVol,         "m3/d",  MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
             ((_MDOutIrrResWdlsVolID       = MFVarGetID (MDVarIrrResWdlsVol,         "m3/d",  MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
             ((_MDOutIrrResAccVolID        = MFVarGetID (MDVarIrrResAccVol,          "m3/d",  MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
                        
                 
             (MFModelAddFunction(_MDIrrigation) == CMfailed)) return (CMfailed);

	MFDefLeaving ("Irrigation");
	return (_MDOutIrrDeficitID);

}


