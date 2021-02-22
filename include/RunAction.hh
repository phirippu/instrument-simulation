//
// Created by phil on 5/17/18.
//

#ifndef G01_RUNACTION_HH
#define G01_RUNACTION_HH


#include <map>
#include <string>
#include <vector>
#include "G4UserRunAction.hh"
#include "G4Accumulable.hh"
#include "globals.hh"
#include "ThreadRun.hh"
#include "EventAction.hh"

extern G4bool b_tuple_written;

/// Run action class
class EventAction;

class RunAction : public G4UserRunAction {
public:
    RunAction(G4String rootFileName);

    ~RunAction() override;

    void BeginOfRunAction(const G4Run *) override;

    void EndOfRunAction(const G4Run *) override;

    G4Run *GenerateRun() override;

private:
    ThreadRun *localRun;
    G4int nofEvents;
    G4int miscIdx;
    G4int pcol, dcol, scol, rcol, ecol, acol, mxtcol, mxpcol, mitcol, mipcol, axcol, xcol, ycol, zcol;
    G4String fRootFileName;
};


#endif //G01_RUNACTION_HH
