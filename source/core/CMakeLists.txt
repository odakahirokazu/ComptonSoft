find_package(HDF5 REQUIRED COMPONENTS C)

include_directories(
  include
  include/nanograms
  ${ANLG4_INC_DIR}
  ${ANLNEXT_INC_DIR}
  ${ROOT_INC_DIR}
  ${G4_INC_DIR}
  ${CLHEP_INC_DIR}
  ${ORT_INC_DIR}
  ${ADD_INC_DIR}
  ${BOOST_INC_DIR}
  ${HDF5_INCLUDE_DIRS}
)

link_directories(
  ${ANLNEXT_LIB_DIR}
  ${ROOT_LIB_DIR}
  ${G4_LIB_DIR}
  ${CLHEP_LIB_DIR}
  ${BOOST_LIB_DIR}
  )

set(CS_CORE_CLASSES
  ### basic types and classes
  src/CSException.cc
  src/ChannelID.cc
  src/VoxelID.cc
  src/DetectorGroup.cc
  src/HitPattern.cc
  src/PhaseSpaceVector.cc
  src/IsotopeInfo.cc
  ### processing/readout
  src/VGainFunction.cc
  src/GainFunctionLinear.cc
  src/GainFunctionCubic.cc
  src/GainFunctionSpline.cc
  src/MultiChannelData.cc
  src/VChannelMap.cc
  src/ChannelMap.cc
  src/ChannelMapDSD.cc
  src/ReadoutModule.cc
  src/DetectorHit.cc
  ### detector units
  src/VDetectorUnit.cc
  src/VRealDetectorUnit.cc
  src/RealDetectorUnit2DPixel.cc
  src/RealDetectorUnit2DStrip.cc
  src/RealDetectorUnitScintillator.cc
  src/RealDetectorUnit3DVoxel.cc
  ### device simulation
  src/EFieldModel.cc
  src/VDeviceSimulation.cc
  src/DeviceSimulation.cc
  src/SimDetectorUnit2DPixel.cc
  src/SimDetectorUnit2DStrip.cc
  src/SimDetectorUnitScintillator.cc
  src/SimDetectorUnit3DVoxel.cc
  src/WeightingPotentialPixel.cc
  src/WeightingPotentialStrip.cc
  ### detector unit factory
  src/VDetectorUnitFactory.cc
  src/RealDetectorUnitFactory.cc
  src/SimDetectorUnitFactory.cc
  ### Compton reconstruction
  src/BasicComptonEvent.cc
  src/VEventReconstructionAlgorithm.cc
  src/StandardEventReconstructionAlgorithm.cc
  src/PhotoAbsorptionEventReconstructionAlgorithm.cc
  src/FocalPlaneEventReconstructionAlgorithm.cc
  src/SGDEventReconstructionAlgorithm.cc
  src/ComptonConstraints.cc
  src/HY2017EventReconstructionAlgorithm.cc
  src/HY2020EventReconstructionAlgorithm.cc
  src/OberlackAlgorithm.cc
  src/TangoAlgorithm.cc
  src/nanograms/NanoGRAMSEventReconstructionAlgorithm.cc
  src/nanograms/NanoGRAMSEnergyComparisonEventReconstructionAlgorithm.cc
  src/nanograms/NanoGRAMSKNFormulaEventReconstructionAlgorithm.cc
  ### sensitive detector
  src/VCSSensitiveDetector.cc
  ### event store
  src/CSEventStore.cc
  ### manager
  src/DetectorSystem.cc
  ### tree IO
  src/HitTreeIO.cc
  src/ComptonEventTreeIO.cc
  src/EventTreeIO.cc
  src/InitialInfoTreeIO.cc
  src/HitTreeIOWithInitialInfo.cc
  src/ComptonEventTreeIOWithInitialInfo.cc
  src/EventTreeIOWithInitialInfo.cc
  ### frame readout
  src/FrameData.cc
  src/OutlierStore.cc
  src/XrayEvent.cc
  src/PolarizedXrayEvent.cc
  src/XrayEventTreeIO.cc
  ### coded aperture
  src/VCodedAperture.cc
  src/CodedAperture.cc
  ### charge transport
  src/VGoalRegion.cc
  src/RectangularGoalRegion.cc
  src/SemiconductorModel.cc
  src/PointCharge.cc
  src/CCECalculation.cc
  src/NumericalField.cc
  ### radioactivation step 2 (RDChains)
  src/DummyDetectorConstruction.cc
  src/IsotopeDatabaseAccess.cc
  src/RateData.cc
  src/TimeProfile.cc
  src/BatemanSolution.cc
  src/RIDecayProperties.cc
  src/RIDecayChains.cc
  src/RIDecayCalculation.cc
  src/nanograms/NanoGRAMSChargeToEnergySpline.cc
  src/nanograms/NanoGRAMSTPCProperty.cc
  src/nanograms/NanoGRAMSFECGeometry.cc
  src/nanograms/NanoGRAMSConfig.cc
  src/nanograms/NanoGRAMSCalibrationData.cc
  src/nanograms/NanoGRAMSHitCalibrator.cc
  src/nanograms/NanoGRAMSLightAnalysis.cc
  src/nanograms/NanoGRAMSQuickLookTreeIO.cc
  src/nanograms/NanoGRAMSTPCTreeIO.cc
  src/nanograms/NanoGRAMSTPCDataProcessor.cc
  )

if(CS_USE_ORT)
  set(CS_ORT_CLASSES
    src/OnnxInference.cc
    src/NeuralNetST2022EventReconstructionAlgorithm.cc
    )
endif()

add_library(CSCore SHARED
  ${CS_CORE_CLASSES}
  ${CS_ORT_CLASSES}
  )

set_source_files_properties(
  src/nanograms/NanoGRAMSCalibrationData.cc
  src/nanograms/NanoGRAMSQuickLookTreeIO.cc
  src/nanograms/NanoGRAMSTPCDataProcessor.cc
  src/nanograms/NanoGRAMSTPCProperty.cc
  PROPERTIES
  COMPILE_OPTIONS "-std=c++20"
)

target_link_libraries(CSCore
  ${ANLG4_LIB}
  ${ANLNEXT_LIB}
  ${ROOT_LIB} 
  ${G4_LIB} 
  ${CLHEP_LIB} 
  ${ORT_LIB} 
  ${ADD_LIB} 
  ${BOOST_LIB}
  ${HDF5_LIBRARIES} 
  ${YAMLCPP_LIB}
)

install(TARGETS CSCore LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

if(INSTALL_HEADERS)
  install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/comptonsoft
    FILES_MATCHING PATTERN "*.hh")
endif(INSTALL_HEADERS)
