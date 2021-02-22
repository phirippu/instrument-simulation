//
// Created by phil on 5/17/18.
//

#include <G4VUserActionInitialization.hh>
#include <utility>
#include <ExceptionHandler.hh>
#include "ActionInitialization.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "PrimaryGeneratorAction.hh"

ActionInitialization::ActionInitialization(G4double particleBeamRadius, G4String rootFileName)
        : G4VUserActionInitialization(), aGunRadius(particleBeamRadius), fRootFileName(std::move(rootFileName)) {}

ActionInitialization::~ActionInitialization() {
//    delete rLocalRunAction;
}

void ActionInitialization::BuildForMaster() const {
    SetUserAction(new RunAction(fRootFileName));
}

void ActionInitialization::Build() const {
    auto evAction = new EventAction();
    auto rLocalRunAction = new RunAction(fRootFileName);
    SetUserAction(evAction);
    SetUserAction(rLocalRunAction);
    auto exc = new ExceptionHandler();
    SetUserAction(new PrimaryGeneratorAction(aGunRadius));
}
