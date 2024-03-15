//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//

/*********************************************
 * Partially reused code of G01, B3, B1,
 * and other examples provided by
 * the Geant4 v.10.x.x distributions
 *
 * (c) Philipp Oleynik 2018-2020
 * The University of Turku
 *
**********************************************/

#ifndef _DETECTORCONSTRUCTION_H_
#define _DETECTORCONSTRUCTION_H_

#include "G4VUserDetectorConstruction.hh"
#include "DetectorConstructionMessenger.hh"

#include <G4PSEnergyDeposit.hh>
#include <G4SDManager.hh>
#include <SensitiveDetector.hh>
#include <G4NistManager.hh>
#include <G4Box.hh>
#include <G4PVPlacement.hh>
#include <G4SystemOfUnits.hh>
#include <list>
#include <G4LogicalVolumeStore.hh>

/// A DetectorConstruction class allowing the use the geometry read from a GDML file

class DetectorConstructionMessenger;

class DetectorConstruction : public G4VUserDetectorConstruction {


public:
    typedef std::tuple<G4int, G4String, G4String> tupDetectorName;
    typedef std::list<tupDetectorName> lsDetectorNameRecords;
private:
    G4VPhysicalVolume *pvWorld;
    std::vector<G4LogicalVolume *>::const_iterator lvciter;
    G4LogicalVolume *lvWorld;
    G4UserLimits *fUserStepLimit;
    G4LogicalVolumeStore *logical_volume_store;
    G4int iDetectorCount, jDetectorCount;
    DetectorConstructionMessenger *fMessenger;

public:
    lsDetectorNameRecords listDetectorNames;
    DetectorConstruction(G4VPhysicalVolume *setWorld, G4LogicalVolume *LV, G4double stepSize);

    ~DetectorConstruction();

    G4VPhysicalVolume *Construct() override;

    void ConstructSDandField() override;

    void InstallDetector(const G4String &log_volume_name, const G4String &detector_name);

    void ListDetectors();

    void ColorizeDetectors();
};


#endif
