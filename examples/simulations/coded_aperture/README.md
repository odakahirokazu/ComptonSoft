How to run simulations of coded apertures.
========================================

#### (1) Construct a mass model gdml file from aperture pattern file.

    ./construct_mass_model.rb
    Generated mass model file will be posed under mass_models/.

#### (2) Run simulation.

    ./run_simulation.rb

#### (3) Make frame data and extract events.

    ./analyze_event_tree.rb

#### (4) Apply decoding.

    ./process_coded_aperture.rb
    