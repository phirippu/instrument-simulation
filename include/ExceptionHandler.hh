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
//
//
//
// ------------------------------------------------------------
//      GEANT 4 class header file
//
//
//      ---------------- G4ExceptionHandler ----------------
//
// Authors: M.Asai - August 2002
//
// ------------------------------------------------------------
//
// Class description:
//
// Abstract base class which need to be notified when G4Exception occurs.
// The concrete class object derived from this base class will be automatically
// registered to G4StateManager and the virtual method Notify() will be invoked
// when G4Exception occurs.

// ------------------------------------------------------------

//
// Created by phil on 10/22/20.
//

#ifndef GEANT4_SIM_GDML_EXCEPTIONHANDLER_HH
#define GEANT4_SIM_GDML_EXCEPTIONHANDLER_HH

#include "globals.hh"
#include "G4VExceptionHandler.hh"
#include "G4ExceptionSeverity.hh"

class ExceptionHandler : public G4VExceptionHandler {

public:

    ExceptionHandler();

    virtual ~ExceptionHandler();

    G4bool operator==(const ExceptionHandler &right) const;

    G4bool operator!=(const ExceptionHandler &right) const;

public: // with description

    virtual G4bool Notify(const char *originOfException,
                          const char *exceptionCode,
                          G4ExceptionSeverity severity,
                          const char *description);
    // Virtual method which will be invoked by G4StateManager when
    // G4Exception occurs.
    // If TRUE returned, core dump will be generated, while FALSE returned,
    // program execution continues.

private:

    ExceptionHandler(const ExceptionHandler &right);

    ExceptionHandler &operator=(const ExceptionHandler &right);

private:
    void DumpTrackInfo();
};


#endif //GEANT4_SIM_GDML_EXCEPTIONHANDLER_HH
