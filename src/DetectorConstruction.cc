//
// Created by phil on 5/17/18.
//

#include <DetectorConstruction.hh>
#include <G4UnionSolid.hh>
#include <G4LogicalVolumeStore.hh>
#include <G4UserLimits.hh>
#include <G4VisExtent.hh>
#include <G4Color.hh>
#include <G4VisAttributes.hh>

void DetectorConstruction::ConstructSDandField() {
    // this method is called before each run.
    // Detectors are added here
    G4cout << "ConstructSDandField" << G4endl;
    G4SDManager::GetSDMpointer()->SetVerboseLevel(1);
    for (const auto &record: listDetectorNames) {
        const auto &detName = std::get<1>(record);
        const auto &volName = std::get<2>(record);
        if (G4SDManager::GetSDMpointer()->FindSensitiveDetector(detName, false)) {
            G4cerr << "Detector FOUND! It should not happen.\n" << G4endl;
        } else {
            // create thread-local SensitiveDetector objects for each detector.
            auto sdMFD = new SensitiveDetector(detName);
            G4SDManager::GetSDMpointer()->AddNewDetector(sdMFD);
            SetSensitiveDetector(volName, sdMFD);
            if (fUserStepLimit) {
                logical_volume_store->GetVolume(volName, true)->SetUserLimits(fUserStepLimit);
            }
        }
    }
}

G4VPhysicalVolume *DetectorConstruction::Construct() {
    return pvWorld;
}

DetectorConstruction::DetectorConstruction(G4VPhysicalVolume *setWorld, G4LogicalVolume *LV, G4double stepSize) {
    G4double stepSizePhysUnits;

    pvWorld = setWorld;
    lvWorld = LV;
    iDetectorCount = 0;
    jDetectorCount = 0;

    stepSizePhysUnits = stepSize * 0.001 * mm;
    if (stepSizePhysUnits > 0) {
        fUserStepLimit = new G4UserLimits(stepSizePhysUnits);
    } else {
        fUserStepLimit = nullptr;
    }
    logical_volume_store = G4LogicalVolumeStore::GetInstance();
    fMessenger = new DetectorConstructionMessenger(this);
}

void DetectorConstruction::InstallDetector(const G4String &log_volume_name, const G4String &detector_name) {

    iDetectorCount++;
    listDetectorNames.emplace_back(iDetectorCount, detector_name, log_volume_name);
}

DetectorConstruction::~DetectorConstruction() {
    delete fUserStepLimit;
    delete fMessenger;

    listDetectorNames.clear();
}

void DetectorConstruction::ListDetectors() {
    if (listDetectorNames.empty()) {
        G4cerr << "Detector list empty." << G4endl;
    } else {
        G4cout << "Detector ID\tName\tLogVolName" << G4endl;
        for (const auto &record: listDetectorNames) {
            G4cout << std::get<0>(record) << "\t"
                   << std::get<1>(record) << "\t"
                   << std::get<2>(record) << G4endl;;
        }
    }
}

void DetectorConstruction::ColorizeDetectors() {
    G4int i;
    auto const colormap = {
            G4Colour(0.050383, 0.029803, 0.527975),
            G4Colour(0.132381, 0.022258, 0.56325),
            G4Colour(0.200445, 0.017902, 0.593364),
            G4Colour(0.254627, 0.013882, 0.615419),
            G4Colour(0.312543, 0.008239, 0.6357),
            G4Colour(0.362553, 0.003243, 0.649245),
            G4Colour(0.417642, 0.000564, 0.65839),
            G4Colour(0.46555, 0.004545, 0.660139),
            G4Colour(0.517933, 0.021563, 0.654109),
            G4Colour(0.562738, 0.051545, 0.641509),
            G4Colour(0.610667, 0.090204, 0.619951),
            G4Colour(0.650746, 0.125309, 0.595617),
            G4Colour(0.69284, 0.165141, 0.564522),
            G4Colour(0.72767, 0.200586, 0.535293),
            G4Colour(0.764193, 0.240396, 0.502126),
            G4Colour(0.798216, 0.280197, 0.469538),
            G4Colour(0.826588, 0.315714, 0.441316),
            G4Colour(0.856547, 0.356066, 0.410322),
            G4Colour(0.881443, 0.392529, 0.383229),
            G4Colour(0.907365, 0.434524, 0.35297),
            G4Colour(0.928329, 0.472975, 0.326067),
            G4Colour(0.949217, 0.517763, 0.295662),
            G4Colour(0.965024, 0.559118, 0.268513),
            G4Colour(0.979233, 0.607532, 0.238013),
            G4Colour(0.98826, 0.652325, 0.211364),
            G4Colour(0.993814, 0.704741, 0.183043),
            G4Colour(0.994141, 0.753137, 0.161404),
            G4Colour(0.988648, 0.809579, 0.145357),
            G4Colour(0.977995, 0.861432, 0.142808),
            G4Colour(0.959276, 0.921407, 0.151566),
    };
    if (listDetectorNames.empty()) {
        G4cerr << "Detector list empty. Nothing to color." << G4endl;
    } else {
        i = 0;
        for (const auto &record: listDetectorNames) {
            auto logical_vol_name = std::get<2>(record);
            G4cerr << "Detector with LV '" << logical_vol_name << "' colored." << G4endl;
            auto color_attribute = new G4VisAttributes(data(colormap)[i++]);
            color_attribute->SetForceWireframe(false);
            color_attribute->SetVisibility(true);
            logical_volume_store->GetVolume(logical_vol_name, true)->SetVisAttributes(color_attribute);
            if (i >= size(colormap)) { i = 0; }
        }
    }
}
