/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDPermafrost.c

dominik.wisser@unh.edu

*******************************************************************************/

#include <stdlib.h>
#include <math.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDNumberOfSoilComputationNodes =114;
static int _MDNumOfSoilLayers =15; 
static int _MDInSoilAvailWaterCapID = MFUnset;
// Output
 
static int  _MDInAirTMeanID			= MFUnset;
static int _MDOutActiveLayerDepthID	= MFUnset;
static int _MDOutThawingDepthID = MFUnset;
static int _MDInSnowDensityID = MFUnset;
static int _MDInSnowDepthID = MFUnset;
static int _MDInIsInitialID= MFUnset;
static int _MDSnowPackChangeID = MFUnset; 
int _MDSoilMoistureOptionID = MFUnset; // 0 : bucket , 1: layered
static int _MDInRelSoilMoistureID=MFUnset;
static int *_MDOutSoilTemperatureIDs	     		   = (int *) NULL;
static int *_MDOutSoilIceIDs	     				   = (int *) NULL;
static int _MDInInitialSoilTempID  = MFUnset;
static int _MDInSoilOrganicLayerID =MFUnset;
 
static int *_MDInSoilpctSiltIDs		        = (int *) NULL;
static int *_MDInSoilpctClayIDs		        = (int *) NULL;
static int *_MDInSoilpctSandIDs		        = (int *) NULL;
static int _MDPermafrostDebug1 = MFUnset; 
static int _MDPermafrostDebug2 = MFUnset; 
static int _MDPermafrostDebug3 = MFUnset; 
static int _MDPermafrostDebug4 = MFUnset; 
static int _MDInSoilBulkDensityID= MFUnset;
static int *_MDInSoilDepthIDs				=(int *) NULL;
double activeLayerInterpol(double,double,double,double);
 
double soilThermalConductivity(double, double , float , float , float , float , float,float);

double heatCapacity (double depth,double temper, double, double, double, double, float, float);
double snowProperties( double, double);
double unfrozenWaterContent(double ,double );
int getSoilLayerNumber(double);
double initialSoilTemperature;
double *soilLayerThickness;//= {0.5, 0.5 ,9 ,20, 70};
double *A;//A[]={0.05,0.035,0.061,0.018, 0.064};
double *B;//[]={-0.1,-0.32,-0.35,-0.17,-0.34};
double *C;//[]={-0.1,-0.32,-0.35,-0.17,-0.34};
double *Cond_Th;//[]={0.34,1.2,1.23,2.12,2.16};
double *Cond_Fr;//[]={0.81,1.6,1.52,2.54,2.51};
double *SandFraction;
double *SiltFraction;
double *ClayFraction;
double organicContent; 
double *OrgContent;
double *BulkDensity;
double *Cvol;//[]={2.1e6,    2.4e6,    2.2e6,    1.8e6,    2.7e6};
double FIT=0.08;
double ALFA0 = 20.14;
double latentHeat =334e3;
double *freezingTemperature;//[]={0,0,0,0,0};
double *Wvol;
//double *Wunf;//={0.02, 0.02, 0.01,0.02,0.02};//TODO : del numbers, assign vector for numSoilLauers (5)
  
double waterTableDepth;
float debug1, debug2, debug3, debug4;
double densityWater=1000;

