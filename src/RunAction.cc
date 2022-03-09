//
// Created by phil on 5/17/18.
//

#include <ThreadRun.hh>
#include <utility>
#include "RunAction.hh"
#include "G4RootAnalysisManager.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"


RunAction::RunAction(G4String rootFileName)
        : G4UserRunAction(), localRun(nullptr), nofEvents(0), fRootFileName(std::move(rootFileName)) {
    fRunID = 0;
    iNtupleIdx = 0;
}

RunAction::~RunAction() = default;

G4Run *RunAction::GenerateRun() {
    localRun = new ThreadRun(fRootFileName);
    return (G4Run *) localRun;
}

// This method is called immediately after GenerateRun
void RunAction::BeginOfRunAction(const G4Run *run) {
    G4RunManager::GetRunManager()->SetRandomNumberStore(false);
    auto analysisManager = G4RootAnalysisManager::Instance();

    miscIdx = analysisManager->CreateNtuple("Simulation Data", "Misc parameters");
    pcol = analysisManager->CreateNtupleSColumn(miscIdx, "Particle");
    npartcol = analysisManager->CreateNtupleDColumn(miscIdx, "Particle Number");
    dcol = analysisManager->CreateNtupleSColumn(miscIdx, "Distribution");
    scol = analysisManager->CreateNtupleSColumn(miscIdx, "Shape");
    rcol = analysisManager->CreateNtupleDColumn(miscIdx, "Radius");
    ecol = analysisManager->CreateNtupleSColumn(miscIdx, "Energy Type");
    acol = analysisManager->CreateNtupleSColumn(miscIdx, "Angular Distribution");

    emincol = analysisManager->CreateNtupleDColumn(miscIdx, "Energy Min");
    emidcol = analysisManager->CreateNtupleDColumn(miscIdx, "Energy Mid");
    emaxcol = analysisManager->CreateNtupleDColumn(miscIdx, "Energy Max");
    alphcol = analysisManager->CreateNtupleDColumn(miscIdx, "Energy Alpha");

    mxtcol = analysisManager->CreateNtupleDColumn(miscIdx, "Max Theta");
    mxpcol = analysisManager->CreateNtupleDColumn(miscIdx, "Max Phi");
    mitcol = analysisManager->CreateNtupleDColumn(miscIdx, "Min Theta");
    mipcol = analysisManager->CreateNtupleDColumn(miscIdx, "Min Phi");

    xcol = analysisManager->CreateNtupleDColumn(miscIdx, "X");
    ycol = analysisManager->CreateNtupleDColumn(miscIdx, "Y");
    zcol = analysisManager->CreateNtupleDColumn(miscIdx, "Z");

    xacol = analysisManager->CreateNtupleDColumn(miscIdx, "Xa");
    yacol = analysisManager->CreateNtupleDColumn(miscIdx, "Ya");
    zacol = analysisManager->CreateNtupleDColumn(miscIdx, "Za");

    analysisManager->FinishNtuple(miscIdx);
    bTupleWritten = FALSE;
}

