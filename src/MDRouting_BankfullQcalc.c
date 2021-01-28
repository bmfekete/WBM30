/******************************************************************************
Phase 1.5 of the WBMsed model.
Calculate bankfull discharge based on 2-yaers and 5-rears maximum discharge reacurance (Q2 and Q5).
It reads the log yearly-maximum discharge  per pixel from a text file (Scripts/year_max_logQ.txt)
generated at the MDBQARTpreprocess module. It then calculate the sum, mean, standard deviation and
skew of its distribution. The Q2 and Q5 are calculated based on the logaritmic Pearson III distribution.
MDQBankfullQcalc.c
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
static int _MDInLogQMaxM2ID         = MFUnset;
static int _MDInLogQMaxM3ID         = MFUnset;
static int _MDInYearCountID         = MFUnset;

// Output
static int _MDOutBankfullQ2ID   	= MFUnset;
static int _MDOutBankfullQ5ID   	= MFUnset;
static int _MDOutBankfullQ10ID   	= MFUnset;
static int _MDOutBankfullQ25ID   	= MFUnset;
static int _MDOutBankfullQ50ID   	= MFUnset;
static int _MDOutBankfullQ100ID   	= MFUnset;
static int _MDOutBankfullQ200ID   	= MFUnset;



static void _MDBankfullQcalc (int itemID) {
	/*static int p=1;*/
	/*static int day=1;*/
	float BankfullQ2,BankfullQ5,BankfullQ10,BankfullQ25,BankfullQ50,BankfullQ100,BankfullQ200;
	float Mean,StdDev,Skew;
	float K2,K5,K10,K25,K50,K100,K200;
	float LogQ2,LogQ5,LogQ10,LogQ25,LogQ50,LogQ100,LogQ200;
	FILE *fpt;

    float M2, M3, N, Var;
        
    if (MFDateGetDayOfYear() == MFDateGetYearLength ()) {
        M2 = MFVarGetFloat(_MDInLogQMaxM2ID, itemID, 0.0);
        M3 = MFVarGetFloat(_MDInLogQMaxM3ID, itemID, 0.0);
        N = MFVarGetInt(_MDInYearCountID, itemID, 0);

        Var = M2/N;
        StdDev = pow(Var, 0.5);
        Skew = (M3/N) / pow(StdDev,3);

        K2   = 0.0041*pow(Skew,3) - 7e-16*pow(Skew,2) - 0.1692*Skew + 2e-14;
		K5   = 0.0004*pow(Skew,4) + 0.0014*pow(Skew,3) - 0.0391*pow(Skew,2) - 0.0477*Skew + 0.8425;
		K10  = 0.0012*pow(Skew,4) - 0.0025*pow(Skew,3) - 0.0513*pow(Skew,2) + 0.1115*Skew + 1.2832;
		K25  = 0.0019*pow(Skew,4) - 0.0089*pow(Skew,3) - 0.0477*pow(Skew,2) + 0.3495*Skew + 1.7501;
		K50  = 0.0022*pow(Skew,4) - 0.0141*pow(Skew,3) - 0.0352*pow(Skew,2) + 0.5391*Skew + 2.0512;
		K100 = 0.0023*pow(Skew,4) - 0.0192*pow(Skew,3) - 0.0151*pow(Skew,2) + 0.7322*Skew + 2.3212;
		K200 = 0.0020*pow(Skew,4) - 0.0243*pow(Skew,3) + 0.0115*pow(Skew,2) + 0.9272*Skew + 2.5679;
		
		LogQ2 = Mean + K2*StdDev;
		LogQ5 = Mean + K5*StdDev;
		LogQ10 = Mean + K10*StdDev;
		LogQ25 = Mean + K25*StdDev;
		LogQ50 = Mean + K50*StdDev;
		LogQ100 = Mean + K100*StdDev;
		LogQ200 = Mean + K200*StdDev;
	
		BankfullQ2 = pow(10,LogQ2);
		BankfullQ5 = pow(10,LogQ5);
		BankfullQ10 = pow(10,LogQ10);
		BankfullQ25 = pow(10,LogQ25);
		BankfullQ50 = pow(10,LogQ50);
		BankfullQ100 = pow(10,LogQ100);
		BankfullQ200 = pow(10,LogQ200);

		MFVarSetFloat (_MDOutBankfullQ2ID, itemID, BankfullQ2);
		MFVarSetFloat (_MDOutBankfullQ5ID, itemID, BankfullQ5);
		MFVarSetFloat (_MDOutBankfullQ10ID, itemID, BankfullQ10);
		MFVarSetFloat (_MDOutBankfullQ25ID, itemID, BankfullQ25);
		MFVarSetFloat (_MDOutBankfullQ50ID, itemID, BankfullQ50);
		MFVarSetFloat (_MDOutBankfullQ100ID, itemID, BankfullQ100);
		MFVarSetFloat (_MDOutBankfullQ200ID, itemID, BankfullQ200);
	}
}

enum { MDinput, MDcalculate, MDcorrected };

int MDRouting_BankfullQcalcDef () {

	MFDefEntering ("BankfullQcalc");
	// inputs set as initial but don't update, always use last values to calc var, skew of distribution
	if (((_MDInLogQMaxM2ID          = MFVarGetID (MDVarRouting_LogQMaxM2,       "",       MFInput,   MFState, MFInitial))  == CMfailed) ||
        ((_MDInLogQMaxM3ID          = MFVarGetID (MDVarRouting_LogQMaxM3,       "",       MFInput,   MFState, MFInitial))  == CMfailed) ||
	    ((_MDOutBankfullQ2ID  		= MFVarGetID (MDVarRouting_BankfullQ2, 		"m2s", 	  MFOutput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutBankfullQ5ID  		= MFVarGetID (MDVarRouting_BankfullQ5, 		"m2s", 	  MFOutput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutBankfullQ10ID  		= MFVarGetID (MDVarRouting_BankfullQ10, 	"m2s", 	  MFOutput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutBankfullQ25ID  		= MFVarGetID (MDVarRouting_BankfullQ25, 	"m2s", 	  MFOutput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutBankfullQ50ID  		= MFVarGetID (MDVarRouting_BankfullQ50, 	"m2s", 	  MFOutput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutBankfullQ100ID  	= MFVarGetID (MDVarRouting_BankfullQ100, 	"m2s", 	  MFOutput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutBankfullQ200ID  	= MFVarGetID (MDVarRouting_BankfullQ200, 	"m2s", 	  MFOutput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInYearCountID          = MFVarGetID (MDVarAux_YearCount,           "yr",     MFInput,   MFState, MFInitial))  == CMfailed) ||
        (MFModelAddFunction (_MDBankfullQcalc) == CMfailed)) return (CMfailed);
	MFDefLeaving  ("BankfullQcalc");
	return (_MDOutBankfullQ5ID);
}
