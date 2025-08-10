#include "MyParticle.h"

ClassImp(MyParticle) // ROOT macro for dictionary generation

MyParticle::MyParticle() : fPt(0), fPhi(0), fY(0), fMass(0), fEta(0), fPDG(0), fCharge(0), fIsPrimary(-999), fStatus(0) {}

MyParticle::MyParticle(double pt, double phi, double y, double mass, double eta, int PDG, short charge, short IsPrimary, int status)
    : fPt(pt), fPhi(phi), fY(y), fMass(mass), fEta(eta), fPDG(PDG), fCharge(charge), fIsPrimary(IsPrimary), fStatus(status) {}