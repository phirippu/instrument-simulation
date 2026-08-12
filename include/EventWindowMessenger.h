//
// Created by phil on 6/2/26.
//

#ifndef GEANT4_SIM_GDML_EVENTWINDOWMESSENGER_H
#define GEANT4_SIM_GDML_EVENTWINDOWMESSENGER_H


// EventWindowMessenger.hh
#pragma once
#include "G4UImessenger.hh"
#include "globals.hh"

class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;

class EventWindowMessenger : public G4UImessenger {
public:
    EventWindowMessenger();
    ~EventWindowMessenger() override;

    void SetNewValue(G4UIcommand* cmd, G4String val) override;

private:
    G4UIdirectory* fEventDir = nullptr;
    G4UIcmdWithADoubleAndUnit* fTimeWindowCmd = nullptr;
};

#endif //GEANT4_SIM_GDML_EVENTWINDOWMESSENGER_H
