/******************************************************************************
Calculating the Sediment Flux with the BQART model: Qs=w*B*Q^n1*A^n2*R*T
where Q is discharge [km3/yr] (calculated by WBM), A is the contributing area [km2],
R is the maximum Relief [km], T is average temperature [c] and B is for geological 
and human factors.

MDSedimentFlux.c

*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>
#include <math.h>
#include <stdlib.h>

// Input
static int _MDInDischargeID   	   = MFUnset;
static int _MDInDischargeAccID	   = MFUnset;
static int _MDInResStorageID	   = MFUnset;
static int _MDInDischMeanID	  	   = MFUnset;
static int _MDInAirTempID   	   = MFUnset;
//static int _MDInContributingAreaID = MFUnset;
static int _MDInContributingAreaAccID = MFUnset;
static int _MDInReliefID 	   	   = MFUnset;
static int _MDInAirTempAcc_timeID  = MFUnset;
static int _MDInTimeStepsID 	   = MFUnset;
static int _MDInIceCoverID 	   	   = MFUnset;
static int _MDInSmallResCapacityID = MFUnset;
static int _MDInBQART_LithologyID  = MFUnset;
static int _MDInBQART_GNPID	   	   = MFUnset;
static int _MDInPopulationID	   = MFUnset;
static int _MDInResCapacityAccID   = MFUnset;
static int _MDInResCapacityID 	   = MFUnset;
static int _MDInTeAaccID		   = MFUnset;
static int _MDOutBQART_TeID 	   = MFUnset; 
static int _MDOutPopulationAccID   = MFUnset;
static int _MDOutMeanGNPID		   = MFUnset;
static int _MDOutBQART_EhID		   = MFUnset;
static int _MDInNewDischargeAccID  = MFUnset;
static int _MDInNewAirTempAcc_timeID = MFUnset;
static int _MDInNewTimeStepsID 	   = MFUnset;
static int _MDInNewSedimentAccID   = MFUnset;
static int _MDInAirTempAcc_spaceID = MFUnset;
static int _MDInUpStreamQsID 	   = MFUnset;
static int _MDInBedloadFluxID 	   = MFUnset;
// Output
static int _MDOutSedimentFluxID 	= MFUnset;
static int _MDOutBQART_BID			= MFUnset;
static int _MDOutBQART_Qbar_km3yID 	= MFUnset;
static int _MDOutBQART_Qbar_m3sID 	= MFUnset;
static int _MDOutBQART_AID 	= MFUnset;
static int _MDOutBQART_RID 	= MFUnset;
static int _MDOutBQART_TID 			= MFUnset;
static int _MDOutPopulationDensityID = MFUnset;
static int _MDOutGNPAreaAccID 		= MFUnset;
static int _MDOutQs_barID 			= MFUnset;
static int _MDInrnseedID 			= MFUnset; 
static int _MDOutLithologyAreaAccID  = MFUnset;
static int _MDOutLithologyMeanID = MFUnset;
static int _MDOutDeltaQsID = MFUnset;


static void _MDSedimentFlux (int itemID) {
	float Qs, Qsbar;
	float w  = 0.02; //in kg/s/km2/C
	float n1 = 0.31;
	float n2 = 0.5;
	int   TimeStep;
	float Qday, Qacc, Qbar_m3s,Qbar_km3y,DischMean;
	float Tday,Tbar,Tacc,T_time;
	float B, I, L, Te, Eh,Lbar,upLithologyArea; 
	float A, R;
	float Ag;
	float tmp,TupSlop,PixSize_km2;
	float ResCapacity, ResCapacityAcc, TeAacc ,deltaTau,upStreamResCapacity,SmallRecCapacity,LargeResCapacity;
	float PopulationAcc,PopuDesity, MeanGNP,GNPAreaAcc;
	static float dailyRand, yearlyRand;
	static int tmpTimeStep,tempTimeStep;
	static int dayCount;
	float s,sigma,cbar, Psi, C;
	float upstream_Qs,deltaQs;

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
	if (R == 0) R = 0.00005;
//Calculating maximum Relief for each pixel

	
// Geting the phase 1 (BQARTpreprocess) values for the first run 
	tmp= MFVarGetInt (_MDInNewTimeStepsID, itemID, 0.0); 
	if (tmp == 0) {
		T_time = MFVarGetFloat (_MDInAirTempAcc_timeID, itemID, 0.0);
		MFVarSetFloat (_MDInNewAirTempAcc_timeID, itemID, T_time);
		Qacc = MFVarGetFloat (_MDInDischargeAccID, itemID, 0.0);
		MFVarSetFloat (_MDInNewDischargeAccID, itemID, Qacc);
		TimeStep = MFVarGetInt (_MDInTimeStepsID, itemID, 0.0);
		MFVarSetInt (_MDInNewTimeStepsID, itemID, TimeStep);
	}
//Accumulate temperature
	T_time = (MFVarGetFloat (_MDInNewAirTempAcc_timeID, itemID, 0.0) + Tday); 
	MFVarSetFloat (_MDInNewAirTempAcc_timeID, itemID, T_time);	
	TupSlop = MFVarGetFloat (_MDInAirTempAcc_spaceID, itemID, 0.0); 
	Tacc = TupSlop + (T_time * PixSize_km2);
	MFVarSetFloat (_MDInAirTempAcc_spaceID, itemID, Tacc);

//Accumulate discharge
	Qacc = (MFVarGetFloat (_MDInNewDischargeAccID, itemID, 0.0)+ Qday);// in m3/s
	MFVarSetFloat (_MDInNewDischargeAccID, itemID, Qacc);			

// Accumulate time steps
	//TimeStep = MFVarGetInt (_MDInTimeStepsID, itemID, 0.0);
	//tempTimeStep = (MFVarGetInt (_MDInNewTimeStepsID, itemID, 0.0)+1);//		!!! Chnaged for constant 7/10/10
	TimeStep = (MFVarGetInt (_MDInNewTimeStepsID, itemID, 0.0) +1 );
	MFVarSetInt (_MDInNewTimeStepsID, itemID, TimeStep);		//	!!! Chnaged for constant 7/10/10
MFVarSetInt (_MDOutBQART_AID, itemID, A);
MFVarSetInt (_MDOutBQART_RID, itemID, R);
//Calculate moving avarege temperature (Tbar) and discharge
	Tbar = Tacc/TimeStep/A;
	MFVarSetFloat (_MDOutBQART_TID, itemID, Tbar);
	Qbar_m3s = Qacc/TimeStep; //in m3/s
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
	Lbar = upLithologyArea/A;
	MFVarSetFloat (_MDOutLithologyMeanID, itemID, (upLithologyArea/A));
	L  = MFVarGetFloat(_MDOutLithologyMeanID, itemID, 0.0);	//no units
	if (L <= 0) L = 1.0;
	
	// Calculating reservoir trapping (Te)
	ResCapacity = 0.0;
	ResCapacityAcc = 0.0;
	SmallRecCapacity = MFVarGetFloat (_MDInSmallResCapacityID, itemID, 0.0)/(pow(1000,3)); //convert from m3 to km3
	if (SmallRecCapacity < 0) SmallRecCapacity = 0.00;
	LargeResCapacity = MFVarGetFloat (_MDInResCapacityID,	   itemID, 0.0);
	if (LargeResCapacity < 0) LargeResCapacity = 0.00;
	ResCapacity = SmallRecCapacity + LargeResCapacity;
	if (ResCapacity < 0) ResCapacity = 0.00;
	
		//accumulate res. storage
	upStreamResCapacity = MFVarGetFloat (_MDInResCapacityAccID, itemID, 0.0);
	ResCapacityAcc = upStreamResCapacity + ResCapacity;
	MFVarSetFloat (_MDInResCapacityAccID, itemID, ResCapacityAcc);
	
	if (ResCapacity > 0.1){	
		if (ResCapacity > 0.5){
			deltaTau = ResCapacityAcc/Qbar_km3y; 
			Te = 1 - (0.05/pow(deltaTau, 0.5));

		}else	Te = 1.0 - (1.0 / (1 + 0.00021 * (ResCapacity/A)));
		
		TeAacc = (MFVarGetFloat (_MDInTeAaccID, itemID, 0.0)/A) + (Te*A);
		MFVarSetFloat (_MDInTeAaccID, itemID, TeAacc);
	}else{
		if (Qbar_km3y == 0.0){
			Te = 0.0;
		}else Te = MFVarGetFloat (_MDInTeAaccID, itemID, 0.0)/A ;
			
	}	
	MFVarSetFloat (_MDOutBQART_TeID, itemID, Te);

	//Calculating Eh
		// Calculating mean population density
	PopulationAcc = MFVarGetFloat(_MDOutPopulationAccID, itemID, 0.0) + MFVarGetFloat(_MDInPopulationID, itemID, 0.0)/25; //devided by 25 for the 06min simulation in order to account for the smaler pixel size.
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
	if (PopuDesity > 200){
		if (MeanGNP > 15000) Eh = 0.3;
		if (MeanGNP < 1000) Eh = 2.0;
	}
	
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
	cbar  = 1.4 - (0.025 * Tbar) + (0.00013 * (R*1000)) + (0.145 *log10(Qsbar));//R in m; Qsbar in kg/s
	if (Qsbar == 0) cbar = 0;
	
/*-------------------
 *  Local Variables
 *-------------------*/
