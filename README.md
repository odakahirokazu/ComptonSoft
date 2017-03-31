Compton Soft
================================================================

- Version: 5.2.11
- Author: Hirokazu Odaka

----

Introduction
----------------------------------------------------------------

Compton Soft is an integrated simulation and analysis software package
for semiconductor radiation detectors including Compton cameras.
This software provides

- processing data obtained with multi-layer semiconductor detector systems,
- Monte Carlo simulations of the semiconductor detector systems, and
- data reduction/analysis including Compton reconstruction.

This software package can be used for wide-range applications of radiation
measurements: X-/gamma-ray astrophysics, nuclear medicine, and imaging
search for radioactive sources.

### Migration to version 5.2

Version 5.2 introduces new design of XML database since it supports new
features. We provide conversion scripts from version 5.1 formats.
Please also check examples about new formts.

### Important notes for version 5.0 users

Version 5.1 was the first big minor update since we released version 5.
This update included many design changes, revision of input/output
formats, and several bug fixes. If you are using version 5.0, we strongly
recommend updating to version 5.2 as 5.0 versions are not stable and use
input/output fotmats which are no longer supported.

### Changes from version 4

When we move to version 5, all software design has been reviewed for C++11
and ANL Next 1.7. We adopt new database schema:

- detector configuration,
- channel map,
- simulation parameters,
- analysis parameters.

We also change data formats, which are used as simulation output:

- hit tree,
- event tree,
- compton event tree (cetree).

The event tree is a new data format that has the same information in the
hit tree but it keeps information in a single row for an event in which
multiple hits are filled into a variable length array. In the hit tree
format, on the other hand, a row corresponds to a single hit. You can
choose either hit tree or event tree as you like.

To migrate to version 5 from version 4, please check an example script for
simulation. Please feel free to contact us if you have any question about
the migration.

If you like to use the old version, we recommend using version 4.15.0
(tagged in the git repository), the latest version of the version 4
series, together with ANL Next 1.6.2. The version 4 series are not
compatible with ANL Next 1.7 or later.

### Platforms

- Mac: the author's standard environment
- Linux

----

Documentation
----------------------------------------------------------------

- [Installation](documentation/installation.md)
- [Data format](documentation/data_format.md)
- [Class library references](http://web.stanford.edu/~hodaka/comptonsoft/doxygen/index.html)
- [ANL module references](http://web.stanford.edu/~hodaka/comptonsoft/cs_modules.xml)

Information
----------------------------------------------------------------

### Contact

- Hirokazu Odaka 
- KIPAC, Stanford University
- hodaka(AT)stanford.edu

### Contributions

- Shin Watanabe
- Shin'ichiro Takeda
- Tamotsu Sato
- Yuto Ichinohe

### GitHub

<https://github.com/odakahirokazu/ComptonSoft/>

### References (academic)

- Odaka et al. (2010) NIM A, 624, 303

### History

- 5.2 | 2016-09-07 | H. Odaka | New design of XML database. Timing process.
- 5.1 | 2015-10-19 | H. Odaka | Stable version. A big minor update since 5.0.
- 5.0 | 2015-08-17 | H. Odaka | For C++11 and ANL Next 1.7.
- 4.9.0 | 2013-05-21 | H. Odaka |  Beta version for version 5
- 4.8.0 | 2011-11-15 | H. Odaka |
- 4.7.0 | 2011-07-29 | H. Odaka | Ruby binding, parallel geometry
- 4.6.0 | 2011-07-21 | H. Odaka | ANL Next 1.0
- 4.5.6 | 2011-07-13 | H. Odaka | analysis for CCMk5, event: NDIP2011
- 4.5.5 | 2011-06-xx | H. Odaka | analysis for CCMk5/development
- 4.5.4 | 2011-06-08 | H. Odaka | CMake
- 4.5 | 2011-04-26 H. Odaka | ANL Next
- 4.4 | 2011-04-04 H. Odaka | introduce sensitive detector
- Version 4 (2009) H. Odaka | General treatment of the detector response (Sugimoto's Master thesis)
- Version 3 (2008) H. Odaka | Treatment of activation background (Odaka), detector response of semiconductor detectors (Takeda's PhD thesis, Aono's Master thesis)
- Version 2 (2007) H. Odaka | Monte Carlo simulation (Odaka's Master Thesis)
- Version 1 (2007) H. Odaka | Data reduction of Si/CdTe Compton cameras (Odaka's Master Thesis)

****************************************************************