static void _MDPermafrostDepth (int itemID) {	
	int isInitial;
	//Initial Temperatures and Depth
	int numSoilLayers=_MDNumOfSoilLayers;
	int TStep =24;
	    // soil properties
	 double peatDensity =130;
		double activeLayerDepth;
		double thawingDepth=0;
		double snowDensity;
		double snowDepth;
	    double airTemp; 
	
	 
		int i;
		int iter;
		int iter0=21;
		if (_MDSoilMoistureOptionID==0){//layers: moisture over whole profile
			numSoilLayers=1;
		}
		 
			
			   for (i = 0;i< numSoilLayers ;i++){
				   Wvol  = (double *) realloc (Wvol,     (i + 1) * sizeof (double));
				  // Wunf  = (double *) realloc (Wunf,     (i + 1) * sizeof (double));
				   freezingTemperature  = (double *) realloc (freezingTemperature,     (i + 1) * sizeof (double));
				   Cond_Th  = (double *) realloc (Cond_Th,     (i + 1) * sizeof (double));
				   Cond_Fr  = (double *) realloc (Cond_Fr,     (i + 1) * sizeof (double));
				   soilLayerThickness  = (double *) realloc (soilLayerThickness,     (i + 1) * sizeof (double));
				   SandFraction  = (double *) realloc (SandFraction,     (i + 1) * sizeof (double));
				   SiltFraction  = (double *) realloc (SiltFraction,     (i + 1) * sizeof (double));
				   ClayFraction  = (double *) realloc (ClayFraction,     (i + 1) * sizeof (double));
				   OrgContent  = (double *) realloc (OrgContent,     (i + 1) * sizeof (double));
				   Cvol  = (double *) realloc (Cvol,     (i + 1) * sizeof (double));
				   BulkDensity  = (double *) realloc (BulkDensity,     (i + 1) * sizeof (double));
				   A  = (double *) realloc (A,     (i + 1) * sizeof (double));
				   B  = (double *) realloc (B,     (i + 1) * sizeof (double));
				   C  = (double *) realloc (B,     (i + 1) * sizeof (double));
				   				   
				  
			   }
			   
			   
			 
			   
			   
			   if (_MDSoilMoistureOptionID==1){//layers: moisture over whole profile
				   // Assumptions for mixing the organic layer in the soil column: 
				   // organic matter = 2 * carbon content
				   // 0.5 of organic matter is in first layer. Depth of first layer = organicContent / DensityOfPeat
				   
				 //  printf ("Model runs in layer mode\n");
				   organicContent = MFVarGetFloat(_MDInSoilOrganicLayerID, itemID,0.0); // in kg/m3
				   OrgContent[0]=2 * (organicContent * 0.5); 
				  
				   A[0]=0.05;B[0]=-0.1;C[0]=-0.1;
				   soilLayerThickness[0]=OrgContent [0]/peatDensity;
				   OrgContent [0]=0.94;//94 Prozent peat
				   BulkDensity[0]=peatDensity; //1st layer is always peat..almost only peat. 
				   SandFraction[0]=0.02;
				   ClayFraction[0]=0.02;
				   SiltFraction[0]=0.02;
				   Wvol[0]=0.65;							  							 
				   Cvol[0]= 2.14e6 * SandFraction[0] +1.8e6 * SiltFraction[0]+ 2.31e6 * ClayFraction[0] + 2.5e6 *OrgContent[0];
				   for (i = 1;i< numSoilLayers ;i++){
					  
					   //Mix Organic layer in the soil column. 	   
					   Cvol[i]= 2.14e6 * SandFraction[i] +1.8e6 * SiltFraction[i]+ 2.31e6 * ClayFraction[i] + 2.5e6 *OrgContent[i];
							 //  Wvol[i]= MFVarGetFloat(_MDInRelSoilMoistureID,itemID,0.0);
							   soilLayerThickness[i] = MFVarGetFloat( _MDInSoilDepthIDs[i], itemID,0.0);
							   soilLayerThickness[numSoilLayers-1]=100;
							   freezingTemperature[i] =0;
							   BulkDensity[i]= MFVarGetFloat( _MDInSoilBulkDensityID,itemID,0.0);
							   SandFraction[i]= MFVarGetFloat( _MDInSoilpctSandIDs[i], itemID,0.0); 
							   SiltFraction[i]= MFVarGetFloat( _MDInSoilpctSiltIDs[i], itemID,0.0); 
							   ClayFraction[i]= MFVarGetFloat( _MDInSoilpctClayIDs[i], itemID,0.0); 
							   A[i]=0.035;
							   B[i]=-0.32;
							   C[i]=-0.32;
							  OrgContent[i] = (exp (-soilLayerThickness[i]*3) * 0.5 * organicContent) /(MDMaximum (BulkDensity[i], 0.1)) ;
							 float sum = SandFraction[i] +  SiltFraction[i]+ClayFraction[i];
							 if (sum < 0.1){
								 SandFraction[i] = 0.33; 
								 SiltFraction[i]= 0.33; 
								 ClayFraction[i]= 0.33; 
								 
								 
							 }
				//			  printf ("Layer =%i OrgFraction = %f BulkDensity[i] %f \n", i,OrgContent[i]*100, BulkDensity[i]);
							  
							   Wvol[i]=0.65;
					     }
				   //set last soil layer to some values so that temperature can be computed up to 100m 
				   SandFraction[numSoilLayers-1]= 0.33; 
				   SiltFraction[numSoilLayers-1]= 0.33; 
				   ClayFraction[numSoilLayers-1]= 0.33; 
			   }
			   if (_MDSoilMoistureOptionID==0){ // Model is run in bucket mode. const soil moisture and properties for entire soil column
				   _MDNumOfSoilLayers=1;
				   organicContent = MFVarGetFloat(_MDInSoilOrganicLayerID, itemID,0.0); 
				   freezingTemperature[0] =0;
				   Cvol[0]= 2.14e6;
				   SandFraction[0]=0.33;
				   ClayFraction[0]=0.33;
				   SiltFraction[0]=0.33;
				   OrgContent[0] =organicContent;
				   soilLayerThickness[0]=100;
				   Wvol[i]=0.65;
			   }
			   
			   for (i = 1;i< numSoilLayers ;i++){
			   soilLayerThickness[i]=soilLayerThickness[i]+soilLayerThickness[i-1];
			   }
		
	double E0;
	
	
	float S;
	double L2;
	double A1, B1;

	double W1, W2;
	double ALFA;
	double H0,H1,H2;
	double C0,C1;
	double G1,G2,G3,G4;
	double L0,L1;

 
	double prevSoilTemp[_MDNumberOfSoilComputationNodes];
	double soilTemp[_MDNumberOfSoilComputationNodes];
	
	double U1[_MDNumberOfSoilComputationNodes];
	double Q1[_MDNumberOfSoilComputationNodes];
	double P1[_MDNumberOfSoilComputationNodes];
	 
	double Ice[_MDNumOfSoilLayers];	
	


	
	isInitial = MFVarGetFloat(_MDInIsInitialID,itemID,0.0);
//	printf ("Isinitial = %i \n", isInitial);
 	double compNodeDepth[] ={0.00,0.05,0.10,0.15,0.20,	0.25,	0.30,	0.35,	0.40,	0.45,	0.50,	0.55,	0.60,	0.65,	0.70,	0.75,	0.80,	0.85,	0.90,	0.95,	1.00,	1.10,	1.20,	1.30,	1.40,	1.50,	1.60,	1.70,	1.80,	1.90,	2.00,	2.10,	2.20,	2.30,	2.40,	2.50,	2.60,	2.70,	2.80,	2.90,	3.00,	3.20,	3.40,	3.60,	3.80,	4.00,	4.20,	4.40,	4.60,	4.80,	5.00,	5.20,	5.40,	5.60,	5.80,	6.00,	6.50,	7.00,	7.50,	8.00,	8.50,	9.00,	9.50,	10.00,	10.50,	11.00,	11.50,	12.00,	12.50,	13.00,	13.50,	14.00,	14.50,	15.00,	15.50,	16.00,	16.50,	17.00,	17.50,	18.00,	18.50,	19.00,	19.50,	20.00,	21.00,	22.00,	23.00,	24.00,	25.00,	26.00,	27.00,	28.00,	29.00,	30.00,	32.00,	34.00,	36.00,	38.00,	40.00,	42.00,	44.00,	46.00,	48.00,	50.00,	55.00,	60.00,	65.00,	70.00,	75.00,	80.00,	85.00,	90.00,	95.00,	100.00};
 	//set activeLayerDepth to maximum soil depth
 	activeLayerDepth=compNodeDepth[_MDNumberOfSoilComputationNodes];
 	initialSoilTemperature = MFVarGetFloat(_MDInInitialSoilTempID,itemID,0.0);
	 
	 
	airTemp        	= 	(double)	MFVarGetFloat 	(_MDInAirTMeanID,				itemID, 1.0);
	snowDepth	    =   (double)	MFVarGetFloat 	(_MDInSnowDepthID,				itemID, 0.0)/1000;// in m!
	snowDensity	    =   (double)	MFVarGetFloat 	(_MDInSnowDensityID,			itemID, 0.0);

	E0= 0.0000014;
	//double *T_init;
	//double *D_init;
	double G0;
	G0=0.015;

	if (isInitial < 1.0){
	
	prevSoilTemp[0]=airTemp;     
	//!  interpolation of the initial conditions    
	    for (i = 1; i< _MDNumberOfSoilComputationNodes; i++){//do i=1,Dn
		prevSoilTemp[i]=initialSoilTemperature + G0 *compNodeDepth[i]; 	

	    }//enddo   
//	    printf ("Done setting initial temperatures  data for item %i\n", itemID);
	}else{ 
		  for (i = 0; i< _MDNumberOfSoilComputationNodes; i++){//do i=1,Dn
			  prevSoilTemp[i]=MFVarGetFloat(_MDOutSoilTemperatureIDs[i],itemID,99);
			 	U1[i]=prevSoilTemp[i];
		  }
	}
	  
	//printf ("Stariting Permafrost comput for itemID %i\n", itemID);
	


	
	//	printf ("Jetzt gehts es los Permafrost comput for itemID %i\n", itemID);

 	//    !--------------   
	     S=TStep*60*60;//   ! 24 hours time step in seconds
	     double maxABS = 1.41e-6;
	            iter = 0;
	        
	            while (iter < iter0 && maxABS > E0){
	              
	//            ! computation of boundary coefficients  G1,G2
	        //    	printf ("ItemID: %i computation of boundary coefficients\n",itemID); 
	            	int layerIndex = getSoilLayerNumber(compNodeDepth[0]);
	         
	            	
	            	L0=soilThermalConductivity(U1[0], Wvol[layerIndex], SandFraction[layerIndex], SiltFraction[layerIndex],ClayFraction[layerIndex], OrgContent[layerIndex], freezingTemperature[layerIndex],BulkDensity[layerIndex]) ;
	              debug1=L0;
	              	
	              	
	            	layerIndex = getSoilLayerNumber(compNodeDepth[1]);
	            	//  printf ("Depth = %f LayerIndex = %i\n",compNodeDepth[1], layerIndex);
	            		                
	            	L1=soilThermalConductivity(U1[1], Wvol[layerIndex], SandFraction[layerIndex], SiltFraction[layerIndex],ClayFraction[layerIndex], OrgContent[layerIndex], freezingTemperature[layerIndex],BulkDensity[layerIndex]) ;
	                        
	                	                
	            //    printf ("item %i L0 = %f L1 = %f \n", itemID,L0, L1);
	                H0=compNodeDepth[1]-compNodeDepth[0];
	                if (snowDepth < E0 || prevSoilTemp[0] > E0){
	                    G1=0.0;
	                    G2=airTemp;
	                  
	                }else if (prevSoilTemp[0] <= 0.0 &&  snowDepth < E0) {
	                         G1=0.0;
	                         G2=airTemp;
	       //                  printf ("item %i L0 = %f L1 = %f \n", itemID,L0, L1);
	                }
	           
	                else{ //! if (snowDepth(j) > 0.) then
	                        ALFA =     snowProperties(snowDensity, snowDepth);
	      //                 printf ("item %i : snow is here \n");
	                      ALFA=(double) 1/ALFA;
	                      C1=heatCapacity(compNodeDepth[0],prevSoilTemp[0], SandFraction[layerIndex], SiltFraction[layerIndex],ClayFraction[layerIndex], OrgContent[layerIndex], BulkDensity[layerIndex], Wvol[layerIndex]);   
	                  	debug2 =C1;	       
	                      W1=(double) 0.5* (L0+L1);
	                      W2=H0*ALFA/W1;
	                      W1=(double) 0.5*pow(H0,2)*C1/W1/S;
	                      G1=(double)1.0 +W1+W2;
	                      G2=(W2*airTemp+W1*prevSoilTemp[0])/G1;
	                      G1=(double) 1/G1;
	              //      printf ("ITEMID %i C1 = %lf G1 = %f G2 = %f \n", itemID,C1, G1, G2);
	                }//endif
	         //       printf ("Permutation coeff, item %i \n",itemID ); 
	//                !----- Permutation and forward elimination
	                P1[1]=G1;
	                Q1[1]=G2;
	          
	                 for (i=1;i<_MDNumberOfSoilComputationNodes-1;i++){//do i=2,N-1
	       //         	 printf ("ITEMID %i N = = %i\n",itemID,i);
	                	//C1 = 2.321e6;
	                	int layerIndex = getSoilLayerNumber(compNodeDepth[i]);
	            	 //   double heatCapacity (double depth,double temper, double sand, double silt, double clay, double organic){
	                    C1=heatCapacity(compNodeDepth[i],prevSoilTemp[i], SandFraction[layerIndex], SiltFraction[layerIndex],ClayFraction[layerIndex], OrgContent[layerIndex], BulkDensity[layerIndex], Wvol[layerIndex]);   
	                    L2=soilThermalConductivity(prevSoilTemp[i+1],Wvol [layerIndex], SandFraction[layerIndex], SiltFraction[layerIndex],ClayFraction[layerIndex], OrgContent[layerIndex],freezingTemperature[layerIndex],BulkDensity[layerIndex]) ;
	                //	printf ("ItemID %i Layer %i Index %i C1 %e L2 %e  OrgContent[layerIndex] %f \n", itemID, i, layerIndex, C1, L2, OrgContent[layerIndex]);            
	              	     if (i==2)debug3 = C1;
	              	     if (i==2) debug4 = L2;
	                  
	                //  printf ("ITemID %i C1 %f  L2  %f layer %i\n",itemID, C1, L2,i);
	                  	                  
	                  H1=compNodeDepth[i+1]-compNodeDepth[i];
	                
	                   H2=(double) 0.5*(H0+H1);
	        //           printf ("ITemID %i H1 %f  H2  %f \n",itemID, H1, H2);
	                   A1=(double) 0.5 * (L0+L1)*S/C1/(H0*H2);
	                   B1=(double) 0.5 *(L1+L2)*S/C1/(H1*H2);
	                   C0=(double) 1.0+A1+B1;
	                   P1[i+1]=B1/(C0-A1*P1[i]);
	                   Q1[i+1]=(A1*Q1[i]+prevSoilTemp[i])*P1[i+1]/B1;
	                   H0=H1 ;
	                   L0=L1 ;
	                   L1=L2;
	     //          	printf ("C1 = %f G3 = %f G4 = %f\n",C1,G3,G4);
	               
	                }
	    //             printf ("EndofLoope C1 = %f G3 = %f G4 = %f\n",C1,G3,G4);
	//                ! computation of the Lower boundary koef. G3 & G4
	              //  C1 = 2.3;
	                 layerIndex = getSoilLayerNumber(compNodeDepth[_MDNumberOfSoilComputationNodes-1]);
	                 C1=heatCapacity(compNodeDepth[_MDNumberOfSoilComputationNodes-1],prevSoilTemp[_MDNumberOfSoilComputationNodes-1], SandFraction[layerIndex], SiltFraction[layerIndex],ClayFraction[layerIndex], OrgContent[layerIndex], BulkDensity[layerIndex], Wvol[layerIndex]);
	               
	                 if (isnan(C1))printf ("C1 is nan! \n" );
	                G3=(double) 0.5*pow(H1,2)*C1/L2/S ;
	                G4=H1*G0+G3*prevSoilTemp[_MDNumberOfSoilComputationNodes-1];
	          //     printf ("itemID = %i C1 = %f G3 = %f G4 = %f L2 = %f C0 %f B1 %f A1 %f H2 %f H1 %f \n", itemID,C1,G3,G4, L2, C0, B1, A1, H2, H1);
	                G3=(double) 1.0/(1.0+G3);
	                G4=G4*G3;
	//                    ! Temperature computation in the last (deepest) grid node
	                    W1=(G3*Q1[_MDNumberOfSoilComputationNodes-1]+G4)/(1.0-G3*P1[_MDNumberOfSoilComputationNodes-1]);
	                  
	                    maxABS=fabs(W1-U1[_MDNumberOfSoilComputationNodes-1]);
	                    U1[_MDNumberOfSoilComputationNodes-1]=W1;
	               
	//                !---- Back substitution
	                i=_MDNumberOfSoilComputationNodes-2;
	                while (i>=0){//DO WHILE (I>=1)
	                    W1=P1[i+1]*U1[i+1]+Q1[i+1];
	//                    ! check for the iterative convergence
	                    if (fabs(W1-U1[i])>maxABS) maxABS=fabs(W1-U1[i]);
	                        U1[i]=W1;
	                        i=i-1;
	            //          printf ("i = %i U1 = %f\n",i,U1[i]);
	                }//ENDDO !WHILE
	                iter=iter+1;
	            }//enddo ! while ((ITER < ITER0).AND.(maxABS > E0))
	           //printf ("Num Iterations %i \n", iter);
	            activeLayerDepth=0;
	            for (i=0;i<_MDNumberOfSoilComputationNodes;i++){
	 //               printf ("Layer = %i Temp = %f MFModelGetDayOfYear()%i\n",i, U1[i], MFDateGetDayOfYear());
	              soilTemp[i]=U1[i];
	            
	               	              
	               //if (i ==0  && soilTemp[0] > 0){ 
	            	//   activeLayerDepth = soilLayerDepth[0];
	              // }
	               if (i==0  && soilTemp[0] > 0){ activeLayerDepth =compNodeDepth[_MDNumberOfSoilComputationNodes-1];};
	               
	               if (i > 0 && soilTemp[i] < 0 && soilTemp[i-1]>0){
		   //               if (itemID ==1126)printf ("MFDateGetDayOfYear() %i ITEM %i Layer %i D1 %f D2 %f T1 %f  T2 %f AL %f \n",MFDateGetDayOfYear(),itemID,i,compNodeDepth[i-1], compNodeDepth[i],soilTemp[i-1],soilTemp[i],activeLayerDepth);
	   
	                   activeLayerDepth = activeLayerInterpol(compNodeDepth[i-1], compNodeDepth[i],soilTemp[i-1],soilTemp[i]);
	               }
	                  if (i > 0 && soilTemp[i] > 0 && soilTemp[i-1] < 0){
	                   thawingDepth = activeLayerInterpol(compNodeDepth[i-1], compNodeDepth[i],soilTemp[i-1],soilTemp[i]);
	               }
	                  Ice[i]=0;
	                 // if (soilTemp[i] < 0+FIT) {Ice[layerIndex]=Wvol[layerIndex]-Wunf[layerIndex];}
	                

	               }
	            
	//    printf ("ActiveL for itemID %i in perma = %f \n", itemID,activeLayerDepth);

	    for (i=0;i< _MDNumberOfSoilComputationNodes;i++){
	    	 
	    	
			MFVarSetFloat (_MDOutSoilTemperatureIDs[i], itemID, soilTemp[i]);
			MFVarSetFloat (_MDOutSoilIceIDs [i], itemID, Ice[i]);
	    	 
	    }
	    
	 
		MFVarSetFloat (_MDInIsInitialID,    			 itemID,  1.0); 
		MFVarSetFloat (_MDOutActiveLayerDepthID,  		 itemID,  activeLayerDepth); 
		MFVarSetFloat (_MDOutThawingDepthID,     		 itemID,  L1); 
		MFVarSetFloat (_MDPermafrostDebug1,     		 itemID,  debug1); 
		MFVarSetFloat (_MDPermafrostDebug2,     		 itemID,  debug2); 
		MFVarSetFloat (_MDPermafrostDebug3,     		 itemID,  debug3); 
		MFVarSetFloat (_MDPermafrostDebug4,     		 itemID,  debug4); 
		
		
	//	printf ("done with itemID%i \n",itemID);    
	
	}

	    int getSoilLayerNumber(double depth){
	    // Number of the Soil Layer Function
	    	if( _MDSoilMoistureOptionID==0)return 0;
	     int Numl=0;
	        int i;
	          i=0;
	          double junk;
	          while (depth > soilLayerThickness[i]+1e-3){
	        	  junk = soilLayerThickness[i];
	        	  i=i+1;
	          }
	          Numl=i;
	    
	        return MDMinimum(_MDNumOfSoilLayers-1, Numl);
	         }
	    

	    
	    
	    double soilThermalConductivity(double temper, double soilMoisture, float fracSand, float fracSilt, float fracClay, float fracOrganic, float freezingTemperature, float bulkdensity){
	//    !/* Soil Thermal Conductivity
	    //	printf ("temp %f soilMoist %f Orga %f \n", temper, soilMoisture, fracOrganic);
	    float CondPeat;	
	   if (temper < freezingTemperature-FIT){
	   	    	CondPeat = 0.0055 * soilMoisture*100 + 0.0141;
	   	    }
	       else{ 
	       	CondPeat = 0.005 * soilMoisture*100 +0.04;
	       }	
	   
	    if  	(fracOrganic > 0.8) return CondPeat;
	    
	  
	    //The following is from Hendrickx et. al, Global Prediction of thermal soil regimes, Proceding of SPIE, Vol 6953 2008
	    float psi_sand; float psi_silt; float psi_clay; float psi_org;
	    float rho_sand; float rho_silt; float rho_clay;
	    float rho_q; float rho_s; float rho_m;
	    float sumAllFractions = fracSand + fracSilt + fracClay + fracOrganic;
	    float A; float B; float C; float D;
	    
	    psi_sand = fracSand /  sumAllFractions;
	    psi_silt = fracSilt /  sumAllFractions;
	    psi_clay = fracClay /  sumAllFractions;
	    psi_org =  fracOrganic /  sumAllFractions;
	    
	    rho_sand = bulkdensity * psi_sand / 2650;
	    rho_silt = bulkdensity * psi_silt / 2650;
	    rho_clay = bulkdensity * psi_clay / 2650;
	    rho_sand = bulkdensity * psi_org / 1500;
	    	    
	    rho_q = rho_sand; rho_m = rho_clay + rho_silt;rho_s = 	psi_sand+ psi_silt+psi_clay+psi_org;
	    A=(0.57 + 1.73 * rho_q + 0.93 * rho_m)/(1-0.74 * rho_q -0.49 * rho_m)-(2.8 * rho_sand)*(1-rho_sand);
	    B = 2.8* rho_sand;
	    C = 1 + (2.6/sqrt(psi_clay));
	    D = 0.03 + 0.7 * pow(rho_sand,2); 
	    float cond = A + B * soilMoisture  - (A -D ) * exp(-pow(C * soilMoisture*100,4));
	//    printf ("Cond = %f rs %f rsi %f rcl %f org %f  sum %f Freezing %f fracOrgn %f\n",cond, rho_sand, rho_silt, rho_clay, rho_org, sumAllFractions, freezingTemperature, fracOrganic);
	    return cond;
	    
 
	   
	    
//	    float CondMineral;
//	    float CondMineral_Thawed =cond;
//	    float CondMineral_Frozen= 2.4;
//	    float sumFractions = fracSand +fracSilt + fracClay +fracOrganic;
//	    float fracMineral = sumFractions- fracOrganic;
//	    
//	            if (temper < freezingTemperature-FIT){
//	            	CondMineral= CondMineral_Thawed;
//	            }
//	            else if (temper > FIT+freezingTemperature){
//	            	CondMineral= CondMineral_Frozen;
//	            }else{
//	            	CondMineral = (double) 0.5 *(CondMineral_Frozen+CondMineral_Thawed);
//	            }
//	           // printf ("Cond = %f\n",(CondPeat * fracOrganic /sumFractions + CondMineral * fracMineral / sumFractions));
//	            return (CondPeat * fracOrganic /sumFractions + CondMineral * fracMineral / sumFractions);
	    }
