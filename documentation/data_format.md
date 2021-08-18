Data Format | Compton Soft
================================================================

[Home](../README.md)

 1. Event Data
----------------------------------------------------------------

### Hit Tree

Hit Tree is a default output format of ComptonSoft. This TTree object
contains information on hits detected in a detector system. The name of
the object is *hittree*. There are 46 branches:

#### [0] eventid (signed 8-byte integer)
Event ID to which the hit belongs.

#### [1] ihit (signed 2-byte integer)
Index of the hits in this event.

#### [2] num_hits (signed 4-byte integer)
Number of hits in this event.

#### [3] ti (signed 8-byte integer)
Time Indicator: this is a measured time by a detector readout system
clock.

#### [4] instrument (signed 2-byte integer)
Instument ID.

#### [5] detector (signed 2-byte integer)
Detector ID.

#### [6] det_section (signed 2-byte integer)
Section index of detector-based channel ID.

#### [7] readout_module (signed 2-byte integer)
Readout module ID.

#### [8] section (signed 2-byte integer)
Section index of readout-based channel ID.

#### [9] channel (signed 2-byte integer)
Channel index (common regardless of detetor-base of readout-base).

#### [10] pixelx (signed 2-byte integer)
Pixel index in the x-axis.

#### [11] pixely (signed 2-byte integer)
Pixel index in the y-axis.

#### [12] rawpha (signed 4-byte integer)
Measured PHA value (raw ADC value).

#### [13] pha (4-byte floating point)
PHA (pulse height amplitude) value.
This branch is used for a corrected value (e.g. pedestal,
common-mode noise corrections are applied).

#### [14] epi (4-byte floating point, keV)
Energy value obtained from the PHA value after gain correction.
(EPI stands for Energy pulse height invariant).

#### [15] epi_error (4-byte floating point, keV)
Error of EPI.

#### [16] flag_data (unsigned 8-byte integer)
Flag data assigned by the detector readout system.

#### [17] flags (unsigned 8-byte integer)
Flags used in Compton Soft.

#### [18] trackid (signed 4-byte integer)
Track ID in Geant4 simulation.

#### [19] particle (signed 4-byte integer)
Particle ID (PDG particle code) in Geant4 simulation.

#### [20] real_time (8-byte floating point)
Raw simulation output of time.

#### [21] time_trig (8-byte floating point)
Time of the trigger.

#### [22] time_group (signed 2-byte integer)
Time group ID. This ID distinguishs different readout groups at temporally
separate readout (different trigger) but belong to the same event ID.
This can happen if a radioactive primary makes decays distributed in a
period longer than a detector readout timescale.

#### [23] real_posx (4-byte floating point)
Raw simulation output of position x.

#### [24] real_posy (4-byte floating point)
Raw simulation output of position y.

#### [25] real_posz (4-byte floating point)
Raw simulation output of position z.

#### [26] edep (4-byte floating point)
Raw simulation output of energy deposit.
If a quenching factor is not unity, this factor is multiplied.

#### [27] echarge (4-byte floating point)
Energy deposit multiplied by charge collection efficiency.

#### [28] process (unsigned 4-byte integer)
Process flags.

#### [29] energy (4-byte floating point)
Reconstructed energy.

#### [30] energy_error (4-byte floating point)
Error of reconstructed energy.

#### [31] posx (4-byte floating point)
Reconstructed position x.

#### [32] posy (4-byte floating point)
Reconstructed position y.

#### [33] posz (4-byte floating point)
Reconstructed position z.

#### [34] posx_error (4-byte floating point)
Error of reconstructed position x.

#### [35] posy_error (4-byte floating point)
Error of reconstructed position y.

#### [36] posz_error (4-byte floating point)
Error of reconstructed position z.

#### [37] local_posx (4-byte floating point)
Reconstructed position x in detector-local coordicates.

#### [38] local_posy (4-byte floating point)
Reconstructed position y in detector-local coordicates.

#### [39] local_posz (4-byte floating point)
Reconstructed position z in detector-local coordicates.

#### [40] local_posx_error (4-byte floating point)
Error of reconstructed position x in detector-local coordicates.

#### [41] local_posy_error (4-byte floating point)
Error of reconstructed position y in detector-local coordicates.

#### [42] local_posz_error (4-byte floating point)
Error of reconstructed position z in detector-local coordicates.

#### [43] time (8-byte floating point)
Reconstructed time.

#### [44] time_error (8-byte floating point)
Error of reconstructed time.

#### [45] grade (signed 4-byte integer)
Grade of this event.

#### [46] ini_energy (4-byte floating point)
Energy of the primary particle of this event.

#### [47] ini_dirx (4-byte floating point)
Direction x of the primary particle of this event.

#### [48] ini_diry (4-byte floating point)
Direction y of the primary particle of this event.

#### [49] ini_dirz (4-byte floating point)
Direction z of the primary particle of this event.

#### [50] ini_time (8-byte floating point)
Time of the primary particle of this event.

#### [51] ini_posx (4-byte floating point)
Position x of the primary particle of this event.

#### [52] ini_posy (4-byte floating point)
Position y of the primary particle of this event.

#### [53] ini_posz (4-byte floating point)
Position z of the primary particle of this event.

#### [54] ini_polarx (4-byte floating point)
Polarization vector x of the primary particle of this event.

#### [55] ini_polary (4-byte floating point)
Polarization vector y of the primary particle of this event.

#### [56] ini_polarz (4-byte floating point)
Polarization vector z of the primary particle of this event.

#### [57] weight (4-byte floating point)
Statistical weight of this event.

### Event Tree

Event Tree has almost the same information as the Hit Tree. But this tree
has different format with a variable-length array.
The object name is *eventtree*.

### Compton Event Tree

The object name is *cetree*.

 2. Database for Simulations and Analysis
----------------------------------------------------------------

All database formats are written in XML and support XSLT stylesheet.
You can view the contents of the database by your web browser.
You can see concrete samples in [examples/simulations](https://github.com/odakahirokazu/ComptonSoft/tree/master/examples/simulations) directory.

![Example of detector configuration](./detector_configuration_xslt.png)

### Detector Configuration

This database file describes detector configuration which includes
detector ID, name, type, geometry, pixel segmentation, position of the
detectors. This file is mandatory for simulations. This database also
desribes readout orders in serialized data readout if this information is
needed. The database can also define detector groups and hit patterns.

### Channel Map

This database file describes relation between geometrical pixels and
electrical readout channels. This database is optional, and is not
necessary if you do not want to simulate readout configuration of your
real detector.

### Detector Parameters

This database file contains information on detector parameters. This file
is mandatory for simulations. Important required information is mapping
of physical volumes in Monte-Carlo simulation to real detectors. This
database can also describe detector parameters used for simulations and
analysis such as noise levels, diffusion properties, and threshold values.

### Channel Properties

This database file contains information on properties associated with
readout channels. You can specify detailed characteristics of each
channel. This file is optional.

****************************************************************
