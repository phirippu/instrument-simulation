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
// Hadrontherapy advanced example for Geant4
// See more at: https://twiki.cern.ch/twiki/bin/view/Geant4/AdvancedExamplesHadrontherapy
//
//
//    ******      SUGGESTED PHYSICS FOR ACCURATE SIMULATIONS    *********
//    ******            IN MEDICAL PHYSICS APPLICATIONS         *********
//
// 'HADRONTHERAPY_1' and 'HADRONTHERAPY_2' are both suggested;
// It can be activated inside any macro file using the command:
// /Physics/addPhysics HADRONTHERAPY_1 (HADRONTHERAPY_2)

/*********************************************
 * Partially reused code of G01, B3, B1,
 * and other examples provided by
 * the Geant4 v.10.x.x distributions
 *
 * (c) Philipp Oleynik 2018-2020
 * The University of Turku
 *
**********************************************/

//
// Created by phil on 10/3/19.
//

#ifndef AALTO_GDML_AALTOPHYSICSLIST_HH
#define AALTO_GDML_AALTOPHYSICSLIST_HH

#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"
#include "G4Region.hh"
#include "G4RegionStore.hh"
#include "G4PhysListFactory.hh"
#include "G4VPhysicsConstructor.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4HadronPhysicsQGSP_BIC.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmStandardPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4StoppingPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4IonBinaryCascadePhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4NeutronTrackingCut.hh"
#include "G4LossTableManager.hh"
#include "G4UnitsTable.hh"
#include "G4ProcessManager.hh"
#include "G4IonFluctuations.hh"
#include "G4IonParametrisedLossModel.hh"
#include "G4EmProcessOptions.hh"
#include "G4ParallelWorldPhysics.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4AutoDelete.hh"
#include "G4HadronPhysicsQGSP_BIC_AllHP.hh"
#include "G4VModularPhysicsList.hh"
#include "G4EmConfigurator.hh"
#include "globals.hh"

class G4VPhysicsConstructor;

class AaltoPhysicsList : public G4VModularPhysicsList {
public:

    AaltoPhysicsList();

    virtual ~AaltoPhysicsList();

    void ConstructParticle();

    void AddPhysicsList(const G4String &name);

    void ConstructProcess();

    void AddPackage(const G4String &name);

private:

    G4EmConfigurator em_config;
    G4String emName;
    G4VPhysicsConstructor *emPhysicsList;
    G4VPhysicsConstructor *decay_List;
    std::vector<G4VPhysicsConstructor *> hadronPhys;
};


#endif //AALTO_GDML_AALTOPHYSICSLIST_HH
