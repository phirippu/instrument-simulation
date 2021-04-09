//
// Created by phil on 10/3/19.
//

#include "AaltoPhysicsList.hh"

AaltoPhysicsList::AaltoPhysicsList() : G4VModularPhysicsList() {
    G4LossTableManager::Instance();
    SetVerboseLevel(0);
    decay_List = new G4DecayPhysics();
    emPhysicsList = new G4EmStandardPhysics_option4();

}

AaltoPhysicsList::~AaltoPhysicsList() {
    delete emPhysicsList;
    delete decay_List;
    hadronPhys.clear();
    for (auto & hadronPhy : hadronPhys) {
        delete hadronPhy;
    }
}

void AaltoPhysicsList::ConstructParticle() {
    decay_List->ConstructParticle();
}

/////////////////////////////////////////////////////////////////////////////
void AaltoPhysicsList::ConstructProcess() {
    // Transportation
    //
    AddTransportation();
    decay_List->ConstructProcess();
    emPhysicsList->ConstructProcess();


    //em_config.AddModels();

    // Hadronic physics
    //
    for (auto & hadronPhy : hadronPhys) {
        hadronPhy->ConstructProcess();
    }
}

/////////////////////////////////////////////////////////////////////////////
void AaltoPhysicsList::AddPhysicsList(const G4String &name) {
    if (verboseLevel > 1) {
        G4cout << "PhysicsList::AddPhysicsList: <" << name << ">" << G4endl;
    }
    if (name == emName) return;

    ///////////////////////////////////
    //   ELECTROMAGNETIC MODELS
    ///////////////////////////////////
    if (name == "standard_opt4") {
        emName = name;
        delete emPhysicsList;
        hadronPhys.clear();
        emPhysicsList = new G4EmStandardPhysics_option4();
        G4RunManager::GetRunManager()->PhysicsHasBeenModified();
        G4cout << "THE FOLLOWING ELECTROMAGNETIC PHYSICS LIST HAS BEEN ACTIVATED: G4EmStandardPhysics_option4"
               << G4endl;

        ////////////////////////////////////////
        //   ELECTROMAGNETIC + HADRONIC MODELS
        ////////////////////////////////////////

    } else if (name == "HADRONTHERAPY_1") {

        AddPhysicsList("standard_opt4");
        hadronPhys.push_back(new G4DecayPhysics());
        hadronPhys.push_back(new G4RadioactiveDecayPhysics());
        hadronPhys.push_back(new G4IonBinaryCascadePhysics());
        hadronPhys.push_back(new G4EmExtraPhysics());
        hadronPhys.push_back(new G4HadronElasticPhysicsHP());
        hadronPhys.push_back(new G4StoppingPhysics());
        hadronPhys.push_back(new G4HadronPhysicsQGSP_BIC_HP());
        hadronPhys.push_back(new G4NeutronTrackingCut());

        G4cout << "HADRONTHERAPY_1 PHYSICS LIST has been activated" << G4endl;
    } else if (name == "HADRONTHERAPY_2") {
        // HP models are switched off
        AddPhysicsList("standard_opt4");
        hadronPhys.push_back(new G4DecayPhysics());
        hadronPhys.push_back(new G4RadioactiveDecayPhysics());
        hadronPhys.push_back(new G4IonBinaryCascadePhysics());
        hadronPhys.push_back(new G4EmExtraPhysics());
        hadronPhys.push_back(new G4HadronElasticPhysics());
        hadronPhys.push_back(new G4StoppingPhysics());
        hadronPhys.push_back(new G4HadronPhysicsQGSP_BIC());
        hadronPhys.push_back(new G4NeutronTrackingCut());

        G4cout << "HADRONTHERAPY_2 PHYSICS LIST has been activated" << G4endl;
    } else {
        G4cout << "PhysicsList::AddPhysicsList: <" << name << ">"
               << " is not defined"
               << G4endl;
    }
}
