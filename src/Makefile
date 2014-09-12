include ../common.mk
INC    = ../include
SRC    = ../src
OBJ    = ../obj
BIN    = ../bin
CC     = gcc $(UNIXCCOPS) -I$(INC) -I$(GHAASDIR)/include
LINK   = gcc
LIBS   = -L$(GHAASDIR)/lib -lMF10 -lCM10 $(UNIXLIBS)

TARGET = $(BIN)/wbmplus.bin

INCLUDES = $(INC)/MD.h

OBJS = $(OBJ)/WBMMain.o\
       $(OBJ)/MDAccBalance.o\
       $(OBJ)/MDAccumulate.o\
       $(OBJ)/MDAvgNSteps.o\
       $(OBJ)/MDBaseFlow.o\
       $(OBJ)/MDBgc.o\
       $(OBJ)/MDBgcDOC.o\
       $(OBJ)/MDBgcDIN.o\
       $(OBJ)/MDBgcDINPlusBiomass.o\
       $(OBJ)/MDBgcFecal.o\
       $(OBJ)/MDBgcRiverLight.o\
       $(OBJ)/MDBgcRiverGPP.o\
       $(OBJ)/MDDIN.o\
       $(OBJ)/MDDO2.o\
       $(OBJ)/MDNitrogenInputs.o\
       $(OBJ)/MDCParams.o\
       $(OBJ)/MDDischarge.o\
       $(OBJ)/MDDischLevel1.o\
       $(OBJ)/MDDischLevel2.o\
       $(OBJ)/MDDischLevel3.o\
       $(OBJ)/MDDischLevel3Accum.o\
       $(OBJ)/MDDischLevel3Cascade.o\
       $(OBJ)/MDDischLevel3Muskingum.o\
       $(OBJ)/MDDischLevel3MuskingumCoeff.o\
       $(OBJ)/MDDischMean.o\
       $(OBJ)/MDEnergyDemand.o\
       $(OBJ)/MDEvapotransp.o\
       $(OBJ)/MDGrossRad.o\
       $(OBJ)/MDLandCover.o\
       $(OBJ)/MDLSAreaIndex.o\
       $(OBJ)/MDIrrGrossDemand.o\
       $(OBJ)/MDIrrRefET.o\
       $(OBJ)/MDIrrRefETHamon.o\
       $(OBJ)/MDIrrRefETFAO.o\
       $(OBJ)/MDIrrSoilMoisture.o\
       $(OBJ)/MDIrrSoilMoistChg.o\
       $(OBJ)/MDIrrUptakeRiver.o\
       $(OBJ)/MDIrrigatedArea.o\
       $(OBJ)/MDIrrUptakeGrdWater.o\
       $(OBJ)/MDPrecipitation.o\
       $(OBJ)/MDPermafrost.o\
       $(OBJ)/MDRainInfiltration.o\
       $(OBJ)/MDRainIntercept.o\
       $(OBJ)/MDRainPotET.o\
       $(OBJ)/MDRainPotETHamon.o\
       $(OBJ)/MDRainPotETJensen.o\
       $(OBJ)/MDRainPotETPstd.o\
       $(OBJ)/MDRainPotETPsTaylor.o\
       $(OBJ)/MDRainPotETPMday.o\
       $(OBJ)/MDRainPotETPMdn.o\
       $(OBJ)/MDRainPotETSWGday.o\
       $(OBJ)/MDRainPotETSWGdn.o\
       $(OBJ)/MDRainSMoistChg.o\
       $(OBJ)/MDRainSMoistLayered.o\
       $(OBJ)/MDRainPotETTurc.o\
       $(OBJ)/MDRainSurfRunoff.o\
       $(OBJ)/MDRainWaterSurplus.o\
       $(OBJ)/MDPotETlib.o\
       $(OBJ)/MDRelHumidity.o\
       $(OBJ)/MDReservoirs.o\
       $(OBJ)/MDRiverbedShapeExp.o\
       $(OBJ)/MDRiverWidth.o\
       $(OBJ)/MDRunoff.o\
       $(OBJ)/MDRunoffMean.o\
       $(OBJ)/MDRunoffVolume.o\
       $(OBJ)/MDSPackChg.o\
       $(OBJ)/MDSmallReservoirs.o\
       $(OBJ)/MDSmallReservoirCap.o\
       $(OBJ)/MDSolarRad.o\
       $(OBJ)/MDSoilAvailWaterCap.o\
       $(OBJ)/MDSMoistChg.o\
       $(OBJ)/MDSurfRunoff.o\
       $(OBJ)/MDSurfRunoffPool.o\
       $(OBJ)/MDStormRunoff.o\
       $(OBJ)/MDWaterBalance.o\
       $(OBJ)/MDWetDays.o\
       $(OBJ)/MDWetlands.o\
       $(OBJ)/MDWetlandArea.o\
       $(OBJ)/MDWind.o\
       $(OBJ)/MDWTempRiver.o\
       $(OBJ)/MDWTempRiverRoute.o\
       $(OBJ)/MDWTempGrdWater.o\
       $(OBJ)/MDWTempSurfRunoff.o\
       $(OBJ)/MDWTempSurfRunoffPool.o\
       $(OBJ)/MDThermalInputs.o\
       $(OBJ)/MDThermalInputs2.o\
       $(OBJ)/MDThermalInputs3.o\
       $(OBJ)/MDWWTP.o\
       $(OBJ)/MDIrrigation.o\
       $(OBJ)/MDDOC.o\
       $(OBJ)/MDLitterFall.o\
       $(OBJ)/MDHydropower.o\
       $(OBJ)/MDSpecCond.o\
       $(OBJ)/MDChloride.o

all: ${TARGET}

clean:
	rm $(TARGET) $(OBJS)

$(TARGET): $(OBJS) $(SRC)/Makefile
	$(LINK) $(OBJS) $(LIBS) -o $(TARGET)

$(OBJ)/%.o: $(SRC)/%.c $(INCLUDES)
	$(CC) -o $(OBJ)/$*.o -c $(SRC)/$*.c
