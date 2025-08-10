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
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TProfile.h"
#include <fstream>
#include <vector>
using namespace Pythia8;

// Function to check if a track is in the forward detectors
bool isForwardTrack(const Particle& p) {
  if (p.isHadron() && p.isCharged() && p.pT() > 0.1) {
    double eta = p.eta();
    if ((eta > -3.7 && eta < -1.7) || (eta > 2.8 && eta < 5.1)) return true;
  }
  return false;
}

// Function to find centrality percentiles from a histogram
std::vector<double> findPercentiles(TH1D* hCalib) {
  double totalIntegral = hCalib->Integral("width");
  std::vector<double> percentiles = {1., 5., 10., 15., 20., 30., 40., 50., 70., 100.};
  std::vector<double> xPercentiles;

  double cumulativeIntegral = 0;
  int nbins = hCalib->GetNbinsX();

  for (double p : percentiles) {
    double threshold = p / 100. * totalIntegral;
    for (int bin = nbins; bin > 0; --bin) {
      cumulativeIntegral = hCalib->Integral(bin, nbins, "width");
      if (cumulativeIntegral >= threshold) {
        double xValue = hCalib->GetBinCenter(bin);
        xPercentiles.push_back(xValue);
        break;
      }
    }
  }
  return xPercentiles;
}

