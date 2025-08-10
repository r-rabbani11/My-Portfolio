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

#include "Pythia8/Pythia.h"
using namespace Pythia8;

//==========================================================================

int main() {

  TFile *outFile = new TFile("main442exp.root", "RECREATE");
  TTree *tree = new TTree("tree", "tree");

  int nCharged, nPions, nKaons, nLambdas, nXis, nOmegas;
  tree->Branch("nCharged", &nCharged, "nCharged/I");
  tree->Branch("nPions", &nPions, "nPions/I");
  tree->Branch("nKaons", &nKaons, "nKaons/I");
  tree->Branch("nLambdas", &nLambdas, "nLambdas/I");
  tree->Branch("nXis", &nXis, "nXis/I");
  tree->Branch("nOmegas", &nOmegas, "nOmegas/I");

  TH1F *hPion = new TH1F("hPion", "pions (mult)", 50, 10., 135.);
  TH1F *hKaon = new TH1F("hKaon", "kaons (mult)", 50, 10., 135.);
  TH1F *hLambda = new TH1F("hLambda", "lambdas (mult)", 50, 10., 135.);
  TH1F *hXi = new TH1F("hXi", "xi (mult)", 50, 10., 135.);
  TH1F *hOmega = new TH1F("hOmega", "omega (mult)", 50, 10., 135.);


  TH1F *hKp = new TH1F("hKp", "kaon / pion (multiplicity)", 50, 10., 135.);
  TH1F *hLp = new TH1F("hLp", "lambda / pion (multiplicity)", 50, 10., 135.);
  TH1F *hXp = new TH1F("hXp", "xi / pion (multiplicity)", 50, 10., 135.);
  TH1F *hOp = new TH1F("hOp", "omega / pion (multiplicity)", 50, 10., 135.);
  

  // Generator. Process selection. LHC initialization.
  Pythia pythia;
  pythia.readString("Beams:eCM = 7000.");
  pythia.readString("SoftQCD:nonDiffractive = on");

  // Enabling flavour ropes, setting model parameters.
  // The model is still untuned. These parameter values
  // are chosen for illustrative purposes.
  pythia.readString("Ropewalk:RopeHadronization = on");
  pythia.readString("Ropewalk:doShoving = off");
  pythia.readString("Ropewalk:doFlavour = on");
  pythia.readString("Ropewalk:r0 = 0.5");
  pythia.readString("Ropewalk:m0 = 0.2");
  pythia.readString("Ropewalk:beta = 0.1");

  // Enabling setting of vertex information.
  pythia.readString("PartonVertex:setVertex = on");

  // Prevent unstable particles from decaying.
  pythia.readString("ParticleDecays:limitTau0 = on");
  pythia.readString("ParticleDecays:tau0Max = 10");

  // If Pythia fails to initialize, exit with error.
  if (!pythia.init()) return 1;


  // Note: High statistics is needed to fill the high multiplicity end
  // of the histograms, especially for Omega.
  const int nEvent = 4000;

  // Begin event loop. Generate event. Skip if error. List first one.
  for (int iEvent = 0; iEvent < nEvent; ++iEvent) {
    if (!pythia.next()) continue;

    // Counters for particle species.
    int nCharged = 0, nPions = 0, nKaons = 0;
    int nLambdas = 0, nXis = 0, nOmegas = 0;
    // Event short notation.
    Event& event = pythia.event;
    for (int i = 0; i < event.size(); ++i){
        Particle& p = event[i];
        // Apply simple, particle level, cuts.
        if (p.isFinal() && abs(p.eta()) < 2.5 && p.pT() > 0.1 ) {
          if(p.isCharged()) ++nCharged;
          int absid = abs(p.id());
          if(absid == 211) ++nPions;
          else if(absid == 310) ++nKaons;
          else if(absid == 3122) ++nLambdas;
          else if(absid == 3312) ++nXis;
          else if(absid == 3334) ++nOmegas;
        }
     }

     // Discard events with event multiplicity less than 10.
     if(nCharged < 10) continue;

     hPion->Fill(nCharged, nPions);
     hKaon->Fill(nCharged, nKaons);
     hLambda->Fill(nCharged, nLambdas);
     hXi->Fill(nCharged, nXis);
     hOmega->Fill(nCharged, nOmegas);

     if (nPions > 0){
       hKp->Fill(nCharged, nKaons/nPions);
       hLp->Fill(nCharged, nLambdas/nPions);
       hXp->Fill(nCharged, nXis/nPions);
       hOp->Fill(nCharged, nOmegas/nPions);
     }  

     tree->Fill();    



  // End of event loop.
  }

  outFile->Write();
  outFile->Close();
  // Statistics. Histograms. Done.
  pythia.stat();
  return 0;
}
