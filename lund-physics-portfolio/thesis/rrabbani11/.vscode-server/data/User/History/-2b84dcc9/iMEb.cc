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

//MAIN COPY OF MAIN442 WITH ROOT FILE AND TREE

#include "Pythia8/Pythia.h"
using namespace Pythia8;
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"

//==========================================================================

int main() {

  TFile *outFile = new TFile("main442exp.root", "RECREATE");
  TTree *tree = new TTree("tree", "tree");

  int nCharged, nPions, nKaons, nLambdas, nXis, nOmegas;
  int forwardMult, highEtaMult, multiplicityClass; // New variables

  tree->Branch("nCharged", &nCharged, "nCharged/I");
  tree->Branch("nPions", &nPions, "nPions/I");
  tree->Branch("nKaons", &nKaons, "nKaons/I");
  tree->Branch("nLambdas", &nLambdas, "nLambdas/I");
  tree->Branch("nXis", &nXis, "nXis/I");
  tree->Branch("nOmegas", &nOmegas, "nOmegas/I");

  tree->Branch("forwardMult", &forwardMult, "forwardMult/I");  // Forward multiplicity (V0M)
  tree->Branch("highEtaMult", &highEtaMult, "highEtaMult/I");  // High-η multiplicity
  tree->Branch("multiplicityClass", &multiplicityClass, "multiplicityClass/I"); // Multiplicity class

  TH1F *hPion = new TH1F("hPion", "pions (mult)", 50, 10., 135.);
  TH1F *hKaon = new TH1F("hKaon", "kaons (mult)", 50, 10., 135.);
  TH1F *hLambda = new TH1F("hLambda", "lambdas (mult)", 50, 10., 135.);
  TH1F *hXi = new TH1F("hXi", "xi (mult)", 50, 10., 135.);
  TH1F *hOmega = new TH1F("hOmega", "omega (mult)", 50, 10., 135.);

  // Multiplicity ratio histograms
  TH1F *hKp = new TH1F("hKp", "kaon / pion (multiplicity)", 50, 10., 135.);
  TH1F *hLp = new TH1F("hLp", "lambda / pion (multiplicity)", 50, 10., 135.);
  TH1F *hXp = new TH1F("hXp", "xi / pion (multiplicity)", 50, 10., 135.);
  TH1F *hOp = new TH1F("hOp", "omega / pion (multiplicity)", 50, 10., 135.);

  // Generator setup
  Pythia pythia;
  pythia.readString("Beams:eCM = 7000.");
  pythia.readString("SoftQCD:nonDiffractive = on");

  // Rope hadronization settings
  pythia.readString("Ropewalk:RopeHadronization = on");
  pythia.readString("Ropewalk:doShoving = off");
  pythia.readString("Ropewalk:doFlavour = on");
  pythia.readString("Ropewalk:r0 = 0.5");
  pythia.readString("Ropewalk:m0 = 0.2");
  pythia.readString("Ropewalk:beta = 0.1");

  pythia.readString("PartonVertex:setVertex = on");
  pythia.readString("ParticleDecays:limitTau0 = on");
  pythia.readString("ParticleDecays:tau0Max = 10");

  if (!pythia.init()) return 1;

  const int nEvent = 10000;

  for (int iEvent = 0; iEvent < nEvent; ++iEvent) {
    if (!pythia.next()) continue;

    
    nCharged = nPions = nKaons = nLambdas = nXis = nOmegas = 0;
    forwardMult = highEtaMult = 0;  // Reset new multiplicity counters

    Event& event = pythia.event;
    for (int i = 0; i < event.size(); ++i) {
      Particle& p = event[i];
      
    

      if (p.isFinal() && p.pT() > 0.1) {
        if (p.isCharged()) ++nCharged;
        int absid = abs(p.id());
        if (absid == 211) ++nPions;
        else if (absid == 310) ++nKaons;
        else if (absid == 3122) ++nLambdas;
        else if (absid == 3312) ++nXis;
        else if (absid == 3334) ++nOmegas;

        // Calculate forward multiplicity (V0M) and high |η| multiplicity
        double eta = p.eta();
        if (eta > 2.8 && eta < 5.1) ++forwardMult;  // V0M region
        if (fabs(eta) > 3.0) ++highEtaMult;         // High-|η| region
      }
    }

    // Discard events with low multiplicity
    if (nCharged < 10) continue;

    // Assign multiplicity classes based on forwardMult (V0M)
    if (forwardMult >= 50) multiplicityClass = 0;  // High Multiplicity
    else if (forwardMult >= 30) multiplicityClass = 1;
    else if (forwardMult >= 15) multiplicityClass = 2;
    else multiplicityClass = 3;  // Low Multiplicity

    if(nCharged < 10) continue;

    }

    tree->Fill();
  }

  outFile->Write();
  outFile->Close();
  pythia.stat();
  return 0;
}
