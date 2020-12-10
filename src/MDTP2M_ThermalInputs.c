/******************************************************************************
GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2007, University of New Hampshire

MDThermalInputs3.c

amiara@ccny.cuny.edu & rob.stewart@unh.edu

Thermal Inputs, withdrawals, and energy of thermoelectric plans (August 2016)

*******************************************************************************/


#include <string.h>
#include <math.h>
#include <MF.h>
#include <MD.h>

//NEW:
static int _MDInCWA_DeltaID              = MFUnset;   // TODO putin
static int _MDInCWA_LimitID              = MFUnset;   // TODO putin
static int _MDInCWA_OnOffID              = MFUnset;   // TODO putin
static int _MDInDownstream_OnOffID       = MFUnset;   // TODO putin
static int _MDInCWA_316b_OnOffID         = MFUnset;   // TODO putin

// Input
static int _MDInRouting_DischargeID      = MFUnset;
static int _MDInDischargeIncomingID      = MFUnset;
static int _MDFlux_QxTID                 = MFUnset;
static int _MDFluxMixing_QxTID           = MFUnset;
static int _MDPlaceHolderID              = MFUnset;

static int _MDInNamePlate1ID             = MFUnset;
static int _MDInFuelType1ID              = MFUnset;
static int _MDInTechnology1ID            = MFUnset;
static int _MDInEfficiency1ID            = MFUnset;
static int _MDInLakeOcean1ID             = MFUnset;  // there may be more of these
static int _MDInDemand1ID                = MFUnset;

static int _MDInNamePlate2ID             = MFUnset;
static int _MDInFuelType2ID              = MFUnset;
static int _MDInTechnology2ID            = MFUnset;
static int _MDInEfficiency2ID            = MFUnset;
static int _MDInDemand2ID                = MFUnset;

static int _MDInNamePlate3ID             = MFUnset;
static int _MDInFuelType3ID              = MFUnset;
static int _MDInTechnology3ID            = MFUnset;
static int _MDInEfficiency3ID            = MFUnset;
static int _MDInDemand3ID                = MFUnset;

static int _MDInNamePlate4ID             = MFUnset;
static int _MDInFuelType4ID              = MFUnset;
static int _MDInTechnology4ID            = MFUnset;
static int _MDInEfficiency4ID            = MFUnset;
static int _MDInDemand4ID                = MFUnset;

static int _MDInWetBulbTempID            = MFUnset;
static int _MDInCommon_AirTemperatureID	 = MFUnset;

// Output
static int _MDWTemp_QxTID                = MFUnset;
static int _MDOutRouting_DischargeID     = MFUnset; // Late-night discharge test
static int _MDOutAvgEfficiencyID	     = MFUnset;
static int _MDOutAvgDeltaTempID		     = MFUnset;
static int _MDOutTotalEvaporationID	     = MFUnset;
static int _MDOutTotalThermalWdlsID	     = MFUnset;
static int _MDOutTotalOptThermalWdlsID   = MFUnset;
static int _MDOutTotalEnergyDemandID     = MFUnset;
static int _MDOutPowerOutput1ID	 	     = MFUnset;
static int _MDOutPowerDeficit1ID	     = MFUnset;
static int _MDOutPowerPercent1ID	     = MFUnset;
static int _MDOutPowerPercentTotalID     = MFUnset;
static int _MDOutLHFractID			     = MFUnset;
static int _MDOutLHFractPostID		     = MFUnset;
static int _MDOutQpp1ID				     = MFUnset;
static int _MDOutOptQO1ID			     = MFUnset;
static int _MDOutPowerDeficitTotalID     = MFUnset;
static int _MDOutPowerOutputTotalID	     = MFUnset;
static int _MDOutPowerOutputTotal1ID     = MFUnset;
static int _MDOutPowerOutputTotal2ID     = MFUnset;
static int _MDOutPowerOutputTotal3ID     = MFUnset;
static int _MDOutPowerOutputTotal4ID     = MFUnset;

static int _MDOutGenerationID            = MFUnset;
static int _MDOutGeneration1ID           = MFUnset;
static int _MDOutGeneration2ID           = MFUnset;
static int _MDOutGeneration3ID           = MFUnset;
static int _MDOutGeneration4ID           = MFUnset;


static int _MDOutTotalReturnFlowID       = MFUnset;
static int _MDOutTotalExternalWaterID    = MFUnset;

static int _MDOutTotalHeatToOceanID      = MFUnset;          // added 010412
static int _MDOutTotalHeatToRivID        = MFUnset;
static int _MDOutTotalHeatToSinkID       = MFUnset;
static int _MDOutTotalHeatToEngID        = MFUnset;
static int _MDOutTotalHeatToElecID       = MFUnset;
static int _MDOutTotalHeatToEvapID       = MFUnset;
static int _MDOutCondenserInletID	     = MFUnset;           // added 122112
static int _MDOutCondenserInlet1ID       = MFUnset;           // added 122112
static int _MDOutCondenserInlet2ID       = MFUnset;           // added 122112
static int _MDOutCondenserInlet3ID       = MFUnset;           // added 122112
static int _MDOutCondenserInlet4ID       = MFUnset;           // added 122112
static int _MDOutSimEfficiencyID         = MFUnset;           // added 122112
static int _MDOutTotalHoursRunID         = MFUnset;           // added 030212
static int _MDOutLossToInletID	         = MFUnset;
static int _MDOutLossToWaterID           = MFUnset; 

static int _MDOutLossToInlet1ID          = MFUnset;           // added 122112
static int _MDOutLossToInlet2ID          = MFUnset;           // added 122112
static int _MDOutLossToInlet3ID          = MFUnset;           // added 122112
static int _MDOutLossToInlet4ID          = MFUnset;           // added 122112

static int _MDOutHeatToRiver1ID          = MFUnset;           // added 122112
static int _MDOutHeatToRiver2ID          = MFUnset;           // added 122112
static int _MDOutHeatToRiver3ID          = MFUnset;           // added 122112
static int _MDOutHeatToRiver4ID          = MFUnset;           // added 122112