void RunAction::EndOfRunAction(const G4Run *run) {
    auto analysisManager = G4RootAnalysisManager::Instance();
    nofEvents = run->GetNumberOfEvent();
    if (nofEvents == 0) return;

    const auto generatorAction = dynamic_cast<const PrimaryGeneratorAction *>(G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
    if ((generatorAction != nullptr) && !bTupleWritten) {
        particle_name = generatorAction->GetParticleGun()->GetParticleDefinition()->GetParticleName();
        dist_type = generatorAction->GetParticleGun()->GetCurrentSource()->GetPosDist()->GetSourcePosType();
        shape_type = generatorAction->GetParticleGun()->GetCurrentSource()->GetPosDist()->GetPosDisType();
        dist_radius = generatorAction->GetParticleGun()->GetCurrentSource()->GetPosDist()->GetRadius();
        energy_dist_type = generatorAction->GetParticleGun()->GetCurrentSource()->GetEneDist()->GetEnergyDisType();
        angular_dist_type = generatorAction->GetParticleGun()->GetCurrentSource()->GetAngDist()->GetDistType();
        angular_max_phi = generatorAction->GetParticleGun()->GetCurrentSource()->GetAngDist()->GetMaxPhi();
        angular_max_theta = generatorAction->GetParticleGun()->GetCurrentSource()->GetAngDist()->GetMaxTheta();
        angular_min_phi = generatorAction->GetParticleGun()->GetCurrentSource()->GetAngDist()->GetMinPhi();
        angular_min_theta = generatorAction->GetParticleGun()->GetCurrentSource()->GetAngDist()->GetMinTheta();
        angular_direction = generatorAction->GetParticleGun()->GetCurrentSource()->GetAngDist()->GetDirection();
        axis_direction = generatorAction->GetPrimaryAxis();

        emin_gun = generatorAction->GetParticleGun()->GetCurrentSource()->GetEneDist()->GetEmin();
        emax_gun = generatorAction->GetParticleGun()->GetCurrentSource()->GetEneDist()->GetEmax();
        emid_gun = generatorAction->GetParticleGun()->GetCurrentSource()->GetEneDist()->GetEzero();
        palp_gun = generatorAction->GetParticleGun()->GetCurrentSource()->GetEneDist()->Getalpha();
    }

    if (!IsMaster()) {
        analysisManager->FillNtupleSColumn(miscIdx, pcol, particle_name);
        analysisManager->FillNtupleDColumn(miscIdx, npartcol, nofEvents);
        analysisManager->FillNtupleSColumn(miscIdx, dcol, dist_type);
        analysisManager->FillNtupleSColumn(miscIdx, scol, shape_type);
        analysisManager->FillNtupleDColumn(miscIdx, rcol, dist_radius);
        analysisManager->FillNtupleSColumn(miscIdx, ecol, energy_dist_type);
        analysisManager->FillNtupleSColumn(miscIdx, acol, angular_dist_type);

        analysisManager->FillNtupleDColumn(miscIdx, emincol, emin_gun);
        analysisManager->FillNtupleDColumn(miscIdx, emidcol, emid_gun);
        analysisManager->FillNtupleDColumn(miscIdx, emaxcol, emax_gun);
        analysisManager->FillNtupleDColumn(miscIdx, alphcol, palp_gun);

        analysisManager->FillNtupleDColumn(miscIdx, mxtcol, angular_max_theta);
        analysisManager->FillNtupleDColumn(miscIdx, mxpcol, angular_max_phi);
        analysisManager->FillNtupleDColumn(miscIdx, mitcol, angular_min_theta);
        analysisManager->FillNtupleDColumn(miscIdx, mipcol, angular_min_phi);
        analysisManager->FillNtupleDColumn(miscIdx, xcol, angular_direction.x());
        analysisManager->FillNtupleDColumn(miscIdx, ycol, angular_direction.y());
        analysisManager->FillNtupleDColumn(miscIdx, zcol, angular_direction.z());
        analysisManager->FillNtupleDColumn(miscIdx, xacol, axis_direction.x());
        analysisManager->FillNtupleDColumn(miscIdx, yacol, axis_direction.y());
        analysisManager->FillNtupleDColumn(miscIdx, zacol, axis_direction.z());
        analysisManager->AddNtupleRow(miscIdx);
    }

    analysisManager->Write();

    if (IsMaster()) {
//        b_tuple_written = TRUE;
        if (analysisManager->IsOpenFile()) { analysisManager->CloseFile(); }
        G4cout << G4endl << "[RunAction] --------------------End of Global Run-----------------------" << G4endl
               << "[RunAction]  The run was " << nofEvents << " events " << G4endl;
    } else {
        G4cout << G4endl << "[RunAction] --------------------End of Local Run------------------------" << G4endl
               << "[RunAction]  The run was " << nofEvents << " events" << G4endl;
    }
}