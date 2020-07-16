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

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <MF.h>
#include <MD.h>

// Input
//static int _MDInDischargeID   	   = MFUnset;
// Output
static int _MDOutBankfullQ2ID   	= MFUnset;
static int _MDOutBankfullQ5ID   	= MFUnset;
static int _MDOutBankfullQ10ID   	= MFUnset;
static int _MDOutBankfullQ25ID   	= MFUnset;
static int _MDOutBankfullQ50ID   	= MFUnset;
static int _MDOutBankfullQ100ID   	= MFUnset;
static int _MDOutBankfullQ200ID   	= MFUnset;

static void _MDBankfullQcalc (int itemID) {
	static int p=1;
	static int day=1;
	float BankfullQ2,BankfullQ5,BankfullQ10,BankfullQ25,BankfullQ50,BankfullQ100,BankfullQ200;
	float Mean,Sum,StdDev,sum_X_Xbar_2,sum_X_Xbar_3,Skew;
	float K2,K5,K10,K25,K50,K100,K200;
	float LogQ2,LogQ5,LogQ10,LogQ25,LogQ50,LogQ100,LogQ200;
	FILE *fpt;
	int i, j,y,col;
	char c;
	static int NumYears = 0;
	static int Max_itemID = 0;
	static float **yearMaxLogQ; //[years][pixels]
	
	if (itemID > Max_itemID) {
//count number of lines in the test file
		fpt = fopen("year_max_logQ.txt", "r");
		while((c = fgetc(fpt)) != EOF)
		if(c == '\n')
		NumYears++;
		printf("NumYears:%d\n",NumYears);
		fclose(fpt);
		if(c != '\n')
		NumYears++;
		
		Max_itemID=itemID+1;
		printf("Max_itemID:%d\n",Max_itemID);
		yearMaxLogQ = (float**) malloc(NumYears*sizeof(float*));
		for (i = 1; i < NumYears; i++){
   			yearMaxLogQ[i] = (float*) malloc(Max_itemID*sizeof(float));} //[1->NumYears][0->]
 		printf("NumYears:%d\n",NumYears);
	//	printf("Max_itemID,itemID: %d, %d\n",Max_itemID,itemID);
	//	yearMaxLogQ[31][Max_itemID+10] = 0.0;
 	}	

	if (day==1){ 
		//printf ("day:%d\n",day);
		if (itemID == Max_itemID-1){
// Seting up the yearly max log(Q) matrix 
			//printf("before zeroing loop\n");	
			for (i = 1; i < NumYears ; i++){
				//printf("in zeroing loop, i=%d\n",i);
				for (j = 1; j < Max_itemID; j++)
					yearMaxLogQ[i][j] = 0.0;
			}
			printf("after zeroing loop\n");			
			fpt = fopen("year_max_logQ.txt","r");
			if(fpt == NULL)
			{
			printf("\nError: File couldn't be opened (1).\n");
			exit(-1);
			}
//geting the values from the text file (generated at the BQARTpreprocess run) 
			//printf("before fscanf loop\n");	
			for(j = 1; j < NumYears ; j++){
				//printf("in fscanf loop, j=%d\n",j);
				for(i = 1; i < Max_itemID ; i++){
					fscanf(fpt, "%f",&yearMaxLogQ[j][i]);
					
				}	
			}
			fclose(fpt);
//Printing out values to check
			//for(i = 1; i < 4; i++) {
				for(j = 1; j < NumYears; j++){
					printf("%f\n ",yearMaxLogQ[j][1]);
					//printf("%f\n ",yearMaxLogQ[j][1]);
				}
		}
	 	Sum=0;
 	//printf("before Sum, NumYears=%d\n",NumYears);
		for (y=1;y<NumYears;y++){
			Sum = Sum + yearMaxLogQ[y][p];
			//printf ("yearMaxLogQ[p][y]: %f\n",yearMaxLogQ[y][p]);
			//printf ("in Sum, y=%d\n",y);
		}
	//printf("after Sum, y:%d\n",y);
		Mean=Sum/(y-1);
		sum_X_Xbar_2 =0;
		sum_X_Xbar_3 =0;
		for (y=1;y<NumYears;y++){
			sum_X_Xbar_2 = sum_X_Xbar_2 + (pow((yearMaxLogQ[y][p] - Mean),2)); 
			sum_X_Xbar_3 = sum_X_Xbar_3 + (pow((yearMaxLogQ[y][p] - Mean),3)); 
		}
	
		StdDev = pow((sum_X_Xbar_2/((y-1)-1)),0.5);
	
		Skew = ((y-1)*sum_X_Xbar_3)/(((y-1)-1)*((y-1)-2)*(pow(StdDev,3)));
	
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
		p++;
		
	/*	printf ("Sum:%f\n",Sum);
		printf ("Mean:%f\n",Mean);
		printf ("sum_X_Xbar_2:%f\n",sum_X_Xbar_2);
		printf ("sum_X_Xbar_3:%f\n",sum_X_Xbar_3);
		printf ("StdDev:%f\n",StdDev);
		printf ("Skew:%f\n",Skew);
		printf ("K2:%f\n",K2);
		printf ("LogQ2:%f\n",LogQ2);
		printf ("BankfullQ2:%f\n",BankfullQ2);
		abort();*/
		
	}	
	if (itemID==1){
		day++;
		//printf ("day:%d\n",day);
	}	
}

enum { MDinput, MDcalculate, MDcorrected };

int MDBankfullQcalcDef() {

	MFDefEntering ("BankfullQcalc");
	if ( 
       // output
    ((_MDOutBankfullQ2ID  		= MFVarGetID (MDVarBankfullQ2, 		"m2s", 	  MFOutput,  MFState, MFBoundary)) == CMfailed) ||
    ((_MDOutBankfullQ5ID  		= MFVarGetID (MDVarBankfullQ5, 		"m2s", 	  MFOutput,  MFState, MFBoundary)) == CMfailed) ||
    ((_MDOutBankfullQ10ID  		= MFVarGetID (MDVarBankfullQ10, 	"m2s", 	  MFOutput,  MFState, MFBoundary)) == CMfailed) ||
    ((_MDOutBankfullQ25ID  		= MFVarGetID (MDVarBankfullQ25, 	"m2s", 	  MFOutput,  MFState, MFBoundary)) == CMfailed) ||
    ((_MDOutBankfullQ50ID  		= MFVarGetID (MDVarBankfullQ50, 	"m2s", 	  MFOutput,  MFState, MFBoundary)) == CMfailed) ||
    ((_MDOutBankfullQ100ID  	= MFVarGetID (MDVarBankfullQ100, 	"m2s", 	  MFOutput,  MFState, MFBoundary)) == CMfailed) ||
    ((_MDOutBankfullQ200ID  	= MFVarGetID (MDVarBankfullQ200, 	"m2s", 	  MFOutput,  MFState, MFBoundary)) == CMfailed) ||
        
       (MFModelAddFunction (_MDBankfullQcalc) == CMfailed)) return (CMfailed);
	MFDefLeaving  ("BankfullQcalc");
	return (_MDOutBankfullQ5ID);
}
