
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

//   .L ../libRootEvent.so
//   root [0] .L analysis.C+
//   root [1] read({"pythia.root"})
//   root [2] read({"pythia1.root", "pythia2.root", ...})
//
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TSystem.h"
#include "TH1D.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TLine.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm> // For std::find
#include "RootEvent.h"

// Function to check if a track is a charged hadron
/*bool isChargedHadron(int pid) {
  // List of charged hadron PDG codes
  const std::vector<int> chargedHadrons = {
    211,   // π+
    -211,  // π-
    321,   // K+
    -321,  // K-
    2212,  // p
    -2212, // anti-p
    3222,  // Σ+
    -3222, // anti-Σ+
    3112,  // Σ-
    -3112, // anti-Σ-
    3312,  // Ξ-
    -3312, // anti-Ξ-
    3334,  // Ω-
    -3334  // anti-Ω-
  };
  // Check if the particle ID is in the list of charged hadrons
  return std::find(chargedHadrons.begin(), chargedHadrons.end(), pid) != chargedHadrons.end();
}
*/
// Function to check if a track is in the forward detectors
bool isForwardTrack(RootTrack& trk) {
  // Check if the track is a charged hadron and has sufficient pT
  if (!isChargedHadron(trk.pid)) return false; // Fixed missing parenthesis
  if (trk.pT <= 0.1) return false;

  // Check if the track is within the forward detector acceptance
  double eta = trk.eta;
  return (eta > -3.7 && eta < -1.7) || (eta > 2.8 && eta < 5.1);
}

// Function to find centrality percentiles
std::vector<double> findPercentiles(TH1D* hCalib) {
  double totalIntegral = hCalib->Integral("width");
  std::vector<double> percentiles = {1., 5., 10., 15., 20., 30., 40., 50., 70., 100.};
  std::vector<double> xPercentiles;
  int nbins = hCalib->GetNbinsX();

  for (double p : percentiles) {
    double threshold = p / 100. * totalIntegral;
    int low = 1, high = nbins;
    while (low <= high) {
      int mid = (low + high) / 2;
      double integral = hCalib->Integral(mid, nbins, "width");
      if (integral >= threshold) {
        low = mid + 1;
      } else {
        high = mid - 1;
      }
    }
    xPercentiles.push_back(hCalib->GetBinCenter(high));
  }
  return xPercentiles;
}

// Function to perform the analysis
void analyze(std::vector<std::string> filenames = {"../pythia.root"}) {
  // Enable multi-threading (if supported by your ROOT version)
  // ROOT::EnableImplicitMT(); // Uncomment if using ROOT >= 6.16

  // Open the calibration file
  TFile calibFile("calibration.root", "READ");
  if (!calibFile.IsOpen()) {
    std::cerr << "Error: Could not open calibration file!" << std::endl;
    return;
  }

  // Load the calibration histogram
  TH1D* hCalib = calibFile.Get<TH1D>("hCalib");
  if (!hCalib) {
    std::cerr << "Error: Could not load calibration histogram!" << std::endl;
    return;
  }

  // Find centrality percentiles
  auto percentiles = findPercentiles(hCalib);

  // Create a TChain and add the input files
  TChain tree("t");
  for (auto &fn : filenames) {
    tree.Add(fn.c_str());
  }

  // Create an event object and set the branch address
  RootEvent* evt = new RootEvent();
  tree.SetBranchAddress("events", &evt);

  // Create profiles for particle yields binned in centrality
  TProfile hPiCent("hPiCent", "nPi", 10, 0, 10);
  TProfile hPCent("hPCent", "nP", 10, 0, 10);
  TProfile hKCent("hKCent", "nK", 10, 0, 10);
  TProfile hLCent("hLCent", "nL", 10, 0, 10);
  TProfile hXCent("hXCent", "nXi", 10, 0, 10);
  TProfile hOmegaCent("hOmegaCent", "nOmega", 10, 0, 10);

  // Loop over events
  Long64_t nEntries = tree.GetEntries();
  for (Long64_t i = 0; i < nEntries; i++) {
    tree.GetEntry(i);

    // Count forward tracks and identified particles
    int nFwd = 0, nPi = 0, nP = 0, nK = 0, nL = 0, nXi = 0, nO = 0;
    for (auto &trk : evt->tracks) {
      if (isForwardTrack(trk)) {
        ++nFwd;
      } else if (abs(trk.y) < 0.5) {
        int id = abs(trk.pid);
        if (id == 211) ++nPi;
        else if (id == 2212) ++nP;
        else if (id == 321) ++nK;
        else if (id == 3122) ++nL;
        else if (id == 3312) ++nXi;
        else if (id == 3334) ++nO;
      }
    }

    // Determine centrality index
    double cIndex = 9.5;
    for (auto p : percentiles) {
      if (double(nFwd) > p) break;
      cIndex -= 1.0;
    }

    // Fill profiles
    hPiCent.Fill(cIndex, nPi);
    hPCent.Fill(cIndex, nP);
    hKCent.Fill(cIndex, nK);
    hLCent.Fill(cIndex, nL);
    hXCent.Fill(cIndex, nXi);
    hOmegaCent.Fill(cIndex, nO);
  }
  
  

  // Save results to a new ROOT file
  TFile outFile("yields.root", "RECREATE");
  hPiCent.Write();
  hPCent.Write();
  hKCent.Write();
  hLCent.Write();
  hXCent.Write();
  hOmegaCent.Write();
  outFile.Close();

  // Clean up
  delete evt;
}