//	    
	    
	double snowProperties (double snowDensity, double snowDepth){
	//    use GLOBVAR
	    double ALFA;
	        ALFA=1.0/ALFA0 +1./((0.018+0.00087*snowDensity)/snowDepth);
	    return ALFA;
	}
	    double heatCapacity (double depth,double temper, double fracSand, double fracSilt, double fracClay, double fracOrganic, float bulkdensity,float soilmoisture){
	 
	    	float cs = 0.73e3;
	    	float co = 1.9e3;
	    	float psi_sand; float psi_silt; float psi_clay; float psi_org;
	    	float psi_m;
	    	float sumAllFractions;
	    	double unfrWater;
	        sumAllFractions = fracSand +fracSilt + fracClay +fracOrganic;
	    	 psi_sand = fracSand /  sumAllFractions;
	    	 psi_silt = fracSilt /  sumAllFractions;
	    	 psi_clay = fracClay /  sumAllFractions;
	    	psi_org =  fracOrganic /  sumAllFractions;
	    	
	    	psi_m = psi_sand+psi_silt+psi_clay;
	    	float Cvol;
	    	Cvol =  cs * psi_m * 2650 +  co * psi_org * 130;// + cw * soilmoisture *1000;
	    	
	    	// Volumetric Heat Capacity i
	    double CAP;

	            int i=getSoilLayerNumber(depth);
	    
	            if (temper <freezingTemperature[i]-FIT){
	            	unfrWater=unfrozenWaterContent(temper, depth);
	                CAP=Cvol+5e2*(Wvol[i]-unfrWater)+1e3*unfrWater;
	              //  if (CAP >= 1.0e7) printf ("11Col = %e org %f CAP %e psi_m %f \n", Cvol, fracOrganic, CAP, psi_m, psi_org, soilmoisture);
	              	    
	            }
	            else if (temper > freezingTemperature[i]+FIT){
	                CAP=Cvol+1e3*Wvol[i];
	            //    if (CAP >= 1.0e7) printf ("C222ol = %e org %f CAP %e psi_m %f \n", Cvol, fracOrganic, CAP, psi_m, psi_org, soilmoisture);
	              	    
	            }
	            else{
	            	unfrWater=unfrozenWaterContent(temper,depth);
	                CAP=0.5*( (Cvol+1e3*Wvol[i] )+( Cvol+5e2* ( Wvol[i]-unfrWater )+1e3*unfrWater ) )+0.5*8e4* (Wvol[i]-unfrWater )/FIT;
	                //CAP=0.5*( (Cvol+1e3*Wvol[i] )+( Cvol+5e2* ( Wvol[i]-unfrWater )+1e3*unfrWater ) )+0.5*latentHeat* (Wvol[i]-unfrWater )/FIT;
	               if (CAP >= 1.0e7) printf ("Temp %f 333Col = %e org %f CAP %e psi_m %f SM %f\n", temper ,Cvol, fracOrganic, CAP, psi_m, soilmoisture);
	              	    
	            }
	     if (CAP >= 1.0e7) printf ("Col = %e org %f CAP %e psi_m %f psi_org %f soilmoisture %f \n", Cvol, fracOrganic, CAP, psi_m, psi_org, soilmoisture);
	       
	            return CAP;
	       }
	    



	double activeLayerInterpol(double upperDepth, double lowerDepth, double upperTemp, double lowerTemp){
	double activeLayerDepth;

	activeLayerDepth = (lowerDepth - upperDepth )/ (upperTemp - lowerTemp)* (0 - lowerTemp);
	if (isnan(activeLayerDepth)) printf ("ActiveLayerThickness = %f uppDepth = %f lowerDepth =%f upperTemp %f, lowerTemp %f\n", activeLayerDepth, upperDepth, lowerDepth, upperTemp, lowerTemp);
	//printf ("ActiveLayerThickness = %f\n", activeLayerDepth+ upperDepth);
		
	return activeLayerDepth + upperDepth;
	}

	double unfrozenWaterContent(double temper,double depth){
	//! uw=aclv*(cclv-|t|)**bclv    function of unfrozen water content
	int i=getSoilLayerNumber(depth);
	double ac = A[i];
	double bc=B[i];
	double cc=C[i];
	float unfrWater;
	//real*8 temper,ac,bc,cc
	//cc = 0.0;
	
	unfrWater=ac * pow(fabs(cc-temper),bc);
	if (isinf(unfrWater)) printf ("unfrWater Inf %f ac %f bc %f cc %f Temp %lf depth %lf\n", unfrWater,ac, bc, cc, temper, depth);
	//return 0.02;
	return unfrWater;
	 }



