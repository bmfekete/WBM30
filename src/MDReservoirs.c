/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2014, UNH - CCNY/CUNY

MDReservoirs.c

dominik.wisser@unh.edu

Updated with a residence time dependent function 
Feb. 2009 DW

*******************************************************************************/


#include <cm.h>
#include <MF.h>
#include <MD.h>
#include <math.h>
 
// Input
static int _MDInDischargeID       = MFUnset;
static int _MDInDischMeanID       = MFUnset;
static int _MDInDischMinID       = MFUnset;
static int _MDInDischMaxID       = MFUnset;
static int _MDInResCapacityID     = MFUnset;


// Output
static int _MDOutResStorageID        = MFUnset;
static int _MDOutResStorageChgID     = MFUnset;
static int _MDOutResReleaseID        = MFUnset;
static int _MDOutResRelease_t_1_ID  = MFUnset;
static int _MDOutDisch_t_1_ID      = MFUnset;
static int _MDOutDisch_t_2_ID      = MFUnset;


static float ANNOUTPUT (float I1[3][1], float I2[2][1], float I3) {
    /*
        I1, I2 & I3 are Example Inputs In Case You Want to Run This Part By It Self and Ceck The Result
        Result Should be: 0.679450444505195
    */
    /*
    float I1[3][1]={
        {0.268400000000000},
        {0.273470000000000},
        {0.869150000000000},
        };
    float I2[2][1]={
        {0.319930000000000},
        {0.229270000000000}
        };
    float I3=0.300000000000000;
    */
    float FirstLayerBias[6][1] = {
        {-1.8349638599010885},
        {0.31681519142609904},
        {0.90820782843797254},
        {0.24315763055718279},
        {0.22750233763000977},
        {-5.9638615037974105},
    };

    float FirstLayerWeight [6][3] = {
        {0.334525270188955,2.32729460498139,-1.08218040335814},
        {-0.132942683641029,-2.07435066955086,-7.71723103154886},
        {-0.961228914800186,-1.59451723505862,7.64275651580055},
        {-0.138149619471609,-0.369268995299719,-1.05878021476956},
        {-0.830754365521219,-1.14297810944283,-0.591263860604682},
        {-2.35234764862494,-0.170592638429000,6.45623554696750},
        };

    float SecondLayerWeight[4][2]= {
        {-0.461376663064363,-2.29680500565283},
        {1.14189579706809,3.60122216280990},
        {5.13965726884100,-8.54024357801218},
        {0.797602695519510,9.04132111622455},
        };

    float SecondLayerBias [4][1]={

        {1.7667185407719592},

        {-0.44451918724448725},

        {0.27782065349273022},

        {0.95132682701557358},

        };

    float ThirdLayerWeight[2][1] ={

        {-3.80846357210734},

        {6.09704986254896},

        };

    float ThirdLayerBias [2][1]= {

        {1.9804896408537742},

        {-1.4736095994793379},

        };

    float FourthLayerWeight1[6][6] = {

        {-0.682425987793126,2.20351531749950,1.39097166906628,-2.39124203637190,2.27986610311803,-0.633039777101503},

        {-0.478344057060439,2.22542158997640,1.32425417280671,-2.91086448864336,2.55366825518159,0.915264601641264},

        {-0.330288477684471,0.636650781140816,-0.267485765559433,-0.0845957816699171,-0.0311750961112513,0.519857878801029},

        {0.871647922418761,-0.0363653064870689,-1.23489938556299,3.48735527686833,-4.09204564273278,-1.29124333772506},

        {-1.14578627733860,0.362029723412182,-1.49653052173892,-2.14316546336101,2.53719571843746,1.03545166271711},

        {-3.93424835569323,-0.140516448398705,-0.287183959118834,2.81056586729428,-1.13796052928068,-1.61032679050170},

        };

    float FourthLayerWeight2[6][4] = {

        {1.73940117579596,4.31257163704923,2.00908995797761,0.542330032098119},

        {1.82716477758809,3.90502202733331,1.71260564791546,0.673923447124240},

        {-0.297770956962901,0.353599592055875,-0.0335628624334481,1.90359407919045},

        {0.211761429085206,-1.37612121135069,-0.303662471752540,1.13329378872693},

        {-0.299078423419769,0.649961868699184,0.173313681427546,0.573538752543163},

        {-3.38710042074078,1.16448748228937,-0.0607349889977066,-2.01182147710900},

        };

    float FourthLayerWeight3[6][2] = {

        {-1.84202629698469,-0.877541434013040},

        {-1.96090394140442,-0.946074755836152},

        {0.0173356328650234,-0.00969615110828418},

        {-0.0839568134797032,-0.129727081135696},

        {0.0356547772242401,0.0691169740242024},

        {0.330775400271462,0.0723975371228814},

        };

    float FourthLayerBias[6][1] = {

        {0.0051805882973190165},

        {1.7022493850334097},

        {-1.493864901771131},

        {-0.49352259943031906},

        {0.78319807803209185},

        {-0.86188699136698088},

        };

    float FifthLayerWeight [1][6]= {

        {-4.60687439047288,4.68810658158703,7.48854158670334,-2.67349645890795,-5.84593641782589,-1.13313370598710},

        };

    float FifthLayerBias = 0.22258525600927653;



    /* LAYER ONE */



    /* printf("Layer ONE\n"); */



    float FirstLayerOut [6][1];



    int i;

    float n;

    for (i=0; i<6; i++){

        n=I1[0][0]*FirstLayerWeight[i][0]+I1[1][0]*FirstLayerWeight[i][1]+I1[2][0]*FirstLayerWeight[i][2]+ FirstLayerBias [i][0];

        FirstLayerOut[i][0]= 2/(1+exp(-2*n))-1;



      /*  printf( "%f\t", FirstLayerWeight[i][0] );

        printf("%f\t",FirstLayerBias[i][0]);

        printf("%f\n",FirstLayerOut[i][0]);

      */

    }



    /* LAYER TWO */



    /* printf("Layer TWO\n"); */

    float SecondLayerOut[4][1];

    for (i=0; i<4; i++) {

        n=I2[0][0]*SecondLayerWeight[i][0]+I2[1][0]*SecondLayerWeight[i][1]+ SecondLayerBias [i][0];

        SecondLayerOut[i][0]= 2/(1+exp(-2*n))-1;



       /* printf( "%f\t", SecondLayerWeight[i][0] );

        printf("%f\t",SecondLayerBias[i][0]);

        printf("%f\n",SecondLayerOut[i][0]);

        */

    }



    /* LAYER THREE */



    /* printf("Layer Three\n"); */

    float ThirdLayerOut[2][1];

    for (i=0; i<2; i++){

        n=I3*ThirdLayerWeight[i][0]+ ThirdLayerBias [i][0];

        ThirdLayerOut[i][0]= 2/(1+exp(-2*n))-1;



       /* printf( "%f\t", ThirdLayerWeight[i][0] );

        printf("%f\t",ThirdLayerBias[i][0]);

        printf("%f\n",ThirdLayerOut[i][0]);

        */

    }



    /* LAYER FOUR */



    /* printf("Layer Four\n"); */

    float FourthLayerOut[6][1];

    for (i=0; i<6; i++){

            n= FirstLayerOut[0][0]*FourthLayerWeight1[i][0]+FirstLayerOut[1][0]*FourthLayerWeight1[i][1]+FirstLayerOut[2][0]*FourthLayerWeight1[i][2]+FirstLayerOut[3][0]*FourthLayerWeight1[i][3]+FirstLayerOut[4][0]*FourthLayerWeight1[i][4]+FirstLayerOut[5][0]*FourthLayerWeight1[i][5]

             + SecondLayerOut[0][0]*FourthLayerWeight2[i][0]+SecondLayerOut[1][0]*FourthLayerWeight2[i][1]+SecondLayerOut[2][0]*FourthLayerWeight2[i][2]+SecondLayerOut[3][0]*FourthLayerWeight2[i][3]

             + ThirdLayerOut[0][0]*FourthLayerWeight3[i][0]+ThirdLayerOut[1][0]*FourthLayerWeight3[i][1]

             + FourthLayerBias [i][0];

            FourthLayerOut[i][0]= 2/(1+exp(-2*n))-1;



        /* printf( "%f\t", FourLayerWeight[i][0] );

        printf("%f\t",FourthLayerBias[i][0]);

        printf("%f\n",FourthLayerOut[i][0]);

        */



    }



    /* LAYER FIVE */



    /* printf("Layer Five (OUTPUT)\n"); */

    float FifthLayerOut;

        n=FourthLayerOut[0][0]*FifthLayerWeight[0][0]+FourthLayerOut[1][0]*FifthLayerWeight[0][1]+FourthLayerOut[2][0]*FifthLayerWeight[0][2]+FourthLayerOut[3][0]*FifthLayerWeight[0][3]+FourthLayerOut[4][0]*FifthLayerWeight[0][4]+FourthLayerOut[5][0]*FifthLayerWeight[0][5]

        + FifthLayerBias;

        FifthLayerOut = 1 / (1 + exp(-n));



       /* printf("%f\n",FifthLayerOut);

       */


    float ANNOUTPUT;

    ANNOUTPUT=FifthLayerOut;

    return (ANNOUTPUT);

}




