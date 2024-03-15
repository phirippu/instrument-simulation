
#include <Randomize.hh>
#include <utility>
#include <G4RunManager.hh>
#include "PrimaryGeneratorAction.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "AnalysisManager.hh"
#include "G4IAEAphspReader.hh"


namespace { G4Mutex myPrimGenMutex = G4MUTEX_INITIALIZER; }
G4IAEAphspReader *PrimaryGeneratorAction::theIAEAReader = nullptr;

PrimaryGeneratorAction::PrimaryGeneratorAction(G4String iaea_File, G4int nThreads)
        : G4VUserPrimaryGeneratorAction(),
          fRadius(10 * cm), fnThreads(nThreads),
          fIAEA_phase_file(std::move(iaea_File)) {
    gun = InitializeGPS();
    fDetectorAxisVector = G4ThreeVector(-1, 0, 0); // default axis
    fPrimaryBeamType = simplebeam;
    fMessengerDir = nullptr;
    fAngle = 0;
    fBeamTypeInt = 0;
    InitTheGun();
    frunManager = G4RunManager::GetRunManager();
    G4cerr << frunManager << G4endl;
}

/*
PrimaryGeneratorAction::PrimaryGeneratorAction(G4double Radius, CLHEP::Hep3Vector axis)
        : G4VUserPrimaryGeneratorAction(), fRadius(Radius),
          fDetectorAxisVector(std::move(axis)) {
    gun = InitializeGPS();
    fPrimaryBeamType = simplebeam;
    fMessengerDir = nullptr;
    fAngle = 0;
    fBeamTypeInt = 0;
    InitTheGun();

}
*/

void PrimaryGeneratorAction::InitTheGun() {
    fMessengerDir = new G4GenericMessenger(this, "/gun/tune/", "Primary generator tuning");
    fGunPosition = G4ThreeVector(29.4 * mm, 0.151 * mm, 0 * mm);
    fGunDirection = G4ThreeVector(0, 0, 1);
    DefineCommands();
    // if the beam is defined in .mac file, try to init IAEA generator if a command was given in a macro file
    if ((!fIAEA_phase_file.empty()) && (theIAEAReader == nullptr)) {
        G4AutoLock lock(&myPrimGenMutex);
        theIAEAReader = new G4IAEAphspReader(fIAEA_phase_file);
        if (theIAEAReader->GetTotalParticles() > 0) {
            G4cout << "Loaded IAEA phase space file " << fIAEA_phase_file << G4endl;
        } else {
            G4cerr << "Can not load IAEA phase space file " << fIAEA_phase_file << G4endl;
        }
        G4ThreeVector psfShift(0., 0., -100. * cm); // the standard shift for radiation therapy
        theIAEAReader->SetGlobalPhspTranslation(psfShift); // a rotation is possible, see the IAEA Reader doc.
        theIAEAReader->SetTotalParallelRuns(fnThreads);
        G4cout << fnThreads << " threads are set up for IAEA phase space." << G4endl;
        theIAEAReader->SetParallelRun(G4Threading::G4GetThreadId());
        G4cout << G4Threading::G4GetThreadId() << " is this thread ID." << G4endl;
    }
}

void PrimaryGeneratorAction::DefineCommands() {
    auto &beamCmd = fMessengerDir->DeclareProperty("beam", fBeamTypeInt);
    G4String guidance = "Integer type of the beam/source.\n";
    beamCmd.SetGuidance(guidance);
    beamCmd.SetParameterName("Beam type", false);
    beamCmd.SetDefaultValue("0");

    auto &axisCmd = fMessengerDir->DeclareProperty("axis", fDetectorAxisVector);
    axisCmd.SetGuidance("The primary axis for angle calculations.\n");
    axisCmd.SetParameterName("Axis", false);
    axisCmd.SetDefaultValue("0 0 1");

    auto &iaeaCmd = fMessengerDir->DeclareProperty("file", fIAEA_phase_file);
    iaeaCmd.SetGuidance("The IAEA phase space file.\n");
    iaeaCmd.SetParameterName("IAEA file", false);
    iaeaCmd.SetDefaultValue("");
}


