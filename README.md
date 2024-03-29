# instrument-simulation
It is a small and flexible tool for charged particle simulations. 
The software is written on the course of the main author's PhD research.

The tool relies on the Geant4 framework. 
The Geant4 license explicitly forbids to use of the name "to endorse or promote software, or products derived therefrom, except with prior written permission by license@geant4.org" https://geant4.web.cern.ch/license/LICENSE.html. 
Thus, there is no "Geant4" in the name. 
The project is small and research-related, so it does not have to be official. 
Internally the project has the name "geant4-sim-gdml". 
It may change, but I see little reason to do that.

## Scope

The tool provides an interface for Geant4 that does not require a user to write
own C++ code to simulate particle interactions with a GDML-defined (GDML - Geometry Description Markup Language) piece of equipment.

The input data are:
- The GDML geometry definition.
- The particle source description.
- The list of sensitive detectors.
- The computation options such as the number of threads.

The output data is a ROOT data file that contains metadata about the simulation and an event-based table of energies deposited into sensitive detectors.
There is no analysis performed at the simulation stage since the primary aim of the software is to allow the flexible analysis
after a computation-heavy Monte-Carlo part has been completed. 

## History

I used custom code for different particle instruments for a couple of years
before I started this project. As the custom code evolved in one instrument, I had to implement the same changes in all software, which eventually became too tedious.
It may sound obvious and predictable for a professional programmer, but the amount
of time spent coding is almost negligible compared to the other research tasks.

## Other software
This software has been tested against SHELLDOSE and MULASSIS. 

[comment]: <> (One should check the GRAS software created by ESA if the total ionizing dose or internal)
[comment]: <> (charge are the required quantities. )

## Usage

`geant4-sim-gdml -i "/path/to/gdml" -o "/path/to/output/ROOT" <other options>...`

The command-line options are processed by getopt. Thus, a short option is `-a <value>` and
the long option is `--option=value`. If the software is used on a cluster with SLURM, it is advised to use long versions of the options to improve the readability of shell scripts.

The command-line options:

- `--input=gdmlfile` or `-i gdmlfile`: gdmlfile is the full name of a GDML file to use as input. This option is required.
- `--output=rootfile` or `-o rootfile`: rootfile is the full name of the ROOT output file. See option `-r` or `--randomid` for more detail.
- `--randomid` or `-r`: append a random hex number to the end of the output file name. Useful when the same SLURM script is executed more than once.
- `--execute=macro` or `-x macro`: macro is the main macro script for Geant4. Mind that sub-macros should have their path defined in the main macro script.
- `--threads=n` or `-c n`: n is an integer; sets the number of threads to use. It should be equal to the number of cores of the computing node.
- `--partnum=n` or `-n n`: n is an integer; the number of incident particles to simulate. Due to Geant4 limitations, a single run can not be larger than 2^31 - 1 particles. Larger values are allowed; the simulation is performed in multiple runs.
- `--particle=name` or `-p name`: the Geant4-compatible incident particle name. See option `--arbitrary` for more details.
- `--stepsize=d`: the user step size for sensitive volumes. The value `d` is in microns. Use only when you know what you are doing.
- `--powerlaw`: use the powerlaw distribution for the incident particle energies. See General Particle Source documentation for more detail.
- `--energylow=d`: the low energy limit for powerlaw in MeV.
- `--energyhigh=d`: the high energy limit for powerlaw in MeV.
- `--alpha=d`: the power index to use for powerlaw.
- `--mono`: use a monoenergetic particle source.
- `--gaussian` use gaussian distribution for the incident particle energies.
- `--energycenter=d`: the energy in MeV to use for `--mono` or the central energy for `--gaussian`.
- `--sigma=d`: the sigma value for `--gaussian`.
- `--useftfp`: use FTFP_BERT physics. See Reference Physics Lists in Geant4 for more detail.
- `--useqgsp`: use QGSP_BERT physics. 
- `--useqgsphp`: use QGSP_BERT physics with EM physics option 3 (slow). 
- `--useftfphp`: use FTFP_BERT physics with EM physics option 4 (even slower). Makes sense for high precision dosimetry.
- `--arbitrary`: allow arbitrary particle source definition in a macro file. See General Particle Source documentation for more detail.
- `--printstat`: print debug information on the logical volumes.


## Output file
The output ROOT file contains two N-tuples.
### "Detector Data"
Has records of <DetectorName> + "_Edep_MeV" and "_Esec_MeV". Edep is the full deposited energy
and Esec is the energy deposited by non-primary particle species.

### "Simulation Data"
Has several metadata about the simulation. The fields are written by each thread; 
thus,to obtain the total particle number one may want to use 
```python
import uproot
import numpy as np
f = uproot.reading.open(...)
sim = f["Detector Data"]
data = f["Simulation Data"]
particle_number = np.sum(data['Particle Number'].array())
```


## Limitations

It is impossible to pack data from several runs into a single ROOT file.
Worker threads are destroyed each time a run ends, and unless there is an external
a non-Geant4 tool that opens, appends, and closes the ROOT file (which is ridiculous),
the data of the worker thread is lost.

It is impossible to run more than 2^31 - 1 particles in a single run. This limitation
is not a priority in Geant4 development.