static void _MDThermalInputs3 (int itemID) {
    float loss_inlet_1               = 0.0;
    float loss_inlet_2               = 0.0;
    float loss_inlet_3               = 0.0;
    float loss_inlet_4               = 0.0;
    float loss_water_1               = 0.0;
    float loss_water_2               = 0.0;
    float loss_water_3               = 0.0;
    float loss_water_4               = 0.0;
    float loss_inlet_total           = 0.0;
    float loss_water_total           = 0.0;
    float heat_to_river_1            = 0.0;
    float heat_to_river_2            = 0.0;
    float heat_to_river_3            = 0.0;
    float heat_to_river_4            = 0.0;
    //NEW//
    float Cp                         = 4.18; //heat capacity
    float itd                        = 12.0; //inlet temperature difference for AIR cooled towers
    float cycles                     = 5.0; // cycles in the tower
    float latent                     = 2264.76; // latent heat of vaporization MJ/m3
    float evap_fraction              = 0.85; // frcation of totoal heat rejected by latent heat transfer - is 0.9 in california report (see Miara & Vorosmarty 2013)
    float min_discharge              = 0.3; // fraction to be left in river
    float inlet_temp_thresh          = 10.0; // temperature above which there is an efficiency hit
    float inlet_temp_thresh_2        = 20.0;
    float inlet_temp                 = 0.0; // to be set later
    float max_river_temp             = 0.0; // to be set later
    float air_inlet_temp_thresh      = 20.0; // temperature above which there is an efficiency hit for the turbine part of ngcc
    float Approach                   = 5.55;
    float efficiency                 = 0.0; // to be set later
    float nameplate_1                = 0.0; // to be set later
    float fuel_type_1                = 0.0; // to be set later
    float technology_1               = 0.0; // to be set later
    float opt_efficiency_1           = 0.0; // to be set later
    float total_withdrawal           = 0.0;
    float nameplate_2                = 0.0; // to be set later
    float fuel_type_2                = 0.0; // to be set later
    float technology_2               = 0.0; // to be set later
    float opt_efficiency_2           = 0.0; // to be set later

    float nameplate_3                = 0.0; // to be set later
    float fuel_type_3                = 0.0; // to be set later
    float technology_3               = 0.0; // to be set later
    float opt_efficiency_3           = 0.0; // to be set later

    float nameplate_4                = 0.0; // to be set later
    float fuel_type_4                = 0.0; // to be set later
    float technology_4               = 0.0; // to be set later
    float opt_efficiency_4           = 0.0; // to be set later

    float nameplate                  = 0.0; // to be set later
    float fuel_type                  = 0.0; // to be set later
    float technology                 = 0.0; // to be set later
    float opt_efficiency             = 0.0; // to be set later

    float opt_deltaT                 = 0.0; // to be set later
    float deltaT                     = 0.0; // to be set later
    float withdrawal                 = 0.0; // to be set later
    float opt_withdrawal             = 0.0; // to be set later
    float available_discharge        = 0.0; // to be set later
    float CWA_limit                  = 0.0;
    float CWA_delta                  = 0.0;
    float CWA_onoff	                 = 0.0;	    
    float post_temperature           = 0.0;
    float operational_capacity       = 0.0;
    float operational_capacity_1     = 0.0;
    float operational_capacity_2     = 0.0;
    float operational_capacity_3     = 0.0;
    float operational_capacity_4     = 0.0;
    float efficiency_hit             = 0.0;
    float opt_heat_out               = 0.0;
    float opt_gas_efficiency         = 0.0;
    float opt_steam_efficiency       = 0.0;
    float gas_efficiency             = 0.0;
    float steam_efficiency           = 0.0;
    float max_heat_cond              = 0.0;
    float max_deltaT                 = 0.0;
    // other heat sink (through flue)
    float sink_bio                   = 0.12;
    float sink_coal                  = 0.12;
    float sink_ngcc                  = 0.2;
    float sink_nuc                   = 0.0;
    float sink_ogs                   = 0.12;
    float sink_oth                   = 0.12;
    float heat_sink                  = 0.0; // to be set later
    // condesner requirements - assumed as withdrawal for once-through
    float cond_bio                   = 35000.0;
    float cond_coal                  = 36350.0;
    float cond_ngcc                  = 11380.0;
    float cond_nuc                   = 44350.0;
    float cond_ogs                   = 35000.0;
    float cond_oth                   = 35450.0;
    float cond                       = 0.0; // to be set later
    //consumption for once-through2
    float cons_bio                   = 300.0;
    float cons_coal                  = 250.0;
    float cons_ngcc                  = 240.0;
    float cons_nuc                   = 269.0;
    float cons_ogs                   = 300.0;
    float cons_oth                   = 279.75;
    float opt_consumption            = 0.0; // to be set later
    //make up for wet tower
    float mup_bio                    = 878.0;
    float mup_coal                   = 1005.0;
    float mup_ngcc                   = 253.0;
    float mup_nuc                    = 1101.0;
    float mup_ogs                    = 1203.0;
    float mup_oth                    = 1046.75;
    float make_up                    = 0.0; // to be set later
    float opt_make_up                = 0.0; // to be set later
    // climate variables
    float air_temp                   = 0.0; //
    float wet_b_temp                 = 0.0; //
    float river_temp                 = 0.0; //
    float discharge                  = 0.0; //

    float opt_heat_cond              = 0.0; //
    float consumption                = 0.0; //
    float heat_allowed               = 0.0; //
    float heat_cond                  = 0.0; //
    float river_temp_initial         = 0.0;
    float a                          = 0.0;
    float adj_dummy                  = 0.0;
    float alpha                      = 0.0;
    float avgDaily_eff_dTemp_1       = 0.0;
    float avgDaily_efficiency_1      = 0.0;
    float b                          = 0.0;
    float blowdown_1                 = 0.0;
    float bypass_percent             = 0.0;
    float bypass_Q                   = 0.0;
    float deltaT_1                   = 0.0; // temperature difference between intake temperature and outlet temperature (C)
    float dTemp_1                    = 0.0;
    float dT_max_1                   = 0.0;
    float Efficiency_1               = 0.0; // 1st Powerplant's Efficiency
    float Eff_loss_1                 = 0.0;
    float eff_gas                    = 0.0;
    float eff_steam                  = 0.0;
    float energyDemand_1             = 0.0; // 1st powerplant demand MWhrs/day
    float evaporation_1              = 0.0;
    float flux_QxT                   = 0.0;
    float flux_QxT_new               = 0.0;
    float fluxmixing_QxT             = 0.0;
    float FuelType_1                 = 0.0; // 1st Powerplant's fueltype in grid cell  (Biomass = 1, Coal = 2, Natural Gas = 3, Nuclear = 4, Oil = 5, Other = 6)
    float LakeOcean                  = 0.0;
    float LH_fract                   = 0.0; // constant (was 0.9)
    float LH_fract_post              = 0.0;
    float NamePlate_1                = 0.0; // 1st Powerplant's nameplate in grid cell (MW)
    float optDeltaT_1                = 0.0; // 1st Powerplant's deltaT
    float opt_QO_1                   = 0.0;
    float output_1                   = 0.0;
    float OT_effluent_1              = 0.0;
    float placeHolder                = 0.0;
    float pot_WTemp_1                = 0.0;
    float Q                          = 0.0; // discharge (m3/s)
    float Q_incoming_1               = 0.0;
    float Q_in_1                     = 0.0;
    float Q_outgoing_1               = 0.0; // discharge in grid cell post power plant evaporation
    float Q_outgoing_WTemp_1         = 0.0; // water temperature post-powerplant with evaporation included (deg C)
    float Qpp_1                      = 0.0;
    float Q_WTemp                    = 0.0; // water temperature pre-powerplant(deg C)
    float riverThresh_temp           = 0.0;
    float State                      = 0.0;
    float Technology_1               = 0.0; // 1st Powerplant's Technology (Once thru = 1, Cooling tower = 2, Dry Cooling = 3, CC with OT = 4.1, CC with CT = 4.2, CC with DC = 4.3, Hybrid = 5)
    float Temp_In_1                  = 0.0;
    float TempLimit                  = 0.0;
    float Tpp_1                      = 0.0;
    float totalDaily_blowdown_1      = 0.0;
    float totalDaily_deficit_1       = 0.0;
    float totalDaily_demand_1        = 0.0;
    float totalDaily_evap_1	         = 0.0;
    float totalDaily_OTeffluent_1    = 0.0;
    float totalDaily_output_1        = 0.0; // 1st powerplant output MWhrs/day
    float totalDaily_returnflow_1    = 0.0;
    float totalDaily_percent_1       = 0.0;
    float totalDaily_target_wdl_1    = 0.0;
    float totalDaily_wdl_1           = 0.0;
    float totalHours_run_1           = 0.0;
    float wdl_1                      = 0.0;
    float wdl_coeff_1                = 0.0;
    float wetBulbT                   = 0.0;
    float drybulbT                   = 0.0;
    float airT                       = 0.0;
    float output_2                   = 0.0;
    float DC_cond_heat_loss          = 0.0;
    float totalDaily_heatToRiv       = 0.0;
    float totalDaily_heatToSink      = 0.0;
    float totalDaily_heatToEng       = 0.0;
    float eff_steam_loss1            = 0.0;
    float heatBalance1               = 0.0;
    float heatBalance2               = 0.0;
    float flux_GJ_new			     = 0.0;
    float flux_GJ_old			     = 0.0;
    float flux_GJ_old_wevap		     = 0.0;
    float totalDaily_heat		     = 0.0;
    float totalDaily_elec		     = 0.0;
    float flux_GJ_old_prist		     = 0.0;
    float non_operatingHrs_heat	     = 0.0;
    float operatingHrs_heat		     = 0.0;
    float total_cell_flux		     = 0.0;
    float pHB2					     = 0.0;
    float flux_GJ_old_prist2	     = 0.0;
    float heatBalance3			     = 0.0;
    float pHB3					     = 0.0;
    float totalDaily_heatToElec	     = 0.0;
    float totalGJ_heatToEng		     = 0.0;
    float totalGJ_heatToSink	     = 0.0;
    float totalGJ_heatToRiv		     = 0.0;
    float totalGJ_heatToElec	     = 0.0;
    float evaporated_heat		     = 0.0;
    float totalGJ_heatToEvap         = 0.0;
    float cell                       = 33;
    float totalGJ_fluxNEW            = 0.0;
    float totalGJ_fluxOLD            = 0.0;
    float waterT_heatToRiv           = 0.0;
    float percent_wTheatToRiv        = 0.0;
    float Q_check                    = 0.0;
    float evaporation_2              = 0.0;
    float totalDaily_evap_2          = 0.0;
    float totalDaily_cons_2          = 0.0;
    float totalDaily_external_2      = 0.0;
    float totalDaily_heatToOcean     = 0.0;
    float Eff_loss_2                 = 0.0;
    float condenser_inlet            = 0.0;
    float energy_Pen                 = 0.0;
    float year                       = 0.0;
    float exp_adj                    = 0.0;
    float blowdown                   = 0.0;
    float vap_fraction               = 0.85;
    float heat_in                    = 0.0;
    float desired_make_up            = 0.0;
    float desired_withdrawal         = 0.0;
    float desired_deltaT             = 0.0;
    float desired_heat_cond          = 0.0;
    float desired_consumption        = 0.0;
    float desired_blowdown           = 0.0;
    float max_make_up                = 0.0;
    float i			                 = 1.0;
    float m			                 = 0.0;
    float consumption_T	             = 0.0;
    float operational_gas_capacity	 = 0.0;
    float operational_steam_capacity = 0.0;
    float steam_heat_in	             = 0.0;
    float inlet_temp_1               = 0.0;
    float inlet_temp_2               = 0.0;
    float inlet_temp_3               = 0.0;
    float inlet_temp_4               = 0.0;
    float demand                     = 0.0;
    float demand_1                   = 0.0;
    float demand_2                   = 0.0;
    float demand_3                   = 0.0;
    float demand_4                   = 0.0;
    float generation                 = 0.0;
    float generation_1               = 0.0;
    float generation_2               = 0.0;
    float generation_3               = 0.0;
    float generation_4               = 0.0;
    float op_hours                   = 0.0;

    float day_discharge              = 0.0;
    float day_total_withdrawal       = 0.0;
    float day_withdrawal             = 0.0;
    float day_consumption            = 0.0;
    float day_post_temperature       = 0.0;

    float withdrawal_T               = 0.0;
    float total_withdrawal_T         = 0.0;
    float heat_to_river_T            = 0.0; 
    float heat_to_river              = 0.0;
    float eff_volume                 = 0.0;
    float eff_temperature            = 0.0;
    float discharge_T                = 0.0;
    float Downstream_onoff           = 0.0;
    float CWA_316b_OnOff             = 0.0;
    float cccc                       = 0.0;

    placeHolder      = MFVarGetFloat (_MDPlaceHolderID,             itemID, 0.0); // running MDWTempRiverRoute, value is previous water Temp
    flux_QxT         = MFVarGetFloat (_MDFlux_QxTID,                itemID, 0.0); // reading in discharge * temp (m3*degC/day)
    fluxmixing_QxT   = MFVarGetFloat (_MDFluxMixing_QxTID,          itemID, 0.0); // reading in discharge * tempmixing (m3*degC/day)
    Q = Q_incoming_1 = MFVarGetFloat (_MDInRouting_DischargeID,     itemID, 0.0);
    air_temp         = MFVarGetFloat (_MDInCommon_AirTemperatureID, itemID, 0.0); //read in air temperature (c)
    nameplate_1      = MFVarGetFloat (_MDInNamePlate1ID,            itemID, 0.0); //todo check
    fuel_type_1      = MFVarGetFloat (_MDInFuelType1ID,             itemID, 0.0);
    technology_1     = MFVarGetFloat (_MDInTechnology1ID,           itemID, 0.0);
    opt_efficiency_1 = MFVarGetFloat (_MDInEfficiency1ID,           itemID, 0.0);
    nameplate_2      = MFVarGetFloat (_MDInNamePlate2ID,            itemID, 0.0); //todo check
    fuel_type_2      = MFVarGetFloat (_MDInFuelType2ID,             itemID, 0.0);        
    technology_2     = MFVarGetFloat (_MDInTechnology2ID,           itemID, 0.0);
    opt_efficiency_2 = MFVarGetFloat (_MDInEfficiency2ID,           itemID, 0.0);
    nameplate_3      = MFVarGetFloat (_MDInNamePlate3ID,            itemID, 0.0); //todo check
    fuel_type_3      = MFVarGetFloat (_MDInFuelType3ID,             itemID, 0.0);        
    technology_3     = MFVarGetFloat (_MDInTechnology3ID,           itemID, 0.0);
    opt_efficiency_3 = MFVarGetFloat (_MDInEfficiency3ID,           itemID, 0.0);
    nameplate_4      = MFVarGetFloat (_MDInNamePlate4ID,            itemID, 0.0); //todo check
    fuel_type_4      = MFVarGetFloat (_MDInFuelType4ID,             itemID, 0.0);        
    technology_4     = MFVarGetFloat (_MDInTechnology4ID,           itemID, 0.0);
    opt_efficiency_4 = MFVarGetFloat (_MDInEfficiency4ID,           itemID, 0.0);
    CWA_316b_OnOff   = MFVarGetFloat (_MDInCWA_316b_OnOffID,        itemID, 0.0);

    /// TPNEW ///
    demand_1         = MFVarGetFloat (_MDInDemand1ID,               itemID, 0.0); //todo check
    demand_2         = MFVarGetFloat (_MDInDemand2ID,               itemID, 0.0); //todo check
    demand_3         = MFVarGetFloat (_MDInDemand3ID,               itemID, 0.0); //todo check
    demand_4         = MFVarGetFloat (_MDInDemand4ID,               itemID, 0.0); //todo check
    //////////// SECTION 316b scenario ///////////////////////
    if ((CWA_316b_OnOff > 0.5) && (CWA_316b_OnOff < 1.5)) {
        nameplate_1 = ((technology_1 == 1 || technology_1 == 4) && nameplate_1 > 0) ? nameplate_1 * 0.98 : nameplate_1;
        nameplate_2 = ((technology_2 == 1 || technology_2 == 4) && nameplate_2 > 0) ? nameplate_2 * 0.98 : nameplate_2;
        nameplate_3 = ((technology_3 == 1 || technology_3 == 4) && nameplate_3 > 0) ? nameplate_3 * 0.98 : nameplate_3;
        nameplate_4 = ((technology_4 == 1 || technology_4 == 4) && nameplate_4 > 0) ? nameplate_4 * 0.98 : nameplate_4;
        technology_1 = (technology_1 == 1) ? 2.0 : technology_1;
        technology_1 = (technology_1 == 4) ? 5.0 : technology_1;
        technology_2 = (technology_2 == 1) ? 2.0 : technology_2;
        technology_2 = (technology_2 == 4) ? 5.0 : technology_2;
        technology_3 = (technology_3 == 1) ? 2.0 : technology_3;
        technology_3 = (technology_3 == 4) ? 5.0 : technology_3;
        technology_4 = (technology_4 == 1) ? 2.0 : technology_4;
        technology_4 = (technology_4 == 4) ? 5.0 : technology_4;
    }

    //////////// Cooling Towers no active ///////////////////////
    if ((CWA_316b_OnOff > 1.5) && (CWA_316b_OnOff < 2.5)) {
        if ((technology_1 == 2) || (technology_1 == 5)) demand_1 = 0;
        if ((technology_2 == 2) || (technology_2 == 5)) demand_2 = 0;
        if ((technology_3 == 2) || (technology_3 == 5)) demand_3 = 0;
        if ((technology_4 == 2) || (technology_4 == 5)) demand_4 = 0;
    }

    ///////////// IF THERE ARE ERRORS IN INPUT DATA:

/*  if (((technology_2 == 2) || (technology_2 == 5)) && ((technology_1 == 1) || (technology_1 == 4))) {
        printf("TECH ERROR \n");
        printf("nameplate_1 = %f, fuel_type_1 = %f, technology_1 = %f, nameplate_2 = %f, fuel_type_2 = %f, technology_2 = %f \n", nameplate_1, fuel_type_1, technology_1, nameplate_2, fuel_type_2, technology_2);
    }

    if ((demand_1 > nameplate_1*24*1.05) || (demand_2 > nameplate_2*24*1.05) || (demand_3 > nameplate_3*24*1.05) || (demand_4 > nameplate_4*24*1.05)) {
        printf("DEMAND ERROR \n");
        printf("demand_1 = %f, nameplate_1 = %f, fuel_type_1 = %f, technology_1 = %f, nameplate_2 = %f, fuel_type_2 = %f, technology_2 = %f, demand_2 = %f \n", demand_1, nameplate_1, fuel_type_1, technology_1, nameplate_2, fuel_type_2, technology_2, demand_2);
        printf("demand_3 = %f, nameplate_3 = %f, fuel_type_3 = %f, technology_3 = %f, nameplate_4 = %f, fuel_type_4 = %f, technology_4 = %f, demand_4 = %f \n", demand_3, nameplate_3, fuel_type_3, technology_3, nameplate_4, fuel_type_4, technology_4, demand_4);
    }
*/

    ///////////
    opt_efficiency_1 = opt_efficiency_1 / 100;
    opt_efficiency_2 = opt_efficiency_2 / 100;
    opt_efficiency_3 = opt_efficiency_3 / 100;
    opt_efficiency_4 = opt_efficiency_4 / 100;

    // 	energyDemand_1      = MFVarGetFloat (_MDInEnergyDemand1ID,      itemID, 0.0);
    drybulbT	     = airT;
    wet_b_temp	     = MFVarGetFloat (_MDInWetBulbTempID,      itemID, 0.0);
    LakeOcean        = MFVarGetFloat (_MDInLakeOcean1ID,       itemID, 0.0);		// 1 is lakeOcean, 0 is nothing
    CWA_limit        = MFVarGetFloat (_MDInCWA_LimitID,        itemID, 0.0);
    CWA_delta        = MFVarGetFloat (_MDInCWA_DeltaID,        itemID, 0.0);
    CWA_onoff	     = MFVarGetFloat (_MDInCWA_OnOffID,        itemID, 0.0);
    Downstream_onoff = MFVarGetFloat (_MDInDownstream_OnOffID, itemID, 0.0);
    discharge = Q;
    day_discharge = Q * 86400;

    //post_temperature = flux_QxT / (discharge * 86400); //for setting initial temperature when there are multiple plants

    river_temp_initial  = (flux_QxT < 0.00001) ? 0.0 : flux_QxT / (Q * 86400);
    CWA_limit           = (CWA_limit < 25.0) ? 32.0 : CWA_limit;
    CWA_delta           = (CWA_delta < 1.0) ? 3.0 : CWA_delta;
    max_river_temp      = (CWA_onoff < 0.5) ? 99 : 99; //NO CWA
    if (CWA_onoff > 0.5) {
        max_river_temp      = (CWA_limit > (CWA_delta+river_temp_initial)) ? CWA_limit : ( CWA_delta + river_temp_initial ); //CWA DELTA ENFORCEMENT (i.e. variance permits to an extent)
    }
    max_river_temp      = (CWA_onoff > 1.5) ? CWA_limit : max_river_temp; //STRICT CWA (removal of variance permits)

    /****************************************************/

	wet_b_temp = (wet_b_temp > 0.0) ? wet_b_temp : 1.0;
    air_temp = (air_temp > 0.0) ? air_temp : 1.0;

    /****************************************************/

	i = (nameplate_2 > 0.0) ? 2.0 : i;
    i = (nameplate_3 > 0.0) ? 3.0 : i;
    i = (nameplate_4 > 0.0) ? 4.0 : i;

    /****************************************************/

    Q_check = ( (LakeOcean > 0.5) || (technology_1 == 3) || (technology_2 == 3) || (technology_3 == 3) || (technology_4 == 3) || (technology_1 == 6) || (technology_2 == 6) || (technology_3 == 6) || (technology_4 == 6) )? 1.0 : Q;

    if (Q_check > 0.000001) {
        Q_WTemp = (Q <= 0.000001) ? Q_WTemp : flux_QxT / (Q * 86400);                      // degC RJS 013112
        if ((nameplate_1 + nameplate_2 + nameplate_3 + nameplate_4) > 0) {
    	for ( m = 1; m < ( i + 1 ); m = m + 1 ) {
            if (m == 1) fuel_type = fuel_type_1;
            if (m == 1) technology = technology_1;
            if (m == 1) opt_efficiency = opt_efficiency_1;
            if (m == 1) nameplate = nameplate_1;
	        if (m == 1) demand = demand_1;

            if (m == 2) fuel_type = fuel_type_2;
            if (m == 2) technology = technology_2;
            if (m == 2) opt_efficiency = opt_efficiency_2;
            if (m == 2) nameplate = nameplate_2;
	        if (m == 2) demand = demand_2;

            if (m == 3) fuel_type = fuel_type_3;
            if (m == 3) technology = technology_3;
            if (m == 3) opt_efficiency = opt_efficiency_3;
            if (m == 3) nameplate = nameplate_3;
	        if (m == 3) demand = demand_3;

            if (m == 4) fuel_type = fuel_type_4;
            if (m == 4) technology = technology_4;
            if (m == 4) opt_efficiency = opt_efficiency_4;
            if (m == 4) nameplate = nameplate_4;
        	if (m == 4) demand = demand_4;

            if (LakeOcean < 0.5)  LakeOcean = 0.0 ;                         // no TODO
            if (LakeOcean > 0.5)  LakeOcean = 1.0 ;                         // lake

            if (fuel_type == 1)   cond = ((cond_bio   * 0.0037854) / 3600) * nameplate; // converts gallons to m3. Units are m3/s
            if (fuel_type == 2)   cond = ((cond_coal  * 0.0037854) / 3600) * nameplate;
            if (fuel_type == 3)   cond = ((cond_ngcc  * 0.0037854) / 3600) * nameplate;
            if (fuel_type == 4)   cond = ((cond_nuc   * 0.0037854) / 3600) * nameplate;
            if (fuel_type == 5)   cond = ((cond_ogs   * 0.0037854) / 3600) * nameplate;
            if (fuel_type == 6)   cond = ((cond_oth   * 0.0037854) / 3600) * nameplate;

            if (fuel_type == 1)   opt_make_up = ((mup_bio   * 0.0037854) / 3600) * nameplate; // converts gallons to m3. Units are m3/s
            if (fuel_type == 2)   opt_make_up = ((mup_coal  * 0.0037854) / 3600) * nameplate;
            if (fuel_type == 3)   opt_make_up = ((mup_ngcc  * 0.0037854) / 3600) * nameplate;
            if (fuel_type == 4)   opt_make_up = ((mup_nuc   * 0.0037854) / 3600) * nameplate;
            if (fuel_type == 5)   opt_make_up = ((mup_ogs   * 0.0037854) / 3600) * nameplate;
            if (fuel_type == 6)   opt_make_up = ((mup_oth   * 0.0037854) / 3600) * nameplate;
 
            if (fuel_type == 1)   opt_consumption = ((cons_bio   * 0.0037854) / 3600) * nameplate; // converts gallons to m3. Units are m3/s
            if (fuel_type == 2)   opt_consumption = ((cons_coal  * 0.0037854) / 3600) * nameplate;
            if (fuel_type == 3)   opt_consumption = ((cons_ngcc  * 0.0037854) / 3600) * nameplate;
            if (fuel_type == 4)   opt_consumption = ((cons_nuc   * 0.0037854) / 3600) * nameplate;
            if (fuel_type == 5)   opt_consumption = ((cons_ogs   * 0.0037854) / 3600) * nameplate;
            if (fuel_type == 6)   opt_consumption = ((cons_oth   * 0.0037854) / 3600) * nameplate;

            if (fuel_type == 1)   heat_sink = sink_bio;
            if (fuel_type == 2)   heat_sink = sink_coal;
            if (fuel_type == 3)   heat_sink = sink_ngcc;
            if (fuel_type == 4)   heat_sink = sink_nuc;
            if (fuel_type == 5)   heat_sink = sink_ogs;
            if (fuel_type == 6)   heat_sink = sink_oth;

	        if ((technology == 4) || (technology == 5))	opt_make_up = ((mup_ngcc  * 0.0037854) / 3600) * nameplate;
            if ((technology == 4) || (technology == 5))     cond = ((cond_ngcc  * 0.0037854) / 3600) * nameplate;
            if ((technology == 4) || (technology == 5))	opt_consumption = ((cons_ngcc  * 0.0037854) / 3600) * nameplate; 
            if ((technology == 4) || (technology == 5))     heat_sink = sink_ngcc;

            // NEW STARTS HERE
            if (nameplate > 0 ) opt_heat_cond = ( nameplate / opt_efficiency ) * ( 1.0 - opt_efficiency - heat_sink); // optimal heat out in MJ/s if power plant oeprates at nameplate capacity
            heat_to_river	    = 0.0; //reset heat to river, so doesn't carry over
            discharge 	    = (i > 1.0 ) ? ((day_discharge - day_consumption)/86400) : discharge;
            available_discharge = (i > 1.0 ) ? 0.7 * ((day_discharge - day_consumption)/86400)  : 0.7 * discharge; // new available discharge accounting for consumption in first part of plant
            river_temp          = (day_post_temperature > 0.0 ) ? day_post_temperature : flux_QxT / (discharge * 86400); // river temp accounting for operating hours
            day_discharge 	    = (i > 1.0 ) ? (day_discharge - day_consumption) : 86400 * discharge;
            heat_in             = nameplate / opt_efficiency;

            // There are two options for withdrawal rate, pick one below and edit out the other ----
            // First one is the median withdrawal rate from Macknick et al 2011, so all thats left is to set the deltaT:
            opt_deltaT = opt_heat_cond / ( 4.18 * cond ); // optimal heat increase of cooling water based on given withdrawal rate

            // Second is setting the opt_deltaT and then defininf the withdrawal rate:
            //    opt_deltaT = 10.0;
            //    cond = opt_heat_out / ( 4.18 * opt_deltaT );

            //////////////////// CODE CALCULATIONS //////////////////////////

            /****************************************************  RECIRCULATING START  ****************************************************/

            // CALCS FOR RECIRCULATING STEAM ------

            if ( (nameplate > 0) && (technology == 2.0) ){
                inlet_temp           = wet_b_temp + Approach; // setting the inlet temperature
                efficiency_hit       = ((0.0165 * pow((inlet_temp - inlet_temp_thresh),2)) + (0.1604 * (inlet_temp - inlet_temp_thresh))) / 100; // efficiency hit due to increased inlet temperature
                efficiency           = ( inlet_temp > inlet_temp_thresh_2 ) ? ( opt_efficiency - (efficiency_hit * opt_efficiency)) : opt_efficiency; // accounting for the efficiency hit
                desired_heat_cond    = heat_in - (heat_in * efficiency) - (heat_in * heat_sink);
                desired_consumption  = desired_heat_cond / ( latent / vap_fraction);
                desired_blowdown     = desired_consumption / (cycles - 1.0);
                desired_make_up      = desired_blowdown + desired_consumption;
                max_make_up          = ( discharge * ( max_river_temp - river_temp) ) / ( ( inlet_temp / cycles ) + ( max_river_temp * ( 1.0 - (1.0 / cycles ) ) ) - river_temp );
 	            max_make_up		     = (max_make_up <= 0.0) ? 0.0 : max_make_up;
        	    make_up		         = ( max_make_up >= desired_make_up ) ? desired_make_up : max_make_up;
	            make_up              = ( make_up <= available_discharge ) ? make_up : available_discharge;
                blowdown             = ( 1.0 / cycles) * make_up;
                consumption          = make_up - blowdown;
                heat_cond            = consumption * ( latent / vap_fraction);
                operational_capacity = heat_cond * ( efficiency / ( 1.0 - heat_sink - efficiency ) );   // operational capacity of power plant post everything --- MW
                post_temperature     = ((( discharge - make_up ) * river_temp ) + ( blowdown * inlet_temp )) / ( discharge - consumption); // outlet temperature after mixing deg C
                operational_capacity = (LakeOcean > 0.5) ? heat_in * efficiency : operational_capacity;
                post_temperature     = (LakeOcean > 0.5) ? river_temp : post_temperature;
                total_withdrawal     = make_up;
            } // ------ END OF RECIRCULATING STEAM

            // CALCS FOR ONCE THROUGH STEAM ------

            if ((nameplate > 0) && (technology == 1.0)) {
                opt_withdrawal       = cond;
                inlet_temp           = river_temp_initial; // setting the inlet temperature
                efficiency_hit       = ((0.0165 * pow((inlet_temp - inlet_temp_thresh),2)) + (0.1604 * (inlet_temp - inlet_temp_thresh))) / 100; // efficiency hit due to increased inlet temperature
                efficiency           = ( inlet_temp > inlet_temp_thresh_2 ) ? ( opt_efficiency - (efficiency_hit * opt_efficiency)) : opt_efficiency; // accounting for the efficiency hit
                desired_heat_cond    = heat_in - (heat_in * efficiency) - (heat_in * heat_sink); // assumption here is that heat loss through gas turbine is constant and any increase in losses goes to the condenser side
                desired_withdrawal   = ( desired_heat_cond / ( 4.18 * opt_deltaT ) ) + opt_consumption; // setting withdrawal rate OR user can set desired delta T
                total_withdrawal     = ( available_discharge >= desired_withdrawal) ? desired_withdrawal : available_discharge; // setting the withdrawal rate based on water availablity - m3/s
                consumption          = ( opt_consumption / opt_withdrawal ) * total_withdrawal ; // setting consumption rate based on withdrawal rate -m3/s
                withdrawal           = total_withdrawal - consumption;
                heat_allowed         = ( max_river_temp * ( discharge - consumption ) ) - ( (discharge - total_withdrawal) * river_temp); // heat allowed into river m3-degC/s
                heat_allowed         = ( heat_allowed < 0.0 ) ? 0.0 : heat_allowed; // heat allowed into river m3-degC/s
                max_deltaT           = (heat_allowed > 0 ) ? ( heat_allowed / withdrawal ) - inlet_temp : 0.0 ; // increase of cooling water degC
                max_deltaT           = (max_deltaT > 30.0) ? 30.0 : max_deltaT; // increase of cooling water degC - with a maximum set to 25 degC
                max_deltaT           = (max_deltaT < 0.0) ? 0.0 : max_deltaT; // increase of cooling water degC - with a maximum set to 25 degC
                max_heat_cond        = withdrawal * max_deltaT * 4.18; // heat out of condesner MJ/s
                heat_cond            = ( max_heat_cond >= desired_heat_cond ) ? desired_heat_cond : max_heat_cond;
                deltaT               = heat_cond / ( withdrawal * 4.18 );
                operational_capacity = heat_cond * ( efficiency / ( 1.0 - heat_sink - efficiency ) );   // operational capacity of power plant post everything --- MW
                post_temperature     = ( ( ( discharge - total_withdrawal ) * river_temp ) + ( withdrawal * ( inlet_temp + deltaT ) ) ) / ( discharge - consumption); // outlet temperature after mixing deg C
                operational_capacity = (LakeOcean > 0.5) ? heat_in * efficiency : operational_capacity;
                post_temperature     = (LakeOcean > 0.5) ? river_temp : post_temperature;
            }       // ------ END OF ONCE THROUGH STEAM

            // CALCS FOR RECIRCULATING NGCC ------

            if ( (nameplate > 0) && (technology == 5.0) ){
                opt_steam_efficiency = opt_efficiency * 0.526315789;
                opt_gas_efficiency   = ( opt_efficiency - opt_steam_efficiency ) / ( 1.0 - opt_steam_efficiency);
                inlet_temp           = wet_b_temp + Approach; // setting the inlet temperature
                efficiency_hit       = ((0.0165 * pow((inlet_temp - inlet_temp_thresh),2)) + (0.1604 * (inlet_temp - inlet_temp_thresh))) / 100; // efficiency hit due to increased inlet temperature
                steam_efficiency     = ( inlet_temp > inlet_temp_thresh_2 ) ? ( opt_steam_efficiency - (efficiency_hit*opt_steam_efficiency) ) : opt_steam_efficiency; // steam efficiency accounting for the steam efficiency hit
                gas_efficiency       = ( air_temp > air_inlet_temp_thresh ) ? ( opt_gas_efficiency * ( 1.0 - ( (0.5845 / 100.0) * (air_temp - air_inlet_temp_thresh) ) ) ) : opt_gas_efficiency; // gas efficiency accounting for hit
                efficiency           = steam_efficiency + gas_efficiency - ( steam_efficiency * gas_efficiency ); // final efficiency accounting for gas and steam hits
	            steam_heat_in	     = heat_in - (gas_efficiency * heat_in);
	            desired_heat_cond    = steam_heat_in - (steam_heat_in * steam_efficiency) - (steam_heat_in * heat_sink); // assumption here is that heat loss through gas turbine is constant and any increase in losses goes to the condenser side
                desired_consumption  = desired_heat_cond / ( latent / vap_fraction);
                desired_blowdown     = desired_consumption / (cycles - 1.0);
                desired_make_up      = desired_blowdown + desired_consumption;
                max_make_up          = ( discharge * ( max_river_temp - river_temp) ) / ( ( inlet_temp / cycles ) + ( max_river_temp * ( 1.0 - (1.0 / cycles ) ) ) - river_temp );
                max_make_up          = (max_make_up <= 0.0) ? 0.0 : max_make_up;
                make_up              = ( max_make_up >= desired_make_up ) ? desired_make_up : max_make_up;
                make_up              = ( make_up <= available_discharge ) ? make_up : available_discharge;
                blowdown             = ( 1.0 / cycles) * make_up;
                consumption          = make_up - blowdown;
                heat_cond            = consumption * ( latent / vap_fraction);
                operational_gas_capacity = gas_efficiency * heat_in;
                operational_steam_capacity = heat_cond * ( steam_efficiency / ( 1.0 - heat_sink - steam_efficiency ) );
                operational_capacity = operational_gas_capacity + operational_steam_capacity;   // operational capacity of power plant post everything --- MW
                post_temperature     = ( ( ( discharge - make_up ) * river_temp ) + ( blowdown * inlet_temp ) ) / ( discharge - consumption); // outlet temperature after mixing deg C
	            operational_capacity = (LakeOcean > 0.5) ? heat_in * efficiency : operational_capacity;
	            post_temperature     = (LakeOcean > 0.5) ? river_temp : post_temperature;
                total_withdrawal	 = make_up;
            }   // ------ END OF RECIRCULATING NGCC

            /**************************************************** //    RECIRCULATING END     // ****************************************************/

            /**************************************************** //    ONCE-THROUGH START    // ****************************************************/

            // CALCS FOR ONCE THROUGH NGCC ------

            if ( (nameplate > 0) && (technology == 4.0) ){
                opt_withdrawal       = cond;
                opt_steam_efficiency = opt_efficiency * 0.526315789;
                opt_gas_efficiency   = ( opt_efficiency - opt_steam_efficiency ) / ( 1.0 - opt_steam_efficiency);
                inlet_temp           = river_temp_initial; // setting the inlet temperature
                efficiency_hit       = ((0.0165 * pow((inlet_temp - inlet_temp_thresh),2)) + (0.1604 * (inlet_temp - inlet_temp_thresh))) / 100; // efficiency hit due to increased inlet temperature
                steam_efficiency     = ( inlet_temp > inlet_temp_thresh_2 ) ? ( opt_steam_efficiency - (opt_steam_efficiency*efficiency_hit) ) : opt_steam_efficiency; // steam efficiency accounting for the steam efficiency hit
                gas_efficiency       = ( air_temp > air_inlet_temp_thresh ) ? ( opt_gas_efficiency * ( 1.0 - ( (0.5845 / 100.0) * (air_temp - air_inlet_temp_thresh) ) ) ) : opt_gas_efficiency; // gas efficiency accounting for hit
                efficiency           = steam_efficiency + gas_efficiency - ( steam_efficiency * gas_efficiency ); // final efficiency accounting for gas and steam hits
	            steam_heat_in	 = heat_in - (gas_efficiency * heat_in);
	            desired_heat_cond    = steam_heat_in - (steam_heat_in * steam_efficiency) - (steam_heat_in * heat_sink); // assumption here is that heat loss through gas turbine is constant and any increase in losses goes to the condenser side
                desired_withdrawal   = ( desired_heat_cond / ( 4.18 * opt_deltaT ) ) + opt_consumption; // setting withdrawal rate OR user can set desired delta T
                total_withdrawal     = ( available_discharge >= desired_withdrawal) ? desired_withdrawal : available_discharge; // setting the withdrawal rate based on water availablity - m3/s
                consumption          = ( opt_consumption / opt_withdrawal ) * total_withdrawal ; // setting consumption rate based on withdrawal rate -m3/s
                withdrawal           = total_withdrawal - consumption;
                heat_allowed         = ( max_river_temp * ( discharge - consumption ) ) - ( (discharge - total_withdrawal) * river_temp); // heat allowed into river m3-degC/s
                heat_allowed         = ( heat_allowed < 0.0 ) ? 0.0 : heat_allowed; // heat allowed into river m3-degC/s
                max_deltaT           = (heat_allowed > 0 ) ? ( heat_allowed / withdrawal ) - inlet_temp : 0.0 ; // increase of cooling water degC
	            max_deltaT           = (max_deltaT > 35.0) ? 35.0 : max_deltaT; // increase of cooling water degC - with a maximum set to 25 degC
                max_deltaT           = (max_deltaT < 0.0) ? 0.0 : max_deltaT; // increase of cooling water degC - with a maximum set to 25 degC
                max_heat_cond        = withdrawal * max_deltaT * 4.18; // heat out of condesner MJ/s
                heat_cond            = ( max_heat_cond >= desired_heat_cond ) ? desired_heat_cond : max_heat_cond;
                deltaT               = heat_cond / ( withdrawal * 4.18 );
	            operational_gas_capacity	=gas_efficiency * heat_in;
	            operational_steam_capacity	=heat_cond * ( steam_efficiency / ( 1.0 - heat_sink - steam_efficiency ) );
	            operational_capacity = operational_gas_capacity + operational_steam_capacity;   // operational capacity of power plant post everything --- MW
                post_temperature     = ( ( ( discharge - total_withdrawal ) * river_temp ) + ( withdrawal * ( inlet_temp + deltaT ) ) ) / ( discharge - consumption); // outlet temperature after mixing deg C
	            operational_capacity = (LakeOcean > 0.5) ? heat_in * efficiency : operational_capacity;
	            post_temperature     = (LakeOcean > 0.5) ? river_temp : post_temperature;
            } // ------ END OF ONCE THROUGH NGCC

            /****************************************************   ONCE-THROUGH END    ****************************************************/

            /****************************************************    DRY COOLING  START ****************************************************/

            // CALCS FOR DRY COOLING STEAM ------

            if ( (nameplate > 0) && (technology == 3.0) ) {
                inlet_temp           = air_temp + itd; // setting the inlet temperature
                efficiency_hit       = ((0.0165 * pow((inlet_temp - inlet_temp_thresh),2)) + (0.1604 * (inlet_temp - inlet_temp_thresh))) / 100; // efficiency hit due to increased inlet temperature
                efficiency           = ( inlet_temp > inlet_temp_thresh_2 ) ? ( opt_efficiency - (efficiency_hit * opt_efficiency)) : opt_efficiency; // accounting for the efficiency hit
                operational_capacity = heat_in * efficiency;
                post_temperature     = river_temp; // outlet temperature after mixing deg C
            } // ------ END OF DRY COOLING STEAM

            if ( (nameplate > 0) && (technology == 6.0) ){
                inlet_temp           = air_temp + itd; // setting the inlet temperature
                opt_steam_efficiency = opt_efficiency * 0.526315789;
                opt_gas_efficiency   = ( opt_efficiency - opt_steam_efficiency ) / ( 1.0 - opt_steam_efficiency);
                efficiency_hit       = ((0.0165 * pow((inlet_temp - inlet_temp_thresh),2)) + (0.1604 * (inlet_temp - inlet_temp_thresh))) / 100; // efficiency hit due to increased inlet temperature
                steam_efficiency     = ( inlet_temp > inlet_temp_thresh_2 ) ? ( opt_steam_efficiency - (opt_steam_efficiency * efficiency_hit) ) : opt_steam_efficiency; // steam efficiency accounting for the steam efficiency hit
                gas_efficiency       = ( air_temp > air_inlet_temp_thresh ) ? ( opt_gas_efficiency * ( 1.0 - ( (0.5845 / 100.0) * (air_temp - air_inlet_temp_thresh) ) ) ) : opt_gas_efficiency; // gas efficiency accounting for hit
                efficiency           = steam_efficiency + gas_efficiency - ( steam_efficiency * gas_efficiency ); // final efficiency accounting for gas and steam hits
	            operational_capacity = heat_in * efficiency;
                post_temperature     = river_temp; // outlet temperature after mixing deg C
            } // ------ END OF DRY COOLING STEAM

            /**************************************************** //    DRY COOLING END    // ****************************************************/

            // Thermal pollution and generation calcs:
            operational_capacity = (operational_capacity + 0.001) > nameplate ? nameplate : operational_capacity;
            generation = ((operational_capacity * 24) > demand) ? demand : (operational_capacity * 24);
            op_hours = generation / operational_capacity;

            day_total_withdrawal = 3600 * op_hours * total_withdrawal;
            day_consumption = 3600 * op_hours * consumption;

            if ((technology == 1) || (technology == 4)) 	 eff_temperature = ( inlet_temp + deltaT );
            if ((technology == 2) || (technology == 5))      eff_temperature = inlet_temp;
            if ((technology == 3) || (technology == 6))      eff_temperature = 0;

            if ((technology == 1) || (technology == 4))      eff_volume = withdrawal;
            if ((technology == 2) || (technology == 5))      eff_volume = blowdown;
            if ((technology == 3) || (technology == 6))      eff_volume = 0;

            heat_to_river = 3600 * op_hours * eff_volume * eff_temperature;
            consumption_T       	= consumption_T + day_consumption; // keeping track of total consumption
            total_withdrawal_T	= total_withdrawal_T + day_total_withdrawal;
            discharge_T		= 86400 * Q;
            day_post_temperature = ( ( ( (86400*discharge) - day_total_withdrawal ) * river_temp ) + heat_to_river ) / ( (86400*discharge) - day_consumption); // outlet temperature after mixing deg C

            if ( (generation < 0.001) || (nameplate < 0.00000001) || ( ((technology == 1) || (technology == 4)) && (LakeOcean > 0.5)) ) {
                generation		= 0.0;
                op_hours		= 0.0;
                day_total_withdrawal	= 0.0;
                day_consumption		= 0.0;
                heat_to_river		= 0.0;
                consumption_T		= 0.0;
                total_withdrawal_T	= 0.0;
                heat_to_river_T		= 0.0;
                discharge_T		= 86400 * Q;
                day_post_temperature	= river_temp_initial;
            }
            heat_to_river_T  = heat_to_river_T + heat_to_river;

            /////////////////////////////////////////////////////////

/*            if ((operational_capacity * 0.8) > nameplate ) {
                printf("error: operational capacity is greater than given nameplate OT\n");
                printf("nameplate = %f, tech = %f, fuel = %f, eff = %f, lake = %f \n", nameplate, technology, fuel_type, opt_efficiency, LakeOcean);
                printf("inlet_temp = %f, consumption = %f, post_temperature = %f, opt_heat_cond = %f \n", inlet_temp, consumption, post_temperature, opt_heat_cond);
            }

            //////////// CHECKING FOR BUGS:

            if (LakeOcean < 0.5) {
                if (( ( technology == 1.0 ) || ( technology == 4.0) ) && CWA_limit > 0.0 && inlet_temp > 0.0 && Q > 0.1) {
                    if ((post_temperature > ( max_river_temp + 0.1 ) ) && (operational_capacity > nameplate) ) {
                        printf("error: river temperature is greater than CWA temperature OT\n");
		                printf("nameplate = %f, tech = %f, fuel = %f, eff = %f, lake = %f \n", nameplate, technology, fuel_type, opt_efficiency, LakeOcean);
                        printf("max_river_temp = %f, make_up = %f, blowdown = %f, discharge = %f, available_discharge = %f, inlet_temp = %f, operational_capacity = %f, heat_cond = %f ,max_make_up = %f, efficiency = %f \n", max_river_temp, withdrawal, blowdown, discharge, available_discharge, inlet_temp, operational_capacity, heat_cond, max_make_up, efficiency);
                        printf("inlet_temp = %f, consumption = %f, post_temperature = %f, opt_heat_cond = %f \n", inlet_temp, consumption, post_temperature, opt_heat_cond);
                    }
                    if (operational_capacity > nameplate) {
                        printf("error: operational capacity is greater than given nameplate OT\n");
                        printf("nameplate = %f, tech = %f, fuel = %f, eff = %f, lake = %f \n", nameplate, technology, fuel_type, opt_efficiency, LakeOcean);
                        printf("max_river_temp = %f, make_up = %f, blowdown = %f, discharge = %f, available_discharge = %f, inlet_temp = %f, operational_capacity = %f, heat_cond = %f ,max_make_up = %f, efficiency = %f \n", max_river_temp, withdrawal, blowdown, discharge, available_discharge, inlet_temp, operational_capacity, heat_cond, max_make_up, efficiency);
                        printf("inlet_temp = %f, consumption = %f, post_temperature = %f, opt_heat_cond = %f \n", inlet_temp, consumption, post_temperature, opt_heat_cond);
                    }
                    if (( inlet_temp > inlet_temp_thresh_2 ) && ( operational_capacity > nameplate )) {
                        printf("error: efficiency hit check needed OT \n");
                        printf("nameplate = %f, tech = %f, fuel = %f, eff = %f, lake = %f \n", nameplate, technology, fuel_type, opt_efficiency, LakeOcean);
                        printf("max_river_temp = %f, make_up = %f, blowdown = %f, discharge = %f, available_discharge = %f, inlet_temp = %f, operational_capacity = %f, heat_cond = %f ,max_make_up = %f, efficiency = %f \n", max_river_temp, withdrawal, blowdown, discharge, available_discharge, inlet_temp, operational_capacity, heat_cond, max_make_up, efficiency);
                        printf("inlet_temp = %f, consumption = %f, post_temperature = %f, opt_heat_cond = %f \n", inlet_temp, consumption, post_temperature, opt_heat_cond);
                    }
                    if (( operational_capacity < 0.0 ) || ( operational_capacity > 9999 )) {
                        printf("error: check operational capacity OT \n");
                        printf("nameplate = %f, tech = %f, fuel = %f, eff = %f, lake = %f \n", nameplate, technology, fuel_type, opt_efficiency, LakeOcean);
                        printf("max_river_temp = %f, make_up = %f, m = %f, discharge = %f, available_discharge = %f, inlet_temp = %f, operational_capacity = %f, heat_cond = %f ,max_make_up = %f, efficiency = %f \n", max_river_temp, withdrawal, m, discharge, available_discharge, inlet_temp, operational_capacity, heat_cond, max_make_up, efficiency);
                        printf("temp = %f, inlet_temp = %f, consumption = %f, post_temperature = %f, opt_heat_cond = %f \n", Q_WTemp, inlet_temp, consumption, post_temperature, opt_heat_cond);
                    }
                    if ( operational_capacity < 0.0 ) {
                        printf("error: check operational - negative operational cap - capacity OT \n");
                        printf("nameplate = %f, tech = %f, fuel = %f, eff = %f, lake = %f \n", nameplate, technology, fuel_type, opt_efficiency, LakeOcean);
                        printf("max_river_temp = %f, make_up = %f, m = %f, discharge = %f, available_discharge = %f, inlet_temp = %f, operational_capacity = %f, heat_cond = %f ,max_make_up = %f, efficiency = %f \n", max_river_temp, withdrawal, m, discharge, available_discharge, inlet_temp, operational_capacity, heat_cond, max_make_up, efficiency);
                        printf("river_temp_initial = %f, river_initial = %f, cwa_delta = %f, cwa_limit = %f \n", river_temp_initial, river_temp, CWA_delta, CWA_limit);
                        printf("inlet_temp = %f, consumption = %f, post_temperature = %f, opt_heat_cond = %f \n", inlet_temp, consumption, post_temperature, opt_heat_cond);
                    }
                }

                if (( ( technology == 2.0 ) || ( technology == 5.0) ) && CWA_limit > 0.0 && inlet_temp > 0.0) {
                    if ((post_temperature > ( max_river_temp + 0.1 ) ) && (operational_capacity > nameplate) ) {
                        printf("error: river temperature is greater than CWA temperature RC\n");
                        printf("nameplate = %f, tech = %f, fuel = %f, eff = %f, lake = %f \n", nameplate, technology, fuel_type, opt_efficiency, LakeOcean);
                        printf("max_river_temp = %f, make_up = %f, blowdown = %f, discharge = %f, available_discharge = %f, inlet_temp = %f, operational_capacity = %f, heat_cond = %f ,max_make_up = %f, efficiency = %f \n", max_river_temp, make_up, blowdown, discharge, available_discharge, inlet_temp, operational_capacity, heat_cond, max_make_up, efficiency);
                        printf("inlet_temp = %f, consumption = %f, post_temperature = %f, opt_heat_cond = %f \n", inlet_temp, consumption, post_temperature, opt_heat_cond);
                    }
                    if (operational_capacity > nameplate) {
                        printf("error: operational capacity is greater than given nameplate RC\n");
                        printf("nameplate = %f, tech = %f, fuel = %f, eff = %f, lake = %f \n", nameplate, technology, fuel_type, opt_efficiency, LakeOcean);
                        printf("max_river_temp = %f, make_up = %f, blowdown = %f, discharge = %f, available_discharge = %f, inlet_temp = %f, operational_capacity = %f, heat_cond = %f ,max_make_up = %f, efficiency = %f \n", max_river_temp, make_up, blowdown, discharge, available_discharge, inlet_temp, operational_capacity, heat_cond, max_make_up, efficiency);
                        printf("inlet_temp = %f, consumption = %f, post_temperature = %f, opt_heat_cond = %f \n", inlet_temp, consumption, post_temperature, opt_heat_cond);
                    }
                    if (( inlet_temp > inlet_temp_thresh_2 ) && ( operational_capacity > nameplate )) {
                        printf("error: efficiency hit check needed RC \n");
                        printf("nameplate = %f, tech = %f, fuel = %f, eff = %f, lake = %f \n", nameplate, technology, fuel_type, opt_efficiency, LakeOcean);
                        printf("max_river_temp = %f, make_up = %f, blowdown = %f, discharge = %f, available_discharge = %f, inlet_temp = %f, operational_capacity = %f, heat_cond = %f ,max_make_up = %f, efficiency = %f \n", max_river_temp, make_up, blowdown, discharge, available_discharge, inlet_temp, operational_capacity, heat_cond, max_make_up, efficiency);
                        printf("inlet_temp = %f, consumption = %f, post_temperature = %f, opt_heat_cond = %f \n", inlet_temp, consumption, post_temperature, opt_heat_cond);
                    }
                    if (( operational_capacity < 0.0 ) || ( operational_capacity > 9999 )) {
                        printf("error: check operational capacity RC \n");
                        printf("nameplate = %f, tech = %f, fuel = %f, eff = %f, lake = %f \n", nameplate, technology, fuel_type, opt_efficiency, LakeOcean);
                        printf("max_river_temp = %f, make_up = %f, blowdown = %f, discharge = %f, available_discharge = %f, inlet_temp = %f, operational_capacity = %f, heat_cond = %f ,max_make_up = %f, efficiency = %f \n", max_river_temp, make_up, blowdown, discharge, available_discharge, inlet_temp, operational_capacity, heat_cond, max_make_up, efficiency);
                        printf("wet_bulb = %f, consumption = %f, post_temperature = %f, opt_heat_cond = %f \n", wet_b_temp, consumption, post_temperature, opt_heat_cond);
                    }
                    if ( operational_capacity < 0.0 ) {
                        printf("error: check operational - negative operational cap - capacity NGCC-RC \n");
                        printf("wet_b_temp = %f, nameplate = %f, tech = %f, fuel = %f, eff = %f, lake = %f \n", wet_b_temp, nameplate, technology, fuel_type, opt_efficiency, LakeOcean);
                        printf("max_river_temp = %f, make_up = %f, blowdown = %f, discharge = %f, available_discharge = %f, inlet_temp = %f, operational_capacity = %f, heat_cond = %f ,max_make_up = %f, efficiency = %f \n", max_river_temp, make_up, blowdown, discharge, available_discharge, inlet_temp, operational_capacity, heat_cond, max_make_up, efficiency);
		                printf("max_make_up = %f, river_initial = %f, cwa_delta = %f, cwa_limit = %f \n", max_make_up, river_temp, CWA_delta, CWA_limit);
                        printf("inlet_temp = %f, consumption = %f, post_temperature = %f, opt_heat_cond = %f \n", inlet_temp, consumption, post_temperature, opt_heat_cond);
                    }
                }
            }
*/
            ////////////////////////////////////////////////////////////////////////////////////

            if (m == 1) inlet_temp_1 = inlet_temp;
            if (m == 2) inlet_temp_2 = inlet_temp;
            if (m == 3) inlet_temp_3 = inlet_temp;
            if (m == 4) inlet_temp_4 = inlet_temp;

            if (m == 1) loss_inlet_1 = nameplate - (efficiency * heat_in);
            if (m == 2) loss_inlet_2 = nameplate - (efficiency * heat_in);
            if (m == 3) loss_inlet_3 = nameplate - (efficiency * heat_in);
            if (m == 4) loss_inlet_4 = nameplate - (efficiency * heat_in);

            if (m == 1) loss_water_1 = nameplate - loss_inlet_1 - operational_capacity;
            if (m == 2) loss_water_2 = nameplate - loss_inlet_2 - operational_capacity;
            if (m == 3) loss_water_3 = nameplate - loss_inlet_3 - operational_capacity;
            if (m == 4) loss_water_4 = nameplate - loss_inlet_4 - operational_capacity;

            if (m == 1) operational_capacity_1 = operational_capacity;
            if (m == 2) operational_capacity_2 = operational_capacity;
            if (m == 3) operational_capacity_3 = operational_capacity;
            if (m == 4) operational_capacity_4 = operational_capacity;

            if (m == 1) generation_1 = generation;
            if (m == 2) generation_2 = generation;
            if (m == 3) generation_3 = generation;
            if (m == 4) generation_4 = generation;

            if (m == 1) heat_to_river_1 = eff_volume * deltaT;
            if (m == 2) heat_to_river_2 = eff_volume * deltaT;
            if (m == 3) heat_to_river_3 = eff_volume * deltaT;
            if (m == 4) heat_to_river_4 = eff_volume * deltaT;
        } // This ends for loop
        } else { // there is no power plant
            consumption_T = 0;
			discharge = Q;
			day_post_temperature = Q_WTemp;
			pot_WTemp_1   = Q_WTemp;
			output_1	  = 0.0;
			wdl_1		  = 0.0;
			evaporation_1 = 0.0;
			blowdown_1	  = 0.0;
			OT_effluent_1 = 0.0;
			totalDaily_output_1 	  = 0.0;		// total MWhrs per day
			totalDaily_deficit_1	  = 0.0;		// energy deficit (MWhrs)
			totalDaily_percent_1	  = 0.0;		// percent of demand fulfilled
			totalHours_run_1		  = 0.0;			// hrs of operation run
			totalDaily_wdl_1		  = 0.0;			// total wdl in m3 per day
			totalDaily_target_wdl_1	  = 0.0;	// total target withdrawal to meet demand based on standard wdl per MW
			avgDaily_eff_dTemp_1	  = 0.0;							// average daily effluent temperature rise divided by optDeltaT_1 (positive is larger than optDeltaT)
			avgDaily_efficiency_1	  = 0.0;				// percentage of optimal efficiency fulfilled
			totalDaily_evap_1		  = 0.0;	// total evaporation in m3 per day
			totalDaily_blowdown_1	  = 0.0;		// total blowdown (or discharge from power plant to river) in m3 per day
			totalDaily_OTeffluent_1   = 0.0;		// total return flow from once through cooling to river (effluent)
			totalDaily_returnflow_1	  = 0.0;	// total (effluent + blowdown) from both "once through" and "recirc" back to river
			totalDaily_demand_1		  = 0.0;
			totalDaily_heatToRiv	  = 0.0;
			totalDaily_heatToEng	  = 0.0;
			totalDaily_heatToSink	  = 0.0;
			Q_outgoing_1			  = Q;
			Q_outgoing_WTemp_1		  = Q_WTemp;
			flux_QxT_new   	   		  = Q_outgoing_WTemp_1 * Q_outgoing_1 * 86400.0;		// late-night discharge test
			flux_GJ_old_prist         = Q_outgoing_WTemp_1 * Q_outgoing_1 * 86400.0;
			flux_GJ_new				  = flux_GJ_old_prist;
			totalDaily_heatToRiv	  = 0.0;
			totalDaily_heatToEng	  = 0.0;
			totalDaily_heatToSink	  = 0.0;
			totalDaily_heatToElec	  = 0.0;
			evaporated_heat			  = 0.0;
	    }
    } else { // there is no flow
        consumption_T = 0;
		discharge = Q;
        day_post_temperature = Q_WTemp;
		pot_WTemp_1	  = Q_WTemp;
		output_1	  = 0.0;
		wdl_1		  = 0.0;
		evaporation_1 = 0.0;
		blowdown_1    = 0.0;
		OT_effluent_1 			 = 0.0;
		totalDaily_output_1 	  = 0.0;		// total MWhrs per day
		totalDaily_deficit_1	  = 0.0;		// energy deficit (MWhrs)
		totalDaily_percent_1	  = 0.0;		// percent of demand fulfilled
		totalHours_run_1		  = 0.0;			// hrs of operation run
		totalDaily_wdl_1		  = 0.0;			// total wdl in m3 per day
		totalDaily_target_wdl_1	  = 0.0;	// total target withdrawal to meet demand based on standard wdl per MW
		avgDaily_eff_dTemp_1	  = 0.0;							// average daily effluent temperature rise divided by optDeltaT_1 (positive is larger than optDeltaT)
		avgDaily_efficiency_1	  = 0.0;				// percentage of optimal efficiency fulfilled
		totalDaily_evap_1		  = 0.0;	// total evaporation in m3 per day
		totalDaily_blowdown_1	  = 0.0;		// total blowdown (or discharge from power plant to river) in m3 per day
		totalDaily_OTeffluent_1   = 0.0;		// total return flow from once through cooling to river (effluent)
		totalDaily_returnflow_1	  = 0.0;	// total (effluent + blowdown) from both "once through" and "recirc" back to river
		totalDaily_demand_1		  = NamePlate_1 > 0.0 ? energyDemand_1 : 0.0;
		totalDaily_heatToRiv	  = 0.0;
		totalDaily_heatToEng	  = 0.0;
		totalDaily_heatToSink	  = 0.0;
		Q_outgoing_1			  = Q;
		Q_outgoing_WTemp_1		  = Q_WTemp;
		flux_QxT_new   	   		  = Q_outgoing_WTemp_1 * Q_outgoing_1 * 86400.0;		// late-night discharge test
		totalDaily_heatToRiv	  = 0.0;
		totalDaily_heatToEng	  = 0.0;
		totalDaily_heatToSink	  = 0.0;
		totalDaily_heatToElec	  = 0.0;
		evaporated_heat			  = 0.0;
	}

    if ((LakeOcean > 0.5) || (Downstream_onoff < 0.5)) {
		consumption_T           = 0;
        discharge               = Q;
        day_post_temperature    = Q_WTemp;
        pot_WTemp_1             = Q_WTemp;
        output_1                = 0.0;
        wdl_1                   = 0.0;
        evaporation_1           = 0.0;
        blowdown_1              = 0.0;
        OT_effluent_1           = 0.0;
        totalDaily_output_1     = 0.0;  // total MWhrs per day
        totalDaily_deficit_1    = 0.0;  // energy deficit (MWhrs)
        totalDaily_percent_1    = 0.0;  // percent of demand fulfilled
        totalHours_run_1        = 0.0;  // hrs of operation run
        totalDaily_wdl_1        = 0.0;  // total wdl in m3 per day
        totalDaily_target_wdl_1 = 0.0;  // total target withdrawal to meet demand based on standard wdl per MW
        avgDaily_eff_dTemp_1    = 0.0;  // average daily effluent temperature rise divided by optDeltaT_1 (positive is larger than optDeltaT)
        avgDaily_efficiency_1   = 0.0;  // percentage of optimal efficiency fulfilled
        totalDaily_evap_1       = 0.0;  // total evaporation in m3 per day
        totalDaily_blowdown_1   = 0.0;  // total blowdown (or discharge from power plant to river) in m3 per day
        totalDaily_OTeffluent_1 = 0.0;  // total return flow from once through cooling to river (effluent)
        totalDaily_returnflow_1 = 0.0;  // total (effluent + blowdown) from both "once through" and "recirc" back to river
        totalDaily_demand_1     = NamePlate_1 > 0.0 ? energyDemand_1 : 0.0;
        totalDaily_heatToRiv    = 0.0;
        totalDaily_heatToEng    = 0.0;
        totalDaily_heatToSink   = 0.0;
        Q_outgoing_1            = Q;
        Q_outgoing_WTemp_1      = Q_WTemp;
        flux_QxT_new            = Q_outgoing_WTemp_1 * Q_outgoing_1 * 86400.0; // late-night discharge test
        totalDaily_heatToRiv    = 0.0;
        totalDaily_heatToEng    = 0.0;
        totalDaily_heatToSink   = 0.0;
        totalDaily_heatToElec   = 0.0;
        evaporated_heat         = 0.0;
    }

    // NEW:
    operational_capacity = operational_capacity_1 + operational_capacity_2 + operational_capacity_3 + operational_capacity_4;
    loss_inlet_total=loss_inlet_1 + loss_inlet_2 + loss_inlet_3 + loss_inlet_4;
    loss_water_total=loss_water_1 + loss_water_2 + loss_water_3 + loss_water_4;
    condenser_inlet = (inlet_temp_1 + inlet_temp_2 + inlet_temp_3 + inlet_temp_4) / (m - 1.0);

    // need: total plant generation, river temp at output, discharge, flux out,
    generation = generation_1 + generation_2 + generation_3 + generation_4;
    Q_outgoing_WTemp_1 = day_post_temperature; 

    //consumption_T=0; // added this for thermal pollution calculations
    Q_outgoing_1 = ((Q*86400) - consumption_T)/86400;

    flux_QxT_new = Q_outgoing_WTemp_1 * Q_outgoing_1 * 86400.0;

/*    if ((nameplate_1 > 187.4) && (nameplate_1 < 187.6) && (Q_WTemp > 40.0) && (Q > 2)) {
        printf(" INTITAL INITIAL INITIAL flux_QxT = %f, cccc= %f \n", flux_QxT, cccc);
        printf(" RESULTS CHECK \n");
        printf("heat_to_river = %f, river_temp = %f \n", heat_to_river, river_temp); 
        printf("river_temp_initial = %f \n", river_temp_initial);
        printf("nameplate_1 = %f, nameplate_2 = %f, nameplate_3 = %f, nameplate_4 = %f \n", nameplate_1, nameplate_2, nameplate_3, nameplate_4);
        printf("CWA_onoff = %f, Q = %f, nameplate = %f, tech = %f, fuel = %f, opt_eff = %f, lake = %f \n", CWA_onoff, Q, nameplate, technology, fuel_type, opt_efficiency, LakeOcean);
        printf("max_river_temp = %f, make_up = %f, blowdown = %f, discharge = %f, available_discharge = %f, inlet_temp = %f, operational_capacity = %f, heat_cond = %f ,max_make_up = %f, efficiency = %f \n", max_river_temp, withdrawal, blowdown, discharge, available_discharge, inlet_temp, operational_capacity, heat_cond, max_make_up, efficiency);
        printf("operational_capacity = %f, demand = %f, generation = %f, op_hours = %f \n", operational_capacity, demand, generation, op_hours);
        printf("temp = %f, inlet_temp = %f, consumption = %f, post_temperature = %f, opt_heat_cond = %f, withdrawal = %f, total_withdrawal = %f \n", Q_WTemp, inlet_temp, consumption, post_temperature, opt_heat_cond, withdrawal, total_withdrawal);
        printf("day_discharge = %f, day_total_withdrawal = %f, day_withdrawal = %f, day_consumption = %f, day_post_temperature = %f \n", day_discharge, day_total_withdrawal, day_withdrawal, day_consumption, day_post_temperature);
        printf("withdrawal_T = %f, total_withdrawal_T = %f, heat_to_river_T = %f, eff_volume = %f, eff_temperature = %f, discharge_T = %f \n", withdrawal_T, total_withdrawal_T, heat_to_river_T, eff_volume, eff_temperature, discharge_T);
        printf("checking \n");
        printf("operational_capacity = %f, consumption_T = %f, generation = %f, Q_outgoing_WTemp_1 = %f, Q_outgoing_1 = %f, flux_QxT_new = %f \n",operational_capacity, consumption_T, generation, Q_outgoing_WTemp_1, Q_outgoing_1, flux_QxT_new);
    }
*/
    if (totalDaily_output_1 < 0.0) {
	    CMmsgPrint(CMmsgUsrError, "!!!!!! NEG OUTPUT: y = %d, m = %d, d = %d, TotalDaily_output_1 = %f, namplate = %f\n", MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), totalDaily_output_1, NamePlate_1);
    }

    if (wdl_1 > Q && Q > 0.0) {
	    CMmsgPrint(CMmsgUsrError,"!!!!!! WDL > Q: y = %d, m = %d, d = %d, wdl_1 = %f, Q = %f, namplate = %f\n", MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), wdl_1, Q, NamePlate_1);
    }

    if (Q_outgoing_WTemp_1 < 0.0) CMmsgPrint(CMmsgUsrError,"!!!!! negative Temp, itemID = %d, y = %d, m = %d, d = %d\n", itemID, MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay());

    MFVarSetFloat(_MDOutLossToInletID,         itemID, loss_inlet_total);
    MFVarSetFloat(_MDOutLossToWaterID,         itemID, loss_water_total);
    MFVarSetFloat(_MDInRouting_DischargeID,    itemID, Q_outgoing_1); //late-night discharge test
    MFVarSetFloat(_MDWTemp_QxTID,              itemID, Q_outgoing_WTemp_1);
    MFVarSetFloat(_MDFlux_QxTID,               itemID, flux_QxT_new);
    MFVarSetFloat(_MDOutTotalThermalWdlsID,    itemID, totalDaily_wdl_1);
    MFVarSetFloat(_MDOutTotalEvaporationID,    itemID, totalDaily_evap_1 + totalDaily_evap_2); //RJS 120912 added totalDaily_evap_2
    MFVarSetFloat(_MDOutTotalExternalWaterID,  itemID, totalDaily_external_2);  // RJS 120912, total external water use.  Sum of this and Evaporation is the TOTAL CONSUMPTION
    MFVarSetFloat(_MDOutTotalOptThermalWdlsID, itemID, totalDaily_target_wdl_1);
    MFVarSetFloat(_MDOutAvgDeltaTempID,        itemID, avgDaily_eff_dTemp_1);
    MFVarSetFloat(_MDOutAvgEfficiencyID,       itemID, efficiency);
    MFVarSetFloat(_MDOutPowerOutput1ID,        itemID, totalDaily_output_1);
    MFVarSetFloat(_MDOutPowerDeficit1ID,       itemID, totalDaily_deficit_1);
    MFVarSetFloat(_MDOutPowerPercent1ID,       itemID, totalDaily_percent_1);
    MFVarSetFloat(_MDOutPowerOutputTotalID,    itemID, operational_capacity);   // AM TODO there is only 1 layer of power plants, so this is same as above
    MFVarSetFloat(_MDOutPowerOutputTotal1ID,   itemID, operational_capacity_1); // AM TODO there is only 1 layer of power plants, so this is same as above
    MFVarSetFloat(_MDOutPowerOutputTotal2ID,   itemID, operational_capacity_2); // AM TODO there is only 1 layer of power plants, so this is same as above
    MFVarSetFloat(_MDOutPowerOutputTotal3ID,   itemID, operational_capacity_3); // AM TODO there is only 1 layer of power plants, so this is same as above
    MFVarSetFloat(_MDOutPowerOutputTotal4ID,   itemID, operational_capacity_4); // AM TODO there is only 1 layer of power plants, so this is same as above
    MFVarSetFloat(_MDOutGenerationID,          itemID, generation);             // AM TODO there is only 1 layer of power plants, so this is same as above
    MFVarSetFloat(_MDOutGeneration1ID,         itemID, generation_1);           // AM TODO there is only 1 layer of power plants, so this is same as above
    MFVarSetFloat(_MDOutGeneration2ID,         itemID, generation_2);           // AM TODO there is only 1 layer of power plants, so this is same as above
    MFVarSetFloat(_MDOutGeneration3ID,         itemID, generation_3);           // AM TODO there is only 1 layer of power plants, so this is same as above
    MFVarSetFloat(_MDOutGeneration4ID,         itemID, generation_4);           // AM TODO there is only 1 layer of power plants, so this is same as above
	MFVarSetFloat(_MDOutPowerDeficitTotalID,   itemID, totalDaily_deficit_1);
	MFVarSetFloat(_MDOutPowerPercentTotalID,   itemID, totalDaily_percent_1);
	MFVarSetFloat(_MDOutTotalEnergyDemandID,   itemID, totalDaily_demand_1);
	MFVarSetFloat(_MDOutTotalReturnFlowID,     itemID, totalDaily_returnflow_1);
	MFVarSetFloat(_MDOutLHFractID,             itemID, LH_fract);
	MFVarSetFloat(_MDOutLHFractPostID,         itemID, LH_fract_post);
	MFVarSetFloat(_MDOutQpp1ID,                itemID, Qpp_1);
	MFVarSetFloat(_MDOutOptQO1ID,              itemID, opt_QO_1);
	MFVarSetFloat(_MDOutTotalHeatToRivID,      itemID, heat_to_river_T);
	MFVarSetFloat(_MDOutTotalHeatToSinkID,     itemID, totalGJ_heatToSink);
	MFVarSetFloat(_MDOutTotalHeatToEngID,      itemID, totalGJ_heatToEng);
	MFVarSetFloat(_MDOutTotalHeatToElecID,     itemID, totalGJ_heatToElec);
	MFVarSetFloat(_MDOutTotalHeatToEvapID,     itemID, totalGJ_heatToEvap);
	MFVarSetFloat(_MDOutCondenserInletID,      itemID, condenser_inlet); 
    MFVarSetFloat(_MDOutCondenserInlet1ID,     itemID, inlet_temp_1);
    MFVarSetFloat(_MDOutCondenserInlet2ID,     itemID, inlet_temp_2);
    MFVarSetFloat(_MDOutCondenserInlet3ID,     itemID, inlet_temp_3);
    MFVarSetFloat(_MDOutCondenserInlet4ID,     itemID, inlet_temp_4);
    MFVarSetFloat(_MDOutLossToInlet1ID,        itemID, loss_inlet_1);
    MFVarSetFloat(_MDOutLossToInlet2ID,        itemID, loss_inlet_2);
    MFVarSetFloat(_MDOutLossToInlet3ID,        itemID, loss_inlet_3);
    MFVarSetFloat(_MDOutLossToInlet4ID,        itemID, loss_inlet_4);
	MFVarSetFloat(_MDOutSimEfficiencyID,       itemID, efficiency);	      // added AM TODO
	MFVarSetFloat(_MDOutTotalHoursRunID,       itemID, totalHours_run_1);
    MFVarSetFloat(_MDOutHeatToRiver1ID,        itemID, heat_to_river_1);
    MFVarSetFloat(_MDOutHeatToRiver2ID,        itemID, heat_to_river_2);
    MFVarSetFloat(_MDOutHeatToRiver3ID,        itemID, heat_to_river_3);
    MFVarSetFloat(_MDOutHeatToRiver4ID,        itemID, heat_to_river_4);
}

