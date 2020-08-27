/******************************************************************************

 GHAAS Water Balance/Transport Model V3.0
 Global Hydrologic Archive and Analysis System
 Copyright 1994-2020, UNH - ASRC/CUNY

 MD.h

 bfekete@gc.cuny.edu

 *******************************************************************************/

#ifndef MD_H_INCLUDED
#define MD_H_INCLUDED

#if defined(__cplusplus)
extern "C" {
#endif

#define MDNoneStr      "none"
#define MDInputStr     "input"
#define MDLookupStr    "lookup"
#define MDCalculateStr "calculate"

// Options
#define MDOptDischarge                       "Discharge"
#define MDOptIrrigation                      "Irrigation"
#define MDOptModel                           "Model"
#define MDOptReservoirs                      "Reservoirs"
#define MDOptRiverbed                        "Riverbed"
#define MDOptRouting                         "Routing"
#define MDOptMuskingum                       "Muskingum"
#define MDOptIrrReferenceET                  "IrrReferenceETP"
#define MDOptIrrIntensity                    "DoubleCropping"
#define MDOptSoilMoisture					 "SoilMoisture"
#define MDOptSoilTemperature				 "SoilTemperature"

#define MDOptIrrigatedAreaMap                "IrrigatedAreaMap"
#define MDOptSoilAvailableWaterCapacity		 "SoilWaterCapacity"
#define MDOptThermalInputs3					 "ThermalInputs3" //change to thermalinputs3 - AM 042415

#define MDOptWetBulbTemp             "WetBulbTemp"
#define MDOptSpecificHumidity             "SpecificHumidity"

//WETBULB
#define MDVarSpecificHumidity        "SpecificHumidity" //for wet bulb
#define MDVarAirPressure             "AirPressure"      //for wet bulb
#define MDVarWetBulbTemp             "WetBulbTemp"      //for wet bulb
#define MDVarRelativeHumidity        "RelativeHumidity" //for wet bulb
#define MDOptRelativeHumidity        "RelativeHumidity" //for wet bulb

//SOLAR TO CLOUD
#define MDOptCloudCover		     "CloudCover"	//cloud cover fraction AM sep2015
#define MDVarCloudCover		     "CloudCover"	//cloud cover fraction AM sep2015

//TP2M
#define MDVarLossToWater					"LossToWater"
#define MDVarLossToInlet                                        "LossToInlet"

#define MDVarAvgEfficiency                                       "AvgEfficiency"                // RJS 062012
#define MDVarAvgDeltaTemp                                        "AvgDeltaTemp"                 // RJS 062012
#define MDVarEfficiency1                                         "Efficiency1"                  // RJS 062012
#define MDVarEfficiency2                                         "Efficiency2"                  // RJS 062012
#define MDVarEfficiency3                                         "Efficiency3"                  // RJS 062012
#define MDVarEfficiency4                                         "Efficiency4"                  // RJS 062012
#define MDVarHeatToRiv                                           "HeatToRiv"                    // RJS 112712
#define MDVarHeatToSink                                          "HeatToSink"                   // RJS 112712
#define MDVarHeatToEng                                           "HeatToEng"                    // RJS 112712
#define MDVarHeatToElec                                          "HeatToElec"                   // RJS 112912
#define MDVarHeatToEvap                                          "HeatToEvap"                   // RJS 120212
#define MDVarLakeOcean1                                          "LakeOcean1"           // RJS 062012
#define MDVarLHFract                                             "LHFract"                      // RJS 071112
#define MDVarLHFractPost                                         "LHFractPost"          // RJS 071112

#define MDVarNamePlate1                                          "NamePlate1"           //RJS 013012
#define MDVarNamePlate2                                          "NamePlate2"           //RJS 013012
#define MDVarNamePlate3                                          "NamePlate3"           //RJS 013012
#define MDVarNamePlate4                                          "NamePlate4"           //RJS 013012

#define MDVarDemand1                                          "Demand1"           //RJS 013012
#define MDVarDemand2                                          "Demand2"           //RJS 013012
#define MDVarDemand3                                          "Demand3"           //RJS 013012
#define MDVarDemand4                                          "Demand4"           //RJS 013012
#define MDVarPowerOutput1                                        "PowerOutput1"         //RJS 013112
#define MDVarPowerDeficit1                                       "PowerDeficit1"                //RJS 013112
#define MDVarPowerPercent1                                       "PowerPercent1"                //RJS 013112
#define MDVarPowerOutputTotal                            "PowerOutputTotal"             //RJS 013112
#define MDVarPowerOutputTotal1                            "PowerOutputTotal1"             //RJS 013112
#define MDVarPowerOutputTotal2                            "PowerOutputTotal2"             //RJS 013112
#define MDVarPowerOutputTotal3                            "PowerOutputTotal3"             //RJS 013112
#define MDVarPowerOutputTotal4                            "PowerOutputTotal4"             //RJS 013112

#define MDVarGeneration                            "Generation"             //RJS 013112
#define MDVarGeneration1                            "Generation1"             //RJS 013112
#define MDVarGeneration2                            "Generation2"             //RJS 013112
#define MDVarGeneration3                            "Generation3"             //RJS 013112
#define MDVarGeneration4                            "Generation4"             //RJS 013112

#define MDVarPowerDeficitTotal                           "PowerDeficitTotal"    //RJS 013112
#define MDVarPowerPercentTotal                           "PowerPercentTotal"    //RJS 013112
#define MDVarQpp1                                                        "Qpp1"                 //RJS 071112
#define MDVarOptQO1                                                      "OptQO1"               //RJS 071112
#define MDVarTechnology1                                                 "Technology1"          // RJS 062012
#define MDVarTechnology2                                                 "Technology2"          // RJS 062012
#define MDVarTechnology3                                                 "Technology3"          // RJS 062012
#define MDVarTechnology4                                                 "Technology4"          // RJS 062012
#define MDVarCWA_316b_OnOff                                           "CWA_316b_OnOff"
#define MDVarDownstream_OnOff                                           "Downstream_OnOff"
#define MDVarCWA_OnOff                                           "CWA_OnOff"
#define MDVarCWA_Delta                                           "CWA_Delta"
#define MDVarCWA_Limit                                           "CWA_Limit"
#define MDVarGW_Temp						 "GW_Temp"
#define MDVarEquil_Temp                                             "Equil_Temp"

#define MDVarTotalHoursRun                                               "TotalHoursRun"        // RJS 030213
#define MDVarTotalThermalWdls                            "TotalThermalWdls"             //RJS 013112
#define MDVarTotalOptThermalWdls                         "TotalOptThermalWdls"  //RJS 020212
#define MDVarTotalEvaporation                            "TotalEvaporation"             // RJS 062012
#define MDVarTotalEnergyDemand                           "TotalEnergyDemand"    // RJS 062012
#define MDVarTotalReturnFlow                             "TotalReturnFlow"              // RJS 062012
#define MDVarCondenserInlet                                     "CondenserInlet"                        //RJS 122112
#define MDVarCondenserInlet1                                     "CondenserInlet1"                        //RJS 122112
#define MDVarCondenserInlet2                                     "CondenserInlet2"                        //RJS 122112
#define MDVarCondenserInlet3                                     "CondenserInlet3"                        //RJS 122112
#define MDVarCondenserInlet4                                     "CondenserInlet4"                        //RJS 122112
#define MDVarSimEfficiency                                      "SimEfficiency"                         //RJS 122112
#define MDVarTotalExternalWater					"TotalExternalWater"		//MIARA 042815

#define MDVarLossToInlet1                                     "LossToInlet1"                        //RJS 122112
#define MDVarLossToInlet2                                     "LossToInlet2"                        //RJS 122112
#define MDVarLossToInlet3                                     "LossToInlet3"                        //RJS 122112
#define MDVarLossToInlet4                                     "LossToInlet4"                        //RJS 122112

#define MDVarHeatToRiver1                                     "HeatToRiver1"                        //RJS 122112
#define MDVarHeatToRiver2                                     "HeatToRiver2"                        //RJS 122112
#define MDVarHeatToRiver3                                     "HeatToRiver3"                        //RJS 122112
#define MDVarHeatToRiver4                                     "HeatToRiver4"                        //RJS 122112

#define MDVarWTempGrdWater                   "GroundWaterTemperature"
#define MDVarWTempRiver                      "RiverTemperature"
#define MDVarWTempSurfRunoff                 "SurfaceROTemperature"
#define MDVarBgcLocalIn_QxT                  "QxT_LocalIn"
#define MDVarRemoval_QxT                     "QxT_Removal"
#define MDVarFlux_QxT                        "QxT_Flux"
#define MDVarStorage_QxT                     "QxT_Storage"
#define MDVarDeltaStorage_QxT                "QxT_DeltaStorage"
#define MDVarWTemp_QxT                       "QxT_WaterTemp"
#define MDVarWTempDeltaT_QxT                 "QxT_deltaT"
#define MDVarFluxMixing_QxT                  "QxT_FluxMixing"
#define MDVarStorageMixing_QxT               "QxT_StorageMixing"
#define MDVarDeltaStorageMixing_QxT          "QxT_DeltaStorageMixing"
#define MDVarWTempMixing_QxT                 "QxT_WaterTempMixing"

//REST
#define MDVarAccBalance                      "AccumBalance"
#define MDVarAccPrecipitation                "AccumPrecipitation"
#define MDVarAccEvapotranspiration           "AccumEvapotranspiration"
#define MDVarAccSoilMoistChange              "AccumSoilMoistureChange"
#define MDVarAccGroundWaterChange            "AccumGroundWaterChange"
#define MDVarAccRunoff                       "AccumRunoff"
#define MDVarAccBalance                      "AccumBalance"
#define MDVarAirTemperature                  "AirTemperature"
#define MDVarAirTempMinimum                  "AirTempMinimum"
#define MDVarAirTempMaximum                  "AirTempMaximum"
#define MDVarAirTemperatureAcc_space		"AirTempAcc_space"
#define MDVarAirTemperatureAcc_time		"AirTempAcc_time"
#define MDVarNewAirTemperatureAcc_time		"NewAirTempAcc_time"
#define MDVarAvgNSteps                       "AverageNSteps"
#define MDVarContributingAreaAcc                       "ContributingAreaAcc"

#define MDVarBaseFlow                        "BaseFlow"

#define MDVarCParamAlbedo                    "Albedo"
#define MDVarCParamCHeight                   "CanopyHeight"
#define MDVarCParamLWidth                    "LeafWidth"
#define MDVarCParamRSS                       "CParamRSS"
#define MDVarCParamR5                        "CParamR5"
#define MDVarCParamCD                        "CParamCD"
#define MDVarCParamCR                        "CParamCR"
#define MDVarCParamGLMax                     "CParamGLMax"
#define MDVarCParamLPMax                     "CParamLPMax"
#define MDVarCParamZ0g                       "CParamZ0g"

#define MDVarDischarge                       "Discharge"
#define MDVarDischarge0                      "Discharge0"
#define MDVarDischarge1                      "Discharge1"
#define MDVarDischObserved                   "DischargeObserved"
#define MDVarDischMean                       "DischargeMean"
#define MDVarDischargeAcc	"DischargeAcc"
#define MDVarNewDischargeAcc	"NewDischargeAcc"
#define MDVarOverBankQ	"OverBankQ"

#define MDVarEvapotranspiration              "Evapotranspiration"
#define MDVarFuelType1						 "FuelType1"			//RJS 013012
#define MDVarFuelType2						 "FuelType2"			//RJS 013012
#define MDVarFuelType3						 "FuelType3"			//RJS 013012
#define MDVarFuelType4						 "FuelType4"			//RJS 013012
#define MDVarGrossRadiance                   "GrossRadiance"
#define MDVarGroundWater                     "GroundWater"
#define MDVarGroundWaterChange               "GroundWaterChange"
#define MDVarGroundWaterRecharge             "GroundWaterRecharge"
#define MDVarGroundWaterUptake               "GroundWaterUptake"
#define MDVarLeafAreaIndex                   "LeafAreaIndex"

#define MDVarIceCover		              	"IceCover"

#define MDVarIrrAreaFraction                 "IrrigatedAreaFraction"

#define MDVarIrrAreaFractionSeason1          "IrrigatedAreaFractionSeason1"
#define MDVarIrrAreaFractionSeason2          "IrrigatedAreaFractionSeason2"

#define MDVarIrrEfficiency                   "IrrigationEfficiency"
#define MDVarIrrEvapotranspiration           "IrrEvapotranspiration"
#define MDVarIrrGrossDemand                  "IrrGrossDemand"
#define MDVarIrrGrowingSeason1Start          "GrowingSeason1_Start"
#define MDVarIrrGrowingSeason2Start          "GrowingSeason2_Start"

#define MDVarIrrIntensity                    "IrrigationIntensity"
#define MDVarIrrNetWaterDemand               "IrrNetWaterDemand"
#define MDVarIrrPrecipitation                "IrrPrecipitation"
#define MDVarIrrRefEvapotrans                "IrrReferenceETP"
#define MDVarIrrReturnFlow                   "IrrReturnFlow"
#define MDVarIrrSoilMoisture                 "IrrSoilMoisture"
#define MDVarIrrSoilMoistChange              "IrrSoilMoistChange"
#define MDVarIrrUptakeBalance                "IrrUptakeBalance"
#define MDVarIrrUptakeExternal               "IrrUptakeExternal"
#define MDVarIrrUptakeExcess                 "IrrUptakeExcess"
#define MDVarIrrUptakeGrdWater               "IrrUptakeGroundWater"
#define MDVarIrrUptakeRiver                  "IrrUptakeRiver"       
#define MDVarIrrWaterBalance                 "IrrWaterBalance"
#define MDNonIrrigatedFraction               "NonIrrigatedFraction"	
#define MDVarIrrDailyRicePerolationRate      "RicePercolationRate"	
#define MDVarIrrRicePondingDepth			 "RicePondingDepth"
#define	MDVarMeanElevation                   "Elevation"
#define MDVarMuskingumC0                     "MuskingumC0"
#define MDVarMuskingumC1                     "MuskingumC1"
#define MDVarMuskingumC2                     "MuskingumC2"

#define MDVarNamePlate1						 "NamePlate1"		//RJS 013012
#define MDVarNamePlate2						 "NamePlate2"		//RJS 013012
#define MDVarNamePlate3						 "NamePlate3"		//RJS 013012
#define MDVarNamePlate4						 "NamePlate4"		//RJS 013012
#define MDVarPowerOutput1					 "PowerOutput1"		//RJS 013112
#define MDVarPowerDeficit1					 "PowerDeficit1"		//RJS 013112
#define MDVarPowerPercent1					 "PowerPercent1"		//RJS 013112
#define MDVarPowerOutputTotal				 "PowerOutputTotal"		//RJS 013112
#define MDVarPowerDeficitTotal				 "PowerDeficitTotal"	//RJS 013112
#define MDVarPowerPercentTotal				 "PowerPercentTotal"	//RJS 013112

#define MDVarPrecipitation                   "Precipitation"
#define MDVarPrecipFraction                  "PrecipitationFraction"
#define MDVarPrecipMonthly                   "MonthlyPrecipitation"

#define MDVarWetlandEvapotranspiration		 "WetlandEvapotranspiration"
#define MDVarWetlandAreaFraction			 "WetlandAreaFraction"

#define MDVarRainEvapotranspiration          "RainEvapotranspiration"
#define MDVarRainInfiltration                "RainInfiltration"
#define MDVarRainInterception                "RainInterception"
#define MDVarRainPotEvapotrans               "RainPET"
#define MDVarRainSoilMoisture                "RainSoilMoisture"
#define MDVarRainSoilMoistureCell            "RainSoilMoistureCell"
#define MDVarRainSoilMoistChange             "RainSoilMoistureChange"
#define MDVarRainSurfRunoff                  "RainSurfRunoff"
#define MDVarRainWaterSurplus                "RainWaterSurplus"

#define MDVarReservoirCapacity               "ReservoirCapacity"
#define MDVarReservoirRelease                "ReservoirRelease"
#define MDVarReservoirStorage                "ReservoirStorage" 
#define MDVarReservoirStorageChange          "ReservoirStorageChange"
#define MDVarRelief                          "ReliefMax"

#define MDVarRelSoilMoisture                 "RelativeSoilMoisture"	
#define MDVarRiverbedAvgDepthMean            "RiverbedAvgDepthMean"
#define MDVarRiverbedSlope                   "RiverbedSlope"
#define MDVarRiverbedShapeExponent           "RiverbedShapeExponent"
#define MDVarRiverbedWidthMean               "RiverbedWidthMean"

#define MDVarRiverbedVelocityMean            "RiverbedVelocityMean"
#define MDVarRiverDepth                      "RiverDepth"
#define MDVarRiverStorage                    "RiverStorage"
#define MDVarRiverStorageChg                 "RiverStorageChange"
#define MDVarRiverWidth                      "RiverbedWidth"

#define MDVarRunoff                          "Runoff"
#define MDVarRunoffMean                      "RunoffMean"
#define MDVarRunoffCorretion                 "RunoffCorrection"
#define MDVarRunoffVolume                    "RunoffVolume"

#define MDVarSedimentFlux		     "SedimentFlux"
#define MDVarBedloadFlux		     "BedloadFlux"

#define MDVarNewSedimentAcc		"NewSedimentAcc"
#define MDVarBQART_A			     "BQART_A"
#define MDVarBQART_B	     	     "BQART_B"
#define MDVarBQART_Qbar_km3y	"BQART_Qbar_km3y"
#define MDVarBQART_Qbar_m3s	"BQART_Qbar_m3s"
#define MDVarBQART_R	     	     "BQART_R"
#define MDVarBQART_T	     	     "BQART_T"
#define MDVarBQART_Lithology	     "BQART_Lithology"
#define MDVarBQART_GNP	     	     "BQART_GNP"
#define MDVarPopulation	     	     "Population"
#define MDVarLithologyAreaAcc	"LithologyAreaAcc"
#define MDVarLithologyMean	"LithologyMean"
#define MDVarPopulationAcc	"PopulationAcc"
#define MDVarPopulationDensity  "PopulationDensity"
#define MDVarGNPAreaAcc		"GNPAreaAcc"
#define MDVarMeanGNP		"MeanGNP"
#define MDVarBQART_Eh		"BQART_Eh"
#define MDVarBQART_Te		"BQART_Te"
#define MDVarTeAacc		"TeAacc"
#define MDVarResStorageAcc	"ResStorageAcc"
#define MDVarBQART_Te		"BQART_Te"
#define MDVarQs_bar		"Qs_bar"
#define MDVarrnseed		"rnseed"
#define MDVarMinSlope		"Slope-Min"
#define MDVarUpStreamQb		"UpStreamQb"
#define MDDeltaBedload		"DeltaBedload"
#define MDVarUpStreamQs		"UpStreamQs"
#define MDDeltaQs		"DeltaQs"
#define MDVarBankfullQ		"BankfullQ"
#define MDVarBankfullQ2		"BankfullQ2"
#define MDVarBankfullQ5		"BankfullQ5"
#define MDVarBankfullQ10	"BankfullQ10"
#define MDVarBankfullQ25		"BankfullQ25"
#define MDVarBankfullQ50		"BankfullQ50"
#define MDVarBankfullQ100		"BankfullQ100"
#define MDVarBankfullQ200		"BankfullQ200"
#define MDVarBankfull_Qn		"Bankfull_Qn"
#define MDVarFlowCoefficient		"FlowCoefficient"

#define MDVarSAGEVegCover                    "SAGEVegCover"
#define MDVarSmallReservoirStorageFrac       "SmallReservoirStorageFraction"	
#define MDVarSmallResCapacity                "SmallReservoirCapacity"
#define MDVarSmallResRelease                 "SmallReservoirRelease"
#define MDVarSmallResStorage                 "SmallReservoirStorage" 
#define MDVarSmallResStorageChange           "SmallReservoirStorageChange"	
#define MDVarSmallResUptake                  "SmallReservoirUptake"
#define MDVarSmallResEvaporation             "SmallReservoirEvaporation"
#define MDVarSnowFall                        "SnowFall"
#define MDVarSnowMelt                        "SnowMelt"
#define MDVarSnowPack                        "SnowPack"
#define MDVarSnowPackChange                  "SnowPackChange"
#define MDVarSoilAvailWaterCap               "AvailableWaterCapacity"
#define MDVarSoilFieldCapacity               "FieldCapacity"
#define MDVarSoilRootingDepth                "RootingDepth"
#define MDVarSoilMoisture                    "SoilMoisture"
#define MDVarSoilMoistChange                 "SoilMoistureChange"
#define MDVarSoilWiltingPoint                "WiltingPoint"
#define MDVarSoilBulkDensity                 "SoilBulkDensity"
#define MDVarTotalThermalWdls				 "TotalThermalWdls"	//RJS 013112
#define MDVarTotalOptThermalWdls			 "TotalOptThermalWdls" //RJS 020212
#define MDVarTimeSteps			"TimeSteps"
#define MDVarNewTimeSteps		"NewTimeSteps"

#define MDVarSolarRadiation                  "SolarRadiation"
#define MDVarSRadDayLength                   "DayLength"
#define MDVarSRadI0HDay                      "I0HDay"
#define MDVarStemAreaIndex                   "StemAreaIndex"
#define MDVarSunShine                        "SunShine"
#define MDVarSurfRunoff                      "SurfaceRunoff"
#define MDVarVaporPressure                   "VaporPressure"
#define MDVarTEMVegCover                     "TEMVegCover"
#define MDVarWBMCover                        "WBMCover"
#define MDVarWaterBalance                    "WaterBalance"
#define MDVarWetDays                         "WetDays"
#define MDVarWetDaysAlpha                    "WetDaysAlpha"
#define MDVarWetDaysBeta                     "WetDaysBeta"
#define MDVarWindSpeed                       "WindSpeed"

#define	MDVarSaturationExcessflow			"SaturationExcessFlow"

#define MDVarWetlandAreaFraction            "WetlandAreaFraction"
#define MDVarWetlandEvapotranspiration	    "WetlandEvapotranspiration"
#define MDVarWetlandSurfROUptake            "WetlandSurfROUptake"
#define MDVarWetlandRunoff                  "WetlandRunoff"
#define MDVarWetlandStorageChange           "WetlandStorageChange"
#define MDVarWetlandCatchmentAreaFactor		"WetlandCatchmentAreaFactor"
#define MDVarWetlandStorage					"WetlandStorage"	
#define MDVarWetlandWaterLevelDepth			"WetlandWaterLevelDepth"


#define MDParGrossRadTAU                     "GrossRadTAU"
#define MDParGroundWatBETA                   "GroundWaterBETA"
#define MDParSoilMoistALPHA                  "SoilMoistureALPHA"
#define MDParInfiltrationFrac                "InfiltrationFraction"
#define MDVarOutSoilDebug					 "DebugOutput"
#define MDVarInitalSoilTemperature		     "InitialSoilTemperature"
#define MDVarLiquidSoilMoisture			     "LiquidSoilMoisture"
#define MDVarActiveLayerDepth                "ActiveLayerDepth"
#define MDVarThawingDepth                    "ThawingDepth"
#define MDVarSnowDepth                       "SnowDepth"
#define MDVarSnowDensity                     "SnowDensity"
#define	MDVarIsInitial                       "IsInitial"
#define MDVarSoilMoistureIsInitial           "SoilMoistureIsInitial"
#define MDVarWinterOnsetDoy                  "WinterOnsetDoY"
#define MDVarWaterTableDepth                 "WaterTableDepth"
#define MDVarSoilOrganicLayer                "SoilOrganicLayer"
#define MDParInfiltrationFracSpatial         "InfiltrationFractionSpatial"
#define MDParSnowMeltThreshold               "SnowMeltThreshold"	
#define	MDParFallThreshold				     "SnowFallThreshold"

#define MDConstInterceptCI  0.3	   // Interception LAI+SAI constant
#define MDConstInterceptCH 10.0    // Interception canopy height constant 
#define MDConstInterceptD   0.2    // Interception rain fraction of the day
#define MDConstLPC          4.0    // minimum projected LAI for "closed" canopy
#define MDConstC1           0.25   // intercept in actual/potential solar radiation to sunshine duration
#define MDConstC2           0.5    // slope in actual/potential solar radiation to sunshine duration
#define MDConstC3           0.2    // longwave cloud correction coefficient
#define MDConstPTALPHA      1.26   // Priestley-Taylor coefficient
#define MDConstCZS          0.13   // Ratio of roughness to height for smooth closed canopies
#define MDConstCZR          0.05   // ratio of roughness to height for rough closed canopies
#define MDConstHR          10.0    // height above which CZR applies, m
#define MDConstHS           1.0    // height below which CZS applies, m
#define MDConstZMINH        2.0    // reference height above the canopy height m
#define MDConstRSS        500.0    // soil surface resistance, s/m TODO Not Used!
#define MDConstWNDRAT       0.3    // ratio of nighttime to daytime wind speed
#define MDConstN            2.5    // wind/diffusivity extinction coefficient
#define MDConstCS           0.035  // ratio of projected stem area to height for closed canopy
#define MDConstGLMIN        0.0003 // cuticular leaf surface conductance for all sides of leaf, m/s
#define MDConstRM        1000.0    // maximum solar radiation, at which FR = 1,  W/m2
#define MDConstRHOTP        2.0    // ratio of total leaf surface area to projected leaf area
#define MDConstCPRHO     1240.0    // volumetric heat capacity of air, J/(K m3)
#define MDConstPSGAMMA      0.067  // psychrometric constant, kPa/K
#define MDConstIGRATE       0.0864 // integrates W/m2 over 1 d to MJ/m2
#define MDConstEtoM         0.4085 // converts MJ/m2 to mm of water
#define MDConstSIGMA      5.67E-08 // Stefan-Boltzmann constant, W/(m2/K4)
#define MDConstK            0.4    // von Karman constant
#define MDMinimum(a,b) (((a) < (b)) ? (a) : (b))
#define MDMaximum(a,b) (((a) > (b)) ? (a) : (b))

int MDAccumPrecipDef();
int MDAccumEvapDef();
int MDAccumSMoistChgDef();
int MDAccumGrdWatChgDef();
int MDAccumRunoffDef();
int MDAccumBalanceDef();
int MDAvgNStepsDef();
int MDBaseFlowDef();

int MDBQARTpreprocessDef(); //NEW !!!!!  (S.C)
int MDBankfullQcalcDef();//NEW !!!!!  (S.C)
int MDBedloadFluxDef();//NEW !!!!!  (S.C)
int MDSedimentFluxDef(); //NEW !!!!!  (S.C)
int MDDischargeBFDef ();

int MDCloudCoverDef ();
int MDRelativeHumidityDef ();
int MDSpecificHumidityDef ();
int MDWetBulbTempDef();

int MDCParamAlbedoDef();
int MDCParamCHeightDef();
int MDCParamLWidthDef();
int MDCParamRSSDef();
int MDCParamR5Def();
int MDCParamCDDef();
int MDCParamCRDef();
int MDCParamGLMaxDef();
int MDCParamLPMaxDef();
int MDCParamZ0gDef();
int MDDischargeDef();
int MDDischLevel1Def();
int MDDischLevel2Def();
int MDDischLevel3Def();
int MDDischLevel3AccumulateDef();
int MDDischLevel3CascadeDef();
int MDDischLevel3MuskingumDef();
int MDDischLevel3MuskingumCoeffDef();
int MDDischMeanDef();
int MDEvapotranspirationDef();
int MDGrossRadDef();
int MDIrrigatedAreaDef();
int MDIrrGrossDemandDef();
int MDIrrRefEvapotransDef();
int MDIrrRefEvapotransFAODef();
int MDIrrRefEvapotransHamonDef();
int MDIrrReturnFlowDef();
int MDIrrSoilMoistureDef();
int MDIrrSoilMoistChgDef();
int MDIrrUptakeRiverDef();
int MDIrrUptakeGrdWaterDef();
int MDReservoirDef();
int MDLandCoverDef();
int MDLeafAreaIndexDef();
int MDRainInfiltrationDef();
int MDRainInterceptDef();
int MDRainPotETDef();
int MDRainPotETHamonDef();
int MDRainPotETJensenDef();
int MDRainPotETPstdDef();
int MDRainPotETPsTaylorDef();
int MDRainPotETPMdayDef();
int MDRainPotETPMdnDef();
int MDRainPotETSWGdayDef();
int MDRainPotETSWGdnDef();
int MDRainPotETTurcDef();
int MDRainSMoistChgDef();
int MDRainSurfRunoffDef();
int MDRainWaterSurplusDef();
int MDWetlandRunoffDef();
int MDRiverbedShapeExponentDef();
int MDRiverWidthDef();

int MDRunoffDef();
int MDRunoffMeanDef();
int MDRunoffVolumeDef();
int MDSPackChgDef();
int MDSPackMeltDef();
int MDSmallReservoirReleaseDef();
int MDSmallReservoirCapacityDef();
int MDSolarRadDef();
int MDSRadDayLengthDef();
int MDSRadI0HDayDef();
int MDSoilAvailWaterCapDef();
int MDSoilMoistChgDef();
int MDSurfRunoffDef();
int MDStemAreaIndexDef();
int MDWaterBalanceDef();
int MDPrecipitationDef();
int MDWetDaysDef();
int MDWetlandAreaDef();
int MDPermafrostDef();
int MDWTempGrdWaterDef();
int MDWTempRiverDef();
int MDWTempRiverRouteDef();
int MDThermalInputs3Def();  //changed AM 042415

int MDWTempSurfRunoffDef();

int MDRainWaterSurplusDef();
int MDRainSMoistChgLayeredSoilDef();
/* PET & Related Functions */

float MDSRadNETLong(float, float, float, float);

float MDPETlibVPressSat(float);
float MDPETlibVPressDelta(float);

float MDWindAdjustment(float, float, float, float, float, float);

float MDPETlibLeafAreaIndex(float, float);
float MDPETlibSteamAreaIndex(float, float);
float MDPETlibRoughnessClosed(float, float);
float MDPETlibRoughness(float, float, float, float, float);
float MDPETlibZPDisplacement(float, float, float, float);
float MDPETlibCanopySurfResistance(float, float, float, float, float, float,
		float, float, float);
float MDPETlibBoundaryResistance(float, float, float, float, float, float,
		float);
float MDPETlibLeafResistance(float, float, float, float, float, float, float,
		float);
float MDPETlibGroundResistance(float, float, float, float, float, float, float);

float MDPETlibPenmanMontieth(float, float, float, float, float);
float MDPETlibShuttleworthWallace(float, float, float, float, float, float,
		float, float, float);

#if defined(__cplusplus)
}
#endif

#endif /* MD_H_INCLUDED */
