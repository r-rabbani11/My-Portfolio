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
// Usage from a ROOT prompt:
//
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


// Test whether or not a track sits in the forward detectors
bool isForwardTrack(RootTrack& trk) {
  if (trk.isHadron && trk.isCharged && trk.pT > 0.1) 
    if ((trk.eta > -3.7 && trk.eta < -1.7) || 
      (trk.eta > 2.8 && trk.eta < 5.1)) return true;
  return false;
}

// Find centrality percentiles of the centrality histogram
std::vector<double> findPercentiles(TH1D* hCalib) {
  double totalIntegral = hCalib->Integral("width");
  vector<double> percentiles = {1.,5.,10.,15.,20., 30., 40., 50., 70., 100.};
  vector<double> xPercentiles;

  double cumulativeIntegral = 0;
  int nbins = hCalib->GetNbinsX();
  
  for (double p : percentiles) {
    double threshold = p/100. * totalIntegral;
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

// Read one or more files, loop over events, make calibration hist. Save to
// a new root file.
void calibration(vector<string> filenames = {"../pythia.root"}) {

  // Create a TChain and add the files
  TChain tree("t");
  for (auto &fn : filenames) {
    tree.Add(fn.c_str());
  }
    
  // Create a calibration histogram 
  TH1D hCalib("hCalib", "nFwd", 100, 0, 200);
  
  RootEvent* evt = nullptr;
  tree.SetBranchAddress("events", &evt);

  // Loop over entries
  Long64_t nEntries = tree.GetEntries();
  for (Long64_t i = 0; i < nEntries; i++) {
    tree.GetEntry(i);
    
    int nFwd = 0;
    // Fill histogram for each track
    for (auto &trk : evt->tracks)
      if (isForwardTrack(trk)) ++nFwd;
    hCalib.Fill(nFwd);
  }
  // Normalize the histogram
  hCalib.Scale(1.0/hCalib.Integral("width"));
  // Save histogram to a new ROOT file
  TFile outFile("calibration.root", "RECREATE");
  hCalib.Write();
  outFile.Close();
  delete evt;
  
}

void drawCalibration(TH1D* hCalib) {
  auto percentiles = findPercentiles(hCalib);
  TCanvas* c1 = new TCanvas();
  hCalib->Draw();
  c1->SetLogy();
  for (double p : percentiles) {
    TLine* l = new TLine(p, 0, p, hCalib->GetBinContent(hCalib->FindBin(p)));
    l->Draw();
  }
}

void drawRatios() {
  // Open the results file
  TFile yieldsFile("yields.root", "READ");
  // Load the histograms
  TProfile* pi = yieldsFile.Get<TProfile>("hPiCent");
  TProfile* p = yieldsFile.Get<TProfile>("hPCent");
  TProfile* k = yieldsFile.Get<TProfile>("hKCent");
  TProfile* L = yieldsFile.Get<TProfile>("hLCent");
  TProfile* X = yieldsFile.Get<TProfile>("hXCent");
  TProfile* O = yieldsFile.Get<TProfile>("hOmegaCent");
  k->Divide(pi);
}

void analyze(vector<string> filenames = {"../pythia.root"}) {

  // Open the calibration file
  TFile calibFile("calibration.root", "READ");
  // Load the calibration histogram
  TH1D* hCalib = calibFile.Get<TH1D>("hCalib");
  auto percentiles = findPercentiles(hCalib);
  delete hCalib;
  // Create profiles binned in centrality class index.
  TProfile hPiCent("hPiCent", "nPi", 10, 0, 10);
  TProfile hPCent("hPCent", "nP", 10, 0, 10);
  TProfile hKCent("hKCent", "nK", 10, 0, 10);
  TProfile hLCent("hLCent", "nL", 10, 0, 10);
  TProfile hXiCent("hXCent", "nXi", 10, 0, 10);
  TProfile hOmegaCent("hOmegaCent", "nOmega", 10, 0, 10);

  // Create a TChain and add the files
  TChain tree("t");
  for (auto &fn : filenames) {
    tree.Add(fn.c_str());
  }
  
  RootEvent* evt = nullptr;
  tree.SetBranchAddress("events", &evt);

  // Loop over entries
  Long64_t nEntries = tree.GetEntries();
  for (Long64_t i = 0; i < nEntries; i++) {
    tree.GetEntry(i);
    
    // Forward multiplicity
    int nFwd = 0;
    // Number of pions
    int nPi = 0;
    // Number of protons
    int nP = 0;
    // Number of Kaons
    int nK = 0;
    // Number of Lambda
    int nL = 0;
    // Number of Xi
    int nXi = 0;
    // Number of Omega
    int nO = 0;

    // Count tracks
    for (auto &trk : evt->tracks) {
      if (isForwardTrack(trk)) ++nFwd;
      else if (abs(trk.y) < 0.5) {
        int id = abs(trk.pid);
        if (id == 211) ++nPi;
        else if(id == 2212) ++nP;
        else if (id == 310) ++nK;
        else if (id == 3122) ++nL;
        else if (id == 3312) ++nXi;
        else if (id == 3334) ++nO;
      }  
    }
    // Find the calibration index
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