int MDTP2M_ThermalInputsDef () {

	MFDefEntering ("Thermal Inputs");
    if (((_MDPlaceHolderID             = MDTP2M_WTempRiverRouteDef()) == CMfailed) ||
        ((_MDInRouting_DischargeID     = MDRouting_DischargeDef())    == CMfailed) ||
        ((_MDInWetBulbTempID           = MDCommon_WetBulbTempDef())   == CMfailed) ||
	    ((_MDInCommon_AirTemperatureID = MFVarGetID (MDVarCommon_AirTemperature,    "degC",      MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInDischargeIncomingID     = MFVarGetID (MDVarRouting_Discharge0,       "m3/s",      MFInput,  MFState, MFInitial))  == CMfailed) ||
	    ((_MDFluxMixing_QxTID          = MFVarGetID (MDVarTP2M_FluxMixing_QxT,      "m3*degC/d", MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDFlux_QxTID                = MFVarGetID (MDVarTP2M_Flux_QxT,            "m3*degC/d", MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
	    ((_MDWTemp_QxTID               = MFVarGetID (MDVarTP2M_Temp_QxT,            "degC",      MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDInNamePlate1ID            = MFVarGetID (MDVarTP2M_NamePlate1,          "MW",        MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInFuelType1ID             = MFVarGetID (MDVarTP2M_FuelType1,           "-",         MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInTechnology1ID           = MFVarGetID (MDVarTP2M_Technology1,         "-",         MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInEfficiency1ID           = MFVarGetID (MDVarTP2M_Efficiency1,         "-",         MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInLakeOcean1ID            = MFVarGetID (MDVarTP2M_LakeOcean1,          "-",         MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInDemand1ID               = MFVarGetID (MDVarTP2M_Demand1,             "MWh",       MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInNamePlate2ID            = MFVarGetID (MDVarTP2M_NamePlate2,          "MW",        MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInFuelType2ID             = MFVarGetID (MDVarTP2M_FuelType2,           "-",         MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInTechnology2ID           = MFVarGetID (MDVarTP2M_Technology2,         "-",         MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInEfficiency2ID           = MFVarGetID (MDVarTP2M_Efficiency2,         "-",         MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInDemand2ID               = MFVarGetID (MDVarTP2M_Demand2,             "MWh",       MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInNamePlate3ID            = MFVarGetID (MDVarTP2M_NamePlate3,          "MW",        MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInFuelType3ID             = MFVarGetID (MDVarTP2M_FuelType3,           "-",         MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInTechnology3ID           = MFVarGetID (MDVarTP2M_Technology3,         "-",         MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInEfficiency3ID           = MFVarGetID (MDVarTP2M_Efficiency3,         "-",         MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInDemand3ID               = MFVarGetID (MDVarTP2M_Demand3,             "MWh",       MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInNamePlate4ID            = MFVarGetID (MDVarTP2M_NamePlate4,          "MW",        MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInFuelType4ID             = MFVarGetID (MDVarTP2M_FuelType4,           "-",         MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInTechnology4ID           = MFVarGetID (MDVarTP2M_Technology4,         "-",         MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInEfficiency4ID           = MFVarGetID (MDVarTP2M_Efficiency4,         "-",         MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInDemand4ID               = MFVarGetID (MDVarTP2M_Demand4,             "MWh",       MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInCWA_DeltaID             = MFVarGetID (MDVarTP2M_CWA_Delta,           "-",         MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInCWA_LimitID             = MFVarGetID (MDVarTP2M_CWA_Limit,           "-",         MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInCWA_OnOffID             = MFVarGetID (MDVarTP2M_CWA_OnOff,           "-",         MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInDownstream_OnOffID      = MFVarGetID (MDVarTP2M_Downstream_OnOff,    "-",         MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDInCWA_316b_OnOffID        = MFVarGetID (MDVarTP2M_CWA_316b_OnOff,      "-",         MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutLossToWaterID          = MFVarGetID (MDVarTP2M_LossToWater,         "MW",        MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutLossToInletID          = MFVarGetID (MDVarTP2M_LossToInlet,         "MW",        MFOutput, MFState, MFBoundary)) == CMfailed) ||
		((_MDOutAvgEfficiencyID        = MFVarGetID (MDVarTP2M_AvgEfficiency,       "-",         MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutAvgDeltaTempID         = MFVarGetID (MDVarTP2M_AvgDeltaTemp,        "-",         MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutTotalEvaporationID     = MFVarGetID (MDVarTP2M_TotalEvaporation,    "m3",        MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDOutTotalExternalWaterID   = MFVarGetID (MDVarTP2M_TotalExternalWater,  "m3",        MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDOutTotalThermalWdlsID     = MFVarGetID (MDVarTP2M_TotalThermalWdls,    "m3",        MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDOutTotalOptThermalWdlsID  = MFVarGetID (MDVarTP2M_TotalOptThermalWdls, "m3",        MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDOutPowerOutput1ID         = MFVarGetID (MDVarTP2M_PowerOutput1,        "MW",        MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDOutPowerDeficit1ID        = MFVarGetID (MDVarTP2M_PowerDeficit1,       "MW",        MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDOutPowerPercent1ID        = MFVarGetID (MDVarTP2M_PowerPercent1,       "MW",        MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutPowerOutputTotalID     = MFVarGetID (MDVarTP2M_PowerOutputTotal,    "MW",        MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDOutPowerOutputTotal1ID    = MFVarGetID (MDVarTP2M_PowerOutputTotal1,   "MW",        MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDOutPowerOutputTotal2ID    = MFVarGetID (MDVarTP2M_PowerOutputTotal2,   "MW",        MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDOutPowerOutputTotal3ID    = MFVarGetID (MDVarTP2M_PowerOutputTotal3,   "MW",        MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDOutPowerOutputTotal4ID    = MFVarGetID (MDVarTP2M_PowerOutputTotal4,   "MW",        MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDOutGenerationID           = MFVarGetID (MDVarTP2M_Generation,          "MWh",       MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDOutGeneration1ID          = MFVarGetID (MDVarTP2M_Generation1,         "MWh",       MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDOutGeneration2ID          = MFVarGetID (MDVarTP2M_Generation2,         "MWh",       MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDOutGeneration3ID          = MFVarGetID (MDVarTP2M_Generation3,         "MWh",       MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDOutGeneration4ID          = MFVarGetID (MDVarTP2M_Generation4,         "MWh",       MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDOutPowerDeficitTotalID    = MFVarGetID (MDVarTP2M_PowerDeficitTotal,   "MW",        MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDOutPowerPercentTotalID    = MFVarGetID (MDVarTP2M_PowerPercentTotal,   "MW",        MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutTotalEnergyDemandID    = MFVarGetID (MDVarTP2M_TotalEnergyDemand,   "MW",        MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDOutTotalReturnFlowID	   = MFVarGetID (MDVarTP2M_TotalReturnFlow,     "m3",        MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDOutLHFractID              = MFVarGetID (MDVarTP2M_LHFract,             "-",         MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutLHFractPostID          = MFVarGetID (MDVarTP2M_LHFractPost,         "-",         MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutQpp1ID                 = MFVarGetID (MDVarTP2M_Qpp1,                "-",         MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutOptQO1ID               = MFVarGetID (MDVarTP2M_OptQO1,              "-",         MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutTotalHeatToRivID	   = MFVarGetID (MDVarTP2M_HeatToRiv,           "GJ",        MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDOutTotalHeatToSinkID	   = MFVarGetID (MDVarTP2M_HeatToSink,          "GJ",        MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDOutTotalHeatToEngID       = MFVarGetID (MDVarTP2M_HeatToEng,           "GJ",        MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDOutTotalHeatToElecID      = MFVarGetID (MDVarTP2M_HeatToElec,          "GJ",        MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDOutTotalHeatToEvapID      = MFVarGetID (MDVarTP2M_HeatToEvap,          "GJ",        MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||
        ((_MDOutCondenserInletID       = MFVarGetID (MDVarTP2M_CondenserInlet,      "degC",      MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutCondenserInlet1ID      = MFVarGetID (MDVarTP2M_CondenserInlet1,     "degC",      MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutCondenserInlet2ID      = MFVarGetID (MDVarTP2M_CondenserInlet2,     "degC",      MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutCondenserInlet3ID      = MFVarGetID (MDVarTP2M_CondenserInlet3,     "degC",      MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutCondenserInlet4ID      = MFVarGetID (MDVarTP2M_CondenserInlet4,     "degC",      MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutHeatToRiver1ID         = MFVarGetID (MDVarTP2M_HeatToRiver1,        "MJ",        MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutHeatToRiver2ID         = MFVarGetID (MDVarTP2M_HeatToRiver2,        "MJ",        MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutHeatToRiver3ID         = MFVarGetID (MDVarTP2M_HeatToRiver3,        "MJ",        MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutHeatToRiver4ID         = MFVarGetID (MDVarTP2M_HeatToRiver4,        "MJ",        MFOutput, MFState, MFBoundary)) == CMfailed) ||
		((_MDOutSimEfficiencyID        = MFVarGetID (MDVarTP2M_SimEfficiency,       "-",         MFOutput, MFState, MFBoundary)) == CMfailed) ||
		((_MDOutTotalHoursRunID        = MFVarGetID (MDVarTP2M_TotalHoursRun,       "-",         MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutLossToInlet1ID         = MFVarGetID (MDVarTP2M_LossToInlet1,        "degC",      MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutLossToInlet2ID         = MFVarGetID (MDVarTP2M_LossToInlet2,        "degC",      MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutLossToInlet3ID         = MFVarGetID (MDVarTP2M_LossToInlet3,        "degC",      MFOutput, MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutLossToInlet4ID         = MFVarGetID (MDVarTP2M_LossToInlet4,        "degC",      MFOutput, MFState, MFBoundary)) == CMfailed) ||
        (MFModelAddFunction (_MDThermalInputs3) == CMfailed)) return (CMfailed);
	MFDefLeaving ("Thermal Inputs");
	return (_MDWTemp_QxTID);
}

