/******************************************************************************

GHAAS Water Balance/Transport Model V2.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDRainSMoistLayered.c

dominik.wisser@unh.edu

*******************************************************************************/

#include <stdlib.h>
#include <math.h>
#include <MF.h>
#include <MD.h>

//float ActETP (float soilMoist[],float MaxSoilMoist[],float potETP, int soilLayerNum, int numSoilLayers)
//static float ActETP(float [], float[],float,int,int);
static float *ActETP(float [],float [], float [], float , int , float ,float [],float);
static float *VGn (float [], float [],int);
static float *VGm (float [],int);
static float *VGalpha (float [], float [],float[],int);

static float *SaturatedSoilMoist (float [], float [], float [], float [], int ); 
static float *WiltingPoint(float [], float [], float[],float[],int);
static float *FieldCapacity(float[],float[],float[],float[],int);
	 
float *ResidSoilMoisture(float[] , float[] ,int, float[]);
static float UnsaturatedConductivity (float , float , float, float, float );
static float SaturatedConductivity (float , float , float, float, float );
float MatrixPotential(float, float , float , float,float,float  );
static float _MDSoilMoistALPHA = 5.0;
 
static int _MDNumberOfSoilMoistureLayers =15;

static int _MDTimeSteps = 1;

// for each soil layer
// soil moisture
// 
// Input
 
static int _MDInPrecipID            = MFUnset;
static int _MDInPotETID             = MFUnset;
static int _MDInInterceptID         = MFUnset;
static int _MDInSPackChgID          = MFUnset;
static int _MDOutRainInfiltrationID = MFUnset;
static int _MDInIrrAreaFracID       = MFUnset;
static int _MDInSoilRootingDepthID  = MFUnset;
static int _MDOutSatExcessFlowID	= MFUnset;
static int _MDOutSoilDebugID	    =MFUnset;
static int _MDInIsInitialID			=MFUnset;

// for each SoilLayer ===========================================
static int *_MDInSoilpctSiltIDs		        = (int *) NULL;
static int *_MDInSoilpctClayIDs		        = (int *) NULL;
static int *_MDInSoilpctSandIDs		        = (int *) NULL;
static int _MDInSoilOrganicLayerID;
static int *_MDSoilRelativeSoilMoistIDs = (int *) NULL;

static int *_MDOutSMLiquidIDs				= (int *) NULL;
static int *_MDOutSMIceDs					= (int *) NULL;
static int *_MDInSoilDepthIDs				=(int *) NULL;
// ============================================================
int soilTemperatureID;