int MDPermafrostDef () {

	
	char soilTemperature[30];
	char iceContent[20];	 
	
	char soilSiltFractionName[25];
	char soilClayFractionName[25];
	char soilSandFractionName[25];
	 
		
 
	char soilDepthName[20];
	
	
	const char *optStr;
    int i;
    
    if (_MDOutActiveLayerDepthID != MFUnset) return (_MDOutActiveLayerDepthID);
	MFDefEntering ("Permafrost");
    
	for (i = 0; i < _MDNumberOfSoilComputationNodes; i++) {
			_MDOutSoilTemperatureIDs =         (int *) realloc (_MDOutSoilTemperatureIDs,         (i + 1) * sizeof (int));
			_MDOutSoilIceIDs	 	 =         (int *) realloc (_MDOutSoilIceIDs,    		      (i + 1) * sizeof (int));
	//}
	//for (i = 0; i < _MDNumberOfSoilLayers; i++) {
			sprintf (soilTemperature,      "SoilLayerTemperature_%02d", i + 1); //  	
			sprintf (iceContent,           "SoilIceContent_%02d", i + 1);
			
			if ((_MDOutSoilTemperatureIDs [i] = MFVarGetID (soilTemperature, "mm", MFDouble,  MFState, MFInitial)) == CMfailed) {
								//				printf("CMfailed in MDInSoilInputDataID \n");
												CMmsgPrint (CMmsgUsrError, "CMfailed in MDOutSoilTemp \n");
												return CMfailed;
											}
			if ((_MDOutSoilIceIDs [i]        = MFVarGetID (iceContent, "mm", MFDouble,  MFState, MFInitial)) == CMfailed) {
					//				printf("CMfailed in MDInSoilInputDataID \n");
									CMmsgPrint (CMmsgUsrError, "CMfailed in MDOutSoilTemp \n");
									return CMfailed;
								}
					
			
	//			 printf ("i = %i soilTempID = %i \n", i,_MDOutSoilTemperatureIDs [i]);
	}
	
	
	
	for (i = 0; i < _MDNumOfSoilLayers; i++) {
				_MDInSoilDepthIDs = (int *) realloc (_MDInSoilDepthIDs, (i + 1) * sizeof (int));
 
				_MDInSoilpctClayIDs  = (int *) realloc (_MDInSoilpctClayIDs,     (i + 1) * sizeof (int));
				_MDInSoilpctSandIDs  = (int *) realloc (_MDInSoilpctSandIDs,     (i + 1) * sizeof (int));
				_MDInSoilpctSiltIDs  = (int *) realloc (_MDInSoilpctSiltIDs,     (i + 1) * sizeof (int));
			 
								
			//	printf ("Reading Layer  = %i of %i\n",i, _MDNumberOfSoilMoistureLayers);
				
				sprintf (soilDepthName, "SoilDepth_%02d", i + 1); //  
 				sprintf (soilSiltFractionName, "SoilSiltPercentage_%02d", i + 1); //  	
				sprintf (soilSandFractionName, "SoilSandPercentage_%02d", i + 1); //
				sprintf (soilClayFractionName, "SoilClayPercentage_%02d", i + 1); //
	 
	
				
				if ((_MDInSoilDepthIDs [i] = MFVarGetID (soilDepthName, "mm", MFInput,  MFState, MFBoundary)) == CMfailed) {
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
				
//				if ((_MDInSoilpctOrganicIDs [i] = MFVarGetID (soilOrganicPercentageName, "mm", MFInput,  MFState, MFBoundary)) == CMfailed) {
//														printf("CMfailed in MDInSoilInputDataID \n");
//														CMmsgPrint (CMmsgUsrError, "CMfailed in MDInSoilDepth \n");
//														return CMfailed;
//													}
				
				if ((_MDInSoilpctClayIDs [i] = MFVarGetID (soilClayFractionName, "mm", MFInput,  MFState, MFBoundary)) == CMfailed) {
														printf("CMfailed in MDInSoilInputDataID \n");
														CMmsgPrint (CMmsgUsrError, "CMfailed in MDInSoilDepth \n");
														return CMfailed;
													}
				
				
			
//				if ((_MDInRelSoilMoistureID [i] = MFVarGetID (soilClayFractionName, "mm", MFInput,  MFState, MFBoundary)) == CMfailed) {
//																printf("CMfailed in MDInSoilInputDataID \n");
//																CMmsgPrint (CMmsgUsrError, "CMfailed in MDInSoilDepth \n");
//																return CMfailed;
//															}
//			
				
				
				 
			}
	
	
	
	
	
	
	
	const char *soilMoistureOptions [] = { "bucket", "layers", (char *) NULL };
	int soilMoistureOptionID;
	if (((optStr = MFOptionGet (MDOptSoilMoisture))  == (char *) NULL) || ((soilMoistureOptionID = CMoptLookup (soilMoistureOptions, optStr, true)) == CMfailed)) {
					CMmsgPrint(CMmsgUsrError," Soil Moisture mode not specifed! Options = 'bucket' or 'layers'\n");
					return CMfailed;
	}
	_MDSoilMoistureOptionID = soilMoistureOptionID;

	
	if ((_MDInSoilAvailWaterCapID = MDSoilAvailWaterCapDef ()) == CMfailed) return CMfailed;
	
	if (_MDSoilMoistureOptionID == 0){ //bucket
		
	if ((_MDInRelSoilMoistureID = MFVarGetID (MDVarLiquidSoilMoisture,             "mm", MFOutput,  MFState, MFInitial))==CMfailed) return CMfailed;
	}
	
	if	(((_MDSnowPackChangeID =  MDSPackChgDef()) == CMfailed)||
	    ((_MDInAirTMeanID          = MFVarGetID (MDVarAirTemperature,             "degC", MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutActiveLayerDepthID          = MFVarGetID (MDVarActiveLayerDepth,             "m", MFOutput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDOutThawingDepthID          = MFVarGetID (MDVarThawingDepth,             "m", MFDouble,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInIsInitialID          = MFVarGetID (MDVarIsInitial,             "m", MFOutput,  MFState, MFInitial)) == CMfailed) ||
	    ((_MDInSoilOrganicLayerID = MFVarGetID (MDVarSoilOrganicLayer,       "kg/m3",   MFInput, MFState,  MFBoundary)) == CMfailed) ||
	    ((_MDInSoilBulkDensityID = MFVarGetID (MDVarSoilBulkDensity,       "kg/m3",   MFInput, MFState,  MFBoundary)) == CMfailed) ||
	   	
	    
	    
	    ((_MDPermafrostDebug1 = MFVarGetID ("PermafrostDebug1",       "kg/m3",   MFOutput, MFState,  MFBoundary)) == CMfailed) ||
	    ((_MDPermafrostDebug2 = MFVarGetID ("PermafrostDebug2",       "kg/m3",   MFOutput, MFState,  MFBoundary)) == CMfailed) ||
	    ((_MDPermafrostDebug3 = MFVarGetID ("PermafrostDebug3",       "kg/m3",   MFOutput, MFState,  MFBoundary)) == CMfailed) ||
	    ((_MDPermafrostDebug4 = MFVarGetID ("PermafrostDebug4",       "kg/m3",   MFOutput, MFState,  MFBoundary)) == CMfailed) ||

	    ((_MDInSnowDensityID = MFVarGetID (MDVarSnowDensity,       "mm",   MFInput, MFState,  MFBoundary)) == CMfailed) ||
		((_MDInSnowDepthID = MFVarGetID (MDVarSnowDepth,       "mm",   MFInput, MFState,  MFBoundary)) == CMfailed) ||
	    ((_MDInInitialSoilTempID  = MFVarGetID (MDVarInitalSoilTemperature, "deg C",   MFInput,  MFState, MFBoundary)) == CMfailed) ||
 //      ((_MDOutRelSoilMoistID     = MFVarGetID (MDVarRelSoilMoisture,     "mm",   MFOutput, MFState, MFBoundary)) == CMfailed) ||
       (MFModelAddFunction (_MDPermafrostDepth) == CMfailed)) return (CMfailed);
	
 
	MFDefLeaving ("Permafrost");
	return (_MDOutActiveLayerDepthID);
}
