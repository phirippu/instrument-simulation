//
// Created by phil on 5/18/18.
//

#include <G4RunManager.hh>
#include "SensitiveDetector.hh"
#include "PrimaryGeneratorAction.hh"

#define DBG 0

SensitiveDetector::SensitiveDetector(const G4String &name)
    : G4MultiFunctionalDetector(name) {
    eDepTotal = 0;
    eSecondary = 0;
    time_g4double = 0;
    generatorAction = nullptr;
}

SensitiveDetector::~SensitiveDetector() {
    ;
}

void SensitiveDetector::Initialize(G4HCofThisEvent *) {
    eDepTotal = 0;
    eSecondary = 0;
    angleIn = -500 * degree;
    generatorAction = dynamic_cast<const PrimaryGeneratorAction *>(G4RunManager::GetRunManager()->
        GetUserPrimaryGeneratorAction());
#if DBG
    G4cout << "Event at \033[31m" << this->GetName() << "\033[0m" << G4endl;
#endif
}

G4bool SensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *) {
    G4double edepThisStep = aStep->GetTotalEnergyDeposit() * MeV;
    if (edepThisStep == 0.) {
        return false;
    }

    if (eDepTotal == 0.0) {
        angleIn = aStep->GetPreStepPoint()->GetMomentum().angle(generatorAction->GetPrimaryAxis());
    }
    eDepTotal += edepThisStep;
    if (aStep->GetPreStepPoint()->GetPhysicalVolume() !=
        aStep->GetPostStepPoint()->GetPhysicalVolume()) {
        eOut += aStep->GetPostStepPoint()->GetKineticEnergy() / MeV;
    }

#if DBG > 1
        G4double deltaE;
        deltaE = aStep->GetDeltaEnergy();
        G4cout << "\033[33m" << this->GetName() << "\t" <<
        aStep->GetPreStepPoint()->GetTouchable()->GetVolume()->GetName()<<  "\t" << edepThisStep <<  "\t" << deltaE <<  "\t" << eDepTotal << "\033[0m" << G4endl;
        G4cout << "Hit " << edepThisStep << " " << aStep->GetTrack()->GetGlobalTime() << G4endl;
#endif
#if DBG
    G4cout << this->GetName() << "\t"
           << aStep->GetPreStepPoint()->GetTouchable()->GetVolume()->GetName() << "\t\t"
           << aStep->GetPostStepPoint()->GetTouchable()->GetVolume()->GetName() << "\t\t"
           << aStep->GetPreStepPoint()->GetKineticEnergy() / MeV << "\t"
           << aStep->GetPostStepPoint()->GetKineticEnergy() / MeV << "\t"
           << edepThisStep << "\t" << this->eDepTotal << "\t"
           << (aStep->GetPreStepPoint()->GetKineticEnergy() / MeV - aStep->GetPostStepPoint()->GetKineticEnergy() / MeV) / edepThisStep << "\t"
           << aStep->GetTrack()->GetParticleDefinition()->GetParticleName() << "\t\t"
           << aStep->GetPreStepPoint()->GetPosition() << "\t\t"
           << aStep->GetPreStepPoint()->GetSensitiveDetector()->GetName() << G4endl;
#endif
    time_g4double = aStep->GetTrack()->GetGlobalTime() / CLHEP::ns;
    if (aStep->GetTrack()->GetParticleDefinition()->GetParticleName() !=
        generatorAction->GetParticleGun()->GetParticleDefinition()->GetParticleName()) { eSecondary += edepThisStep; }
    return true;
}

void SensitiveDetector::EndOfEvent(G4HCofThisEvent *) {
#if DBG
    {
        G4cout << this->GetName() << "::EndOfEvent \033[32m"
               << this->eDepTotal << " "
               << " \033[0m" << G4endl;
    };
#endif
}