// Output
static int _MDOutEvaptrsID          = MFUnset;
static int _MDOutSoilMoistCellID    = MFUnset;
static int _MDOutSoilMoistID        = MFUnset;
static int _MDOutSMoistChgID        = MFUnset;
static int _MDOutWaterTableDepthID = MFUnset;
static int _MDActiveLayerDepthID   = MFUnset;
static void _MDRainSMoistChg (int itemID) {	
 
	float initialSoilMoisture=0.9;
	float precip;            // Precipitation [mm/dt]
	float potETP;               // Potential evapotranspiration [mm/dt]
 
	float snowpackChg;          // Snow pack change [mm/dt]
	float irrAreaFrac = 0.0; // Irrigated area fraction
	float meanSmoist      = 0.0; // Averaged over all layers Soil moisture [mm/dt]
	float meanSmoistChange   = 0.0; // Soil moisture change averaged over all layers [mm/dt]
	float waterTableDepthParameter =1.0;
	int i;
	float isInitial = MFVarGetFloat(_MDInIsInitialID,itemID,0.0);
	float rootDepth;			//in mm to be consistent with WBMstable data!
// Local
	float liquidIn;
 
	int numSoilLayers;
	numSoilLayers 	= 	_MDNumberOfSoilMoistureLayers;
 	precip       	= 		MFVarGetFloat 	(_MDInPrecipID,  		        itemID, 0.0);
	snowpackChg    	=	 	MFVarGetFloat 	(_MDInSPackChgID,				itemID, 0.0);
	potETP          = 		MFVarGetFloat 	(_MDInPotETID,					itemID, 0.0);
	//intercept       =	   _MDInInterceptID   != MFUnset ? MFVarGetFloat (_MDInInterceptID,   itemID, 0.0) : 0.0;
	//irrAreaFrac  	= 		_MDInIrrAreaFracID != MFUnset ? MFVarGetFloat (_MDInIrrAreaFracID, itemID, 0.0) : 0.0;
	rootDepth 	    = 		MFVarGetFloat (_MDInSoilRootingDepthID,  itemID, 0.0);
 
	float snowMelt;
	
	  if (snowpackChg >  0.0) //Snow Accumulation, no liquid precipitation
		 snowMelt = 0.0; 
	  	 liquidIn= precip - fabs(snowpackChg);
	  if (snowpackChg <= 0.0){ 
		  snowMelt = fabs(snowpackChg); //Add Snowmelt
	  	  liquidIn = precip + snowMelt;
	  }
	  	  
		int dt;
		float soilDepth				[numSoilLayers];
		float accumSoilDepth		[numSoilLayers];
		float vertFlow				[numSoilLayers-1];
		float ice 					[numSoilLayers];	
		
		float satExcess				[numSoilLayers];	
		float _liq 					[numSoilLayers];
		float _previousLiq			[numSoilLayers];
		 
		float bulkDensity			[numSoilLayers];
		float pctSand				[numSoilLayers];
		float pctSilt				[numSoilLayers];
		float pctClay				[numSoilLayers];
		float pctOrganicM			[numSoilLayers];
		float inf2LowestLayer=0;
		float *actETP;
		float *VanGn;
		float *VanGa;
		float *VanGm;
		float *permWP;
		float *fieldCap;

		float *residSoilMoist;
		float *maxSoilMoist;
		float baseflow=0;
		float dt_inflow =0;
		float surfaceRO=0;
		float waterTableDepth;
		float activeLayerDepth;
		float waterInAllLayers=0;
		float iceInAllLayers=0;
		float waterIN=0;
		float waterOUT=0;
		float smPrevious=0;
		float smCurrent=0;
		float actualET=0;
		float dt_surfaceRO=0;
		float balance;
		float K1;float K1Sat;
		float K2;float K2Sat;
		waterTableDepth= MFVarGetFloat(_MDOutWaterTableDepthID,itemID,0.0);

		for (i=0;i<numSoilLayers;i++){  
			soilDepth[i] = 1000 * MFVarGetFloat( _MDInSoilDepthIDs[i], itemID,0.0); // in mm!
			if (soilDepth[i]==0) soilDepth[i]=10; //set soilDepth to 10 mm if no data.
			pctSand[i]= MFVarGetFloat( _MDInSoilpctSandIDs[i], itemID,0.0); 
			pctSilt[i]= MFVarGetFloat( _MDInSoilpctSiltIDs[i], itemID,0.0); 
			pctClay[i]= MFVarGetFloat( _MDInSoilpctClayIDs[i], itemID,0.0); 
			pctOrganicM[i]=2.5;
		 	bulkDensity[i]=1600;
		   _previousLiq[i]=MFVarGetFloat( _MDOutSMLiquidIDs[i], itemID,0.0);
		

		//	if (itemID == 200)printf("PrevSM = smPrevious %f lowestLayer %f  \n",smPrevious, _previousLiq[numSoilLayers-1]);
			ice[i]=0.0;		
		
		}
		
		if(soilTemperatureID==1){
			
		//	activeLayerDepth= MFVarGetFloat( _MDActiveLayerDepthID, itemID,0.0); 
		//	printf ("ActiveLayerDepth = %f\n", activeLayerDepth);
			
		}
		 
		residSoilMoist= ResidSoilMoisture(pctOrganicM, pctClay,numSoilLayers, soilDepth);
		//		
				 
		for (i=0;i<numSoilLayers;i++){
			accumSoilDepth[i] =soilDepth[i];
	//		if (_previousLiq[i]< 0 ) printf("SM prolbme\n");
			//ice[i]=MFVarGetFloat( _MDOutSMIceDs[i], itemID,0.0);
			if (i < numSoilLayers-1)smPrevious+=_previousLiq[i]+ice[i]-residSoilMoist[i];//do not incldue changes in lowest layer in balance
					
		}
		accumSoilDepth[0]=soilDepth[0];
		for (i=1;i<numSoilLayers;i++){accumSoilDepth[i] +=accumSoilDepth[i-1]+soilDepth[i];}
		float prevLowestLayerLiq = _previousLiq[numSoilLayers-1];
		float lowestLayerChange=0;
		maxSoilMoist = SaturatedSoilMoist(pctSand,pctClay,pctOrganicM,soilDepth,numSoilLayers);
		//maxSoilMoist[numSoilLayers -1] = 1e20; //last layer is runoff pool... infinite capactity
//		
		VanGn=VGn(pctSand, pctClay, numSoilLayers);
		fieldCap = FieldCapacity(pctSand,pctClay,pctOrganicM,soilDepth,numSoilLayers);
		permWP= WiltingPoint(pctSand,pctClay,pctOrganicM,soilDepth, numSoilLayers);
//		
//		kSat=SatConductivity (pctSand,pctClay,pctOrganicM,numSoilLayers); // mm/timeStep
		VanGa= VGalpha (pctSand, pctClay, bulkDensity,numSoilLayers);
//		
		VanGm = VGm(VanGn,numSoilLayers);
		
		//TODO FieldCapacity(pctSand,pctClay,pctOrganicM,numSoilLayers);
		//time step limit courant number..
		
		//dt < dz/Ksat
	
		if (itemID ==124){
			int j;
			for (j =0; j < numSoilLayers;j++){
			//	printf ("SoilDepth for layer %i : %f\n", j, soilDepth[j]);
			}
			
		}
		
		potETP=MFVarGetFloat (_MDInPotETID,             itemID, 0.0)/_MDTimeSteps;
		//DEBUG
		//liquidIn=0;
		//if (MFDateGetDayOfYear ()== 1) liquidIn =10; 
				
		float gamma = 0.8;
		 
		dt_inflow = (liquidIn / 		_MDTimeSteps)* (gamma);
		dt_surfaceRO  = liquidIn - dt_inflow;
////		printf ("hier\n");
		for (dt = 0;dt<_MDTimeSteps; dt ++){
			surfaceRO += dt_surfaceRO;
			actETP= ActETP ( _previousLiq, fieldCap, permWP,  potETP, numSoilLayers, rootDepth,accumSoilDepth,activeLayerDepth);
		//	if (itemID==606)printf ("ItemID = %i  MFDateGetDayOfYear %i prevLiq = %f  maxSM %f   soilDepth %f \n",itemID,MFDateGetDayOfYear(), _previousLiq[0], maxSoilMoist[0], soilDepth[0]);

			if (isInitial <1.0)	_previousLiq[numSoilLayers-1]= initialSoilMoisture * maxSoilMoist[i];
			for (i=0;i<numSoilLayers-1;i++){
		//		if (_previousLiq[i] > maxSoilMoist[i]) printf ("Previous SM =%f maxSM =%f \n", _previousLiq[i] , maxSoilMoist[i]);
			
				if (isInitial <1.0){
					_previousLiq[i] = initialSoilMoisture * maxSoilMoist[i];
				//	if (itemID==139)printf ("ItemID = %i  MFDateGetDayOfYear %i Is Initial prevLiq = %f  maxSM %f   soilDepth %f \n",itemID,MFDateGetDayOfYear(), _previousLiq[0], maxSoilMoist[0], soilDepth[0]);
				}
				_previousLiq[i]=MDMaximum(residSoilMoist[i],_previousLiq[i]);
				if (i==0) {
					_liq[i]= _previousLiq[i]+dt_inflow-actETP[i]; 
//							initialSoilMoisture * maxSoilMoist[i];
				}else{
					_liq[i]= _previousLiq[i]- actETP[i]+vertFlow[i-1];
					if (_liq[i]< 0){
						//printf ("liq < 0: %f at item %i layer %i   \n", _liq[i], itemID, i);
						_liq[i]=0;
					}
				}
////				

				K1 =        UnsaturatedConductivity (pctSand[i], pctClay[i],pctOrganicM[i], _liq[i],maxSoilMoist[i]);
				K2=         UnsaturatedConductivity (pctSand[i+1], pctClay[i+1],pctOrganicM[i+1], _liq[i],maxSoilMoist[i+1]);
				K1Sat =     SaturatedConductivity (pctSand[i], pctClay[i],pctOrganicM[i], _liq[i],maxSoilMoist[i]);
				K2Sat=      SaturatedConductivity (pctSand[i+1], pctClay[i+1],pctOrganicM[i+1], _liq[i],maxSoilMoist[i+1]);

				float meanKS = (K1 + K2) / 2; 				
				meanKS = MDMaximum (K1Sat, meanKS);
				meanKS = MDMaximum (K2Sat, meanKS);
 				float matrHead1;
				float matrHead2;
				matrHead1= MatrixPotential( VanGa[i], _liq[i], VanGm[i], VanGn[i], residSoilMoist[i], maxSoilMoist[i] );	
				matrHead2 = MatrixPotential( VanGa[i+1], _liq[i+1], VanGm[i+1], VanGn[i+1], residSoilMoist[i+1], maxSoilMoist[i+1] );	
				
				float Grad = (matrHead1 - matrHead2) / ((soilDepth[i] + soilDepth[i+1])/2);
			 vertFlow[i]=-Grad * meanKS ;
			 vertFlow[i]=MDMinimum(vertFlow[i],_liq[i]);
			 vertFlow[i]=MDMaximum(vertFlow[i],0);

			_liq[i]=_liq[i]-vertFlow[i];	
		} //numSoilLayers
		
////			
////						
			 // no drainage from lowest layr ..now taken care of in VGHydrConductivity
			vertFlow[i]=MDMaximum(0,vertFlow[i]);
////	
			satExcess[numSoilLayers-1]=0.0;
////			
	//	if (itemID==130)printf ("============Day number %i\n",MFDateGetDayOfYear ());
			for (i=numSoilLayers-2;i>-1;i--){ // from bottom !!
		//		if (itemID==626) printf("itemID %i Day %i Layer %i liq[0]=%f liq[1]=%f liq[2]=%f liq[3]=%f liq[4]=%f liq[5]=%f liq[6]=%f\n",itemID,MFDateGetDayOfYear(), i,_liq[0], _liq[1], _liq[2],_liq[3], _liq[4], _liq[5] ,_liq[6]);

					if (_liq[i] + ice[i] + satExcess[i+1] >= maxSoilMoist[i]){
						satExcess[i]= _liq[i]+ice[i]+satExcess[i+1] - maxSoilMoist[i];
						if (i==0){
				//		if (itemID==200)printf ("satExcess[i] %f _liq[i] %f ice[i] %f satExcess[i+1] %f maxSoilMoist[i] %f  \n",satExcess[i],_liq[i],ice[i],satExcess[i+1], maxSoilMoist[i]);
						
						surfaceRO+= satExcess[i];
								
						}else{
			//			if (itemID==139)printf (" liqvorher %f for layer %i vert1 %f vert2 %f itemID %i \n", _liq[i],i, vertFlow[i], vertFlow[i+1],itemID);
				//		_liq[i]= maxSoilMoist[i]-ice[i];
					//	vertFlow[i]=0;
				//		if (itemID==139)printf (" liqnachher %f for layer %i vert1 %f vert2 %f itemID %i maxSM %f ice %f stEXcess %f maxSM %f \n", _liq[i],i, vertFlow[i], vertFlow[i+1],itemID, maxSoilMoist[i],ice[i],satExcess[i], maxSoilMoist[i] );
											 
					}
////////				
////////					
				}else{
//////					
					satExcess[i]=0;
				}
					if (i>=1)  _liq[i]= _previousLiq[i]-satExcess[i]+satExcess[i+1]- vertFlow[i]- actETP[i]+vertFlow[i-1];
					if (0==i) _liq[i]=  _previousLiq[i]-surfaceRO+satExcess[i+1]- vertFlow[i]- actETP[i]+liquidIn;
			//		if (_liq[i]<0)printf ("Liq < 0 at item %i  Layer %i Liq %fsatExcess %f\n",itemID,i,_liq[i], satExcess[i]);
////				if (itemID==200)printf ("i%i satExcess%f surfRO%f\n", i,satExcess[i], surfaceRO);
//////			// water balance for each layer
					if (i >0)waterIN = vertFlow[i-1] + satExcess[i+1];
					if (i>0)waterOUT = actETP[i] +vertFlow[i]+(_liq[i]-_previousLiq[i])+satExcess[i];
					balance =waterIN-waterOUT;
					if (fabs(balance) >= 0.001 && i >0 ){
						printf("Delta = %f waterIN = %f itemID %i Layer %i vertFlow[i-1] %f  satExcess[i+1] %f maxSM %f\n",balance, waterIN,itemID, i, vertFlow[i-1] , satExcess[i+1], maxSoilMoist[i]);
						printf("Deltad = %f waterOUT = %f itemID %i Layer %i  actETP[i] %f vertFlow[i] %f (_liq[i] %f ,_previousLiq[i]) %f satExcess[i]%f \n",balance,waterOUT,itemID, i,  actETP[i] ,vertFlow[i],_liq[i],_previousLiq[i], satExcess[i]);

						if (itemID==139)printf("Delta = %f waterIN = %f itemID %i Layer %i vertFlow[i-1] %f  satExcess[i+1] %f maxSM %f\n",balance, waterIN,itemID, i, vertFlow[i-1] , satExcess[i+1], maxSoilMoist[i]);
						if (itemID==139)printf("Deltad = %f waterOUT = %f itemID %i Layer %i  actETP[i] %f vertFlow[i] %f (_liq[i] %f ,_previousLiq[i]) %f satExcess[i]%f \n",balance,waterOUT,itemID, i,  actETP[i] ,vertFlow[i],_liq[i],_previousLiq[i], satExcess[i]);
				//								accumSoilDepth[i-1]
		//				 printf("WaterBalanceError in Layer %i Depth=%f at item %i waterOUT = %f  actETP[i] %f satExcess[i] %f vertFlow[i] %f deltaS %f\n",i,soilDepth[i],itemID, waterOUT,actETP[i],  satExcess[i], vertFlow[i], (_liq[i]-_previousLiq[i]));
						 						 
						//if (itemID==200)printf ("waterIN = %f\n",waterIN);
						//if (itemID==200)printf ("waterOUT = %f\n",waterOUT);
												
					}
			} //for looop form bottom layer
////		
////			if (itemID==200)printf ("i%i liqnacher %f \n", i,_liq[0]);
////		
////			
	//			if (itemID==200) printf("itemID %i max0 =%f max1 =%f max2=%f max3=%f\n",itemID, maxSoilMoist[0],maxSoilMoist[1],maxSoilMoist[2],maxSoilMoist[3]);
////					//
////			//Last layer is runoff detention pool ! 
				float beta = 0.0167;
				inf2LowestLayer  +=vertFlow[numSoilLayers-2];
				baseflow = beta* _liq[numSoilLayers-1];
				_liq[numSoilLayers-1]=_previousLiq[numSoilLayers-1] + vertFlow[numSoilLayers-2] -baseflow;
	//		if (itemID==139) printf("itemID %i Day %i TimEStp%i IN=%f  et = %f liq0 =  %f liq1 = %f liq2=%f liq3=%f liq4 %f surfaceRO %f dt_inflow %f EXCESS[1] %f \n",itemID,MFDateGetDayOfYear(), dt,liquidIn,actETP[0], _liq[0], _liq[1], _liq[2], _liq[3],_liq[4], surfaceRO,dt_inflow, satExcess[1]);
				////	
////		
////		 
			} // time step loop
//		
//float watbal0;
//float in0 = dt_inflow;
////float out0=vertFlow[0];
		float soilMoist= 0; 	
//		float iceInAllLayers=0;
		for (i=0;i<numSoilLayers-1;i++){ // exclude lowest layer from calculation
			smCurrent+=_liq[i]+ice[i]-residSoilMoist[i];
		//	if (itemID==626) printf ("i %i ice[i] %f residSoilMoist[i] %f _liq[i] %f\n", itemID,ice[i],residSoilMoist[i],_liq[i]);
			_previousLiq[i]=_liq[i]-residSoilMoist[i];
			actualET +=actETP[i];
//			if (_liq[i]<0)printf ("SM negative! %f\n", _liq[i]);
			waterInAllLayers+=_liq[i]+ice[i]-residSoilMoist[i];
//			if (itemID==200)printf("waterinAllLayers %f\n", waterInAllLayers);
			iceInAllLayers+=ice[i];
			soilMoist += (_liq[i]+ice[i])/maxSoilMoist[i] * soilDepth[i] ; 
			MFVarSetFloat (_MDOutSMLiquidIDs[i], itemID, _liq[i]);
			MFVarSetFloat (_MDOutSMIceDs[i], itemID, 0);
			MFVarSetFloat (_MDSoilRelativeSoilMoistIDs[i], itemID, (_liq[i]+ice[i])/maxSoilMoist[i]);
		}
		MFVarSetFloat (_MDOutSMLiquidIDs[numSoilLayers-1], itemID, _liq[numSoilLayers-1]);
		
	//		
		
		waterTableDepth = waterTableDepthParameter* (accumSoilDepth[numSoilLayers-1] - soilMoist);
//		if (itemID==200)printf ("WaterTableDepth = %f Param %f depth %f moist %f \n", waterTableDepth,  waterTableDepthParameter, accumSoilDepth[numSoilLayers-1], soilMoist);

//		if (itemID==200) printf("itemID %i Day %i TimEStp%i IN=%f  et = %f ice1 =  %f ice2 = %f ice3=%f ice4=%f surfaceRO %f dt_inflow %f WP %f\n",itemID,MFDateGetDayOfYear(), dt,liquidIn,actETP[1], ice[0], ice[1], ice[2], ice[3], surfaceRO,dt_inflow,permWP[1]);
//	
//	if (itemID==200) 	 printf("WaterInAllLayers %f\n", waterInAllLayers);
		lowestLayerChange=  _liq[numSoilLayers-1]-prevLowestLayerLiq-residSoilMoist[numSoilLayers-1]; 
//	//	if (itemID==267)printf("waterin all layers = %f ice %f inFlow %f surfRO %f dSLowestLayer%f \n",waterInAllLayers, iceInAllLayers,liquidIn, surfaceRO,baseflow);
//			
		meanSmoistChange= smCurrent-smPrevious;
//		if (meanSmoistChange > 200)printf ("meanSmoistChange= %f smCurrent = %f smPrevious = %f\n",meanSmoistChange, smCurrent,smPrevious );
//		
		waterIN = liquidIn;
		waterOUT = meanSmoistChange +  surfaceRO + actualET +lowestLayerChange ;
		balance= waterIN - waterOUT;
//	if (itemID==200)		printf ("WaterINAllLayers %f TimeSteps %i OUT =%f meanSmoistChange= %f surfaceRO %f actualET %f lowestLayerChange %f\n",waterInAllLayers+_liq[numSoilLayers-1], _MDTimeSteps,waterOUT,meanSmoistChange,surfaceRO,actualET,lowestLayerChange); 	
//	
		if (fabs(balance) > 0.001 && isInitial > 0){
	//	printf (" ====WaterBalance for item %i %f in MDRainSolMoisLayered!in =%f out%f item%i surfRO %f maxSoil%f baseFlowDelta %f item %i lowestLayerChange %f\n",itemID, balance, waterIN, waterOUT,itemID, surfaceRO, maxSoilMoist[0], _liq[numSoilLayers-1], itemID,lowestLayerChange);
 	//		if (itemID==200)	printf (" ====WaterBalance in MDRainSolMoisLayered!in =%f out%f item%i surfRO %f maxSoil%f baseFlowDelta %f item %i\n", waterIN, waterOUT,itemID, surfaceRO, maxSoilMoist[0], _liq[numSoilLayers-1], itemID); 
	//		if (itemID==200)	printf ("PrevSMLowest %f liqIN = %f deltaS = %f  surfaceRO %f actualET %f currentSM%f previousSM%f \n", prevLowestLayerLiq, waterIN, meanSmoistChange, surfaceRO,actualET, smCurrent, smPrevious); 
			//if (itemID==200)	printf ("PrevSMLowest %f liqIN = %f deltaS = %f  surfaceRO %f actualET %f currentSM%f previousSM%f \n", prevLowestLayerLiq, waterIN, meanSmoistChange, surfaceRO,actualET, smCurrent, smPrevious); 
	//		if (itemID==200)	printf ("maxSM 1 %f maxSM 2 %f maxSM 3 %f maxSM %f \n",maxSoilMoist[0],maxSoilMoist[1],maxSoilMoist[2],maxSoilMoist[3]); 	
			
		}
//	if (itemID==200) printf("itemID %i Day %i TimEStp%i IN=%f  et = %f liq1 =  %f liq2 = %f liq3=%f liq4=%f liq5 %f surfaceRO %f dt_inflow %f \n",itemID,MFDateGetDayOfYear(), dt,liquidIn,actETP[0], _liq[0], _liq[1], _liq[2], _liq[3],_liq[4], surfaceRO,dt_inflow);
			
//	

	MFVarSetFloat (_MDOutSoilMoistCellID, itemID, meanSmoist);
	MFVarSetFloat (_MDOutEvaptrsID,       itemID, actualET * (1.0 - irrAreaFrac));
	MFVarSetFloat (_MDOutSoilMoistID,     itemID, meanSmoist     * (1.0 - irrAreaFrac)); 
	MFVarSetFloat (_MDOutRainInfiltrationID,     itemID,  inf2LowestLayer  * (1.0 - irrAreaFrac)); 
	MFVarSetFloat (_MDOutSatExcessFlowID,     itemID, surfaceRO  * (1.0 - irrAreaFrac)); 
	MFVarSetFloat (_MDOutSMoistChgID,     itemID, meanSmoistChange  * (1.0 - irrAreaFrac));
	MFVarSetFloat (_MDOutSoilDebugID, itemID, K1Sat);
	MFVarSetFloat (_MDOutWaterTableDepthID, itemID, waterTableDepth);
	MFVarSetFloat (_MDInIsInitialID,    			 itemID,  1.0);
	//printf ("SatExcessFlow = %f\n",  MFVarGetFloat(_MDOutSatExcessFlowID,itemID,0.0));
	free(actETP);// c sucks c sucks c totally sucks
	free (VanGn);
	free(fieldCap);
	free (permWP);
	free(residSoilMoist);
	free(maxSoilMoist);
	free(VanGa); free(VanGm);
	//free(kSat);
////	printf ("hier2\n");
}

