add_library(ComptonSoftApplications)

set(ComptonSoftApplications_DataProcess_Classes)

set(ComptonSoftApplications_DataProcess_Modules
  ### processing analysis
  data_process/src/CorrectPHA.cc
  data_process/src/RecalculateEPI.cc
  data_process/src/CalculatePedestalLevels.cc
  data_process/src/InitialConditionFilter.cc
  data_process/src/WeightByInitialDirection.cc
  data_process/src/RecalculateSimulationNoise.cc
  data_process/src/SelectEventsOnFocalPlane.cc
  data_process/src/SelectTime.cc
  data_process/src/FilterByGoodTimeIntervals.cc
  ### IO
  data_process/src/ReadDataFile.cc
  data_process/src/ReadDataFile_VME3.cc
  data_process/src/ReadDataFile_SpW2.cc
  data_process/src/ReadDataFile_NT.cc
  data_process/src/ReadDataFile_NB0.cc
  ### basic histograming
  data_process/src/HistogramPHA.cc
  data_process/src/HistogramEnergySpectrum.cc
  data_process/src/QuickAnalysisForDSD.cc
)

set(ComptonSoftApplications_ComptonCamera_Classes
  ### Compton reconstruction
  compton_camera/src/VEventReconstructionAlgorithm.cc
  compton_camera/src/StandardEventReconstructionAlgorithm.cc
  compton_camera/src/PhotoAbsorptionEventReconstructionAlgorithm.cc
  compton_camera/src/FocalPlaneEventReconstructionAlgorithm.cc
  compton_camera/src/SGDEventReconstructionAlgorithm.cc
  compton_camera/src/ComptonConstraints.cc
  compton_camera/src/HY2017EventReconstructionAlgorithm.cc
  compton_camera/src/HY2020EventReconstructionAlgorithm.cc
  compton_camera/src/OberlackAlgorithm.cc
  compton_camera/src/TangoAlgorithm.cc
  ### IO
  compton_camera/src/ComptonEventTreeIO.cc
  compton_camera/src/ComptonEventTreeIOWithInitialInfo.cc
)

if(CS_USE_ORT)
  set(ComptonSoftApplications_ComptonCamera_Classes
    ${ComptonSoftApplications_ComptonCamera_Classes}
    compton_camera/src/OnnxInference.cc
    compton_camera/src/NeuralNetST2022EventReconstructionAlgorithm.cc
  )
endif()

set(ComptonSoftApplications_ComptonCamera_Modules
  compton_camera/src/EventReconstruction.cc
  compton_camera/src/SelectFullDeposit.cc
  compton_camera/src/ComptonEventFilter.cc
  ### histograming
  compton_camera/src/HistogramEnergy1D.cc
  compton_camera/src/HistogramEnergy2D.cc
  compton_camera/src/HistogramARM.cc
  compton_camera/src/HistogramARMByPositionMeasurement.cc
  compton_camera/src/HistogramAzimuthAngle.cc
  compton_camera/src/Histogram2DDeltaEnergyWithARM.cc
  compton_camera/src/ResponseMatrix.cc
  compton_camera/src/BackProjection.cc
  compton_camera/src/BackProjectionSky.cc
  compton_camera/src/EfficiencyMapSky.cc
  ### IO
  compton_camera/src/WriteComptonEventTree.cc
  compton_camera/src/ReadComptonEventTree.cc
)

set(ComptonSoftApplications_XrayImager_Classes
  xray_imager/src/XrayEventTreeIO.cc
)

