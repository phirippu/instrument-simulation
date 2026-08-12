//
// Created by phil on 6/2/26.
//

#include "EventWindowMessenger.h"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4SystemOfUnits.hh"
#include "DAQSettings.h"

EventWindowMessenger::EventWindowMessenger() {
    fEventDir = new G4UIdirectory("/event/");
    fEventDir->SetGuidance("Event-level controls");

    fTimeWindowCmd = new G4UIcmdWithADoubleAndUnit("/event/timeWindow", this);
    fTimeWindowCmd->SetGuidance("Set acquisition time window for accepting hits.");
    fTimeWindowCmd->SetParameterName("tw", false);
    fTimeWindowCmd->SetUnitCategory("Time");
    fTimeWindowCmd->SetDefaultUnit("ns");
    fTimeWindowCmd->SetRange("tw>=0.");
    fTimeWindowCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

EventWindowMessenger::~EventWindowMessenger() {
    delete fTimeWindowCmd;
    delete fEventDir;
}

void EventWindowMessenger::SetNewValue(G4UIcommand* cmd, const G4String val) {
    if (cmd == fTimeWindowCmd) {
        const auto tw = G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(val);
        DAQSettings::Instance().SetTimeWindow(tw);
        G4cout << "[DAQ] Time window set to " << tw/CLHEP::ns << " ns\n";
    }
}