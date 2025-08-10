#include "MyParticle.h"
ClassImp(MyParticle)
MyParticle::MyParticle():
TObject(),
  fPt(-999),
  fPhi(-999),
  fY(-999),
  fMass(-999),
  fEta(-999),
  fPDG(-999),
  fCharge(-999),
  fIsPrimary(-999),
  fStatus(0)
{};
MyParticle::MyParticle(double pt, double phi, double y, double mass, double eta, int PDG, short charge, short PrimaryFlag, int status):
  TObject(),
  fPt(pt),
  fPhi(phi),
  fY(y),
  fMass(mass),
  fEta(eta),
  fPDG(PDG),
  fCharge(charge),
  fIsPrimary(PrimaryFlag),
  fStatus(status)
{};