set(ComptonSoftApplications_XrayImager_Modules
  ### Frame data processes
  xray_imager/src/AssignTime.cc
  xray_imager/src/DefineFrame.cc
  xray_imager/src/ConstructFrame.cc
  xray_imager/src/FillFrame.cc
  xray_imager/src/LoadFrame.cc
  xray_imager/src/LoadReducedFrame.cc
  xray_imager/src/XrayEventCollection.cc
  xray_imager/src/XrayEventSelection.cc
  xray_imager/src/AnalyzeFrame.cc
  xray_imager/src/WriteXrayEventTree.cc
  xray_imager/src/ReadXrayEventTree.cc
  xray_imager/src/SortEventTreeWithTime.cc
  xray_imager/src/WriteBadPixels.cc
  xray_imager/src/AnalyzeDarkFrame.cc
  xray_imager/src/LoadRootFrame.cc
  xray_imager/src/DetectBadFrames.cc
  xray_imager/src/SetBadFrames.cc
  xray_imager/src/SetDynamicPedestals.cc
  xray_imager/src/SetPedestalsByMedian.cc
  xray_imager/src/HistogramFramePedestal.cc
  xray_imager/src/HistogramXrayEventSpectrum.cc
  xray_imager/src/HistogramXrayEventAzimuthAngle.cc
  xray_imager/src/HistogramXrayEventProperties.cc
  xray_imager/src/HistogramRawFrameImage.cc
  xray_imager/src/ExtractXrayEventImage.cc
  xray_imager/src/GetInputFilesFromDirectory.cc
  xray_imager/src/SelectEventsWithDetectorSpectrum.cc
  xray_imager/src/ExtractPhotoelectronTrajectory.cc
)

if(CS_USE_FITSIO)
  set(ComptonSoftApplications_XrayImager_Modules
    ${ComptonSoftApplications_XrayImager_Modules}
    ### Frame data processes
    xray_imager/src/MakeFrameFITS.cc
    xray_imager/src/WritePedestals.cc
    xray_imager/src/SetPedestals.cc
    xray_imager/src/SelectEventsWithCelestialSpectrum.cc
  )
endif()

if(CS_USE_HSQUICKLOOK)
  set(ComptonSoftApplications_XrayImager_Modules
    ${ComptonSoftApplications_XrayImager_Modules}
    xray_imager/src/PushToQuickLookDB.cc
    xray_imager/src/PushXrayEventToQuickLookDB.cc
    xray_imager/src/PushHistogramToQuickLookDB.cc
    xray_imager/src/LoadMetaDataFile.cc
    xray_imager/src/ExtractXrayEventImageFromQuickLookDB.cc
  )
endif()

set(ComptonSoftApplications_CodedAperture_Classes
  ### coded aperture
  coded_aperture/src/VCodedAperture.cc
  coded_aperture/src/CodedAperture.cc
)

set(ComptonSoftApplications_CodedAperture_Modules
  ### coded aperture
  coded_aperture/src/ProcessCodedAperture.cc
  coded_aperture/src/HistogramDecodedImage.cc
)

set(ComptonSoftApplications_ChargeTransport_Classes
  ### charge transport
  charge_transport/src/VGoalRegion.cc
  charge_transport/src/RectangularGoalRegion.cc
  charge_transport/src/SemiconductorModel.cc
  charge_transport/src/PointCharge.cc
  charge_transport/src/CCECalculation.cc
  charge_transport/src/NumericalField.cc
)

set(ComptonSoftApplications_Radioactivation_Classes
  ### radioactivation step 2 (RDChains)
  radioactivation/src/DummyDetectorConstruction.cc
  radioactivation/src/IsotopeDatabaseAccess.cc
  radioactivation/src/RateData.cc
  radioactivation/src/TimeProfile.cc
  radioactivation/src/BatemanSolution.cc
  radioactivation/src/RIDecayProperties.cc
  radioactivation/src/RIDecayChains.cc
  radioactivation/src/RIDecayCalculation.cc
)

set(ComptonSoftApplications_Radioactivation_Modules
  ### radioactivation simulation
  radioactivation/src/RadioactivationEventStore.cc
  radioactivation/src/RadioactivationStackingAction.cc
  radioactivation/src/RadioactivationUserActionAssembly.cc
  radioactivation/src/RadioactivationTreeIO.cc
  radioactivation/src/RadioactiveDecayUserActionAssembly.cc
)

set(ComptonSoftApplications_Missions_ASTROH_Classes
  missions/astroh/src/SGDEventTree.cc
  # missions/astroh/src/ReadHXIEventTree.cc
  missions/astroh/src/ReadSGDEventTree.cc
  missions/astroh/src/SimulateCXBShieldPlate.cc
  missions/astroh/src/RescaleSimulationNoiseOfSGDSiUntriggered.cc
  missions/astroh/src/UniformlyRandomizeEPI.cc
)

