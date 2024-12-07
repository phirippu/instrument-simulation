//
// Created by phil on 9/1/20.
//

#ifndef GEANT4_SIM_GDML_DETECTORCONSTRUCTIONMESSENGER_HH
#define GEANT4_SIM_GDML_DETECTORCONSTRUCTIONMESSENGER_HH


#include <G4UImessenger.hh>
#include <G4UIcmdWithAString.hh>
#include <G4UIcmdWithoutParameter.hh>
#include "DetectorConstruction.hh"

class DetectorConstruction;

class DetectorConstructionMessenger : public G4UImessenger
{
    friend class DetectorConstruction;

public:
    void SetNewValue(G4UIcommand*, G4String) override;

private:
    explicit DetectorConstructionMessenger(DetectorConstruction* DetConstr);
    ~DetectorConstructionMessenger() override;

    DetectorConstruction* fConstruction;
    G4UIdirectory* fConstructionDir;
    G4UIcmdWithAString* fCreateCmd;
    G4UIcmdWithoutParameter* fListCmd;
    G4UIcmdWithoutParameter* fColorCmd;
    G4UIcmdWithoutParameter* fColorACmd;
};


#endif //GEANT4_SIM_GDML_DETECTORCONSTRUCTIONMESSENGER_HH
