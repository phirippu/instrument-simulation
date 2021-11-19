//
// Created by phil on 5/17/18.
//

#include "ThreadRun.hh"
#include "g4root.hh"
#include <G4Event.hh>
#include <G4RunManager.hh>
#include <G4SDManager.hh>
#include <RunAction.hh>
#include "G4RunManagerFactory.hh"
#include <valarray>


ThreadRun::ThreadRun(const G4String &rootFileName) : G4Run() {

    analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetFileName(rootFileName);
    if ( G4Threading::IsMultithreadedApplication() ) analysisManager->SetNtupleMerging(true);

    dConstruction = dynamic_cast<const DetectorConstruction *>((G4RunManager::GetRunManager())->GetUserDetectorConstruction());
    sdCollection.clear();
    for (const auto &record: dConstruction->listDetectorNames) {
        const auto &detName = std::get<1>(record);
        auto detObject = G4SDManager::GetSDMpointer()->FindSensitiveDetector(detName, true);
        sdCollection.push_back(dynamic_cast<SensitiveDetector *>(detObject));
    }

    iNtupleIdx = analysisManager->CreateNtuple("Detector Data", "Deposited energies");

    for (auto &detector_record: dConstruction->listDetectorNames) {
        analysisManager->CreateNtupleDColumn(iNtupleIdx, std::get<2>(detector_record) + "_Edep_MeV");
    };
    for (auto &detector_record: dConstruction->listDetectorNames) {
        analysisManager->CreateNtupleDColumn(iNtupleIdx, std::get<2>(detector_record) + "_Esec_MeV");
    };
    analysisManager->CreateNtupleDColumn(iNtupleIdx, "Gun_energy_MeV");
    analysisManager->CreateNtupleDColumn(iNtupleIdx, "Gun_angle_deg");
    analysisManager->CreateNtupleDColumn(iNtupleIdx, "Gun_theta_deg");
    analysisManager->CreateNtupleDColumn(iNtupleIdx, "Gun_phi_deg");
    analysisManager->FinishNtuple(iNtupleIdx);
    if (!analysisManager->IsOpenFile()) {
        analysisManager->OpenFile();
    }
}

void ThreadRun::RecordEvent(const G4Event *aEvent) {
    G4double angleGun = 0, energyGun = 0;
    G4double thetaGun = 0, phiGun = 0;
    G4int column_index = 0;
    const unsigned long data_length = sdCollection.size();
    std::valarray<G4double> deposited_energies;
    std::valarray<G4double> deposited_energies_from_secondaries;

    if (data_length > 0) {
        const auto generatorAction = dynamic_cast<const PrimaryGeneratorAction *>(G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
        if (generatorAction != nullptr) {
            auto particle_direction = generatorAction->GetParticleGun()->GetParticleMomentumDirection();
            angleGun = particle_direction.angle(generatorAction->GetPrimaryAxis()) / deg;
            angleGun = fabs(angleGun);
            thetaGun = particle_direction.getTheta() / CLHEP::degree;
            phiGun = particle_direction.getPhi() / CLHEP::degree;
            energyGun = generatorAction->GetParticleGun()->GetParticleEnergy();
        }
        deposited_energies.resize(data_length);
        // Check the deposited energies. Any non-zero deposit to detectors is recorded in a full detector record.
        for (auto &detector_record: sdCollection) {
            deposited_energies[column_index++] = detector_record->GetETotal() / MeV;
            /* #include "G4SystemOfUnits.hh" ; static constexpr double megaelectronvolt = 1. ;*/
        }
        column_index = 0;
        if (deposited_energies.sum() > 0) {
            for (auto &x: deposited_energies) { analysisManager->FillNtupleDColumn(iNtupleIdx, column_index++, x); }
            for (auto &detector_record: sdCollection) {
                analysisManager->FillNtupleDColumn(iNtupleIdx, column_index++, detector_record->GetESecondary() / MeV);
            }
            analysisManager->FillNtupleDColumn(iNtupleIdx, column_index++, energyGun);
            analysisManager->FillNtupleDColumn(iNtupleIdx, column_index++, angleGun);
            analysisManager->FillNtupleDColumn(iNtupleIdx, column_index++, thetaGun);
            analysisManager->FillNtupleDColumn(iNtupleIdx, column_index, phiGun);
            analysisManager->AddNtupleRow(iNtupleIdx);
        }
        for (auto &detector_record: sdCollection) {
            (detector_record)->ClearETotal();
        }
    }
    G4Run::RecordEvent(aEvent);

}

void ThreadRun::Merge(const G4Run *aTSRun) {

    G4cout << "[ThreadRun] Merge" << G4endl;
    G4Run::Merge(aTSRun);
}

ThreadRun::~ThreadRun() {
    G4cerr << "[ThreadRun] Run is finished" << G4endl;
}