set(ComptonSoftApplications_Missions_ASTROH_Modules
  ### HXI/SGD
  missions/astroh/src/ComptonModeFilter.cc # for old SGD
  missions/astroh/src/HXIEventSelection.cc
  ### simulation
  ### primary geneator
  missions/astroh/src/AHRadiationBackgroundPrimaryGenerator.cc
  ### user actions
  missions/astroh/src/SampleOpticalDepth.cc
  ### others
  missions/astroh/src/AssignG4CopyNumber.cc
  missions/astroh/src/DumpMass.cc
)

if(CS_USE_FITSIO)
  set(ComptonSoftApplications_Missions_ASTROH_Modules
    ${ComptonSoftApplications_Missions_ASTROH_Modules}
    ### data process
    missions/astroh/src/AEAttitudeCorrection.cc
    missions/astroh/src/SGDEventFITS.cc
    missions/astroh/src/ReadSGDEventFITS.cc
    missions/astroh/src/WriteSGDEventFITS.cc
    missions/astroh/src/HXIEventFITS.cc
    missions/astroh/src/ReadHXIEventFITS.cc
    missions/astroh/src/WriteHXIEventFITS.cc
    missions/astroh/src/FilterByGoodTimeIntervalsForSGD.cc
    missions/astroh/src/FilterByGoodTimeIntervalsForHXI.cc
    missions/astroh/src/SXIFrameData.cc
    missions/astroh/src/SXIXrayEvent.cc
    missions/astroh/src/ConstructSXIFrame.cc
    missions/astroh/src/AssignSXIGrade.cc
    missions/astroh/src/AEAssignWeightWithResponseMatrix.cc
    ### simulation
    missions/astroh/src/AHRayTracingPrimaryGenerator.cc
    missions/astroh/src/AEObservationPrimaryGenerator.cc
    missions/astroh/src/CelestialSourcePrimaryGenerator.cc
  )
endif(CS_USE_FITSIO)

if(CS_USE_SIMX)
  set(ComptonSoftApplications_Missions_ASTROH_Modules
    ${ComptonSoftApplications_Missions_ASTROH_Modules}
    ### basic interface
    missions/astroh/src/SimXIF.cc
    ### primary generator
    missions/astroh/src/SimXPrimaryGenerator.cc
    ### pick up data
    missions/astroh/src/AHStandardUserActionAssembly.cc
    ### other
    missions/astroh/src/OutputSimXPrimaries.cc
    missions/astroh/src/GenerateSimXEvent.cc
  )
endif(CS_USE_SIMX)

target_sources(ComptonSoftApplications
  PRIVATE
  ${ComptonSoftApplications_DataProcess_Classes}
  ${ComptonSoftApplications_DataProcess_Modules}
  ${ComptonSoftApplications_ComptonCamera_Classes}
  ${ComptonSoftApplications_ComptonCamera_Modules}
  ${ComptonSoftApplications_XrayImager_Classes}
  ${ComptonSoftApplications_XrayImager_Modules}
  ${ComptonSoftApplications_CodedAperture_Classes}
  ${ComptonSoftApplications_CodedAperture_Modules}
  ${ComptonSoftApplications_ChargeTransport_Classes}
  ${ComptonSoftApplications_ChargeTransport_Modules}
  ${ComptonSoftApplications_Radioactivation_Classes}
  ${ComptonSoftApplications_Radioactivation_Modules}
  ${ComptonSoftApplications_Missions_ASTROH_Classes}
  ${ComptonSoftApplications_Missions_ASTROH_Modules}
)

target_include_directories(ComptonSoftApplications
  PUBLIC
  ${ORT_INC_DIR}
  ${CFITSIO_INC_DIR}
  ${MONGOCXX_INC_DIR}
  ${HSQUICKLOOK_INC_DIR}
  data_process/include
  compton_camera/include
  xray_imager/include
  coded_aperture/include
  charge_transport/include
  radioactivation/include
  missions/astroh/include
)

target_link_libraries(ComptonSoftApplications
  ComptonSoftCore
  ${ORT_LIB}
  ${CFITSIO_LIB}
  ${MONGOCXX_LIB}
  ${HSQUICKLOOK_LIB}
)

install(TARGETS ComptonSoftApplications
  LIBRARY
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

if(INSTALL_HEADERS)
  install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/comptonsoft
    FILES_MATCHING PATTERN "*.hh")
endif(INSTALL_HEADERS)
