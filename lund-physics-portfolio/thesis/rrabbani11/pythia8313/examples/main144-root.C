// main144-root.C is a part of the PYTHIA event generator.
// Copyright (C) 2025 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL v2 or later, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// Authors: Christian Bierlich <christian.bierlich@fysik.lu.se>

// ROOT macro demonstrating how one can read in ROOT files generated with
// main144, using the classes from RootEvent.h, compiled into the dictionary
// shared library libRootEvent.so.
// The following use case is demonstrated:
// After generating one or more ROOT files, read in the contents, and make a
// histogram.
//
// Usage from a ROOT prompt:
//
//   .L libRootEvent.so
//   root [0] .L main144-root.C+
//   root [1] read({"pythia.root"})
//   root [2] read({"pythia1.root", "pythia2.root", ...})
//

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TSystem.h"
#include "TH1D.h"
#include "TCanvas.h"
#include <iostream>
#include <vector>
#include <string>
#include "RootEvent.h"

// Read one or more files, loop over events, draw a histogram.
void read(std::vector<std::string> filenames = {"pythia.root"}) {

  // 1) Create a TChain and add the files
  TChain* tree = new TChain("t");
  for (auto &fn : filenames) {
    tree->Add(fn.c_str());
  }
  
  // 2) Create and fill histogram
  
  // Create a histogram (particle pT)
  TH1D* hPt = new TH1D("hpT", "p_{T} (GeV)", 100, 0.0, 10.0);
  // Create another histogram (pion pseudorapidity)
  TH1D* hEtaPi = new TH1D("hEtaPi", "eta (pions)", 20, -10, 10.0);

  RootEvent* evt = nullptr;
  tree->SetBranchAddress("events", &evt);

  // Loop over entries
  Long64_t nEntries = tree->GetEntries();
  for (Long64_t i = 0; i < nEntries; i++) {
    tree->GetEntry(i);
    
    double w = evt->weight;
    // Fill histogram for each track
    for (auto &trk : evt->tracks) {
      // Fill simple histogram
      hPt->Fill(trk.pT, w);
      // Fill conditional histogram
      if (abs(trk.pid) == 211)
        hEtaPi->Fill(trk.eta, w); 
    }
  }
  
  // 4) Draw
  TCanvas* c2 = new TCanvas("cMulti", "Multi File", 800, 600);
  hPt->Draw();
  hEtaPi->Draw();
}
