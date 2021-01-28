/******************************************************************************
Calculating the Sediment Flux with the BQART model: Qs=w*B*Q^n1*A^n2*R*T
where Q is discharge [km3/yr] (calculated by WBM), A is the contributing area [km2],
R is the maximum Relief [km], T is average temperature [c] and B is for geological 
and human factors.

MDSedimentFlux.c
WBMsedNEWS1.2 - reservoir trapping Te was changed to exclude the small res calculation 
module (as a new larger reservoir dataset is now in use) and reduce the minimum threshold
from >0.1 to >0.0
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>
#include <math.h>
#include <stdlib.h>

// Input
static int _MDInDischargeID           = MFUnset;
static int _MDInDischMeanID           = MFUnset;
static int _MDInAirTempID             = MFUnset;
static int _MDInContributingAreaAccID = MFUnset;
static int _MDInReliefID              = MFUnset;
static int _MDInAirTempAcc_timeID     = MFUnset;
static int _MDInTimeStepsID           = MFUnset;
static int _MDInIceCoverID            = MFUnset;
static int _MDInSmallResCapacityID    = MFUnset;
static int _MDInBQART_LithologyID     = MFUnset;
static int _MDInBQART_GNPID           = MFUnset;
static int _MDInPopulationID          = MFUnset;
static int _MDInResCapacityAccID      = MFUnset;
static int _MDInResCapacityID         = MFUnset;
static int _MDInTeAaccID              = MFUnset;
static int _MDOutBQART_TeID           = MFUnset; 
static int _MDOutPopulationAccID      = MFUnset;
static int _MDOutMeanGNPID            = MFUnset;
static int _MDOutBQART_EhID           = MFUnset;
static int _MDInAirTempAcc_spaceID    = MFUnset;
static int _MDInUpStreamQsID 	      = MFUnset;
static int _MDInBedloadFluxID 	      = MFUnset;
static int _MDInMDVarSedPristineID    = MFUnset;		
// Output
static int _MDOutSedimentFluxID       = MFUnset;
static int _MDOutBQART_BID            = MFUnset;
static int _MDOutBQART_Qbar_km3yID    = MFUnset;
static int _MDOutBQART_Qbar_m3sID     = MFUnset;
static int _MDOutBQART_AID            = MFUnset;
static int _MDOutBQART_RID            = MFUnset;
static int _MDOutBQART_TID            = MFUnset;
static int _MDOutPopulationDensityID  = MFUnset;
static int _MDOutGNPAreaAccID         = MFUnset;
static int _MDOutQs_barID             = MFUnset;
static int _MDOutLithologyAreaAccID   = MFUnset;
static int _MDOutLithologyMeanID      = MFUnset;
static int _MDOutDeltaQsID            = MFUnset;
static int _MDOutQsConcID             = MFUnset;
static int _MDOutQsYieldID            = MFUnset;

static void _MDSedimentFlux (int itemID) {
	float Qs, Qsbar;
	float w  = 0.02; //in kg/s/km2/C
	float n1 = 0.31;
	float n2 = 0.5;
	int   TimeStep,SedPristine;
	float Qday, DischMean, Qbar_m3s,Qbar_km3y;//, Qacc;
	float Tday,Tbar,Tacc,T_time;
	float B, I, L, Te, Eh,upLithologyArea;// ,Lbar
	float A, R;
	float Ag;
	float tmp,TupSlop,PixSize_km2;
	float ResCapacity, ResCapacityAcc, TeQacc ,deltaTau,upStreamResCapacity,LargeResCapacity,SmallRecCapacity;
//	float SmallRecCapacity;
	float PopulationAcc,PopuDesity, MeanGNP,GNPAreaAcc;
	static float dailyRand, yearlyRand;
//	static int tmpTimeStep,tempTimeStep;
//	static int dayCount;
	float s,sigma,cbar, Psi, C;
	float QsConc,QsYield; //upstream_Qs,deltaQs,
	float percentDiff=0;
//Geting the values of these parameters
	PixSize_km2 =(MFModelGetArea(itemID)/pow(1000,2));
	Qday = MFVarGetFloat (_MDInDischargeID   , 	itemID, 0.0);	// in m3/s	
	DischMean = MFVarGetFloat (_MDInDischMeanID, 	itemID, 0.0);	// in m3/s
	Tday = MFVarGetFloat (_MDInAirTempID, 		itemID, 0.0);	// in C	
	R    = MFVarGetFloat (_MDInReliefID, 		itemID, 0.0);	// in m 
//Calculating contributing area for each pixel
	A = MFVarGetFloat (_MDInContributingAreaAccID, itemID, 0.0) + (MFModelGetArea (itemID)/(pow(1000,2)));// convert from m2 to km2  //calculating the contributing area
	MFVarSetFloat (_MDInContributingAreaAccID, itemID, A);
	//A    = MFVarGetFloat (_MDInContributingAreaID, 	itemID, 0.0);	//in km2
	//if (A <= 0) A = PixSize_km2;
	R = R/1000; // convert to km	
	if (R <= 0) R = 0.00005;
//Calculating maximum Relief for each pixel


//Accumulate temperature
	T_time = (MFVarGetFloat (_MDInAirTempAcc_timeID, itemID, 0.0) + Tday); 
	MFVarSetFloat (_MDInAirTempAcc_timeID, itemID, T_time);	
	TupSlop = MFVarGetFloat (_MDInAirTempAcc_spaceID, itemID, 0.0); 
	Tacc = TupSlop + (T_time * PixSize_km2);
	MFVarSetFloat (_MDInAirTempAcc_spaceID, itemID, Tacc);

	TimeStep = (MFVarGetInt (_MDInTimeStepsID, itemID, 0) + 1);
	MFVarSetInt (_MDInTimeStepsID, itemID, TimeStep); //	!!! Chnaged for constant 7/10/10
	MFVarSetFloat (_MDOutBQART_AID, itemID, A);
	MFVarSetFloat (_MDOutBQART_RID, itemID, R);
//Calculate moving avarege temperature (Tbar) and discharge
	Tbar = Tacc/TimeStep/A;
	MFVarSetFloat (_MDOutBQART_TID, itemID, Tbar);
	Qbar_m3s = DischMean; //in m3/s
	//Qbar_m3s = Qacc/TimeStep; //in m3/s
	MFVarSetFloat (_MDOutBQART_Qbar_m3sID, itemID, Qbar_m3s); 
	if (Qbar_m3s  > 0){ //convert to km3/y	
		Qbar_km3y = (Qbar_m3s/(pow(1000,3)))*(365*24*60*60);
 	}else	Qbar_km3y = 0;
	MFVarSetFloat (_MDOutBQART_Qbar_km3yID, itemID, Qbar_km3y); // in km3/yr
// Calculating B
	Ag = MFVarGetFloat (_MDInIceCoverID, itemID, 0.0);	//in %	
	I  = 1 + 0.09 * Ag; 
	
	//Calculating catchmant average lithology
	L  = MFVarGetFloat (_MDInBQART_LithologyID, itemID, 0.0);	//no units
	if (L <= 0) L = 1.0;
	upLithologyArea =  MFVarGetFloat(_MDOutLithologyAreaAccID, itemID, 0.0) + L * (MFModelGetArea (itemID)/pow(1000,2));
	MFVarSetFloat (_MDOutLithologyAreaAccID, itemID, upLithologyArea);
	//Lbar = upLithologyArea/A;
	MFVarSetFloat (_MDOutLithologyMeanID, itemID, (upLithologyArea/A));
	L  = MFVarGetFloat(_MDOutLithologyMeanID, itemID, 0.0);	//no units
	if (L <= 0) L = 1.0;
	SedPristine = MFVarGetInt (_MDInMDVarSedPristineID, itemID, 0.0);
	if (SedPristine == 0 ){ //NOT pristine -- disturbed
		// Calculating reservoir trapping (Te)
		ResCapacity = 0.0;
		ResCapacityAcc = 0.0;
		//TeQacc = 0.0;
		SmallRecCapacity = MFVarGetFloat (_MDInSmallResCapacityID, itemID, 0.0)/(pow(1000,3)); //convert from m3 to km3
		if (SmallRecCapacity < 0) SmallRecCapacity = 0.00;
		LargeResCapacity = MFVarGetFloat (_MDInResCapacityID,	   itemID, 0.0);
		if (LargeResCapacity < 0.0001) LargeResCapacity = 0.0000000;
		ResCapacity = SmallRecCapacity + LargeResCapacity;
		//ResCapacity = LargeResCapacity;
		if (ResCapacity < 0) ResCapacity = 0.00;
	
		//Calculating basin trapping efficiency Using Vorosmarty et al. 1997 method
		upStreamResCapacity = MFVarGetFloat (_MDInResCapacityAccID, itemID, 0.0);
		ResCapacityAcc = upStreamResCapacity + ResCapacity;
		MFVarSetFloat (_MDInResCapacityAccID, itemID, ResCapacityAcc);
		//TeAacc = MFVarGetFloat (_MDInTeAaccID, itemID, 0.0); ///new
		TeQacc = MFVarGetFloat (_MDInTeAaccID, itemID, 0.0); ///new
		if (ResCapacity > 0.0001){  //reservoir pixel
			deltaTau = ResCapacityAcc/Qbar_km3y; ///may want to change to daily Q!!!!!!
			Te = 1 - (0.05/pow(deltaTau, 0.5));
			TeQacc = Te * Qbar_km3y;
			//TeQacc = TeQacc + Te * Qbar_km3y;
			MFVarSetFloat (_MDInTeAaccID, itemID, TeQacc);
		} else{ // non reservoir pixel basin "outlet"
				Te = TeQacc / Qbar_km3y;
			}
		if (Qbar_km3y == 0.0) Te = 0.0;
		if (Te > 1) Te = 1.0;
		if (Te < 0) Te = 0.0;

		//Calculating Eh
		// Calculating mean population density
		PopulationAcc = MFVarGetFloat(_MDOutPopulationAccID, itemID, 0.0) + MFVarGetFloat(_MDInPopulationID, itemID, 0.0); 
		MFVarSetFloat (_MDOutPopulationAccID, itemID, PopulationAcc);
		PopuDesity = PopulationAcc/A;
		MFVarSetFloat (_MDOutPopulationDensityID, itemID, PopuDesity);
		// Calculating mean GNP
		GNPAreaAcc = MFVarGetFloat(_MDOutGNPAreaAccID, itemID, 0.0) + (MFVarGetFloat(_MDInBQART_GNPID, itemID, 0.0) * PixSize_km2);
		MFVarSetFloat (_MDOutGNPAreaAccID, itemID, GNPAreaAcc);
		if (GNPAreaAcc == 0)
			MeanGNP = 0;
		else
			MeanGNP = GNPAreaAcc/A;
		MFVarSetFloat (_MDOutMeanGNPID, itemID, MeanGNP);
	
		Eh = 1.0;
		if (MeanGNP > 20000){
			if (PopuDesity > 30)Eh = 0.3;
		}
		if (MeanGNP < 2500){
			if (PopuDesity > 140)Eh = 2.0;
		}	
	
	}
	if (SedPristine == 1) { // Pristine sediment !!!!!
		Te = 0.0;
		Eh = 1.0;
	}	
	
	if (SedPristine == 2) { // semi-Pristine sediment (no reservoirs !!!!!
		//Calculating Eh
		// Calculating mean population density
		PopulationAcc = MFVarGetFloat(_MDOutPopulationAccID, itemID, 0.0) + MFVarGetFloat(_MDInPopulationID, itemID, 0.0); //devided by 25 for the 06min simulation in order to account for the smaler pixel size.
		MFVarSetFloat (_MDOutPopulationAccID, itemID, PopulationAcc);
		PopuDesity = PopulationAcc/A;
		MFVarSetFloat (_MDOutPopulationDensityID, itemID, PopuDesity);
		// Calculating mean GNP
		GNPAreaAcc = MFVarGetFloat(_MDOutGNPAreaAccID, itemID, 0.0) + (MFVarGetFloat(_MDInBQART_GNPID, itemID, 0.0) * PixSize_km2);
		MFVarSetFloat (_MDOutGNPAreaAccID, itemID, GNPAreaAcc);
		if (GNPAreaAcc == 0)
			MeanGNP = 0;
		else
			MeanGNP = GNPAreaAcc/A;
		MFVarSetFloat (_MDOutMeanGNPID, itemID, MeanGNP);
	
		Eh = 1.0;
		if (MeanGNP > 20000){
			if (PopuDesity > 30)Eh = 0.3;
		}
		if (MeanGNP < 2500){
			if (PopuDesity > 140)Eh = 2.0;
		}	
	
		Te = 0.0;
	}	//end SedPristine == 2
	
	MFVarSetFloat (_MDOutBQART_TeID, itemID, Te);	
	MFVarSetFloat (_MDOutBQART_EhID, itemID, Eh);
	
	B = I * L * (1 - Te) * Eh;
	if (B < 0){
		B = 0;
	}

	MFVarSetFloat (_MDOutBQART_BID, itemID, B);

// Calculating sediment flux (Qsbar)	
	if (Tbar>=2)	
		Qsbar = w * B * pow(Qbar_km3y,n1) * pow(A,n2) * R * Tbar; //in kg/s
	else
		Qsbar = 2*w * B * pow(Qbar_km3y,n1) * pow(A,n2) * R; //in kg/s
	
	MFVarSetFloat (_MDOutQs_barID, itemID, Qsbar);

// The Psi model. Calculating daily sediment flux (Qs).
	s     = 0.17 + (0.0000183 * Qbar_m3s); //Qbar in m3/s
	sigma = 0.763 * pow(0.99995, Qbar_m3s);//Qbar in m3/s
	//cbar  = 1.4 - (0.025 * Tbar) + (0.00013 * (R*1000)) + (0.145 *log10(Qsbar));//R in m; Qsbar in kg/s
	//cbar  = 0.1 - (0.025 * Tbar) + (0.00013 * (R*1000)) + (0.145 *log(Qsbar));//R in m; Qsbar in kg/s  !!! Changed 6/20/2013
	cbar  = 1.4 - (0.025 * Tbar) + (0.00013 * R) + (0.145 *log10(Qsbar));//R in km; Qsbar in kg/s !!!---- Fixed (missing'()') on 2/5/2019!!!
	//cbar  = 0.1 - (0.025 * Tbar) + (0.00013 * (R)) + (0.145 *log10(Qsbar));//R in km; Qsbar in kg/s  -- not good Cbar too low!!
	if (Qsbar == 0) cbar = 0;
	MFVarSetFloat (_MDOutDeltaQsID, itemID, cbar);

dailyRand = 0.00001; // Eliminate daily randomness !!!
yearlyRand = 0.00001;// Eliminate Yearly randomness!!!

	Psi= exp((sigma * dailyRand)); 
	
	C = s * yearlyRand + cbar;
	if (Qday > 0)
		Qs =  (Psi * Qsbar * pow(Qday/Qbar_m3s, C)); //Qs in kg/s?
	else Qs = 0;

	QsYield = Qs / A; // calculating basin sediment yield kg/s/km2
	if (Qday > 0)
		QsConc = Qs/Qday; // Sediment Concentration g/L
	else QsConc = 0;
		
	MFVarSetFloat (_MDOutSedimentFluxID, itemID, Qs);
	// Calculate Qs budget
	//upstream_Qs =  MFVarGetFloat (_MDInUpStreamQsID,itemID, 0.0);
	//deltaQs = upstream_Qs - Qs; //local bedload budget
//	MFVarSetFloat (_MDOutDeltaQsID, itemID, deltaQs); 
//	MFVarSetFloat (_MDInUpStreamQsID , itemID, (upstream_Qs*-1));
//	MFVarSetFloat (_MDInUpStreamQsID , itemID, Qs); 
	MFVarSetFloat (_MDOutQsConcID, itemID, QsConc);
	MFVarSetFloat (_MDOutQsYieldID, itemID, QsYield); 
}

enum { MDinput, MDcalculate, MDcorrected };

int MDSediment_FluxOLDDef() {
	
	MFDefEntering ("SedimentFlux");
	
	if (((_MDInDischargeID 		     = MDRouting_DischargeDef ())          == CMfailed) ||
		((_MDInSmallResCapacityID    = MDReservoir_FarmPondCapacityDef ()) == CMfailed) ||
	    ((_MDInDischMeanID 		     = MDAux_MeanDiscargehDef ())          == CMfailed) ||
	    ((_MDInAirTempID             = MFVarGetID (MDVarCommon_AirTemperature,              "degC",     MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInAirTempAcc_timeID     = MFVarGetID (MDVarSediment_AirTemperatureAcc_time,    "degC",     MFInput,  MFState, MFInitial))  == CMfailed) ||
	    ((_MDInTimeStepsID           = MFVarGetID (MDVarSediment_TimeSteps,                 MFNoUnit,   MFInput,  MFState, MFInitial))  == CMfailed) ||
	    ((_MDInReliefID              = MFVarGetID (MDVarSediment_Relief,                    "m",        MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInIceCoverID            = MFVarGetID (MDVarCommon_IceCover,                    MFNoUnit,	MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInBQART_LithologyID     = MFVarGetID (MDVarSediment_BQART_Lithology,           MFNoUnit,	MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInBQART_GNPID           = MFVarGetID (MDVarSediment_BQART_GNP,                 MFNoUnit,	MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInPopulationID          = MFVarGetID (MDVarSediment_Population,                MFNoUnit,   MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInAirTempAcc_spaceID    = MFVarGetID (MDVarSediment_AirTemperatureAcc_space,   "degC" ,    MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInResCapacityID         = MFVarGetID (MDVarReservoir_Capacity,                 "km3"	,   MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInTeAaccID              = MFVarGetID (MDVarSediment_TeAacc,                    MFNoUnit,   MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInContributingAreaAccID = MFVarGetID (MDVarSediment_ContributingAreaAcc,       "km2",      MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInUpStreamQsID          = MFVarGetID (MDVarSediment_UpStreamQs,                "kg/s",     MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInMDVarSedPristineID    = MFVarGetID (MDVarSediment_Pristine,                  MFNoUnit, 	MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutSedimentFluxID       = MFVarGetID (MDVarSediment_SuspendedFlux,             "kg/s",     MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutBQART_BID            = MFVarGetID (MDVarSediment_BQART_B,                   MFNoUnit,   MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutBQART_Qbar_m3sID     = MFVarGetID (MDVarSediment_BQART_Qbar_m3s,            "m3/s",     MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutBQART_Qbar_km3yID    = MFVarGetID (MDVarSediment_BQART_Qbar_km3y,           "km3/y",    MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutBQART_AID            = MFVarGetID (MDVarSediment_BQART_A,                   "km2",      MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutBQART_RID            = MFVarGetID (MDVarSediment_BQART_R,                   "km" ,      MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutBQART_TID            = MFVarGetID (MDVarSediment_BQART_T,                   "degC",     MFRoute,  MFState, MFBoundary)) == CMfailed) ||
   	    ((_MDOutPopulationAccID      = MFVarGetID (MDVarSediment_PopulationAcc,             MFNoUnit,   MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInResCapacityAccID      = MFVarGetID (MDVarSediment_ResStorageAcc,             "km3",      MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutPopulationDensityID  = MFVarGetID (MDVarSediment_PopulationDensity,         "km2",      MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutGNPAreaAccID         = MFVarGetID (MDVarSediment_GNPAreaAcc,                MFNoUnit,   MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutMeanGNPID            = MFVarGetID (MDVarSediment_MeanGNP,                   MFNoUnit,   MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutBQART_EhID           = MFVarGetID (MDVarSediment_BQART_Eh,                  MFNoUnit,   MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutBQART_TeID           = MFVarGetID (MDVarSediment_BQART_Te,                  MFNoUnit,   MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutQs_barID             = MFVarGetID (MDVarSediment_Qs_bar,                    "kg/s",     MFRoute,  MFState, MFBoundary)) == CMfailed) ||
 	    ((_MDOutLithologyAreaAccID   = MFVarGetID (MDVarSediment_LithologyAreaAcc,          MFNoUnit,   MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutLithologyMeanID      = MFVarGetID (MDVarSediment_LithologyMean,             MFNoUnit,   MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutDeltaQsID            = MFVarGetID (MDVarSediment_DeltaQs,                   "kg/s",     MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutQsConcID             = MFVarGetID (MDVarSediment_QsConc,                    "kg/m3",    MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutQsYieldID            = MFVarGetID (MDVarSediment_QsYield,                   "kg/s/km2", MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    (MFModelAddFunction (_MDSedimentFlux) == CMfailed)) return (CMfailed);

	MFDefLeaving  ("SedimentFlux");
	return (_MDOutSedimentFluxID);
}
