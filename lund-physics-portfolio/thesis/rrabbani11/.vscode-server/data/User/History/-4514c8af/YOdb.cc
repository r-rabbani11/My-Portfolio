// main442.cc is a part of the PYTHIA event generator.
// Copyright (C) 2025 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL v2 or later, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// Authors: Christian Bierlich <christian.bierlich@fysik.lu.se>

// Keywords: hadronization; colour reconnection; rope hadronization

// This program provides a demonstration of the flavour rope model supplied
// in the Rope Hadronization framework. It produces four histograms
// showing the ratio of respectively K^0_s, Lambda_0, Cascade and Omega^-
// to pions as function of event multiplicity.
// No kind of Levy-Tsallis fitting, triggering or similar is done
// is this simple demonstration analysis. It should therefore not
// be taken as anything but a proof of concept.

//TRIAL PROGRAM FOR DNCH/DETA MULTIPLICITY CLASS AND YIELD RATIOS AGAIN


#include "Pythia8/Pythia.h"
using namespace Pythia8;
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"

int main() {

  TFile *outFile = new TFile("main442exp2_modified.root", "RECREATE");
  TTree *tree = new TTree("tree", "tree");

  int nCharged, nPions, nKaons, nLambdas, nXis, nOmegas;
  int forwardMult, highEtaMult, multiplicityClass;
  
  tree->Branch("nCharged", &nCharged, "nCharged/I");
  tree->Branch("nPions", &nPions, "nPions/I");
  tree->Branch("nKaons", &nKaons, "nKaons/I");
  tree->Branch("nLambdas", &nLambdas, "nLambdas/I");
  tree->Branch("nXis", &nXis, "nXis/I");
  tree->Branch("nOmegas", &nOmegas, "nOmegas/I");
  tree->Branch("forwardMult", &forwardMult, "forwardMult/I");
  tree->Branch("highEtaMult", &highEtaMult, "highEtaMult/I");
  tree->Branch("multiplicityClass", &multiplicityClass, "multiplicityClass/I");

  TH1F *hRatio[10];
  for (int i = 0; i < 10; ++i) {
    hRatio[i] = new TH1F(Form("hRatio_%d", i), "Strange-to-Pion Ratio (mult class)", 50, 10., 135.);
  }

  Pythia pythia;
  pythia.readString("Beams:eCM = 7000.");
  pythia.readString("SoftQCD:nonDiffractive = on");
  pythia.readString("Ropewalk:RopeHadronization = on");
  pythia.readString("Ropewalk:doFlavour = on");
  if (!pythia.init()) return 1;

  const int nEvent = 10000;
  for (int iEvent = 0; iEvent < nEvent; ++iEvent) {
    if (!pythia.next()) continue;

    nCharged = nPions = nKaons = nLambdas = nXis = nOmegas = 0;
    forwardMult = highEtaMult = 0;
    multiplicityClass = -1;
    
    Event& event = pythia.event;
    for (int i = 0; i < event.size(); ++i){
      Particle& p = event[i];
      if (p.isFinal() && p.isCharged()) {
        double eta = p.eta();
        if ((eta > 2.8 && eta < 5.1) || (eta > -3.8 && eta < -1.7)) ++forwardMult;
        if (fabs(eta) > 3.0 && fabs(eta) < 5.0) ++highEtaMult;
      }
      if (p.isFinal() && abs(p.eta()) < 2.5 && p.pT() > 0.1) {
        if(p.isCharged()) ++nCharged;
        int absid = abs(p.id());
        if(absid == 211) ++nPions;
        else if(absid == 310) ++nKaons;
        else if(absid == 3122) ++nLambdas;
        else if(absid == 3312) ++nXis;
        else if(absid == 3334) ++nOmegas;
      }
    }

    int multThresholds[10] = {100, 70, 50, 40, 30, 20, 15, 10, 5, 1};
    for (int i = 0; i < 10; ++i) {
      if (forwardMult >= multThresholds[i]) {
        multiplicityClass = i;
        break;
      }
    }
    
    if (nCharged < 10) continue;
    
    double strangeSum = nKaons + 2.0 * nLambdas + 3.0 * nXis + 4.0 * nOmegas;
    if (nPions > 0) {
      hRatio[multiplicityClass]->Fill(nCharged, strangeSum / double(nPions));
    }
    tree->Fill();
  }

  outFile->Write();
  outFile->Close();
  pythia.stat();
  return 0;
}