static void _MDReservoirNeuralNet (int itemID) {

// Input
	float discharge;         // Current discharge [m3/s]
	float meanDischarge;     // Long-term mean annual discharge [m3/s]
	float resCapacity;       // Reservoir capacity [km3]
    float discharge_t_1;
    float discharge_t_2;
    float discharge_t_3;
    float discharge_min;
    float discharge_max;

// Output
	float resStorage;     // Reservoir storage [km3]
	float resStorageChg;  // Reservoir storage change [km3/dt]
	float resRelease;     // Reservoir release [m3/s] 
    float res_release_t_1;
    float res_release_t_2;

// local
	float prevResStorage; // Reservoir storage from the previous time step [km3]
	float dt;             // Time step length [s]
	float balance;		  // water balance [m3/s]

// Parameters
	float drySeasonPct = 0.60;	// RJS 071511
	float wetSeasonPct = 0.16;	// RJS 071511
	float year = 0;				// RJS 082311
	
	discharge     = MFVarGetFloat (_MDInDischargeID,    itemID, 0.0);
	meanDischarge = MFVarGetFloat (_MDInDischMeanID,    itemID, discharge);
	year 		  = MFDateGetCurrentYear();

	if ((resCapacity = MFVarGetFloat (_MDInResCapacityID, itemID, 0.0)) <= 0.0) { 
		MFVarSetFloat (_MDOutResStorageID,    itemID, 0.0); 
		MFVarSetFloat (_MDOutResStorageChgID, itemID, 0.0); 
		MFVarSetFloat (_MDOutResReleaseID,    itemID, discharge);
		return;
	}

	discharge_min = MFVarGetFloat(_MDInDischMinID, itemID, 0.0);
	discharge_max = MFVarGetFloat(_MDInDischMaxID, itemID, 0.0);      
	discharge_t_1 = discharge;
	discharge_t_2 = MFVarGetFloat(_MDOutDisch_t_1_ID, itemID, 0.0);
	discharge_t_3 = MFVarGetFloat(_MDOutDisch_t_2_ID, itemID, 0.0);

	res_release_t_1 = MFVarGetFloat(_MDOutResReleaseID, itemID, 0.0);
	res_release_t_2 = MFVarGetFloat(_MDOutResRelease_t_1_ID, itemID, 0.0);

// put code here...

//


	MFVarSetFloat(_MDOutDisch_t_1_ID, itemID, discharge_t_1);
	MFVarSetFloat(_MDOutDisch_t_2_ID, itemID, discharge_t_2);
	MFVarSetFloat(_MDOutResReleaseID, itemID, resRelease);
	MFVarSetFloat(_MDOutResRelease_t_1_ID, itemID, res_release_t_1);
}

