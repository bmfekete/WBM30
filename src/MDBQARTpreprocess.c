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
#include <math.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInDischargeID   	   = MFUnset;
static int _MDInDischargeAccID	   = MFUnset;
static int _MDInDischMeanID	   = MFUnset;
static int _MDInAirTempID   	   = MFUnset;
static int _MDInAirTempAcc_spaceID = MFUnset;
static int _MDInAirTempAcc_timeID = MFUnset;
static int _MDInTimeStepsID 	   = MFUnset;
static int _MDInContributingAreaAccID = MFUnset;

// Output
static int _MDOutBQART_Qbar_m3sID 	= MFUnset;
static int _MDOutBQART_Qbar_km3yID = MFUnset;
static int _MDOutBQART_TID 	= MFUnset;
static int _MDOutBQART_AID 	= MFUnset;
static int _MDOutBQART_RID 	= MFUnset;

static void _MDQBARTpreprocess (int itemID) {
	int TimeStep,p,i,d,j;
	float Qday, Qbar,Qacc, Qbar_km3y,Qbar_m3s;
	float Tday,Tbar,Tacc,A,T_time,T_old;
	float TupSlop,PixSize_km2;
	static int Max_itemID = 0;
	//static int year_count=1;
	static float *PixelmaxQ;
	static float **dailyQ; 
	
	if (itemID > Max_itemID) {
		Max_itemID=itemID+1;
		printf("Max_itemID:%d\n",Max_itemID);
		PixelmaxQ = (float*) malloc(Max_itemID*sizeof(float));
		dailyQ = (float**) malloc(Max_itemID*sizeof(float*));
		for (i = 1; i < Max_itemID+1; i++)
   			dailyQ[i] = (float*) malloc(366*sizeof(float));
   		for (i = 1; i < 366; i++)
			for (j = 1; j < Max_itemID; j++)
				dailyQ[j][i] = 0.0;	
	}	

	//printf("sizeof(Max_itemID):%d\n",(sizeof(PixelmaxQ)/sizeof(float)));
	static int pix=1;
	static int day=1;
	FILE * textfile;
	//printf ("itemID:%d\n ", itemID);
	//printf("Max_itemID:%d\n",Max_itemID);
	Qday = MFVarGetFloat (_MDInDischargeID , itemID, 0.0);	// in m3/s	
	//printf ("pix:%d\n ", pix);
	dailyQ[pix][day]=Qday;
	//if (pix==1) printf ("pix=1. Qday=:%f\n ", Qday);
   	pix++;

   	if (itemID==1){ //last pixelID
   		//printf ("day:%d\n ", day);
   		if (day==365){
   			printf("year count\n ");
   			//year_count++;
   			textfile = fopen("year_max_logQ.txt","a");
   			printf ("Writing to Scripts/year_max_logQ.txt\n");
   			for (p=1;p<Max_itemID;p++){
   				PixelmaxQ[p] = dailyQ[p][1];
   				//printf ("PixelmaxQ[p]:%f\n ", PixelmaxQ[p]);
   				for (d=2; d<366; d++){
   					if (PixelmaxQ[p] < dailyQ[p][d]) PixelmaxQ[p] = dailyQ[p][d];
   				}
   				if (PixelmaxQ[p]>0) PixelmaxQ[p]=log10(PixelmaxQ[p]);
   				fprintf (textfile,"%f ", PixelmaxQ[p]);
   			}
			fprintf (textfile,"\n");
			fclose (textfile);
			Max_itemID = 0;
			day = 0;
		}
		day++;
   		pix=1;
   	}	
   	
//Geting the values of these parameters
	
	Qbar = MFVarGetFloat (_MDInDischMeanID   , itemID, 0.0);	// in m3/s
	Tday = MFVarGetFloat (_MDInAirTempID     , itemID, 0.0);	// in C	
//	A    = MFVarGetFloat (_MDInContributingAreaID, 	itemID, 0.0);	//in km2
	A = MFVarGetFloat (_MDInContributingAreaAccID, itemID, 0.0) + (MFModelGetArea (itemID)/(pow(1000,2)));// convert from m2 to km2  //calculating the contributing area
	MFVarSetFloat (_MDInContributingAreaAccID, itemID, A);
	PixSize_km2 =(MFModelGetArea(itemID)/pow(1000,2));
//	printf("PixSize_km2: %f\n",PixSize_km2);
//	printf("A: %f\n",A);
//Calculate Relief
//	LocalElev = MFVarGetFloat (_MDInElevationID  , itemID, 0.0)/1000;//convert to km
//	printf("LocalElev: %f\n",LocalElev);
//	MaxElev = MFVarGetFloat (_MDOutElevationMaxID, itemID, 0.0);
//	if (A == PixSize_km2) MaxElev=LocalElev;
//	MFVarSetFloat (_MDOutElevationMaxID, itemID, -MaxElev);
//	MFVarSetFloat (_MDOutElevationMaxID, itemID, LocalElev);
//	printf("MaxElev: %f\n",MaxElev);
//	R = MaxElev-LocalElev;
//	printf("R: %f\n",R);
	
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

int MDBQARTpreprocessDef() {
	MFDefEntering ("QBARTpreprocess");
	
	if (((_MDInDischMeanID           = MDDischMeanDef ()) == CMfailed) ||
	    ((_MDInDischargeID           = MDDischargeDef ()) == CMfailed) || 
	    ((_MDInAirTempID             = MFVarGetID (MDVarAirTemperature,         "degC",  MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInContributingAreaAccID = MFVarGetID (MDVarContributingAreaAcc,    "km2",   MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInAirTempAcc_timeID     = MFVarGetID (MDVarAirTemperatureAcc_time, "degC",  MFOutput, MFState, MFInitial))  == CMfailed) ||
	    ((_MDInAirTempAcc_spaceID    = MFVarGetID (MDVarAirTemperatureAcc_space,"degC",  MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInDischargeAccID        = MFVarGetID (MDVarDischargeAcc,           "m3/s",  MFOutput, MFState, MFInitial))  == CMfailed) ||
	    ((_MDInTimeStepsID           = MFVarGetID (MDVarTimeSteps,              MFNoUnit,MFOutput, MFState, MFInitial))  == CMfailed) ||

        // output
	    ((_MDOutBQART_AID  	         = MFVarGetID (MDVarBQART_A,                "km2",   MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutBQART_RID            = MFVarGetID (MDVarBQART_R,                "km" ,   MFRoute,  MFState, MFBoundary)) == CMfailed) ||
            ((_MDOutBQART_Qbar_m3sID     = MFVarGetID (MDVarBQART_Qbar_m3s,         "m3s",   MFOutput, MFState, MFBoundary)) == CMfailed) ||
            ((_MDOutBQART_Qbar_km3yID    = MFVarGetID (MDVarBQART_Qbar_km3y,        "km3/y", MFOutput, MFState, MFBoundary)) == CMfailed) ||
  	    ((_MDOutBQART_TID            = MFVarGetID (MDVarBQART_T, 	            "degC",  MFOutput, MFState, MFBoundary)) == CMfailed) || 
	    (MFModelAddFunction (_MDQBARTpreprocess) == CMfailed)) return (CMfailed);
	MFDefLeaving  ("QBARTpreprocess");
	return (_MDOutBQART_TID);
}
