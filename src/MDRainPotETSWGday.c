/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDPotETSWGday.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <math.h>
#include <MF.h>
#include <MD.h>

static int _MDInDayLengthID     = MFUnset;
static int _MDInI0HDayID        = MFUnset;
static int _MDInCParamAlbedoID  = MFUnset;
static int _MDInCParamCHeightID = MFUnset;
static int _MDInCParamLWidthID  = MFUnset;
static int _MDInCParamRSSID     = MFUnset;
static int _MDInCParamR5ID      = MFUnset;
static int _MDInCParamCDID      = MFUnset;
static int _MDInCParamCRID      = MFUnset;
static int _MDInCParamGLMaxID   = MFUnset;
static int _MDInCParamZ0gID     = MFUnset;
static int _MDInLeafAreaIndexID = MFUnset;
static int _MDInStemAreaIndexID = MFUnset;

static int _MDInAtMeanID        = MFUnset;
static int _MDInAtMinID         = MFUnset;
static int _MDInSolRadID        = MFUnset;
static int _MDInVPressID        = MFUnset;
static int _MDInWSpeedID        = MFUnset;
static int _MDOutPetID          = MFUnset;

static void _MDRainPotETSWGday (int itemID) {
// daily Shuttleworth-Wallace-Gurney (1985, 1990) PE in mm for day
// Input
	float dayLen;  // daylength in fraction of day
 	float i0hDay;  // daily potential insolation on horizontal [MJ/m2]
	float albedo;  // albedo 
	float height;  // canopy height [m]
	float lWidth;  // average leaf width [m]
	float rss;     // soil surface resistance [s/m]
	float r5;      // solar radiation at which conductance is halved [W/m2]
	float cd;      // vpd at which conductance is halved [kPa]
	float cr;      // light extinction coefficient for projected LAI
	float glMax;   // maximum leaf surface conductance for all sides of leaf [m/s]
	float z0g;     // z0g       - ground surface roughness [m]
 	float lai;     // projected leaf area index
	float sai;     // projected stem area index
	float airT;    // air temperatur [degree C]
	float airTMin; // daily minimum air temperature [degree C] 
	float solRad;  // daily solar radiation on horizontal [MJ/m2]
	float vPress;  // daily average vapor pressure [kPa]
	float wSpeed;  // average wind speed for the day [m/s] 
	float sHeat = 0.0; // average subsurface heat storage for day [W/m2]
// Local
	float solNet;  // average net solar radiation for daytime [W/m2]
	float lngNet;  // average net longwave radiation [W/m2]
	float z0;      // roughness parameter [m] 
 	float disp;    // height of zero-plane [m]
	float z0c;     // roughness parameter (closed canopy)
	float dispc;   // zero-plane displacement (closed canopy)
	float aa;      // available energy [W/m2]
	float asubs;   // available energy at ground [W/m2]
	float es;      // vapor pressure at airT [kPa]
	float delta;   // dEsat/dTair [kPa/K]
 	float dd;      // vapor pressure deficit [kPa]
 	float rsc;     // canopy resistance [s/m]
	float le;      // latent heat [W/m2]
	float rn;      // net radiation [W/m2]
	float rns;     // net radiation at ground [W/m2]
	float raa;     // aerodynamic resistance [s/m]
	float rac;     // leaf boundary layer resistance [s/m]
	float ras;     // ground aerodynamic resistance [s/m]
// Output
	float pet;

	if (MFVarTestMissingVal (_MDInDayLengthID,    itemID) ||
		 MFVarTestMissingVal (_MDInI0HDayID,       itemID) ||
		 MFVarTestMissingVal (_MDInCParamAlbedoID, itemID) ||
		 MFVarTestMissingVal (_MDInCParamCHeightID,itemID) ||
		 MFVarTestMissingVal (_MDInCParamLWidthID, itemID) ||
		 MFVarTestMissingVal (_MDInCParamRSSID,    itemID) ||
		 MFVarTestMissingVal (_MDInCParamR5ID,     itemID) ||
		 MFVarTestMissingVal (_MDInCParamCDID,     itemID) ||
		 MFVarTestMissingVal (_MDInCParamCRID,     itemID) ||
		 MFVarTestMissingVal (_MDInCParamGLMaxID,  itemID) ||
		 MFVarTestMissingVal (_MDInCParamZ0gID,    itemID) ||
		 MFVarTestMissingVal (_MDInLeafAreaIndexID,itemID) ||
		 MFVarTestMissingVal (_MDInStemAreaIndexID,itemID) ||
		 MFVarTestMissingVal (_MDInAtMeanID,       itemID) ||
		 MFVarTestMissingVal (_MDInAtMinID,        itemID) ||
		 MFVarTestMissingVal (_MDInSolRadID,       itemID) ||
		 MFVarTestMissingVal (_MDInVPressID,       itemID) ||
		 MFVarTestMissingVal (_MDInWSpeedID,       itemID)) { MFVarSetMissingVal (_MDOutPetID,itemID); return; }

	dayLen  = MFVarGetFloat (_MDInDayLengthID,     itemID, 0.1);
	i0hDay  = MFVarGetFloat (_MDInI0HDayID,        itemID,  0.0);
	albedo  = MFVarGetFloat (_MDInCParamAlbedoID,  itemID,  0.0);
	height  = MFVarGetFloat (_MDInCParamCHeightID, itemID,  0.0);
	lWidth  = MFVarGetFloat (_MDInCParamLWidthID,  itemID,  0.0);
	rss     = MFVarGetFloat (_MDInCParamRSSID,     itemID,  0.0);
	r5      = MFVarGetFloat (_MDInCParamR5ID,      itemID,  0.0);
	cd      = MFVarGetFloat (_MDInCParamCDID,      itemID,  0.0);
	cr      = MFVarGetFloat (_MDInCParamCRID,      itemID,  0.0);
	glMax   = MFVarGetFloat (_MDInCParamGLMaxID,   itemID,  0.0);
	z0g     = MFVarGetFloat (_MDInCParamZ0gID,     itemID,  0.0);
	lai     = MFVarGetFloat (_MDInLeafAreaIndexID, itemID,  0.0);
	sai     = MFVarGetFloat (_MDInStemAreaIndexID, itemID,  0.0);
	airT    = MFVarGetFloat (_MDInAtMeanID,        itemID,  0.0);
	airTMin = MFVarGetFloat (_MDInAtMinID,         itemID,  0.0);
	solRad  = MFVarGetFloat (_MDInSolRadID,        itemID,  0.0);
	vPress  = MFVarGetFloat (_MDInVPressID,        itemID,  0.0);
	wSpeed  = fabs (MFVarGetFloat (_MDInWSpeedID,  itemID,  0.0));
	if (wSpeed < 0.2) wSpeed = 0.2;

	solNet  = (1.0 - albedo) * solRad / MDConstIGRATE;
	
	z0c     = MDPETlibRoughnessClosed (height,z0g);
	dispc   = height - z0c / 0.3;
	disp    = MDPETlibZPDisplacement (height,lai,sai,z0g);
	z0      = MDPETlibRoughness (disp,height,lai,sai,z0g);

	lngNet  = MDSRadNETLong (i0hDay,airT,solRad,vPress);
	rn      = solNet + lngNet;
	aa      = rn - sHeat;
	rns     = rn * exp (-cr * (lai + sai));
	asubs   = rns - sHeat;
	es      = MDPETlibVPressSat   (airT);
	delta   = MDPETlibVPressDelta (airT);
	dd      = es - vPress; 

	rsc     = MDPETlibCanopySurfResistance (airTMin,solRad,dd,lai,sai,r5,cd,cr,glMax);
	raa     = MDPETlibBoundaryResistance   (wSpeed,height,z0g,z0c,dispc,z0,disp);
	rac     = MDPETlibLeafResistance       (wSpeed,height,lWidth,z0g,lai,sai,z0c,dispc);
	ras     = MDPETlibGroundResistance     (wSpeed,height,z0g,z0c,dispc,z0,disp);
//	ras=70;
//	raa=50;
	rsc=70;
	le      = MDPETlibShuttleworthWallace  (rss,aa,asubs,dd,raa,rac,ras,rsc,delta);

	pet = MDConstEtoM * MDConstIGRATE * le;
//	float pet1 = MFVarGetFloat (_MDInRefETPIDDEBUG,   itemID,  0.0);
//	printf ("height %f lWidth %f rss %f r4 %f cd %f cr %f glMax %f z0g %f lai %f sai %f \n",height , lWidth ,rss, r5 ,cd ,cr, glMax ,z0g ,lai , sai );
//	printf ("pet BMF %f pet FAO %f albedo %f\n",pet,pet1,albedo);
	MFVarSetFloat (_MDOutPetID,itemID,pet);
}

