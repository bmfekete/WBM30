/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

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

// Configuration options
#define MDOptConfig_Discharge                   "Discharge"
#define MDOptConfig_Irrigation                  "Irrigation"
#define MDOptConfig_Model                       "Model"
#define MDOptConfig_Reservoirs                  "Reservoirs"
#define MDOptConfig_Routing                     "Routing"

// Irrigation options
#define MDOptIrrigation_AreaMap                 "IrrigatedAreaMap"
#define MDOptIrrigation_ReferenceET             "IrrReferenceETP"

// River routing options
#define MDOptRouting_Muskingum                  "Muskingum"
#define MDOptRouting_Riverbed                   "Riverbed"

// Weather options
#define MDOptWeather_CloudCover		            "CloudCover"
#define MDOptWeather_SpecificHumidity           "SpecificHumidity"
#define MDOptWeather_RelativeHumidity           "RelativeHumidity" //for wet bulb
#define MDOptWeather_WetBulbTemp                "WetBulbTemp"

// Constant parameters
#define MDParGrossRadTAU                        "GrossRadTAU"
#define MDParGroundWatBETA                      "GroundWaterBETA"
#define MDParInfiltrationFrac                   "InfiltrationFraction"
#define MDParSoilMoistALPHA                     "SoilMoistureALPHA"
#define	MDParSnowFallThreshold				    "SnowFallThreshold"
#define MDParSnowMeltThreshold                  "SnowMeltThreshold"

// Auxiliary variables
#define MDVarAux_AccBalance                     "AccumBalance"
#define MDVarAux_AccEvapotranspiration          "AccumEvapotranspiration"
#define MDVarAux_AccSoilMoistChange             "AccumSoilMoistureChange"
#define MDVarAux_AccGroundWaterChange           "AccumGroundWaterChange"
#define MDVarAux_AccPrecipitation               "AccumPrecipitation"
#define MDVarAux_AccRunoff                      "AccumRunoff"
#define MDVarAux_AvgNSteps                      "AverageNSteps"
#define MDVarAux_DischMean                      "DischargeMean"

// Common variables
#define MDVarCommon_AirTemperature              "AirTemperature"
#define MDVarCommon_AirTempMinimum              "AirTempMinimum"
#define MDVarCommon_AirTempMaximum              "AirTempMaximum"
#define MDVarCommon_AirPressure                 "AirPressure"
#define MDVarCommon_CloudCover		            "CloudCover"
#define MDVarCommon_HumiditySpecific            "SpecificHumidity"
#define MDVarCommon_HumidityRelative            "RelativeHumidity"
#define MDVarCommon_IceCover		            "IceCover"
#define	MDVarCommon_MeanElevation               "Elevation"
#define MDVarCommon_Precipitation               "Precipitation"
#define MDVarCommon_PrecipFraction              "PrecipitationFraction"
#define MDVarCommon_PrecipMonthly               "MonthlyPrecipitation"
#define MDVarCommon_SnowDepth                   "SnowDepth"
#define MDVarCommon_SnowDensity                 "SnowDensity"
#define MDVarCommon_SnowFall                    "SnowFall"
#define MDVarCommon_WetBulbTemp                 "WetBulbTemp"
#define MDVarCommon_WetDays                     "WetDays"
#define MDVarCommon_WindSpeed                   "WindSpeed"

