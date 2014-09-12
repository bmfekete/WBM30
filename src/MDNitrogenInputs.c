/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDNitrogenInputs.c  - Nitrogen Loading

rob.stewart@unh.edu

*******************************************************************************/

#include <cm.h>
#include <MF.h>
#include <MD.h>
#include <math.h>

// Input

static int _MDInLandUseID		= MFUnset;
static int _MDInRunoffID		= MFUnset;
static int _MDInRunoffVolID		= MFUnset;
static int _MDInRiverOrderID		= MFUnset;		//RJS 090508
static int _MDInDINLoadConcID		= MFUnset;		//RJS 102810
static int _MDInRunoffPoolMassRelID     = MFUnset;		//RJS 050511
static int _MDInGrdWatMassRelID		= MFUnset;		//RJS 050511
static int _MDInLawnFractionID          = MFUnset;		//RJS 051111
static int _MDInLoadAdjustID		= MFUnset;		//RJS 112211
static int _MDInTotalPointID		= MFUnset;
static int _MDInLandUseSubID		= MFUnset;
static int _MDInLandUseAgID		= MFUnset;
static int _MDInLocalLoad_DINID         = MFUnset;              //RJS 011414
static int _MDInLocalLoad_Con_DINID     = MFUnset;              //RJS 081814
static int _MDInLocalLoad_Dec_DINID     = MFUnset;              //RJS 081814
static int _MDInLocalLoad_Mix_DINID     = MFUnset;              //RJS 081814
static int _MDInLocalLoad_Lak_DINID     = MFUnset;              //RJS 081914
static int _MDInLandUseConID            = MFUnset;              //RJS 081914
static int _MDInLandUseDecID            = MFUnset;              //RJS 081914
static int _MDInLandUseMixID            = MFUnset;              //RJS 081914
static int _MDInH2OFractionID           = MFUnset;              //RJS 081914

// Output

static int _MDOutLocalLoad_DINID                   = MFUnset;		// RJS 090308
static int _MDOutLocalConc_DINID                   = MFUnset;		// RJS 090308
static int _MDOutLocalLoadnew_DINID                = MFUnset;		// RJS 061511	just to CHECK whether the new and old loading are the same
static int _MDOutDINLoadConcID                     = MFUnset;		// KYLE
static int _MDOutLocalLoad_Sub_DINID               = MFUnset;
static int _MDOutLocalLoad_Agr_DINID		   = MFUnset;
static int _MDOutDINSubLoadConcID		   = MFUnset;
static int _MDOutDINAgLoadConcID		   = MFUnset;
static int _MDOutLocalLoad_PnET_DINID              = MFUnset;
static int _MDOutLocalLoad_Lak_DINID               = MFUnset;
static int _MDOutLocalLoad_For_DINID               = MFUnset;
static int _MDOutLocalConc_Sub_DINID               = MFUnset;
static int _MDOutLocalConc_Agr_DINID               = MFUnset;
static int _MDOutLocalConc_Lak_DINID               = MFUnset;
static int _MDOutLocalConc_For_DINID               = MFUnset;


static void _MDNitrogenInputsInput (int itemID) {

	//Input
	float runoff;		//mm/day
	float runoffVol; 	//m3/sec
	float LocalConc_DIN;
	float riverOrder;
        float LocalLoad_DIN_in; // kg/d

	//Output
	float LocalLoad_DIN;

//	runoff             = MFVarGetFloat (_MDInRunoffID,         	  itemID, 0.0); // mm / d
//	runoffVol          = MFVarGetFloat (_MDInRunoffVolID,             itemID, 0.0); // m3/sec
//	LocalConc_DIN	   = MFVarGetFloat (_MDInDINLoadConcID,           itemID, 0.0); // mg/L
        LocalLoad_DIN_in   = MFVarGetFloat (_MDInLocalLoad_DINID,         itemID, 0.0); // kg/d
//	riverOrder	   = MFVarGetFloat (_MDInRiverOrderID,            itemID, 0.0);

//	LocalLoad_DIN      = runoffVol * 86400 * LocalConc_DIN / 1000; // kg/day
        LocalLoad_DIN      = LocalLoad_DIN_in;                         // kg/day
        
        
	MFVarSetFloat (_MDOutLocalLoad_DINID,        itemID, LocalLoad_DIN);	  // RJS 090308

}

