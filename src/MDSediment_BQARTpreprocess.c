/******************************************************************************
Phase 1 of the WBMsed model.
Calculating the long-term temperature (Tbar)and discgarge (Qbar) averages BQART model:
Qs=w*B*Qbar^n1*A^n2*R*Tbar
This module temporally accumulate temperature and discharge and calculate the time steps
(i.e. iterations (days)) for each pixel. 
The yearly outputs of the last year are then exported to a new directory ('BQARTmeanInputs')  
and their date is remove (these staps  are handeled in the BQARTpreprocess.sh script).
These datasets are then used as inputs in the MDSedimentFlux.c module to calculate the 
averages needed for the BQART model (e.g. TempAcc/TimeSteps).  

MDQBARTpreprocess.c
sagy.cohen@colorado.edu.au
last update: May 16 2011
*******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>
#include <math.h>

// Input
static int _MDInDischargeID   	   = MFUnset;
static int _MDInDischargeAccID	   = MFUnset;
static int _MDInDischMeanID	   = MFUnset;
static int _MDInAirTempID   	   = MFUnset;
static int _MDInAirTempAcc_spaceID = MFUnset;
static int _MDInAirTempAcc_timeID = MFUnset;
static int _MDInTimeStepsID 	   = MFUnset;
static int _MDInContributingAreaAccID = MFUnset;
static int _MDInBankfullQ5ID =    MFUnset;

// Output
static int _MDOutBQART_Qbar_m3sID 	= MFUnset;
static int _MDOutBQART_Qbar_km3yID = MFUnset;
static int _MDOutBQART_TID 	= MFUnset;
static int _MDOutBQART_AID 	= MFUnset;
static int _MDOutBQART_RID 	= MFUnset;

static int _MDOutDischMaxID = MFUnset;
static int _MDOutYearCountID = MFUnset;
static int _MDOutLogQMaxM2ID = MFUnset;
static int _MDOutLogQMaxM3ID = MFUnset;
static int _MDOutMeanLogQMaxID = MFUnset;

// use global arrays rather than static local vars in function 


static void _MDQBARTpreprocess (int itemID) {
	int TimeStep;
	float Qday, Qbar,Qacc, Qbar_km3y,Qbar_m3s;
    float Qmax = 0;
	float Tday,Tbar,Tacc,A,T_time,T_old;
	float TupSlop,PixSize_km2;

    int n, nA; 
    float mu, muA, M2, M2A, M3, M3A, del;
    float logQmax;
    float dummy;


    if (MFDateGetDayOfYear() == 1) {
        MFVarSetFloat(_MDOutDischMaxID, itemID, 0.0);
    }


	Qday = MFVarGetFloat (_MDInDischargeID , itemID, 0.0);	// in m3/s	
    Qmax = MFVarGetFloat(_MDOutDischMaxID, itemID, 0.0);
    if (Qday > Qmax) {
        MFVarSetFloat(_MDOutDischMaxID, itemID, Qday);
    } else {
        MFVarSetFloat(_MDOutDischMaxID, itemID, Qmax);
    }


    if (MFDateGetDayOfYear() == MFDateGetYearLength()) {
        Qmax = MFVarGetFloat(_MDOutDischMaxID, itemID, 0.0);
        logQmax = 0;
        if (Qmax > 0) 
            logQmax = log10(Qmax);
        // online (onepass) variance calculation, continues in MDBankfullQcalc.c (T.Terriberry)
        nA = MFVarGetInt(_MDOutYearCountID, itemID, 0);
        n = nA + 1;
        muA = MFVarGetFloat(_MDOutMeanLogQMaxID, itemID, 0.0);
        M2A = MFVarGetFloat(_MDOutLogQMaxM2ID, itemID, 0.0);
        M3A = MFVarGetFloat(_MDOutLogQMaxM3ID, itemID, 0.0);
        del = logQmax - muA;
        mu = muA + del / n;
        M2 = M2A + (del * del * nA / n);
        M3 = M3A + (del * del * del * nA * (nA - 1) / (n * n)) + (3 * -M2A * del / n);

        MFVarSetInt(_MDOutYearCountID, itemID, n);
        MFVarSetFloat(_MDOutMeanLogQMaxID, itemID, mu);
        MFVarSetFloat(_MDOutLogQMaxM2ID, itemID, M2);
        MFVarSetFloat(_MDOutLogQMaxM3ID, itemID, M3);

        // call this just to make bankfull calcs and save vals
        dummy = MFVarGetFloat(_MDInBankfullQ5ID, itemID, 0.0);
    } 
	
    Qbar = MFVarGetFloat (_MDInDischMeanID   , itemID, 0.0);	// in m3/s
	Tday = MFVarGetFloat (_MDInAirTempID     , itemID, 0.0);	// in C	
//	A    = MFVarGetFloat (_MDInContributingAreaID, 	itemID, 0.0);	//in km2
	A = MFVarGetFloat (_MDInContributingAreaAccID, itemID, 0.0) + (MFModelGetArea (itemID)/(pow(1000,2)));// convert from m2 to km2  //calculating the contributing area
	MFVarSetFloat (_MDInContributingAreaAccID, itemID, A);
	PixSize_km2 =(MFModelGetArea(itemID)/pow(1000,2));

MFVarSetInt (_MDOutBQART_AID, itemID, A);
//MFVarSetInt (_MDOutBQART_RID, itemID, R);
//Accumulate temperature
	TupSlop = MFVarGetFloat (_MDInAirTempAcc_spaceID, itemID, 0.0); 
	T_old = MFVarGetFloat (_MDInAirTempAcc_timeID, itemID, 0.0) ; 
	T_time = T_old+ Tday;
	Tacc = TupSlop + (T_time * PixSize_km2);
	MFVarSetFloat (_MDInAirTempAcc_spaceID, itemID, Tacc);
	MFVarSetFloat (_MDInAirTempAcc_timeID, itemID, T_time);
//Accumulate discharge
	Qacc = (MFVarGetFloat (_MDInDischargeAccID, itemID, 0.0)+ Qday);// in m3/s
	MFVarSetFloat (_MDInDischargeAccID, itemID, Qacc);

// Accumulate time steps
	TimeStep = (MFVarGetInt (_MDInTimeStepsID, itemID, 0.0)+1);	
	MFVarSetInt (_MDInTimeStepsID, itemID, TimeStep);

//Calculate moving avarege temperature (Tbar) and discharge 
	Tbar = Tacc/TimeStep/A;
	Qbar_m3s = Qacc/TimeStep; //in m3/s
	MFVarSetFloat (_MDOutBQART_Qbar_m3sID, itemID, Qbar_m3s);
	if (Qbar_m3s > 0){ //convert to km3/y	
		Qbar_km3y = (Qbar_m3s/(pow(1000,3)))*(365*24*60*60); 
 	}else
		Qbar_km3y = 0;
	
// exporting outputs flux to files
	MFVarSetFloat (_MDOutBQART_TID, itemID, Tbar);
	MFVarSetFloat (_MDOutBQART_Qbar_km3yID, itemID, Qbar_km3y);
}

enum { MDinput, MDcalculate, MDcorrected };

int MDSediment_BQARTpreprocessDef () {
	
	MFDefEntering ("QBARTpreprocess");
	
	if (((_MDInDischargeID  = MDSediment_DischargeBFDef ()) == CMfailed) || 
	    ((_MDInDischMeanID  = MDAux_MeanDiscargehDef ())    == CMfailed) ||
	    ((_MDInBankfullQ5ID = MDRouting_BankfullQcalcDef()) == CMfailed) ||
        ((_MDInAirTempID             = MFVarGetID (MDVarCommon_AirTemperature,           "degC",    MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInContributingAreaAccID = MFVarGetID (MDVarSediment_ContributingAreaAcc,    "km2",     MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInAirTempAcc_timeID     = MFVarGetID (MDVarSediment_AirTemperatureAcc_time, "degC",    MFOutput, MFState, MFInitial))  == CMfailed) ||
	    ((_MDInAirTempAcc_spaceID    = MFVarGetID (MDVarSediment_AirTemperatureAcc_space, "degC",   MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInDischargeAccID        = MFVarGetID (MDVarSediment_DischargeAcc,            "m3/s",   MFOutput, MFState, MFInitial))  == CMfailed) ||
	    ((_MDInTimeStepsID           = MFVarGetID (MDVarSediment_TimeSteps,               MFNoUnit, MFOutput, MFState, MFInitial))  == CMfailed) ||
        ((_MDOutBQART_AID            = MFVarGetID (MDVarSediment_BQART_A,                 "km2",    MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutBQART_RID            = MFVarGetID (MDVarSediment_BQART_R,                 "km" ,    MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutBQART_Qbar_m3sID     = MFVarGetID (MDVarSediment_BQART_Qbar_m3s,          "m3s",    MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutBQART_Qbar_km3yID    = MFVarGetID (MDVarSediment_BQART_Qbar_km3y,         "km3/y",  MFOutput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutBQART_TID            = MFVarGetID (MDVarSediment_BQART_T,                 "degC",   MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutDischMaxID           = MFVarGetID (MDVarAux_DischMax,                     "m3/s",   MFOutput, MFState, MFInitial))  == CMfailed) ||
        ((_MDOutYearCountID          = MFVarGetID (MDVarAux_YearCount,                    "yr",     MFOutput, MFState, MFInitial))  == CMfailed) ||
        ((_MDOutLogQMaxM2ID          = MFVarGetID (MDVarRouting_LogQMaxM2,                "m3/s",   MFOutput, MFState, MFInitial))  == CMfailed) ||
        ((_MDOutLogQMaxM3ID          = MFVarGetID (MDVarRouting_LogQMaxM3,                "m3/s",   MFOutput, MFState, MFInitial))  == CMfailed) ||
        ((_MDOutMeanLogQMaxID        = MFVarGetID (MDVarRouting_MeanLogQMax ,             "m3/s",   MFOutput, MFState, MFInitial))  == CMfailed) ||
       (MFModelAddFunction (_MDQBARTpreprocess) == CMfailed)) return (CMfailed);

	MFDefLeaving  ("QBARTpreprocess");
	return (_MDOutBQART_TID);
}
