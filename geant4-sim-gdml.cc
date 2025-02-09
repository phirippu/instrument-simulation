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

#include "G4RunManagerFactory.hh"
#include "DetectorConstruction.hh"
#include "G4GDMLParser.hh"
#include "G4RunManager.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "geant4-sim-gdml.h"
#include <ActionInitialization.hh>
#include <cstdlib>
#include <fcntl.h>
#include <G4DecayPhysics.hh>
#include <G4IonElasticPhysics.hh>
#include <G4IonPhysics.hh>
#include <unistd.h>
#include <vector>
#include <getopt.h>
#include <boost/format.hpp>
#include <G4PhysListFactory.hh>
#include <G4RadioactiveDecayPhysics.hh>
#include <PhysicsList.hh>
#include <PhysicsListMessenger.hh>
#include "G4HadronicParameters.hh"


G4bool bTupleWritten;

void print_usage()
{
    G4cout << G4endl;
    G4cout << "Usage: load_gdml " << G4endl
        << "-c [--threads] <number of threads>" << G4endl
        << "-e [--energylow | --energycenter] <low energy margin or central energy (use -m/--mono), MeV>" << G4endl
        << "-y [--energyhigh] <high energy margin, MeV>" << G4endl
        << "-g [--gaussian] use Gaussian distribution" << G4endl
        << "-i [--inputfile] <input gdml file : mandatory>" << G4endl
        << "-l [--powerlaw] use powerlaw distribution" << G4endl
        << "-m [--mono] use monoenergetic beam" << G4endl
        << "-n [--partnum] <number of particles to be simulated>" << G4endl
        << "-o [--output] <output file name [default:output.root]>" << G4endl
        << "-p [--particle] <particle name [default:geantino]>" << G4endl
        << "-q [--quick] enable FTFP physics" << G4endl
        << "-r [--randomid] enable random seed printed as the output suffix" << G4endl
        << "-s [--sigma] <gaussian sigma, MeV>" << G4endl
        << "-x [--execute] <macro file name>" << G4endl
        << "-z [--printstat] print statistics on logical volumes" << G4endl;

    G4cout << G4endl;
}