static void _MDNitrogenInputsInput2 (int itemID) {

	//Input
	float runoff                    = 0.0;		//mm/day
	float runoffVol                 = 0.0; 	//m3/sec
	float LocalConc_DIN             = 0.0;
	float riverOrder                = 0.0;
        float LocalLoad_Con_DIN_in      = 0.0; // kg/d
        float LocalLoad_Dec_DIN_in      = 0.0; // kg/d
        float LocalLoad_Mix_DIN_in      = 0.0; // kg/d
        float LocalLoad_Lak_DIN_in      = 0.0; // kg/d
        float luSub                     = 0.0; // proportion suburban or developed
        float luAgr                     = 0.0; // proportion agriculture
        float LocalLoad_Con_DIN         = 0.0;
        float LocalLoad_Dec_DIN         = 0.0;
        float LocalLoad_Mix_DIN         = 0.0;
        float LocalLoad_Sub_DIN         = 0.0;
        float LocalLoad_Agr_DIN         = 0.0;
        float LocalLoad_For_DIN         = 0.0;
        float LocalLoad_Lak_DIN         = 0.0;
        float LocalConc_Sub_DIN         = 0.0;
        float LocalConc_Agr_DIN         = 0.0;
        float LocalConc_For_DIN         = 0.0;
        float LocalConc_Lak_DIN         = 0.0;
        float scale                     = 0.0;
        float asym                      = 0.0;
        float xMid                      = 0.0;
        float LocalLoad_DIN_PnET        = 0.0;
        float luForest                  = 0.0;
        float luLak                     = 0.0;
        float luCon                     = 0.0;
        float luMix                     = 0.0;
        float luDec                     = 0.0;
        float area_m2                   = 0.0;
        float LocalConc_Sub_DIN2        = 0.0;
        float LocalConc_Agr_DIN2        = 0.0;

        
	//Output
	float LocalLoad_DIN;
        
        if (_MDInLandUseSubID  != MFUnset)	luSub = MFVarGetFloat (_MDInLandUseSubID,  itemID, 0.0);	//KAW 2013 05 08 Suburban Land Use
	if (_MDInLandUseAgID   != MFUnset)	luAgr = MFVarGetFloat (_MDInLandUseAgID,   itemID, 0.0);	//KAW 2013 05 08 Agricultural Land Use
        if (_MDInLandUseConID  != MFUnset)	luCon = MFVarGetFloat (_MDInLandUseConID,  itemID, 0.0) * 100;	//KAW 2013 05 08 Suburban Land Use
	if (_MDInLandUseDecID  != MFUnset)	luDec = MFVarGetFloat (_MDInLandUseDecID,  itemID, 0.0) * 100;	//KAW 2013 05 08 Agricultural Land Use
	if (_MDInLandUseMixID  != MFUnset)	luMix = MFVarGetFloat (_MDInLandUseMixID,  itemID, 0.0) * 100;	//KAW 2013 05 08 Agricultural Land Use
	if (_MDInH2OFractionID != MFUnset)	luLak = MFVarGetFloat (_MDInH2OFractionID, itemID, 0.0) * 100;	//KAW 2013 05 08 Agricultural Land Use

        area_m2                = MFModelGetArea (itemID);
        LocalLoad_Con_DIN_in   = MFVarGetFloat (_MDInLocalLoad_Con_DINID,         itemID, 0.0) * area_m2 / ((luCon / 100) * area_m2); // g / m2 for Dec area only (not for entire grid cell)
        LocalLoad_Dec_DIN_in   = MFVarGetFloat (_MDInLocalLoad_Dec_DINID,         itemID, 0.0) * area_m2 / ((luDec / 100) * area_m2); // g / m2 for Con area only (not for entire grid cell)
        LocalLoad_Mix_DIN_in   = MFVarGetFloat (_MDInLocalLoad_Mix_DINID,         itemID, 0.0) * area_m2 / ((luMix / 100) * area_m2); // g / m2 for Con area only (not for entire grid cell)
        LocalLoad_Lak_DIN_in   = MFVarGetFloat (_MDInLocalLoad_Lak_DINID,         itemID, 0.0) * area_m2 / ((luLak) * area_m2); // g / m2 for Lak area only (not for entire grid cell)
        
        LocalLoad_Con_DIN_in   = luCon > 0.0 ? LocalLoad_Con_DIN_in : 0.0;
        LocalLoad_Dec_DIN_in   = luDec > 0.0 ? LocalLoad_Dec_DIN_in : 0.0;
        LocalLoad_Mix_DIN_in   = luMix > 0.0 ? LocalLoad_Mix_DIN_in : 0.0;
        LocalLoad_Lak_DIN_in   = luLak > 0.0 ? LocalLoad_Lak_DIN_in : 0.0;
      
        luSub = (luSub + luAgr + luLak) > 100.0 ? luSub / (luSub + luAgr + luLak) * 100.0 : luSub;        // RJS 110813    
        luAgr = (luSub + luAgr + luLak) > 100.0 ? luAgr / (luSub + luAgr + luLak) * 100.0 : luAgr;        // RJS 110813
                
        runoff             = MFVarGetFloat (_MDInRunoffID,         itemID, 0.0); 	// mm / d
	runoffVol          = MFVarGetFloat (_MDInRunoffVolID,      itemID, 0.0); 	// m3/sec

        luForest           = 100 - luSub - luAgr - luLak;
       
        if (runoffVol > 0.0) {    
        LocalConc_For_DIN  = (luCon + luDec + luMix) > 0.0 ? ((LocalLoad_Con_DIN_in * (luCon / 100) * area_m2 / 1000) + (LocalLoad_Dec_DIN_in * (luDec / 100) * area_m2 / 1000) + (LocalLoad_Mix_DIN_in * (luMix / 100) * area_m2 / 1000)) / (runoffVol * ((luCon / 100) + (luDec / 100) + (luMix / 100)) * 86400) * 1000 : 0.0;
        LocalConc_Lak_DIN  = luLak > 0.0 ? (LocalLoad_Lak_DIN_in * (luLak / 100) * area_m2 / 1000) / (runoffVol * (luLak / 100) * 86400) * 1000 : 0.0;    
        }
        
 //       LocalLoad_DIN_PnET = ((luDec / (luDec + luCon)) * (luForest / 100) * LocalLoad_Dec_DIN_in * area_m2 / 1000) + ((luCon / (luDec + luCon)) * (luForest / 100) * LocalLoad_Con_DIN_in * area_m2 / 1000) + ((luLak / 100) * LocalLoad_Lak_DIN_in * area_m2 / 1000); // kg /day
        
	scale          = 12.2;
	asym           = 1.4;

	if (runoff < 0.00001) {
		LocalConc_Sub_DIN = 0.0;		
		LocalConc_Agr_DIN = 0.0;	
	}

	else {
            xMid = 60.0 + 12.0 * log(runoff);   // RJS 110713 more accurately represents Wollheim et al. 2008 for runoff
            LocalConc_Sub_DIN  = asym / (1 + pow(2.718281828, (xMid - luSub) / scale));
            LocalConc_Agr_DIN  = (asym * 3.5) / (1 + pow(2.718281828, (xMid - luAgr) / scale)) - ((asym * 3.5) / (1 + pow(2.718281828, (xMid - 0.0) / scale))); 
	
            LocalConc_Sub_DIN2  = (asym / (1 + pow(2.718281828, (xMid - luSub) / scale))) - (asym / (1 + pow(2.718281828, (xMid - 0.0) / scale))) + LocalConc_For_DIN;
            LocalConc_Agr_DIN2  = (asym * 3.5) / (1 + pow(2.718281828, (xMid - luAgr) / scale)) - ((asym * 3.5) / (1 + pow(2.718281828, (xMid - 0.0) / scale))); 
	
        }

        /////// Option 1 /////////
        
//	LocalLoad_Sub_DIN  = (luSub / 100) * runoffVol * 86400 * LocalConc_Sub_DIN / 1000; 		// kg/day //KAW 2013 05 08 Loading from suburban part of each grid cell
//	LocalLoad_Agr_DIN  = (luAgr / 100) * runoffVol * 86400 * LocalConc_Agr_DIN / 1000; 		// kg/day //KAW 2013 05 08 Loading from the agricultural part of each grid cell
//      LocalLoad_Lak_DIN  = (luLak / 100) * runoffVol * 86400 * LocalConc_Lak_DIN / 1000;              // kg/day
//      LocalLoad_For_DIN  = (luForest / 100) * runoffVol * 86400 * LocalConc_For_DIN / 1000;           // kg/day
	
//      LocalLoad_DIN      = LocalLoad_Agr_DIN + LocalLoad_Sub_DIN + LocalLoad_Lak_DIN + LocalLoad_For_DIN;                // kg/day //KAW 2013 05 08: Sum of all N loadings  Total point is from MMM's Code for point source N inputs
//      LocalConc_DIN      = runoffVol > 0.0 ? LocalLoad_DIN / (runoffVol * 86400) * 1000 : 0.0;

        /////// Option 2 /////////
    
        LocalLoad_Sub_DIN = runoffVol * 86400 * LocalConc_Sub_DIN2 / 1000;                         // kg/day
        LocalLoad_Agr_DIN = runoffVol * 86400 * LocalConc_Agr_DIN2 / 1000;                         // kg/day
        LocalLoad_For_DIN = runoffVol * (luForest / 100) * 86400 * LocalConc_For_DIN / 1000;       // kg/day
        LocalLoad_Lak_DIN = runoffVol * (luLak / 100) * 86400 * LocalConc_Lak_DIN / 1000;          // kg/day     
        
        LocalLoad_DIN      = LocalLoad_Agr_DIN + LocalLoad_Sub_DIN + LocalLoad_For_DIN + LocalLoad_Lak_DIN;     // kg/day 
        LocalConc_DIN      = runoffVol > 0.0 ? LocalLoad_DIN / (runoffVol * 86400) * 1000 : 0.0;

        
   //     if (itemID == 7734) printf("area_m2 = %f,  luSub = %f, luAgr = %f, luForest = %f, luLak = %f, luCon = %f, luDec = %f, Load=%f, Conc=%f\n", area_m2, luSub, luAgr, luForest, luLak, luCon, luDec, LocalLoad_DIN, LocalConc_DIN);
   //     if (itemID == 7734) printf("runoffVol = %f, localConc_Lak_DIN = %f, LocalLoad_Lak_DIN = %f, LocalLoad_Lak_DIN_in = %f\n",runoffVol, LocalConc_Lak_DIN, LocalLoad_Lak_DIN, LocalLoad_Lak_DIN_in);
   //     if (itemID == 7734) printf("LocalLoad_Agr_DIN = %f, LocalLoad_Sub_DIN = %f, LocalLoad_Lak_DIN = %f, LocalLoad_For_DIN = %f\n", LocalLoad_Agr_DIN, LocalLoad_Sub_DIN, LocalLoad_Lak_DIN, LocalLoad_For_DIN);
        
	MFVarSetFloat (_MDOutLocalLoad_DINID,         itemID, LocalLoad_DIN);	  // RJS 090308
	MFVarSetFloat (_MDOutLocalConc_DINID,         itemID, LocalConc_DIN);	  // RJS 090308
        MFVarSetFloat (_MDOutLocalLoad_Sub_DINID,     itemID, LocalLoad_Sub_DIN);	  	// RJS 090308
	MFVarSetFloat (_MDOutLocalLoad_Agr_DINID,     itemID, LocalLoad_Agr_DIN);	  	// RJS 090308
	MFVarSetFloat (_MDOutLocalLoad_Lak_DINID,    itemID,  LocalLoad_Lak_DIN);	  	// RJS 090308
	MFVarSetFloat (_MDOutLocalLoad_For_DINID,    itemID,  LocalLoad_For_DIN);	  	// RJS 090308
	MFVarSetFloat (_MDOutLocalConc_Sub_DINID,     itemID, LocalConc_Sub_DIN);	  	// RJS 090308
	MFVarSetFloat (_MDOutLocalConc_Agr_DINID,     itemID, LocalConc_Agr_DIN);	  	// RJS 090308
	MFVarSetFloat (_MDOutLocalConc_Lak_DINID,    itemID,  LocalConc_Lak_DIN);	  	// RJS 090308
	MFVarSetFloat (_MDOutLocalConc_For_DINID,    itemID,  LocalConc_For_DIN);	  	// RJS 090308

}

