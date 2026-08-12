//
// Created by phil on 5/18/18.
//

#ifndef SENSITIVEDETECTOR_HH
#define SENSITIVEDETECTOR_HH

#include "G4VSensitiveDetector.hh"
#include "G4Run.hh"
#include "PrimaryGeneratorAction.hh"
#include <G4MultiFunctionalDetector.hh>

class SensitiveDetector : public G4MultiFunctionalDetector {
public:
    explicit SensitiveDetector(const G4String &name);
    explicit SensitiveDetector(const G4String &name, G4int cherenkov_enabled);

    ~SensitiveDetector() override;

    // methods from base class
    void Initialize(G4HCofThisEvent *) override;


    void EndOfEvent(G4HCofThisEvent *) override;

    [[nodiscard]] G4double GetETotalMeV() const { return eDepTotal; }
    [[nodiscard]] G4double GetAngle() const { return angleIn; }
    [[nodiscard]] G4double GetGlobalTime() const { return globaltime_g4double; }
    [[nodiscard]] G4double GetLocalTime() const { return localtime_g4double; }
    [[nodiscard]] G4double GetESecondaryMeV() const { return eSecondary; }
    [[nodiscard]] G4double GetEOut() const { return eOut; }

    [[nodiscard]] G4double GetLengthCm() const {
        return track_length_cm;
    }

    void ClearDetectorMeasurements() {
        eDepTotal = 0;
        eSecondary = 0;
        angleIn = 0.0;
        eOut = 0.;
        track_length_cm = 0.;
    }

protected:
    G4bool ProcessHits(G4Step *step, G4TouchableHistory *) override;

private:
    G4double eDepTotal{}, eSecondary{}, angleIn{}, eOut{}, globaltime_g4double{}, localtime_g4double{};
    G4double track_length_cm{};
    const PrimaryGeneratorAction *generatorAction;
    G4int fCherenkov_enabled;
};


#endif