static void _MDReservoirDW (int itemID) {

// Input
   float discharge;         // Current discharge [m3/s]
   float meanDischarge;     // Long-term mean annual discharge [m3/s]
   float resCapacity;       // Reservoir capacity [km3]

// Output
   float resStorage;     // Reservoir storage [km3]
   float resStorageChg;  // Reservoir storage change [km3/dt]
   float resRelease;     // Reservoir release [m3/s] 

// local
   float prevResStorage; // Reservoir storage from the previous time step [km3]
   float dt;             // Time step length [s]
   float balance;      // water balance [m3/s]

// Parameters
   float drySeasonPct = 0.60; // RJS 071511
   float wetSeasonPct = 0.16; // RJS 071511
   float year = 0;            // RJS 082311

   discharge     = MFVarGetFloat (_MDInDischargeID,    itemID, 0.0);
   meanDischarge = MFVarGetFloat (_MDInDischMeanID,    itemID, discharge);
   year       = MFDateGetCurrentYear();

   if ((resCapacity = MFVarGetFloat (_MDInResCapacityID, itemID, 0.0)) <= 0.0) {
		MFVarSetFloat (_MDOutResStorageID,    itemID, 0.0);
		MFVarSetFloat (_MDOutResStorageChgID, itemID, 0.0);
		MFVarSetFloat (_MDOutResReleaseID,    itemID, discharge);
		return;
	}
	dt = MFModelGet_dt ();
	prevResStorage = MFVarGetFloat(_MDOutResStorageID, itemID, 0.0);

	resRelease = discharge > meanDischarge ? wetSeasonPct * discharge  : drySeasonPct * discharge + (meanDischarge - discharge);

	resStorage = prevResStorage + (discharge - resRelease) * 86400.0 / 1e9;

	if (resStorage > resCapacity) {
		resRelease = discharge * dt / 1e9 + prevResStorage - resCapacity;
		resRelease = resRelease * 1e9 / dt;
		resStorage = resCapacity;
	}
	else if (resStorage < 0.0) {
		resRelease = prevResStorage + discharge  * dt / 1e9;
		resRelease = resRelease * 1e9 / dt;
		resStorage=0;
	}

	resStorageChg = resStorage - prevResStorage;

	balance = discharge - resRelease - (resStorageChg / 86400 * 1e9); // water balance

	MFVarSetFloat (_MDOutResStorageID,    itemID, resStorage);
	MFVarSetFloat (_MDOutResStorageChgID, itemID, resStorageChg);
}