static void _MDNitrogenInputsPartitioned (int itemID) {

	//Input
	float runoffPoolMassRel = 0.0;	//kg/day	RJS 050511
	float grdWatMassRel		= 0.0;	//kg/day	RJS 050511
	float runoffVol			= 0.0;  //m3/sec	RJS 050511	need this to call runoffVol
	float temp				= 0.0;	//kg/day	RJS 061611

	//Output
	float LocalLoad_DIN;

	runoffVol		   = MFVarGetFloat (_MDInRunoffVolID,         itemID, 0.0);	// m3/s  RJS 050511
	runoffPoolMassRel  = MFVarGetFloat (_MDInRunoffPoolMassRelID, itemID, 0.0); // kg/d  RJS 050511
	grdWatMassRel      = MFVarGetFloat (_MDInGrdWatMassRelID,     itemID, 0.0); // kg/d	 RJS 050511

	LocalLoad_DIN      = runoffPoolMassRel + grdWatMassRel; // kg/day	RJS 050511
	temp               = LocalLoad_DIN;						// kg/day	RJS 061611

	MFVarSetFloat (_MDOutLocalLoad_DINID,    itemID, LocalLoad_DIN);	 // RJS 050511
	MFVarSetFloat (_MDOutLocalLoadnew_DINID, itemID, temp);				 // RJS 061611
}