float hydroran2(long *idum);
static float fac, rsq, v1, v2, unival1,unival2;
static int shift;
static long rnseed;
FILE * pFileDay;
FILE * pFileYear;

/*--------------------------------------------------------------------
 *  First generate a set of uniform random numbers in [0.0, 1.0].
 *  ran2 is from "Numerical Recipes in C", p282, 2nd ed.
 *  For the first deviate, use rnseed as the seed, making sure that
 *  it is negative, this initializes ran2 appropriately.
 *  For subsequent years, use the generated seed; dumlong should not
 *  be altered between successive deviates in a sequence.
 *--------------------------------------------------------------------*/
/*---------------------------------------------------------
 *  Next generate Gaussian distributed deviates.
 *  The routine returns two random numbers for each pass,
 *  so loop through the array at a step of 2.
 *  GASDEV, From "Numerical Recipes in C", p.289, 2nd ed.
 *---------------------------------------------------------*/

	if (tmpTimeStep != TimeStep){
		if (tmp == 0.0)
			rnseed = -850; 
		else
			if (rnseed == 0.0) {
				rnseed = MFVarGetFloat (_MDInrnseedID, itemID, 0.0);
			}	
		if (shift == 0){		
			do{
				unival1 = hydroran2(&rnseed);
				unival2 = hydroran2(&rnseed);
			      	v1 = 2.0 * unival1 - 1.0;
			      	v2 = 2.0 * unival2 - 1.0;
			      	rsq = (v1*v1) + (v2*v2);
			}while(rsq >= 1.0 || rsq == 0.0 );
			fac = sqrt(-2.0*log(rsq)/rsq);
			MFVarSetFloat (_MDInrnseedID, itemID, -(rnseed/1000000));
			dailyRand = (double)v1*fac;
			shift = 1;
		}else{
			dailyRand = (double)v2*fac;
			shift = 0;
		}
//dailyRand = 0.01;
		pFileDay = fopen ("dailyRand.txt","a");
		fprintf (pFileDay, "dailyRand, rnseed: %f %ld\n",dailyRand,rnseed);
		fclose (pFileDay);
		tmpTimeStep = TimeStep; 
		dayCount++;
	}

	if (dayCount == 1) {
		yearlyRand = (double)v1*fac;
		dayCount++;
		printf("yearlyRand: %f\n",yearlyRand);
		pFileYear = fopen ("YearlyRand.txt","a");		
		fprintf (pFileYear, "yearlyRand, rnseed %f %ld\n",yearlyRand,rnseed);
		fclose (pFileYear);
	}
	

	Psi= exp((sigma * dailyRand)); 
	
	C = s * yearlyRand + cbar;
	if (Qday > 0)
		Qs =  (Psi * Qsbar * pow(Qday/Qbar_m3s, C)); //Qs in kg/s?
	else Qs = 0;
	MFVarSetFloat (_MDOutSedimentFluxID, itemID, Qs);
	// Calculate Qs budget
	upstream_Qs =  MFVarGetFloat (_MDInUpStreamQsID,itemID, 0.0);
	deltaQs = upstream_Qs - Qs; //local bedload budget
	MFVarSetFloat (_MDOutDeltaQsID, itemID, deltaQs); 
	MFVarSetFloat (_MDInUpStreamQsID , itemID, (upstream_Qs*-1));
	MFVarSetFloat (_MDInUpStreamQsID , itemID, Qs);  

}

