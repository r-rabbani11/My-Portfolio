// RootEvent.cxx
#include "RootEvent.h"

#ifdef PY8ROOT
bool RootTrack::init(Pythia8::Particle &p) {
    // Implementation of the init method for Pythia8::Particle
    // Example:
    phi = p.phi();
    eta = p.eta();
    y = p.y();
    pT = p.pT();
    pid = p.id();
    isHadron = p.isHadron();
    isCharged = p.isCharged();
    return true;
}

bool RootEvent::init(const Pythia8::Info *infoPtr) {
    // Implementation of the init method for Pythia8::Info
    // Example:
    weight = infoPtr->weight();
    return true;
}
#endif

ClassImp(RootTrack)
ClassImp(RootEvent)