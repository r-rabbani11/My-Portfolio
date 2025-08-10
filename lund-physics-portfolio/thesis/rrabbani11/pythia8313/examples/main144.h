// main144.h - auxiliary classes for use in main144.cc

//==========================================================================

#include "Pythia8/Pythia.h"
using namespace Pythia8;

//==========================================================================

// Implement a user supplied UserHooks derived class inside
// this wrapper, which will allow you to give settings
// that can be supplied in the .cmnd file.

class UserHooksWrapper : public UserHooks {
public:
   UserHooksWrapper() : settings(nullptr) { }

   // Add the settings you want available in the run card
   // in this method.
   void additionalSettings(Settings* settingsIn) {
     settings = settingsIn;
     settings->addFlag("UserHooks:doMPICut",false);
     settings->addMode("UserHooks:nMPICut",0, true, false, 0, 0);
   }

   // Override the relevant methods from UserHooks here.
   bool canVetoPartonLevel() final {
     if(settings->flag("UserHooks:doMPICut")) {
             return true;
     }
     return false;
   }

   bool doVetoPartonLevel(const Event& ) final {
     if( infoPtr->nMPI() < settings->mode("UserHooks:nMPICut"))
       return true;
     return false;
   }


private:
  Settings* settings;
};

#ifdef PY8ROOT
#include "TFile.h"
#include "TTree.h"
#include "RootEvent.h"

bool RootTrack::init(Pythia8::Particle &p) {
  if (p.isFinal()) {
    phi = p.phi();
    eta = p.eta();
    y   = p.y();
    pT  = p.pT();
    pid = p.id();
    return true;
  }
  return false;
}

bool RootEvent::init(const Pythia8::Info *infoPtr) {
  tracks.clear();
  weight = infoPtr->weight();
  return true;
}
#endif // PY8ROOT