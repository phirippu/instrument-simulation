//
// Created by phil on 6/2/26.
//

#ifndef GEANT4_SIM_GDML_DAQSETTINGS_H
#define GEANT4_SIM_GDML_DAQSETTINGS_H
// DAQSettings.hh
#pragma once
#include <CLHEP/Units/SystemOfUnits.h>

#include "G4Types.hh"

class DAQSettings {
public:
    static DAQSettings &Instance() {
        static DAQSettings s;
        return s;
    }

    void SetTimeWindow(G4double tw) { fTimeWindow = tw; }
    [[nodiscard]] G4double GetTimeWindow() const { return fTimeWindow; }

private:
    DAQSettings() = default;

    G4double fTimeWindow = 0. * CLHEP::ns; // 0 => no window (accept all)
};

#endif //GEANT4_SIM_GDML_DAQSETTINGS_H
