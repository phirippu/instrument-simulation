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
#include "G4RunManager.hh"
#include "G4Run.hh"


void DetectorConstruction::ConstructSDandField() {
    // this method is called before each run.
    // Detectors are added here
    G4cout << "ConstructSDandField" << G4endl;
    G4SDManager::GetSDMpointer()->SetVerboseLevel(1);
    for (const auto &record: listDetectorNames) {
        const auto &detName = std::get<1>(record);
        const auto &volName = std::get<2>(record);
        const auto &cherenkov_enabled = std::get<3>(record);
        if (G4SDManager::GetSDMpointer()->FindSensitiveDetector(detName, false)) {
            G4cerr << "[ERROR] Detector was already FOUND! It should not happen.\n" << G4endl;
        } else {
            // create thread-local SensitiveDetector objects for each detector.
            auto sdMFD = new SensitiveDetector(detName, cherenkov_enabled);
            G4SDManager::GetSDMpointer()->AddNewDetector(sdMFD);
            SetSensitiveDetector(volName, sdMFD);
            if (fUserStepLimit) {
                logical_volume_store->GetVolume(volName, true)->SetUserLimits(fUserStepLimit);
            }
            if (cherenkov_enabled == 1) {
                if (G4Threading::IsMasterThread()) {
                    DefineOpticalProperties(logical_volume_store->GetVolume(volName, true));
                }
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

void DetectorConstruction::InstallDetector(const G4String &log_volume_name, const G4String &detector_name,
                                           const G4int cherenkov) {
    iDetectorCount++;
    listDetectorNames.emplace_back(iDetectorCount, detector_name, log_volume_name, cherenkov);
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
                    << std::get<2>(record) << "\t"
                    << std::get<3>(record) << G4endl;;
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
            // this object has to exist during the whole time a vis engine runs.
            // TODO: fix with a vector that is deleted upon ~DetectorConstruction()
            color_attribute->SetForceWireframe(false);
            color_attribute->SetVisibility(true);
            logical_volume_store->GetVolume(logical_vol_name, true)->SetVisAttributes(color_attribute);
            if (i >= size(colormap)) { i = 0; }
        }
    }
}


void DetectorConstruction::ColorizeEverything() {
    G4int i;
    auto const colormap = {
        G4Colour(0.267004, 0.004874, 0.329415),
        G4Colour(0.277018, 0.050344, 0.375715),
        G4Colour(0.282656, 0.100196, 0.42216),
        G4Colour(0.282623, 0.140926, 0.457517),
        G4Colour(0.277134, 0.185228, 0.489898),
        G4Colour(0.267968, 0.223549, 0.512008),
        G4Colour(0.253935, 0.265254, 0.529983),
        G4Colour(0.239346, 0.300855, 0.540844),
        G4Colour(0.221989, 0.339161, 0.548752),
        G4Colour(0.206756, 0.371758, 0.553117),
        G4Colour(0.190631, 0.407061, 0.556089),
        G4Colour(0.177423, 0.437527, 0.557565),
        G4Colour(0.163625, 0.471133, 0.558148),
        G4Colour(0.151918, 0.500685, 0.557587),
        G4Colour(0.139147, 0.533812, 0.555298),
        G4Colour(0.127568, 0.566949, 0.550556),
        G4Colour(0.120565, 0.596422, 0.543611),
        G4Colour(0.12138, 0.629492, 0.531973),
        G4Colour(0.134692, 0.658636, 0.517649),
        G4Colour(0.166383, 0.690856, 0.496502),
        G4Colour(0.20803, 0.718701, 0.472873),
        G4Colour(0.266941, 0.748751, 0.440573),
        G4Colour(0.327796, 0.77398, 0.40664),
        G4Colour(0.404001, 0.800275, 0.362552),
        G4Colour(0.477504, 0.821444, 0.318195),
        G4Colour(0.565498, 0.84243, 0.262877),
        G4Colour(0.647257, 0.8584, 0.209861),
        G4Colour(0.741388, 0.873449, 0.149561),
        G4Colour(0.82494, 0.88472, 0.106217),
        G4Colour(0.916242, 0.896091, 0.100717),
    };
    i = 0;
    auto lvs = G4LogicalVolumeStore::GetInstance();
    std::vector<G4LogicalVolume *>::const_iterator lvciter;
    for (lvciter = lvs->begin(); lvciter != lvs->end(); lvciter++) {
        if (auto logical_vol_name = (*lvciter)->GetName()) {
            auto color_attribute = new G4VisAttributes(data(colormap)[i++]);
            // this object has to exist during the whole time a vis engine runs.
            // TODO: fix with a vector that is deleted upon ~DetectorConstruction()
            color_attribute->SetForceWireframe(false);
            color_attribute->SetVisibility(true);
            auto volume = logical_volume_store->GetVolume(logical_vol_name, true);
            G4StrUtil::to_lower(logical_vol_name);
            if (logical_vol_name.find("world") == std::string::npos) {
                volume->SetVisAttributes(color_attribute);
                G4cerr << "Logical volume '" << logical_vol_name << "' colored." << G4endl;
                if (i >= size(colormap)) { i = 0; }
            }
        }
    }
}

void DetectorConstruction::DefineOpticalProperties(G4LogicalVolume *optical_volume) {
    G4Material *detectorMaterial = optical_volume->GetMaterial();

    // Create a properties table
    auto *mpt = new G4MaterialPropertiesTable();

    // Refractive index (example: constant value)
    const G4int numEntries = 10;
    G4double photonEnergy[numEntries] = {
        0.104 * eV, 0.552 * eV, 1.000 * eV, 1.447 * eV, 1.895 * eV, 2.343 * eV, 2.790 * eV, 3.238 * eV, 3.686 * eV,
        4.133 * eV
    };
    G4double refractiveIndex[numEntries] = {1.937, 1.733, 1.724, 1.715, 1.704, 1.689, 1.671, 1.649, 1.625, 1.596};

    mpt->AddProperty("RINDEX", photonEnergy, refractiveIndex, numEntries);

    // Example: Add absorption length (adjust values as needed)
    G4double absorptionLength[numEntries] = {
        1.0 * m, 1.0 * m, 1.0 * m, 1.0 * m, 1.0 * m, 1.0 * m, 1.0 * m, 1.0 * m, 1.0 * m, 1.0 * m
    };
    mpt->AddProperty("ABSLENGTH", photonEnergy, absorptionLength, numEntries);

    // Assign the properties to the material
    detectorMaterial->SetMaterialPropertiesTable(mpt);
    G4cerr << "Material " << reinterpret_cast<int64_t>(detectorMaterial) << " SetMaterialPropertiesTable " <<
            reinterpret_cast<int64_t>(mpt) << G4endl;
}
