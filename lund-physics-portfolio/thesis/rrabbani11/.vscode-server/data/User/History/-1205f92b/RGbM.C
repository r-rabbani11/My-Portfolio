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
#include "RootEvent.h"
#include <ROOT/TThreadedObject.hxx>

bool isForwardTrack(RootTrack& trk) {
  if (!trk.isHadron || !trk.isCharged || trk.pT <= 0.1) return false;
  double eta = trk.eta;
  return (eta > -3.7 && eta < -1.7) || (eta > 2.8 && eta < 5.1);
}

std::vector<double> findPercentiles(TH1D* hCalib) {
  double totalIntegral = hCalib->Integral("width");
  vector<double> percentiles = {1., 5., 10., 15., 20., 30., 40., 50., 70., 100.};
  vector<double> xPercentiles;
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

void analyze(vector<string> filenames = {"../pythia.root"}) {
  ROOT::EnableImplicitMT(); // Enable multi-threading

  TFile calibFile("calibration.root", "READ");
  if (!calibFile.IsOpen()) {
    std::cerr << "Error: Could not open calibration file!" << std::endl;
    return;
  }
  TH1D* hCalib = calibFile.Get<TH1D>("hCalib");
  if (!hCalib) {
    std::cerr << "Error: Could not load calibration histogram!" << std::endl;
    return;
  }
  auto percentiles = findPercentiles(hCalib);

  TChain tree("t");
  for (auto &fn : filenames) tree.Add(fn.c_str());

  RootEvent* evt = new RootEvent();
  tree.SetBranchAddress("events", &evt);

  TProfile hPiCent("hPiCent", "nPi", 10, 0, 10);
  TProfile hPCent("hPCent", "nP", 10, 0, 10);
  TProfile hKCent("hKCent", "nK", 10, 0, 10);
  TProfile hLCent("hLCent", "nL", 10, 0, 10);
  TProfile hXiCent("hXCent", "nXi", 10, 0, 10);
  TProfile hOmegaCent("hOmegaCent", "nOmega", 10, 0, 10);

  Long64_t nEntries = tree.GetEntries();
  for (Long64_t i = 0; i < nEntries; i++) {
    tree.GetEntry(i);
    int nFwd = 0, nPi = 0, nP = 0, nK = 0, nL = 0, nXi = 0, nO = 0;

    for (auto &trk : evt->tracks) {
      if (isForwardTrack(trk)) ++nFwd;
      else if (abs(trk.y) < 0.5) {
        int id = abs(trk.pid);
        if (id == 211) ++nPi;
        else if (id == 2212) ++nP;
        else if (id == 310) ++nK;
        else if (id == 3122) ++nL;
        else if (id == 3312) ++nXi;
        else if (id == 3334) ++nO;
      }
    }

    double cIndex = 9.5;
    for (auto p : percentiles) {
      if (double(nFwd) > p) break;
      cIndex -= 1.0;
    }
    hPiCent.Fill(cIndex, nPi);
    hPCent.Fill(cIndex, nP);
    hKCent.Fill(cIndex, nK);
    hLCent.Fill(cIndex, nL);
    hXiCent.Fill(cIndex, nXi);
    hOmegaCent.Fill(cIndex, nO);
  }

  TFile outFile("yields.root", "RECREATE");
  hPiCent.Write();
  hPCent.Write();
  hKCent.Write();
  hLCent.Write();
  hXiCent.Write();
  hOmegaCent.Write();
  outFile.Close();

  delete evt;
}