// Core variables
#define MDVarCore_BaseFlow                      "BaseFlow"
#define MDVarCore_Evapotranspiration            "Evapotranspiration"
#define MDVarCore_GrossRadiance                 "GrossRadiance"
#define MDVarCore_GroundWater                   "GroundWater"
#define MDVarCore_GroundWaterChange             "GroundWaterChange"
#define MDVarCore_GroundWaterRecharge           "GroundWaterRecharge"
#define MDVarCore_GroundWaterUptake             "GroundWaterUptake"
#define MDVarCore_LandCoverSAGE                 "SAGEVegCover"
#define MDVarCore_LandCoverTEM                  "TEMVegCover"
#define MDVarCore_LandCoverWBM                  "WBMCover"
#define MDVarCore_LeafAreaIndex                 "LeafAreaIndex"
#define MDVarCore_RainEvapotranspiration        "RainEvapotranspiration"
#define MDVarCore_RainInfiltration              "RainInfiltration"
#define MDVarCore_RainInterception              "RainInterception"
#define MDVarCore_RainPotEvapotrans             "RainPET"
#define MDVarCore_RainSoilMoisture              "RainSoilMoisture"
#define MDVarCore_RainSoilMoistureCell          "RainSoilMoistureCell"
#define MDVarCore_RainSoilMoistChange           "RainSoilMoistureChange"
#define MDVarCore_RainSurfRunoff                "RainSurfRunoff"
#define MDVarCore_RainWaterSurplus              "RainWaterSurplus"
#define MDVarCore_RelSoilMoisture               "RelativeSoilMoisture"
#define MDVarCore_Runoff                        "Runoff"
#define MDVarCore_RunoffMean                    "RunoffMean"
#define MDVarCore_RunoffVolume                  "RunoffVolume"
#define MDVarCore_SnowMelt                      "SnowMelt"
#define MDVarCore_SnowPack                      "SnowPack"
#define MDVarCore_SnowPackChange                "SnowPackChange"
#define MDVarCore_SolarRadiation                "SolarRadiation"
#define MDVarCore_SolarRadDayLength             "DayLength"
#define MDVarCore_SolarRadI0HDay                "I0HDay"
#define MDVarCore_SoilAvailWaterCap             "SoilWaterCapacity"
#define MDVarCore_SoilFieldCapacity             "FieldCapacity"
#define MDVarCore_SoilRootingDepth              "RootingDepth"
#define MDVarCore_SoilMoisture                  "SoilMoisture"
#define MDVarCore_SoilMoistChange               "SoilMoistureChange"
#define MDVarCore_SoilWiltingPoint              "WiltingPoint"
#define MDVarCore_StemAreaIndex                 "StemAreaIndex"
#define MDVarCore_SunShine                      "SunShine"
#define MDVarCore_SurfRunoff                    "SurfaceRunoff"
#define MDVarCore_VaporPressure                 "VaporPressure"
#define MDVarCore_WaterBalance                  "WaterBalance"

// Data assimilation variables
#define MDVarDataAssim_DischObserved            "DischargeObserved"
#define MDVarDataAssim_RunoffCorretion          "RunoffCorrection"

// Irrigation variables
#define MDVarIrrigation_AreaFraction            "IrrigatedAreaFraction"
#define MDVarIrrigation_AreaFractionSeason1     "IrrigatedAreaFractionSeason1"
#define MDVarIrrigation_AreaFractionSeason2     "IrrigatedAreaFractionSeason2"
#define MDVarIrrigation_Efficiency              "IrrigationEfficiency"
#define MDVarIrrigation_Evapotranspiration      "IrrEvapotranspiration"
#define MDVarIrrigation_GrossDemand             "IrrGrossDemand"
#define MDVarIrrigation_GrowingSeason1Start     "GrowingSeason1_Start"
#define MDVarIrrigation_GrowingSeason2Start     "GrowingSeason2_Start"
#define MDVarIrrigation_Intensity               "IrrigationIntensity"
#define MDVarIrrigation_NetDemand               "IrrNetWaterDemand"
#define MDVarIrrigation_Precipitation           "IrrPrecipitation"
#define MDVarIrrigation_RefEvapotrans           "IrrReferenceETP"
#define MDVarIrrigation_Runoff                  "IrrRunoff"
#define MDVarIrrigation_ReturnFlow              "IrrReturnFlow"
#define MDVarIrrigation_SoilMoisture            "IrrSoilMoisture"
#define MDVarIrrigation_SoilMoistChange         "IrrSoilMoistureChange"
#define MDVarIrrigation_UptakeBalance           "IrrUptakeBalance"
#define MDVarIrrigation_UptakeExternal          "IrrUptakeExternal"
#define MDVarIrrigation_UptakeExcess            "IrrUptakeExcess"
#define MDVarIrrigation_UptakeGrdWater          "IrrUptakeGroundWater"
#define MDVarIrrigation_UptakeRiver             "IrrUptakeRiver"
#define MDVarIrrigation_WaterBalance            "IrrWaterBalance"
#define MDVarIrrigation_DailyRicePerolationRate "RicePercolationRate"
#define MDVarIrrigation_RicePondingDepth        "RicePondingDepth"

