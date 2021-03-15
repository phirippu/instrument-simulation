// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************

/*********************************************
 * Partially reused code of G01, B3, B1,
 * and other examples provided by
 * the Geant4 v.10.x.x distributions
 *
 * (c) Philipp Oleynik 2018-2021
 * The University of Turku
 *
**********************************************/

#include "AaltoPhysicsList.hh"
#include "AnalysisManager.hh"
#include "DetectorConstruction.hh"
#include "G4GDMLParser.hh"
#include "G4RunManager.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include <ActionInitialization.hh>
#include <G4MTRunManager.hh>
#include <FTFP_BERT.hh>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <getopt.h>
#include <boost/format.hpp>

G4bool b_tuple_written;

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

int main(int argc, char **argv) {

    AaltoPhysicsList *phys = nullptr;
    G4Random::setTheEngine(new CLHEP::RanecuEngine);
    G4String physName = "HADRONTHERAPY_1";
    G4String strPart = G4String("geantino");
    G4bool set_and_use_gaussian = FALSE;
    G4bool set_and_use_monoenergy = FALSE;
    G4bool set_and_use_powerlaw = FALSE;
    G4bool visOpen = TRUE;
    G4bool massPrinted = FALSE;
    G4bool randomID = FALSE;
    G4bool use_ftfp = FALSE;
    G4UIExecutive *ui = nullptr;
    G4VisManager *visManager = nullptr;
    G4double particleBeamRadius = 10 * cm;
    G4double particleEnergyMeV_E1 = 0.001;
    G4double particleEnergyMeV_E2 = 0.001;
    G4double particle_sigma = 0.001;
    G4double powerlaw_alpha = 0;
    G4int nThreads = G4Threading::G4GetNumberOfCores();
    G4int particlesNumber = 1;
    char *ptr;

    G4Random::setTheEngine(new CLHEP::RanecuEngine);
    long seed;

    int randomData = open("/dev/urandom", 0);
    if (randomData < 0) {
        G4cerr << "something went wrong with random" << G4endl;
        exit(1);
    } else {
        ssize_t result = read(randomData, &seed, sizeof seed);
        if (result < 0) {
            G4cerr << "something went really wrong with random" << G4endl;
            exit(1);
        }
    }

    CLHEP::HepRandom::setTheSeed(seed);
    G4Random::setTheSeed(seed);

    G4String gdml_filename = G4String("../block.gdml");
    G4String output_ROOT_FileName = "output";
    G4String macro_filename = "run.mac";

    static struct option long_options[] =
            {
                    {"particle",     optional_argument, nullptr, 'p'},
                    {"output",       optional_argument, nullptr, 'o'},
                    {"threads",      optional_argument, nullptr, 'c'},
                    {"energylow",    optional_argument, nullptr, 'e'},
                    {"energycenter", optional_argument, nullptr, 'e'},
                    {"energyhigh",   optional_argument, nullptr, 'y'},
                    {"gaussian",     optional_argument, nullptr, 'g'},
                    {"powerlaw",     optional_argument, nullptr, 'l'},
                    {"alpha",        optional_argument, nullptr, 'a'},
                    {"mono",         optional_argument, nullptr, 'm'},
                    {"sigma",        optional_argument, nullptr, 's'},
                    {"quick",        optional_argument, nullptr, 'q'},
                    {"randomid",     optional_argument, nullptr, 'r'},
                    {"inputfile",    required_argument, nullptr, 'i'},
                    {"execute",      optional_argument, nullptr, 'x'},
                    {"printstat",    optional_argument, nullptr, 'z'},
                    {nullptr, 0,                        nullptr, 0}
            };
    const char *const short_options = ":a:c:e:y:glmi:n:o:p:qrs:x:z";
    int opt;
    while ((opt = getopt_long(argc, argv, short_options, long_options, nullptr)) != -1) {
        switch (opt) {
            case 'a':
                powerlaw_alpha = strtod(optarg, &ptr);
                G4cout << "Alpha set to " << powerlaw_alpha << G4endl;
                break;
            case 'c':
                nThreads = strtol(optarg, &ptr, 10);
                G4cout << "N threads set to " << nThreads << G4endl;
                break;
            case 'e':
                particleEnergyMeV_E1 = strtod(optarg, &ptr);
                G4cout << "Energy E1 set to " << particleEnergyMeV_E1 << G4endl;
                break;
            case 'y':
                particleEnergyMeV_E2 = strtod(optarg, &ptr);
                G4cout << "Energy E2 set to " << particleEnergyMeV_E2 << G4endl;
                break;
            case 'g':
                set_and_use_gaussian = TRUE;
                G4cout << "Gaussian set" << set_and_use_gaussian << G4endl;
                break;
            case 'i':
                gdml_filename = G4String(optarg);
                G4cout << "GDML file set " << gdml_filename << G4endl;
                break;
            case 'l':
                set_and_use_powerlaw = TRUE;
                G4cout << "Power law set " << set_and_use_powerlaw << G4endl;
                break;
            case 'm':
                set_and_use_monoenergy = TRUE;
                G4cout << "Mono beam set " << set_and_use_monoenergy << G4endl;
                break;
            case 'n':
                particlesNumber = strtol(optarg, &ptr, 10);
                G4cout << "Particle number per run set to " << particlesNumber << G4endl;
                visOpen = FALSE;
                break;
            case 'o':
                output_ROOT_FileName = G4String(optarg);
                G4cout << "ROOT output to " << output_ROOT_FileName << G4endl;
                break;
            case 'p':
                strPart = G4String(optarg);
                G4cout << "Particle set " << strPart << G4endl;
                break;
            case 'q':
                physName = "standard_opt4";
                use_ftfp = TRUE;
                G4cout << "Standard opt 4 physics set" << G4endl;
                break;
            case 'r':
                randomID = TRUE;
                G4cout << "Random ROOT file ID will be used" << G4endl;
                break;
            case 's':
                particle_sigma = strtod(optarg, &ptr);
                G4cout << "Gaussian sigma set" << particle_sigma << G4endl;
                break;
            case 'x':
                macro_filename = G4String(optarg);
                G4cout << "Macro to be executed " << macro_filename << G4endl;
                break;
            case 'z':
                massPrinted = TRUE;
                G4cout << "Statistics on logical volumes will be printed to stderr" << G4endl;
                break;
            case 0:  // a flag is set, no other action is taken
                break;
            case ':':   /* missing option argument */
                fprintf(stderr, "%s: option `-%c' requires an argument\n",
                        argv[0], optopt);
                break;
            default:
                G4cerr << argv[0] << ": Unknown option " << opt << G4endl;
                print_usage();
                break;
        }
    }

    G4cout << "[INFO] Will use GDML file name " << gdml_filename << G4endl;
    G4GDMLParser parser;
    parser.Read(gdml_filename);
    if (visOpen) {
        ui = new G4UIExecutive(argc, argv);
    }

#ifdef G4MULTITHREADED
    auto *runManager = new G4MTRunManager;
    if (nThreads > 0) runManager->SetNumberOfThreads(nThreads);
#else
    G4RunManager* runManager = new G4RunManager;
#endif

    runManager->SetUserInitialization(new DetectorConstruction(parser.GetWorldVolume(),
                                                               parser.GetWorldVolume()->GetLogicalVolume()));

    if (use_ftfp) {
        auto physics = new FTFP_BERT();
        runManager->SetUserInitialization(physics);

    } else {
        phys = new AaltoPhysicsList();
        phys->AddPhysicsList(physName);
        runManager->SetUserInitialization(phys);
    }

    output_ROOT_FileName.toLower();
    output_ROOT_FileName = output_ROOT_FileName.substr(0, output_ROOT_FileName.find(".root"));
    if (randomID) {
        output_ROOT_FileName = output_ROOT_FileName + "_0x" + (boost::format("%016x") % seed).str() + ".root";
    }
    G4cout << "[INFO] Will use output file name " << output_ROOT_FileName << G4endl;

    runManager->SetUserInitialization(new ActionInitialization(particleBeamRadius, output_ROOT_FileName));
    runManager->Initialize();

    if (visOpen) {
        visManager = new G4VisExecutive;
        visManager->Initialize();
    }

    G4UImanager *UImanager = G4UImanager::GetUIpointer();

    // set default mono source in case no other parameters override the behavior
    UImanager->ApplyCommand("/gps/particle " + strPart);
    UImanager->ApplyCommand("/gps/ene/mono " + G4UIcommand::ConvertToString(particleEnergyMeV_E1) + " MeV");
    UImanager->ApplyCommand("/control/execute " + macro_filename);

    if (set_and_use_monoenergy) { // override whatever macro sets
        UImanager->ApplyCommand("/gps/ene/mono " + G4UIcommand::ConvertToString(particleEnergyMeV_E1) + " MeV");
    } else if (set_and_use_powerlaw) {
        UImanager->ApplyCommand("/gps/ene/type Pow");
        UImanager->ApplyCommand("/gps/ene/min " + G4UIcommand::ConvertToString(particleEnergyMeV_E1) + " MeV");
        UImanager->ApplyCommand("/gps/ene/max " + G4UIcommand::ConvertToString(particleEnergyMeV_E2) + " MeV");
        UImanager->ApplyCommand("/gps/ene/alpha " + G4UIcommand::ConvertToString(powerlaw_alpha));
    } else if (set_and_use_gaussian) {
        UImanager->ApplyCommand("/gps/ene/type Gauss");
        UImanager->ApplyCommand("/gps/ene/mono " + G4UIcommand::ConvertToString(particleEnergyMeV_E1) + " MeV");
        UImanager->ApplyCommand("/gps/ene/sigma " + G4UIcommand::ConvertToString(particle_sigma) + " MeV");
    }

    if (massPrinted) {
        auto lvs = G4LogicalVolumeStore::GetInstance();
        std::vector<G4LogicalVolume *>::const_iterator lvciter;
        for (lvciter = lvs->begin(); lvciter != lvs->end(); lvciter++) {
            if ((*lvciter)->GetName()) {
                G4cerr << "Logical volume name " << (*lvciter)->GetName() << "\t" <<
                       (*lvciter)->GetMass() / gram << " gram\n" <<
                       (*lvciter)->GetSolid()->GetExtent() <<
                       G4endl;
            }
        }
    }

    if (visOpen) {
        UImanager->ApplyCommand("/control/execute vis.mac");
        UImanager->ApplyCommand("/vis/scene/endOfEventAction accumulate");
        ui->SessionStart();
        delete ui;
        delete visManager;
    } else {
        G4cout << "[INFO] Batch mode run. Nparticles " << particlesNumber << G4endl;
        UImanager->SetVerboseLevel(0);
        UImanager->ApplyCommand(G4String("/run/beamOn ") + G4UIcommand::ConvertToString(particlesNumber));
    }

    auto analysisManager = G4AnalysisManager::Instance();
    if (analysisManager->IsOpenFile()) { analysisManager->CloseFile(); }

    delete runManager;

    return 0;
}
