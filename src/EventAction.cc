//7
// Created by phil on 5/17/18.
//

#include "EventAction.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "AnalysisManager.hh"


EventAction::EventAction()
        : G4UserEventAction() {}

EventAction::~EventAction()
= default;

void EventAction::BeginOfEventAction(const G4Event *) {
}

void EventAction::EndOfEventAction(const G4Event * /*evt */) {
}