static void _MDNitrogenInputsCalc (int itemID) {

	// Input

	float luSub = 0.0;
	float luAgr = 0.0;
	float runoff;			// mm/day
	float runoffVol;		// m3/sec

	float scale;
	float asym;
	float xMid;

	float LocalConc_DIN      = 0.0;
	float LocalLoad_DIN      = 0.0;

	float LocalConc_Sub_DIN  = 0.0;		//KAW 2013 05 08 Suburban Loading Concentration
	float LocalLoad_Sub_DIN  = 0.0;		//KAW 2013 05 08 Suburban Loading

	float LocalConc_Agr_DIN   = 0.0;		//KAW 2013 05 08 Agricultural Loading Concentration
	float LocalLoad_Agr_DIN   = 0.0;		//KAW 2013 05 08 Agricultural Loading

	float riverOrder;
	float loadAdjust = 0.0;			// RJS 112211
	float Total_point = 0.0;

// test

	if (_MDInRiverOrderID != MFUnset)  riverOrder = MFVarGetFloat (_MDInRiverOrderID, itemID, 0.0);	// RJS 090508
//	if (_MDInLandUseID    != MFUnset)	luSub = MFVarGetFloat (_MDInLandUseID,    itemID, 0.0);
	if (_MDInLandUseSubID != MFUnset)	luSub = MFVarGetFloat (_MDInLandUseSubID, itemID, 0.0);	//KAW 2013 05 08 Suburban Land Use
	if (_MDInLandUseAgID  != MFUnset)	luAgr = MFVarGetFloat (_MDInLandUseAgID,  itemID, 0.0);	//KAW 2013 05 08 Agricultural Land Use

//	luSub = luSub + luAg > 100 ? 100 : luSub + luAg;                                // commented out 110813
	luSub = luSub + luAgr > 100.0 ? luSub / (luSub + luAgr) * 100.0 : luSub;        // RJS 110813    
        luAgr = luSub + luAgr > 100.0 ? luAgr / (luSub + luAgr) * 100.0 : luAgr;        // RJS 110813

        
	runoff             = MFVarGetFloat (_MDInRunoffID,         itemID, 0.0); 	// mm / d
	runoffVol          = MFVarGetFloat (_MDInRunoffVolID,      itemID, 0.0); 	// m3/sec
	loadAdjust	   = MFVarGetFloat (_MDInLoadAdjustID,     itemID, 1.0);	// RJS 112211, adjusts loads, keep at 1 if nodata

	scale          = 12.2;
	asym           = 1.4;

	if (runoff < 0.00001) {
//		LocalConc_DIN = 0.0;                    // RJS commented out 110813
		LocalConc_Sub_DIN = 0.0;		//KAW 2013 05 08
		LocalConc_Agr_DIN = 0.0;		//KAW 2013 05 08
	}

	else {
            xMid = 60.0 + 12.0 * log(runoff);   // RJS 110713 more accurately represents Wollheim et al. 2008 for runoff
//              xMid           = 51.388 + 19.459 * log(runoff); //RJS 07-29-08	MMM changed from log10 to log 2013-03-13
//		LocalConc_DIN  = asym / (1 + pow(2.718281828, (xMid - luSub) / scale)); // mg/L
            LocalConc_Sub_DIN  = asym / (1 + pow(2.718281828, (xMid - luSub) / scale)); 		// mg/L //KAW 2013 05 08
            LocalConc_Agr_DIN  = ((asym * 3.5) / (1 + pow(2.718281828, (xMid - luAgr) / scale))) - ((asym * 3.5) / (1 + pow(2.718281828, (xMid - 0.0) / scale))); 	// mg/L //KAW 2013 05 08 (to remove double counting of natural inputs) Concentrations in Agriculture headwater stream are 3.5x the concentrations in suburban stream (Price, unpublished data)
	}


//	LocalLoad_DIN  = runoffVol * 86400 * LocalConc_DIN / 1000; // kg/day            // RJS commented out 110813
	LocalLoad_Sub_DIN  = runoffVol * 86400 * LocalConc_Sub_DIN / 1000; 		// kg/day //KAW 2013 05 08 Loading from suburban part of each grid cell
	LocalLoad_Agr_DIN  = runoffVol * 86400 * LocalConc_Agr_DIN / 1000; 		// kg/day //KAW 2013 05 08 Loading from the agricultural part of each grid cell
	LocalLoad_DIN      = LocalLoad_Agr_DIN + LocalLoad_Sub_DIN;                     // kg/day //KAW 2013 05 08: Sum of all N loadings  Total point is from MMM's Code for point source N inputs

	if (loadAdjust > 0.0) LocalLoad_DIN =  LocalLoad_DIN * loadAdjust; 	// RJS 112211

 //       if (itemID == 1357) printf("itemID=%d, luAgr = %f, luSub = %f, load_agr = %f, load_sub = %f, totalLoad = %f\n",itemID,luAgr,luSub,LocalLoad_Agr_DIN,LocalLoad_Sub_DIN,LocalLoad_DIN);
        
        //if(LocalConc_Sub_DIN > 1.4 || LocalConc_Ag_DIN > 4.9) printf("runoff = %f, luSub = %f, luAg = %f, xMid = %f, Sub_conc = %f, Ag_conc = %f\n", runoff, luSub, luAg, xMid, LocalConc_Sub_DIN, LocalConc_Ag_DIN);
        //if (itemID == 809) printf("**** itemID = %d, y=%d, m=%d, d=%d, runoff=%f, luSub=%f, luAg=%f, xMid=%f, Sub_conc=%f, Ag_conc=%f, Subload=%f, Agload=%f, total=%f\n", itemID, MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), runoff, luSub, luAg, xMid, LocalConc_Sub_DIN, LocalConc_Ag_DIN, LocalLoad_Sub_DIN, LocalLoad_Ag_DIN, LocalLoad_DIN);
	//if (itemID == 31) printf("***** itemID = %d, year = %d, month = %d, day = %d, xMid = %f, luSub = %f, scale = %f, asym = %f\n", itemID, MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), xMid, luSub, scale, asym);
	//if (itemID == 31) printf("runoffVol = %f, runoff = %f, LocalLoad_DIN = %f, LocalConc_DIN = %f, Total_point= %f, loadAdjust = %f\n", runoffVol, runoff, LocalLoad_DIN, LocalConc_DIN, Total_point, loadAdjust);  //mmm commented out 2013-3-13

 //       printf("loadAdjust = %f\n", loadAdjust);

	MFVarSetFloat (_MDOutLocalLoad_DINID,       itemID, LocalLoad_DIN);	  	// RJS 090308
	MFVarSetFloat (_MDOutLocalLoad_Sub_DINID,   itemID, LocalLoad_Sub_DIN);	  	// RJS 090308
	MFVarSetFloat (_MDOutDINSubLoadConcID, 	    itemID, LocalConc_Sub_DIN);  	// KAW 2013/03/15
	MFVarSetFloat (_MDOutLocalLoad_Agr_DINID,   itemID, LocalLoad_Agr_DIN);	  	// RJS 090308
	MFVarSetFloat (_MDOutDINAgLoadConcID, 	    itemID, LocalConc_Agr_DIN);		// KAW 2013/03/15
}

