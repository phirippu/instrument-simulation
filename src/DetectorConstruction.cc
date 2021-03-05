//
// Created by phil on 5/17/18.
//

#include <DetectorConstruction.hh>
#include <G4UnionSolid.hh>
#include <G4LogicalVolumeStore.hh>
#include <G4UserLimits.hh>
#include <G4VisExtent.hh>

void DetectorConstruction::ConstructSDandField() {
    // this method is called before each run.
    // Detectors are added here
    G4cout << "ConstructSDandField" << G4endl;
    G4SDManager::GetSDMpointer()->SetVerboseLevel(1);
    for (const auto &record : listDetectorNames) {
        const auto& detName = std::get<1>(record);
        const auto& volName = std::get<2>(record);
        if (G4SDManager::GetSDMpointer()->FindSensitiveDetector(detName, false)) {
            G4cerr << "Detector FOUND! It should not happen.\n" << G4endl;
        } else {
            // create thread-local SensitiveDetector objects for each detector.
            auto sdMFD = new SensitiveDetector(detName);
            G4SDManager::GetSDMpointer()->AddNewDetector(sdMFD);
            SetSensitiveDetector(volName, sdMFD);
        }
    }
}

G4VPhysicalVolume *DetectorConstruction::Construct() {
    std::vector<G4LogicalVolume *>::const_iterator lvciter;
    for (lvciter = lvs->begin(); lvciter != lvs->end(); lvciter++) {
        if ((*lvciter)->GetName().contains("target")) {
            (*lvciter)->SetUserLimits(fTinyStepLimit);
            G4cerr << "/detector/add " << (*lvciter)->GetName() << G4endl;
        }
    }

    return pvWorld;
}

DetectorConstruction::DetectorConstruction(G4VPhysicalVolume *setWorld, G4LogicalVolume *LV) {
    pvWorld = setWorld;
    lvWorld = LV;
    iDetectorCount = 0;
    jDetectorCount = 0;
    G4double tiny10um = 0.01 * mm;
    fTinyStepLimit = new G4UserLimits(tiny10um);
    lvs = G4LogicalVolumeStore::GetInstance();
    fMessenger = new DetectorConstructionMessenger(this);
}

void DetectorConstruction::InstallDetector(const G4String &log_volume_name, const G4String &detector_name) {

    iDetectorCount++;
    listDetectorNames.emplace_back(iDetectorCount, detector_name, log_volume_name);
}

DetectorConstruction::~DetectorConstruction() {
    delete fTinyStepLimit;
    delete fMessenger;

    listDetectorNames.clear();
}

void DetectorConstruction::ListDetectors() {
    if (listDetectorNames.empty()) {
        G4cout << "Detector list empty." << G4endl;
    } else {
        G4cout << "Detector ID\tName\tLogVolName" << G4endl;
        for (const auto &record : listDetectorNames) {
            G4cout << std::get<0>(record) << "\t"
                   << std::get<1>(record) << "\t"
                   << std::get<2>(record) << G4endl;
        }
    }
}
