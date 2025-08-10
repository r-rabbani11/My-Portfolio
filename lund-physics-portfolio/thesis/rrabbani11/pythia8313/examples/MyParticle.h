#ifndef MYPARTICLE__H
#define MYPARTICLE__H
#include <math.h>
#include "TObject.h"
class MyParticle : public TObject {
  public:
  MyParticle();
  MyParticle(double pt, double phi, double y, double mass, double eta, int PDG, short charge, short IsPrimary=-999, int status=0);
  ClassDef(MyParticle,1);
  double Pt() { return fPt; };
  double Phi() { return fPhi; };
  double Eta() { return fEta; };
  double P() { return fPt*cosh(fEta); };
  double Pz() { return fPt*sinh(fEta); };
  double M() { return fMass; };
  double Y() { return fY; };
  double Mt() { return sqrt(fPt*fPt + fMass*fMass); };
  double E() { return Mt()*cosh(fY); };
  bool IsPrimary() { return fIsPrimary==1; };
  bool NeutralDecay() {return !fIsPrimary; };
  bool ChargedDecay() {return fIsPrimary==2; };
  double fPt;
  double fPhi;
  double fY;
  double fMass;
  double fEta;
  int fPDG;
  short fCharge;
  short fIsPrimary;
  int fStatus;
};
#endif