// Reservoir variables
#define MDVarReservoir_FarmPondStorageFrac      "SmallReservoirStorageFraction"
#define MDVarReservoir_FarmPondSmallResCapacity "SmallReservoirCapacity"
#define MDVarReservoir_FarmPondRelease          "SmallReservoirRelease"
#define MDVarReservoir_FarmPondStorage          "SmallReservoirStorage"
#define MDVarReservoir_FarmPondStorageChange    "SmallReservoirStorageChange"
#define MDVarReservoir_FarmPontUptake           "SmallReservoirUptake"
#define MDVarReservoir_FarmPondEvaporation      "SmallReservoirEvaporation"
#define MDVarReservoir_Capacity                 "ReservoirCapacity"
#define MDVarReservoir_Release                  "ReservoirRelease"
#define MDVarReservoir_Storage                  "ReservoirStorage"
#define MDVarReservoir_StorageChange            "ReservoirStorageChange"

// Routing variables
#define MDVarRouting_BankfullQ                  "BankfullQ"
#define MDVarRouting_BankfullQ2                 "BankfullQ2"
#define MDVarRouting_BankfullQ5                 "BankfullQ5"
#define MDVarRouting_BankfullQ10                "BankfullQ10"
#define MDVarRouting_BankfullQ25                "BankfullQ25"
#define MDVarRouting_BankfullQ50                "BankfullQ50"
#define MDVarRouting_BankfullQ100               "BankfullQ100"
#define MDVarRouting_BankfullQ200               "BankfullQ200"
#define MDVarRouting_Bankfull_Qn                "Bankfull_Qn"
#define MDVarRouting_Discharge                  "Discharge"
#define MDVarRouting_Discharge0                 "Discharge0"
#define MDVarRouting_Discharge1                 "Discharge1"
#define MDVarRouting_MuskingumC0                "MuskingumC0"
#define MDVarRouting_MuskingumC1                "MuskingumC1"
#define MDVarRouting_MuskingumC2                "MuskingumC2"
#define MDVarRouting_RiverbedAvgDepthMean       "RiverbedAvgDepthMean"
#define MDVarRouting_RiverbedSlope              "RiverbedSlope"
#define MDVarRouting_RiverbedShapeExponent      "RiverbedShapeExponent"
#define MDVarRouting_RiverbedWidthMean          "RiverbedWidthMean"
#define MDVarRouting_RiverbedVelocityMean       "RiverbedVelocityMean"
#define MDVarRouting_RiverDepth                 "RiverDepth"
#define MDVarRouting_RiverStorage               "RiverStorage"
#define MDVarRouting_RiverStorageChg            "RiverStorageChange"
#define MDVarRouting_RiverWidth                 "RiverWidth"