PrimaryGeneratorAction::~PrimaryGeneratorAction() {
    G4AutoLock lock(&myPrimGenMutex);
    if (theIAEAReader) {
        delete theIAEAReader;
        theIAEAReader = nullptr;
    }
    delete gun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent) {
//    const G4ThreeVector centreSiDet(29.4017 * mm, 0.15 * mm, 786.881 * mm);
    const G4ThreeVector Aalto1_centreSphereOfRadiation(0. * mm, 0. * mm, 950 * mm);
    const G4ThreeVector Radmon_centreSphereOfRadiation(29.4 * mm, 0. * mm, 786 * mm);
    G4ThreeVector centreSphereOfRadiation(0. * mm, 0. * mm, 0. * mm);
    G4double u = G4UniformRand();
    G4double v = G4UniformRand();
    G4double the, phi, r;
    G4ThreeVector sphericalUniGunDirection;
    G4ThreeVector x(1.0, 0.0, 0.0);
    G4ThreeVector y(1.0, 0.0, 0.0);
    G4ThreeVector sphericalUniGunPosition(0.0, 0.0, 0.0);
    fPrimaryBeamType = static_cast<beamType>(fBeamTypeInt);

    switch (fPrimaryBeamType) {
        case macrodefined:;
            break;
        case simplebeam:
            gun->GetCurrentSource()->GetPosDist()->SetCentreCoords(fGunPosition);
            gun->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(fGunDirection);
            break;

        case simplepowerlaw:
            gun->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Pow");
            gun->GetCurrentSource()->GetEneDist()->SetEmin(1 * MeV);
            gun->GetCurrentSource()->GetEneDist()->SetEmax(10 * MeV);
            gun->GetCurrentSource()->GetEneDist()->SetAlpha(-3);
            gun->GetCurrentSource()->GetPosDist()->SetCentreCoords(fGunPosition);
            gun->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(fGunDirection);
            break;

        case custompowerlawbeam:
            gun->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Pow");
            gun->GetCurrentSource()->GetPosDist()->SetCentreCoords(fGunPosition);
            gun->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(fGunDirection);
            break;

        case spherepowerlaw:
            gun->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Pow");
        case sphere:
            if (fRadius == 30 * cm) {
                centreSphereOfRadiation = Aalto1_centreSphereOfRadiation;
            } else {
                centreSphereOfRadiation = Radmon_centreSphereOfRadiation;
            }
            phi = 360 * degree * u;
            the = acos(1 - 2 * v) * radian;

            sphericalUniGunPosition.setRThetaPhi(fRadius, the, phi);
            x = -sphericalUniGunPosition / sphericalUniGunPosition.r();
            y.setRThetaPhi(x.r(), x.theta() + 90 * deg, x.phi());

            u = G4UniformRand();
            v = G4UniformRand();
            phi = 360 * degree * u;
            the = acos(sqrt(v)) * radian; // Lambertian

            sphericalUniGunDirection = x * cos(the) + y * sin(the);
            sphericalUniGunDirection.rotate(x, phi);
            sphericalUniGunPosition = sphericalUniGunPosition + centreSphereOfRadiation;
            gun->GetCurrentSource()->GetPosDist()->SetCentreCoords(sphericalUniGunPosition);
            gun->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(sphericalUniGunDirection);
            break;

        case nativesphere:

            gun->GetCurrentSource()->GetPosDist()->SetPosDisType("Surface");
            gun->GetCurrentSource()->GetPosDist()->SetPosDisShape("Sphere");
            gun->GetCurrentSource()->GetPosDist()->SetRadius(fRadius);
            gun->GetCurrentSource()->GetAngDist()->SetAngDistType("cos");
            gun->GetCurrentSource()->GetPosDist()->SetCentreCoords(centreSphereOfRadiation);
            gun->GetCurrentSource()->GetPosDist()->SetCentreCoords(Radmon_centreSphereOfRadiation);
            break;

        case gaussianbeam:
            gun->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Gauss");
            if (gun->GetCurrentSource()->GetEneDist()->GetEzero() == 0.0) {
                gun->GetCurrentSource()->GetEneDist()->SetEzero(1 * MeV);
            };
        case thickbeam:

            const G4double BeamDist = 35 * cm;
            const G4double BeamRadius = fRadius;
            const G4double BeamTheta =
                    90 * degree; // Theta is measured as angle to Z-axis, [0, pi]. Zero theta gives a polar vector.
            const G4double BeamPhi = 90 * degree + fAngle;
            G4ThreeVector local_x(1.0, 0.0, 0.0);
            G4ThreeVector local_y(1.0, 0.0, 0.0);
            G4ThreeVector local_z(1.0, 0.0, 0.0);
            G4ThreeVector BeamGunPosition(0.0, 0.0, 0.0);

            phi = 360 * degree * u;
            r = BeamRadius * sqrt(v);

            BeamGunPosition.setRThetaPhi(BeamDist, BeamTheta, BeamPhi);
            local_z = -BeamGunPosition / BeamGunPosition.r();
            local_y.setRThetaPhi(1, local_z.theta() + 90 * degree, local_z.phi());
            local_x = local_y.cross(local_z);

            BeamGunPosition = BeamGunPosition + local_x * r * cos(phi) + local_y * r * sin(phi);
            BeamGunPosition = BeamGunPosition + fCentreOfDetector;

            gun->GetCurrentSource()->GetPosDist()->SetCentreCoords(BeamGunPosition);
            gun->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(local_z);
            break;
    }
//    G4cerr << gun->GetParticleEnergy()<< "MeV"<< G4endl ;
//    gun->GetParticleDefinition()->DumpTable();
//    G4cerr << << "MeV"<< G4endl ;
    if (theIAEAReader) {
        G4AutoLock lock(&myPrimGenMutex);
//        theIAEAReader->SetParallelRun(G4Threading::G4GetThreadId());
        theIAEAReader->GeneratePrimaryVertex(anEvent);
    } else {
        gun->GeneratePrimaryVertex(anEvent);
    }
}


G4GeneralParticleSource *PrimaryGeneratorAction::InitializeGPS() {
    auto *gps = new G4GeneralParticleSource();
//    gps->GetCurrentSource()->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle("proton"));
    gps->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Mono");
    gps->GetCurrentSource()->GetPosDist()->SetPosDisType("Point");
    gps->GetCurrentSource()->GetPosDist()->SetCentreCoords(fGunPosition);
    gps->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(fGunDirection);
    return gps;
}

