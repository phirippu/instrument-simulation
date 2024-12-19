//
// Created by phil on 5/18/18.
//

#ifndef SENSITIVEDETECTOR_HH
#define SENSITIVEDETECTOR_HH

#include "G4VSensitiveDetector.hh"
#include "G4SystemOfUnits.hh"

#include "G4Run.hh"
#include "PrimaryGeneratorAction.hh"

#include <vector>
#include <G4MultiFunctionalDetector.hh>
#include <G4PSEnergyDeposit.hh>

class G4PSEnergyDeposit;

class SensitiveDetector : public G4MultiFunctionalDetector {
public:
    explicit SensitiveDetector(const G4String &name);

    ~SensitiveDetector() override;

    // methods from base class
    void Initialize(G4HCofThisEvent *) override;

    G4bool ProcessHits(G4Step *step, G4TouchableHistory *) override;

    void EndOfEvent(G4HCofThisEvent *) override;

    [[nodiscard]] G4double GetETotal() const { return eDepTotal; }
    [[nodiscard]] G4double GetAngle() const { return angleIn; }
    [[nodiscard]] G4double GetTime() const { return time_g4double; }
    [[nodiscard]] G4double GetESecondary() const { return eSecondary; }
    [[nodiscard]] G4double GetEOut() const { return eOut; }

    void ClearETotal() {
        eDepTotal = 0;
        eSecondary = 0;
        angleIn = 0.0;
        eOut = 0.;
    }

private:
    G4double eDepTotal{}, eSecondary{}, angleIn{}, eOut{}, time_g4double{};
    const PrimaryGeneratorAction *generatorAction;
};


#endif
