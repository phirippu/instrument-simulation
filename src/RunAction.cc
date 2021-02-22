//
// Created by phil on 5/17/18.
//

#include <ThreadRun.hh>
#include <utility>
#include "RunAction.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "AnalysisManager.hh"


RunAction::RunAction(G4String rootFileName)
        : G4UserRunAction(), localRun(nullptr), nofEvents(0), fRootFileName(std::move(rootFileName)) {
}

RunAction::~RunAction() = default;

G4Run *RunAction::GenerateRun() {
    localRun = new ThreadRun(fRootFileName);
    return (G4Run *) localRun;
}


void RunAction::BeginOfRunAction(const G4Run *run) {
    G4cout << "[RunAction] Run " << run->GetRunID() << " start." << G4endl;
    G4RunManager::GetRunManager()->SetRandomNumberStore(false);
    auto analysisManager = G4AnalysisManager::Instance();
    miscIdx = analysisManager->CreateNtuple("Simulation Data", "Misc parameters");
    pcol = analysisManager->CreateNtupleSColumn(miscIdx, "Particle");
    dcol = analysisManager->CreateNtupleSColumn(miscIdx, "Distribution");
    scol = analysisManager->CreateNtupleSColumn(miscIdx, "Shape");
    rcol = analysisManager->CreateNtupleDColumn(miscIdx, "Radius");
    ecol = analysisManager->CreateNtupleSColumn(miscIdx, "Energy Type");
    acol = analysisManager->CreateNtupleSColumn(miscIdx, "Angular Distribution");

    mxtcol = analysisManager->CreateNtupleDColumn(miscIdx, "Max Theta");
    mxpcol = analysisManager->CreateNtupleDColumn(miscIdx, "Max Phi");
    mitcol = analysisManager->CreateNtupleDColumn(miscIdx, "Min Theta");
    mipcol = analysisManager->CreateNtupleDColumn(miscIdx, "Min Phi");

    xcol = analysisManager->CreateNtupleDColumn(miscIdx, "X");
    ycol = analysisManager->CreateNtupleDColumn(miscIdx, "Y");
    zcol = analysisManager->CreateNtupleDColumn(miscIdx, "Z");

    analysisManager->FinishNtuple(miscIdx);
    b_tuple_written = FALSE;
}

void RunAction::EndOfRunAction(const G4Run *run) {
    auto analysisManager = G4RootAnalysisManager::Instance();
    nofEvents = run->GetNumberOfEvent();
    if (nofEvents == 0) return;

    const auto generatorAction = dynamic_cast<const PrimaryGeneratorAction *>(G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
    if ((generatorAction != nullptr) && !b_tuple_written) {
        auto particle_name = generatorAction->GetParticleGun()->GetParticleDefinition()->GetParticleName();
        auto dist_type = generatorAction->GetParticleGun()->GetCurrentSource()->GetPosDist()->GetSourcePosType();
        auto shape_type = generatorAction->GetParticleGun()->GetCurrentSource()->GetPosDist()->GetPosDisType();
        auto dist_radius = generatorAction->GetParticleGun()->GetCurrentSource()->GetPosDist()->GetRadius();
        auto energy_dist_type = generatorAction->GetParticleGun()->GetCurrentSource()->GetEneDist()->GetEnergyDisType();
        auto angular_dist_type = generatorAction->GetParticleGun()->GetCurrentSource()->GetAngDist()->GetDistType();
        auto angular_max_phi = generatorAction->GetParticleGun()->GetCurrentSource()->GetAngDist()->GetMaxPhi();
        auto angular_max_theta = generatorAction->GetParticleGun()->GetCurrentSource()->GetAngDist()->GetMaxTheta();
        auto angular_min_phi = generatorAction->GetParticleGun()->GetCurrentSource()->GetAngDist()->GetMinPhi();
        auto angular_min_theta = generatorAction->GetParticleGun()->GetCurrentSource()->GetAngDist()->GetMinTheta();
        auto angular_direction = generatorAction->GetParticleGun()->GetCurrentSource()->GetAngDist()->GetDirection();

        analysisManager->FillNtupleSColumn(miscIdx, pcol, particle_name);
        analysisManager->FillNtupleSColumn(miscIdx, dcol, dist_type);
        analysisManager->FillNtupleSColumn(miscIdx, scol, shape_type);
        analysisManager->FillNtupleDColumn(miscIdx, rcol, dist_radius);
        analysisManager->FillNtupleSColumn(miscIdx, ecol, energy_dist_type);
        analysisManager->FillNtupleSColumn(miscIdx, acol, angular_dist_type);
        analysisManager->FillNtupleDColumn(miscIdx, mxtcol, angular_max_theta);
        analysisManager->FillNtupleDColumn(miscIdx, mxpcol, angular_max_phi);
        analysisManager->FillNtupleDColumn(miscIdx, mitcol, angular_min_theta);
        analysisManager->FillNtupleDColumn(miscIdx, mipcol, angular_min_phi);
        analysisManager->FillNtupleDColumn(miscIdx, xcol, angular_direction.x());
        analysisManager->FillNtupleDColumn(miscIdx, ycol, angular_direction.y());
        analysisManager->FillNtupleDColumn(miscIdx, zcol, angular_direction.z());
        analysisManager->AddNtupleRow(miscIdx);
        b_tuple_written = TRUE;
    }
    analysisManager->Write();

    if (IsMaster()) {
        G4cout << G4endl << "[RunAction] --------------------End of Global Run-----------------------" << G4endl
               << "[RunAction]  The run was " << nofEvents << " events " << G4endl;
        if (analysisManager->IsOpenFile()) { analysisManager->CloseFile(); }
    } else {
        G4cout << G4endl << "[RunAction] --------------------End of Local Run------------------------" << G4endl
               << "[RunAction]  The run was " << nofEvents << " events" << G4endl;
    }
}