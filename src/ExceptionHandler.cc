//
// Created by phil on 10/22/20.
//

#include "ExceptionHandler.hh"
#include "G4RunManager.hh"
#include "G4RunManagerKernel.hh"
#include "G4StateManager.hh"
#include "G4Step.hh"
#include "G4SteppingManager.hh"
#include "G4String.hh"
#include "G4Track.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"


ExceptionHandler::ExceptionHandler() {
}

ExceptionHandler::~ExceptionHandler() {
}

ExceptionHandler::ExceptionHandler(const ExceptionHandler &)
        : G4VExceptionHandler() {
}

ExceptionHandler &ExceptionHandler::operator=(const ExceptionHandler &) {
    return *this;
}

G4bool ExceptionHandler::operator==(const ExceptionHandler &right) const {
    return (this == &right);
}

G4bool ExceptionHandler::operator!=(const ExceptionHandler &right) const {
    return (this != &right);
}

G4bool ExceptionHandler::Notify(const char *originOfException,
                                const char *exceptionCode,
                                G4ExceptionSeverity severity,
                                const char *description) {
    static const G4String es_banner
            = "\n-------- EEEE ------- G4Exception-START -------- EEEE -------\n";
    static const G4String ee_banner
            = "\n-------- EEEE -------- G4Exception-END --------- EEEE -------\n";
    static const G4String ws_banner
            = "\n-------- WWWW ------- G4Exception-START -------- WWWW -------\n";
    static const G4String we_banner
            = "\n-------- WWWW -------- G4Exception-END --------- WWWW -------\n";
    std::ostringstream message;
    if (strcmp("GeomNav1002", exceptionCode) == 0) {
        // G4cerr << "Geometry navigation problem ignored." << G4endl;
        return false;
    }

    message << "*** G4Exception : " << exceptionCode << G4endl
            << "      issued by : " << originOfException << G4endl
            << description << G4endl;
    G4bool abortionForCoreDump = false;
    G4ApplicationState aps = G4StateManager::GetStateManager()->GetCurrentState();
    switch (severity) {
        case FatalException:
            G4cerr << es_banner << message.str() << "*** Fatal Exception *** core dump ***" << G4endl;
            DumpTrackInfo();
            G4cerr << ee_banner << G4endl;
            abortionForCoreDump = true;
            break;
        case FatalErrorInArgument:
            G4cerr << es_banner << message.str() << "*** Fatal Error In Argument *** core dump ***" << G4endl;
            DumpTrackInfo();
            G4cerr << ee_banner << G4endl;
            abortionForCoreDump = true;
            break;
        case RunMustBeAborted:
            if (aps == G4State_GeomClosed || aps == G4State_EventProc) {
                G4cerr << es_banner << message.str() << "*** Run Must Be Aborted ***" << G4endl;
                DumpTrackInfo();
                G4cerr << ee_banner << G4endl;
                G4RunManager::GetRunManager()->AbortRun(false);
            }
            abortionForCoreDump = false;
            break;
        case EventMustBeAborted:
            if (aps == G4State_EventProc) {
                G4cerr << es_banner << message.str() << "*** Event Must Be Aborted ***" << G4endl;
                DumpTrackInfo();
                G4cerr << ee_banner << G4endl;
                G4RunManager::GetRunManager()->AbortEvent();
            }
            abortionForCoreDump = false;
            break;
        default:
            G4cout << ws_banner << message.str() << "*** This is just a warning message. ***"
                   << we_banner << G4endl;
            abortionForCoreDump = false;
            break;
    }
    return abortionForCoreDump;
}

void ExceptionHandler::DumpTrackInfo() {
    G4ApplicationState aps = G4StateManager::GetStateManager()->GetCurrentState();
    G4SteppingManager *steppingMgr = G4RunManagerKernel::GetRunManagerKernel()
            ->GetTrackingManager()->GetSteppingManager();
    const G4Track *theTrack = steppingMgr->GetfTrack();
    const G4Step *theStep = steppingMgr->GetfStep();

    if (aps != G4State_EventProc || !theTrack) {
        G4cerr << " **** Track information is not available at this moment" << G4endl;
    } else {
        G4cerr << "G4Track (" << theTrack << ") - track ID = " << theTrack->GetTrackID()
               << ", parent ID = " << theTrack->GetParentID() << G4endl;
        G4cerr << " Particle type : " << theTrack->GetParticleDefinition()->GetParticleName();
        if (theTrack->GetCreatorProcess()) {
            G4cerr
                    << " - creator process : " << theTrack->GetCreatorProcess()->GetProcessName()
                    << ", creator model : " << theTrack->GetCreatorModelName() << G4endl;
        } else { G4cerr << " - creator process : not available" << G4endl; }
        G4cerr << " Kinetic energy : " << G4BestUnit(theTrack->GetKineticEnergy(), "Energy")
               << " - Momentum direction : " << theTrack->GetMomentumDirection() << G4endl;
    }

    if (aps != G4State_EventProc || !theStep) {
        G4cerr << " **** Step information is not available at this moment" << G4endl;
    } else {
        G4cerr << " Step length : " << G4BestUnit(theStep->GetStepLength(), "Length")
               << " - total energy deposit : " << G4BestUnit(theStep->GetTotalEnergyDeposit(), "Energy")
               << G4endl;
        G4cerr << " Pre-step point : " << theStep->GetPreStepPoint()->GetPosition();
        G4cerr << " - Physical volume : ";
        if (theStep->GetPreStepPoint()->GetPhysicalVolume()) {
            G4cerr << theStep->GetPreStepPoint()->GetPhysicalVolume()->GetName();
            if (theStep->GetPreStepPoint()->GetMaterial()) {
                G4cerr << " (" << theStep->GetPreStepPoint()->GetMaterial()->GetName() << ")";
            } else { G4cerr << " (material not available)"; }
        } else { G4cerr << "not available"; }
        G4cerr << G4endl;
        if (theStep->GetPreStepPoint()->GetProcessDefinedStep()) {
            G4cerr << " - defined by : "
                   << theStep->GetPreStepPoint()->GetProcessDefinedStep()->GetProcessName()
                   << " - step status : " << theStep->GetPreStepPoint()->GetStepStatus() << G4endl;
        } else {
            G4cerr << " - defined by : not available" << G4endl;
        }
        G4cerr << " Post-step point : " << theStep->GetPostStepPoint()->GetPosition();
        G4cerr << " - Physical volume : ";
        if (theStep->GetPostStepPoint()->GetPhysicalVolume()) {
            G4cerr << theStep->GetPostStepPoint()->GetPhysicalVolume()->GetName();
            if (theStep->GetPostStepPoint()->GetMaterial()) {
                G4cerr << " (" << theStep->GetPostStepPoint()->GetMaterial()->GetName() << ")";
            } else { G4cerr << " (material not available)"; }
        } else { G4cerr << "not available"; }
        G4cerr << G4endl;
        if (theStep->GetPostStepPoint()->GetProcessDefinedStep()) {
            G4cerr << " - defined by : "
                   << theStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName()
                   << " - step status : " << theStep->GetPostStepPoint()->GetStepStatus() << G4endl;
        } else {
            G4cerr << " - defined by : not available" << G4endl;
        }
        G4cerr << " *** Note: Step information might not be properly updated." << G4endl;
    }
}

