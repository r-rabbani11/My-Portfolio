#ifndef ROOTEVENT_H
#define ROOTEVENT_H

#include <vector>
#include "TObject.h"

// Only forward-declare Pythia if we compile with -DPY8ROOT
#ifdef PY8ROOT
namespace Pythia8 {
  class Particle;
  class Info;
}
#endif

// -------------------------------------------------------------------------
// RootTrack
// -------------------------------------------------------------------------
class RootTrack : public TObject {
public:
  double phi;
  double eta;
  double y;
  double pT;
  int    pid;
  bool isHadron;
  bool isCharged;

  RootTrack() : phi(0), eta(0), y(0), pT(0), pid(0), isHadron(false), 
    isCharged(false) {}

#ifdef PY8ROOT
  // Defined in main145.cc when compiled with -DPY8ROOT
  bool init(Pythia8::Particle &p);
#endif

  ClassDef(RootTrack, 1)
};

// -------------------------------------------------------------------------
// RootEvent
// -------------------------------------------------------------------------
class RootEvent : public TObject {
public:
  double weight;
  std::vector<RootTrack> tracks;
#ifdef PY8ROOT
  bool init(const Pythia8::Info *infoPtr);
#endif

  ClassDef(RootEvent, 1)
};

#endif