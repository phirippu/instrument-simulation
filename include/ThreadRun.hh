//
// Created by phil on 5/17/18.
//

#ifndef THREADRUN_HH
#define THREADRUN_HH


#include <G4Run.hh>
#include <map>
#include <G4ToolsAnalysisManager.hh>
#include "SensitiveDetector.hh"
#include "DetectorConstruction.hh"

class ThreadRun : public G4Run {
public:
    ThreadRun(const G4String& rootFileName);
    ~ThreadRun();
    void RecordEvent(const G4Event*) override;
    void Merge(const G4Run*) override;
private:
    std::vector<SensitiveDetector *> sdCollection;
    const DetectorConstruction *dConstruction;
    G4int iNtupleIdx;
    G4ToolsAnalysisManager *analysisManager;
};

#endif //THREADRUN_HH
