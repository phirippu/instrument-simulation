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
    G4int pcol, dcol, scol, rcol, ecol, acol, mxtcol, mxpcol, mitcol, mipcol, xcol, ycol, zcol, xacol, yacol, zacol, emincol, emaxcol, emidcol, npartcol;
    G4String fRootFileName;

    G4String particle_name;
    G4String dist_type;
    G4String shape_type;
    G4double dist_radius;
    G4String energy_dist_type;
    G4String angular_dist_type;
    G4double angular_max_phi;
    G4double angular_max_theta;
    G4double angular_min_phi;
    G4double angular_min_theta;
    CLHEP::Hep3Vector angular_direction;
    CLHEP::Hep3Vector axis_direction;

    G4double emin_gun;
    G4double emax_gun;
    G4double emid_gun;

};


#endif //G01_RUNACTION_HH
