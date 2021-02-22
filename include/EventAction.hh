//
// Created by phil on 5/17/18.
//

#ifndef EVENTACTION_H
#define EVENTACTION_H

#include <G4UserEventAction.hh>
#include <G4THitsMap.hh>
#include <G4SDManager.hh>
#include "globals.hh"
#include "RunAction.hh"
#include "SensitiveDetector.hh"
#include "DetectorConstruction.hh"

class RunAction;
class SensitiveDetector;

class EventAction : public G4UserEventAction {
public:
    explicit EventAction();

    ~EventAction() override;

    void BeginOfEventAction(const G4Event *) override;

    void EndOfEventAction(const G4Event *) override;

private:

};

#endif //EVENTACTION_H