enum {MDcalculate, MDinput, MDinput2, MDnone};

int MDNitrogenInputsDef () {

			int  optID = MFUnset;													    //RJS 10-28-10
			const char *optStr, *optName = MDOptDINInputs;								//RJS 10-28-10
			const char *options [] = { MDCalculateStr, MDInputStr, MDInput2Str, MDNoneStr, (char *) NULL };		//RJS 10-28-10

	MFDefEntering ("Nitrogen Inputs");

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);  //RJS 02-11-10

	switch (optID) {	//RJS 02-11-10

	case MDnone:
	if (((_MDInLandUseID             = MFVarGetID (MDVarLandUseSpatial,       "-",  MFInput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInRunoffID   		 = MFVarGetID (MDVarRunoff,              "mm",  MFInput,  MFFlux, MFBoundary)) == CMfailed) ||
	    ((_MDInRunoffVolID           = MDRunoffVolumeDef ()) == CMfailed) ||
	    ((_MDInLawnFractionID        = MFVarGetID (MDVarLawnFraction,      "-", MFInput, MFState, MFBoundary)) == CMfailed) ||		//RJS 051111
	    ((_MDInRiverOrderID          = MFVarGetID (MDVarRiverOrder,  	  	  "-",  MFInput, MFState, MFBoundary)) == CMfailed) ||	// RJS 090308
	    ((_MDInLoadAdjustID          = MFVarGetID (MDVarLoadAdjust,           "-", MFInput, MFState, MFBoundary)) == CMfailed)  ||  // RJS 112211
	    ((_MDOutLocalLoad_DINID      = MFVarGetID (MDVarLocalLoadDIN,    "kg/day", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||  // RJS 090308
	    ((_MDOutLocalLoadnew_DINID   = MFVarGetID (MDVarLocalLoadDINnew, "kg/day", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||	// RJS 061511	created to compare new and old loads ONLY.
	    (MFModelAddFunction (_MDNitrogenInputsPartitioned) == CMfailed)) return (CMfailed);
		break;

	case MDinput:
	if (((_MDInLocalLoad_DINID       = MFVarGetID (MDVarInLocalLoadDIN,       "kg/day", MFInput, MFFlux, MFBoundary)) == CMfailed) || 
//          ((_MDInGrdWatMassRelID       = MFVarGetID (MDVarGroundWaterMassRel,   "kg/day", MFInput, MFFlux, MFBoundary)) == CMfailed) ||	//RJS 050511
//          ((_MDInRunoffPoolMassRelID   = MFVarGetID (MDVarRunoffPoolMassRel,    "kg/day", MFInput, MFFlux, MFBoundary)) == CMfailed) ||	//RJS 050511
//          ((_MDInRunoffVolID           = MDRunoffVolumeDef ()) == CMfailed) ||															//RJS 050511
	    ((_MDOutLocalLoad_DINID      = MFVarGetID (MDVarLocalLoadDIN,         "kg/day", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||	//RJS 050511
	    (MFModelAddFunction (_MDNitrogenInputsInput) == CMfailed)) return (CMfailed);
		break;
                
        case MDinput2:
	if (((_MDInRunoffID   		 = MFVarGetID (MDVarRunoff,              "mm",  MFInput,  MFFlux, MFBoundary)) == CMfailed) ||	//RJS 10-28-10
	    ((_MDInRunoffVolID           = MDRunoffVolumeDef ()) == CMfailed) ||			// RJS 10-28-10
            ((_MDInLandUseSubID          = MFVarGetID (MDVarLandUseSpatialSub,       "-",  MFInput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInLandUseAgID           = MFVarGetID (MDVarLandUseSpatialAg,        "-",  MFInput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInLandUseConID          = MFVarGetID (MDVarLandUseSpatialCon,       "-",  MFInput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInLandUseDecID          = MFVarGetID (MDVarLandUseSpatialDec,       "-",  MFInput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInLandUseMixID          = MFVarGetID (MDVarLandUseSpatialMix,       "-",  MFInput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInH2OFractionID         = MFVarGetID (MDVarH2OFracSpatial,          "-",  MFInput, MFState, MFBoundary)) == CMfailed) ||		//commented out 082812
            ((_MDInLocalLoad_Con_DINID   = MFVarGetID (MDVarInLocalLoadConDIN,    "g/m2",  MFInput, MFFlux, MFBoundary)) == CMfailed) || 
            ((_MDInLocalLoad_Dec_DINID   = MFVarGetID (MDVarInLocalLoadDecDIN,    "g/m2",  MFInput, MFFlux, MFBoundary)) == CMfailed) || 
            ((_MDInLocalLoad_Mix_DINID   = MFVarGetID (MDVarInLocalLoadMixDIN,    "g/m2",  MFInput, MFFlux, MFBoundary)) == CMfailed) || 
            ((_MDInLocalLoad_Lak_DINID   = MFVarGetID (MDVarInLocalLoadLakDIN,    "g/m2",  MFInput, MFFlux, MFBoundary)) == CMfailed) || 
            ((_MDOutLocalLoad_DINID      = MFVarGetID (MDVarLocalLoadDIN,         "kg/day", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||	//RJS 050511
            ((_MDOutLocalConc_DINID      = MFVarGetID (MDVarLocalConcDIN,         "mg/L", MFOutput, MFState, MFBoundary)) == CMfailed) ||	//RJS 050511
	    ((_MDOutLocalLoad_PnET_DINID = MFVarGetID (MDVarLocalLoadPnETDIN,      "kg/day", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||	//RJS 050511
	    ((_MDOutLocalLoad_Sub_DINID  = MFVarGetID (MDVarLocalLoadSubDIN,      "kg/day", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||	//RJS 050511
	    ((_MDOutLocalLoad_Agr_DINID  = MFVarGetID (MDVarLocalLoadAgDIN,      "kg/day",  MFOutput, MFFlux, MFBoundary)) == CMfailed) ||	//RJS 050511
            ((_MDOutLocalLoad_Lak_DINID  = MFVarGetID (MDVarLocalLoadLakDIN,      "kg/day", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||	//RJS 050511
	    ((_MDOutLocalLoad_For_DINID  = MFVarGetID (MDVarLocalLoadForDIN,      "kg/day",  MFOutput, MFFlux, MFBoundary)) == CMfailed) ||	//RJS 050511
   	    ((_MDOutLocalConc_Sub_DINID  = MFVarGetID (MDVarLocalConcSubDIN,      "mg/L", MFOutput, MFState, MFBoundary)) == CMfailed) ||	//RJS 050511
	    ((_MDOutLocalConc_Agr_DINID  = MFVarGetID (MDVarLocalConcAgDIN,      "mg/L",  MFOutput, MFState, MFBoundary)) == CMfailed) ||	//RJS 050511
            ((_MDOutLocalConc_Lak_DINID  = MFVarGetID (MDVarLocalConcLakDIN,      "mg/L", MFOutput, MFState, MFBoundary)) == CMfailed) ||	//RJS 050511
	    ((_MDOutLocalConc_For_DINID  = MFVarGetID (MDVarLocalConcForDIN,      "mg/L",  MFOutput, MFState, MFBoundary)) == CMfailed) ||	//RJS 050511
     (MFModelAddFunction (_MDNitrogenInputsInput2) == CMfailed)) return (CMfailed);
		break;

	case MDcalculate:
	if (((_MDInRunoffID   			 = MFVarGetID (MDVarRunoff,              "mm",  MFInput,  MFFlux, MFBoundary)) == CMfailed) ||	//RJS 10-28-10
	    ((_MDInRunoffVolID           = MDRunoffVolumeDef ()) == CMfailed) ||			// RJS 10-28-10
//	    ((_MDInDINLoadConcID         = MFVarGetID (MDVarDINLoadConc,       "mg/L",  MFInput, MFFlux, MFBoundary))  == CMfailed) ||	//RJS 10-28-10
//	    ((_MDInLandUseID             = MFVarGetID (MDVarLandUseSpatial,       "-",  MFInput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInLandUseSubID             = MFVarGetID (MDVarLandUseSpatialSub,       "-",  MFInput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInLandUseAgID             	= MFVarGetID (MDVarLandUseSpatialAg,       "-",  MFInput, MFState, MFBoundary)) == CMfailed) ||
//	    ((_MDInTotalPointID			 = MDPointSourceDef ()) == CMfailed) ||															//MMM Added this so that Nitrogeninputs knows it must run MDPointSource in order to calculate localload
	    ((_MDInLoadAdjustID          = MFVarGetID (MDVarLoadAdjust,           "-", MFInput, MFState, MFBoundary)) == CMfailed)  ||  // RJS 112211
	    ((_MDOutLocalLoad_Sub_DINID     = MFVarGetID (MDVarLocalLoadSubDIN,    "kg/day", MFOutput, MFFlux, MFBoundary))  == CMfailed) ||	//KAW 2013 05 08
	    ((_MDOutLocalLoad_Agr_DINID      = MFVarGetID (MDVarLocalLoadAgDIN,    "kg/day", MFOutput, MFFlux, MFBoundary))  == CMfailed) ||	//KAW 2013 05 08
	    ((_MDOutDINSubLoadConcID        = MFVarGetID (MDVarDINSubLoadConc,      "mg/L",  MFOutput, MFState, MFBoundary)) == CMfailed) || 	//KAW 2013 05 08
	    ((_MDOutDINAgLoadConcID        	= MFVarGetID (MDVarDINAgLoadConc,      "mg/L",  MFOutput, MFState, MFBoundary)) == CMfailed) || 	//KAW 2013 05 08
	    ((_MDOutLocalLoad_DINID      = MFVarGetID (MDVarLocalLoadDIN,    "kg/day", MFOutput, MFFlux, MFBoundary))  == CMfailed) ||	//RJS 10-28-10
	    ((_MDOutDINLoadConcID        = MFVarGetID (MDVarDINLoadConc,      "mg/L",  MFOutput, MFState, MFBoundary)) == CMfailed) || 	// KYLE
	    ((_MDInRiverOrderID          = MFVarGetID (MDVarRiverOrder,           "-", MFInput,  MFState, MFBoundary)) == CMfailed) ||  //RJS 10-29-10
	    (MFModelAddFunction (_MDNitrogenInputsCalc) == CMfailed)) return (CMfailed);
	   break;

	default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}

	MFDefLeaving ("Nitrogen Inputs");
	return (_MDOutLocalLoad_DINID);

}