enum { MDinput, MDcalculate, MDcorrected };

int MDSedimentFluxDef() {
	
	MFDefEntering ("SedimentFlux");
	
	if (((_MDInDischargeID 		  = MDDischargeBFDef   ()) == CMfailed) || 
		((_MDInSmallResCapacityID = MDSmallReservoirCapacityDef  ()) == CMfailed) ||
	    ((_MDInDischMeanID 		  =	MDDischMeanDef     ()) 			 == CMfailed) ||
	    ((_MDInBedloadFluxID 	  =	MDBedloadFluxDef   ())			 == CMfailed) ||

    ((_MDInAirTempID 	  		= MFVarGetID (MDVarAirTemperature,		   "degC",  MFInput, MFState, MFBoundary)) == CMfailed) ||
	((_MDInAirTempAcc_timeID   	= MFVarGetID (MDVarAirTemperatureAcc_time, "degC",	MFInput, MFState, MFBoundary)) == CMfailed) ||
	((_MDInDischargeAccID  		= MFVarGetID (MDVarDischargeAcc,     	   "m3/s",	MFInput, MFState, MFBoundary)) == CMfailed) ||
	((_MDInTimeStepsID     		= MFVarGetID (MDVarTimeSteps,      		 MFNoUnit,	MFInput, MFState, MFBoundary)) == CMfailed) ||
	//((_MDInElevationID 	= MFVarGetID (MDVarontributingArea,		"km2", 	MFInput, MFState, MFBoundary)) == CMfailed) ||
	((_MDInReliefID   			= MFVarGetID (MDVarRelief,					  "m", 	MFInput, MFState, MFBoundary)) == CMfailed) ||
	((_MDInIceCoverID   		= MFVarGetID (MDVarIceCover,	     	MFNoUnit,	MFInput, MFState, MFBoundary)) == CMfailed) ||
	((_MDInBQART_LithologyID 	= MFVarGetID (MDVarBQART_Lithology, 	MFNoUnit,	MFInput, MFState, MFBoundary)) == CMfailed) ||
	((_MDInBQART_GNPID  	 	= MFVarGetID (MDVarBQART_GNP,	       	MFNoUnit,	MFInput, MFState, MFBoundary)) == CMfailed) ||
	((_MDInPopulationID	   	= MFVarGetID (MDVarPopulation,     		MFNoUnit,	MFInput, MFState, MFBoundary)) == CMfailed) ||
    	((_MDInNewAirTempAcc_timeID = MFVarGetID (MDVarNewAirTemperatureAcc_time,"degC",MFOutput,MFState, MFInitial)) == CMfailed) ||
	((_MDInAirTempAcc_spaceID = MFVarGetID (MDVarAirTemperatureAcc_space,"degC"  ,MFRoute, MFState, MFBoundary)) == CMfailed) ||
	((_MDInNewDischargeAccID  = MFVarGetID (MDVarNewDischargeAcc,     "m3/s"	,MFOutput, MFState, MFInitial)) == CMfailed) ||
	((_MDInNewSedimentAccID   = MFVarGetID (MDVarNewSedimentAcc,      "m3/s"	,MFOutput, MFState, MFInitial)) == CMfailed) ||
	((_MDInNewTimeStepsID     = MFVarGetID (MDVarNewTimeSteps,      MFNoUnit	,MFOutput, MFState, MFInitial)) == CMfailed) ||
	((_MDInResCapacityID   	  = MFVarGetID (MDVarReservoirCapacity,    "km3"	,MFInput,  MFState, MFBoundary)) == CMfailed) ||
	((_MDInTeAaccID  	  = MFVarGetID (MDVarTeAacc,     		 	 ""		,MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	((_MDInrnseedID  		= MFVarGetID (MDVarrnseed,     		 	 ""		,MFOutput, MFState, MFInitial)) == CMfailed) ||	
	((_MDInContributingAreaAccID= MFVarGetID (MDVarContributingAreaAcc,		"km2", 	 MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	((_MDInUpStreamQsID  		= MFVarGetID (MDVarUpStreamQs,  "kg/s",	MFRoute, MFState, MFBoundary)) == CMfailed) ||
//	((_MDInAreaID  = MFVarGetID (MDVarArea, "km2", MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	// output
    ((_MDOutSedimentFluxID  	= MFVarGetID (MDVarSedimentFlux, 	"kg/s"		,MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	((_MDOutBQART_BID  			= MFVarGetID (MDVarBQART_B, 	  MFNoUnit		,MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	((_MDOutBQART_Qbar_m3sID 	= MFVarGetID (MDVarBQART_Qbar_m3s, 	 "m3s",  MFOutput,  MFState, MFBoundary)) == CMfailed) ||
	((_MDOutBQART_Qbar_km3yID 	= MFVarGetID (MDVarBQART_Qbar_km3y,"km3/y",  MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	((_MDOutBQART_AID  		= MFVarGetID (MDVarBQART_A, 	"km2", 	  MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	((_MDOutBQART_RID  		= MFVarGetID (MDVarBQART_R, 	"km" , 	  MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	((_MDOutBQART_TID  			= MFVarGetID (MDVarBQART_T, 		"degC", MFRoute,  MFState, MFBoundary)) == CMfailed) ||
   	((_MDOutPopulationAccID 	= MFVarGetID (MDVarPopulationAcc, 		"", MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	((_MDInResCapacityAccID 	= MFVarGetID (MDVarResStorageAcc, 	 "km3", MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	((_MDOutPopulationDensityID = MFVarGetID (MDVarPopulationDensity,"km2",	MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutGNPAreaAccID  		= MFVarGetID (MDVarGNPAreaAcc,  	" ", 	MFRoute, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutMeanGNPID  			= MFVarGetID (MDVarMeanGNP,  		" ", 	MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutBQART_EhID  		= MFVarGetID (MDVarBQART_Eh, 		 " ", 	MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutBQART_TeID  		= MFVarGetID (MDVarBQART_Te, 		 " ", 	MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutQs_barID			= MFVarGetID (MDVarQs_bar, 		"kg/s",MFOutput, MFState, MFBoundary)) == CMfailed) ||
 	((_MDOutLithologyAreaAccID 	= MFVarGetID (MDVarLithologyAreaAcc,	"",	MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	((_MDOutLithologyMeanID  	= MFVarGetID (MDVarLithologyMean,	"" , 	MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutDeltaQsID			= MFVarGetID (MDDeltaQs  			,"kg/s",MFOutput, MFState, MFBoundary)) == CMfailed) ||
	(MFModelAddFunction (_MDSedimentFlux) == CMfailed)) return (CMfailed);

	MFDefLeaving  ("SedimentFlux");
	return (_MDOutSedimentFluxID);
}
