//
// Created by phil on 5/18/18.
//

#include "DAQSettings.h"
#include "PrimaryGeneratorAction.hh"
#include "SensitiveDetector.hh"

#include <G4OpticalPhoton.hh>
#include <G4RadioactiveDecay.hh>
#include <G4RootAnalysisManager.hh>
#include <G4RunManager.hh>

#include "G4Cerenkov.hh"
#include "G4VProcess.hh"
#include "G4OpticalPhoton.hh"
#include "G4EmProcessSubType.hh"
#include "G4AnalysisManager.hh"

#define DBG 0

SensitiveDetector::SensitiveDetector(const G4String &name)
    : G4MultiFunctionalDetector(name) {
    eDepTotal = 0;
    eSecondary = 0;
    globaltime_g4double = 0;
    generatorAction = nullptr;
    fCherenkov_enabled = 0;
}

SensitiveDetector::SensitiveDetector(const G4String &name, G4int cherenkov_enabled)
    : G4MultiFunctionalDetector(name) {
    eDepTotal = 0;
    eSecondary = 0;
    globaltime_g4double = 0;
    generatorAction = nullptr;
    fCherenkov_enabled = cherenkov_enabled;
}

SensitiveDetector::~SensitiveDetector() {
    ;
}

void SensitiveDetector::Initialize(G4HCofThisEvent *) {
    eDepTotal = 0;
    eSecondary = 0;
    angleIn = -500 * CLHEP::degree;
    generatorAction = dynamic_cast<const PrimaryGeneratorAction *>(G4RunManager::GetRunManager()->
        GetUserPrimaryGeneratorAction());
#if DBG
    G4cout << "Event at \033[31m" << this->GetName() << "\033[0m" << G4endl;
#endif
}

G4bool SensitiveDetector::ProcessHits(G4Step *step, G4TouchableHistory *ROhist) {
    G4Track *track = step->GetTrack();
    static auto const optical_photon = G4OpticalPhoton::OpticalPhotonDefinition();
    const G4double globalTime = track->GetGlobalTime();
    // const auto logical_detector_volume = ROhist->GetVolume()->GetLogicalVolume();
    const G4double edepThisStep = step->GetTotalEnergyDeposit() / CLHEP::MeV;
    const G4ThreeVector &displace = step->GetDeltaPosition();
    const auto tw = DAQSettings::Instance().GetTimeWindow();
    const G4ParticleDefinition *particle = track->GetDefinition();

    // Check if the particle is an optical photon
    if (particle == optical_photon) {
        track->SetTrackStatus(fStopAndKill);
        if (track->GetCreatorProcess()->GetProcessSubType() == fCerenkov) {
            G4double photonEnergy = track->GetKineticEnergy() / CLHEP::eV;
            G4double wavelength_nm = (1240.0 / photonEnergy);
            auto analysisManager = G4RootAnalysisManager::Instance();
            analysisManager->FillH1(0, wavelength_nm, 1);
            track_length_cm += 1;
        }
    }

    if (tw > 0.) {
        const auto lt = step->GetPreStepPoint()->GetLocalTime();
        const auto plt = step->GetPostStepPoint()->GetLocalTime();

#if DBG
        G4cout << "Global time of " << step->GetTrack()->GetParticleDefinition()->GetParticleName() << " " <<
                globalTime << " Ltime " << plt << " PreStep Ltime " << lt << " local delta " << plt - lt <<
                " PTime " << generatorAction->GetParticleGun()->GetParticleTime() <<
                " Track T " << step->GetTrack()->GetGlobalTime() <<
                " Track T " << 1 << G4endl;
#endif

        // if (aStep->GetTrack()->GetCreatorProcess()->GetProcessType() == fDecay) {
        //     if (aStep->GetTrack()->GetCreatorProcess()->GetProcessSubType() == fRadioactiveDecay) {
        //         theGlobalTime0
        //     }
        // }

        if ((plt - lt) > tw) {
            // Step took too long, outside the acquisition window
#if DBG
            G4cout << step->GetTrack()->GetParticleDefinition()->GetParticleName() << " discarded" << G4endl;
#endif
            return false;
        }
    }

    if (edepThisStep == 0.) {
        return false;
    }

    if (eDepTotal == 0.0) {
        angleIn = step->GetPreStepPoint()->GetMomentum().angle(generatorAction->GetPrimaryAxis());
    }

#if DBG > 2
    G4double deltaE;
    deltaE = step->GetDeltaEnergy();
    G4cout << "\033[33m" << this->GetName() << "\t" <<
            step->GetPreStepPoint()->GetTouchable()->GetVolume()->GetName() << "\t" << edepThisStep << "\t" << deltaE
            << "\t" << eDepTotal << "\033[0m" << G4endl;
    G4cout << "Hit " << edepThisStep << " " << step->GetTrack()->GetGlobalTime() << G4endl;
#endif
#if DBG > 3
    G4cout << this->GetName() << "\t"
            << step->GetPreStepPoint()->GetTouchable()->GetVolume()->GetName() << "\t\t"
            << step->GetPostStepPoint()->GetTouchable()->GetVolume()->GetName() << "\t\t"
            << step->GetPreStepPoint()->GetKineticEnergy() / MeV << "\t\t"
            << step->GetPostStepPoint()->GetKineticEnergy() / MeV << "\t\t"
            << "\033[33m" << edepThisStep << "\t" << this->eDepTotal << "\033[0m" << "\t"
            << (step->GetPreStepPoint()->GetKineticEnergy() / MeV - step->GetPostStepPoint()->GetKineticEnergy() /
                MeV) / edepThisStep << "\t"
            << step->GetTrack()->GetParticleDefinition()->GetParticleName() << "\t\t"
            << step->GetPreStepPoint()->GetPosition() << "\t\t"
            << step->GetPreStepPoint()->GetSensitiveDetector()->GetName() << G4endl;
#endif
    globaltime_g4double = step->GetTrack()->GetGlobalTime() / CLHEP::ns;
    localtime_g4double = step->GetPreStepPoint()->GetLocalTime() / CLHEP::ns;

    if (step->GetTrack()->GetTrackID() != 1) { eSecondary += edepThisStep; } else {
        if (step->GetPreStepPoint()->GetPhysicalVolume() !=
            step->GetPostStepPoint()->GetPhysicalVolume()) {
            eOut += step->GetPostStepPoint()->GetKineticEnergy() / CLHEP::MeV;
        }
    }

    // track_length_cm += displace.mag()/CLHEP::cm;
    eDepTotal += edepThisStep;
    return true;
}

void SensitiveDetector::EndOfEvent(G4HCofThisEvent *) {
#if DBG
    {
        G4cout << this->GetName() << "::EndOfEvent eDep \033[32m"
                << this->eDepTotal << " "
                << " \033[0m" << G4endl;
    };
#endif
}
