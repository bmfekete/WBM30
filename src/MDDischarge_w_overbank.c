/******************************************************************************
WBMsed V2.0
MDDischarge_w_overbank.c
Add an overbank water loss and storage.

sagy.cohen@colorado.edu
*******************************************************************************/

#include <string.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInDischLevel1ID   = MFUnset;
static int _MDInDischObservedID = MFUnset;
static int _MDInBankfullQID 	= MFUnset;
static int _MDInBankfullQ5ID 	= MFUnset;
static int _MDInBankfullQ10ID 	= MFUnset;
static int _MDInBankfullQ25ID 	= MFUnset;
static int _MDInBankfullQ50ID 	= MFUnset;
static int _MDInBankfullQ100ID 	= MFUnset;
static int _MDInBankfullQ200ID 	= MFUnset;
static int _MDInBankfull_QnID  	= MFUnset;
static int _MDInFlowCoefficientID= MFUnset;

// Output
static int _MDOutDischargeID     = MFUnset;
static int _MDOutOverBankQID     = MFUnset;

static void _MDDischargeBF (int itemID) {
	float discharge,Qbf,FlowCoeff,QfromFP; 
	float ExcessQ = 0;
	float Qfp = 0;
	int Q_n;
	
	FlowCoeff = MFVarGetFloat (_MDInFlowCoefficientID, itemID, 0.0);; // coefficient for water flow from floodplain to river 
	
	discharge = MFVarGetFloat (_MDInDischLevel1ID,   itemID, 0.0);

	Q_n = MFVarGetFloat (_MDInBankfull_QnID, itemID, 0.0);
	//printf("Q_n:%d ",Q_n);
	/*switch(Q_n){
		case 2: Qbf  = MFVarGetFloat (_MDInBankfullQ2ID, itemID, 0.0); 
		case 10: Qbf  = MFVarGetFloat (_MDInBankfullQ10ID, itemID, 0.0); 
		case 25: Qbf  = MFVarGetFloat (_MDInBankfullQ25ID, itemID, 0.0); 
		case 50: Qbf  = MFVarGetFloat (_MDInBankfullQ50ID, itemID, 0.0); 	
		case 100: Qbf  = MFVarGetFloat (_MDInBankfullQ100ID, itemID, 0.0); 	
		case 200: Qbf  = MFVarGetFloat (_MDInBankfullQ200ID, itemID, 0.0); 
	}*/

	if (Q_n == 0) Qbf  = MFVarGetFloat (_MDInBankfullQID, itemID, 0.0); 
	else if (Q_n == 5) Qbf  = MFVarGetFloat (_MDInBankfullQ10ID, itemID, 0.0);
	else if (Q_n == 10) Qbf  = MFVarGetFloat (_MDInBankfullQ10ID, itemID, 0.0); 
	else if (Q_n == 25) Qbf  = MFVarGetFloat (_MDInBankfullQ25ID, itemID, 0.0); 
	else if (Q_n == 50) Qbf  = MFVarGetFloat (_MDInBankfullQ50ID, itemID, 0.0); 
	else if (Q_n == 100) Qbf  = MFVarGetFloat (_MDInBankfullQ100ID, itemID, 0.0); 
	else if (Q_n == 200) Qbf  = MFVarGetFloat (_MDInBankfullQ200ID, itemID, 0.0); 

	Qbf = Qbf * 0.5; //BANKFULL DISCHARGE ADJUSTMENT !!!!!!!!
	//printf("Qbf:%f ",Qbf);
	Qfp = MFVarGetFloat (_MDOutOverBankQID,   itemID, 0.0); //excess water stored in floodplain the day before
	
	if (discharge > Qbf){
		ExcessQ = Qfp + (discharge - Qbf);
		discharge = Qbf;
	}	
	else {
		QfromFP = (FlowCoeff * (Qbf - discharge));	
		if (QfromFP > Qfp) QfromFP = Qfp;	
		ExcessQ = Qfp - QfromFP;
		if (ExcessQ < 0) ExcessQ =0;
		discharge = discharge + QfromFP;
		if (discharge > Qbf) discharge = Qbf;	
	}

	if (_MDInDischObservedID != MFUnset)
		 discharge = MFVarGetFloat (_MDInDischObservedID, itemID, discharge);
	
	MFVarSetFloat (_MDOutOverBankQID, itemID, ExcessQ);
	MFVarSetFloat (_MDOutDischargeID, itemID, discharge);
}

enum { MDinput, MDcalculate, MDcorrected };

int MDDischargeBFDef() {
	int optID = MFUnset;
	const char *optStr, *optName = MDOptDischarge;
	const char *options [] = { MDInputStr, MDCalculateStr, "corrected", (char *) NULL };

	if (_MDOutDischargeID != MFUnset) return (_MDOutDischargeID);

	MFDefEntering ("DischargeBF");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);
	switch (optID) {
		case MDinput: _MDOutDischargeID = MFVarGetID (MDVarDischarge,     "m3/s",  MFInput,  MFState, MFBoundary); break;
		case MDcorrected:
			if ((_MDInDischObservedID   = MFVarGetID (MDVarDischObserved, "m3/s",  MFInput,  MFState, MFBoundary)) == CMfailed)
				return (CMfailed);
		case MDcalculate:
			if (((_MDOutDischargeID     = MFVarGetID (MDVarDischarge,     "m3/s", MFRoute,  MFState, MFBoundary)) == CMfailed) ||
				((_MDInDischLevel1ID    = MDDischLevel1Def ()) == CMfailed) ||
				((_MDInBankfullQID     = MFVarGetID (MDVarBankfullQ,    "m3/s", MFInput,  MFState, MFBoundary)) == CMfailed) ||
				((_MDInBankfullQ5ID     = MFVarGetID (MDVarBankfullQ5,    "m3/s", MFInput,  MFState, MFBoundary)) == CMfailed) ||
				((_MDInBankfullQ10ID    = MFVarGetID (MDVarBankfullQ10,   "m3/s", MFInput,  MFState, MFBoundary)) == CMfailed) ||
				((_MDInBankfullQ10ID    = MFVarGetID (MDVarBankfullQ25,   "m3/s", MFInput,  MFState, MFBoundary)) == CMfailed) ||
				((_MDInBankfullQ50ID    = MFVarGetID (MDVarBankfullQ50,   "m3/s", MFInput,  MFState, MFBoundary)) == CMfailed) ||
				((_MDInBankfullQ100ID   = MFVarGetID (MDVarBankfullQ100,  "m3/s", MFInput,  MFState, MFBoundary)) == CMfailed) ||
				((_MDInBankfullQ200ID   = MFVarGetID (MDVarBankfullQ200,  "m3/s", MFInput,  MFState, MFBoundary)) == CMfailed) ||
				((_MDInBankfull_QnID  	= MFVarGetID (MDVarBankfull_Qn,	  "m3/s", MFInput,  MFState, MFBoundary)) == CMfailed) ||
				((_MDInFlowCoefficientID= MFVarGetID (MDVarFlowCoefficient,"m3/s", MFInput,  MFState, MFBoundary)) == CMfailed) ||
				((_MDOutOverBankQID     = MFVarGetID (MDVarOverBankQ,     "m3/s", MFOutput, MFState, MFInitial)) == CMfailed) ||
				(MFModelAddFunction (_MDDischargeBF) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving  ("DischargeBF");
	return (_MDOutDischargeID);
}