int main(int argc, char** argv)
{
    G4PhysListFactory g4PhysListFactory;
    G4VModularPhysicsList* pPhysicsList = nullptr;
    PhysicsListMessenger* pPhysicsListMessenger;
    G4String physicsListName = ".";
    G4Random::setTheEngine(new CLHEP::RanecuEngine);
    G4String strPart = G4String("geantino");
    G4bool set_and_use_gaussian = FALSE;
    G4bool set_and_use_monoenergy = FALSE;
    G4bool set_and_use_powerlaw = FALSE;
    G4bool visOpen = TRUE;
    G4bool massPrinted = FALSE;
    G4bool randomID = FALSE;
    G4bool arbitrary_energy_spectrum = FALSE;
    G4UIExecutive* ui = nullptr;
    G4VisManager* visManager = nullptr;
    //    G4double particleBeamRadius = 10 * cm;
    G4double stepSizeMicrons = 0.0;
    G4double particleEnergyMeV_E1 = 0.001;
    G4double particleEnergyMeV_E2 = 0.001;
    G4double particle_sigma = 0.001;
    G4double powerlaw_alpha = 0;
    G4double decay_time_g4double = 1.0 * CLHEP::year;
    G4int nThreads = G4Threading::G4GetNumberOfCores();
    G4long particlesNumber = 1;
    G4GDMLParser g4GdmlParser;
    char* ptr;

    G4Random::setTheEngine(new CLHEP::RanecuEngine);
    long seed;

    int randomData = open("/dev/urandom", 0);
    if (randomData < 0)
    {
        G4cerr << "something went wrong with random" << G4endl;
        exit(1);
    }
    else
    {
        ssize_t result = read(randomData, &seed, sizeof seed);
        if (result < 0)
        {
            G4cerr << "something went really wrong with random" << G4endl;
            exit(1);
        }
    }

    CLHEP::HepRandom::setTheSeed(seed);
    G4Random::setTheSeed(seed);

    G4String gdml_filename = G4String(".");
    G4String macro_filename = G4String(".");
    G4String output_ROOT_FileName = "output";
    G4String iaea_phase_space_filename = "";

    static struct option long_options[] =
    {
        {"partnum", optional_argument, nullptr, PARTICLE_NUMBER_OPT},
        {"particle", optional_argument, nullptr, PARTICLE_NAME_OPT},
        {"output", optional_argument, nullptr, OUTPUT_NAME_OPT},
        {"threads", optional_argument, nullptr, THREAD_NUM_OPT},
        {"stepsize", optional_argument, nullptr, STEP_SIZE_OPT},
        {"energylow", optional_argument, nullptr, ENERGY_LOW_LIMIT_OPT},
        {"energycenter", optional_argument, nullptr, ENERGY_CENTER_OPT},
        {"energyhigh", optional_argument, nullptr, ENERGY_HIGH_LIMIT_OPT},
        {"gaussian", no_argument, nullptr, GAUSSIAN_ENERGY_OPT},
        {"powerlaw", no_argument, nullptr, POWERLAW_SPECTRUM_OPT},
        {"alpha", optional_argument, nullptr, POWERLAW_ALPHA_OPT},
        {"mono", no_argument, nullptr, MONOENERGETIC_OPT},
        {"arbitrary", no_argument, nullptr, ARBITRARY_SPECTRUM_OPT},
        {"sigma", optional_argument, nullptr, GAUSSIAN_SIGMA_OPT},
        {"useftfp", no_argument, nullptr, FTFP_PHYSICS_OPT},
        {"useqgsp", no_argument, nullptr, QGSP_PHYSICS_OPT},
        {"useqgsphp", no_argument, nullptr, QGSP_PHYSICS_HIGH_PRECISION_OPT},
        {"useftfphp", no_argument, nullptr, FTFP_PHYSICS_HIGH_PRECISION_OPT},
        {"randomid", no_argument, nullptr, RANDOM_FILE_ID_OPT},
        {"inputfile", required_argument, nullptr, INPUT_FILE_NAME_OPT},
        {"execute", optional_argument, nullptr, MACRO_FILE_NAME_OPT},
        {"printstat", no_argument, nullptr, PRINT_STATISTICS_ON_DETECTORS_OPT},
        {"loadphsp", required_argument, nullptr, LOAD_IAEA_PHSP_FILE},
        {"finaldecay", no_argument, nullptr, ULTRA_LONG_ALLOWED_DECAY_TIME},
        {nullptr, 0, nullptr, 0}
    };
    const char* const short_options = ":a:c:e:y:glbmi:n:o:p:qrs:x:z";
    int opt;
    while ((opt = getopt_long(argc, argv, short_options, long_options, nullptr)) != -1)
    {
        switch (opt)
        {
        case ULTRA_LONG_ALLOWED_DECAY_TIME:
            decay_time_g4double = 1.0e+60 * CLHEP::year;
            break;
        case LOAD_IAEA_PHSP_FILE:
            iaea_phase_space_filename = G4String(optarg);
            break;
        case STEP_SIZE_OPT:
            stepSizeMicrons = strtod(optarg, &ptr);
            G4cout << "Step size in microns " << stepSizeMicrons << G4endl;
            break;
        case 'a':
        case POWERLAW_ALPHA_OPT:
            powerlaw_alpha = strtod(optarg, &ptr);
            G4cout << "Alpha set to " << powerlaw_alpha << G4endl;
            break;
        case 'c':
        case THREAD_NUM_OPT:
            nThreads = (int)strtol(optarg, &ptr, 10);
            G4cout << "N threads set to " << nThreads << G4endl;
            break;
        case 'e':
        case ENERGY_LOW_LIMIT_OPT:
            particleEnergyMeV_E1 = strtod(optarg, &ptr);
            G4cout << "Energy E1 set to " << particleEnergyMeV_E1 << G4endl;
            break;
        case 'y':
        case ENERGY_HIGH_LIMIT_OPT:
            particleEnergyMeV_E2 = strtod(optarg, &ptr);
            G4cout << "Energy E2 set to " << particleEnergyMeV_E2 << G4endl;
            break;
        case 'g':
        case GAUSSIAN_ENERGY_OPT:
            set_and_use_gaussian = TRUE;
            G4cout << "Gaussian set" << set_and_use_gaussian << G4endl;
            break;
        case 'i':
        case INPUT_FILE_NAME_OPT:
            gdml_filename = G4String(optarg);
            G4cout << "GDML file set " << gdml_filename << G4endl;
            break;
        case 'l':
        case POWERLAW_SPECTRUM_OPT:
            set_and_use_powerlaw = TRUE;
            G4cout << "Power law set " << set_and_use_powerlaw << G4endl;
            break;
        case 'b':
        case ARBITRARY_SPECTRUM_OPT:
            arbitrary_energy_spectrum = TRUE;
            G4cout << "Arbitrary spectrum set " << arbitrary_energy_spectrum << G4endl;
            break;
        case 'm':
        case MONOENERGETIC_OPT:
            set_and_use_monoenergy = TRUE;
            G4cout << "Mono beam set " << set_and_use_monoenergy << G4endl;
            break;
        case 'n':
        case PARTICLE_NUMBER_OPT:
            particlesNumber = strtol(optarg, &ptr, 10);
            G4cout << "Particle number per run set to " << particlesNumber << G4endl;
            visOpen = FALSE;
            break;
        case 'o':
        case OUTPUT_NAME_OPT:
            output_ROOT_FileName = G4String(optarg);
            G4cout << "ROOT output to " << output_ROOT_FileName << G4endl;
            break;
        case 'p':
        case PARTICLE_NAME_OPT:
            strPart = G4String(optarg);
            G4cout << "Particle set " << strPart << G4endl;
            break;
        case 'q':
        case FTFP_PHYSICS_OPT:
            physicsListName = "FTFP_BERT";
            G4cout << "FTFP_BERT physics is selected." << G4endl;
            break;
        case QGSP_PHYSICS_OPT:
            physicsListName = "QGSP_BERT";
            G4cout << "QGSP_BERT physics is selected." << G4endl;
            break;
        case QGSP_PHYSICS_HIGH_PRECISION_OPT:
            physicsListName = "QGSP_BERT";
            G4cout << "QGSP_BERT high precision (em3) physics is selected." << G4endl;
            break;
        case FTFP_PHYSICS_HIGH_PRECISION_OPT:
            physicsListName = "FTFP_BERT_EMZ";
            G4cout << "FTFP_BERT_EMZ high precision (em4) physics is selected." << G4endl;
            break;
        case 'r':
        case RANDOM_FILE_ID_OPT:
            randomID = TRUE;
            G4cout << "Random ROOT file ID will be used" << G4endl;
            break;
        case 's':
        case GAUSSIAN_SIGMA_OPT:
            particle_sigma = strtod(optarg, &ptr);
            G4cout << "Gaussian sigma set" << particle_sigma << G4endl;
            break;
        case 'x':
        case MACRO_FILE_NAME_OPT:
            macro_filename = G4String(optarg);
            G4cout << "Macro to be executed " << macro_filename << G4endl;
            break;
        case 'z':
        case PRINT_STATISTICS_ON_DETECTORS_OPT:
            massPrinted = TRUE;
            G4cout << "Statistics on logical volumes will be printed to stderr" << G4endl;
            break;
        case 0: // a flag is set, no other action is taken
            break;
        case ':': /* missing option argument */
            fprintf(stderr, "%s: option `-%c' requires an argument\n",
                    argv[0], optopt);
            break;
        default:
            G4cerr << argv[0] << ": Unknown option " << opt << G4endl;
            print_usage();
            break;
        }
    }

    if (gdml_filename == ".")
    {
        G4cout << "[ERROR] GDML file name is invalid or not set up properly." << G4endl;
        exit(2);
    }
    if (macro_filename == ".")
    {
        G4cout << "[ERROR] Macro file name is invalid or not set up properly." << G4endl;
        exit(2);
    }

    G4cout << "[INFO] Will use GDML file " << gdml_filename << G4endl;

    //    output_ROOT_FileName.toLower();
    output_ROOT_FileName = G4StrUtil::to_lower_copy(output_ROOT_FileName);
    output_ROOT_FileName = output_ROOT_FileName.substr(0, output_ROOT_FileName.find(".root"));
    if (randomID)
    {
        output_ROOT_FileName = output_ROOT_FileName + "_0x" + (boost::format("%016x") % seed).str() + ".root";
    }
    G4cout << "[INFO] Will use output file " << output_ROOT_FileName << G4endl;

    g4GdmlParser.Read(gdml_filename);
    if (visOpen)
    {
        ui = new G4UIExecutive(argc, argv);
    }

#ifdef G4MULTITHREADED
    auto runManager = G4RunManagerFactory::CreateRunManager();

    if (nThreads > 0) runManager->SetNumberOfThreads(nThreads);
#else
    G4RunManager* runManager = new G4RunManager;
#endif

    runManager->SetUserInitialization(new DetectorConstruction(g4GdmlParser.GetWorldVolume(),
                                                               g4GdmlParser.GetWorldVolume()->GetLogicalVolume(),
                                                               stepSizeMicrons));

    // If the physics is defined by the command line, use the appropriate name.
    if ("." != physicsListName && g4PhysListFactory.IsReferencePhysList(physicsListName))
    {
        pPhysicsList = g4PhysListFactory.GetReferencePhysList(physicsListName);
        pPhysicsList->RegisterPhysics(new G4IonElasticPhysics());
        pPhysicsList->RegisterPhysics(new G4IonPhysics);
        pPhysicsList->RegisterPhysics(new G4DecayPhysics);
        pPhysicsList->RegisterPhysics(new G4RadioactiveDecayPhysics);
        G4HadronicParameters::Instance()->SetTimeThresholdForRadioactiveDecay(decay_time_g4double);
        pPhysicsListMessenger = new PhysicsListMessenger();
        if (!pPhysicsList)
        {
            G4cout << "[ERROR] Physics list could not be created." << G4endl;
            exit(3);
        }
        G4cout << "[NOTE] Physics list was created." << G4endl;
        pPhysicsListMessenger->SayHello();
    }
    else
    // If there was no command-line definition of physics, use default EM physics without hadron processes.
    {
        pPhysicsList = new PhysicsList();
        pPhysicsList->RegisterPhysics(new G4RadioactiveDecayPhysics);
        G4HadronicParameters::Instance()->SetTimeThresholdForRadioactiveDecay(decay_time_g4double);
    }


    runManager->SetUserInitialization(pPhysicsList);
    runManager->SetUserInitialization(
        new ActionInitialization(output_ROOT_FileName, iaea_phase_space_filename, nThreads));
    runManager->Initialize();

    if (visOpen)
    {
        visManager = new G4VisExecutive;
        visManager->Initialize();
    }

    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    UImanager->ApplyCommand("/control/execute " + macro_filename);

    if (arbitrary_energy_spectrum == FALSE)
    {
        // set default mono source in case no other parameters override the behavior
        UImanager->ApplyCommand("/gps/particle " + strPart);
        UImanager->ApplyCommand("/gps/ene/mono " + G4UIcommand::ConvertToString(particleEnergyMeV_E1) + " MeV");
    }
    if (set_and_use_monoenergy)
    {
        // override whatever macro sets
        UImanager->ApplyCommand("/gps/ene/mono " + G4UIcommand::ConvertToString(particleEnergyMeV_E1) + " MeV");
    }
    else if (set_and_use_powerlaw)
    {
        UImanager->ApplyCommand("/gps/ene/type Pow");
        UImanager->ApplyCommand("/gps/ene/min " + G4UIcommand::ConvertToString(particleEnergyMeV_E1) + " MeV");
        UImanager->ApplyCommand("/gps/ene/max " + G4UIcommand::ConvertToString(particleEnergyMeV_E2) + " MeV");
        UImanager->ApplyCommand("/gps/ene/alpha " + G4UIcommand::ConvertToString(powerlaw_alpha));
    }
    else if (set_and_use_gaussian)
    {
        UImanager->ApplyCommand("/gps/ene/type Gauss");
        UImanager->ApplyCommand("/gps/ene/mono " + G4UIcommand::ConvertToString(particleEnergyMeV_E1) + " MeV");
        UImanager->ApplyCommand("/gps/ene/sigma " + G4UIcommand::ConvertToString(particle_sigma) + " MeV");
    }

    if (massPrinted)
    {
        auto lvs = G4LogicalVolumeStore::GetInstance();
        std::vector<G4LogicalVolume*>::const_iterator lvciter;
        for (lvciter = lvs->begin(); lvciter != lvs->end(); lvciter++)
        {
            if ((*lvciter)->GetName())
            {
                G4cerr << "Logical volume name " << (*lvciter)->GetName() << "\t" <<
                    (*lvciter)->GetMass() / gram << " gram\n" <<
                    (*lvciter)->GetSolid()->GetExtent() <<
                    G4endl;
            }
        }
    }

    if (visOpen)
    {
        UImanager->ApplyCommand("/control/execute ../macro/vis.mac");
        UImanager->ApplyCommand("/vis/scene/endOfEventAction accumulate");
        if (ui)
        {
            ui->SessionStart();
            delete ui;
        }
        delete visManager;
    }
    else
    {
        G4cout << "[INFO] Batch mode run. Nparticles " << particlesNumber << G4endl;
        UImanager->SetVerboseLevel(0);
        if (particlesNumber <= RUN_CUT_THRESHOLD)
        {
            runManager->BeamOn((int)particlesNumber);
        }
        else
        {
            runManager->BeamOn((int)RUN_CUT_THRESHOLD);
        }
    }
    delete runManager;
    return 0;
}