int MDRainPotETSWGdayDef () {
	if (_MDOutPetID != MFUnset) return (_MDOutPetID);

	MFDefEntering ("Rainfed Potential Evapotranspiration (Shuttleworth - Wallace [day])");
	if (((_MDInDayLengthID     = MDSRadDayLengthDef ()) == CMfailed) ||
	    ((_MDInI0HDayID        = MDSRadI0HDayDef    ()) == CMfailed) ||
	    ((_MDInCParamAlbedoID  = MDCParamAlbedoDef  ()) == CMfailed) ||
	    ((_MDInCParamCHeightID = MDCParamCHeightDef ()) == CMfailed) ||
	    ((_MDInCParamLWidthID  = MDCParamLWidthDef  ()) == CMfailed) ||
	    ((_MDInCParamRSSID     = MDCParamRSSDef     ()) == CMfailed) ||
	    ((_MDInCParamR5ID      = MDCParamR5Def      ()) == CMfailed) ||
	    ((_MDInCParamCDID      = MDCParamCDDef      ()) == CMfailed) ||
	    ((_MDInCParamCRID      = MDCParamCRDef      ()) == CMfailed) ||
	    ((_MDInCParamGLMaxID   = MDCParamGLMaxDef   ()) == CMfailed) ||
	    ((_MDInCParamZ0gID     = MDCParamZ0gDef     ()) == CMfailed) ||
	    ((_MDInLeafAreaIndexID = MDLeafAreaIndexDef ()) == CMfailed) ||
	    ((_MDInStemAreaIndexID = MDStemAreaIndexDef ()) == CMfailed) ||
	    ((_MDInSolRadID        = MDSolarRadDef      ()) == CMfailed) ||
	    ((_MDInAtMeanID  = MFVarGetID (MDVarAirTemperature, "degC",  MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInAtMinID   = MFVarGetID (MDVarAirTempMinimum, "degC",  MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInVPressID  = MFVarGetID (MDVarVaporPressure,  "kPa",   MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInWSpeedID  = MFVarGetID (MDVarWindSpeed,      "m/s",   MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutPetID    = MFVarGetID (MDVarRainPotEvapotrans,  "mm",    MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    (MFModelAddFunction (_MDRainPotETSWGday) == CMfailed)) return (CMfailed);
	MFDefLeaving  ("Rainfed Potential Evapotranspiration (Shuttleworth - Wallace [day])");
	return(_MDOutPetID);
}
