/******************************************************************************
Calculating the Bedload Flux using the modified Begnold(1966) equation

MDBedloadFlux.c

Updated 3/28/2019 to include critical velocity parameter

Eqations:
Modified Bagnold (1966): [Ps/Ps-P][(Pg(Q^b)S)/(g tan(f)] ~~ constant slope (on/off)
Lammers & Bledsoe (2018): 0.004((PgQS/w)-0.1]^1.5)(Ds^-0.5)(Q/w)-0.5)*w ~~constant slope (on/off)
Ashley: 45.7(Q^0.69)(S^0.88)(Qs^0.31); Empirical was: 0.02(Q^0.7)(Qs^0.25)(V^-1.2)
Syvitski et al., (2019): (Ps/Ps-Pf)pf (Q^b)S (0.01 V/u)
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>
#include <math.h>
#include <stdlib.h>

// Input
static int _MDInDischargeID   		= MFUnset;
static int _MDInDischMeanID	  	   	= MFUnset;
static int _MDInRiverSlopeID	    = MFUnset;
//static int _MDInUpStreamQbID	    = MFUnset;
static int _MDInMDVarVelocityTHID	    = MFUnset;
static int _MDInRiverbedVelocityMeanID  = MFUnset;
static int _MDInSedimentFluxID = MFUnset;
//static int _MDInRiverbedWidthMeanID = MFUnset;
static int _MDInMDVarBedloadEquationID = MFUnset;
static int _MDInWTemp_QxT 			= MFUnset;
//static int _MDInRiverWidthID 	= MFUnset;
//static int _MDInWaterDensityID	= MFUnset;
static int _MDInQsConcID			= MFUnset;
static int _MDInQs_barID 			= MFUnset;
// Output
static int _MDOutBedloadFluxID 		= MFUnset;
static int _MDOutSettlingVelocityID = MFUnset;
static int _MDOutKinematicViscosityID = MFUnset;
//static int _MDOutDeltaBedloadID 	= MFUnset;
static int _MDOutWaterDensityID		= MFUnset;
static int _MDOutStreamPowerID		= MFUnset;
static int _MDOutCritStreamPowerID	= MFUnset;
static int _MDOutParticleSizeID		= MFUnset;
static int _MDOutQb_bar_AshleyID	= MFUnset;

static void _MDBedloadFlux (int itemID) {
	float trnfac, rslope,Qday, Qsday, Qsbar, DischMean, alphabed, Qb_bar_ashley;//, pixel_length; 
	float rhofluid, rhosand, trneff,anglerep,degTOr;
	float Qb_kg_sec;//, Qb_kg_day;
	//float upstream_Qb, deltaQb;
	float velocityTH, vMean, settV, kinematicV, nomi,rho;
	float Qc,Ds,constDs, d, constSlope, streamPower, critStreamPower, rwidth, Tw, constTw;
	float g = 9.8;
	int bedloadEq;
	// Set parameters
	rhosand	 = 2670;			// Sand density
	trneff	 = 0.1;  			// Bedload efficency - changed from 0.1 on April 2019
	anglerep = 32.21;			// Limiting angle
	degTOr 	 = 1.745329252e-2;	// 2.0*PI/360.0 convert degrees to radians 
	alphabed = 1.0; 			// Q coefficient 
	constSlope = 0.001;			// river slope
	constDs = 0.001; 			// particle size (m)
	constTw = 20; 				// water temperature
	critStreamPower = 0.1; 		//critical stream power k/m/s, W/m2
	velocityTH = MFVarGetFloat (_MDInMDVarVelocityTHID, itemID, 0.0);
	vMean     = MFVarGetFloat (_MDInRiverbedVelocityMeanID,  itemID, 0.0);
	
	DischMean = MFVarGetFloat (_MDInDischMeanID, 	itemID, 0.0);	
	rwidth = 15.0 * pow(DischMean,0.5); //from Cohen et al. (2014)
	MFVarSetFloat (_MDOutSettlingVelocityID, itemID, rwidth);
	Qday = MFVarGetFloat (_MDInDischargeID ,itemID, 0.0);	// in m3/s	
	Qsday = MFVarGetFloat (_MDInSedimentFluxID ,itemID, 0.0);
	Qsbar = MFVarGetFloat (_MDInQs_barID ,itemID, 0.0);
	rslope = MFVarGetFloat (_MDInRiverSlopeID,itemID, 0.0);// in %	
	Tw =  MFVarGetFloat (_MDInWTemp_QxT ,itemID, 0.0);	// simulated water temperature in degC
	Qc = MFVarGetFloat(_MDInQsConcID ,itemID, 0.0);			// Fluid density
	rhofluid = 1000; 			// Constant Fluid density
	if (Qc > 0.0 ){
		rho = 1000*(1 - (Tw+288.9414)/(508929.2*(Tw+68.12963))* pow((Tw-3.9863),2));
		rhofluid = rho + Qc;
	}
	MFVarSetFloat (_MDOutWaterDensityID, itemID, rhofluid);
	
	Qb_bar_ashley = 2818 * pow(DischMean, 1.01) * pow(rslope, 1.49);
	MFVarSetFloat (_MDOutQb_bar_AshleyID, itemID, Qb_bar_ashley);
	Ds = 0.66 * pow(DischMean, 0.76) * pow(rslope, 1.12) * pow((Qsbar + Qb_bar_ashley),-0.39);// in [m]; by Tom Ashley (Apr 2020)
	MFVarSetFloat (_MDOutParticleSizeID, itemID, Ds);
	
	streamPower = (rhofluid * g * Qday * rslope) / rwidth; 
	critStreamPower = 0.1*(rhofluid * pow(((2.65-1)*g*Ds),1.5));
	MFVarSetFloat (_MDOutStreamPowerID, itemID, streamPower);
	MFVarSetFloat (_MDOutCritStreamPowerID, itemID, critStreamPower);
	
	Qb_kg_sec = 0.0;
	
	if (streamPower > critStreamPower){
		bedloadEq = MFVarGetInt (_MDInMDVarBedloadEquationID, itemID, 0.0);
		switch(bedloadEq){
			case 1:
			//Modified Bagnold~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				trnfac = (rhofluid * rhosand * trneff) / ((rhosand - rhofluid) * tan(anglerep*degTOr));
				MFVarSetFloat (_MDOutKinematicViscosityID, itemID, trnfac);
				Qb_kg_sec = trnfac * rslope * pow(Qday, alphabed); // in kg/s 
				break;
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			case 2:
				//Lammers & Bledsoe (2018): 0.000004((PgQS/w)-0.1]^1.5)(Ds^-0.5)(Q/w)-0.5)*w ~~~~~~~~~~~~~~~~~~~~
				//	printf("in Lammers & Bledsoe (2018)");
				Qb_kg_sec = (0.000143 * pow((streamPower - critStreamPower), 1.5) * pow(Ds,-0.5)* pow((Qday/rwidth),-0.5)) * rwidth;
				break;
				//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~	
			case 3:	
				//Ashley: 45.7(Q^0.69)(S^0.88)(Qs^0.31); OLD empirical0.02(Q^0.7)(Qs^0.25)(V^-1.2)~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				Qb_kg_sec  = 2818 * pow(Qday, 1.01) * pow(rslope, 1.49);
				//Qb_kg_sec = 45.7 * pow(Qday,0.69) * pow(Qsday,0.31) * pow(rslope,0.88);

				break;
				//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~	
			case 4:
				//Syvitski et al. (2019): (Ps/Ps-Pf)pf (Q^b)S (0.01 V/u) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
				//printf("in Syvitski et al. (2019)");
				kinematicV = (0.001791/(1+ (0.03368*Tw) + (0.00021 * pow(Tw,2))))/rhofluid;
				nomi = ((rhosand/rhofluid) - 1.0) * g;
				d = Ds * pow((nomi / (pow(kinematicV,2.0))), (1.0/3.0)); 
				settV = ((8.0 * kinematicV)/Ds) * (pow(1.0 + (0.139 * pow(d, 3.0)),0.5) - 1.0);
				Qb_kg_sec = ((rhosand/(rhosand-rhofluid))*rhofluid) * pow(Qday, alphabed) * rslope * (0.01*(vMean/settV));
				MFVarSetFloat (_MDOutSettlingVelocityID, itemID, settV);
				MFVarSetFloat (_MDOutKinematicViscosityID, itemID, kinematicV);
				break;	
				
		}
	}
	if(Qb_kg_sec < 0.0) Qb_kg_sec = 0.0 ; // in kg/s  
	MFVarSetFloat (_MDOutBedloadFluxID, itemID, Qb_kg_sec);
}

enum { MDinput, MDcalculate, MDcorrected };

int MDSediment_BedloadFluxDef() {
	
	MFDefEntering ("BedloadFlux");
	
	if (((_MDInDischargeID            = MDSediment_DischargeBFDef ()) == CMfailed) || 
	    ((_MDInSedimentFluxID         = MDSediment_FluxDef ())        == CMfailed) ||
	    ((_MDInWTemp_QxT              = MDTP2M_WTempRiverDef ())      == CMfailed) ||
	    ((_MDInDischMeanID            = MFVarGetID (MDVarAux_DischMean,               "m3/s",   MFRoute,  MFState, MFInitial))  == CMfailed) ||
	    ((_MDInQs_barID               = MFVarGetID (MDVarSediment_Qs_bar,             "kg/s",   MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInRiverSlopeID           = MFVarGetID (MDVarRouting_RiverSlope,          "m/km",   MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInMDVarVelocityTHID      = MFVarGetID (MDVarSediment_VelocityTH,         MFNoUnit, MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInRiverbedVelocityMeanID = MFVarGetID (MDVarRouting_RiverVelocityMean,   "m/s",    MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInMDVarBedloadEquationID = MFVarGetID (MDVarSediment_BedloadEquation,    MFNoUnit, MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInQsConcID               = MFVarGetID (MDVarSediment_QsConc,             "kg/m3",  MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutBedloadFluxID         = MFVarGetID (MDVarSediment_BedloadFlux,        "kg/s",   MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutSettlingVelocityID    = MFVarGetID (MDVarSediment_SettlingVelocity,   "m/s",    MFOutput, MFState, MFBoundary)) == CMfailed) ||
 	    ((_MDOutKinematicViscosityID  = MFVarGetID (MDVarSediment_KinematicViscosity, "m2/s",   MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutWaterDensityID        = MFVarGetID (MDVarSediment_WaterDensity,       "kg/m3",  MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutStreamPowerID         = MFVarGetID (MDVarSediment_StreamPower,        "W/m2",   MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutCritStreamPowerID     = MFVarGetID (MDVarSediment_CritStreamPower,    "W/m2",   MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutParticleSizeID        = MFVarGetID (MDVarSediment_ParticleSize,       "mm",     MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutQb_bar_AshleyID       = MFVarGetID (MDVarSediment_Qb_bar_Ashley,      "kg/s",   MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    (MFModelAddFunction (_MDBedloadFlux) == CMfailed)) return (CMfailed);

	MFDefLeaving  ("BedloadFlux");
	return (_MDOutBedloadFluxID);
}