/******************************************************************************
Calculating the Sediment Flux with the BQART model: Qs=w*B*Q^n1*A^n2*R*T
where Q is discharge [km3/yr] (calculated by WBM), A is the contributing area [km2],
R is the maximum Relief [km], T is average temperature [c] and B is for geological 
and human factors.

MDSedimentFlux.c

*******************************************************************************/

#include <math.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInDischargeID   	= MFUnset;
static int _MDInMinSlopeID	    = MFUnset;
static int _MDInUpStreamQbID	    = MFUnset;

// Output
static int _MDOutBedloadFluxID 	= MFUnset;
static int _MDOutDeltaBedloadID 	= MFUnset;

static void _MDBedloadFlux (int itemID) {
	float trnfac, rslope,Qday, alphabed,pixel_length; 
	float rhofluid, rhosand, trneff,rhosed,rhowater,anglerep,degTOr;
	float Qb_kg_sec, Qb_kg_day;
	float upstream_Qb, deltaQb,i;
char string [256];
	// Set parameters
	rhofluid = 1000; 			// Fluid density
	rhosand	 = 2670;			// Sand density
	trneff	 = 0.1;  			// Bedload efficency
	anglerep = 32.21;			// Limiting angle
	degTOr 	 = 1.745329252e-2;	// 2.0*PI/360.0 convert degrees to radians 
	alphabed = 1.0;
	
	trnfac = ( rhofluid * rhosand * trneff ) / ((rhosand - rhofluid) * tan( anglerep*degTOr ));
	//printf("Bedload,trnfac: %f\n",trnfac);
	pixel_length = pow( MFModelGetArea(itemID), 0.5 );
	//printf("pixel_length: %f\n",pixel_length);
	rslope = MFVarGetFloat (_MDInMinSlopeID,itemID, 0.0)/pixel_length;// in %	
	Qday = MFVarGetFloat (_MDInDischargeID ,itemID, 0.0);	// in m3/s	
	Qb_kg_sec = trnfac * rslope * pow( Qday, alphabed ); // in kg/s
	Qb_kg_day = Qb_kg_sec * (24*60*60); // in kg/s  
	MFVarSetFloat (_MDOutBedloadFluxID, itemID, Qb_kg_sec);
// Calculate Qb budget
	upstream_Qb =  MFVarGetFloat (_MDInUpStreamQbID,itemID, 0.0);
	deltaQb = upstream_Qb - Qb_kg_sec; //local bedload budget
	MFVarSetFloat (_MDOutDeltaBedloadID, itemID, deltaQb); 
	MFVarSetFloat (_MDInUpStreamQbID , itemID, (upstream_Qb*-1));
	MFVarSetFloat (_MDInUpStreamQbID , itemID, Qb_kg_sec);  
}

enum { MDinput, MDcalculate, MDcorrected };

int MDBedloadFluxDef() {
	
	MFDefEntering ("BedloadFlux");
	
	if (((_MDInDischargeID  = MDDischargeBFDef    ())			 == CMfailed) || 
	((_MDInMinSlopeID  		= MFVarGetID (MDVarMinSlope,  " ",	MFInput, MFState, MFBoundary)) == CMfailed) ||
	((_MDInUpStreamQbID  	= MFVarGetID (MDVarUpStreamQb,  " ",	MFRoute, MFState, MFBoundary)) == CMfailed) ||

	// output
 	((_MDOutBedloadFluxID	= MFVarGetID (MDVarBedloadFlux, "kg/s",MFOutput, MFState, MFBoundary)) == CMfailed) ||
 	((_MDOutDeltaBedloadID	= MFVarGetID (MDDeltaBedload  , "kg/s",MFOutput, MFState, MFBoundary)) == CMfailed) ||
	(MFModelAddFunction (_MDBedloadFlux) == CMfailed)) return (CMfailed);

	MFDefLeaving  ("BedloadFlux");
	return (_MDOutBedloadFluxID);
}