int main() {
  // ROOT File and Tree
  TFile* outFile = new TFile("main442exp4.root", "RECREATE");
  TTree* tree = new TTree("tree", "tree");

  int nCharged, nPions, nKaons, nLambdas, nXis, nOmegas, nForward;
  tree->Branch("nCharged", &nCharged, "nCharged/I");
  tree->Branch("nPions", &nPions, "nPions/I");
  tree->Branch("nKaons", &nKaons, "nKaons/I");
  tree->Branch("nLambdas", &nLambdas, "nLambdas/I");
  tree->Branch("nXis", &nXis, "nXis/I");
  tree->Branch("nOmegas", &nOmegas, "nOmegas/I");
  tree->Branch("nForward", &nForward, "nForward/I");

  // Histograms
  TH1D* hForwardMult = new TH1D("hForwardMult", "Forward Multiplicity; N_{ch} (2.5 < |#eta| < 5); Events", 100, 0, 200);

  // Profiles for particle yields vs. centrality
  TProfile* pPion = new TProfile("pPion", "Pions; Centrality Class; N_{#pi}", 10, 0, 10);
  TProfile* pKaon = new TProfile("pKaon", "Kaons; Centrality Class; N_{K}", 10, 0, 10);
  TProfile* pLambda = new TProfile("pLambda", "Lambdas; Centrality Class; N_{#Lambda}", 10, 0, 10);
  TProfile* pXi = new TProfile("pXi", "Xis; Centrality Class; N_{#Xi}", 10, 0, 10);
  TProfile* pOmega = new TProfile("pOmega", "Omegas; Centrality Class; N_{#Omega}", 10, 0, 10);

  // Profiles for particle ratios vs. centrality
  TProfile* pKp = new TProfile("pKp", "Kaon / Pion; Centrality Class; K / #pi", 10, 0, 10);
  TProfile* pLp = new TProfile("pLp", "Lambda / Pion; Centrality Class; #Lambda / #pi", 10, 0, 10);
  TProfile* pXp = new TProfile("pXp", "Xi / Pion; Centrality Class; #Xi / #pi", 10, 0, 10);
  TProfile* pOp = new TProfile("pOp", "Omega / Pion; Centrality Class; #Omega / #pi", 10, 0, 10);

  // Generator initialization
  Pythia pythia;
  pythia.readString("Beams:idA = 2212");
  pythia.readString("Beams:idB = 2212");
  pythia.readString("Beams:eCM = 7000.");
  pythia.readString("SoftQCD:nonDiffractive = on");

  // Set relevant particles to be stable (ALICE convention)
  pythia.readString("310:onMode = off");
  pythia.readString("311:onMode = off");
  pythia.readString("3122:onMode = off");
  pythia.readString("3312:onMode = off");
  pythia.readString("3334:onMode = off");
  pythia.readString("3222:onMode = off");
  pythia.readString("3322:onMode = off");
  pythia.readString("3112:onMode = off");

  // Parameter of the MPI model to keep total multiplicity reasonable
  pythia.readString("MultiPartonInteractions:pT0Ref = 2.15");

  // Parameters related to Junction formation/QCD-based Color Reconnection (CR)
  pythia.readString("BeamRemnants:remnantMode = 1");
  pythia.readString("BeamRemnants:saturation = 5");
  pythia.readString("ColourReconnection:mode = 1");
  pythia.readString("ColourReconnection:allowDoubleJunRem = off");
  pythia.readString("ColourReconnection:m0 = 0.3");
  pythia.readString("ColourReconnection:allowJunctions = on");
  pythia.readString("ColourReconnection:junctionCorrection = 1.2");
  pythia.readString("ColourReconnection:timeDilationMode = 2");
  pythia.readString("ColourReconnection:timeDilationPar = 0.18");

  // Enable rope hadronization
  pythia.readString("Ropewalk:RopeHadronization = on");

  // Enable string shoving (without affecting dynamics)
  pythia.readString("Ropewalk:doShoving = on");
  pythia.readString("Ropewalk:tInit = 1.5");
  pythia.readString("Ropewalk:deltat = 0.05");
  pythia.readString("Ropewalk:tShove = 0.1");
  pythia.readString("Ropewalk:gAmplitude = 0.");

  // Enable rope flavor effects
  pythia.readString("Ropewalk:doFlavour = on");

  // Parameters of the rope model
  pythia.readString("Ropewalk:r0 = 0.5");
  pythia.readString("Ropewalk:m0 = 0.2");
  pythia.readString("Ropewalk:beta = 0.1");

  // Enable setting of vertex information
  pythia.readString("PartonVertex:setVertex = on");
  pythia.readString("PartonVertex:protonRadius = 0.7");
  pythia.readString("PartonVertex:emissionWidth = 0.1");


  if (!pythia.init()) return 1;

  // Event loop
  const int nEvent = 10000;

  // Store forward multiplicity and particle yields for each event
  std::vector<int> forwardMultiplicities;
  std::vector<int> pionYields;
  std::vector<int> kaonYields;
  std::vector<int> lambdaYields;
  std::vector<int> xiYields;
  std::vector<int> omegaYields;

  for (int iEvent = 0; iEvent < nEvent; ++iEvent) {
    if (!pythia.next()) continue;

    nCharged = nPions = nKaons = nLambdas = nXis = nOmegas = nForward = 0;
    Event& event = pythia.event;

    // Loop over particles in the event
    for (int i = 0; i < event.size(); ++i) {
      Particle& p = event[i];
      if (p.isFinal() && p.pT() > 0.1) {
        if (isForwardTrack(p)) ++nForward;  // Forward multiplicity
        if (abs(p.eta()) < 2.5) {          // Midrapidity particles
          if (p.isCharged()) ++nCharged;
          int absid = abs(p.id());
          if (absid == 211) ++nPions;
          else if (absid == 310) ++nKaons;
          else if (absid == 3122) ++nLambdas;
          else if (absid == 3312) ++nXis;
          else if (absid == 3334) ++nOmegas;
        }
      }
    }

    //if (nCharged < 10) continue;

    // Store event information
    forwardMultiplicities.push_back(nForward);
    pionYields.push_back(nPions);
    kaonYields.push_back(nKaons);
    lambdaYields.push_back(nLambdas);
    xiYields.push_back(nXis);
    omegaYields.push_back(nOmegas);

    // Fill forward multiplicity histogram
    hForwardMult->Fill(nForward);

    // Fill tree
    tree->Fill();
  }

  // Normalize forward multiplicity histogram
  hForwardMult->Scale(1.0 / hForwardMult->Integral("width"));

  // Determine centrality percentiles
  std::vector<double> percentiles = findPercentiles(hForwardMult);

  // Fill centrality-based profiles
  for (size_t i = 0; i < forwardMultiplicities.size(); ++i) {
    nForward = forwardMultiplicities[i];
    double cIndex = 9.5;
    for (double p : percentiles) {
      if (double(nForward) > p) break;
      cIndex -= 1.0;
    }

    // Fill centrality-based profiles
    pPion->Fill(cIndex, pionYields[i]);
    pKaon->Fill(cIndex, kaonYields[i]);
    pLambda->Fill(cIndex, lambdaYields[i]);
    pXi->Fill(cIndex, xiYields[i]);
    pOmega->Fill(cIndex, omegaYields[i]);

    if (pionYields[i] > 0) {
      pKp->Fill(cIndex, double(kaonYields[i]) / pionYields[i]);
      pLp->Fill(cIndex, 2.0 * double(lambdaYields[i]) / pionYields[i]);  // Scale by 2
      pXp->Fill(cIndex, 6.0 * double(xiYields[i]) / pionYields[i]);      // Scale by 6
      pOp->Fill(cIndex, 16.0 * double(omegaYields[i]) / pionYields[i]);  // Scale by 16
    }
  }

  // Write output
  outFile->Write();
  outFile->Close();

  pythia.stat();
  return 0;
}