enum { MDnone, MDcalculate, MDneuralnet };

int MDReservoirDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDOptReservoirs;
	const char *options [] = { MDNoneStr, MDCalculateStr, "neuralnet", (char *) NULL };
 
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
 
	if ((optID == MDnone) || (_MDOutResReleaseID != MFUnset)) return (_MDOutResReleaseID);

	MFDefEntering ("Reservoirs");
	switch (optID) {
		case MDcalculate:
			if (((_MDInDischMeanID      = MDDischMeanDef ())   == CMfailed) ||
			    ((_MDInDischargeID      = MDDischLevel2Def ()) == CMfailed) ||
			    ((_MDInResCapacityID    = MFVarGetID (MDVarReservoirCapacity,      "km3",  MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutResStorageID    = MFVarGetID (MDVarReservoirStorage,       "km3",  MFOutput, MFState, MFInitial))  == CMfailed) ||
			    ((_MDOutResStorageChgID = MFVarGetID (MDVarReservoirStorageChange, "km3",  MFOutput, MFState, MFBoundary)) == CMfailed) ||		//RJS, changed MFBoundary o MFIniial
			    ((_MDOutResReleaseID    = MFVarGetID (MDVarReservoirRelease,       "m3/s", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
			    (MFModelAddFunction (_MDReservoirDW) == CMfailed)) return (CMfailed);
			break;
        case MDneuralnet:
			if (((_MDInDischargeID      = MDDischLevel2Def ()) == CMfailed) ||
			    ((_MDInResCapacityID    = MFVarGetID (MDVarReservoirCapacity,      "m3",  MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInDischMinID    = MFVarGetID (MDVarDischargeMin,      "m3/s",  MFInput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInDischMaxID    = MFVarGetID (MDVarDischargeMax,      "m3/s",  MFInput,  MFState, MFBoundary)) == CMfailed) ||
                ((_MDOutDisch_t_1_ID     = MFVarGetID(MDVarDischarge_t_1,      "m3/s",  MFOutput, MFState, MFInitial)) == CMfailed) ||
                ((_MDOutDisch_t_2_ID     = MFVarGetID(MDVarDischarge_t_2,      "m3/s",  MFOutput, MFState, MFInitial)) == CMfailed) ||
			    ((_MDOutResStorageID    = MFVarGetID (MDVarReservoirStorage,       "m3",  MFOutput, MFState, MFInitial))  == CMfailed) ||
			    ((_MDOutResStorageChgID = MFVarGetID (MDVarReservoirStorageChange, "m3",  MFOutput, MFState, MFBoundary)) == CMfailed) ||		//RJS, changed MFBoundary o MFIniial
			    ((_MDOutResReleaseID    = MFVarGetID (MDVarReservoirRelease,       "m3/s", MFOutput, MFFlux,  MFInitial)) == CMfailed) ||
			    ((_MDOutResRelease_t_1_ID    = MFVarGetID (MDVarReservoirRelease_t_1, "m3/s", MFOutput, MFFlux,  MFInitial)) == CMfailed) ||
			    (MFModelAddFunction (_MDReservoirNeuralNet) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Reservoirs");
	return (_MDOutResReleaseID); 
}
