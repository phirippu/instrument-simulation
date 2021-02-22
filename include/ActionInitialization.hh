//
// Created by phil on 5/17/18.
//

#ifndef G01_ACTIONINITIALIZATION_HH
#define G01_ACTIONINITIALIZATION_HH

#include <pwdefs.hh>
#include "G4VUserActionInitialization.hh"
#include "RunAction.hh"

class ActionInitialization : public G4VUserActionInitialization {
public:
    explicit ActionInitialization(G4double particleBeamRadius, G4String rootFileName);

    ~ActionInitialization() override;

    void BuildForMaster() const override;

    void Build() const override;

private:
    G4double aGunRadius;
    G4String fRootFileName;
};


#endif //G01_ACTIONINITIALIZATION_HH
