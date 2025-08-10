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
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"

int main() {

  TFile *outFile = new TFile("main442exp.root", "RECREATE");
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

  TH1F *hPion = new TH1F("hPion", "pions (mult)", 50, 10., 135.);
  TH1F *hKaon = new TH1F("hKaon", "kaons (mult)", 50, 10., 135.);
  TH1F *hLambda = new TH1F("hLambda", "lambdas (mult)", 50, 10., 135.);
  TH1F *hXi = new TH1F("hXi", "xi (mult)", 50, 10., 135.);
  TH1F *hOmega = new TH1F("hOmega", "omega (mult)", 50, 10., 135.);

  TH1F *hKp = new TH1F("hKp", "kaon / pion (multiplicity)", 50, 10., 135.);
  TH1F *hLp = new TH1F("hLp", "lambda / pion (multiplicity)", 50, 10., 135.);
  TH1F *hXp = new TH1F("hXp", "xi / pion (multiplicity)", 50, 10., 135.);
  TH1F *hOp = new TH1F("hOp", "omega / pion (multiplicity)", 50, 10., 135.);

  Pythia pythia;
  pythia.readString("Beams:eCM = 7000.");
  pythia.readString("SoftQCD:nonDiffractive = on");
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
    forwardMult = highEtaMult = 0;
    multiplicityClass = -1;
    
    Event& event = pythia.event;
    for (int i = 0; i < event.size(); ++i){
      Particle& p = event[i];
      if (p.isFinal() && p.isCharged()) {
        double eta = p.eta();
        if (eta > 2.8 && eta < 5.1) ++forwardMult;
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

    if (forwardMult >= 50) multiplicityClass = 0;
    else if (forwardMult >= 30) multiplicityClass = 1;
    else if (forwardMult >= 15) multiplicityClass = 2;
    else multiplicityClass = 3;

    if (nCharged < 10) continue;

    hPion->Fill(nCharged, nPions);
    hKaon->Fill(nCharged, nKaons);
    hLambda->Fill(nCharged, nLambdas);
    hXi->Fill(nCharged, nXis);
    hOmega->Fill(nCharged, nOmegas);

    if (nPions > 0) {
      hKp->Fill(nCharged, double(nKaons) / double(nPions));
      hLp->Fill(nCharged, 2.0 * double(nLambdas) / double(nPions));
      hXp->Fill(nCharged, 6.0 * double(nXis) / double(nPions));
      hOp->Fill(nCharged, 16.0 * double(nOmegas) / double(nPions));
    }
    tree->Fill();
  }

  outFile->Write();
  outFile->Close();
  pythia.stat();
  return 0;
}
