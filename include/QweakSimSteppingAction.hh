
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#ifndef QweakSimSteppingAction_h
#define QweakSimSteppingAction_h 1
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// geant4 includes
#include "G4Types.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4TrackVector.hh"
#include "G4SteppingManager.hh"
#include "G4UserSteppingAction.hh"
#include <TString.h>

// geant4 classes
class G4ParticleDefinition;

// user classes
class QweakSimUserInformation;
class QweakSimEPEvent;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
class QweakSimSteppingAction : public G4UserSteppingAction
{

public:
    QweakSimSteppingAction(QweakSimUserInformation* myUInfo, QweakSimEPEvent* myEPEvent);
    ~QweakSimSteppingAction() {
        G4cout<<"Stepping Action Desctructor"<<G4endl;
        double allp=0;
        for(int i=0; i<(int)_nProc.size(); i++) allp+=_nProc[i];
        for(int i=0; i<(int)_procName.size(); i++)
            G4cout<<i<<"\t"<<_procName[i].Data()<<"\t\t"<<_nProc[i]<<"\t"<<_nProc[i]/allp<<G4endl;
	G4cout<<"~~~ Primary ids ~~~ "<<G4endl;
	for(int i=0; i<(int)_primaryID.size(); i++)
	  G4cout<<i<<" "<<_primaryID[i]<<G4endl;
    };

    void UserSteppingAction(const G4Step*);

    G4int GetNumOfAtRestSecondaries() {
        return fpSteppingManager->GetfN2ndariesAtRestDoIt();
    };
    G4int GetNumOfAlongStepSecondaries() {
        return fpSteppingManager->GetfN2ndariesAlongStepDoIt();
    };
    G4int GetNumOfPostStepSecondaries() {
        return fpSteppingManager->GetfN2ndariesPostStepDoIt();
    };
    G4int GetTotalNumOfSecondaries() {
        return GetNumOfAtRestSecondaries() + GetNumOfAlongStepSecondaries() + GetNumOfPostStepSecondaries();
    };

    G4int GetTrackVectorStartIndex();
    G4int GetTrackVectorSize();

    G4ParticleDefinition *GetSecondaryParticleDefinition(G4int idx);
    G4String              GetSecondaryParticleName(G4int idx);
    G4double              GetSecondaryParticleTotalEnergy(G4int idx);
    G4double              GetSecondaryParticleKineticEnergy(G4int idx);
    G4double              GetSecondaryParticleXOrigin(G4int idx);
    G4double              GetSecondaryParticleYOrigin(G4int idx);
    G4double              GetSecondaryParticleZOrigin(G4int idx);
    G4ThreeVector         GetSecondaryParticleOrigin(G4int idx);
    G4ThreeVector         GetSecondaryParticleMomentum(G4int idx);
    G4String              GetSecondaryCreatorProcessName(G4int idx);

private:

    std::vector<int> _primaryID;
    std::vector<TString> _procName;
    std::vector<int> _nProc;
    G4int evtGenStatus;
    G4int myEventCounter;
    G4TrackVector *fSecondary;
    QweakSimUserInformation* myUserInfo;

    QweakSimEPEvent *myEvent;
    G4double RandomPositionZ;
    G4double targetCenterPositionZ;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