int MDRainSMoistChgLayeredSoilDef () {
	int ret = 0;
	float par;
	int i;
	char soilLiquidName [16];
	char soilSiltFractionName[22];
	char soilClayFractionName[22];
	char soilSandFractionName[22];
	char soilRelativeSoilMoistName[30];
		
	char soilIceName[20];
	char soilDepthName[20];
	const char *optStr;
	const char *soilTemperatureOptions [] = { "none", "calculate", (char *) NULL };

	if (_MDOutSMoistChgID != MFUnset) return (_MDOutSMoistChgID);

		if (((optStr = MFOptionGet (MDOptSoilTemperature))  == (char *) NULL) || ((soilTemperatureID = CMoptLookup (soilTemperatureOptions, optStr, true)) == CMfailed)) {
						CMmsgPrint(CMmsgUsrError," Soil TemperatureOption not specifed! Options = 'none' or 'calculate'\n");
						return CMfailed;
		}

		MFDefEntering ("Rainfed Layered Soil Moisture");
		if (soilTemperatureID ==1){
	//		printf ("SoilTemp is on before\n");
		
			if ((ret = MDPermafrostDef ()) == CMfailed) return (CMfailed);
			 
//			printf ("SoilTemp is on fater \n");
		//	 if ((_MDActiveLayerDepthID= MFVarGetID (MDVarActiveLayerDepth, "mm", MFOutput, MFState, MFBoundary))  == CMfailed) return CMfailed;	
			 if  ((_MDActiveLayerDepthID          = MFVarGetID (MDVarActiveLayerDepth,             "m", MFOutput,  MFState, MFBoundary)) == CMfailed)return CMfailed;
				 
		}
		
	if (((optStr = MFOptionGet (MDParSoilMoistALPHA))  != (char *) NULL) && (sscanf (optStr,"%f",&par) == 1)) _MDSoilMoistALPHA = par;
	if (((optStr = MFOptionGet ("SoilMoistureTimeSteps"))  != (char *) NULL) && (sscanf (optStr,"%f",&par) == 1)) _MDTimeSteps = par;
	if (((optStr = MFOptionGet ("SoilMoistureTimeSteps"))  != (char *) NULL) && (sscanf (optStr,"%f",&par) == 1)) _MDTimeSteps = par;

	if ((ret = MDIrrGrossDemandDef ()) == CMfailed) return (CMfailed);
	if ((ret != MFUnset)  &&
	  ((_MDInIrrAreaFracID         = MDIrrigatedAreaDef    ())==  CMfailed) )
		return (CMfailed);

		for (i = 0; i < _MDNumberOfSoilMoistureLayers; i++) {
			_MDInSoilDepthIDs = (int *) realloc (_MDInSoilDepthIDs, (i + 1) * sizeof (int));
			_MDOutSMLiquidIDs = (int *) realloc (_MDOutSMLiquidIDs, (i + 1) * sizeof (int));
			_MDOutSMIceDs	  = (int *) realloc (_MDOutSMIceDs,     (i + 1) * sizeof (int));
			_MDInSoilpctClayIDs  = (int *) realloc (_MDInSoilpctClayIDs,     (i + 1) * sizeof (int));
			_MDInSoilpctSandIDs  = (int *) realloc (_MDInSoilpctSandIDs,     (i + 1) * sizeof (int));
			_MDInSoilpctSiltIDs  = (int *) realloc (_MDInSoilpctSiltIDs,     (i + 1) * sizeof (int));
			_MDSoilRelativeSoilMoistIDs  = (int *) realloc (_MDSoilRelativeSoilMoistIDs,     (i + 1) * sizeof (int));
				
			sprintf (soilDepthName,  "SoilDepth_%02d",  i + 1); //
			sprintf (soilLiquidName, "SoilLiquid_%02d", i + 1); //  
			sprintf (soilIceName, "SoilIce_%02d", i + 1); //  
			sprintf (soilSiltFractionName, "SoilSiltPercentage_%02d", i + 1); //  	
			sprintf (soilSandFractionName, "SoilSandPercentage_%02d", i + 1); //
			sprintf (soilClayFractionName, "SoilClayPercentage_%02d", i + 1); //
			sprintf (soilRelativeSoilMoistName, "SoilRelativeSoilMoisture_%02d", i + 1); //
			
			if ((_MDInSoilDepthIDs [i] = MFVarGetID (soilDepthName, "mm", MFInput,  MFState, MFBoundary)) == CMfailed) {
				printf("CMfailed in MDInSoilInputDataID \n");
				CMmsgPrint (CMmsgUsrError, "CMfailed in MDInSoilDepth \n");
				return CMfailed;
			}
		 
			if ((_MDOutSMLiquidIDs [i] = MFVarGetID (soilLiquidName, "mm", MFOutput,  MFState, MFInitial)) == CMfailed) {
							printf("CMfailed in MDInSoilInputDataID \n");
							CMmsgPrint (CMmsgUsrError, "CMfailed in MDInSoilDepth \n");
							return CMfailed;
						}
			if ((_MDOutSMIceDs [i] = MFVarGetID (soilIceName, "mm", MFOutput,  MFState, MFInitial)) == CMfailed) {
							printf("CMfailed in MDInSoilInputDataID \n");
							CMmsgPrint (CMmsgUsrError, "CMfailed in MDInSoilDepth \n");
							return CMfailed;
						}
			
			if ((_MDInSoilpctSiltIDs [i] = MFVarGetID (soilSiltFractionName, "mm", MFInput,  MFState, MFBoundary)) == CMfailed) {
									printf("CMfailed in MDInSoilInputDataID \n");
									CMmsgPrint (CMmsgUsrError, "CMfailed in MDInSoilDepth \n");
									return CMfailed;
								}
			
			if ((_MDInSoilpctSandIDs [i] = MFVarGetID (soilSandFractionName, "mm", MFInput,  MFState, MFBoundary)) == CMfailed) {
											printf("CMfailed in MDInSoilInputDataID \n");
											CMmsgPrint (CMmsgUsrError, "CMfailed in MDInSoilDepth \n");
											return CMfailed;
										}
			
			
			if ((_MDInSoilpctClayIDs [i] = MFVarGetID (soilClayFractionName, "mm", MFInput,  MFState, MFBoundary)) == CMfailed) {
													printf("CMfailed in MDInSoilInputDataID \n");
													CMmsgPrint (CMmsgUsrError, "CMfailed in MDInSoilDepth \n");
													return CMfailed;
												}
			
			if ((_MDSoilRelativeSoilMoistIDs [i] = MFVarGetID (soilRelativeSoilMoistName, "mm", MFOutput,  MFState, MFBoundary)) == CMfailed) {
																printf("CMfailed in MDInSoilInputDataID \n");
																CMmsgPrint (CMmsgUsrError, "CMfailed in MDInSoilDepth \n");
																return CMfailed;
															}
		
			
			
			 
		}
		
	   if ((_MDOutRainInfiltrationID= MFVarGetID (MDVarRainInfiltration, "mm", MFOutput, MFState, MFBoundary))  == CMfailed) return CMfailed;	
	   if ((_MDOutSatExcessFlowID= MFVarGetID (MDVarSaturationExcessflow, "mm", MFOutput, MFState, MFBoundary))  == CMfailed) return CMfailed;	
	   if ((_MDOutSoilDebugID = MFVarGetID(MDVarOutSoilDebug,"mm", MFOutput, MFState, MFBoundary))  == CMfailed) return CMfailed;	
	
	if (((_MDInPrecipID            = MDPrecipitationDef     ()) == CMfailed) ||
	    ((_MDInSPackChgID          = MDSPackChgDef          ()) == CMfailed) ||
	    ((_MDInPotETID             = MDRainPotETDef         ()) == CMfailed) ||
	    ((_MDInInterceptID         = MDRainInterceptDef     ()) == CMfailed) ||
		((_MDOutWaterTableDepthID = MFVarGetID (MDVarWaterTableDepth,       "mm",   MFOutput, MFState,  MFBoundary)) == CMfailed) ||
	   ((_MDInSoilOrganicLayerID = MFVarGetID (MDVarSoilOrganicLayer,       "kg/m3",   MFInput, MFState,  MFBoundary)) == CMfailed) ||
			
	//    ((_MDInSoilAvailWaterCapID = MDSoilAvailWaterCapDef ()) == CMfailed) ||
	    ((_MDOutEvaptrsID          = MFVarGetID (MDVarRainEvapotranspiration,     "mm",   MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDInSoilRootingDepthID   = MFVarGetID (MDVarSoilRootingDepth,  "mm",   MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInIsInitialID          = MFVarGetID (MDVarSoilMoistureIsInitial,             "m", MFOutput,  MFState, MFInitial)) == CMfailed) ||

 	     	((_MDOutSoilMoistCellID    = MFVarGetID (MDVarRainSoilMoistureCell,       "mm",   MFOutput, MFState, MFInitial))  == CMfailed) ||
	    ((_MDOutSoilMoistID        = MFVarGetID (MDVarRainSoilMoisture,           "mm",   MFOutput, MFState, MFBoundary)) == CMfailed) ||
	         
	    ((_MDOutSMoistChgID        = MFVarGetID (MDVarRainSoilMoistChange,        "mm",   MFOutput, MFState, MFBoundary)) == CMfailed) ||
        
        (MFModelAddFunction (_MDRainSMoistChg) == CMfailed)) return (CMfailed);

	MFDefLeaving ("Rainfed Layered Soil Moisture");
	return (_MDOutSMoistChgID);
}
 
float *ResidSoilMoisture (float pctCarbon [], float pctClay[], int numSoilLayers, float soilDepth[]){
	int i;
	float *rs;
	
	rs = malloc(numSoilLayers*sizeof *rs);
	for (i=0;i<numSoilLayers;i++){
		rs[i]=0.015+0.005 * pctClay[i] + 0.014 * pctCarbon[i]* soilDepth[i];
		rs[i]=0;
//		printf ("RS= %f\n", rs[i]);
	}
	rs[numSoilLayers-1]=0;
	return rs;
	
}
 
float *ActETP (float soilMoist[],float fieldCap[], float wiltPnt[], float potETP,  int numSoilLayers, float rootDepth,float accumSoilDepth[],float activeLayerDepthInMeters)
{
int i;
float *et;
et = malloc(numSoilLayers*sizeof *et);

float accum=0;
for (i=0;i<numSoilLayers-1;i++){et[i]=0.0;}

float stressFactor;
for (i=0;i<numSoilLayers-1;i++){
	int rootsInLayer=1;
	if (i>1){
		if (accumSoilDepth[i-1]> rootDepth ) rootsInLayer=0;
	}
//	printf ("accumDepth [i-1]%f rootdepth %f activeLayerDepth %f \n",accumSoilDepth[i-1], rootDepth,activeLayerDepthInMeters);
	et[i]=0;
	stressFactor =0;
	if (soilMoist[i] > wiltPnt[i]){stressFactor= (soilMoist[i]-wiltPnt[i])/(fieldCap[i]-wiltPnt[i]);}
//	if (stressFactor < 0)stressFactor=0; //TODO
	stressFactor =1;
	if (stressFactor>1)stressFactor =1;
	
	if (rootsInLayer ==1){
		et[i] = MDMinimum(soilMoist[i],potETP * stressFactor-accum);
		if (et[i]<0)et[i]=0;
	//	printf ("ET= %f PotET = %f liq = %f RD =%f, accumDepth %f\n", et[i], potETP, soilMoist[i], rootDepth, accumSoilDepth[i]);
		
		accum +=et[i];
		
		if (accum > potETP){
			et[i] = (accum - potETP) ;
			break; // enough water, leave for loop
		}
		
	}		
	// et[i]=0;
}

et[numSoilLayers -1]=0; // no et from lowest layer - baseflow
return et;//et[soilLayerNum];
}

// c sucks soo much 
float MatrixPotential(float VanGa, float soilMoisture, float VanGm, float VanGn,float resSoilMoisture,float maxSoilMoisture ){
 
	float matPot;
	//		 if (soilMoisture > resSoilMoisture){
			 
			float effSat = (soilMoisture- resSoilMoisture)/(maxSoilMoisture-resSoilMoisture);
			

			if (effSat < 0.00) effSat =0.00001;
			if (effSat > 1.0) effSat =1;
			float junk1 = pow(effSat,(-1/VanGm));
			float junk2 = junk1 -1;
			float junk3 = pow(junk2,(1/VanGn));
			matPot =  junk3/VanGa;
		
	//	 }
			 
	//		 else{
	//			 matPot=maxPot; 
	//		  }
	if (isinf(matPot) || isnan(matPot)){	
	//	printf (" EffSat %f Matrixpo= %f a = %f n=%f m=%f soilMo%f max%f residualSoilMoist%f\n",effSat, matPot, VanGa, VanGn, VanGm, soilMoisture, maxSoilMoisture, resSoilMoisture);
	matPot =1e20;
	}
		 return matPot;
}


float *VGn(float pctSand[], float pctClay[],int numSoilLayers){
	//Verrecken method
	float *n;
	
	int i;
	n = malloc(numSoilLayers*sizeof *n);
	 for (i=0;i<numSoilLayers;i++){
		 float lnn =  0.053-0.009* pctSand[i]*100 -0.013* pctClay[i]*100 +0.00015* pow(pctSand[i]*100,2); 
		 n[i]=exp(lnn);
		 n[i]=1.49;
	 }
	//float lnn =
	return n;
}

float *VGm(float VG_n[],int numSoilLayers){
	float *m;
	int i;
	m = malloc(numSoilLayers*sizeof *m);
	 for (i=0;i<numSoilLayers;i++){
		 
		 m[i]=1-1/VG_n[i];
	 }
	//float lnn =
	return m;
}


float *VGalpha(float pctSand[], float pctClay[],float bulkD[],int numSoilLayers){
	//Verreecken Method
	float *a;
	int i;
	a = malloc(numSoilLayers*sizeof *a);
	 for (i=0;i<numSoilLayers;i++){
		 float lna =  -2.486+0.025 * pctSand[i]*100 -0.351* pctClay[i]*100  - 2.617 * bulkD[i]/1000 -0.023 * pctClay[i]*100; 
		 a[i]=exp(lna);
		 a[i] = MDMaximum (0.001, a[i]);
	 
	 }
	return a;
}

float *SaturatedSoilMoist (float Sand[], float Clay[], float OrganicMatter[], float soilDepth[], int numSoilLayers){
	int i;
		float *sat;
		sat = malloc(numSoilLayers*sizeof *sat);
		for (i=0;i<numSoilLayers;i++){
		float phi_33t;
		float phi_33;
		float phi_s33t;
		float phi_s33;
		phi_33t= -0.251* Sand[i] +0.195* Clay[i] +0.011* OrganicMatter[i] +0.006 *(Sand[i] * OrganicMatter[i]) -0.027*(Clay[i] * OrganicMatter[i]) +0.452*(Sand[i]*Clay[i])+0.299;
		phi_33 =phi_33t + (1.283*phi_33t*phi_33t -0.374 * phi_33t-0.015);

		phi_s33t = 0.278 * Sand[i] + 0.034 * Clay[i]  + 0.022 * OrganicMatter[i] -0.018 *(Sand[i]* OrganicMatter[i]) -0.027 *(Clay[i] * OrganicMatter[i]) -0.584* (Sand[i] * Clay[i] ) +0.078;

		phi_s33= phi_s33t +0.6360 * phi_s33t -0.107;
		
		sat[i] = phi_33 + phi_s33 - 0.097 * Sand[i]+0.043;
		sat[i]= sat[i]* soilDepth[i];///1000;
		
	//	printf ("sat SM %f=  soilDepth + %f \n", sat[i], soilDepth[i]);
		}
	
return sat;
}

float *WiltingPoint (float Sand[], float Clay[], float OrganicMatter[], float soilDepth[], int numSoilLayers){
	//should not be applied beyon organicMatter > 8% 
	// equations from Saxton and Rawls (2006); Soil water characteristics
	// get size of array : 
		
//	printf ("numLayers %i soilDp3 %f \n", numSoilLayers, soilDepth[3]);

	int i;
	float *wp;
	wp = malloc(numSoilLayers*sizeof *wp);
	for (i=0;i<numSoilLayers;i++){
	float phi_1500t;
	
	phi_1500t= -0.024* *(Sand)+0.487 * *(Clay)+0.006* *(OrganicMatter) +0.005* (*(Sand )* *(OrganicMatter))-0.013*(*(Clay)* *(OrganicMatter))+0.068*(*(Sand) * *(Clay))+0.031;
	wp[i] =(phi_1500t+(0.14*phi_1500t -0.02))*soilDepth[i];
	if (wp[i]<=0) wp[i]=50 *soilDepth[i]; 
	//printf ("Layer %i wp %f \n", i, wp[i]);

	}
	return wp; 
}

float *FieldCapacity (float Sand[], float Clay[], float OrganicMatter[],float soilDepth[], int numSoilLayers){
	//should not be applied beyon organicMatter > 8% 
	// equations from Saxton and Rawls (2006); Soil water characteristics 
	// FC and PWP return fraction of volume !!!!!!!!!!!!
	int i;
		float *fc;
		fc = malloc(numSoilLayers*sizeof *fc);
		for (i=0;i<numSoilLayers;i++){
	float phi_33t;
 
	phi_33t= -0.251* Sand[i] +0.195* Clay[i] +0.011* OrganicMatter[i] +0.006 *(Sand[i] * OrganicMatter[i]) -0.027*(Clay[i] * OrganicMatter[i]) +0.452*Sand[i]*Clay[i]+0.299;
	fc[i] =(phi_33t + (1.283*phi_33t*phi_33t -0.374 * phi_33t-0.015))*soilDepth[i];
	if (fc[i]<=0) fc[i]=150 *soilDepth[i];	
		}
	return fc;
}
 
float UnsaturatedConductivity (float sand, float clay, float organicMatter,float soilMoisture, float maxSoilMoisture){
// Saxton and Rawls 2006
		float KSat;// KSat in mm/h
		float KS;// KSat in mm/h
				
float phi_sat;
float phi_33;
float phi_33t;
float phi_1500;
float phi_1500t;
float phi_s33;
float phi_s33t;
float lamda;
float B;

phi_33t= -0.251* sand +0.195* clay +0.011* organicMatter +0.006 *(sand * organicMatter) -0.027*(clay * organicMatter) +0.452*(sand*clay)+0.299;
phi_33 =phi_33t + (1.283*phi_33t*phi_33t -0.374 * phi_33t-0.015);

phi_s33t = 0.278 * sand + 0.034 * clay  + 0.022 * organicMatter -0.018 *(sand* organicMatter) -0.027 *(clay * organicMatter) -0.584* (sand * clay ) +0.078;
phi_s33= phi_s33t +0.6360 * phi_s33t -0.107;

 
phi_1500t= -0.024*sand+0.487 * clay+0.006*organicMatter +0.005*(sand*organicMatter)-0.013*(clay*organicMatter)+0.068*(sand * clay)+0.031;
phi_1500= phi_1500t+(0.14*phi_1500t -0.02);

B = (log(1500)-log(33))/(log(phi_33)-log(phi_1500));
lamda = 1/B;
phi_sat = phi_33 + phi_s33 -0.097 * sand+0.043;
KSat = (1930*pow((phi_sat - phi_33),(3-lamda)))*24 / _MDTimeSteps; // in mm/timeStep
float expo = 3 + (2 /lamda);
if (soilMoisture > maxSoilMoisture )soilMoisture =maxSoilMoisture;
KS= KSat*pow((soilMoisture/ maxSoilMoisture),expo);
//printf ("KS = %f KSat %f expo %f, soilMoist %f maxSoilMoist %f \n", KS, KSat, expo, soilMoisture, maxSoilMoisture);
return KS;
}

float SaturatedConductivity (float sand, float clay, float organicMatter,float soilMoisture, float maxSoilMoisture){
// Saxton and Rawls 2006
float KSat;// KSat in mm/h
float phi_sat;
float phi_33;
float phi_33t;
float phi_1500;
float phi_1500t;
float phi_s33;
float phi_s33t;
float lamda;
float B;

phi_33t= -0.251* sand +0.195* clay +0.011* organicMatter +0.006 *(sand * organicMatter) -0.027*(clay * organicMatter) +0.452*(sand*clay)+0.299;
phi_33 =phi_33t + (1.283*phi_33t*phi_33t -0.374 * phi_33t-0.015);

phi_s33t = 0.278 * sand + 0.034 * clay  + 0.022 * organicMatter -0.018 *(sand* organicMatter) -0.027 *(clay * organicMatter) -0.584* (sand * clay ) +0.078;


phi_s33= phi_s33t +0.6360 * phi_s33t -0.107;


phi_1500t= -0.024*sand+0.487 * clay+0.006*organicMatter +0.005*(sand*organicMatter)-0.013*(clay*organicMatter)+0.068*(sand * clay)+0.031;
phi_1500= phi_1500t+(0.14*phi_1500t -0.02);

B = (log(1500)-log(33))/(log(phi_33)-log(phi_1500));
lamda = 1/B;
phi_sat = phi_33 + phi_s33 -0.097 * sand+0.043;
KSat = (1930*pow((phi_sat - phi_33),(3-lamda)))*24 / _MDTimeSteps; // in mm/timeStep

if (soilMoisture > maxSoilMoisture )soilMoisture =maxSoilMoisture;

//printf ("KS = %f KSat %f expo %f, soilMoist %f maxSoilMoist %f \n", KS, KSat, expo, soilMoisture, maxSoilMoisture);
return KSat;
}
