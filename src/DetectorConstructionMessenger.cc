//
// Created by phil on 9/1/20.
//

#include <G4RunManager.hh>
#include <G4Color.hh>
#include "DetectorConstructionMessenger.hh"


void DetectorConstructionMessenger::SetNewValue(G4UIcommand *g4command, G4String g4string_name) {
    if (g4command == fCreateCmd) {
        fConstruction->InstallDetector(g4string_name, g4string_name);
        G4RunManager::GetRunManager()->GeometryHasBeenModified(true);
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
    if (g4command == fListCmd) {
        fConstruction->ListDetectors();
    }
    if (g4command == fColorCmd) {
        fConstruction->ColorizeDetectors();
    }
}

DetectorConstructionMessenger::DetectorConstructionMessenger(DetectorConstruction *DetConstr) :
        fConstruction(DetConstr) {
    fConstructionDir = new G4UIdirectory("/detector/");
    fConstructionDir->SetGuidance("Sensitive detector manipulations.");
    fCreateCmd = new G4UIcmdWithAString("/detector/add", this);
    fCreateCmd->SetGuidance("Add a sensitive detector into stack.");
    fCreateCmd->SetParameterName("logical_volume", false);
    fCreateCmd->AvailableForStates(G4State_Idle);
    fListCmd = new G4UIcmdWithoutParameter("/detector/list", this);
    fListCmd->SetGuidance("List sensitive detectors.");
    fListCmd->AvailableForStates(G4State_Idle);
    fColorCmd = new G4UIcmdWithoutParameter("/detector/color", this);
    fColorCmd->SetGuidance("Colorize sensitive detectors.");
    fColorCmd->AvailableForStates(G4State_Idle);
}

DetectorConstructionMessenger::~DetectorConstructionMessenger() {
    delete fConstructionDir;
    delete fCreateCmd;
    delete fListCmd;
}