// Sediment variable
#define MDVarSediment_DischargeAcc	            "DischargeAcc"
#define MDVarSediment_NewDischargeAcc	        "NewDischargeAcc"
#define MDVarSediment_OverBankQ	                "OverBankQ"
#define MDVarSediment_Relief                    "ReliefMax"
#define MDVarSediment_SuspendedFlux		        "SedimentFlux"
#define MDVarSediment_BedloadFlux		        "BedloadFlux"
#define MDVarSediment_AirTemperatureAcc_space	"AirTempAcc_space"
#define MDVarSediment_AirTemperatureAcc_time    "AirTempAcc_time"
#define MDVarSediment_NewAirTemperatureAcc_time "NewAirTempAcc_time"
#define MDVarSediment_ContributingAreaAcc       "ContributingAreaAcc"
#define MDVarSediment_TimeSteps                 "TimeSteps"
#define MDVarSediment_NewTimeSteps              "NewTimeSteps"
#define MDVarSediment_NewSedimentAcc		    "NewSedimentAcc"
#define MDVarSediment_BQART_A			        "BQART_A"
#define MDVarSediment_BQART_B	     	        "BQART_B"
#define MDVarSediment_BQART_Qbar_km3y           "BQART_Qbar_km3y"
#define MDVarSediment_BQART_Qbar_m3s            "BQART_Qbar_m3s"
#define MDVarSediment_BQART_R                   "BQART_R"
#define MDVarSediment_QART_T                    "BQART_T"
#define MDVarSediment_BQART_Lithology           "BQART_Lithology"
#define MDVarSediment_BQART_GNP                 "BQART_GNP"
#define MDVarSediment_Population                "Population"
#define MDVarSediment_LithologyAreaAcc          "LithologyAreaAcc"
#define MDVarSediment_LithologyMean             "LithologyMean"
#define MDVarSediment_PopulationAcc             "PopulationAcc"
#define MDVarSediment_PopulationDensity         "PopulationDensity"
#define MDVarSediment_GNPAreaAcc                "GNPAreaAcc"
#define MDVarSediment_MeanGNP                   "MeanGNP"
#define MDVarSediment_BQART_Eh                  "BQART_Eh"
#define MDVarSediment_BQART_Te                  "BQART_Te"
#define MDVarSediment_TeAacc                    "TeAacc"
#define MDVarSediment_ResStorageAcc             "ResStorageAcc"
#define MDVarSediment_Qs_bar                    "Qs_bar"
#define MDVarSediment_rnseed                    "rnseed"
#define MDVarSediment_MinSlope                  "Slope-Min"
#define MDVarSediment_UpStreamQb                "UpStreamQb"
#define MDVarSediment_DeltaBedload              "DeltaBedload"
#define MDVarSediment_UpStreamQs                "UpStreamQs"
#define MDVarSediment_DeltaQs                   "DeltaQs"
#define MDVarSediment_FlowCoefficient           "FlowCoefficient"

