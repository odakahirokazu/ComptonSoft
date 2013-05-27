if(USE_GDML)
  set(app_list
    simHXISGD_PW
    simHXISGD_PW_RawHit
    simHXISGD_PW_Pol
    simHXISGD_Flat
    simHXISGD_Flat_RawHit
    simHXISGD_Flat_Pol
    simHXISGD_PS
    simHXISGD_PS_RawHit
    simHXISGD_Isot
    simHXISGD_InVolume
    simHXISGD_RD_TR
    simHXISGD_RDInVolume_TR
    # viewGDML
    )

  include(exe)
endif(USE_GDML)