# instrument-simulation
It is a small and flexible tool for charged particle simulations.

The tool relies on the Geant4 framework. The Geant4 license explicitly forbids to use of the name "to endorse or promote software, or products derived therefrom, except with prior written permission by license@geant4.org" https://geant4.web.cern.ch/license/LICENSE.html. Thus, there is no "Geant4" in the name. The project is small and research-related, so it does not have to be official.

Internally the project has the name "geant4-sim-gdml". It may change, but I see infinitesimal reasons to do that.

```cpp
void print_usage() {
    G4cout << G4endl;
    G4cout << "Usage: load_gdml " << G4endl
           << "-c [--threads] <number of threads>" << G4endl
           << "-e [--energylow | --energycenter] <low energy margin or central energy (use -m/--mono), MeV>" << G4endl
           << "-y [--energyhigh] <high energy margin, MeV>" << G4endl
           << "-g [--gaussian] use Gaussian distribution" << G4endl
           << "-i [--inputfile] <input gdml file : mandatory>" << G4endl
           << "-l [--powerlaw] use powerlaw distribution" << G4endl
           << "-m [--mono] use monoenergetic beam" << G4endl
           << "-n <number of particles to be simulated>" << G4endl
           << "-o [--output] <output file name [default:output.root]>" << G4endl
           << "-p [--particle] <particle name [default:geantino]>" << G4endl
           << "-q [--quick] enable quick physics (standard EM)" << G4endl
           << "-r [--randomid] enable random seed printed as the output suffix" << G4endl
           << "-s [--sigma] <gaussian sigma, MeV>" << G4endl
           << "-x [--execute] <macro file name>" << G4endl
           << "-z [--printstat] print statistics on logical volumes" << G4endl;

    G4cout << G4endl;
}
```