// TP2M Variables
#define MDVarTP2M_AvgEfficiency                 "AvgEfficiency"
#define MDVarTP2M_AvgDeltaTemp                  "AvgDeltaTemp"
#define MDVarTP2M_CondenserInlet                "CondenserInlet"
#define MDVarTP2M_CondenserInlet1               "CondenserInlet1"
#define MDVarTP2M_CondenserInlet2               "CondenserInlet2"
#define MDVarTP2M_CondenserInlet3               "CondenserInlet3"
#define MDVarTP2M_CondenserInlet4               "CondenserInlet4"
#define MDVarTP2M_CWA_Delta                     "CWA_Delta"
#define MDVarTP2M_CWA_Limit                     "CWA_Limit"
#define MDVarTP2M_CWA_OnOff                     "CWA_OnOff"
#define MDVarTP2M_CWA_316b_OnOff                "CWA_316b_OnOff"
#define MDVarTP2M_Demand1                       "Demand1"
#define MDVarTP2M_Demand2                       "Demand2"
#define MDVarTP2M_Demand3                       "Demand3"
#define MDVarTP2M_Demand4                       "Demand4"
#define MDVarTP2M_Downstream_OnOff              "Downstream_OnOff"
#define MDVarTP2M_Equil_Temp                    "Equil_Temp"
#define MDVarTP2M_Efficiency1                   "Efficiency1"
#define MDVarTP2M_Efficiency2                   "Efficiency2"
#define MDVarTP2M_Efficiency3                   "Efficiency3"
#define MDVarTP2M_Efficiency4                   "Efficiency4"
#define MDVarTP2M_FuelType1						"FuelType1"
#define MDVarTP2M_FuelType2						"FuelType2"
#define MDVarTP2M_FuelType3						"FuelType3"
#define MDVarTP2M_FuelType4						"FuelType4"
#define MDVarTP2M_Generation                    "Generation"
#define MDVarTP2M_Generation1                   "Generation1"
#define MDVarTP2M_Generation2                   "Generation2"
#define MDVarTP2M_Generation3                   "Generation3"
#define MDVarTP2M_Generation4                   "Generation4"
#define MDVarTP2M_GW_Temp					    "GW_Temp"
#define MDVarTP2M_HeatToRiver1                  "HeatToRiver1"
#define MDVarTP2M_HeatToRiver2                  "HeatToRiver2"
#define MDVarTP2M_HeatToRiver3                  "HeatToRiver3"
#define MDVarTP2M_HeatToRiver4                  "HeatToRiver4"
#define MDVarTP2M_LossToInlet                   "LossToInlet"
#define MDVarTP2M_LossToInlet1                  "LossToInlet1"
#define MDVarTP2M_LossToInlet2                  "LossToInlet2"
#define MDVarTP2M_LossToInlet3                  "LossToInlet3"
#define MDVarTP2M_LossToInlet4                  "LossToInlet4"
#define MDVarTP2M_LossToWater					"LossToWater"
#define MDVarTP2M_HeatToRiv                     "HeatToRiv"
#define MDVarTP2M_HeatToSink                    "HeatToSink"
#define MDVarTP2M_HeatToEng                     "HeatToEng"
#define MDVarTP2M_HeatToElec                    "HeatToElec"
#define MDVarTP2M_HeatToEvap                    "HeatToEvap"
#define MDVarTP2M_LakeOcean1                    "LakeOcean1"
#define MDVarTP2M_LHFract                       "LHFract"
#define MDVarTP2M_LHFractPost                   "LHFractPost"
#define MDVarTP2M_NamePlate1                    "NamePlate1"
#define MDVarTP2M_NamePlate2                    "NamePlate2"
#define MDVarTP2M_NamePlate3                    "NamePlate3"
#define MDVarTP2M_NamePlate4                    "NamePlate4"
#define MDVarTP2M_PowerOutput1                  "PowerOutput1"
#define MDVarTP2M_PowerDeficit1                 "PowerDeficit1"
#define MDVarTP2M_PowerPercent1                 "PowerPercent1"
#define MDVarTP2M_PowerOutputTotal              "PowerOutputTotal"
#define MDVarTP2M_PowerOutputTotal1             "PowerOutputTotal1"
#define MDVarTP2M_PowerOutputTotal2             "PowerOutputTotal2"
#define MDVarTP2M_PowerOutputTotal3             "PowerOutputTotal3"
#define MDVarTP2M_PowerOutputTotal4             "PowerOutputTotal4"
#define MDVarTP2M_PowerDeficitTotal             "PowerDeficitTotal"
#define MDVarTP2M_PowerPercentTotal             "PowerPercentTotal"
#define MDVarTP2M_Qpp1                          "Qpp1"
#define MDVarTP2M_OptQO1                        "OptQO1"
#define MDVarTP2M_Technology1                   "Technology1"
#define MDVarTP2M_Technology2                   "Technology2"
#define MDVarTP2M_Technology3                   "Technology3"
#define MDVarTP2M_Technology4                   "Technology4"
#define MDVarTP2M_TotalEvaporation              "TotalEvaporation"
#define MDVarTP2M_TotalEnergyDemand             "TotalEnergyDemand"
#define MDVarTP2M_TotalHoursRun                 "TotalHoursRun"
#define MDVarTP2M_TotalThermalWdls              "TotalThermalWdls"
#define MDVarTP2M_TotalOptThermalWdls           "TotalOptThermalWdls"
#define MDVarTP2M_TotalReturnFlow               "TotalReturnFlow"
#define MDVarTP2M_SimEfficiency                 "SimEfficiency"
#define MDVarTP2M_TotalExternalWater			"TotalExternalWater"
#define MDVarTP2M_WTempGrdWater                 "GroundWaterTemperature"
#define MDVarTP2M_WTempRiver                    "RiverTemperature"
#define MDVarTP2M_WTempSurfRunoff               "SurfaceROTemperature"
#define MDVarTP2M_WTLocalIn_QxT                 "QxT_LocalIn"
#define MDVarTP2M_Removal_QxT                   "QxT_Removal"
#define MDVarTP2M_Flux_QxT                      "QxT_Flux"
#define MDVarTP2M_Storage_QxT                   "QxT_Storage"
#define MDVarTP2M_DeltaStorage_QxT              "QxT_DeltaStorage"
#define MDVarTP2M_Temp_QxT                      "QxT_WaterTemp"
#define MDVarTP2M_WTempDeltaT_QxT               "QxT_deltaT"
#define MDVarTP2M_FluxMixing_QxT                "QxT_FluxMixing"
#define MDVarTP2M_StorageMixing_QxT             "QxT_StorageMixing"
#define MDVarTP2M_DeltaStorageMixing_QxT        "QxT_DeltaStorageMixing"
#define MDVarTP2M_WTempMixing_QxT               "QxT_WaterTempMixing"
#define MDOptTP2M_ThermalInputs					"ThermalInputs3"

