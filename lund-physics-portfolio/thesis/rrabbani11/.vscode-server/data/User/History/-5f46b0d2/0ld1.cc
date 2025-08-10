#include <iostream>

#include "Pythia8/Pythia8.h"

int main() {
  int nevents = 100;
  Pythia8::Pythia pythia;
  pythia.readString("Beams:idA = 2212");
  pythia.readString("Beams:idB = 2212");
  pythia.readString("Beams:eCM = 14000.");
  pythia.readString("SoftQCD:all = on");
  pythia.readString("HardQCD:all = on");

  for (int i = 0; i < nevents; ++i) {
    
    if(!pythia.next()) continue; // skip if event generation failed

    int entries = pythia.event.size();
    std::cout << "Event: " << i << std::endl;
    std::cout << "Event size: " << entries << std::endl;
    for(int j = 0; j < entries; ++j) {
      int id = pythia.event[j].id();

      double m = pythia.event[j].m();
      double px = pythia.event[j].px();
      double py = pythia.event[j].py();
      double pz = pythia.event[j].pz();
      double pabs = sqrt(px*px + py*py + pz*pz);

      std::cout << id << " " << m << " " << px << " " << py << " " << pz << " " << pabs << std::endl;
    }
      
  }
  return 0;
}
