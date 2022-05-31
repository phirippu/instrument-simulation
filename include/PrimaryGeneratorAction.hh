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
/// \file persistency/gdml/G01/include/G01PrimaryGeneratorAction.hh
/// \brief Definition of the G01PrimaryGeneratorAction class
//
//
// $Id: PrimaryGeneratorAction.hh,v 1.2 2008-11-10 15:39:34 gcosmo Exp $
//
//

#ifndef _G01PRIMARYGENERATORACTION_H_
#define _G01PRIMARYGENERATORACTION_H_

#include "G4VUserPrimaryGeneratorAction.hh"
#include <G4ParticleGun.hh>
#include <G4GenericMessenger.hh>
#include "globals.hh"
#include "G4GeneralParticleSource.hh"
#include "G4IAEAphspReader.hh"

class G4Event;

class G4GeneralParticleSource;

class G4ParticleGun;

/// Minimal primary generator action to demonstrate the use of GDML geometries

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:

    explicit PrimaryGeneratorAction(G4String iaea_File);

    /*explicit PrimaryGeneratorAction(G4double Radius, CLHEP::Hep3Vector axis);*/

    ~PrimaryGeneratorAction() override;

    void GeneratePrimaries(G4Event *anEvent) override;

    [[nodiscard]] const G4GeneralParticleSource *GetParticleGun() const { return gun; }

    [[nodiscard]] CLHEP::Hep3Vector GetPrimaryAxis() const { return fDetectorAxisVector; }

    void DefineCommands();

    void InitTheGun();

    enum beamType : G4int {
        simplebeam = 0,
        sphere = 1,
        thickbeam = 2,
        simplepowerlaw = 3,
        custompowerlawbeam = 4,
        spherepowerlaw = 5,
        gaussianbeam = 6,
        nativesphere = 7,
        macrodefined = 8,
    };

private:
    G4GeneralParticleSource *InitializeGPS();

    G4GeneralParticleSource *gun;
//    G4ParticleGun *fParticleGun;
    G4double fRadius;
    G4double fAngle;
    G4int fBeamTypeInt;
    CLHEP::Hep3Vector fCentreOfDetector;
    CLHEP::Hep3Vector fDetectorAxisVector;
    CLHEP::Hep3Vector fGunPosition;
    CLHEP::Hep3Vector fGunDirection;
    G4GenericMessenger *fMessengerDir;
    beamType fPrimaryBeamType;
    G4String fIAEA_phase_file;

    static G4IAEAphspReader * theIAEAReader;

};

#endif