// Varying Parameters
#define MDVarParam_Albedo                       "Albedo"
#define MDVarParam_CHeight                      "CanopyHeight"
#define MDVarParam_LWidth                       "LeafWidth"
#define MDVarParam_RSS                          "CParamRSS"
#define MDVarParam_R5                           "CParamR5"
#define MDVarParam_CD                           "CParamCD"
#define MDVarParam_CR                           "CParamCR"
#define MDVarParam_GLMax                        "CParamGLMax"
#define MDVarParam_LPMax                        "CParamLPMax"
#define MDVarParam_Z0g                          "CParamZ0g"
#define MDVarParam_WetDaysAlpha                 "WetDaysAlpha"
#define MDVarParam_WetDaysBeta                  "WetDaysBeta"

// Hardwired constants
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

int MDAux_AccumBalanceDef ();
int MDAux_AccumEvapDef ();
int MDAux_AccumGrdWatChgDef ();
int MDAux_AccumPrecipDef ();
int MDAux_AccumRunoffDef ();
int MDAux_AccumSMoistChgDef ();
int MDAux_AvgNStepsDef ();
int MDAux_MeanDiscargehDef ();
int MDAux_MeanRunoffDef ();

int MDCommon_CloudCoverDef ();
int MDCommon_GrossRadDef ();
int MDCommon_HumidityRelativeDef ();
int MDCommon_HumiditySpecificDef ();
int MDCommon_PrecipitationDef ();
int MDCommon_SolarRadDef ();
int MDCommon_SolarRadDayLengthDef ();
int MDCommon_SolarRadI0HDayDef ();
int MDCommon_WetBulbTempDef ();
int MDCommon_WetDaysDef ();

int MDCore_BaseFlowDef ();
int MDCore_EvapotranspirationDef ();
int MDCore_RainInfiltrationDef ();
int MDCore_RainInterceptDef ();
int MDCore_RainPotETDef ();
int MDCore_RainPotETHamonDef ();
int MDCore_RainPotETJensenDef ();
int MDCore_RainPotETPstdDef ();
int MDCore_RainPotETPsTaylorDef ();
int MDCore_RainPotETPMdayDef ();
int MDCore_RainPotETPMdnDef ();
int MDCore_RainPotETSWGdayDef ();
int MDCore_RainPotETSWGdnDef ();
int MDCore_RainPotETTurcDef ();
int MDCore_RainSMoistChgDef ();
int MDCore_RainSurfRunoffDef ();
int MDCore_RainWaterSurplusDef ();
int MDCore_RunoffDef ();
int MDCore_RunoffVolumeDef ();
int MDCore_SnowPackChgDef ();
int MDCore_SnowPackMeltDef ();
int MDCore_SoilAvailWaterCapDef ();
int MDCore_SoilMoistChgDef ();
int MDCore_SurfRunoffDef ();
int MDCore_WaterBalanceDef ();

int MDIrrigation_IrrAreaDef ();
int MDIrrigation_GrossDemandDef ();
int MDIrrigation_ReferenceETDef ();
int MDIrrigation_ReferenceETFAODef ();
int MDIrrigation_Reference_ETHamonDef ();
int MDIrrigation_RunoffDef ();
int MDIrrigation_ReturnFlowDef ();
int MDIrrigation_SoilMoistDef ();
int MDIrrigation_SoilMoistChgDef ();
int MDIrrigation_UptakeRiverDef ();
int MDIrrigation_UptakeGrdWaterDef ();

int MDParam_LCAlbedoDef ();
int MDParam_LCHeightDef ();
int MDParam_LCLeafWidthDef ();
int MDParam_LCRSSDef ();
int MDParam_LCR5Def ();
int MDParam_LCCDDef ();
int MDParam_LCCRDef ();
int MDParam_LCGLMaxDef ();
int MDParam_LCLPMaxDef ();
int MDParam_LCZ0gDef ();
int MDParam_LandCoverMappingDef ();
int MDParam_LeafAreaIndexDef ();
int MDParam_LCStemAreaIndexDef ();

int MDRouting_DischargeDef ();
int MDRouting_DischargeInChannelDef ();
int MDRouting_DischargeInChannelAccumulateDef ();
int MDRouting_DischargeInChannelCascadeDef ();
int MDRouting_DischargeInChannelMuskingumDef ();
int MDRouting_DischargeInChannelMuskingumCoeffDef ();
int MDRouting_DischargeReleaseDef ();
int MDRouting_DischargeUptake ();
int MDRouting_RiverbedShapeExponentDef ();
int MDRouting_RiverWidthDef ();

int MDReservoir_OperationDef ();
int MDReservoir_FarmPondReleaseDef ();
int MDReservoir_FarmPondCapacityDef ();

int MDSediment_BQARTpreprocessDef (); //NEW !!!!!  (S.C)
int MDSediment_BankfullQcalcDef ();//NEW !!!!!  (S.C)
int MDSediment_BedloadFluxDef ();//NEW !!!!!  (S.C)
int MDSediment_FluxDef (); //NEW !!!!!  (S.C)
int MDSediment_DischargeBFDef ();

int MDTP2M_WTempGrdWaterDef ();
int MDTP2M_WTempRiverDef ();
int MDTP2M_WTempRiverRouteDef ();
int MDTP2M_ThermalInputsDef ();  //changed AM 042415
int MDTP2M_WTempSurfRunoffDef();

int MDCore_RainWaterSurplusDef ();
/* PET & Related Functions */

float MDSRadNETLong (float, float, float, float);

float MDPETlibVPressSat (float);
float MDPETlibVPressDelta (float);

float MDWindAdjustment (float, float, float, float, float, float);

float MDPETlibLeafAreaIndex (float, float);
float MDPETlibSteamAreaIndex (float, float);
float MDPETlibRoughnessClosed (float, float);
float MDPETlibRoughness (float, float, float, float, float);
float MDPETlibZPDisplacement (float, float, float, float);
float MDPETlibCanopySurfResistance (float, float, float, float, float, float, float, float, float);
float MDPETlibBoundaryResistance (float, float, float, float, float, float, float);
float MDPETlibLeafResistance (float, float, float, float, float, float, float, float);
float MDPETlibGroundResistance (float, float, float, float, float, float, float);
float MDPETlibPenmanMontieth (float, float, float, float, float);
float MDPETlibShuttleworthWallace (float, float, float, float, float, float, float, float, float);

#if defined(__cplusplus)
}
#endif

#endif /* MD_H_INCLUDED */
